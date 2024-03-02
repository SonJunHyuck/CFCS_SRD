#include "agent.h"

void Agent::Init(const float InMass, const float InRadius, const float InPreferedSpeed, const glm::vec3& InPosition)
{
    Mass = InMass;
    InverseMass = 1 / Mass;
    Radius = InRadius;
    PreferedSpeed = InPreferedSpeed;
    Position = InPosition;
    PredictedPosition = InPosition;
    SRD = InPosition;
}

void Agent::PlanVelocity()
{
    glm::vec3 PlanedVelocity = SRD - Position;

    float Distance = glm::length(PlanedVelocity);
    
    if (Distance > _EPSILON)
    {
        PlanedVelocity /= Distance;  // normalize
        PlanedVelocity *= std::min(PreferedSpeed, Distance);

        float T = 0.01f;
        Velocity = T * PlanedVelocity + (1 - T) * Velocity;
        PredictedPosition += Velocity * DELTA_TIME;
        PredictedPosition.y = 0;  // clear trash
    }
}

void Agent::CorrectPosition()
{
    if (DeltaPositionCounter > 0)
    {
        PredictedPosition += DeltaPosition / static_cast<float>(DeltaPositionCounter);
        DeltaPosition = VEC_ZERO;
        DeltaPositionCounter = 0;
    }
}

Agent AgentFactory::Create(const uint32_t& InAgentId, const uint8_t& InGroupId, const float InMass, const float InRadius, const float InPreferedSpeed, const glm::vec3& InPosition)
{
    Agent OutAgent = Agent(InAgentId, InGroupId);

    OutAgent.Init(InMass, InRadius, InPreferedSpeed, InPosition);

    return OutAgent;
}