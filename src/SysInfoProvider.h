#ifndef SYSINFOPROVIDER_H
#define SYSINFOPROVIDER_H

#include <QObject>
#include <QTimer>
#include <QString>
#include <QVariantList>

#ifdef Q_OS_OS2
#  define INCL_DOS
#  define INCL_DOSERRORS
#  include <os2.h>
#endif

class SysInfoProvider : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString currentTime   READ currentTime   NOTIFY timeChanged)
    Q_PROPERTY(QString currentDate   READ currentDate   NOTIFY timeChanged)
    Q_PROPERTY(int     utcOffsetMin  READ utcOffsetMin  NOTIFY timeChanged)
    Q_PROPERTY(int     cpuPercent  READ cpuPercent  NOTIFY statsChanged)
    Q_PROPERTY(int     ramPercent  READ ramPercent  NOTIFY statsChanged)
    Q_PROPERTY(qint64  ramUsedMB   READ ramUsedMB   NOTIFY statsChanged)
    Q_PROPERTY(qint64  ramTotalMB  READ ramTotalMB  NOTIFY statsChanged)
    Q_PROPERTY(int          diskPercent READ diskPercent NOTIFY statsChanged)
    Q_PROPERTY(qint64       diskFreeMB  READ diskFreeMB  NOTIFY statsChanged)
    Q_PROPERTY(qint64       diskUsedMB  READ diskUsedMB  NOTIFY statsChanged)
    Q_PROPERTY(qint64       diskTotalMB READ diskTotalMB NOTIFY statsChanged)
    Q_PROPERTY(QVariantList drives      READ drives      NOTIFY statsChanged)

public:
    explicit SysInfoProvider(QObject *parent = nullptr);

    QString currentTime()  const;
    QString currentDate()  const;
    int     utcOffsetMin() const;
    int     cpuPercent()  const { return m_cpuPercent; }
    int     ramPercent()  const { return m_ramPercent; }
    qint64  ramUsedMB()   const { return m_ramUsedMB; }
    qint64  ramTotalMB()  const { return m_ramTotalMB; }
    int          diskPercent() const { return m_diskPercent; }
    qint64       diskFreeMB()  const { return m_diskFreeMB; }
    qint64       diskUsedMB()  const { return m_diskUsedMB; }
    qint64       diskTotalMB() const { return m_diskTotalMB; }
    QVariantList drives()      const { return m_drives; }

signals:
    void timeChanged();
    void statsChanged();

private slots:
    void updateTime();
    void updateStats();

private:
    QTimer m_clockTimer;
    QTimer m_statsTimer;

    int    m_cpuPercent  = 0;
    int    m_ramPercent  = 0;
    qint64 m_ramUsedMB   = 0;
    qint64 m_ramTotalMB  = 0;
    int          m_diskPercent = 0;
    qint64       m_diskFreeMB  = 0;
    qint64       m_diskUsedMB  = 0;
    qint64       m_diskTotalMB = 0;
    QVariantList m_drives;

    void readRam();
    void readCpu();
    void readDisk();

#ifdef Q_OS_OS2
    typedef APIRET (APIENTRY *PFN_DOSPERFSYSCALL)(ULONG, ULONG, ULONG, ULONG);
    PFN_DOSPERFSYSCALL m_pfnDosPerfSysCall = nullptr;

    // Must hold one entry per CPU — 4-CPU system writes 4 × 32 bytes
    static const int MAX_CPUS = 32;
    struct CpuUtil {
        ULONG timeLow, timeHigh;
        ULONG idleLow, idleHigh;
        ULONG busyLow, busyHigh;
        ULONG intrLow, intrHigh;
    };
    CpuUtil m_prevUtil[MAX_CPUS];
    bool    m_cpuInitialized = false;

    void initCpu();
#else
    // Linux fallback delta
    qint64 m_prevIdle  = 0;
    qint64 m_prevTotal = 0;
#endif
};

#endif // SYSINFOPROVIDER_H
