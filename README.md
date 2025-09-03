# OpenGLThingy - Cross-Platform Migration

This is the migrated cross-platform version of the OpenGLThingy project, converted from Visual Studio to CMake for multi-platform compatibility.

## Migration Changes

This project has been migrated from a Visual Studio C++ project to a cross-platform CMake-based build system that supports both Windows and Linux.

### What was migrated:
- Visual Studio project files → CMake build system
- Platform-specific code already had cross-platform compatibility
- Added build scripts for both Windows and Linux
- Maintained all existing functionality and dependencies

## Dependencies

### Windows
- GLFW 3.x (included in Dependencies folder)
- GLEW (included in Dependencies folder)
- OpenGL 3.3+
- Visual Studio 2019/2022 or compatible C++ compiler

### Linux
Required system packages (install via package manager):
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install libglfw3-dev libglew-dev libgl1-mesa-dev

# Fedora/CentOS/RHEL
sudo dnf install glfw-devel glew-devel mesa-libGL-devel

# Arch Linux
sudo pacman -S glfw-x11 glew mesa
```

## Building the Project

### Windows
```batch
# Option 1: Use the build script
build.bat

# Option 2: Manual build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . --config Debug
```

### Linux
```bash
# Option 1: Use the build script
chmod +x build.sh
./build.sh

# Option 2: Manual build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

## Project Structure

```
OpenGLThingy/
├── CMakeLists.txt          # Cross-platform build configuration
├── build.sh               # Linux build script
├── build.bat               # Windows build script
├── README.md               # This file
├── src/                    # Source code
│   ├── main.cpp           # Application entry point
│   ├── Application.cpp/h   # Main application class
│   ├── Renderer.cpp/h      # OpenGL renderer
│   ├── Shader.cpp/h        # Shader management
│   ├── Cube.cpp/h          # Cube rendering logic
│   ├── tests/              # Test framework
│   └── vendor/             # Third-party libraries (ImGui, GLM, stb_image)
├── res/                    # Resources (shaders, textures, etc.)
├── Dependencies/           # Windows-specific pre-built libraries
│   ├── GLFW/              # GLFW library for Windows
│   └── GLEW/              # GLEW library for Windows
└── build/                  # Build output directory
```

## Cross-Platform Notes

### Memory Management
The code already includes cross-platform memory allocation:
- Windows: Uses `_malloca()` for stack allocation
- Linux: Uses `malloc()` for heap allocation

### Debug Assertions
Platform-specific debug breaks:
- Windows: Uses `__debugbreak()`
- Linux: Uses `raise(SIGTRAP)`

### Library Linking
- Windows: Uses pre-built libraries from Dependencies folder
- Linux: Uses system-installed development packages

## Development Environment Setup

### Windows with Visual Studio
1. Open the project folder in Visual Studio 2019/2022
2. Visual Studio should automatically detect the CMakeLists.txt
3. Build using the CMake configuration

### Linux with any IDE/Editor
1. Install required dependencies (see Dependencies section)
2. Use any C++ IDE that supports CMake (CLion, Qt Creator, etc.)
3. Or build from command line using the provided scripts

## Troubleshooting

### Linux Build Issues
If you encounter missing library errors:
```bash
# Make sure all development packages are installed
sudo apt-get install build-essential cmake
sudo apt-get install libglfw3-dev libglew-dev libgl1-mesa-dev

# Verify library installation
ldconfig -p | grep -E "(glfw|GLEW|GL)"
```

### Windows Build Issues
- Ensure you have Visual Studio 2019 or later with C++ tools
- The Dependencies folder contains pre-built libraries for x64 architecture
- Make sure CMake is installed and accessible from PATH

## Original Visual Studio Project
The original Visual Studio project files are preserved in this migration folder for reference:
- `OpenGLThingy.sln` - Solution file
- `OpenGLThingy.vcxproj` - Project file
- `OpenGLThingy.vcxproj.filters` - Project filters

## License
This project maintains the same license as the original OpenGLThingy project.