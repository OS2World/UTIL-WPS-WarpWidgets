#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>
#include <QStandardPaths>
#include "WidgetEngine.h"
#include "SysInfoProvider.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("WarpWidgets");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("OS2World");

    WidgetEngine engine;

    // Register C++ types so QML widgets can use them
    qmlRegisterSingletonType<SysInfoProvider>(
        "OS2Widgets", 1, 0, "SysInfo",
        [](QQmlEngine *, QJSEngine *) -> QObject * {
            return new SysInfoProvider();
        }
    );

    // Find widgets/ — next to exe (installed layout) or ../widgets (dev layout: exe in bin/)
    QString widgetDir = QDir(QCoreApplication::applicationDirPath()).filePath("widgets");
    if (!QDir(widgetDir).exists())
        widgetDir = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("../widgets");
    engine.loadWidgetsFromDir(widgetDir);

    return app.exec();
}
