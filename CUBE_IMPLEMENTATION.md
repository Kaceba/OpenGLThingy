# Rotating 3D Cube Implementation

## Overview
This implementation adds a rotating 3D cube to the existing OpenGL renderer while maintaining RAII principles and separation of concerns.

## New Components

### Cube Class (`src/Cube.h` / `src/Cube.cpp`)
- **Purpose**: Encapsulates 3D cube geometry and rendering logic
- **Key Features**:
  - RAII resource management with smart pointers
  - 24 vertices (4 per face) with positions and normals
  - Proper face normals for lighting calculations
  - Move semantics support, non-copyable for resource safety

### 3D Shader (`res/shaders/Cube.shader`)
- **Lighting Model**: Phong lighting with ambient, diffuse, and specular components
- **Uniforms**:
  - `u_MVP`: Model-View-Projection matrix
  - `u_Model`: Model matrix for world space calculations
  - `u_Color`: Base cube color
  - `u_LightPos`: Light position in world space
  - `u_ViewPos`: Camera position for specular calculation

## Application Integration

### Modified Files
- `src/Application.h`: Added cube resources and rotation state
- `src/Application.cpp`: Integrated cube rendering pipeline
- `src/Shader.h/.cpp`: Added `SetUniform3f` method for vec3 uniforms

### New Features
- **ImGui Controls**: "Show Cube" checkbox and rotation speed slider
- **Dual Rendering**: Supports both 2D quads and 3D cube simultaneously
- **Frame-Rate Independent**: Rotation speed in degrees per second
- **Proper 3D Setup**: Perspective projection, depth testing, camera positioning

## Technical Details

### 3D Pipeline Setup
- **Projection**: Perspective with 45° FOV, aspect ratio matching window
- **View Matrix**: Camera positioned at (3,3,3) looking at origin
- **Depth Testing**: Enabled with GL_LESS function

### Rotation System
- Independent X and Y axis rotation with different speeds
- Rotations applied in model matrix before MVP calculation
- Smooth frame-rate independent animation

### Cross-Platform Compatibility
- Tested syntax compatibility for both Windows (MSVC) and Linux (GCC)
- Uses existing platform-specific debugging macros (`#ifdef _WIN32`)
- OpenGL 3.3 core profile for broad hardware support

## Usage
1. Compile and run the application
2. Check "Show Cube" in the ImGui control panel
3. Adjust rotation speed with the slider (0-180 degrees/sec)
4. Both quads and cube can be displayed simultaneously