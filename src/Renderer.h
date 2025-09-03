#pragma once

#include <GL/glew.h>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

#ifdef _WIN32
    #define ASSERT(x) if (!(x)) __debugbreak();
#else
    #include <signal.h>
    #define ASSERT(x) if (!(x)) raise(SIGTRAP);
#endif
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

class Renderer
{
private:

public:
	void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
	void Clear() const;
	
};