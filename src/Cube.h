#pragma once

#include <memory>
#include <glm/glm.hpp>

class VertexArray;
class VertexBuffer;
class IndexBuffer;
class Shader;
class Renderer;

class Cube {
public:
    explicit Cube(float size = 2.0f);
    ~Cube() = default;

    Cube(const Cube&) = delete;
    Cube& operator=(const Cube&) = delete;
    Cube(Cube&&) = default;
    Cube& operator=(Cube&&) = default;

    void Render(const Renderer& renderer, const Shader& shader,
                const glm::mat4& model, const glm::mat4& view,
                const glm::mat4& projection) const;

private:
    std::unique_ptr<VertexArray> m_vertexArray;
    std::unique_ptr<VertexBuffer> m_vertexBuffer;
    std::unique_ptr<IndexBuffer> m_indexBuffer;
};
