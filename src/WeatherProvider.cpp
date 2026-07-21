#include "WeatherProvider.h"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QDateTime>
#include <QSettings>
#include <QDebug>

static const QStringList DEFAULT_CITIES = {
    // Americas
    "New York", "Los Angeles", "Chicago", "Toronto", "Mexico City",
    "Buenos Aires", "Sao Paulo", "Lima",
    // Europe
    "London", "Paris", "Berlin", "Madrid", "Rome", "Amsterdam",
    "Moscow", "Stockholm", "Warsaw", "Lisbon",
    // Middle East & Africa
    "Dubai", "Istanbul", "Cairo", "Lagos", "Nairobi", "Cape Town",
    // Asia
    "Tokyo", "Beijing", "Shanghai", "Mumbai", "Delhi",
    "Bangkok", "Singapore", "Seoul", "Hong Kong",
    // Oceania
    "Sydney", "Melbourne"
};

WeatherProvider::WeatherProvider(QObject *parent)
    : QObject(parent)
    , m_cities(DEFAULT_CITIES)
    , m_defaultCount(DEFAULT_CITIES.size())
{
    // Append saved custom cities
    QSettings s(QSettings::IniFormat, QSettings::UserScope, "OS2World", "WarpWidgets");
    QStringList custom = s.value("weather/customCities").toStringList();
    for (const QString &c : custom)
        if (!c.trimmed().isEmpty()) m_cities.append(c.trimmed());

    int saved = s.value("weather/cityIndex", 0).toInt();
    m_cityIndex = (saved >= 0 && saved < m_cities.size()) ? saved : 0;
    connect(&m_nam, &QNetworkAccessManager::finished,
            this, &WeatherProvider::onReplyFinished);

    // Auto-refresh every 30 minutes
    connect(&m_refreshTimer, &QTimer::timeout, this, &WeatherProvider::fetchWeather);
    m_refreshTimer.start(30 * 60 * 1000);

    // Initial fetch
    fetchWeather();
}

void WeatherProvider::saveCity()
{
    QSettings s(QSettings::IniFormat, QSettings::UserScope, "OS2World", "WarpWidgets");
    s.setValue("weather/cityIndex", m_cityIndex);
    s.sync();
}

void WeatherProvider::nextCity()
{
    m_cityIndex = (m_cityIndex + 1) % m_cities.size();
    saveCity();
    emit cityIndexChanged();
    fetchWeather();
}

void WeatherProvider::prevCity()
{
    m_cityIndex = (m_cityIndex - 1 + m_cities.size()) % m_cities.size();
    saveCity();
    emit cityIndexChanged();
    fetchWeather();
}

void WeatherProvider::selectCity(int index)
{
    if (index < 0 || index >= m_cities.size()) return;
    m_cityIndex = index;
    saveCity();
    emit cityIndexChanged();
    fetchWeather();
}

void WeatherProvider::refresh()
{
    fetchWeather();
}

void WeatherProvider::addCity(const QString &name)
{
    QString city = name.trimmed();
    if (city.isEmpty()) return;
    int existing = m_cities.indexOf(city);
    if (existing >= 0) {
        m_cityIndex = existing;
    } else {
        m_cities.append(city);
        m_cityIndex = m_cities.size() - 1;
        saveCustomCities();
        emit citiesChanged();
    }
    saveCity();
    emit cityIndexChanged();
    fetchWeather();
}

void WeatherProvider::removeCurrentCity()
{
    if (!isCustomCity()) return;
    m_cities.removeAt(m_cityIndex);
    saveCustomCities();
    emit citiesChanged();
    m_cityIndex = qBound(0, m_cityIndex - 1, m_cities.size() - 1);
    saveCity();
    emit cityIndexChanged();
    fetchWeather();
}

void WeatherProvider::saveCustomCities()
{
    QSettings s(QSettings::IniFormat, QSettings::UserScope, "OS2World", "WarpWidgets");
    s.setValue("weather/customCities", QVariant(m_cities.mid(m_defaultCount)));
    s.sync();
}

void WeatherProvider::fetchWeather()
{
    if (m_loading) return;

    m_city    = m_cities.at(m_cityIndex);
    m_loading = true;
    emit loadingChanged();

    // wttr.in — free, no API key, returns JSON
    QString cityEncoded = m_city;
    cityEncoded.replace(" ", "+");
    QUrl url(QString("https://wttr.in/%1?format=j1").arg(cityEncoded));

    QNetworkRequest req(url);
    req.setRawHeader("User-Agent", "WarpWidgets/1.0");
    m_nam.get(req);
}

void WeatherProvider::onReplyFinished(QNetworkReply *reply)
{
    m_loading = false;
    emit loadingChanged();

    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Weather fetch error:" << reply->errorString();
        m_description = "Network error";
        m_icon        = "?";
        emit dataChanged();
        reply->deleteLater();
        return;
    }

    QByteArray      data = reply->readAll();
    QJsonParseError err;
    QJsonDocument   doc  = QJsonDocument::fromJson(data, &err);
    reply->deleteLater();

    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        qWarning() << "Weather JSON parse error:" << err.errorString();
        m_description = "Parse error";
        m_icon        = "?";
        emit dataChanged();
        return;
    }

    QJsonObject root = doc.object();
    QJsonArray  cur  = root["current_condition"].toArray();
    if (cur.isEmpty()) {
        m_description = "No data";
        emit dataChanged();
        return;
    }

    QJsonObject c = cur[0].toObject();
    m_temperature = c["temp_C"].toString() + "°C";
    m_feelsLike   = "Feels " + c["FeelsLikeC"].toString() + "°C";
    m_humidity    = "Hum " + c["humidity"].toString() + "%";
    m_windSpeed   = "Wind " + c["windspeedKmph"].toString() + " km/h";

    QJsonArray descArr = c["weatherDesc"].toArray();
    m_description = descArr.isEmpty()
                    ? "Unknown"
                    : descArr[0].toObject()["value"].toString();

    m_icon        = weatherIcon(m_description);
    m_lastUpdated = QDateTime::currentDateTime().toString("hh:mm");

    emit dataChanged();
}

QString WeatherProvider::weatherIcon(const QString &desc)
{
    QString d = desc.toLower();
    if (d.contains("thunder") || d.contains("storm"))  return "⛈"; // ⛈
    if (d.contains("snow") || d.contains("blizzard"))  return "❄"; // ❄
    if (d.contains("sleet") || d.contains("freezing")) return "🌨"; // 🌨
    if (d.contains("drizzle") || d.contains("shower")) return "🌦"; // 🌦
    if (d.contains("rain"))                             return "🌧"; // 🌧
    if (d.contains("fog") || d.contains("mist")
        || d.contains("haze"))                          return "🌫"; // 🌫
    if (d.contains("overcast") || d.contains("cloudy"))return "☁";  // ☁
    if (d.contains("partly") || d.contains("partial")) return "⛅";  // ⛅
    if (d.contains("sunny") || d.contains("clear"))    return "☀";  // ☀
    return "🌤"; // 🌤 default
}
