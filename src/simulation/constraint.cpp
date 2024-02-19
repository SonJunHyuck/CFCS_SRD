#include "constraint.h"
#include "agent.h"

void ConstraintAvoide(Agent& OutAgent1, Agent& OutAgent2)
{
    glm::vec3 Correction1 = VEC_ZERO;
    glm::vec3 Correction2 = VEC_ZERO;

    if(OutAgent1.GroupId == OutAgent2.GroupId)
    {
        return;
    }

    const float Distance = glm::distance(OutAgent1.Position, OutAgent2.Position);
    const float PredictedDistance = glm::distance(OutAgent1.PredictedPosition, OutAgent2.PredictedPosition);
    const float RadiusSum = OutAgent1.Radius + OutAgent2.Radius;
    float RadiusSumSq = glm::pos(RadiusSum, 2);

    if(Distance < RadiusSum)
    {
        float Penetration = RadiusSum - Distance;
        RadiusSumSq = glm::pow(Penetration, 2);
    }

    // Avoidance Model (Weiss)
    glm::vec3 V = ( (OutAgent1.PredictedPosition - OutAgent1.Position) / DeltaTime ) - ( (OutAgent2.PredictedPosition - OutAgent2.Position) / DeltaTime ); 
    glm::vec3 P = OutAgent1.Position - OutAgent2.Position;

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

            const glm::vec3 Hat1 = OutAgent1.Position + TaoHat * OutAgent1.Velocity;
            const glm::vec3 Hat2 = OutAgent2.Position + TaoHat * OutAgent2.Velocity;

            const glm::vec3 Dia1 = OutAgent1.Position + TaoDia * OutAgent1.Velocity;
            const glm::vec3 Dia1 = OutAgent2.Position + TaoDia * OutAgent2.Velocity;

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
                    OutAgent1.InverseMass * static_cast<double>( pow(glm::length(Gradient1), 2) ) + 
                    OutAgent2.InverseMass * static_cast<double>( pow(glm::length(Gradient2), 2) )
                )
                
                Correction1 = Gradient1 * (-s) * OutAgent1.InverseMass * Stiffness;
                Correction2 = Gradient2 * (-s) * OutAgent2.InverseMass * Stiffness;

                if(ON_AVOIDANCE_MODEL)
                {
                    glm::vec3 AvoidanceDir1 = Dia1 + Correction1;
                    glm::vec3 AvoidanceDir2 = Dia2 + Correction2;

                    glm::vec3 AvoidanceUnit = glm::normalize(AvoidanceDir1 - AvoidanceDir2);

                    glm::vec3 AvoidanceDelta = (AvoidanceDir1 - Hat1) - (AvoidanceDir2 - Hat2);
                    glm::vec3 AvoidanceDeltaNormal = AvoidanceUnit * glm::dot(AvoidanceDelta, AvoidanceUnit);
                    glm::vec3 AvoidanceDeltaDelta = AvoidanceDelta - AvoidanceDeltaNormal;

                    Correction1 += AvoidanceDeltaDelta * OutAgent1.PreferedSpeed * DELTA_TIME;
                    Correction2 -= AvoidanceDeltaDelta * OutAgent2.PreferedSpeed * DELTA_TIME;

                    ClampVec3(Correction1, AVOIDANCE_LimitAccel);
                    ClampVec3(Correction2, AVOIDANCE_LimitAccel);

                    OutAgent1.DeltaPosition += Correction1;
                    OutAgent2.DeltaPosition += Correction2;

                    OutAgent1.DeltaPositionCounter++;
                    OutAgent2.DeltaPositionCounter++;
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