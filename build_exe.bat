@echo off
:: ===============================
::  build_exe.bat - Build executable only
::  Author: DragonTaki
:: ===============================
cd /d %~dp0

:: Set up VS compilation environment
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

:: Create "build" folder
if not exist build mkdir build
cd build

:: Use CMake to generate VS2022 project
echo [1/3] Configuring CMake project...
cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release ..

if %errorlevel% neq 0 (
    echo [Error] CMake configuration failed.
    pause
    exit /b %errorlevel%
)

:: Build project (Release mode)
echo [2/3] Building project...
cmake --build . --config Release

if %errorlevel% neq 0 (
    echo [Error] Build failed.
    pause
    exit /b %errorlevel%
)

:: Show output result
echo [3/3] Build completed successfully!
echo Executable located at:
echo   %cd%\bin\MathExpressionsSolver.exe
echo ========================================================
pause
