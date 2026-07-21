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
        anchors { centerIn: parent; verticalCenterOffset: 8 }
        spacing: 6

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "WarpWidgets 1.0"
            color: "#44AAFF"
            font { pixelSize: Math.round(22 * Widget.fontScale); bold: true }
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Right-drag to move  •  ▾ for menu"
            color: "#666666"
            font.pixelSize: Math.round(13 * Widget.fontScale)
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "ArcaOS / OS2World"
            color: "#444444"
            font.pixelSize: Math.round(12 * Widget.fontScale)
        }
    }
}
