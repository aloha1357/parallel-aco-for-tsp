@echo off
REM Build script for Parallel ACO for TSP (Windows)
setlocal enabledelayedexpansion

echo === Parallel ACO for TSP Build Script (Windows) ===

REM Check if build directory exists
if not exist "build" (
    echo Creating build directory...
    mkdir build
)

cd build

echo Configuring CMake...
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_STANDARD=20

if !errorlevel! neq 0 (
    echo CMake configuration failed!
    exit /b 1
)

echo Building project...
cmake --build . --config Debug

if !errorlevel! neq 0 (
    echo Build failed!
    exit /b 1
)

echo Running unit tests...
if exist "Debug\unit_tests.exe" (
    Debug\unit_tests.exe
) else if exist "unit_tests.exe" (
    unit_tests.exe
) else (
    echo Unit tests executable not found, checking with ctest...
    ctest --verbose -C Debug
)

echo Build completed successfully!

echo === Build Summary ===
echo Executables built:
dir /b *.exe Debug\*.exe 2>nul

pause
