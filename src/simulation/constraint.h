#pragma once

#include <vector>

#include "../common.h"

class Agent;

class Constraint
{
    public:

    std::vector<uint32_t> AgentIndices;
    glm::vec3 PositionDelta;
    uint8_t bIsActive;

    Constraint();

    virtual void Project(const std::vector<uint32_t>& Agents) = 0;
    virtual ~Constraint();
};

class ConstraintAvoidance : public Constraint
{
    const float K;
    const float Tao;
    const float MaValue;

    uint32_t HostAgentId;
    uint32_t GuestAgentId;

    float CollisionMargin;
    float Radius;
    float LimitAccel;

    ConstraintAvoidance(uint32_t InHostAgentId, uint32_t InGuestAgentId);

    virtual voiud Project(const std::vector<uint32_t>& Agents);
}

const float AVOIDANCE_STIFFNESS = 1.5f;
const float AVOIDANCE_TAO0 = 10.0f;
const float AVOIDANCE_LimitAccel = 0.2f;
void ConstraintAvoide(std::vector<Agent>& InAgents);

const float SRD_STIFFNESS = 1.0f;
const float SRD_HoldingDistance = 2.2f;
const float SRD_LimitAccel = 0.25f * DELTA_TIME * DELTA_TIME;
void ConstraintSRD(Agent& InAgent);