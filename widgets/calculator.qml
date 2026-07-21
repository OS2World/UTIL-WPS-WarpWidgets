import QtQuick 2.9

Rectangle {
    id: root
    width:  220 * Widget.fontScale
    height: 340 * Widget.fontScale
    color:  Widget.bgColor
    radius: 12

    property string display:  "0"
    property string operand:  ""
    property string operator: ""
    property bool   newNum:   true

    function press(key) {
        if (key >= "0" && key <= "9" || key === ".") {
            if (key === "." && display.indexOf(".") >= 0) return
            if (newNum) {
                display = (key === ".") ? "0." : key
                newNum  = false
            } else {
                if (display === "0" && key !== ".") display = key
                else display = display + key
            }
        } else if (key === "C") {
            display  = "0"
            operand  = ""
            operator = ""
            newNum   = true
        } else if (key === "±") {
            if (display !== "0")
                display = display.charAt(0) === "-"
                          ? display.substring(1)
                          : "-" + display
        } else if (key === "%") {
            display = "" + (parseFloat(display) / 100)
            newNum  = true
        } else if (key === "=") {
            if (operator !== "" && operand !== "") {
                var a = parseFloat(operand)
                var b = parseFloat(display)
                var r
                if      (operator === "+") r = a + b
                else if (operator === "−") r = a - b
                else if (operator === "×") r = a * b
                else if (operator === "÷") r = b !== 0 ? a / b : "ERR"
                // Format: trim unnecessary decimals
                if (r !== "ERR") {
                    var s = "" + parseFloat(r.toFixed(10))
                    display = s.length > 12 ? parseFloat(r.toPrecision(8)).toString() : s
                } else {
                    display = "ERR"
                }
                operator = ""
                operand  = ""
                newNum   = true
            }
        } else {
            // +, −, ×, ÷
            if (operator !== "" && operand !== "" && !newNum) {
                // chain: compute pending first
                root.press("=")
            }
            operand  = display
            operator = key
            newNum   = true
        }
    }

    // Menu button
    Rectangle {
        anchors { top: parent.top; right: parent.right; margins: 6 }
        width: 18; height: 18; radius: 9; z: 10
        color: menuBtn.containsMouse ? "#334466" : "#555555"
        Text { anchors.centerIn: parent; text: "▾"; color: "#FFFFFF"; font.pixelSize: 14 }
        MouseArea { id: menuBtn; anchors.fill: parent; hoverEnabled: true
            onClicked: Widget.requestContextMenu() }
    }

    Column {
        anchors { fill: parent; topMargin: 28; margins: 10 }
        spacing: 8

        // Display
        Rectangle {
            width: parent.width; height: 52 * Widget.fontScale
            color: "#111111"; radius: 6
            Text {
                anchors { right: parent.right; verticalCenter: parent.verticalCenter
                          rightMargin: 10 }
                text:  root.display
                color: "#FFFFFF"
                font { pixelSize: Math.round(28 * Widget.fontScale); family: "monospace" }
                fontSizeMode: Text.HorizontalFit
                minimumPixelSize: 14
                width: parent.width - 16
                horizontalAlignment: Text.AlignRight
            }
            // Operator indicator
            Text {
                anchors { left: parent.left; top: parent.top; margins: 6 }
                text:  root.operator
                color: "#44AAFF"
                font.pixelSize: Math.round(13 * Widget.fontScale)
            }
        }

        // Buttons
        property real bw: (width - 18) / 4
        property real bh: Math.round(44 * Widget.fontScale)

        Repeater {
            model: [
                ["C",  "±",  "%",  "÷"],
                ["7",  "8",  "9",  "×"],
                ["4",  "5",  "6",  "−"],
                ["1",  "2",  "3",  "+"],
                ["0",  "",   ".",  "="]
            ]

            Row {
                spacing: 6
                Repeater {
                    model: modelData
                    Rectangle {
                        width:  (modelData === "0") ? parent.parent.bw * 2 + 6 :
                                (modelData === "")  ? 0 : parent.parent.bw
                        height: parent.parent.bh
                        visible: modelData !== ""
                        radius: 6
                        color: {
                            if (modelData === "C" || modelData === "±" || modelData === "%")
                                return btnArea.containsMouse ? "#888888" : "#666666"
                            if (modelData === "÷" || modelData === "×" ||
                                modelData === "−" || modelData === "+")
                                return btnArea.containsMouse ? "#66BBFF" : "#44AAFF"
                            if (modelData === "=")
                                return btnArea.containsMouse ? "#66BBFF" : "#44AAFF"
                            return btnArea.containsMouse ? "#444444" : "#333333"
                        }
                        Text {
                            anchors.centerIn: parent
                            text:  modelData
                            color: (modelData === "C" || modelData === "±" || modelData === "%")
                                   ? "#000000" : "#FFFFFF"
                            font { pixelSize: Math.round(18 * Widget.fontScale); bold: true }
                        }
                        MouseArea {
                            id: btnArea; anchors.fill: parent; hoverEnabled: true
                            onClicked: root.press(modelData)
                        }
                    }
                }
            }
        }
    }
}
