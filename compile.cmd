@echo off
rem WarpWidgets build script for ArcaOS / OS2 Warp
rem Run this from the project root: Z:\
rem Output: Z:\bin\warpwidgets.exe

rem Required linker settings for bww bitwise works Qt 5.15 on OS/2
set EMXOMFLD_TYPE=WLINK
set EMXOMFLD_LINKER=wlink.exe

rem Move into the source directory
cd src

rem Generate Makefile
echo Generating Makefile...
qmake-qt5 warpwidgets.pro
if errorlevel 1 (
    echo ERROR: qmake-qt5 failed
    cd ..
    exit /b 1
)

rem Build and save log
echo Building...
make > ..\bin\build.log 2>&1
if errorlevel 1 (
    echo ERROR: Build failed. Check bin\build.log for details.
    type ..\bin\build.log
    cd ..
    exit /b 1
)

cd ..
echo.
echo Build successful. Binary: bin\warpwidgets.exe
