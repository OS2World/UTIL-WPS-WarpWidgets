#ifndef WIDGETENGINE_H
#define WIDGETENGINE_H

#include <QObject>
#include <QList>
#include <QQuickView>
#include <QString>
#include <QSettings>

class TranslationProvider;

class WidgetEngine : public QObject
{
    Q_OBJECT
public:
    explicit WidgetEngine(QObject *parent = nullptr);
    ~WidgetEngine();

    void setLang(TranslationProvider *lang) { m_lang = lang; }
    TranslationProvider *lang() const       { return m_lang; }

    void loadWidgetsFromDir(const QString &path);
    void openWidget(const QString &qmlFile);
    void openNewInstance(const QString &qmlFile);
    void applyColorToAll(const QString &color);

    QStringList closedWidgets() const;
    QString     postitPath()    const;

public slots:
    void saveState();

private:
    void loadWidget(const QString &qmlFile, const QPoint &pos,
                    const QString &key = QString());

    QList<QQuickView *>             m_views;
    QList<class WidgetController *> m_controllers;
    QSettings                       m_settings;
    QString                         m_widgetDir;
    TranslationProvider            *m_lang = nullptr;
};

#endif // WIDGETENGINE_H
