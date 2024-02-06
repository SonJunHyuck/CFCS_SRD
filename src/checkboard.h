#pragma once

#include "common.h"
#include "buffer.h"
#include "vertex_layout.h"
#include "program.h"

CLASS_PTR(CheckBoard);
class CheckBoard
{
private:
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 color;
    };

public:
    static CheckBoardUPtr Create(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, uint32_t primitiveType);
    static CheckBoardUPtr CreateCheckBoard(const uint32_t inSize, const uint32_t inSquare);
    
    const VertexLayout* GetVertexLayout() const { return m_vertexLayout.get(); }
    BufferPtr GetVertexBuffer() const { return m_vertexBuffer; }
    BufferPtr GetIndexBuffer() const { return m_indexBuffer; }

    void Draw(const Program *program) const;

private:
    CheckBoard() {}
    void Init(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, uint32_t primitiveType);

    uint32_t m_primitiveType{ GL_TRIANGLES };
    VertexLayoutUPtr m_vertexLayout;

    BufferPtr m_vertexBuffer;
    BufferPtr m_indexBuffer;
};