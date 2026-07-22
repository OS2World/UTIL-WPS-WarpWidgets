# WarpWidgets
Desktop widget engine for ArcaOS / OS/2 Warp. Displays configurable floating widgets on the Workplace Shell desktop, including a clock, analog clock, calendar, system monitor, disk usage, weather, calculator, and sticky notes.

## LICENSE
* BSD 3-Clause

## COMPILE TOOLS
* Qt 5.15.2 (bww bitwise works port)
* GCC 9.2.0 (i686-pc-os2-emx)
* qmake-qt5
* make
* wlink

## DEPENDENCIES
Install with yum before compiling:

    yum install qt5-qtbase qt5-qtbase-gui qt5-qtbase-devel qt5-qtdeclarative qt5-qtdeclarative-devel

## COMPILING
Set the required linker environment variables and run qmake + make from the src/ directory:

    set EMXOMFLD_TYPE=WLINK
    set EMXOMFLD_LINKER=wlink.exe
    cd src
    qmake-qt5 warpwidgets.pro
    make

Or run the provided build script from the project root:

    compile.cmd

Output binary will be placed in the bin/ directory.

## RUNNING
Copy the contents of bin/ and widgets/ to the same destination folder, then run:

    warpwidgets.exe

Right-drag any widget to move it. Click the ▾ button for the context menu (color, font size, add/remove widgets).

## AUTHORS
* Martin Iturbide

## LINKS
* OS2World Community - https://www.os2world.com
* bww bitwise works Qt5 for OS/2 - https://www.bitwiseworks.com
