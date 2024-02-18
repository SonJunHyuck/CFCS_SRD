#pragma once

#include "../common.h"
#include "buffer.h"
#include "vertex_layout.h"
#include "program.h"

struct ParticleAttribute
{
    float index;
    glm::vec2 position;
};

CLASS_PTR(Particle);
class Particle
{
public:
    static ParticleUPtr Create(const uint32_t inParticleNum, uint32_t primitiveType);

    const VertexLayout *GetVertexLayout() const { return m_vertexLayout.get(); }
    const uint32_t GetParticleNum() const { return m_particleNum; }
    
    void SetParticleNum(uint32_t inParticleNum) { m_particleNum = inParticleNum; }

    void Draw(const Program* program) const;

    uint32_t m_particleMAX { 1024 * 32 };
private:
    Particle() {}
    void Init(const uint32_t inParticleNum, uint32_t primitiveType);

    uint32_t m_primitiveType{GL_POINTS};
    VertexLayoutUPtr m_vertexLayout;
    
    BufferPtr m_particleIndexBuffer;
    BufferPtr m_particlePositionBuffer;

    uint32_t m_particleNum { 1024 * 32 };
};