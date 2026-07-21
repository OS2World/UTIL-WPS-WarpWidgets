#define INCL_WINWINDOWMGR
#define INCL_WINSWITCHLIST
#define INCL_DOS
#define INCL_DOSERRORS
#include <cstdlib>
#include "WidgetEngine.h"
#include "SysInfoProvider.h"
#include "WeatherProvider.h"
#include <QDir>
#include <QFileInfo>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickView>
#include <QQuickItem>
#include <QApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QAction>
#include <QColorDialog>
#include <QColor>
#include <QPoint>
#include <QCursor>
#include <QTimer>
#include <QSettings>
#include <QDebug>

static const QString DEFAULT_BG = "#1A1A1A";
static const QString APP_NAME   = "WarpWidgets";

// ---------------------------------------------------------------------------
// WidgetController — exposed to QML as "Widget"
// ---------------------------------------------------------------------------
class WidgetController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double  fontScale READ fontScale NOTIFY fontScaleChanged)
    Q_PROPERTY(QString bgColor   READ bgColor   NOTIFY bgColorChanged)

public:
    explicit WidgetController(QQuickView *view, WidgetEngine *engine,
                              const QString &settingsKey)
        : QObject(view), m_view(view), m_engine(engine), m_key(settingsKey)
        , m_settings(QSettings::IniFormat, QSettings::UserScope,
                     "OS2World", APP_NAME)
    {
        m_settings.beginGroup("widgets");
        m_fontScale = m_settings.value(m_key + "/fontScale", 1.0).toDouble();
        m_bgColor   = m_settings.value(m_key + "/bgColor", DEFAULT_BG).toString();
        m_settings.endGroup();
    }

    double  fontScale()   const { return m_fontScale; }
    QString bgColor()     const { return m_bgColor; }
    QString settingsKey() const { return m_key; }

    void setFontScale(double s)
    {
        s = qBound(0.5, s, 2.5);
        if (qAbs(s - m_fontScale) < 0.01) return;
        m_fontScale = s;
        save("fontScale", m_fontScale);
        emit fontScaleChanged();
    }

    void setBgColor(const QString &color)
    {
        if (color == m_bgColor) return;
        m_bgColor = color;
        save("bgColor", m_bgColor);
        m_view->setColor(QColor(m_bgColor));
        emit bgColorChanged();
    }

public slots:
    void increaseFontSize() { setFontScale(m_fontScale + 0.15); }
    void decreaseFontSize() { setFontScale(m_fontScale - 0.15); }
    void defaultFontSize()  { setFontScale(1.0); }
    void closeWidget()      { m_engine->saveState(); m_view->close(); }
    void quitApp()          { m_engine->saveState(); _exit(0); }

    Q_INVOKABLE void requestContextMenu()
    {
        emit contextMenuRequested(QCursor::pos());
    }

    Q_INVOKABLE QString getExtra(const QString &key, const QString &def = QString()) const
    {
        QSettings s(QSettings::IniFormat, QSettings::UserScope, "OS2World", APP_NAME);
        s.beginGroup("widgets");
        QString v = s.value(m_key + "/extra/" + key, def).toString();
        s.endGroup();
        return v;
    }

    Q_INVOKABLE void setExtra(const QString &key, const QString &value)
    {
        m_settings.beginGroup("widgets");
        m_settings.setValue(m_key + "/extra/" + key, value);
        m_settings.endGroup();
        m_settings.sync();
    }

signals:
    void fontScaleChanged();
    void bgColorChanged();
    void contextMenuRequested(QPoint globalPos);

private:
    void save(const QString &prop, const QVariant &val)
    {
        m_settings.beginGroup("widgets");
        m_settings.setValue(m_key + "/" + prop, val);
        m_settings.endGroup();
        m_settings.sync();
    }

    QQuickView   *m_view;
    WidgetEngine *m_engine;
    QString       m_key;
    QSettings     m_settings;
    double        m_fontScale = 1.0;
    QString       m_bgColor;
};

// ---------------------------------------------------------------------------
// DragFilter — RIGHT button drags, keyboard shortcuts, ˅ button → context menu
// ---------------------------------------------------------------------------
class DragFilter : public QObject
{
    Q_OBJECT
public:
    explicit DragFilter(QQuickView *view, WidgetEngine *engine,
                        WidgetController *ctrl, bool isGlobal)
        : QObject(view), m_view(view), m_engine(engine)
        , m_ctrl(ctrl), m_isGlobal(isGlobal), m_dragging(false)
    {
        connect(ctrl, &WidgetController::contextMenuRequested,
                this, &DragFilter::showContextMenu);
    }

public slots:
    void showContextMenu(const QPoint &globalPos)
    {
        // Heap-allocated + popup() avoids nested event loop issues on OS/2 PM
        QMenu *menu = new QMenu();
        menu->setAttribute(Qt::WA_DeleteOnClose);

        QAction *closeAct = menu->addAction("Close this widget");

        QMenu *fontMenu = menu->addMenu("Font");
        QAction *fInc = fontMenu->addAction("Increase  (Ctrl++)");
        QAction *fDef = fontMenu->addAction("Default   (Ctrl+0)");
        QAction *fDec = fontMenu->addAction("Decrease  (Ctrl-)");

        QString colorLabel = m_isGlobal
            ? "Background Color  (applies to ALL widgets)"
            : "Background Color";
        QMenu *colorMenu = menu->addMenu(colorLabel);

        struct Preset { const char *name; const char *hex; };
        static const Preset presets[] = {
            { "Dark Gray (default)", "#1A1A1A" },
            { "Black",               "#000000" },
            { "Dark Blue",           "#0A1628" },
            { "Dark Green",          "#0A2010" },
            { "Dark Purple",         "#1A0A28" },
            { "Dark Red",            "#200808" },
            { "Dark Teal",           "#082020" },
        };
        QList<QAction *> colorActs;
        for (const Preset &p : presets) {
            QAction *act = colorMenu->addAction(p.name);
            act->setData(QString(p.hex));
            colorActs << act;
        }
        colorMenu->addSeparator();
        QAction *customAct = colorMenu->addAction("Custom...");

        QAction *newPostitAct = nullptr;
        QString postitPath = m_engine->postitPath();
        if (!postitPath.isEmpty())
            newPostitAct = menu->addAction("New Post-It");

        QMenu *addMenu = menu->addMenu("Add Widget");
        QStringList closed = m_engine->closedWidgets();
        if (closed.isEmpty()) {
            QAction *none = addMenu->addAction("(all widgets are open)");
            none->setEnabled(false);
        } else {
            for (const QString &path : closed) {
                QString name = QFileInfo(path).baseName();
                if (!name.isEmpty()) name[0] = name[0].toUpper();
                QAction *act = addMenu->addAction(name);
                act->setData(path);
            }
        }

        menu->addSeparator();
        QAction *quitAct = menu->addAction("Quit WarpWidgets");

        connect(menu, &QMenu::triggered, this, [=](QAction *chosen) {
            if (newPostitAct && chosen == newPostitAct) {
                m_engine->openNewInstance(postitPath);
            } else if (chosen == closeAct) {
                m_engine->saveState(); m_view->close();
            } else if (chosen == fInc) {
                m_ctrl->increaseFontSize();
            } else if (chosen == fDef) {
                m_ctrl->defaultFontSize();
            } else if (chosen == fDec) {
                m_ctrl->decreaseFontSize();
            } else if (chosen == customAct) {
                QColor picked = QColorDialog::getColor(
                    QColor(m_ctrl->bgColor()), nullptr, "Choose background color");
                if (picked.isValid())
                    applyColor(picked.name());
            } else if (chosen == quitAct) {
                m_engine->saveState(); _exit(0);
            } else if (colorActs.contains(chosen)) {
                applyColor(chosen->data().toString());
            } else if (!chosen->data().isNull()) {
                m_engine->openWidget(chosen->data().toString());
            }
        });

        menu->popup(globalPos);
    }

protected:
    bool eventFilter(QObject *, QEvent *event) override
    {
        switch (event->type()) {

        // Keyboard: Ctrl++ / Ctrl+- / Ctrl+0
        case QEvent::KeyPress: {
            QKeyEvent *ke = static_cast<QKeyEvent *>(event);
            if (ke->modifiers() & Qt::ControlModifier) {
                switch (ke->key()) {
                case Qt::Key_Plus:
                case Qt::Key_Equal: m_ctrl->increaseFontSize(); return true;
                case Qt::Key_Minus: m_ctrl->decreaseFontSize(); return true;
                case Qt::Key_0:     m_ctrl->defaultFontSize();  return true;
                default: break;
                }
            }
            break;
        }

        case QEvent::MouseButtonPress: {
            QMouseEvent *e = static_cast<QMouseEvent *>(event);
            m_view->requestActivate();
            // RIGHT button = drag
            if (e->button() == Qt::RightButton) {
                m_dragging  = true;
                m_dragStart = e->globalPos() - m_view->position();
            }
            break;
        }

        case QEvent::MouseMove: {
            QMouseEvent *e = static_cast<QMouseEvent *>(event);
            if (m_dragging && (e->buttons() & Qt::RightButton))
                m_view->setPosition(e->globalPos() - m_dragStart);
            break;
        }

        case QEvent::MouseButtonRelease: {
            QMouseEvent *e = static_cast<QMouseEvent *>(event);
            if (e->button() == Qt::RightButton) {
                m_dragging = false;
                m_engine->saveState();
            }
            break;
        }

        default: break;
        }
        return false;
    }

private:
    void applyColor(const QString &hex)
    {
        if (m_isGlobal)
            m_engine->applyColorToAll(hex);
        else
            m_ctrl->setBgColor(hex);
    }

    QQuickView       *m_view;
    WidgetEngine     *m_engine;
    WidgetController *m_ctrl;
    bool              m_isGlobal;
    bool              m_dragging;
    QPoint            m_dragStart;
};

// ---------------------------------------------------------------------------
// WidgetEngine
// ---------------------------------------------------------------------------
WidgetEngine::WidgetEngine(QObject *parent)
    : QObject(parent)
    , m_settings(QSettings::IniFormat, QSettings::UserScope,
                 "OS2World", APP_NAME)
{
    connect(qApp, &QCoreApplication::aboutToQuit, this, &WidgetEngine::saveState);
}

WidgetEngine::~WidgetEngine()
{
    qDeleteAll(m_views);
}

void WidgetEngine::loadWidgetsFromDir(const QString &dirPath)
{
    m_widgetDir = dirPath;
    QDir dir(dirPath);
    if (!dir.exists()) { qWarning() << "Widget dir not found:" << dirPath; return; }

    // Load regular (single-instance) widgets
    for (const QFileInfo &fi : dir.entryInfoList({"*.qml"}, QDir::Files)) {
        if (fi.fileName() == "postit.qml") continue; // handled separately below
        const QString key = fi.fileName();
        m_settings.beginGroup("widgets");
        bool open = m_settings.value(key + "/open", true).toBool();
        int  sx   = m_settings.value(key + "/x", -1).toInt();
        int  sy   = m_settings.value(key + "/y", -1).toInt();
        m_settings.endGroup();

        if (!open) continue;

        QPoint pos(20, 20);
        if (sx >= 0 && sy >= 0) {
            pos = QPoint(sx, sy);
        } else {
            for (QQuickView *v : m_views)
                pos.ry() += v->rootObject() ? (int)v->rootObject()->height() + 10 : 0;
        }
        loadWidget(fi.absoluteFilePath(), pos, key);
    }

    // Restore saved Post-It instances
    QString postitPath = dir.filePath("postit.qml");
    if (QFile::exists(postitPath)) {
        m_settings.beginGroup("widgets");
        int count = m_settings.value("postit/count", 0).toInt();
        m_settings.endGroup();
        for (int i = 1; i <= count; ++i) {
            QString key = QString("postit.qml#%1").arg(i);
            m_settings.beginGroup("widgets");
            bool open = m_settings.value(key + "/open", true).toBool();
            int  sx   = m_settings.value(key + "/x", -1).toInt();
            int  sy   = m_settings.value(key + "/y", -1).toInt();
            m_settings.endGroup();
            if (!open) continue;
            QPoint pos(sx >= 0 && sy >= 0 ? QPoint(sx, sy) : QPoint(40 + i * 20, 40 + i * 20));
            loadWidget(postitPath, pos, key);
        }
        // If no saved instances, open one by default
        if (count == 0)
            openNewInstance(postitPath);
    }
}

void WidgetEngine::openNewInstance(const QString &qmlFile)
{
    // Find next free instance number
    m_settings.beginGroup("widgets");
    int count = m_settings.value("postit/count", 0).toInt() + 1;
    m_settings.setValue("postit/count", count);
    m_settings.endGroup();
    m_settings.sync();

    QString key = QString("%1#%2").arg(QFileInfo(qmlFile).fileName()).arg(count);
    QPoint pos(40 + (count % 5) * 30, 40 + (count % 5) * 30);
    loadWidget(qmlFile, pos, key);
}

void WidgetEngine::openWidget(const QString &qmlFile)
{
    m_settings.beginGroup("widgets");
    QString key = QFileInfo(qmlFile).fileName();
    m_settings.setValue(key + "/open", true);
    int sx = m_settings.value(key + "/x", -1).toInt();
    int sy = m_settings.value(key + "/y", -1).toInt();
    m_settings.endGroup();

    QPoint pos(20, 20);
    if (sx >= 0 && sy >= 0) {
        pos = QPoint(sx, sy);
    } else {
        for (QQuickView *v : m_views)
            pos.ry() += v->rootObject() ? (int)v->rootObject()->height() + 10 : 0;
    }
    loadWidget(qmlFile, pos, key);
}

QStringList WidgetEngine::closedWidgets() const
{
    QDir dir(m_widgetDir);
    if (!dir.exists()) return {};
    QSet<QString> open;
    for (QQuickView *v : m_views)
        if (v->isVisible()) open.insert(v->source().toLocalFile());
    QStringList result;
    for (const QFileInfo &fi : dir.entryInfoList({"*.qml"}, QDir::Files))
        if (!open.contains(fi.absoluteFilePath()))
            result << fi.absoluteFilePath();
    return result;
}

QString WidgetEngine::postitPath() const
{
    QString p = QDir(m_widgetDir).filePath("postit.qml");
    return QFile::exists(p) ? p : QString();
}

void WidgetEngine::applyColorToAll(const QString &color)
{
    for (WidgetController *ctrl : m_controllers)
        ctrl->setBgColor(color);
}

void WidgetEngine::loadWidget(const QString &qmlFile, const QPoint &pos,
                              const QString &keyOverride)
{
    QQuickView *view = new QQuickView();
    const QString key = keyOverride.isEmpty()
                        ? QFileInfo(qmlFile).fileName() : keyOverride;
    bool isGlobal = (key == "welcome.qml");

    SysInfoProvider  *sysInfo = new SysInfoProvider(view);
    WeatherProvider  *weather = new WeatherProvider(view);
    WidgetController *ctrl    = new WidgetController(view, this, key);
    m_controllers.append(ctrl);

    view->rootContext()->setContextProperty("SysInfo",    sysInfo);
    view->rootContext()->setContextProperty("Weather",    weather);
    view->rootContext()->setContextProperty("Widget",     ctrl);
    view->rootContext()->setContextProperty("widgetFile", qmlFile);

    view->setSource(QUrl::fromLocalFile(qmlFile));
    if (view->status() == QQuickView::Error) {
        qWarning() << "Failed to load widget:" << qmlFile;
        m_controllers.removeLast();
        delete view;
        return;
    }

    view->setFlags(Qt::FramelessWindowHint);
    view->setColor(QColor(ctrl->bgColor()));
    view->setResizeMode(QQuickView::SizeViewToRootObject);
    view->setPosition(pos);

    DragFilter *filter = new DragFilter(view, this, ctrl, isGlobal);
    view->installEventFilter(filter);
    view->show();

#ifdef Q_OS_OS2
    // Remove from XCenter / OS2 switch list so it doesn't appear in the taskbar
    {
        HWND hwnd = (HWND)view->winId();
        HSWITCH hsw = WinQuerySwitchHandle(hwnd, 0);
        if (hsw != NULLHANDLE)
            WinRemoveSwitchEntry(hsw);
    }
#endif

    m_views.append(view);
}

void WidgetEngine::saveState()
{
    m_settings.beginGroup("widgets");
    for (int i = 0; i < m_views.size(); ++i) {
        QQuickView *v = m_views[i];
        // Use controller key (which may include instance suffix like #1)
        QString key = (i < m_controllers.size())
                      ? m_controllers[i]->settingsKey()
                      : QFileInfo(v->source().toLocalFile()).fileName();
        m_settings.setValue(key + "/open", v->isVisible());
        m_settings.setValue(key + "/x",    v->position().x());
        m_settings.setValue(key + "/y",    v->position().y());
    }
    m_settings.endGroup();
    m_settings.sync();
}

#include "WidgetEngine.moc"
