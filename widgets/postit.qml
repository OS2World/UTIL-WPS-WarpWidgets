import QtQuick 2.9

Rectangle {
    id: root
    width:  220 * Widget.fontScale
    height: 200 * Widget.fontScale
    color:  "#F5E642"
    radius: 4

    Component.onCompleted: {
        noteArea.text = Widget.getExtra("text", "")
    }

    Timer {
        id: saveTimer
        interval: 800; repeat: false
        onTriggered: Widget.setExtra("text", noteArea.text)
    }

    // Top bar
    Rectangle {
        id: topBar
        anchors { top: parent.top; left: parent.left; right: parent.right }
        height: 24
        color: "#D4C800"
        radius: 4
        Rectangle {
            anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
            height: 4; color: parent.color
        }
        Text {
            anchors.centerIn: parent
            text: "Post-It"
            color: "#555500"
            font { pixelSize: 11; bold: true }
        }
        // Menu button
        Rectangle {
            anchors { right: parent.right; verticalCenter: parent.verticalCenter; rightMargin: 4 }
            width: 18; height: 18; radius: 9
            color: menuBtn.containsMouse ? "#AA9900" : "transparent"
            Text { anchors.centerIn: parent; text: "▾"; color: "#555500"; font.pixelSize: 14 }
            MouseArea { id: menuBtn; anchors.fill: parent; hoverEnabled: true
                onClicked: Widget.requestContextMenu() }
        }
    }

    // Lined rules
    Column {
        anchors { top: topBar.bottom; left: parent.left; right: parent.right; bottom: parent.bottom }
        spacing: 0
        Repeater {
            model: 8
            Rectangle {
                width: root.width
                height: Math.round((root.height - 24) / 8)
                color: "transparent"
                Rectangle {
                    anchors.bottom: parent.bottom
                    x: 8; width: parent.width - 16; height: 1
                    color: "#CCAA00"; opacity: 0.4
                }
            }
        }
    }

    // Text input
    TextEdit {
        id: noteArea
        anchors { top: topBar.bottom; left: parent.left; right: parent.right; bottom: parent.bottom }
        anchors.margins: 8
        anchors.topMargin: 6
        wrapMode: TextEdit.Wrap
        clip: true
        color: "#333300"
        font { pixelSize: Math.round(14 * Widget.fontScale); family: "sans-serif" }
        onTextChanged: saveTimer.restart()
    }
}
