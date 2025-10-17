@echo off
cd /d %~dp0

call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

cl ^
/std:c++latest ^
/Zc:__cplusplus ^
/EHsc ^
/utf-8 ^
/I"../src" ^
/I"../external/fmtlib/fmt_12.0.0" ^
../src/core/*.cpp ^
../src/logic/*.cpp ^
../src/util/*.cpp ^
test_run.cpp ^
/Fe:test_run.exe

if exist test_run.exe (
    test_run.exe
)
pause
