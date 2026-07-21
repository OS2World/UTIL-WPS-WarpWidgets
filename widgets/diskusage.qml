import QtQuick 2.9

Rectangle {
    width:  280 * Widget.fontScale
    height: 100 * Widget.fontScale
    color:  Widget.bgColor
    radius: 10

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

    Column {
        anchors { fill: parent; margins: 14; topMargin: 30 }
        spacing: 4

        Row {
            width: parent.width
            Text { text: "DISK"; color: "#AAAAAA"; font.pixelSize: Math.round(15 * Widget.fontScale); width: parent.width / 2 }
            Text {
                text: SysInfo.diskFreeMB + " MB free"
                color: "#FFFFFF"; font.pixelSize: Math.round(15 * Widget.fontScale)
                horizontalAlignment: Text.AlignRight; width: parent.width / 2
            }
        }

        Rectangle {
            width: parent.width; height: Math.round(10 * Widget.fontScale); color: "#333333"; radius: 5
            Rectangle {
                width: parent.width * (SysInfo.diskPercent / 100); height: parent.height
                color: SysInfo.diskPercent > 90 ? "#FF4444" : SysInfo.diskPercent > 70 ? "#FFAA00" : "#AA44FF"
                radius: 5
                Behavior on width { NumberAnimation { duration: 400 } }
            }
        }

        Text {
            text: SysInfo.diskUsedMB + " / " + SysInfo.diskTotalMB + " MB used"
            color: "#666666"; font.pixelSize: Math.round(13 * Widget.fontScale)
            width: parent.width; horizontalAlignment: Text.AlignRight
        }
    }
}
