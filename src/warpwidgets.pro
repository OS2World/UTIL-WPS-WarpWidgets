QT += core gui qml quick network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = warpwidgets
TEMPLATE = app

SOURCES += \
    main.cpp \
    WidgetEngine.cpp \
    SysInfoProvider.cpp \
    WeatherProvider.cpp

HEADERS += \
    WidgetEngine.h \
    SysInfoProvider.h \
    WeatherProvider.h

RESOURCES += \
    resources.qrc

CONFIG += c++11

# Output binaries to ../bin (one level up from src/)
DESTDIR     = ../bin
OBJECTS_DIR = ../bin/.obj
MOC_DIR     = ../bin/.moc
RCC_DIR     = ../bin/.rcc
