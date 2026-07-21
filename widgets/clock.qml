import QtQuick 2.9

Rectangle {
    width:  280 * Widget.fontScale
    height: 110 * Widget.fontScale
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
        anchors { centerIn: parent; verticalCenterOffset: 8 }
        spacing: 6

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text:  SysInfo.currentTime
            color: "#FFFFFF"
            font { pixelSize: Math.round(48 * Widget.fontScale); bold: true; family: "monospace" }
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text:  SysInfo.currentDate
            color: "#AAAAAA"
            font.pixelSize: Math.round(16 * Widget.fontScale)
        }
    }
}
