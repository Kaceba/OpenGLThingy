#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h" 

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	const char* glsl_version = "#version 330";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(1600, 900, "OpenGLThingy", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)
	{
		//do something;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;

	/*const float vrt = 900.0f;
	const float hrz = vrt*1.7778;*/
	{
		//float positions[] = {
		//	((hrz / 2) / 2),				(((vrt / 2) / 2)*1.7777) -350.0f,				0.0f, 0.0f, // 0
		//	((hrz / 2) + ((hrz / 2) / 2)),	(((vrt / 2) / 2)*1.7777) -350.0f,				1.0f, 0.0f, // 1
		//	((hrz / 2) + ((hrz / 2) / 2)),	(((vrt / 2) + ((vrt / 2) / 2))*1.7777) -350.0f,	1.0f, 1.0f, // 2
		//	((hrz / 2) / 2),				(((vrt / 2) + ((vrt / 2) / 2))*1.7777) -350.0f,	0.0f, 1.0f, // 3
		//};

		float positions[] = {
			600.0f,		250.0f,		0.0f, 0.0f, // 0
			1000.0f,	250.0f,		1.0f, 0.0f, // 1
			1000.0f,	650.0f,		1.0f, 1.0f, // 2
			600.0f,		650.0f,		0.0f, 1.0f, // 3
		};

		unsigned int indices[]{
			0, 1, 2,
			2, 3, 0,
		};

		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		VertexArray va;
		VertexBuffer vb(positions, 4 * 4 * sizeof(float));

		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);

		IndexBuffer ib(indices, 6);

		Shader shader("res/shaders/Basic.shader");
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

		Texture texture("res/textures/eula.png");
		texture.Bind();
		shader.SetUniform1i("u_Texture", 0);

		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();

		Renderer renderer;

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);
		ImGui::StyleColorsDark();

		glm::vec3 translationA(-400, 0, 0);
		glm::vec3 translationB(400, 0, 0);

		glm::mat4 proj = glm::ortho(0.0f, 1600.0f, 0.0f, 900.0f, -1.0f, 1.0f);
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

		float r = 0.0f;
		float increment = 0.05f;
		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			renderer.Clear();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			{
				shader.Bind();
				shader.SetUniform4f("u_Color", r, 1.0f, 1.0f, 1.0f);
			}

			{
				glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
				glm::mat4 mvp = proj * view * model;
				shader.Bind();
				shader.SetUniformMat4f("u_MVP", mvp);
				renderer.Draw(va, ib, shader);
			}

			{
				glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
				glm::mat4 mvp = proj * view * model;
				shader.Bind();
				shader.SetUniformMat4f("u_MVP", mvp);
				renderer.Draw(va, ib, shader);
			}

			if (r > 1.00f)
				increment = -0.001f;
			else if (r < 0.75f)
				increment = 0.010f;

			r += increment;

			//ImGui window
			{
				ImGui::Begin("Disconocopticon");
				ImGui::SliderFloat2("Translation1", &translationA.x, -800.0f, 800.0f);
				ImGui::SliderFloat2("Translation2", &translationB.x, -800.0f, 800.0f);
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
				ImGui::End();
			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			/* Swap front and back buffers */
			GLCall(glfwSwapBuffers(window));

			/* Poll for and process events */
			GLCall(glfwPollEvents());
		}
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
	glfwTerminate();
	return 0;
}