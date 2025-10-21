@echo off
:: ===============================
::  build.bat - Build executable and run
::  Author: DragonTaki
:: ===============================
cd /d %~dp0

:: Set up VS compilation environment
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

:: Create "build" and "obj" folders
if not exist build mkdir build
if not exist build\obj mkdir build\obj

:: Clean up old .obj files
del /q build\obj\*.obj 2>nul

:: Recursively compile all .cpp files in the src folder
for /R src %%F in (*.cpp) do (
    echo Compiling %%F
    rem Convert backslashes in the src relative path to underscores to avoid conflicts between obj files with the same name in subfolders
    set "relPath=%%~pF"
    setlocal enabledelayedexpansion
    set "relPath=!relPath:\=_!"
    cl ^
    /std:c++latest ^
    /Zc:__cplusplus ^
    /EHsc ^
    /Od ^
    /Zi ^
    /utf-8 ^
    /I"." ^
    /I"src" ^
    /I"external\fmtlib\fmt_12.0.0" ^
    /c %%F ^
    /Fo"build\obj\!relPath!%%~nF.obj" ^
    /Fd"build\main.pdb"
    endlocal
)

:: Compile fmt.cpp
echo Compiling external\fmtlib\fmt_12.0.0\src\fmt.cpp
cl ^
/std:c++latest ^
/Zc:__cplusplus ^
/EHsc ^
/Od ^
/Zi ^
/utf-8 ^
/I"." ^
/I"external\fmtlib\fmt_12.0.0" ^
/c external\fmtlib\fmt_12.0.0\src\fmt.cpp ^
/Fo"build\obj\fmt.obj" ^
/Fd"build\main.pdb"

:: Link
link /DEBUG /PDB:"build\main.pdb" /OUT:"build\main.exe" build\obj\*.obj
