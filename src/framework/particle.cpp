#include "particle.h"

ParticleUPtr Particle::Create(const uint32_t inParticleNum, uint32_t primitiveType)
{
    auto particle = ParticleUPtr(new Particle());
    particle->Init(inParticleNum, primitiveType);
    return std::move(particle);
}

void Particle::Draw(const Program* program) const
{
    m_vertexLayout->Bind();
    glDrawArrays(GL_POINTS, 0, m_particleNum);
}

void Particle::Init(const uint32_t inParticleNum, uint32_t primitiveType)
{
    std::vector<float> particleIndexData = std::vector<float>(inParticleNum);
    std::vector<glm::vec2> particlePositionData = std::vector<glm::vec2>(inParticleNum);

    for (int i = 0; i < inParticleNum; i++) 
    {
        particleIndexData[i] = (float)i;
        particlePositionData[i] = glm::vec2(0);
    }

    m_vertexLayout = VertexLayout::Create();

    m_particleIndexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, particleIndexData.data(), sizeof(float), inParticleNum);
    m_particleIndexBuffer->Bind();
    m_vertexLayout->SetAttrib(0, 1, GL_FLOAT, false, 0, 0);

    m_particlePositionBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, particlePositionData.data(), sizeof(glm::vec2), inParticleNum);
    m_particlePositionBuffer->Bind();
    m_vertexLayout->SetAttrib(1, 2, GL_FLOAT, false, 0, 0);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_particleIndexBuffer->Get());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_particlePositionBuffer->Get());
}