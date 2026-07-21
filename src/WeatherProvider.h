#ifndef WEATHERPROVIDER_H
#define WEATHERPROVIDER_H

#include <QObject>
#include <QTimer>
#include <QStringList>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class WeatherProvider : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString     city          READ city          NOTIFY dataChanged)
    Q_PROPERTY(QString     temperature   READ temperature   NOTIFY dataChanged)
    Q_PROPERTY(QString     feelsLike     READ feelsLike     NOTIFY dataChanged)
    Q_PROPERTY(QString     description   READ description   NOTIFY dataChanged)
    Q_PROPERTY(QString     humidity      READ humidity      NOTIFY dataChanged)
    Q_PROPERTY(QString     windSpeed     READ windSpeed     NOTIFY dataChanged)
    Q_PROPERTY(QString     icon          READ icon          NOTIFY dataChanged)
    Q_PROPERTY(QString     lastUpdated   READ lastUpdated   NOTIFY dataChanged)
    Q_PROPERTY(bool        loading       READ loading       NOTIFY loadingChanged)
    Q_PROPERTY(QStringList cities        READ cities        NOTIFY citiesChanged)
    Q_PROPERTY(int         cityIndex     READ cityIndex     NOTIFY cityIndexChanged)
    Q_PROPERTY(bool        isCustomCity  READ isCustomCity  NOTIFY cityIndexChanged)

public:
    explicit WeatherProvider(QObject *parent = nullptr);

    QString     city()         const { return m_city; }
    QString     temperature()  const { return m_temperature; }
    QString     feelsLike()    const { return m_feelsLike; }
    QString     description()  const { return m_description; }
    QString     humidity()     const { return m_humidity; }
    QString     windSpeed()    const { return m_windSpeed; }
    QString     icon()         const { return m_icon; }
    QString     lastUpdated()  const { return m_lastUpdated; }
    bool        loading()      const { return m_loading; }
    QStringList cities()       const { return m_cities; }
    int         cityIndex()    const { return m_cityIndex; }
    bool        isCustomCity() const { return m_cityIndex >= m_defaultCount; }

    Q_INVOKABLE void nextCity();
    Q_INVOKABLE void prevCity();
    Q_INVOKABLE void selectCity(int index);
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void addCity(const QString &name);
    Q_INVOKABLE void removeCurrentCity();

signals:
    void dataChanged();
    void loadingChanged();
    void cityIndexChanged();
    void citiesChanged();

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    void fetchWeather();
    void saveCity();
    void saveCustomCities();
    QString weatherIcon(const QString &desc);

    QNetworkAccessManager m_nam;
    QTimer                m_refreshTimer;

    QStringList m_cities;
    int         m_cityIndex   = 0;
    int         m_defaultCount = 0;

    QString m_city;
    QString m_temperature;
    QString m_feelsLike;
    QString m_description;
    QString m_humidity;
    QString m_windSpeed;
    QString m_icon;
    QString m_lastUpdated;
    bool    m_loading = false;
};

#endif // WEATHERPROVIDER_H
