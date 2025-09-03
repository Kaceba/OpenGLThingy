#!/bin/bash

# Cross-platform build script for OpenGLThingy

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build the project
echo "Building project..."
cmake --build . --config Debug

echo "Build complete!"
echo "Executable location: build/OpenGLThingy"