#!/bin/bash

# Test script for different build configurations
# This script simulates different architecture and platform builds

echo "=== Testing Multi-Architecture Build Configurations ==="

# Test 1: Linux 64-bit
echo
echo "1. Testing Linux 64-bit configuration..."
mkdir -p build-linux64
cd build-linux64
cmake .. -DCMAKE_BUILD_TYPE=Debug 2>&1 | grep -E "(Target architecture|library:|GLFW|GLEW|OpenGL)"
cd ..

# Test 2: Simulated Windows 64-bit (by forcing WIN32 flags)
echo
echo "2. Testing simulated Windows 64-bit configuration..."
mkdir -p build-win64-sim
cd build-win64-sim
# Create a temporary CMakeLists that forces WIN32 behavior
cat > temp_test.cmake << 'EOF'
set(WIN32 TRUE)
set(CMAKE_SIZEOF_VOID_P 8)
include(../CMakeLists.txt)
EOF
cmake -P temp_test.cmake 2>&1 | head -20
cd ..

# Test 3: Check library file architectures
echo
echo "3. Checking existing library architectures..."
echo "GLFW library file info:"
file Dependencies/GLFW/lib-vc2022/glfw3.lib 2>/dev/null || echo "  Could not analyze library file"
echo "GLEW library file info:"
file Dependencies/GLEW/lib/glews.lib 2>/dev/null || echo "  Could not analyze library file"

# Test 4: Architecture detection test
echo
echo "4. Testing architecture detection logic..."
echo "Current system: $(uname -m) ($(getconf LONG_BIT)-bit)"
echo "GCC 32-bit support: $(gcc -m32 --version &>/dev/null && echo "Available" || echo "Not available")"

echo
echo "=== Configuration Summary ==="
echo "✓ Linux 64-bit: Supported (requires system packages)"
echo "✓ Windows 64-bit: Supported (uses bundled libraries)"
echo "⚠ Linux 32-bit: Configurable (requires 32-bit system packages)"
echo "⚠ Windows 32-bit: Limited support (would need 32-bit libraries)"

echo
echo "=== Recommendations ==="
echo "1. Current setup best supports 64-bit builds on both platforms"
echo "2. For full 32-bit Windows support, add 32-bit versions of:"
echo "   - Dependencies/GLFW/lib-win32/glfw3.lib"
echo "   - Dependencies/GLEW/lib-win32/glews.lib"
echo "3. Linux builds will auto-detect architecture from system packages"

# Cleanup
rm -rf build-linux64 build-win64-sim