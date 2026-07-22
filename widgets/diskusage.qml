import QtQuick 2.9

Rectangle {
    id: root
    width:  280 * Widget.fontScale
    color:  Widget.bgColor
    radius: 10

    // ── Drive visibility state ────────────────────────────────────────────────
    property var  hiddenDrives: []
    property bool configMode:   false

    Component.onCompleted: {
        var saved = Widget.getExtra("hiddenDrives", "")
        hiddenDrives = (saved.length > 0) ? saved.split(",") : []
    }

    function isDriveHidden(letter) {
        return hiddenDrives.indexOf(letter) >= 0
    }

    function toggleDrive(letter) {
        var h   = hiddenDrives.slice()
        var idx = h.indexOf(letter)
        if (idx >= 0) h.splice(idx, 1)
        else          h.push(letter)
        hiddenDrives = h
        Widget.setExtra("hiddenDrives", h.join(","))
    }

    // Visible drives (config mode shows all; normal mode hides excluded ones)
    property var allDrives:     SysInfo.drives
    property int visibleCount:  {
        if (configMode) return allDrives.length
        var n = 0
        for (var i = 0; i < allDrives.length; i++)
            if (!isDriveHidden(allDrives[i].letter)) n++
        return n
    }

    height: {
        var rows = configMode ? allDrives.length : visibleCount
        return (rows === 0 ? 60 : 44 + rows * 62) * Widget.fontScale
    }

    // ── ▾ menu button ─────────────────────────────────────────────────────────
    Rectangle {
        anchors { top: parent.top; right: parent.right; margins: 6 }
        width: 18; height: 18; radius: 9
        color: menuBtnArea.containsMouse ? "#334466" : "#444444"
        Text { anchors.centerIn: parent; text: "▾"; color: "#FFFFFF"; font.pixelSize: 14 }
        MouseArea {
            id: menuBtnArea; anchors.fill: parent; hoverEnabled: true
            onClicked: Widget.requestContextMenu()
        }
    }

    // ── ⚙ config toggle button ────────────────────────────────────────────────
    Rectangle {
        anchors { top: parent.top; right: parent.right; rightMargin: 28; topMargin: 6 }
        width: 18; height: 18; radius: 9
        color: cfgBtnArea.containsMouse
               ? (root.configMode ? "#665533" : "#334466")
               : (root.configMode ? "#886600" : "#444444")
        Text { anchors.centerIn: parent; text: "⚙"; color: "#FFFFFF"; font.pixelSize: 11 }
        MouseArea {
            id: cfgBtnArea; anchors.fill: parent; hoverEnabled: true
            onClicked: root.configMode = !root.configMode
        }
    }

    // ── Header ────────────────────────────────────────────────────────────────
    Text {
        id: header
        anchors { top: parent.top; left: parent.left; margins: 14 }
        text: root.configMode
              ? "SELECT DRIVES"
              : (Lang.language, Lang.t("disk.title"))
        color: root.configMode ? "#FFAA33" : "#AAAAAA"
        font.pixelSize: Math.round(13 * Widget.fontScale)
        font.bold: true
    }

    // ── Drive rows ────────────────────────────────────────────────────────────
    Column {
        anchors {
            top: header.bottom; topMargin: Math.round(8 * Widget.fontScale)
            left: parent.left; right: parent.right
            leftMargin: 14; rightMargin: 14
        }
        spacing: Math.round(8 * Widget.fontScale)

        Repeater {
            model: root.allDrives

            Item {
                id: driveRow
                property bool hidden: root.isDriveHidden(modelData.letter)
                // In normal mode skip hidden drives entirely
                visible: root.configMode || !hidden
                width:  parent.width
                height: visible ? Math.round(54 * Widget.fontScale) : 0

                // Config mode: checkbox + dimming
                Rectangle {
                    visible: root.configMode
                    anchors { left: parent.left; verticalCenter: parent.verticalCenter }
                    width: 18; height: 18; radius: 3
                    color: driveRow.hidden ? "#333333" : "#44AAFF"
                    border.color: "#44AAFF"; border.width: 1
                    Text {
                        anchors.centerIn: parent
                        text: driveRow.hidden ? "" : "✓"
                        color: "#FFFFFF"; font.pixelSize: 12; font.bold: true
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: root.toggleDrive(modelData.letter)
                    }
                }

                // Drive letter + free space
                Row {
                    id: topRow
                    anchors.left: root.configMode
                                  ? parent.left : parent.left
                    anchors.leftMargin: root.configMode
                                        ? Math.round(26 * Widget.fontScale) : 0
                    width: parent.width - anchors.leftMargin
                    opacity: (root.configMode && driveRow.hidden) ? 0.35 : 1.0

                    Text {
                        text: modelData.letter
                        color: "#DDDDDD"
                        font.pixelSize: Math.round(15 * Widget.fontScale)
                        font.bold: true
                        width: Math.round(36 * Widget.fontScale)
                    }
                    Text {
                        text: modelData.freeMB + " " + (Lang.language, Lang.t("disk.free"))
                        color: "#FFFFFF"
                        font.pixelSize: Math.round(14 * Widget.fontScale)
                        horizontalAlignment: Text.AlignRight
                        width: parent.width - Math.round(36 * Widget.fontScale)
                    }
                }

                // Progress bar
                Rectangle {
                    anchors.top: topRow.bottom
                    anchors.topMargin: Math.round(4 * Widget.fontScale)
                    anchors.left: root.configMode
                                  ? parent.left : parent.left
                    anchors.leftMargin: root.configMode
                                        ? Math.round(26 * Widget.fontScale) : 0
                    width:  parent.width - anchors.leftMargin
                    height: Math.round(10 * Widget.fontScale)
                    color: "#333333"; radius: 5
                    opacity: (root.configMode && driveRow.hidden) ? 0.35 : 1.0
                    Rectangle {
                        width: parent.width * (modelData.percent / 100)
                        height: parent.height
                        color: modelData.percent > 90 ? "#FF4444"
                             : modelData.percent > 70 ? "#FFAA00"
                             : "#AA44FF"
                        radius: 5
                        Behavior on width { NumberAnimation { duration: 400 } }
                    }
                }

                // Used / total
                Text {
                    anchors.bottom: parent.bottom
                    anchors.left: root.configMode
                                  ? parent.left : parent.left
                    anchors.leftMargin: root.configMode
                                        ? Math.round(26 * Widget.fontScale) : 0
                    text: modelData.usedMB + " / " + modelData.totalMB + " MB"
                    color: "#666666"
                    font.pixelSize: Math.round(12 * Widget.fontScale)
                    width: parent.width - anchors.leftMargin
                    horizontalAlignment: Text.AlignRight
                    opacity: (root.configMode && driveRow.hidden) ? 0.35 : 1.0
                }

                // Whole-row tap in config mode toggles the drive
                MouseArea {
                    anchors.fill: parent
                    enabled: root.configMode
                    onClicked: root.toggleDrive(modelData.letter)
                }
            }
        }
    }

    // ── Empty state ───────────────────────────────────────────────────────────
    Text {
        visible: root.visibleCount === 0 && !root.configMode
        anchors.centerIn: parent
        text: (Lang.language, Lang.t("disk.nodrives"))
        color: "#666666"
        font.pixelSize: Math.round(14 * Widget.fontScale)
    }
}
