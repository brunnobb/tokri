@echo off
setlocal enabledelayedexpansion

rem Check standard build directories
if exist "build\Desktop_Qt_6_12_0_MSVC2022_64bit_Release\Tokri.exe" (
    set "EXE=build\Desktop_Qt_6_12_0_MSVC2022_64bit_Release\Tokri.exe"
) else if exist "build\Release\Tokri.exe" (
    set "EXE=build\Release\Tokri.exe"
) else if exist "build\Tokri.exe" (
    set "EXE=build\Tokri.exe"
) else (
    echo Error: Tokri.exe not found. Please build the project first.
    exit /b 1
)

set OUTDIR=dist\Windows
if not exist "%OUTDIR%" mkdir "%OUTDIR%"

rem Deploy Qt
windeployqt --dir "%OUTDIR%" "%EXE%"

rem Copy exe to release (overwrite)
copy /Y "%EXE%" "%OUTDIR%\Tokri.exe"

endlocal
