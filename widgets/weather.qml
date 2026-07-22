import QtQuick 2.9

Rectangle {
    id: root
    width:  280 * Widget.fontScale
    height: 210 * Widget.fontScale
    color:  Widget.bgColor
    radius: 10

    property bool showDropdown: false
    property bool addingCity:   false

    // Close dropdown when clicking outside
    MouseArea {
        anchors.fill: parent
        enabled: root.showDropdown
        onClicked: root.showDropdown = false
    }

    // Menu button
    Rectangle {
        anchors { top: parent.top; right: parent.right; margins: 6 }
        width: 18; height: 18; radius: 9; z: 20
        color: menuBtnArea.containsMouse ? "#334466" : "#444444"
        Text { anchors.centerIn: parent; text: "▾"; color: "#FFFFFF"; font.pixelSize: 14 }
        MouseArea { id: menuBtnArea; anchors.fill: parent; hoverEnabled: true
            onClicked: { root.showDropdown = false; Widget.requestContextMenu() } }
    }

    Column {
        anchors { fill: parent; margins: 12; topMargin: 30 }
        spacing: 6

        // ── City selector row ─────────────────────────────────────────────
        Row {
            width: parent.width
            spacing: 4
            visible: !root.addingCity

            // Dropdown trigger button
            Rectangle {
                width: parent.width - (Weather.isCustomCity ? 58 : 32); height: 26
                radius: 4
                color: dropTrigger.containsMouse ? "#334466" : "#1A2233"
                border.color: root.showDropdown ? "#44AAFF" : "#333355"
                border.width: 1

                Row {
                    anchors { fill: parent; leftMargin: 8; rightMargin: 4 }
                    spacing: 4
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        width: parent.width - 18
                        text:  Weather.loading ? (Lang.language, Lang.t("weather.loading")) : Weather.city
                        color: "#44AAFF"
                        font { pixelSize: Math.round(14 * Widget.fontScale); bold: true }
                        elide: Text.ElideRight
                    }
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: root.showDropdown ? "▴" : "▾"
                        color: "#44AAFF"; font.pixelSize: 11
                    }
                }
                MouseArea {
                    id: dropTrigger; anchors.fill: parent; hoverEnabled: true
                    onClicked: {
                        root.addingCity   = false
                        root.showDropdown = !root.showDropdown
                    }
                }
            }

            // Add city button
            Rectangle {
                width: 26; height: 26; radius: 4
                color: addArea.containsMouse ? "#224422" : "#222222"
                Text { anchors.centerIn: parent; text: "+"; color: "#44DD44"; font.pixelSize: 16 }
                MouseArea { id: addArea; anchors.fill: parent; hoverEnabled: true
                    onClicked: {
                        root.showDropdown = false
                        cityInput.text    = ""
                        root.addingCity   = true
                        cityInput.forceActiveFocus()
                    }
                }
            }

            // Remove button — only for custom cities
            Rectangle {
                width: 26; height: 26; radius: 4
                visible: Weather.isCustomCity
                color: removeArea.containsMouse ? "#442222" : "#222222"
                Text { anchors.centerIn: parent; text: "−"; color: "#DD4444"; font.pixelSize: 18 }
                MouseArea { id: removeArea; anchors.fill: parent; hoverEnabled: true
                    onClicked: Weather.removeCurrentCity() }
            }
        }

        // ── Inline city-add input row ─────────────────────────────────────
        Row {
            width: parent.width
            spacing: 4
            visible: root.addingCity

            Rectangle {
                width: parent.width - 58; height: 26
                color: "#222222"; radius: 4
                border.color: "#44AAFF"; border.width: 1
                TextInput {
                    id: cityInput
                    anchors { fill: parent; leftMargin: 7; rightMargin: 7; topMargin: 4 }
                    color: "#FFFFFF"
                    font.pixelSize: Math.round(14 * Widget.fontScale)
                    clip: true; maximumLength: 60
                    Keys.onReturnPressed: {
                        if (text.trim() !== "") Weather.addCity(text.trim())
                        root.addingCity = false
                    }
                    Keys.onEscapePressed: { root.addingCity = false }
                }
            }
            Rectangle {
                width: 26; height: 26; radius: 4
                color: confirmArea.containsMouse ? "#224422" : "#222222"
                Text { anchors.centerIn: parent; text: "✓"; color: "#44DD44"; font.pixelSize: 14 }
                MouseArea { id: confirmArea; anchors.fill: parent; hoverEnabled: true
                    onClicked: {
                        if (cityInput.text.trim() !== "") Weather.addCity(cityInput.text.trim())
                        root.addingCity = false
                    }
                }
            }
            Rectangle {
                width: 26; height: 26; radius: 4
                color: cancelArea.containsMouse ? "#442222" : "#222222"
                Text { anchors.centerIn: parent; text: "✕"; color: "#DD4444"; font.pixelSize: 13 }
                MouseArea { id: cancelArea; anchors.fill: parent; hoverEnabled: true
                    onClicked: root.addingCity = false }
            }
        }

        // ── Icon + temperature ────────────────────────────────────────────
        Row {
            width: parent.width
            spacing: 10

            Item {
                id: wicon
                width: 52 * Widget.fontScale; height: 52 * Widget.fontScale
                anchors.verticalCenter: parent.verticalCenter
                property real sz: Math.min(width, height)
                property real u:  sz / 10
                property string wtype: {
                    var d = Weather.description.toLowerCase()
                    if (d.indexOf("thunder") >= 0 || d.indexOf("storm")   >= 0) return "thunder"
                    if (d.indexOf("snow")    >= 0 || d.indexOf("blizzard") >= 0 || d.indexOf("sleet") >= 0) return "snow"
                    if (d.indexOf("rain")    >= 0 || d.indexOf("drizzle")  >= 0 || d.indexOf("shower") >= 0) return "rain"
                    if (d.indexOf("fog")     >= 0 || d.indexOf("mist")     >= 0 || d.indexOf("haze")   >= 0) return "fog"
                    if (d.indexOf("overcast") >= 0) return "cloudy"
                    if (d.indexOf("cloud")   >= 0) return "partly"
                    return "sunny"
                }
                Rectangle {
                    visible: wicon.wtype === "sunny" || wicon.wtype === "partly"
                    x: wicon.wtype === "partly" ? wicon.sz*0.36 : wicon.sz*0.12
                    y: wicon.wtype === "partly" ? 0 : wicon.sz*0.12
                    width: wicon.wtype === "partly" ? wicon.sz*0.58 : wicon.sz*0.76
                    height: width; radius: width/2; color: "#FFD020"
                }
                Rectangle {
                    visible: wicon.wtype !== "sunny" && wicon.wtype !== "fog"
                    x: 0; y: wicon.wtype === "partly" ? wicon.sz*0.42 : wicon.sz*0.30
                    width: wicon.sz*0.90; height: wicon.sz*0.38; radius: height/2
                    color: wicon.wtype === "thunder" ? "#556677" : "#CCCCCC"
                }
                Rectangle {
                    visible: wicon.wtype !== "sunny" && wicon.wtype !== "fog"
                    x: wicon.sz*0.22; y: wicon.wtype === "partly" ? wicon.sz*0.28 : wicon.sz*0.14
                    width: wicon.sz*0.46; height: wicon.sz*0.34; radius: height/2
                    color: wicon.wtype === "thunder" ? "#556677" : "#CCCCCC"
                }
                Repeater {
                    model: (wicon.wtype === "rain" || wicon.wtype === "thunder") ? 3 : 0
                    Rectangle {
                        x: wicon.sz*(0.12+index*0.30); y: wicon.sz*0.78
                        width: Math.max(2, wicon.u*0.7); height: wicon.sz*0.20; radius: width/2; color: "#66AAFF"
                    }
                }
                Repeater {
                    model: wicon.wtype === "snow" ? 4 : 0
                    Rectangle {
                        x: wicon.sz*(0.06+index*0.24); y: wicon.sz*0.78
                        width: Math.max(4, wicon.u); height: width; radius: width/2; color: "#BBDDFF"
                    }
                }
                Rectangle {
                    visible: wicon.wtype === "thunder"
                    x: wicon.sz*0.36; y: wicon.sz*0.62
                    width: Math.max(3, wicon.u*0.8); height: wicon.sz*0.28; color: "#FFD020"
                    transform: Rotation { angle: 15; origin.x: width/2; origin.y: height/2 }
                }
                Repeater {
                    model: wicon.wtype === "fog" ? 4 : 0
                    Rectangle {
                        x: wicon.sz*(0.05+(index%2)*0.10); y: wicon.sz*(0.12+index*0.22)
                        width: wicon.sz*(0.88-(index%2)*0.18)
                        height: Math.max(3, wicon.u*0.55); radius: height/2; color: "#999999"
                    }
                }
            }

            Column {
                anchors.verticalCenter: parent.verticalCenter
                spacing: 2
                Text { text: Weather.temperature; color: "#FFFFFF"
                    font { pixelSize: Math.round(36 * Widget.fontScale); bold: true } }
                Text { text: Weather.feelsLike; color: "#AAAAAA"
                    font.pixelSize: Math.round(13 * Widget.fontScale) }
            }
        }

        Text { width: parent.width; text: Weather.description; color: "#CCCCCC"
            font.pixelSize: Math.round(14 * Widget.fontScale); wrapMode: Text.WordWrap }

        Row {
            width: parent.width; spacing: 16
            Text { text: Weather.humidity;  color: "#888888"; font.pixelSize: Math.round(13 * Widget.fontScale) }
            Text { text: Weather.windSpeed; color: "#888888"; font.pixelSize: Math.round(13 * Widget.fontScale) }
        }

        Row {
            width: parent.width; spacing: 8
            Text {
                text: Weather.lastUpdated.length ? (Lang.language, Lang.t("weather.updated")) + Weather.lastUpdated : ""
                color: "#444444"; font.pixelSize: Math.round(11 * Widget.fontScale)
                width: parent.width - 60
            }
            Rectangle {
                width: 54; height: 18; radius: 4
                color: refreshArea.containsMouse ? "#334466" : "#222222"
                Text { anchors.centerIn: parent; text: (Lang.language, Lang.t("weather.refresh")); color: "#44AAFF"; font.pixelSize: 11 }
                MouseArea { id: refreshArea; anchors.fill: parent; hoverEnabled: true; onClicked: Weather.refresh() }
            }
        }
    }

    // ── Dropdown overlay — floats over widget content ─────────────────────
    Rectangle {
        id: dropdown
        visible: root.showDropdown
        z: 10
        anchors { top: parent.top; left: parent.left; right: parent.right; topMargin: 52 }
        height: Math.min(Weather.cities.length * 28, 180) + 2
        color: "#111122"
        border.color: "#44AAFF"; border.width: 1
        radius: 4
        clip: true

        ListView {
            id: cityList
            anchors { fill: parent; margins: 1 }
            model: Weather.cities
            clip: true
            currentIndex: Weather.cityIndex

            delegate: Rectangle {
                width: cityList.width; height: 28
                color: {
                    if (index === Weather.cityIndex)  return "#223355"
                    if (itemArea.containsMouse)        return "#1A2233"
                    return "transparent"
                }
                Row {
                    anchors { fill: parent; leftMargin: 10 }
                    spacing: 6
                    // Bullet for custom cities (index >= number of built-in cities)
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: "★"; font.pixelSize: 9
                        color: "#44AAFF"
                        visible: index >= 35   // DEFAULT_CITIES count
                    }
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: modelData
                        color: index === Weather.cityIndex ? "#44AAFF" : "#CCCCCC"
                        font { pixelSize: Math.round(13 * Widget.fontScale)
                               bold: index === Weather.cityIndex }
                    }
                }
                MouseArea {
                    id: itemArea; anchors.fill: parent; hoverEnabled: true
                    onClicked: {
                        Weather.selectCity(index)
                        root.showDropdown = false
                    }
                }
            }

            // Scroll to selected city when dropdown opens
            onVisibleChanged: if (visible) positionViewAtIndex(Weather.cityIndex, ListView.Center)
        }
    }
}
