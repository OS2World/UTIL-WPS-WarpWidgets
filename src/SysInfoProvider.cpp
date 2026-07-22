#include "SysInfoProvider.h"
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QStorageInfo>
#include <QDebug>
#include <cstring>


SysInfoProvider::SysInfoProvider(QObject *parent)
    : QObject(parent)
{
    // Clock: update every second
    connect(&m_clockTimer, &QTimer::timeout, this, &SysInfoProvider::updateTime);
    m_clockTimer.start(1000);

    // Stats: update every 2 seconds
    connect(&m_statsTimer, &QTimer::timeout, this, &SysInfoProvider::updateStats);
    m_statsTimer.start(2000);

    // Prime values immediately
    updateTime();
    updateStats();
}

QString SysInfoProvider::currentTime() const
{
    return QDateTime::currentDateTime().toString("hh:mm:ss");
}

QString SysInfoProvider::currentDate() const
{
    return QDateTime::currentDateTime().toString("dddd, MMMM d yyyy");
}

int SysInfoProvider::utcOffsetMin() const
{
    return QDateTime::currentDateTime().offsetFromUtc() / 60;
}

void SysInfoProvider::updateTime()
{
    emit timeChanged();
}

void SysInfoProvider::updateStats()
{
    readRam();
    readCpu();
    readDisk();
    emit statsChanged();
}

// ---------------------------------------------------------------------------
// RAM
// ---------------------------------------------------------------------------
void SysInfoProvider::readRam()
{
#ifdef Q_OS_OS2
    ULONG ulPhysTotal = 0;
    ULONG ulPhysAvail = 0;
    DosQuerySysInfo(QSV_TOTPHYSMEM, QSV_TOTPHYSMEM, &ulPhysTotal, sizeof(ULONG)); /* QSV_TOTPHYSMEM = 7 */
    DosQuerySysInfo(QSV_TOTAVAILMEM, QSV_TOTAVAILMEM, &ulPhysAvail, sizeof(ULONG));
    m_ramTotalMB = (qint64)ulPhysTotal / (1024 * 1024);
    m_ramUsedMB  = ((qint64)ulPhysTotal - (qint64)ulPhysAvail) / (1024 * 1024);
    m_ramPercent = m_ramTotalMB > 0
                   ? (int)(m_ramUsedMB * 100 / m_ramTotalMB)
                   : 0;
#else
    // Linux fallback — read /proc/meminfo
    QFile f("/proc/meminfo");
    if (!f.open(QIODevice::ReadOnly)) return;
    QTextStream ts(&f);
    qint64 total = 0, free_ = 0, buffers = 0, cached = 0;
    QString line;
    while (ts.readLineInto(&line)) {
        QStringList parts = line.split(QRegExp("\\s+"));
        if (parts.size() < 2) continue;
        qint64 val = parts[1].toLongLong(); // kB
        if (parts[0] == "MemTotal:")   total   = val;
        if (parts[0] == "MemFree:")    free_   = val;
        if (parts[0] == "Buffers:")    buffers = val;
        if (parts[0] == "Cached:")     cached  = val;
    }
    qint64 used = total - free_ - buffers - cached;
    m_ramTotalMB = total / 1024;
    m_ramUsedMB  = used  / 1024;
    m_ramPercent = total > 0 ? (int)(used * 100 / total) : 0;
#endif
}

// ---------------------------------------------------------------------------
// CPU
// ---------------------------------------------------------------------------
#ifdef Q_OS_OS2
// Get DosPerfSysCall by ordinal 976 — DOSCALLS is always loaded, use QueryModuleHandle
void SysInfoProvider::initCpu()
{
    if (m_pfnDosPerfSysCall) return;
    memset(m_prevUtil, 0, sizeof(m_prevUtil));
    HMODULE hmod = NULLHANDLE;
    if (DosQueryModuleHandle((PCSZ)"DOSCALLS", &hmod) != 0) return;
    DosQueryProcAddr(hmod, 976, NULL, (PFN *)&m_pfnDosPerfSysCall);
}
#endif

void SysInfoProvider::readCpu()
{
#ifdef Q_OS_OS2
    initCpu();
    if (!m_pfnDosPerfSysCall) { m_cpuPercent = 0; return; }

    // Allocate MAX_CPUS entries — OS/2 writes one CPUUTIL per CPU
    // A single-entry buffer overflows on multi-CPU systems (e.g. 4-CPU = 128 bytes)
    CpuUtil cur[MAX_CPUS];
    memset(cur, 0, sizeof(cur));
    APIRET rc = m_pfnDosPerfSysCall(0x63 /*CMD_KI_RDCNT*/, (ULONG)cur, 0, 0);
    if (rc != 0) { m_cpuPercent = 0; return; }

    if (!m_cpuInitialized) {
        memcpy(m_prevUtil, cur, sizeof(cur));
        m_cpuInitialized = true;
        m_cpuPercent     = 0;
        return;
    }

    // Sum deltas across all CPUs for a system-wide percentage
    #define LL2F(hi, lo) (4294967296.0 * (double)(hi) + (double)(lo))
    double timeDelta = 0, busyDelta = 0, intrDelta = 0;
    for (int i = 0; i < MAX_CPUS; i++) {
        double td = LL2F(cur[i].timeHigh, cur[i].timeLow)
                  - LL2F(m_prevUtil[i].timeHigh, m_prevUtil[i].timeLow);
        if (td <= 0 && i > 0) break; // no more CPU entries
        timeDelta += td;
        busyDelta += LL2F(cur[i].busyHigh, cur[i].busyLow)
                   - LL2F(m_prevUtil[i].busyHigh, m_prevUtil[i].busyLow);
        intrDelta += LL2F(cur[i].intrHigh, cur[i].intrLow)
                   - LL2F(m_prevUtil[i].intrHigh, m_prevUtil[i].intrLow);
    }
    #undef LL2F

    if (timeDelta > 0)
        m_cpuPercent = (int)qBound(0.0, (busyDelta + intrDelta) / timeDelta * 100.0, 100.0);
    else
        m_cpuPercent = 0;

    memcpy(m_prevUtil, cur, sizeof(cur));
#else
    // Linux: /proc/stat first line  "cpu  user nice sys idle iowait irq softirq"
    QFile f("/proc/stat");
    if (!f.open(QIODevice::ReadOnly)) return;
    QString line = f.readLine();
    f.close();

    QStringList parts = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    if (parts.size() < 5) return;

    qint64 user   = parts[1].toLongLong();
    qint64 nice   = parts[2].toLongLong();
    qint64 sys    = parts[3].toLongLong();
    qint64 idle   = parts[4].toLongLong();
    qint64 iowait = parts.size() > 5 ? parts[5].toLongLong() : 0;

    qint64 total  = user + nice + sys + idle + iowait;
    qint64 dTotal = total - m_prevTotal;
    qint64 dIdle  = idle  - m_prevIdle;

    m_cpuPercent = (dTotal > 0) ? (int)((dTotal - dIdle) * 100 / dTotal) : 0;
    m_prevTotal  = total;
    m_prevIdle   = idle;
#endif
}

// ---------------------------------------------------------------------------
// Disk  (all drives)
// ---------------------------------------------------------------------------
void SysInfoProvider::readDisk()
{
    m_drives.clear();

#ifdef Q_OS_OS2
    struct FSALLOCATE {
        ULONG idFileSystem;
        ULONG cSectorUnit;
        ULONG cUnit;
        ULONG cUnitAvail;
        USHORT cbSector;
    };
    // OS/2 drive numbers: 1=A, 2=B, 3=C, ..., 26=Z. Skip A/B (floppies).
    for (int driveNum = 3; driveNum <= 26; ++driveNum) {
        FSALLOCATE fsa;
        memset(&fsa, 0, sizeof(fsa));
        if (DosQueryFSInfo((ULONG)driveNum, 1, &fsa, sizeof(fsa)) != 0)
            continue;
        qint64 total = (qint64)fsa.cUnit      * fsa.cSectorUnit * fsa.cbSector;
        if (total <= 0) continue;
        qint64 free_ = (qint64)fsa.cUnitAvail * fsa.cSectorUnit * fsa.cbSector;
        qint64 totalMB = total / (1024 * 1024);
        qint64 freeMB  = free_ / (1024 * 1024);
        qint64 usedMB  = totalMB - freeMB;
        int    pct     = (int)(usedMB * 100 / totalMB);

        QVariantMap entry;
        entry["letter"]  = QString(QChar('A' + driveNum - 1)) + ":";
        entry["totalMB"] = totalMB;
        entry["freeMB"]  = freeMB;
        entry["usedMB"]  = usedMB;
        entry["percent"] = pct;
        m_drives.append(entry);
    }
#else
    for (const QStorageInfo &si : QStorageInfo::mountedVolumes()) {
        if (!si.isReady() || si.isReadOnly()) continue;
        qint64 totalMB = si.bytesTotal() / (1024 * 1024);
        if (totalMB <= 0) continue;
        qint64 freeMB  = si.bytesFree()  / (1024 * 1024);
        qint64 usedMB  = totalMB - freeMB;
        QVariantMap entry;
        entry["letter"]  = si.rootPath();
        entry["totalMB"] = totalMB;
        entry["freeMB"]  = freeMB;
        entry["usedMB"]  = usedMB;
        entry["percent"] = (int)(usedMB * 100 / totalMB);
        m_drives.append(entry);
    }
#endif

    // Keep legacy single-drive properties in sync with the first drive (C: on OS/2)
    if (!m_drives.isEmpty()) {
        QVariantMap first = m_drives.first().toMap();
        m_diskTotalMB = first["totalMB"].toLongLong();
        m_diskFreeMB  = first["freeMB"].toLongLong();
        m_diskUsedMB  = first["usedMB"].toLongLong();
        m_diskPercent = first["percent"].toInt();
    }
}
