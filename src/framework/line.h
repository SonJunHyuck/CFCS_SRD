#pragma once

#include "../common.h"
#include "buffer.h"
#include "vertex_layout.h"
#include "program.h"

CLASS_PTR(Line)
class Line
{
public:
    static LineUPtr Create();
    static LineUPtr Create(const std::vector<glm::vec3> &positions);
    
    const VertexLayout* GetVertexLayout() const { return m_vertexLayout.get(); }
    BufferPtr GetVertexBuffer() const { return m_vertexBuffer; }
    BufferPtr GetIndexBuffer() const { return m_indexBuffer; }

    void Update(const std::vector<glm::vec3> &positions);
    void Draw(const Program *program) const;

    bool IsDrawable { false };

private:
    Line() {}
    void Init(const std::vector<glm::vec3> &positions);

    uint32_t m_primitiveType{ GL_LINE_STRIP };
    VertexLayoutUPtr m_vertexLayout;

    BufferPtr m_vertexBuffer;
    BufferPtr m_indexBuffer;
};