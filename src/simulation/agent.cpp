#include "agent.h"

void Agent::Init()
{

}

void Agent::PlanVelocity()
{
    glm::vec3 PlanedVelocity = SRD - Position;

    float Distance = glm::length(PlanedVelocity);
    if (Distance > _EPSILON)
    {
        PlanedVelocity /= Distance;
        PlanedVelocity *= std::min(PreferedSpeed, Distance);

        Velocity = PlanedVelocity;
        PredictedPosition += Velocity * DELTA_TIME;
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

Agent AgentFactory::CreateAgent(const uint32_t& InAgentId, const uint8_t& InGroupId)
{
    Agent OutAgent = Agent(InAgentId, InGroupId);

    OutAgent.Init();

    return OutAgent;
}