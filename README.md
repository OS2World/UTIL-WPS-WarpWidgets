# UTIL-WPS-WarpWidgets
A Desktop Widget Tool for ArcaOS

## Compile Tools
* Libraries: yum install qt5-qtbase qt5-qtbase-gui qt5-qtbase-devel qt5-qtdeclarative qt5-qtdeclarative-devel

## Compile Command
cd src
set EMXOMFLD_TYPE=WLINK
set EMXOMFLD_LINKER=wlink.exe
qmake-qt5 warpwidgets.pro
make

or run compile.cmd

## 
