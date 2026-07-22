import QtQuick 2.9

Rectangle {
    id: root
    width:  280 * Widget.fontScale
    height: (drives.count === 0 ? 60 : 44 + drives.count * 62) * Widget.fontScale
    color:  Widget.bgColor
    radius: 10

    // ▾ menu button
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

    // Header
    Text {
        anchors { top: parent.top; left: parent.left; margins: 14 }
        text: (Lang.language, Lang.t("disk.title"))
        color: "#AAAAAA"
        font.pixelSize: Math.round(13 * Widget.fontScale)
        font.bold: true
    }

    // One row per drive
    Column {
        anchors { top: parent.top; topMargin: Math.round(34 * Widget.fontScale)
                  left: parent.left; right: parent.right
                  leftMargin: 14; rightMargin: 14 }
        spacing: Math.round(8 * Widget.fontScale)

        Repeater {
            id: drives
            model: SysInfo.drives

            Item {
                width: parent.width
                height: Math.round(54 * Widget.fontScale)

                // Drive letter + free space
                Row {
                    id: topRow
                    width: parent.width
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
                    width: parent.width
                    height: Math.round(10 * Widget.fontScale)
                    color: "#333333"; radius: 5
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
                    text: modelData.usedMB + " / " + modelData.totalMB + " MB"
                    color: "#666666"
                    font.pixelSize: Math.round(12 * Widget.fontScale)
                    width: parent.width
                    horizontalAlignment: Text.AlignRight
                }
            }
        }
    }

    // Shown only when no drives detected
    Text {
        visible: drives.count === 0
        anchors.centerIn: parent
        text: (Lang.language, Lang.t("disk.nodrives"))
        color: "#666666"
        font.pixelSize: Math.round(14 * Widget.fontScale)
    }
}
