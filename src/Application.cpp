#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996) // Disable deprecation warnings for MSVC
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

// GLFW error callback to surface startup errors
static void GLFWErrorCallback(int error, const char* description)
{
    std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
}

// Framebuffer resize callback to update viewport
static void FramebufferSizeCallback(GLFWwindow* /*window*/, int width, int height)
{
    glViewport(0, 0, width, height);
}

/**
 * @brief Constructs the OpenGLApp and initializes member variables.
 */
OpenGLApp::OpenGLApp()
{
    translationA = glm::vec3(-400.0f, 0.0f, 0.0f);
    translationB = glm::vec3(400.0f, 0.0f, 0.0f);
    colorValue = 0.0f;
    colorSpeed = 0.25f;
    colorDirection = 1.0f;
    lastFrameTime = 0.0;
    projection = glm::mat4(1.0f); // Identity matrix
    view = glm::mat4(1.0f);       // Identity matrix
    projection3D = glm::mat4(1.0f);
    view3D = glm::mat4(1.0f);
    cubeRotationX = 0.0f;
    cubeRotationY = 0.0f;
}

/**
 * @brief Destructor. Cleans up resources.
 */
OpenGLApp::~OpenGLApp()
{
    Cleanup();
}

/**
 * @brief Initializes the application (GLFW, OpenGL, ImGui, scene).
 * @return true if successful, false otherwise.
 */
bool OpenGLApp::Initialize()
{
    if (!InitializeGLFW()) return false;
    if (!InitializeOpenGL()) return false;
    if (!InitializeImGui()) return false;
    if (!SetupScene()) return false;

    // Initialize timing
    lastFrameTime = glfwGetTime();

    return true;
}

/**
 * @brief Main loop. Updates and renders until window is closed.
 */
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

/**
 * @brief Initializes GLFW and creates the window/context.
 * @return true if successful, false otherwise.
 */
bool OpenGLApp::InitializeGLFW()
{
    // Register error callback early so we see failures
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
    glfwSwapInterval(1); // VSync enabled

    // Set initial viewport size and register framebuffer resize callback
    int fbw, fbh;
    glfwGetFramebufferSize(window, &fbw, &fbh);
    glViewport(0, 0, fbw, fbh);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

    glfwInitialized = true;
    return true;
}

/**
 * @brief Initializes GLEW and OpenGL state.
 * @return true if successful, false otherwise.
 */
bool OpenGLApp::InitializeOpenGL()
{
    // Required for core profile to get proper function pointers
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return false;
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Enable depth testing for 3D rendering
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    openglInitialized = true;
    return true;
}

/**
 * @brief Initializes ImGui context and sets up bindings.
 * @return true if successful, false otherwise.
 */
bool OpenGLApp::InitializeImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    // GLSL version string for ImGui OpenGL backend
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

/**
 * @brief Sets up the scene: geometry, buffers, shader, texture, renderer.
 * @return true if successful, false otherwise.
 */
bool OpenGLApp::SetupScene()
{
    // Vertex positions and texture coordinates for a quad
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
        texture = std::make_unique<Texture>("res/textures/myimage.png");
        renderer = std::make_unique<Renderer>();
        
        // Initialize 3D cube resources
        cube = std::make_unique<Cube>(1.0f);
        cubeShader = std::make_unique<Shader>("res/shaders/Cube.shader");
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to setup scene: " << e.what() << std::endl;
        return false;
    }

    // Set up projection and view matrices
    projection = glm::ortho(0.0f, static_cast<float>(WINDOW_WIDTH),
        0.0f, static_cast<float>(WINDOW_HEIGHT),
        -1.0f, 1.0f);
    view = glm::mat4(1.0f);
    
    // Set up 3D projection and view matrices
    projection3D = glm::perspective(glm::radians(45.0f), 
        static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT),
        0.1f, 100.0f);
    view3D = glm::lookAt(glm::vec3(3.0f, 3.0f, 3.0f),  // Camera position
                         glm::vec3(0.0f, 0.0f, 0.0f),   // Look at origin
                         glm::vec3(0.0f, 1.0f, 0.0f));  // Up vector

    if (shader) shader->Bind();
    if (texture) texture->Bind();
    if (shader) shader->SetUniform1i("u_Texture", 0);

    sceneSetup = true;
    return true;
}

/**
 * @brief Updates application state (color animation).
 */
void OpenGLApp::Update()
{
    if (!glfwInitialized)
        return;

    double currentTime = glfwGetTime();
    deltaTime = static_cast<float>(currentTime - lastFrameTime);
    lastFrameTime = currentTime;

    // Animate colorValue between 0.75 and 1.0 in a frame-rate independent way
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
    
    // Update cube rotation if cube is visible
    if (showCube)
    {
        cubeRotationX += cubeRotationSpeed * deltaTime;
        cubeRotationY += cubeRotationSpeed * 0.7f * deltaTime; // Slightly different speed for Y
        
        // Keep rotations within 0-360 degrees for cleaner values
        if (cubeRotationX >= 360.0f) cubeRotationX -= 360.0f;
        if (cubeRotationY >= 360.0f) cubeRotationY -= 360.0f;
    }
}

/**
 * @brief Renders the scene and UI.
 */
void OpenGLApp::Render()
{
    if (!renderer) return; // ensure resources exist
    renderer->Clear();

    // Start ImGui frame if initialized
    if (imguiInitialized)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    // Render cube first (3D content with depth testing)
    if (showCube && cube && cubeShader && renderer)
    {
        RenderCube();
    }
    
    // Render 2D quads with depth testing disabled
    if (showQuads && va && ib && shader && renderer)
    {
        // Temporarily disable depth testing for 2D content
        glDisable(GL_DEPTH_TEST);
        
        shader->Bind();
        shader->SetUniform4f("u_Color", colorValue, 1.0f, 1.0f, 1.0f);
        
        RenderQuad(translationA);
        RenderQuad(translationB);
        
        // Re-enable depth testing
        glEnable(GL_DEPTH_TEST);
    }

    if (imguiInitialized)
    {
        RenderUI();

        // Render ImGui UI
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}

/**
 * @brief Renders a quad at the given translation.
 * @param translation The translation vector for the quad.
 */
void OpenGLApp::RenderQuad(const glm::vec3& translation)
{
    if (!shader || !renderer || !va || !ib) return;

    glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
    glm::mat4 mvp = projection * view * model;
    shader->SetUniformMat4f("u_MVP", mvp);

    renderer->Draw(*va, *ib, *shader);
}

/**
 * @brief Renders the rotating 3D cube.
 * 
 * Sets up model matrix with rotation, binds cube shader with
 * appropriate uniforms for lighting, and renders the cube.
 */
void OpenGLApp::RenderCube()
{
    if (!cube || !cubeShader || !renderer) return;
    
    cubeShader->Bind();
    
    // Create model matrix with rotation
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(cubeRotationX), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(cubeRotationY), glm::vec3(0.0f, 1.0f, 0.0f));
    
    // Set shader uniforms
    glm::mat4 mvp = projection3D * view3D * model;
    cubeShader->SetUniformMat4f("u_MVP", mvp);
    cubeShader->SetUniformMat4f("u_Model", model);
    cubeShader->SetUniform3f("u_Color", 0.8f, 0.6f, 0.2f); // Orange-ish color
    cubeShader->SetUniform3f("u_LightPos", 2.0f, 2.0f, 2.0f);
    cubeShader->SetUniform3f("u_ViewPos", 3.0f, 3.0f, 3.0f);
    cubeShader->SetUniformBool("u_UseTexture", cubeUseTexture);
    
    // Bind texture if using texture mode
    if (cubeUseTexture && texture) {
        texture->Bind();
        cubeShader->SetUniform1i("u_Texture", 0);
    }
    
    cube->Render(*renderer, *cubeShader, model, view3D, projection3D);
}

/**
 * @brief Renders the ImGui UI controls.
 */
void OpenGLApp::RenderUI()
{
    if (!imguiInitialized) return;

    // Set fixed window size and position - increased width to prevent text cropping
    ImGui::SetNextWindowSize(ImVec2(380, 520), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    
    ImGui::Begin("OpenGL Renderer Controls", nullptr, 
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    
    // Set column width to ensure proper text layout
    ImGui::PushItemWidth(-120.0f); // Leave 120px for labels

    // === SCENE OBJECTS ===
    ImGui::SeparatorText("Scene Objects");
    ImGui::Checkbox("Show 2D Quads", &showQuads);
    ImGui::Checkbox("Show 3D Cube", &showCube);
    
    ImGui::Spacing();

    // === 2D QUAD SETTINGS ===
    if (showQuads)
    {
        ImGui::SeparatorText("2D Quad Settings");
        ImGui::SliderFloat2("Quad 1 Pos", &translationA.x, -800.0f, 800.0f);
        ImGui::SliderFloat2("Quad 2 Pos", &translationB.x, -800.0f, 800.0f);
        ImGui::Spacing();
    }
    
    // === 3D CUBE SETTINGS ===
    if (showCube)
    {
        ImGui::SeparatorText("3D Cube Settings");
        ImGui::SliderFloat("Rotation Speed", &cubeRotationSpeed, 0.0f, 180.0f, "%.0f°/sec");
        ImGui::Checkbox("Use Texture", &cubeUseTexture);
        ImGui::Text("Rotation: X=%.0f° Y=%.0f°", cubeRotationX, cubeRotationY);
        ImGui::Spacing();
    }

    // === PERFORMANCE INFO ===
    ImGui::SeparatorText("Performance");
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("FPS: %.1f (%.2fms/frame)", io.Framerate, 1000.0f / io.Framerate);
    
    // Get OpenGL version and truncate if too long (safe C++ version)
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
    
    // === APPLICATION CONTROLS ===
    ImGui::Spacing();
    if (ImGui::Button("Quit Application", ImVec2(-1, 0)))
    {
        if (window)
            glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    ImGui::PopItemWidth(); // Restore default item width
    ImGui::End();
}

/**
 * @brief Cleans up resources and shuts down ImGui and GLFW.
 */
void OpenGLApp::Cleanup()
{
    // Shutdown ImGui if it was initialized
    if (imguiInitialized)
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        imguiInitialized = false;
    }

    // Reset owned resources
    renderer.reset();
    va.reset();
    vb.reset();
    ib.reset();
    shader.reset();
    texture.reset();
    
    // Reset 3D cube resources
    cube.reset();
    cubeShader.reset();
    
    sceneSetup = false;

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
#pragma warning(pop) // Restore warning settings
#endif
