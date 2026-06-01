#include "Cube.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Renderer.h"

#include <glm/gtc/matrix_transform.hpp>

Cube::Cube(float size) {
    const float h = size * 0.5f;

    // Per vertex: x, y, z, nx, ny, nz, u, v
    const float vertices[] = {
        // Front (z = +h)
        -h, -h,  h,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         h, -h,  h,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         h,  h,  h,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -h,  h,  h,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        // Back (z = -h)
         h, -h, -h,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        -h, -h, -h,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        -h,  h, -h,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         h,  h, -h,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        // Left (x = -h)
        -h, -h, -h, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -h, -h,  h, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -h,  h,  h, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -h,  h, -h, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        // Right (x = +h)
         h, -h,  h,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         h, -h, -h,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         h,  h, -h,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         h,  h,  h,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        // Bottom (y = -h)
        -h, -h, -h,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
         h, -h, -h,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         h, -h,  h,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        -h, -h,  h,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        // Top (y = +h)
        -h,  h,  h,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
         h,  h,  h,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         h,  h, -h,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        -h,  h, -h,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    const unsigned int indices[] = {
        // Front
        0,  1,  2,   2,  3,  0,
        // Back
        4,  5,  6,   6,  7,  4,
        // Left
        8,  9,  10,  10, 11, 8,
        // Right
        12, 13, 14,  14, 15, 12,
        // Bottom
        16, 17, 18,  18, 19, 16,
        // Top
        20, 21, 22,  22, 23, 20
    };

    m_vertexArray = std::make_unique<VertexArray>();
    m_vertexBuffer = std::make_unique<VertexBuffer>(vertices, sizeof(vertices));

    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(3);
    layout.Push<float>(2);
    m_vertexArray->AddBuffer(*m_vertexBuffer, layout);

    m_indexBuffer = std::make_unique<IndexBuffer>(indices, sizeof(indices) / sizeof(indices[0]));
}

void Cube::Render(const Renderer& renderer, const Shader& shader,
                  const glm::mat4& model, const glm::mat4& view,
                  const glm::mat4& projection) const {
    glm::mat4 mvp = projection * view * model;
    shader.SetUniformMat4f("u_MVP", mvp);
    renderer.Draw(*m_vertexArray, *m_indexBuffer, shader);
}
