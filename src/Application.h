#pragma once

#include <memory>
#include <glm/glm.hpp>  // Needed for glm::vec3 and glm::mat4

struct GLFWwindow;
class Renderer;
class VertexArray;
class VertexBuffer;
class IndexBuffer;
class Shader;
class Texture;

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
    void RenderUI();
    void Cleanup();

    GLFWwindow* window = nullptr;  // Pointer is fine
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<VertexArray> va;
    std::unique_ptr<VertexBuffer> vb;
    std::unique_ptr<IndexBuffer> ib;
    std::unique_ptr<Shader> shader;
    std::unique_ptr<Texture> texture;

    // Animation state
    float colorValue = 0.0f;
    // colorSpeed is units per second, colorDirection is ±1
    float colorSpeed = 0.25f;
    float colorDirection = 1.0f;

    glm::vec3 translationA;
    glm::vec3 translationB;
    glm::mat4 projection;
    glm::mat4 view;

    // Initialization flags to make Cleanup robust
    bool glfwInitialized = false;
    bool openglInitialized = false;
    bool imguiInitialized = false;
    bool sceneSetup = false;

    // Timing for frame-rate independent updates
    double lastFrameTime = 0.0; // seconds
    float deltaTime = 0.0f;     // seconds
};
