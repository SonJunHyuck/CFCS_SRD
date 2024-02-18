#include "constraint.h"
#include "agent.h"

void ConstraintAvoide(std::vector<Agent>& InAgents)
{
    Agent Agent1 = InAgents[0];
    Agent Agent2 = InAgents[1];

    glm::vec3 Correction1 = VEC_ZERO;
    glm::vec3 Correction2 = VEC_ZERO;

    if(Agent1.GroupId == Agent2.GroupId)
    {
        return;
    }

    const float Distance = glm::distance(Agent1.Position, Agent2.Position);
    const float PredictedDistance = glm::distance(Agent1.PredictedPosition, Agent2.PredictedPosition);
    const float RadiusSum = Agent1.Radius + Agent2.Radius;
    float RadiusSumSq = glm::pos(RadiusSum, 2);

    if(Distance < RadiusSum)
    {
        float Penetration = RadiusSum - Distance;
        RadiusSumSq = glm::pow(Penetration, 2);
    }

    // Avoidance Model (Weiss)
    glm::vec3 V = ( (Agent1.PredictedPosition - Agent1.Position) / DeltaTime ) - ( (Agent2.PredictedPosition - Agent2.Position) / DeltaTime ); 
    glm::vec3 P = Agent1.Position - Agent2.Position;

    float DotP = glm::dot(P, P);

    // Root Equation
    float a = glm::dot(V, V);
    float b = glm::dot(-V, P);
    float c = glm::pow(Distance, 2) - RadiusSumSq;
    float d_sq = (b * b) - (a * c);
    float d = sqrtf(d_sq);

    if(d_sq > 0 && (a < -_EPSILON || a > _EPSILON))
    {
        const float NegTao = (b - d) / a;
        const float PosTao = (b + d) / a;

        if(NegTao > 0 && NegTao < AVOIDANCE_TAO0)
        {
            // Min + Pos
            const float Tao = (PosTao < NegTao) && (PosTao > 0) ? PosTao : NegTao;
            const float Tao_Hat = floorf(Tao / DELTA_TIME) * DELTA_TIME;
            const float Tao_Dia = Tao_Hat + DELTA_TIME;

            const glm::vec3 Hat1 = Agent1.Position + TaoHat * Agent1.Velocity;
            const glm::vec3 Hat2 = Agent2.Position + TaoHat * Agent2.Velocity;

            const glm::vec3 Dia1 = Agent1.Position + TaoDia * Agent1.Velocity;
            const glm::vec3 Dia1 = Agent2.Position + TaoDia * Agent2.Velocity;

            const float CollisionMargin = RadiusSum * 1.05f;
            const float ConstraintValue = glm::distance(Dia1, Dia2) - CollisionMargin;

            if(ConstraintValue < 0)
            {
                glm::vec3 Normal = (Dia1 - Dia2) / glm::distance(Dia1, Dia2);

                const float Gradient1 = Normal;
                const float Gradient2 = -Normal;

                const float Stiffness = AVOIDANCE_STIFFNESS * exp( -(TaoHat * TaoHat) / AVOIDANCE_TAO0 );

                const float s = ConstraintValue / 
                ( 
                    Agent1.InverseMass * static_cast<double>( pow(glm::length(Gradient1), 2) ) + 
                    Agent2.InverseMass * static_cast<double>( pow(glm::length(Gradient2), 2) )
                )
                
                Correction1 = Gradient1 * (-s) * Agent1.InverseMass * Stiffness;
                Correction2 = Gradient2 * (-s) * Agent2.InverseMass * Stiffness;

                if(ON_AVOIDANCE_MODEL)
                {
                    glm::vec3 AvoidanceDir1 = Dia1 + Correction1;
                    glm::vec3 AvoidanceDir2 = Dia2 + Correction2;

                    glm::vec3 AvoidanceUnit = glm::normalize(AvoidanceDir1 - AvoidanceDir2);

                    glm::vec3 AvoidanceDelta = (AvoidanceDir1 - Hat1) - (AvoidanceDir2 - Hat2);
                    glm::vec3 AvoidanceDeltaNormal = AvoidanceUnit * glm::dot(AvoidanceDelta, AvoidanceUnit);
                    glm::vec3 AvoidanceDeltaDelta = AvoidanceDelta - AvoidanceDeltaNormal;

                    Correction1 += AvoidanceDeltaDelta * Agent1.PreferedSpeed * DELTA_TIME;
                    Correction2 -= AvoidanceDeltaDelta * Agent2.PreferedSpeed * DELTA_TIME;

                    ClampVec3(Correction1, AVOIDANCE_LimitAccel);
                    ClampVec3(Correction2, AVOIDANCE_LimitAccel);

                    Agent1.DeltaPosition += Correction1;
                    Agent2.DeltaPosition += Correction2;
                }
            }
        }
    }
}

void ConstraintSRD(Agent& InAgent)
{
    glm::vec3 Correction = VEC_ZERO;

    const float Coefficient = -0.5f;
    const float Distance = glm::distance(InAgent.Position, InAgent.SRD);
    const float SRD_STIFFNESS = 1.0f;

    if(Distance > SRD_HoldingDistance)
    {
        glm::vec3 ContactNormal = InAgent.Position - InAgent.SRD;
        Correction += ContactNormal * Coefficient * SRD_STIFFNESS;
        ClampVec3(Correction, SRD_LimitAccel);

        InAgent.DeltaPosition += Correction;
    }
}