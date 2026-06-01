#pragma once

#include <memory>
#include <glm/glm.hpp>

struct GLFWwindow;
class Renderer;
class VertexArray;
class VertexBuffer;
class IndexBuffer;
class Shader;
class Texture;
class Cube;

class OpenGLApp
{
public:
    OpenGLApp();
    ~OpenGLApp();

    bool Initialize();
    void Run();

private:
    bool InitializeGLFW();
    bool InitializeOpenGL();
    bool InitializeImGui();
    bool SetupScene();
    void Update();
    void Render();
    void RenderQuad(const glm::vec3& translation);
    void RenderCube();
    void RenderUI();
    void Cleanup();

    GLFWwindow* window = nullptr;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<VertexArray> va;
    std::unique_ptr<VertexBuffer> vb;
    std::unique_ptr<IndexBuffer> ib;
    std::unique_ptr<Shader> shader;
    std::unique_ptr<Texture> texture;

    std::unique_ptr<Cube> cube;
    std::unique_ptr<Shader> cubeShader;

    float colorValue = 0.0f;
    float colorSpeed = 0.25f;
    float colorDirection = 1.0f;
    glm::vec3 translationA{-400.0f, 0.0f, 0.0f};
    glm::vec3 translationB{ 400.0f, 0.0f, 0.0f};
    glm::mat4 projection{1.0f};
    glm::mat4 view{1.0f};

    glm::mat4 projection3D{1.0f};
    glm::mat4 view3D{1.0f};

    bool showQuads = true;
    bool showCube = false;
    bool cubeUseTexture = false;

    float cubeRotationX = 0.0f;
    float cubeRotationY = 0.0f;
    float cubeRotationSpeed = 45.0f; // degrees per second

    bool glfwInitialized = false;
    bool imguiInitialized = false;

    double lastFrameTime = 0.0;
    float deltaTime = 0.0f;
};
