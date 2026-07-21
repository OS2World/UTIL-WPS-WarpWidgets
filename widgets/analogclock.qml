import QtQuick 2.9

Rectangle {
    id: root
    width:  240 * Widget.fontScale
    height: 268 * Widget.fontScale
    color:  Widget.bgColor
    radius: 12

    // Use SysInfo strings — proven correct on OS/2 (new Date() gives UTC)
    property int cHour:   parseInt(SysInfo.currentTime.split(":")[0]) || 0
    property int cMinute: parseInt(SysInfo.currentTime.split(":")[1]) || 0
    property int cSecond: parseInt(SysInfo.currentTime.split(":")[2]) || 0

    property real secs:  cSecond
    property real mins:  cMinute + secs / 60
    property real hours: (cHour % 12) + mins / 60

    // Angles in radians, 0 = 12 o'clock, clockwise
    property real secAngle:  secs  / 60 * 2 * Math.PI
    property real minAngle:  mins  / 60 * 2 * Math.PI
    property real hourAngle: hours / 12 * 2 * Math.PI

    // Menu button
    Rectangle {
        anchors { top: parent.top; right: parent.right; margins: 6 }
        width: 18; height: 18; radius: 9
        color: menuBtn.containsMouse ? "#334466" : "#555555"
        Text { anchors.centerIn: parent; text: "▾"; color: "#FFFFFF"; font.pixelSize: 14 }
        MouseArea { id: menuBtn; anchors.fill: parent; hoverEnabled: true
            onClicked: Widget.requestContextMenu() }
    }

    Item {
        id: face
        anchors {
            top: parent.top; left: parent.left; right: parent.right; bottom: dateText.top
            topMargin: 28; leftMargin: 12; rightMargin: 12; bottomMargin: 6
        }

        property real sz: Math.min(width, height)
        property real r:  sz / 2
        property real cx: width  / 2
        property real cy: height / 2

        // White clock face
        Rectangle {
            x: face.cx - face.r;  y: face.cy - face.r
            width: face.sz;       height: face.sz
            radius: face.r
            color: "#F4F4F4"
            border.color: "#BBBBBB"; border.width: 2
        }

        // Tick marks — static angles, transform:Rotation known to work for these
        Repeater {
            model: 60
            Rectangle {
                property bool hr: (index % 5 === 0)
                x: face.cx - width / 2
                y: face.cy - face.r + 3
                width:  hr ? 3  : 1.5
                height: hr ? 14 : 8
                color:  hr ? "#333333" : "#AAAAAA"
                transform: Rotation {
                    angle:    index * 6
                    origin.x: width / 2
                    origin.y: face.r - 3
                }
            }
        }

        // Hour numbers
        Repeater {
            model: 12
            Text {
                property real a:  (index + 1) / 12.0 * 2 * Math.PI - Math.PI / 2
                property real nr: face.r - Math.round(27 * Widget.fontScale)
                text:  index + 1
                color: "#111111"
                font { pixelSize: Math.round(face.r * 0.20); bold: true; family: "sans-serif" }
                x: face.cx + Math.cos(a) * nr - width  / 2
                y: face.cy + Math.sin(a) * nr - height / 2
            }
        }

        // ── Hands drawn as dot-trails using x/y math, no rotation needed ──

        // Hour hand — thick, short
        Repeater {
            model: 20
            Rectangle {
                property real frac: (index + 1) / 20
                x: face.cx + frac * face.r * 0.52 * Math.sin(root.hourAngle) - 5
                y: face.cy - frac * face.r * 0.52 * Math.cos(root.hourAngle) - 5
                width: 10; height: 10; radius: 5
                color: "#222222"
                z: 2
            }
        }

        // Minute hand — medium, longer
        Repeater {
            model: 28
            Rectangle {
                property real frac: (index + 1) / 28
                x: face.cx + frac * face.r * 0.74 * Math.sin(root.minAngle) - 4
                y: face.cy - frac * face.r * 0.74 * Math.cos(root.minAngle) - 4
                width: 7; height: 7; radius: 4
                color: "#222222"
                z: 2
            }
        }

        // Second hand — thin, long
        Repeater {
            model: 34
            Rectangle {
                property real frac: (index + 1) / 34
                x: face.cx + frac * face.r * 0.88 * Math.sin(root.secAngle) - 1
                y: face.cy - frac * face.r * 0.88 * Math.cos(root.secAngle) - 1
                width: 3; height: 3; radius: 2
                color: "#E05000"
                z: 3
            }
        }

        // Second hand tail
        Repeater {
            model: 8
            Rectangle {
                property real frac: (index + 1) / 8
                x: face.cx - frac * face.r * 0.20 * Math.sin(root.secAngle) - 1
                y: face.cy + frac * face.r * 0.20 * Math.cos(root.secAngle) - 1
                width: 3; height: 3; radius: 2
                color: "#E05000"
                z: 3
            }
        }

        // Centre cap
        Rectangle {
            x: face.cx - 6;  y: face.cy - 6
            width: 12; height: 12; radius: 6
            color: "#E05000"; z: 4
        }
        Rectangle {
            x: face.cx - 3;  y: face.cy - 3
            width: 6;  height: 6;  radius: 3
            color: "#F4F4F4"; z: 5
        }
    }

    Text {
        id: dateText
        anchors { bottom: parent.bottom; horizontalCenter: parent.horizontalCenter; bottomMargin: 8 }
        text: {
            var days   = ["Sun","Mon","Tue","Wed","Thu","Fri","Sat"]
            var months = ["Jan","Feb","Mar","Apr","May","Jun",
                          "Jul","Aug","Sep","Oct","Nov","Dec"]
            // Parse date from SysInfo.currentDate "dddd, MMMM d yyyy"
            var s = SysInfo.currentDate
            var pos = s.indexOf(", ")
            if (pos < 0) return ""
            var parts = s.substring(pos + 2).split(" ")
            var mNames = ["january","february","march","april","may","june",
                          "july","august","september","october","november","december"]
            var mIdx = mNames.indexOf((parts[0]||"").toLowerCase())
            var day  = parseInt(parts[1]||"1")
            var dow  = s.substring(0, pos).toLowerCase()
            var dNames = ["sunday","monday","tuesday","wednesday","thursday","friday","saturday"]
            var dIdx = dNames.indexOf(dow)
            return (dIdx >= 0 ? days[dIdx] : "") + "  " +
                   (mIdx >= 0 ? months[mIdx] : "") + " " + day
        }
        color: "#AAAAAA"
        font.pixelSize: Math.round(13 * Widget.fontScale)
    }
}
