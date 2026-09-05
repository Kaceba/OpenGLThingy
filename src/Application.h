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

	GLFWwindow* m_Window = nullptr;
	std::unique_ptr<Renderer> m_Renderer;
	std::unique_ptr<VertexArray> m_VA;
	std::unique_ptr<VertexBuffer> m_VB;
	std::unique_ptr<IndexBuffer> m_IB;
	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<Texture> m_Texture;

	std::unique_ptr<Cube> m_Cube;
	std::unique_ptr<Shader> m_CubeShader;

	float m_ColorValue = 0.0f;
	float m_ColorSpeed = 0.25f;
	float m_ColorDirection = 1.0f;
	glm::vec3 m_TranslationA{-400.0f, 0.0f, 0.0f};
	glm::vec3 m_TranslationB{ 400.0f, 0.0f, 0.0f};
	glm::mat4 m_Projection{1.0f};
	glm::mat4 m_View{1.0f};

	glm::mat4 m_Projection3D{1.0f};
	glm::mat4 m_View3D{1.0f};

	bool m_ShowQuads = true;
	bool m_ShowCube = false;
	bool m_CubeUseTexture = false;

	float m_CubeRotationX = 0.0f;
	float m_CubeRotationY = 0.0f;
	float m_CubeRotationSpeed = 45.0f; // degrees per second

	bool m_GlfwInitialized = false;
	bool m_ImguiInitialized = false;

	double m_LastFrameTime = 0.0;
	float m_DeltaTime = 0.0f;
};
