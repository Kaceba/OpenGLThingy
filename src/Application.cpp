#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996) // MSVC deprecation noise
#endif

#include "Application.h"

#include "Config.h"
#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Cube.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

static void GLFWErrorCallback(int error, const char* description)
{
    std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
}

static void FramebufferSizeCallback(GLFWwindow* /*window*/, int width, int height)
{
    glViewport(0, 0, width, height);
}

OpenGLApp::OpenGLApp() = default;

OpenGLApp::~OpenGLApp()
{
    Cleanup();
}

bool OpenGLApp::Initialize()
{
    if (!InitializeGLFW()) return false;
    if (!InitializeOpenGL()) return false;
    if (!InitializeImGui()) return false;
    if (!SetupScene()) return false;

    lastFrameTime = glfwGetTime();

    return true;
}

void OpenGLApp::Run()
{
    while (window && !glfwWindowShouldClose(window))
    {
        Update();
        Render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

bool OpenGLApp::InitializeGLFW()
{
    // Register before glfwInit so init failures surface
    glfwSetErrorCallback(GLFWErrorCallback);

    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL Application", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // VSync

    int fbw, fbh;
    glfwGetFramebufferSize(window, &fbw, &fbh);
    glViewport(0, 0, fbw, fbh);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

    glfwInitialized = true;
    return true;
}

bool OpenGLApp::InitializeOpenGL()
{
    // Required for core profile to expose modern GL function pointers
    glewExperimental = GL_TRUE;
    if (GLenum err = glewInit(); err != GLEW_OK && err != GLEW_ERROR_NO_GLX_DISPLAY)
    {
        // GLEW_ERROR_NO_GLX_DISPLAY is harmless on Wayland/EGL — GLEW
        // queries GLX state even when the context isn't GLX, but the GL
        // function pointers still load correctly.
        // TODO(branch): replace GLEW with GLAD to remove the GLX poke
        // entirely. GLAD is a generated header-only loader, no system
        // dep, works identically on Wayland/X11/EGL/macOS/Windows.
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
        return false;
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    return true;
}

bool OpenGLApp::InitializeImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    const char* glsl_version = "#version 330";
    if (!ImGui_ImplGlfw_InitForOpenGL(window, true) ||
        !ImGui_ImplOpenGL3_Init(glsl_version))
    {
        std::cerr << "Failed to initialize ImGui" << std::endl;
        return false;
    }

    imguiInitialized = true;
    return true;
}

bool OpenGLApp::SetupScene()
{
    float positions[] = {
        600.0f, QUAD_Y_POS, 0.0f, 0.0f,
        600.0f + QUAD_SIZE, QUAD_Y_POS, 1.0f, 0.0f,
        600.0f + QUAD_SIZE, QUAD_Y_POS + QUAD_HEIGHT, 1.0f, 1.0f,
        600.0f, QUAD_Y_POS + QUAD_HEIGHT, 0.0f, 1.0f
    };

    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    try
    {
        va = std::make_unique<VertexArray>();
        vb = std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        va->AddBuffer(*vb, layout);

        ib = std::make_unique<IndexBuffer>(indices, 6);

        shader = std::make_unique<Shader>("res/shaders/Basic.shader");
        texture = std::make_unique<Texture>("res/textures/myimage.png");
        renderer = std::make_unique<Renderer>();

        cube = std::make_unique<Cube>(1.0f);
        cubeShader = std::make_unique<Shader>("res/shaders/Cube.shader");
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to setup scene: " << e.what() << std::endl;
        return false;
    }

    projection = glm::ortho(0.0f, static_cast<float>(WINDOW_WIDTH),
        0.0f, static_cast<float>(WINDOW_HEIGHT),
        -1.0f, 1.0f);
    view = glm::mat4(1.0f);

    projection3D = glm::perspective(glm::radians(45.0f),
        static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT),
        0.1f, 100.0f);
    view3D = glm::lookAt(glm::vec3(3.0f, 3.0f, 3.0f),
                         glm::vec3(0.0f, 0.0f, 0.0f),
                         glm::vec3(0.0f, 1.0f, 0.0f));

    shader->Bind();
    texture->Bind();
    shader->SetUniform1i("u_Texture", 0);

    return true;
}

void OpenGLApp::Update()
{
    double currentTime = glfwGetTime();
    deltaTime = static_cast<float>(currentTime - lastFrameTime);
    lastFrameTime = currentTime;

    const float minVal = 0.75f;
    const float maxVal = 1.0f;

    colorValue += colorDirection * colorSpeed * deltaTime;
    if (colorValue > maxVal)
    {
        colorValue = maxVal;
        colorDirection = -1.0f;
    }
    else if (colorValue < minVal)
    {
        colorValue = minVal;
        colorDirection = 1.0f;
    }

    if (showCube)
    {
        cubeRotationX += cubeRotationSpeed * deltaTime;
        cubeRotationY += cubeRotationSpeed * 0.7f * deltaTime;

        if (cubeRotationX >= 360.0f) cubeRotationX -= 360.0f;
        if (cubeRotationY >= 360.0f) cubeRotationY -= 360.0f;
    }
}

void OpenGLApp::Render()
{
    renderer->Clear();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (showCube)
    {
        RenderCube();
    }

    if (showQuads)
    {
        glDisable(GL_DEPTH_TEST);

        shader->Bind();
        shader->SetUniform4f("u_Color", colorValue, 1.0f, 1.0f, 1.0f);

        RenderQuad(translationA);
        RenderQuad(translationB);

        glEnable(GL_DEPTH_TEST);
    }

    RenderUI();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void OpenGLApp::RenderQuad(const glm::vec3& translation)
{
    glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
    glm::mat4 mvp = projection * view * model;
    shader->SetUniformMat4f("u_MVP", mvp);

    renderer->Draw(*va, *ib, *shader);
}

void OpenGLApp::RenderCube()
{
    cubeShader->Bind();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(cubeRotationX), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(cubeRotationY), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 mvp = projection3D * view3D * model;
    cubeShader->SetUniformMat4f("u_MVP", mvp);
    cubeShader->SetUniformMat4f("u_Model", model);
    cubeShader->SetUniform3f("u_Color", 0.8f, 0.6f, 0.2f);
    cubeShader->SetUniform3f("u_LightPos", 2.0f, 2.0f, 2.0f);
    cubeShader->SetUniform3f("u_ViewPos", 3.0f, 3.0f, 3.0f);
    cubeShader->SetUniformBool("u_UseTexture", cubeUseTexture);

    if (cubeUseTexture) {
        texture->Bind();
        cubeShader->SetUniform1i("u_Texture", 0);
    }

    cube->Render(*renderer, *cubeShader, model, view3D, projection3D);
}

void OpenGLApp::RenderUI()
{
    ImGui::SetNextWindowSize(ImVec2(380, 520), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);

    ImGui::Begin("OpenGL Renderer Controls", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    ImGui::PushItemWidth(-120.0f); // leave 120px for labels

    ImGui::SeparatorText("Scene Objects");
    ImGui::Checkbox("Show 2D Quads", &showQuads);
    ImGui::Checkbox("Show 3D Cube", &showCube);

    ImGui::Spacing();

    if (showQuads)
    {
        ImGui::SeparatorText("2D Quad Settings");
        ImGui::SliderFloat2("Quad 1 Pos", &translationA.x, -800.0f, 800.0f);
        ImGui::SliderFloat2("Quad 2 Pos", &translationB.x, -800.0f, 800.0f);
        ImGui::Spacing();
    }

    if (showCube)
    {
        ImGui::SeparatorText("3D Cube Settings");
        ImGui::SliderFloat("Rotation Speed", &cubeRotationSpeed, 0.0f, 180.0f, "%.0f°/sec");
        ImGui::Checkbox("Use Texture", &cubeUseTexture);
        ImGui::Text("Rotation: X=%.0f° Y=%.0f°", cubeRotationX, cubeRotationY);
        ImGui::Spacing();
    }

    ImGui::SeparatorText("Performance");
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("FPS: %.1f (%.2fms/frame)", io.Framerate, 1000.0f / io.Framerate);

    const char* glVersion = (const char*)glGetString(GL_VERSION);
    if (glVersion && strlen(glVersion) > 30) {
        std::string versionStr(glVersion);
        versionStr = versionStr.substr(0, 30) + "...";
        ImGui::Text("OpenGL: %s", versionStr.c_str());
    } else {
        ImGui::Text("OpenGL: %s", glVersion ? glVersion : "Unknown");
    }

    ImGui::Spacing();
    ImGui::Separator();

    ImGui::Spacing();
    if (ImGui::Button("Quit Application", ImVec2(-1, 0)))
    {
        if (window)
            glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    ImGui::PopItemWidth();
    ImGui::End();
}

void OpenGLApp::Cleanup()
{
    if (imguiInitialized)
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        imguiInitialized = false;
    }

    renderer.reset();
    va.reset();
    vb.reset();
    ib.reset();
    shader.reset();
    texture.reset();

    cube.reset();
    cubeShader.reset();

    if (window)
    {
        glfwDestroyWindow(window);
        window = nullptr;
    }

    if (glfwInitialized)
    {
        glfwTerminate();
        glfwInitialized = false;
    }
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
