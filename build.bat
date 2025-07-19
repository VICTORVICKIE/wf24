@echo off
echo Building WF24 LVGL SDL Simulator...

:: Get the number of processors
set JOBS=%NUMBER_OF_PROCESSORS%
echo Detected %JOBS% logical processors. Using -j%JOBS% for make.

if not exist build mkdir build
cd build

echo Configuring with MinGW...
cmake .. -G "MinGW Makefiles"

if %ERRORLEVEL% NEQ 0 (
    echo CMake configuration failed!
    exit /b 1
)

echo Building...
mingw32-make -j%JOBS%

if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    exit /b 1
)

echo Build completed successfully!
echo Executable is in: bin/main.exe
echo SDL2.dll is in: bin/SDL2.dll

cd ..
