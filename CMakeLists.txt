cmake_minimum_required(VERSION 3.16)

project(OpenGLThingy VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Detect architecture
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_BITS "64")
    set(ARCH_NAME "x64")
else()
    set(ARCH_BITS "32")
    set(ARCH_NAME "Win32")
endif()

message(STATUS "Target architecture: ${ARCH_BITS}-bit (${ARCH_NAME})")

# Platform-specific library handling
if(WIN32)
    # Windows: Use bundled libraries
    set(GLFW3_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Dependencies/GLFW/include")
    set(GLEW_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Dependencies/GLEW/include")
    
    # Note: The current Dependencies folder only contains 64-bit libraries
    # For 32-bit support, you would need to add separate 32-bit library folders
    if(ARCH_BITS STREQUAL "32")
        message(WARNING "32-bit Windows build requested but only 64-bit libraries are available in Dependencies folder")
        message(WARNING "You may need to obtain 32-bit versions of GLFW and GLEW libraries")
        message(STATUS "Falling back to 64-bit libraries - build may fail on 32-bit targets")
    endif()
    
    set(GLFW3_LIBRARY "${CMAKE_SOURCE_DIR}/Dependencies/GLFW/lib-vc2022/glfw3.lib")
    set(GLEW_LIBRARY "${CMAKE_SOURCE_DIR}/Dependencies/GLEW/lib/glews.lib")
    set(OPENGL_LIBRARY "opengl32")
    add_definitions(-DGLEW_STATIC)
    
else()
    # Linux: Try to find system libraries (auto-detects architecture)
    find_library(GLFW3_LIBRARY NAMES glfw glfw3)
    if(NOT GLFW3_LIBRARY)
        message(STATUS "System GLFW not found. You may need to install libglfw3-dev")
        if(ARCH_BITS STREQUAL "32")
            message(STATUS "For 32-bit Ubuntu/Debian: sudo apt-get install libglfw3-dev:i386")
        else()
            message(STATUS "For 64-bit Ubuntu/Debian: sudo apt-get install libglfw3-dev")
        endif()
        message(STATUS "For now, creating a CMake configuration that can be built when libraries are available")
        set(GLFW3_LIBRARY "glfw3")
    endif()
    
    find_library(GLEW_LIBRARY NAMES GLEW)
    if(NOT GLEW_LIBRARY)
        message(STATUS "System GLEW not found. You may need to install libglew-dev")
        if(ARCH_BITS STREQUAL "32")
            message(STATUS "For 32-bit Ubuntu/Debian: sudo apt-get install libglew-dev:i386")
        else()
            message(STATUS "For 64-bit Ubuntu/Debian: sudo apt-get install libglew-dev")
        endif()
        set(GLEW_LIBRARY "GLEW")
    endif()
    
    find_library(OPENGL_LIBRARY NAMES GL)
    if(NOT OPENGL_LIBRARY)
        message(STATUS "OpenGL library not found. You may need to install mesa development packages")
        if(ARCH_BITS STREQUAL "32")
            message(STATUS "For 32-bit Ubuntu/Debian: sudo apt-get install libgl1-mesa-dev:i386")
        else()
            message(STATUS "For 64-bit Ubuntu/Debian: sudo apt-get install libgl1-mesa-dev")
        endif()
        set(OPENGL_LIBRARY "GL")
    endif()
    
    set(GLFW3_INCLUDE_DIR "")
    set(GLEW_INCLUDE_DIR "")
    add_definitions(-DGLEW_STATIC)
endif()

# Define source files
set(SOURCES
    src/main.cpp
    src/Application.cpp
    src/Cube.cpp
    src/IndexBuffer.cpp
    src/Renderer.cpp
    src/Shader.cpp
    src/texture.cpp
    src/VertexArray.cpp
    src/VertexBuffer.cpp
    src/tests/TestClearColor.cpp
    
    # Vendor sources
    src/vendor/glm/detail/glm.cpp
    src/vendor/imgui/imgui.cpp
    src/vendor/imgui/imgui_demo.cpp
    src/vendor/imgui/imgui_draw.cpp
    src/vendor/imgui/imgui_impl_glfw.cpp
    src/vendor/imgui/imgui_impl_opengl3.cpp
    src/vendor/imgui/imgui_tables.cpp
    src/vendor/imgui/imgui_widgets.cpp
    src/vendor/stb_image/stb_image.cpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    src
    src/vendor
    src/vendor/imgui
    src/vendor/glm
    src/vendor/stb_image
    ${GLFW3_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIR}
)

# Link libraries
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${GLFW3_LIBRARY}
        ${GLEW_LIBRARY}
        ${OPENGL_LIBRARY}
    )
    # Windows-specific definitions
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        WIN32
        _CONSOLE
        GLEW_STATIC
    )
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${GLFW3_LIBRARY}
        ${GLEW_LIBRARY}
        ${OPENGL_LIBRARY}
        ${CMAKE_DL_LIBS}
    )
    # Linux-specific definitions
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        GLEW_STATIC
    )
endif()

# Debug/Release configurations
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE _DEBUG)
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /W3 /sdl)
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -g)
    endif()
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE NDEBUG)
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /W3 /sdl /O2)
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -O2)
    endif()
endif()

# Platform-specific settings
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# Copy resources to build directory
file(COPY res DESTINATION ${CMAKE_BINARY_DIR})

# Print configuration info
message(STATUS "OpenGL found: ${OPENGL_FOUND}")
message(STATUS "GLFW include: ${GLFW3_INCLUDE_DIR}")
message(STATUS "GLFW library: ${GLFW3_LIBRARY}")
message(STATUS "GLEW include: ${GLEW_INCLUDE_DIR}")
message(STATUS "GLEW library: ${GLEW_LIBRARY}")