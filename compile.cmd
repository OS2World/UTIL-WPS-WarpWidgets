@echo off
rem WarpWidgets build script for ArcaOS / OS2 Warp
rem Run from the project root directory.
rem Output: bin\warpwidgets.exe
rem
rem If the build fails with "wlink.exe: No such file or directory",
rem change the line below to:  set EMXOMFLD_LINKER=wl.exe

set EMXOMFLD_TYPE=WLINK
set EMXOMFLD_LINKER=wlink.exe

rem ── Create bin directory if needed ───────────────────────────────────────────
if not exist bin md bin

rem ── Generate Makefile ────────────────────────────────────────────────────────
cd src
echo Generating Makefile...
qmake-qt5 warpwidgets.pro
if errorlevel 1 (
    echo ERROR: qmake-qt5 failed
    cd ..
    exit /b 1
)

rem ── Build (stderr to screen so errors are visible; stdout to log) ─────────────
echo Building...
make > ..\bin\build.log
cd ..

rem ── Check by file existence, not exit code ───────────────────────────────────
if exist bin\warpwidgets.exe (
    echo.
    echo Build successful. Binary: bin\warpwidgets.exe
) else (
    echo.
    echo Build FAILED. See bin\build.log for details.
    exit /b 1
)
