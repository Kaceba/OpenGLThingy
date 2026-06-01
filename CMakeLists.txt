cmake_minimum_required(VERSION 3.16)
project(OpenGLThingy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/Application.cpp
    src/Cube.cpp
    src/IndexBuffer.cpp
    src/Renderer.cpp
    src/Shader.cpp
    src/Texture.cpp
    src/VertexArray.cpp
    src/VertexBuffer.cpp

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

target_include_directories(${PROJECT_NAME} PRIVATE
    src
    src/vendor
    src/vendor/imgui
    src/vendor/glm
    src/vendor/stb_image
)

if(WIN32)
    target_include_directories(${PROJECT_NAME} PRIVATE
        Dependencies/GLFW/include
        Dependencies/GLEW/include
    )
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/Dependencies/GLFW/lib-vc2022/glfw3.lib
        ${CMAKE_SOURCE_DIR}/Dependencies/GLEW/lib/glews.lib
        opengl32
    )
    target_compile_definitions(${PROJECT_NAME} PRIVATE GLEW_STATIC)
else()
    find_package(OpenGL REQUIRED)
    find_package(glfw3 REQUIRED)
    find_package(GLEW REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        OpenGL::GL
        glfw
        GLEW::GLEW
    )
endif()

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()
