#include "line.h"

LineUPtr Line::Create()
{
    auto line = LineUPtr(new Line());
    
    return std::move(line);
}

LineUPtr Line::Create(const std::vector<glm::vec3> &positions)
{
    auto line = LineUPtr(new Line());
    line->Init(positions);
    return std::move(line);
}

void Line::Update(const std::vector<glm::vec3> &positions)
{
    Init(positions);
    IsDrawable = true;
}

void Line::Draw(const Program *program) const
{
    m_vertexLayout->Bind();
    glDrawElements(m_primitiveType, m_indexBuffer->GetCount(), GL_UNSIGNED_INT, 0);
}

void Line::Init(const std::vector<glm::vec3> &positions)
{   
    std::vector<uint32_t> indices;

    for(uint32_t i = 0; i < positions.size(); i++)
    {
        indices.push_back(i);
    }

    m_vertexLayout.reset();
    m_vertexLayout = VertexLayout::Create();
    m_vertexBuffer.reset();
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, positions.data(), sizeof(glm::vec3), positions.size());
    m_indexBuffer.reset();
    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices.data(), sizeof(uint32_t), indices.size());
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, false, sizeof(glm::vec3), 0); // pos
}