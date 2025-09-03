#include "Cube.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Renderer.h"

#include <glm/gtc/matrix_transform.hpp>

/**
 * @brief Constructs a cube with the specified size.
 * 
 * Generates cube geometry, creates OpenGL buffers, and sets up
 * vertex array object with proper vertex layout.
 * 
 * @param size Side length of the cube
 */
Cube::Cube(float size) {
    GenerateGeometry(size);
    SetupBuffers();
}

/**
 * @brief Renders the cube using the provided rendering components.
 * 
 * Binds necessary uniforms to the shader and issues the draw call.
 * The shader is expected to have u_MVP uniform for the combined
 * model-view-projection matrix.
 * 
 * @param renderer The renderer instance
 * @param shader The shader program to use
 * @param model Model transformation matrix
 * @param view View transformation matrix  
 * @param projection Projection transformation matrix
 */
void Cube::Render(const Renderer& renderer, const Shader& shader,
                  const glm::mat4& model, const glm::mat4& view, 
                  const glm::mat4& projection) const {
    if (!m_vertexArray || !m_indexBuffer) {
        return; // Safety check
    }
    
    // Calculate MVP matrix
    glm::mat4 mvp = projection * view * model;
    
    // Set shader uniforms
    shader.SetUniformMat4f("u_MVP", mvp);
    
    // Render the cube
    renderer.Draw(*m_vertexArray, *m_indexBuffer, shader);
}

/**
 * @brief Gets the vertex array for external access.
 * @return Reference to the vertex array object
 */
const VertexArray& Cube::GetVertexArray() const {
    return *m_vertexArray;
}

/**
 * @brief Gets the index buffer for external access.  
 * @return Reference to the index buffer object
 */
const IndexBuffer& Cube::GetIndexBuffer() const {
    return *m_indexBuffer;
}

/**
 * @brief Generates cube vertex and index data.
 * 
 * Creates a unit cube centered at origin with positions and normals.
 * Each face has 4 vertices to allow proper per-face normals.
 * 
 * @param size Side length of the cube
 */
void Cube::GenerateGeometry(float size) {
    const float halfSize = size * 0.5f;
    
    // Cube vertices with positions, normals, and texture coordinates
    // Format: x, y, z, nx, ny, nz, u, v (8 floats per vertex)
    float vertices[] = {
        // Front face (z = +halfSize)
        -halfSize, -halfSize,  halfSize,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         halfSize, -halfSize,  halfSize,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         halfSize,  halfSize,  halfSize,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -halfSize,  halfSize,  halfSize,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        
        // Back face (z = -halfSize)
         halfSize, -halfSize, -halfSize,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        -halfSize, -halfSize, -halfSize,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        -halfSize,  halfSize, -halfSize,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         halfSize,  halfSize, -halfSize,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        
        // Left face (x = -halfSize)
        -halfSize, -halfSize, -halfSize, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -halfSize, -halfSize,  halfSize, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -halfSize,  halfSize,  halfSize, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -halfSize,  halfSize, -halfSize, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        
        // Right face (x = +halfSize)
         halfSize, -halfSize,  halfSize,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         halfSize, -halfSize, -halfSize,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         halfSize,  halfSize, -halfSize,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         halfSize,  halfSize,  halfSize,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        
        // Bottom face (y = -halfSize)
        -halfSize, -halfSize, -halfSize,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
         halfSize, -halfSize, -halfSize,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         halfSize, -halfSize,  halfSize,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        -halfSize, -halfSize,  halfSize,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        
        // Top face (y = +halfSize)
        -halfSize,  halfSize,  halfSize,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
         halfSize,  halfSize,  halfSize,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         halfSize,  halfSize, -halfSize,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        -halfSize,  halfSize, -halfSize,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };
    
    // Copy vertices to member array
    for (int i = 0; i < VERTICES_COUNT * 8; ++i) {
        m_vertices[i] = vertices[i];
    }
    
    // Cube indices (2 triangles per face)
    unsigned int indices[] = {
        // Front face
        0,  1,  2,   2,  3,  0,
        // Back face  
        4,  5,  6,   6,  7,  4,
        // Left face
        8,  9,  10,  10, 11, 8,
        // Right face
        12, 13, 14,  14, 15, 12,
        // Bottom face
        16, 17, 18,  18, 19, 16,
        // Top face
        20, 21, 22,  22, 23, 20
    };
    
    // Copy indices to member array
    for (int i = 0; i < INDICES_COUNT; ++i) {
        m_indices[i] = indices[i];
    }
}

/**
 * @brief Sets up OpenGL buffers and vertex array object.
 * 
 * Creates vertex buffer, index buffer, and vertex array with proper
 * layout specification. Configures vertex attributes for position
 * and normal data.
 */
void Cube::SetupBuffers() {
    // Create vertex array object
    m_vertexArray = std::make_unique<VertexArray>();
    
    // Create and populate vertex buffer
    m_vertexBuffer = std::make_unique<VertexBuffer>(
        m_vertices, VERTICES_COUNT * 8 * sizeof(float));
    
    // Set up vertex buffer layout
    VertexBufferLayout layout;
    layout.Push<float>(3); // Position (x, y, z)
    layout.Push<float>(3); // Normal (nx, ny, nz)
    layout.Push<float>(2); // Texture coordinates (u, v)
    
    // Add buffer to vertex array with layout
    m_vertexArray->AddBuffer(*m_vertexBuffer, layout);
    
    // Create index buffer
    m_indexBuffer = std::make_unique<IndexBuffer>(m_indices, INDICES_COUNT);
}