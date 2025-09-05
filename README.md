# OpenGLThingy

A cross-platform OpenGL application demonstrating modern OpenGL rendering techniques with interactive 3D graphics and GUI controls.

https://github.com/user-attachments/assets/6f912ff5-c5fe-49a4-8147-b25e15b3b1e3

## What This Program Does

OpenGLThingy is an educational OpenGL application that renders animated 3D cubes with the following features:

- **3D Cube Rendering**: Displays textured 3D cubes with proper depth testing and perspective projection
- **Interactive GUI**: Uses ImGui for real-time parameter adjustment including:
  - Object positioning controls
  - Color animation speed adjustment
  - Shader parameter tweaking
- **Animated Graphics**: Dynamic color cycling and transformations
- **Modern OpenGL**: Uses OpenGL 3.3+ with vertex buffer objects, shaders, and modern rendering pipeline
- **Cross-Platform**: Runs on both Windows and Linux systems

The application serves as a practical example of:
- OpenGL context creation and management
- Vertex/Index buffer usage
- Shader compilation and uniform management
- Texture loading and binding
- 3D mathematics with GLM
- ImGui integration for debugging interfaces

## Dependencies

### Windows
The project includes pre-built libraries in the `Dependencies/` folder:
- **GLFW 3.x** - Window management and input handling
- **GLEW** - OpenGL extension loading
- **OpenGL 3.3+** - Graphics API (provided by graphics drivers)
- **Visual Studio 2019/2022** - C++ compiler (or compatible toolchain)

### Linux
Install development packages via your distribution's package manager:

#### Ubuntu/Debian (64-bit)
```bash
sudo apt-get update
sudo apt-get install libglfw3-dev libglew-dev libgl1-mesa-dev build-essential cmake
```

#### Fedora/CentOS/RHEL
```bash
sudo dnf install glfw-devel glew-devel mesa-libGL-devel gcc-c++ cmake
```

#### Arch Linux
```bash
sudo pacman -S glfw-x11 glew mesa cmake gcc
```

#### 32-bit Linux Support
For 32-bit builds, enable multiarch and install 32-bit packages:
```bash
sudo dpkg --add-architecture i386
sudo apt-get update
sudo apt-get install libglfw3-dev:i386 libglew-dev:i386 libgl1-mesa-dev:i386
```

## Building the Project

### Quick Build

#### Windows
```batch
build.bat
```

#### Linux
```bash
chmod +x build.sh
./build.sh
```

### Manual Build

#### Windows
```batch
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . --config Debug
```

#### Linux
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

### Architecture-Specific Builds

#### Windows 64-bit (default)
```batch
mkdir build-win64
cd build-win64
cmake .. -A x64
cmake --build . --config Debug
```

#### Linux 32-bit
```bash
mkdir build-linux32
cd build-linux32
cmake .. -DCMAKE_CXX_FLAGS="-m32" -DCMAKE_C_FLAGS="-m32"
cmake --build .
```

## Running the Application

After building, the executable will be in the `build/` directory:
- **Windows**: `build/Debug/OpenGLThingy.exe`
- **Linux**: `build/OpenGLThingy`

The application automatically copies required resources (shaders, textures) to the build directory.

## Reasons for the implementations

### Cross-Platform Design
- **CMake Build System**: Provides consistent builds across different platforms and toolchains
- **Platform Abstractions**: Code uses conditional compilation for platform-specific features (memory allocation, debug breaks)
- **Dependency Management**: Windows uses bundled libraries while Linux uses system packages for better integration

### Modern OpenGL Architecture
- **Vertex Array Objects**: Efficiently manage vertex attribute state
- **Buffer Objects**: Store vertex and index data on GPU for performance
- **Shader Programs**: Use GLSL for programmable rendering pipeline
- **Uniform Buffers**: Pass transformation matrices and parameters to shaders

### Educational Value
The project demonstrates:
- **3D Graphics Fundamentals**: Model-view-projection transformations, texture mapping, depth testing
- **Real-Time Rendering**: Game loop with update/render cycle
- **GUI Integration**: Immediate mode GUI for parameter adjustment and debugging
- **Resource Management**: Proper OpenGL object lifecycle management

### Code Organization
- **Abstraction Layers**: Wrapper classes for OpenGL objects (Shader, Texture, VertexArray, etc.)
- **RAII Pattern**: Automatic resource cleanup using destructors
- **Modular Design**: Separate classes for different rendering concerns
- **Test Framework**: Extensible testing system for different rendering scenarios

## Project Structure

```
OpenGLThingy/
├── CMakeLists.txt          # Cross-platform build configuration
├── build.sh/build.bat      # Platform-specific build scripts
├── src/                    # Source code
│   ├── main.cpp           # Application entry point
│   ├── Application.cpp/h   # Main application class and GLFW management
│   ├── Renderer.cpp/h      # OpenGL rendering abstraction
│   ├── Shader.cpp/h        # GLSL shader compilation and management
│   ├── Texture.cpp/h       # Texture loading and binding
│   ├── Cube.cpp/h          # Cube geometry and rendering
│   ├── VertexArray.cpp/h   # Vertex array object wrapper
│   ├── VertexBuffer.cpp/h  # Vertex buffer object wrapper
│   ├── IndexBuffer.cpp/h   # Index buffer object wrapper
│   ├── tests/              # Test framework for rendering scenarios
│   └── vendor/             # Third-party libraries
│       ├── imgui/          # Dear ImGui for GUI
│       ├── glm/            # OpenGL Mathematics library
│       └── stb_image/      # Image loading library
├── res/                    # Resources (shaders, textures)
│   ├── shaders/           # GLSL shader files
│   └── textures/          # Texture image files
└── Dependencies/           # Windows pre-built libraries
    ├── GLFW/              # GLFW library
    └── GLEW/              # GLEW library
```

## Development Environment

### Windows with Visual Studio
1. Open the project folder in Visual Studio 2019/2022
2. Visual Studio automatically detects CMakeLists.txt
3. Build using CMake configuration

### Linux/Cross-Platform IDEs
- **CLion**: Full CMake support with debugging
- **Qt Creator**: CMake project support
- **VS Code**: With CMake Tools extension
- **Command Line**: Use provided build scripts or manual CMake commands

## Troubleshooting

### Linux Build Issues
```bash
# Verify required packages are installed
dpkg -l | grep -E "(libglfw|libglew|libgl1-mesa)"

# Install missing development tools
sudo apt-get install build-essential cmake

# Check library linking
ldd build/OpenGLThingy
```

### Windows Build Issues
- Ensure Visual Studio 2019+ with C++ tools is installed
- Verify CMake is in PATH: `cmake --version`
- The Dependencies folder contains x64 libraries only
- For 32-bit builds, obtain 32-bit versions of GLFW and GLEW

### Runtime Issues
- **"Failed to initialize GLFW"**: Update graphics drivers
- **Shader compilation errors**: Check `res/shaders/` directory exists
- **Texture loading failures**: Verify `res/textures/` contains required files
- **Black screen**: Ensure OpenGL 3.3+ support in graphics drivers

## Known Issues and Potential Improvements

### Current Limitations

#### Platform Support
- **Windows 32-bit**: Only 64-bit libraries are included in Dependencies folder
  - Need to obtain and add 32-bit versions of GLFW and GLEW for full 32-bit Windows support
- **macOS**: Not currently supported
  - Would require additional CMake configuration and dependency management
- **ARM64**: No specific support for ARM-based systems (Apple Silicon, ARM Linux)

#### Graphics and Rendering
- **Fixed Pipeline Elements**: Some legacy OpenGL patterns could be modernized
- **Limited Texture Formats**: Currently only supports basic image formats via stb_image
- **No HDR Support**: Limited to standard dynamic range rendering
- **Fixed Cube Geometry**: Hardcoded cube vertices, could benefit from mesh loading system

#### Code Architecture
- **Error Handling**: Limited error recovery in some OpenGL operations
- **Resource Management**: Some OpenGL resources could benefit from more robust RAII wrappers
- **Memory Allocation**: Mixed use of `_malloca()` vs `malloc()` could be standardized
- **Configuration**: Hardcoded constants that could be moved to config files

#### Build System
- **Dependency Bundling**: Large Dependencies folder increases repository size
- **Package Management**: Could benefit from modern C++ package managers (vcpkg, Conan)
- **Build Caching**: No ccache or similar build acceleration
- **Static Analysis**: No integrated linting or static analysis tools

### Potential Improvements

#### Short-term Enhancements
1. **Add Windows 32-bit library support**
   - Download and organize 32-bit GLFW/GLEW libraries
   - Update CMake configuration for proper library selection

2. **Improve error handling**
   - Add more comprehensive OpenGL error checking
   - Better graceful failure modes for missing resources

3. **Enhanced GUI**
   - Add more ImGui controls for shader parameters
   - Performance metrics display (FPS, frame time)
   - Scene graph visualization

4. **Resource management**
   - Implement proper asset loading system
   - Add resource hot-reloading for development

#### Medium-term Goals
1. **Cross-platform packaging**
   - Windows installer/portable builds
   - Linux AppImage or Flatpak packages
   - macOS app bundle support

2. **Graphics improvements**
   - Normal mapping and advanced lighting
   - Shadow mapping implementation
   - Post-processing effects pipeline

3. **Code modernization**
   - Migrate to C++20 features where beneficial
   - Improve const-correctness throughout codebase
   - Add comprehensive unit testing

4. **Build system enhancements**
   - Integration with vcpkg or Conan for dependency management
   - Automated CI/CD pipeline for multiple platforms
   - Code coverage reporting

#### Long-term Enhancements
1. **Advanced rendering features**
   - Physically-based rendering (PBR)
   - Deferred rendering pipeline
   - Compute shader integration

2. **Platform expansion**
   - WebGL/WebAssembly port for browser deployment
   - Mobile platform support (Android/iOS)
   - Console platform investigation

3. **Educational features**
   - Interactive shader editor
   - Step-by-step rendering pipeline visualization
   - Performance profiling tools

### Performance Considerations

#### Current Performance Issues
- **CPU-GPU synchronization**: Some blocking OpenGL calls could be optimized
- **Memory allocations**: Frequent small allocations in render loop
- **Shader compilation**: Done at runtime, could be pre-compiled for production

#### Optimization Opportunities
- **Batched rendering**: Group similar objects to reduce draw calls
- **Instanced rendering**: For multiple similar objects
- **GPU profiling**: Add timing queries for bottleneck identification
- **Multi-threading**: Separate render and update threads

## License

Do whatever you want with it, it's not like i own anything in this :)
