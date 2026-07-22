import QtQuick 2.9

Rectangle {
    id: root
    width:  500 * Widget.fontScale
    height: 210 * Widget.fontScale
    color:  Widget.bgColor
    radius: 12

    // Menu button
    Rectangle {
        anchors { top: parent.top; right: parent.right; margins: 6 }
        width: 18; height: 18; radius: 9
        color: menuBtnArea.containsMouse ? "#334466" : "#333333"
        Text { anchors.centerIn: parent; text: "▾"; color: "#FFFFFF"; font.pixelSize: 14 }
        MouseArea { id: menuBtnArea; anchors.fill: parent; hoverEnabled: true
            onClicked: Widget.requestContextMenu() }
    }

    // ── City list with standard UTC offsets (minutes) ────────────────────
    property var cityList: [
        { name: "Local",        offset: 9999  },  // special: use system time as-is
        { name: "UTC",          offset: 0     },
        { name: "London",       offset: 0     },
        { name: "Paris",        offset: 60    },
        { name: "Berlin",       offset: 60    },
        { name: "Cairo",        offset: 120   },
        { name: "Moscow",       offset: 180   },
        { name: "Dubai",        offset: 240   },
        { name: "Karachi",      offset: 300   },
        { name: "Mumbai",       offset: 330   },
        { name: "Dhaka",        offset: 360   },
        { name: "Bangkok",      offset: 420   },
        { name: "Beijing",      offset: 480   },
        { name: "Tokyo",        offset: 540   },
        { name: "Sydney",       offset: 600   },
        { name: "Auckland",     offset: 720   },
        { name: "Sao Paulo",    offset: -180  },
        { name: "New York",     offset: -300  },
        { name: "Chicago",      offset: -360  },
        { name: "Denver",       offset: -420  },
        { name: "Los Angeles",  offset: -480  },
        { name: "Anchorage",    offset: -540  },
        { name: "Honolulu",     offset: -600  }
    ]

    property int cityIdx: 0

    // Persist selection across restarts
    Component.onCompleted: {
        var saved = parseInt(Widget.getExtra("calCityIdx", "0"))
        if (saved >= 0 && saved < cityList.length) cityIdx = saved
    }

    function nextCity() {
        cityIdx = (cityIdx + 1) % cityList.length
        Widget.setExtra("calCityIdx", cityIdx.toString())
    }
    function prevCity() {
        cityIdx = (cityIdx - 1 + cityList.length) % cityList.length
        Widget.setExtra("calCityIdx", cityIdx.toString())
    }

    // ── Parse system local time from SysInfo (proven correct on OS/2) ────
    property int sysHour:   parseInt(SysInfo.currentTime.split(":")[0]) || 0
    property int sysMinute: parseInt(SysInfo.currentTime.split(":")[1]) || 0

    // ── Compute city time from UTC offset ────────────────────────────────
    property int displayHour: {
        var city = cityList[cityIdx]
        if (city.offset === 9999) return sysHour          // Local
        var utcMin = sysHour * 60 + sysMinute - SysInfo.utcOffsetMin
        var cityMin = ((utcMin + city.offset) % 1440 + 1440) % 1440
        return Math.floor(cityMin / 60)
    }
    property int displayMinute: {
        var city = cityList[cityIdx]
        if (city.offset === 9999) return sysMinute
        var utcMin = sysHour * 60 + sysMinute - SysInfo.utcOffsetMin
        var cityMin = ((utcMin + city.offset) % 1440 + 1440) % 1440
        return cityMin % 60
    }

    property string timeStr: {
        var h = displayHour,   hs = h < 10 ? "0" + h : "" + h
        var m = displayMinute, ms = m < 10 ? "0" + m : "" + m
        return hs + ":" + ms
    }

    // ── Parse current date from SysInfo.currentDate ───────────────────────
    property var _dp: {
        var s   = SysInfo.currentDate
        var pos = s.indexOf(", ")
        return pos < 0 ? ["","1","2000"] : s.substring(pos + 2).split(" ")
    }
    property var monthMap: ({
        "january":0,"february":1,"march":2,"april":3,"may":4,"june":5,
        "july":6,"august":7,"september":8,"october":9,"november":10,"december":11
    })
    property var monthNames: (Lang.language, [
        Lang.t("month.1"),  Lang.t("month.2"),  Lang.t("month.3"),
        Lang.t("month.4"),  Lang.t("month.5"),  Lang.t("month.6"),
        Lang.t("month.7"),  Lang.t("month.8"),  Lang.t("month.9"),
        Lang.t("month.10"), Lang.t("month.11"), Lang.t("month.12")
    ])
    property int cMonth: { var n=(_dp[0]||"").toLowerCase(); var v=monthMap[n]; return v!==undefined?v:0 }
    property int cDay:   parseInt(_dp[1]||"1")    || 1
    property int cYear:  parseInt(_dp[2]||"2000") || 2000

    property int firstWeekday: {
        var y=cYear, m=cMonth+1
        var t=[0,3,2,5,0,3,5,1,4,6,2,4]
        var yy = m < 3 ? y-1 : y
        var dow=(yy+Math.floor(yy/4)-Math.floor(yy/100)+Math.floor(yy/400)+t[m-1]+1)%7
        return dow===0 ? 6 : dow-1
    }
    property int daysInMonth: {
        var m=cMonth+1
        if (m===2) return ((cYear%4===0&&cYear%100!==0)||cYear%400===0)?29:28
        return (m===4||m===6||m===9||m===11)?30:31
    }

    // ── Layout ────────────────────────────────────────────────────────────
    Column {
        anchors { fill: parent; topMargin: 30; bottomMargin: 8; leftMargin: 16; rightMargin: 16 }
        spacing: 0

        // City selector row
        Row {
            width: parent.width
            spacing: 4

            Rectangle {
                width: 22; height: 22; radius: 4
                color: prevCityArea.containsMouse ? "#334466" : "#222222"
                Text { anchors.centerIn: parent; text: "◀"; color: "#44AAFF"; font.pixelSize: 12 }
                MouseArea { id: prevCityArea; anchors.fill: parent; hoverEnabled: true; onClicked: root.prevCity() }
            }

            Text {
                width: parent.width - 52
                text:  root.cityList[root.cityIdx].name
                color: "#44AAFF"
                font { pixelSize: Math.round(13 * Widget.fontScale); bold: true }
                horizontalAlignment: Text.AlignHCenter
                elide: Text.ElideRight
            }

            Rectangle {
                width: 22; height: 22; radius: 4
                color: nextCityArea.containsMouse ? "#334466" : "#222222"
                Text { anchors.centerIn: parent; text: "▶"; color: "#44AAFF"; font.pixelSize: 12 }
                MouseArea { id: nextCityArea; anchors.fill: parent; hoverEnabled: true; onClicked: root.nextCity() }
            }
        }

        // Clock + calendar row
        Row {
            width: parent.width
            spacing: 12
            topPadding: 4

            // Large clock
            Text {
                width: parent.width * 0.36
                anchors.verticalCenter: parent.verticalCenter
                text:  root.timeStr
                color: "#FFFFFF"
                font { pixelSize: Math.round(64 * Widget.fontScale); bold: true; family: "monospace" }
                horizontalAlignment: Text.AlignLeft
                fontSizeMode: Text.HorizontalFit
                minimumPixelSize: 18
            }

            // Calendar
            Column {
                width: parent.width * 0.62
                anchors.verticalCenter: parent.verticalCenter
                spacing: Math.round(3 * Widget.fontScale)

                Text {
                    width: parent.width
                    text:  root.monthNames[root.cMonth]
                    color: "#FFFFFF"
                    font { pixelSize: Math.round(13 * Widget.fontScale); bold: true; letterSpacing: 2 }
                    horizontalAlignment: Text.AlignHCenter
                }

                Row {
                    width: parent.width
                    property real cw: width / 7
                    Repeater {
                        model: (Lang.language, [
                            Lang.t("day.mon"), Lang.t("day.tue"), Lang.t("day.wed"),
                            Lang.t("day.thu"), Lang.t("day.fri"), Lang.t("day.sat"),
                            Lang.t("day.sun")
                        ])
                        Text {
                            width: parent.cw
                            text:  modelData
                            color: index >= 5 ? "#555555" : "#777777"
                            font.pixelSize: Math.round(11 * Widget.fontScale)
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                }

                Grid {
                    id: calGrid
                    width: parent.width; columns: 7
                    property real cw: width / 7
                    property real ch: Math.round(22 * Widget.fontScale)

                    Repeater {
                        model: 42
                        delegate: Item {
                            width: calGrid.cw; height: calGrid.ch
                            property int  dayNum:  index - root.firstWeekday + 1
                            property bool valid:   dayNum >= 1 && dayNum <= root.daysInMonth
                            property bool today:   valid && dayNum === root.cDay
                            property bool weekend: (index % 7) >= 5

                            Rectangle {
                                anchors.centerIn: parent
                                width: Math.round(20 * Widget.fontScale); height: width; radius: width / 2
                                color: "#FFFFFF"; visible: today
                            }
                            Text {
                                anchors.centerIn: parent
                                text:  valid ? dayNum : ""
                                color: today ? "#000000" : weekend ? "#555555" : "#CCCCCC"
                                font { pixelSize: Math.round(12 * Widget.fontScale); bold: today }
                            }
                        }
                    }
                }
            }
        }
    }
}
