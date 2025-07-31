#!/bin/bash

# Build script for Parallel ACO for TSP
# This script handles the complete build process including dependencies

set -e  # Exit on any error

echo "=== Parallel ACO for TSP Build Script ==="

# Check if build directory exists
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir build
fi

cd build

echo "Configuring CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_STANDARD=20

echo "Building project..."
cmake --build . -j$(nproc)

echo "Running unit tests..."
if [ -f "./unit_tests" ]; then
    ./unit_tests
else
    echo "Unit tests executable not found, checking with ctest..."
    ctest --verbose
fi

echo "Build completed successfully!"

# Generate coverage report if available
if command -v lcov &> /dev/null; then
    echo "Generating coverage report..."
    make coverage 2>/dev/null || echo "Coverage target not available"
fi

echo "=== Build Summary ==="
echo "Executables built:"
ls -la aco_main unit_tests bdd_runner 2>/dev/null || echo "Some executables may not be built due to missing dependencies"
