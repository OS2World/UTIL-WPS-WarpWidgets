#ifndef TRANSLATIONPROVIDER_H
#define TRANSLATIONPROVIDER_H

#include <QObject>
#include <QString>
#include <QHash>
#include <QStringList>
#include <QSettings>

class TranslationProvider : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString language READ language NOTIFY languageChanged)

public:
    explicit TranslationProvider(QObject *parent = nullptr);

    QString language() const { return m_lang; }

    // QML: text: (Lang.language, Lang.t("key"))  — comma forces re-eval on language change
    Q_INVOKABLE QString t(const QString &key) const;

    // C++ convenience
    QString operator()(const QString &key) const { return t(key); }

    static QStringList availableCodes();
    static QString nativeName(const QString &code);

public slots:
    void setLanguage(const QString &code);

signals:
    void languageChanged();

private:
    QString m_lang;
    QHash<QString, QHash<QString, QString>> m_db;   // m_db[lang][key]
    QSettings m_settings;

    void build();
    void add(const QString &key,
             const QString &en,
             const QString &es,
             const QString &de,
             const QString &ru);
};

#endif // TRANSLATIONPROVIDER_H
