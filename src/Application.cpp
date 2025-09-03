#include "Application.h"

#include "Config.h"
#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Texture.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

// Constructor
OpenGLApp::OpenGLApp()
{
    translationA = glm::vec3(-400.0f, 0.0f, 0.0f);
    translationB = glm::vec3(400.0f, 0.0f, 0.0f);
    colorValue = 0.0f;
    colorIncrement = 0.05f;
}

// Destructor
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

    return true;
}

void OpenGLApp::Run()
{
    while (!glfwWindowShouldClose(window))
    {
        Update();
        Render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

bool OpenGLApp::InitializeGLFW()
{
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
    glfwSwapInterval(1); // VSync enabled
    return true;
}

bool OpenGLApp::InitializeOpenGL()
{
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return false;
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

bool OpenGLApp::InitializeImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    const char* glsl_version = "#version 330";
    if (!ImGui_ImplGlfw_InitForOpenGL(window, true) ||
        !ImGui_ImplOpenGL3_Init(glsl_version))
    {
        std::cerr << "Failed to initialize ImGui" << std::endl;
        return false;
    }

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
        layout.Push<float>(2); // position
        layout.Push<float>(2); // tex coords
        va->AddBuffer(*vb, layout);

        ib = std::make_unique<IndexBuffer>(indices, 6);

        shader = std::make_unique<Shader>("res/shaders/Basic.shader");
        texture = std::make_unique<Texture>("res/textures/eula.png");
        renderer = std::make_unique<Renderer>();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to setup scene: " << e.what() << std::endl;
        return false;
    }

    // Projection and view matrices
    projection = glm::ortho(0.0f, static_cast<float>(WINDOW_WIDTH),
        0.0f, static_cast<float>(WINDOW_HEIGHT),
        -1.0f, 1.0f);
    view = glm::mat4(1.0f);

    shader->Bind();
    texture->Bind();
    shader->SetUniform1i("u_Texture", 0);

    return true;
}

void OpenGLApp::Update()
{
    if (colorValue > 1.0f) colorIncrement = -0.001f;
    else if (colorValue < 0.75f) colorIncrement = 0.01f;

    colorValue += colorIncrement;
}

void OpenGLApp::Render()
{
    renderer->Clear();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    shader->Bind();
    shader->SetUniform4f("u_Color", colorValue, 1.0f, 1.0f, 1.0f);

    RenderQuad(translationA);
    RenderQuad(translationB);

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

void OpenGLApp::RenderUI()
{
    ImGui::Begin("Controls");

    ImGui::SliderFloat2("Translation 1", &translationA.x, -800.0f, 800.0f);
    ImGui::SliderFloat2("Translation 2", &translationB.x, -800.0f, 800.0f);

    ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
        1000.0f / io.Framerate, io.Framerate);

    ImGui::End();
}

void OpenGLApp::Cleanup()
{
    if (window)
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        glfwTerminate();
        window = nullptr;
    }
}
