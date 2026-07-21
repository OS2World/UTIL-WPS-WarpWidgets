import QtQuick 2.9

Rectangle {
    width:  280 * Widget.fontScale
    height: 130 * Widget.fontScale
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
        spacing: 12

        Column {
            width: parent.width
            spacing: 4
            Row {
                width: parent.width
                Text { text: "RAM"; color: "#AAAAAA"; font.pixelSize: Math.round(15 * Widget.fontScale); width: parent.width / 2 }
                Text {
                    text: SysInfo.ramUsedMB + " / " + SysInfo.ramTotalMB + " MB"
                    color: "#FFFFFF"; font.pixelSize: Math.round(15 * Widget.fontScale)
                    horizontalAlignment: Text.AlignRight; width: parent.width / 2
                }
            }
            Rectangle {
                width: parent.width; height: Math.round(10 * Widget.fontScale); color: "#333333"; radius: 5
                Rectangle {
                    width: parent.width * (SysInfo.ramPercent / 100); height: parent.height
                    color: SysInfo.ramPercent > 80 ? "#FF4444" : "#44AAFF"; radius: 5
                    Behavior on width { NumberAnimation { duration: 400 } }
                }
            }
        }

        Column {
            width: parent.width
            spacing: 4
            Row {
                width: parent.width
                Text { text: "CPU"; color: "#AAAAAA"; font.pixelSize: Math.round(15 * Widget.fontScale); width: parent.width / 2 }
                Text {
                    text: SysInfo.cpuPercent + "%"
                    color: "#FFFFFF"; font.pixelSize: Math.round(15 * Widget.fontScale)
                    horizontalAlignment: Text.AlignRight; width: parent.width / 2
                }
            }
            Rectangle {
                width: parent.width; height: Math.round(10 * Widget.fontScale); color: "#333333"; radius: 5
                Rectangle {
                    width: parent.width * (SysInfo.cpuPercent / 100); height: parent.height
                    color: SysInfo.cpuPercent > 80 ? "#FF4444" : "#44FF88"; radius: 5
                    Behavior on width { NumberAnimation { duration: 400 } }
                }
            }
        }
    }
}
