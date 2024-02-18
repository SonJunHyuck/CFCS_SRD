#include "agent.h"

Agent::Agent()
{
    InverseMass = 1 / Mass;
    Init();
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