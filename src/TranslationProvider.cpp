#include "TranslationProvider.h"

TranslationProvider::TranslationProvider(QObject *parent)
    : QObject(parent)
    , m_settings(QSettings::IniFormat, QSettings::UserScope, "WarpWidgets", "WarpWidgets")
{
    build();
    m_lang = m_settings.value("app/language", "en").toString();
    if (!availableCodes().contains(m_lang))
        m_lang = "en";
}

QStringList TranslationProvider::availableCodes()
{
    return { "en", "es", "de", "ru" };
}

QString TranslationProvider::nativeName(const QString &code)
{
    if (code == "es") return QString::fromUtf8("Espa\xC3\xB1ol");
    if (code == "de") return QString::fromUtf8("Deutsch");
    if (code == "ru") return QString::fromUtf8("\xD0\xA0\xD1\x83\xD1\x81\xD1\x81\xD0\xBA\xD0\xB8\xD0\xB9");
    return "English";
}

void TranslationProvider::setLanguage(const QString &code)
{
    if (code == m_lang || !availableCodes().contains(code)) return;
    m_lang = code;
    m_settings.setValue("app/language", code);
    m_settings.sync();
    emit languageChanged();
}

QString TranslationProvider::t(const QString &key) const
{
    auto it = m_db.find(m_lang);
    if (it != m_db.end()) {
        auto jt = it->find(key);
        if (jt != it->end()) return *jt;
    }
    // Fall back to English
    auto it2 = m_db.find("en");
    if (it2 != m_db.end()) {
        auto jt2 = it2->find(key);
        if (jt2 != it2->end()) return *jt2;
    }
    return key;
}

// ---------------------------------------------------------------------------
void TranslationProvider::add(const QString &key,
                              const QString &en, const QString &es,
                              const QString &de, const QString &ru)
{
    m_db["en"][key] = en;
    m_db["es"][key] = es;
    m_db["de"][key] = de;
    m_db["ru"][key] = QString::fromUtf8(ru.toUtf8());
}

void TranslationProvider::build()
{
    // ── Context menu ─────────────────────────────────────────────────────────
    add("menu.close",
        "Close this widget",
        "Cerrar este widget",
        "Widget schlie\xC3\x9Fen",
        "\xD0\x97\xD0\xB0\xD0\xBA\xD1\x80\xD1\x8B\xD1\x82\xD1\x8C \xD0\xB2\xD0\xB8\xD0\xB4\xD0\xB6\xD0\xB5\xD1\x82");

    add("menu.font",
        "Font",
        "Fuente",
        "Schrift",
        "\xD0\xA8\xD1\x80\xD0\xB8\xD1\x84\xD1\x82");

    add("menu.font.increase",
        "Increase  (Ctrl++)",
        "Aumentar  (Ctrl++)",
        "Gr\xC3\xB6\xC3\x9Fer  (Ctrl++)",
        "\xD0\xA3\xD0\xB2\xD0\xB5\xD0\xBB\xD0\xB8\xD1\x87\xD0\xB8\xD1\x82\xD1\x8C  (Ctrl++)");

    add("menu.font.default",
        "Default   (Ctrl+0)",
        "Por defecto  (Ctrl+0)",
        "Standard  (Ctrl+0)",
        "\xD0\x9F\xD0\xBE \xD1\x83\xD0\xBC\xD0\xBE\xD0\xBB\xD1\x87.  (Ctrl+0)");

    add("menu.font.decrease",
        "Decrease  (Ctrl-)",
        "Reducir  (Ctrl-)",
        "Kleiner  (Ctrl-)",
        "\xD0\xA3\xD0\xBC\xD0\xB5\xD0\xBD\xD1\x8C\xD1\x88\xD0\xB8\xD1\x82\xD1\x8C  (Ctrl-)");

    add("menu.color.all",
        "Background Color  (applies to ALL widgets)",
        "Color de fondo  (todos los widgets)",
        "Hintergrundfarbe  (alle Widgets)",
        "\xD0\xA6\xD0\xB2\xD0\xB5\xD1\x82 \xD1\x84\xD0\xBE\xD0\xBD\xD0\xB0  (\xD0\xB2\xD1\x81\xD0\xB5 \xD0\xB2\xD0\xB8\xD0\xB4\xD0\xB6\xD0\xB5\xD1\x82\xD1\x8B)");

    add("menu.color",
        "Background Color",
        "Color de fondo",
        "Hintergrundfarbe",
        "\xD0\xA6\xD0\xB2\xD0\xB5\xD1\x82 \xD1\x84\xD0\xBE\xD0\xBD\xD0\xB0");

    add("menu.color.darkgray",
        "Dark Gray (default)",
        "Gris oscuro (predeterminado)",
        "Dunkelgrau (Standard)",
        "\xD0\xA2\xD1\x91\xD0\xBC\xD0\xBD\xD0\xBE-\xD1\x81\xD0\xB5\xD1\x80\xD1\x8B\xD0\xB9 (\xD0\xBF\xD0\xBE \xD1\x83\xD0\xBC\xD0\xBE\xD0\xBB\xD1\x87.)");

    add("menu.color.black",
        "Black",
        "Negro",
        "Schwarz",
        "\xD0\xA7\xD1\x91\xD1\x80\xD0\xBD\xD1\x8B\xD0\xB9");

    add("menu.color.darkblue",
        "Dark Blue",
        "Azul oscuro",
        "Dunkelblau",
        "\xD0\xA2\xD1\x91\xD0\xBC\xD0\xBD\xD0\xBE-\xD1\x81\xD0\xB8\xD0\xBD\xD0\xB8\xD0\xB9");

    add("menu.color.darkgreen",
        "Dark Green",
        "Verde oscuro",
        "Dunkelgr\xC3\xBCn",
        "\xD0\xA2\xD1\x91\xD0\xBC\xD0\xBD\xD0\xBE-\xD0\xB7\xD0\xB5\xD0\xBB\xD1\x91\xD0\xBD\xD1\x8B\xD0\xB9");

    add("menu.color.darkpurple",
        "Dark Purple",
        "P\xC3\xBArpura oscuro",
        "Dunkellila",
        "\xD0\xA2\xD1\x91\xD0\xBC\xD0\xBD\xD0\xBE-\xD1\x84\xD0\xB8\xD0\xBE\xD0\xBB\xD0\xB5\xD1\x82\xD0\xBE\xD0\xB2\xD1\x8B\xD0\xB9");

    add("menu.color.darkred",
        "Dark Red",
        "Rojo oscuro",
        "Dunkelrot",
        "\xD0\xA2\xD1\x91\xD0\xBC\xD0\xBD\xD0\xBE-\xD0\xBA\xD1\x80\xD0\xB0\xD1\x81\xD0\xBD\xD1\x8B\xD0\xB9");

    add("menu.color.darkteal",
        "Dark Teal",
        "Cerceta oscuro",
        "Dunkelblaugr\xC3\xBCn",
        "\xD0\xA2\xD1\x91\xD0\xBC\xD0\xBD\xD0\xBE-\xD0\xB1\xD0\xB8\xD1\x80\xD1\x8E\xD0\xB7\xD0\xBE\xD0\xB2\xD1\x8B\xD0\xB9");

    add("menu.color.custom",
        "Custom...",
        "Personalizado...",
        "Benutzerdefiniert...",
        "\xD0\x94\xD1\x80\xD1\x83\xD0\xB3\xD0\xBE\xD0\xB9...");

    add("menu.newpostit",
        "New Post-It",
        "Nuevo Post-It",
        "Neues Post-It",
        "\xD0\x9D\xD0\xBE\xD0\xB2\xD1\x8B\xD0\xB9 \xD1\x81\xD1\x82\xD0\xB8\xD0\xBA\xD0\xB5\xD1\x80");

    add("menu.addwidget",
        "Add Widget",
        "Agregar widget",
        "Widget hinzuf\xC3\xBCgen",
        "\xD0\x94\xD0\xBE\xD0\xB1\xD0\xB0\xD0\xB2\xD0\xB8\xD1\x82\xD1\x8C \xD0\xB2\xD0\xB8\xD0\xB4\xD0\xB6\xD0\xB5\xD1\x82");

    add("menu.allopen",
        "(all widgets are open)",
        "(todos los widgets abiertos)",
        "(alle Widgets ge\xC3\xB6\xC3\xB6ffnet)",
        "(\xD0\xB2\xD1\x81\xD0\xB5 \xD0\xB2\xD0\xB8\xD0\xB4\xD0\xB6\xD0\xB5\xD1\x82\xD1\x8B \xD0\xBE\xD1\x82\xD0\xBA\xD1\x80\xD1\x8B\xD1\x82\xD1\x8B)");

    add("menu.language",
        "Language",
        "Idioma",
        "Sprache",
        "\xD0\xAF\xD0\xB7\xD1\x8B\xD0\xBA");

    add("menu.quit",
        "Quit WarpWidgets",
        "Salir de WarpWidgets",
        "WarpWidgets beenden",
        "\xD0\x97\xD0\xB0\xD0\xBA\xD1\x80\xD1\x8B\xD1\x82\xD1\x8C WarpWidgets");

    // ── Disk usage widget ─────────────────────────────────────────────────────
    add("disk.title",
        "DISK USAGE",
        "USO DE DISCO",
        "DATENTR\xC3\x84GER",
        "\xD0\x98\xD0\xA1\xD0\x9F\xD0\x9E\xD0\x9B\xD0\xAC\xD0\x97\xD0\x9E\xD0\x92\xD0\x90\xD0\x9D\xD0\x98\xD0\x95 \xD0\x94\xD0\x98\xD0\xA1\xD0\x9A\xD0\x90");

    add("disk.free",
        "MB free",
        "MB libres",
        "MB frei",
        "\xD0\x9C\xD0\x91 \xD1\x81\xD0\xB2\xD0\xBE\xD0\xB1\xD0\xBE\xD0\xB4\xD0\xBD\xD0\xBE");

    add("disk.nodrives",
        "No drives found",
        "Sin unidades",
        "Keine Laufwerke",
        "\xD0\x94\xD0\xB8\xD1\x81\xD0\xBA\xD0\xB8 \xD0\xBD\xD0\xB5 \xD0\xBD\xD0\xB0\xD0\xB9\xD0\xB4\xD0\xB5\xD0\xBD\xD1\x8B");

    // ── Weather widget ────────────────────────────────────────────────────────
    add("weather.loading",
        "Loading...",
        "Cargando...",
        "Lade...",
        "\xD0\x97\xD0\xB0\xD0\xB3\xD1\x80\xD1\x83\xD0\xB7\xD0\xBA\xD0\xB0...");

    add("weather.updated",
        "Updated ",
        "Actualizado ",
        "Aktualisiert ",
        "\xD0\x9E\xD0\xB1\xD0\xBD\xD0\xBE\xD0\xB2\xD0\xBB\xD0\xB5\xD0\xBD\xD0\xBE ");

    add("weather.refresh",
        "\xE2\x86\xBB Refresh",
        "\xE2\x86\xBB Actualizar",
        "\xE2\x86\xBB Aktualisieren",
        "\xE2\x86\xBB \xD0\x9E\xD0\xB1\xD0\xBD\xD0\xBE\xD0\xB2\xD0\xB8\xD1\x82\xD1\x8C");

    // ── Welcome widget ────────────────────────────────────────────────────────
    add("welcome.hint",
        "Right-drag to move  \xE2\x80\xA2  \xE2\x96\xBE for menu",
        "Arrastrar derecho: mover  \xE2\x80\xA2  \xE2\x96\xBE men\xC3\xBA",
        "Rechtsklick-Drag: verschieben  \xE2\x80\xA2  \xE2\x96\xBE Men\xC3\xBC",
        "\xD0\x9F\xD0\xB5\xD1\x80\xD0\xB5\xD1\x82\xD0\xB0\xD1\x89\xD0\xB8\xD1\x82\xD1\x8C: \xD0\xBF\xD0\xBF\xD0\xBA\xD0\xBC  \xE2\x80\xA2  \xE2\x96\xBE \xD0\xBC\xD0\xB5\xD0\xBD\xD1\x8E");

    // ── Calendar — month names (uppercase) ───────────────────────────────────
    add("month.1",  "JANUARY",   "ENERO",      "JANUAR",
        "\xD0\xAF\xD0\x9D\xD0\x92\xD0\x90\xD0\xA0\xD0\xAC");
    add("month.2",  "FEBRUARY",  "FEBRERO",    "FEBRUAR",
        "\xD0\xA4\xD0\x95\xD0\x92\xD0\xA0\xD0\x90\xD0\x9B\xD0\xAC");
    add("month.3",  "MARCH",     "MARZO",      "M\xC3\x84RZ",
        "\xD0\x9C\xD0\x90\xD0\xA0\xD0\xA2");
    add("month.4",  "APRIL",     "ABRIL",      "APRIL",
        "\xD0\x90\xD0\x9F\xD0\xA0\xD0\x95\xD0\x9B\xD0\xAC");
    add("month.5",  "MAY",       "MAYO",       "MAI",
        "\xD0\x9C\xD0\x90\xD0\x99");
    add("month.6",  "JUNE",      "JUNIO",      "JUNI",
        "\xD0\x98\xD0\xAE\xD0\x9D\xD0\xAC");
    add("month.7",  "JULY",      "JULIO",      "JULI",
        "\xD0\x98\xD0\xAE\xD0\x9B\xD0\xAC");
    add("month.8",  "AUGUST",    "AGOSTO",     "AUGUST",
        "\xD0\x90\xD0\x92\xD0\x93\xD0\xA3\xD0\xA1\xD0\xA2");
    add("month.9",  "SEPTEMBER", "SEPTIEMBRE", "SEPTEMBER",
        "\xD0\xA1\xD0\x95\xD0\x9D\xD0\xA2\xD0\xAF\xD0\x91\xD0\xA0\xD0\xAC");
    add("month.10", "OCTOBER",   "OCTUBRE",    "OKTOBER",
        "\xD0\x9E\xD0\x9A\xD0\xA2\xD0\xAF\xD0\x91\xD0\xA0\xD0\xAC");
    add("month.11", "NOVEMBER",  "NOVIEMBRE",  "NOVEMBER",
        "\xD0\x9D\xD0\x9E\xD0\xAF\xD0\x91\xD0\xA0\xD0\xAC");
    add("month.12", "DECEMBER",  "DICIEMBRE",  "DEZEMBER",
        "\xD0\x94\xD0\x95\xD0\x9A\xD0\x90\xD0\x91\xD0\xAA");

    // ── Calendar — weekday single-letter headers (Mon..Sun) ──────────────────
    // EN: Mon Tue Wed Thu Fri Sat Sun
    // ES: Lun Mar Mié Jue Vie Sáb Dom → L M X J V S D
    // DE: Mo  Di  Mi  Do  Fr  Sa  So  → M D M D F S S
    // RU: Пн  Вт  Ср  Чт  Пт  Сб  Вс → П В С Ч П С В
    add("day.mon", "M", "L", "M", "\xD0\x9F");
    add("day.tue", "T", "M", "D", "\xD0\x92");
    add("day.wed", "W", "X", "M", "\xD0\xA1");
    add("day.thu", "T", "J", "D", "\xD0\xA7");
    add("day.fri", "F", "V", "F", "\xD0\x9F");
    add("day.sat", "S", "S", "S", "\xD0\xA1");
    add("day.sun", "S", "D", "S", "\xD0\x92");
}
