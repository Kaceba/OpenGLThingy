@echo off
REM Cross-platform build script for OpenGLThingy (Windows)

REM Create build directory
if not exist build mkdir build
cd build

REM Configure with CMake
echo Configuring with CMake...
cmake .. -DCMAKE_BUILD_TYPE=Debug

REM Build the project
echo Building project...
cmake --build . --config Debug

echo Build complete!
echo Executable location: build\Debug\OpenGLThingy.exe
pause