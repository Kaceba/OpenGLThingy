#pragma once

#include <memory>
#include <glm/glm.hpp>

class VertexArray;
class VertexBuffer;
class IndexBuffer;
class Shader;
class Renderer;

/**
 * @brief A 3D cube primitive with RAII resource management.
 * 
 * This class encapsulates the geometry, buffers, and rendering logic
 * for a 3D cube. It follows RAII principles with automatic resource
 * cleanup and provides a simple interface for rendering rotating cubes.
 */
class Cube {
public:
    /**
     * @brief Constructs a cube and initializes OpenGL resources.
     * @param size The side length of the cube (default: 2.0f)
     */
    explicit Cube(float size = 2.0f);
    
    /**
     * @brief Destructor automatically cleans up resources via RAII.
     */
    ~Cube() = default;
    
    // Non-copyable for simplicity, but moveable
    Cube(const Cube&) = delete;
    Cube& operator=(const Cube&) = delete;
    Cube(Cube&&) = default;
    Cube& operator=(Cube&&) = default;
    
    /**
     * @brief Renders the cube with the given transformation matrices.
     * @param renderer The renderer to use for drawing
     * @param shader The shader to use for rendering
     * @param model The model transformation matrix
     * @param view The view matrix
     * @param projection The projection matrix
     */
    void Render(const Renderer& renderer, const Shader& shader,
                const glm::mat4& model, const glm::mat4& view, 
                const glm::mat4& projection) const;
    
    /**
     * @brief Gets the vertex array object for direct access if needed.
     * @return Reference to the vertex array
     */
    const VertexArray& GetVertexArray() const;
    
    /**
     * @brief Gets the index buffer for direct access if needed.
     * @return Reference to the index buffer
     */
    const IndexBuffer& GetIndexBuffer() const;

private:
    void GenerateGeometry(float size);
    void SetupBuffers();
    
    std::unique_ptr<VertexArray> m_vertexArray;
    std::unique_ptr<VertexBuffer> m_vertexBuffer;
    std::unique_ptr<IndexBuffer> m_indexBuffer;
    
    // Cube geometry data
    static constexpr unsigned int VERTICES_COUNT = 24; // 4 vertices per face * 6 faces
    static constexpr unsigned int INDICES_COUNT = 36;  // 2 triangles per face * 3 vertices * 6 faces
    
    float m_vertices[VERTICES_COUNT * 8]; // position(3) + normal(3) + texCoord(2) per vertex
    unsigned int m_indices[INDICES_COUNT];
};