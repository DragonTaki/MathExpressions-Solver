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
../external/fmtlib/fmt_12.0.0/src/fmt.cpp ^
../src/core/AppLogger.cpp ^
../src/core/FilenameFormatter.cpp ^
../src/core/LogFileManager.cpp ^
../src/logic/Constraint.cpp ^
test_constraints.cpp ^
/Fe:test_constraints.exe

if exist test_constraints.exe (
    test_constraints.exe
)
pause
