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

	m_LastFrameTime = glfwGetTime();

	return true;
}

void OpenGLApp::Run()
{
	while (m_Window && !glfwWindowShouldClose(m_Window))
	{
		Update();
		Render();

		glfwSwapBuffers(m_Window);
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

	m_Window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL Application", nullptr, nullptr);
	if (!m_Window)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(m_Window);
	glfwSwapInterval(1); // VSync

	int fbw, fbh;
	glfwGetFramebufferSize(m_Window, &fbw, &fbh);
	glViewport(0, 0, fbw, fbh);
	glfwSetFramebufferSizeCallback(m_Window, FramebufferSizeCallback);

	m_GlfwInitialized = true;
	return true;
}

bool OpenGLApp::InitializeOpenGL()
{
	// Required for core profile to expose modern GL function pointers
	glewExperimental = GL_TRUE;
	if (GLenum err = glewInit(); err != GLEW_OK && err != GLEW_ERROR_NO_GLX_DISPLAY)
	{
		// GLEW_ERROR_NO_GLX_DISPLAY is harmless on Wayland/EGL; GL function pointers still load fine.
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
	if (!ImGui_ImplGlfw_InitForOpenGL(m_Window, true) ||
		!ImGui_ImplOpenGL3_Init(glsl_version))
	{
		std::cerr << "Failed to initialize ImGui" << std::endl;
		return false;
	}

	m_ImguiInitialized = true;
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
		m_VertexArray = std::make_unique<VertexArray>();
		m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));

		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		m_VertexArray->AddBuffer(*m_VertexBuffer, layout);

		m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);

		m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
		m_Texture = std::make_unique<Texture>("res/textures/myimage.png");
		m_Renderer = std::make_unique<Renderer>();

		m_Cube = std::make_unique<Cube>(1.0f);
		m_CubeShader = std::make_unique<Shader>("res/shaders/Cube.shader");
	}
	catch (const std::exception& e)
	{
		std::cerr << "Failed to setup scene: " << e.what() << std::endl;
		return false;
	}

	m_Projection = glm::ortho(0.0f, static_cast<float>(WINDOW_WIDTH),
		0.0f, static_cast<float>(WINDOW_HEIGHT),
		-1.0f, 1.0f);

	m_Projection3D = glm::perspective(glm::radians(45.0f),
		static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT),
		0.1f, 100.0f);
	m_View3D = glm::lookAt(glm::vec3(3.0f, 3.0f, 3.0f),
						   glm::vec3(0.0f, 0.0f, 0.0f),
						   glm::vec3(0.0f, 1.0f, 0.0f));

	m_Shader->Bind();
	m_Texture->Bind();
	m_Shader->SetUniform1i("u_Texture", 0);

	return true;
}

void OpenGLApp::Update()
{
	double currentTime = glfwGetTime();
	float deltaTime = static_cast<float>(currentTime - m_LastFrameTime);
	m_LastFrameTime = currentTime;

	const float minVal = 0.75f;
	const float maxVal = 1.0f;

	m_ColorValue += m_ColorDirection * m_ColorSpeed * deltaTime;
	if (m_ColorValue > maxVal)
	{
		m_ColorValue = maxVal;
		m_ColorDirection = -1.0f;
	}
	else if (m_ColorValue < minVal)
	{
		m_ColorValue = minVal;
		m_ColorDirection = 1.0f;
	}

	if (m_ShowCube)
	{
		m_CubeRotationX += m_CubeRotationSpeed * deltaTime;
		m_CubeRotationY += m_CubeRotationSpeed * 0.7f * deltaTime;

		if (m_CubeRotationX >= 360.0f) m_CubeRotationX -= 360.0f;
		if (m_CubeRotationY >= 360.0f) m_CubeRotationY -= 360.0f;
	}
}

void OpenGLApp::Render()
{
	m_Renderer->Clear();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if (m_ShowCube)
	{
		RenderCube();
	}

	if (m_ShowQuads)
	{
		glDisable(GL_DEPTH_TEST);

		m_Shader->Bind();
		m_Shader->SetUniform4f("u_Color", m_ColorValue, 1.0f, 1.0f, 1.0f);

		RenderQuad(m_TranslationA);
		RenderQuad(m_TranslationB);

		glEnable(GL_DEPTH_TEST);
	}

	RenderUI();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void OpenGLApp::RenderQuad(const glm::vec3& translation)
{
	glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
	glm::mat4 mvp = m_Projection * model;
	m_Shader->SetUniformMat4f("u_MVP", mvp);

	m_Renderer->Draw(*m_VertexArray, *m_IndexBuffer, *m_Shader);
}

void OpenGLApp::RenderCube()
{
	m_CubeShader->Bind();

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(m_CubeRotationX), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(m_CubeRotationY), glm::vec3(0.0f, 1.0f, 0.0f));

	m_CubeShader->SetUniformMat4f("u_Model", model);
	m_CubeShader->SetUniform3f("u_Color", 0.8f, 0.6f, 0.2f);
	m_CubeShader->SetUniform3f("u_LightPos", 2.0f, 2.0f, 2.0f);
	m_CubeShader->SetUniform3f("u_ViewPos", 3.0f, 3.0f, 3.0f);
	m_CubeShader->SetUniformBool("u_UseTexture", m_CubeUseTexture);

	if (m_CubeUseTexture)
	{
		m_Texture->Bind();
		m_CubeShader->SetUniform1i("u_Texture", 0);
	}

	m_Cube->Render(*m_Renderer, *m_CubeShader, model, m_View3D, m_Projection3D);
}

void OpenGLApp::RenderUI()
{
	ImGui::SetNextWindowSize(ImVec2(380, 520), ImGuiCond_Always);
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);

	ImGui::Begin("OpenGL Renderer Controls", nullptr,
				 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

	ImGui::PushItemWidth(-120.0f); // leave 120px for labels

	ImGui::SeparatorText("Scene Objects");
	ImGui::Checkbox("Show 2D Quads", &m_ShowQuads);
	ImGui::Checkbox("Show 3D Cube", &m_ShowCube);

	ImGui::Spacing();

	if (m_ShowQuads)
	{
		ImGui::SeparatorText("2D Quad Settings");
		ImGui::SliderFloat2("Quad 1 Pos", &m_TranslationA.x, -800.0f, 800.0f);
		ImGui::SliderFloat2("Quad 2 Pos", &m_TranslationB.x, -800.0f, 800.0f);
		ImGui::Spacing();
	}

	if (m_ShowCube)
	{
		ImGui::SeparatorText("3D Cube Settings");
		ImGui::SliderFloat("Rotation Speed", &m_CubeRotationSpeed, 0.0f, 180.0f, "%.0f°/sec");
		ImGui::Checkbox("Use Texture", &m_CubeUseTexture);
		ImGui::Text("Rotation: X=%.0f° Y=%.0f°", m_CubeRotationX, m_CubeRotationY);
		ImGui::Spacing();
	}

	ImGui::SeparatorText("Performance");
	ImGuiIO& io = ImGui::GetIO();
	ImGui::Text("FPS: %.1f (%.2fms/frame)", io.Framerate, 1000.0f / io.Framerate);

	const char* glVersion = (const char*)glGetString(GL_VERSION);
	if (glVersion && strlen(glVersion) > 30)
	{
		std::string versionStr(glVersion);
		versionStr = versionStr.substr(0, 30) + "...";
		ImGui::Text("OpenGL: %s", versionStr.c_str());
	}
	else
	{
		ImGui::Text("OpenGL: %s", glVersion ? glVersion : "Unknown");
	}

	ImGui::Spacing();
	ImGui::Separator();

	ImGui::Spacing();
	if (ImGui::Button("Quit Application", ImVec2(-1, 0)))
	{
		if (m_Window)
			glfwSetWindowShouldClose(m_Window, GLFW_TRUE);
	}

	ImGui::PopItemWidth();
	ImGui::End();
}

void OpenGLApp::Cleanup()
{
	if (m_ImguiInitialized)
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		m_ImguiInitialized = false;
	}

	m_Renderer.reset();
	m_VertexArray.reset();
	m_VertexBuffer.reset();
	m_IndexBuffer.reset();
	m_Shader.reset();
	m_Texture.reset();

	m_Cube.reset();
	m_CubeShader.reset();

	if (m_Window)
	{
		glfwDestroyWindow(m_Window);
		m_Window = nullptr;
	}

	if (m_GlfwInitialized)
	{
		glfwTerminate();
		m_GlfwInitialized = false;
	}
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
