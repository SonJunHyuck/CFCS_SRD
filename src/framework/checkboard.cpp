#include "checkboard.h"

CheckBoardUPtr CheckBoard::Create(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, uint32_t primitiveType)
{
    auto checkboard = CheckBoardUPtr(new CheckBoard());
    checkboard->Init(vertices, indices, primitiveType);
    return std::move(checkboard);
}

CheckBoardUPtr CheckBoard::CreateCheckBoard(const uint32_t inSize, const uint32_t inSquare)
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    glm::vec3 color = VEC_ONE * 1.0f;
    glm::vec3 normal = glm::vec3(0, 1, 0);

	float maxX = inSize * 0.5f, maxY = inSize * 0.5f;
	float minX = inSize * -0.5f, minY = inSize * -0.5f;

    int x, y, i;
	float xp, yp, xd, yd;
	xd = (maxX - minX) / ((float)inSquare);
	yd = (maxY - minY) / ((float)inSquare);

    for (x = 0, xp = minX; x < inSquare; x++, xp += xd)
    {
        for (y = 0, yp = minY, i = x; y < inSquare; y++, yp += yd)
        {
            Vertex vertex1 { glm::vec3(xp, -1.0f, yp), normal, color };  // Left Bottom
            Vertex vertex2 { glm::vec3(xp, -1.0f, yp + yd), normal, color };  // Left Top
            Vertex vertex3 { glm::vec3(xp + xd, -1.0f, yp + yd), normal, color };  // Right Top
            Vertex vertex4 { glm::vec3(xp + xd, -1.0f, yp), normal, color };  // Right Bottom
            //SPDLOG_INFO("{} {}", minX, maxX);

            vertices.push_back(vertex1);
            vertices.push_back(vertex2);
            vertices.push_back(vertex3);
            vertices.push_back(vertex4);
            
			indices.push_back(0 + 4 * y + x * inSquare * 4);
			indices.push_back(1 + 4 * y + x * inSquare * 4);
            
			indices.push_back(1 + 4 * y + x * inSquare * 4);
			indices.push_back(2 + 4 * y + x * inSquare * 4);
            
			indices.push_back(2 + 4 * y + x * inSquare * 4);
			indices.push_back(3 + 4 * y + x * inSquare * 4);
            
			indices.push_back(3 + 4 * y + x * inSquare * 4);
			indices.push_back(0 + 4 * y + x * inSquare * 4);
        }
    }

    return Create(vertices, indices, GL_LINES);
}

void CheckBoard::Draw(const Program *program) const
{
    m_vertexLayout->Bind();
    glDrawElements(m_primitiveType, m_indexBuffer->GetCount(), GL_UNSIGNED_INT, 0);
}

void CheckBoard::Init(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, uint32_t primitiveType)
{   
    m_primitiveType = primitiveType;
    m_vertexLayout = VertexLayout::Create();
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices.data(), sizeof(Vertex), vertices.size());
    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices.data(), sizeof(uint32_t), indices.size());
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, false, sizeof(Vertex), 0); // pos
    m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, false, sizeof(Vertex), offsetof(Vertex, normal));
    m_vertexLayout->SetAttrib(2, 3, GL_FLOAT, false, sizeof(Vertex), offsetof(Vertex, color));
}