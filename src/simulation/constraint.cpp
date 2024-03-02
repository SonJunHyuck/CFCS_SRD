#include "constraint.h"
#include "agent.h"

void FrictionConstraint(Agent& OutAgent1, Agent& OutAgent2, const float& InStiffness)
{
    glm::vec3 Correction1 = VEC_ZERO;  // delta == correction
    glm::vec3 Correction2 = VEC_ZERO;
    const float WeightedCoefficient1 = 0.5f;
    const float WeightedCoefficient2 = -0.5f;

    const float PredictedDistance = glm::distance(OutAgent1.PredictedPosition, OutAgent2.PredictedPosition);
    const float RadiusSum = OutAgent1.Radius + OutAgent2.Radius;
    const float CollisionMargin = RadiusSum * 1.05f;
    float PredictedPenetration = PredictedDistance - CollisionMargin;
    
    glm::vec3 ContactNormal = VEC_ZERO;
    glm::vec3 CollisionTangent = VEC_ZERO;

    glm::vec3 PredictedWeightedDelta1 = VEC_ZERO;
    glm::vec3 PredictedWeightedDelta2 = VEC_ZERO;
    
    glm::vec3 Out = VEC_ZERO;

    // Overlap
    if (PredictedPenetration < 0)
    {
        ContactNormal = (OutAgent1.PredictedPosition - OutAgent2.PredictedPosition) / PredictedDistance;

        // Kinetic
        Correction1 = -WeightedCoefficient1 * ContactNormal * PredictedPenetration;
        Correction2 = -WeightedCoefficient2 * ContactNormal * PredictedPenetration;

        PredictedWeightedDelta1 = Correction1 + OutAgent1.PredictedPosition;
        PredictedWeightedDelta2 = Correction2 + OutAgent2.PredictedPosition;

        float WeightedDeltaDistance = distance(PredictedWeightedDelta1, PredictedWeightedDelta2);
        ContactNormal.x = -(PredictedWeightedDelta1.z - PredictedWeightedDelta2.z) / WeightedDeltaDistance;
        ContactNormal.z = (PredictedWeightedDelta1.x - PredictedWeightedDelta2.x) / WeightedDeltaDistance;

        CollisionTangent = (PredictedWeightedDelta1 - OutAgent1.Position) - (PredictedWeightedDelta2 - OutAgent2.Position);

        ProjectOnVec3(CollisionTangent, ContactNormal, Out);

        float OutLength = glm::length(Out);

        if(OutLength >= FRICTION_MUI_STATIC * PredictedDistance)
        {
            float Coefficient = FRICTION_MUI_KINEMATIC * PredictedDistance / OutLength;
            Coefficient = std::min(1.0f, Coefficient);
            Out *= Coefficient;
        }

        Correction1 += -Out * WeightedCoefficient1;
        Correction2 += -Out * WeightedCoefficient2;

        ClampVec3(Correction1, FRICTION_LimitAccel);
        ClampVec3(Correction2, FRICTION_LimitAccel);

        OutAgent1.DeltaPosition += Correction1;
        OutAgent2.DeltaPosition += Correction2;

        OutAgent1.DeltaPositionCounter++;
        OutAgent2.DeltaPositionCounter++;
    }
    else  // Not overlap
    {
        const float Distance = glm::distance(OutAgent1.Position, OutAgent2.Position);
        float RadiusSumSq = pow(RadiusSum, 2);
        if (Distance < RadiusSum)
        {
            float Penetration = RadiusSum - Distance;
            RadiusSumSq = pow(Penetration, 2);
        }

        glm::vec3 V1 = (OutAgent1.PredictedPosition - OutAgent1.Position) / DELTA_TIME;
        glm::vec3 V2 = (OutAgent2.PredictedPosition - OutAgent2.Position) / DELTA_TIME;
        glm::vec3 V = V1 - V2;

        glm::vec3 P = OutAgent1.Position - OutAgent2.Position;

        float DotP = glm::dot(P, P);

        // Root Equation
        float a = glm::dot(V, V);
        float b = glm::dot(-V, P);
        float c = pow(Distance, 2) - RadiusSumSq;
        float d_sq = (b * b) - (a * c);

        if(d_sq > 0 && (a < -_EPSILON || a > _EPSILON))
        {
            float d = sqrtf(d_sq);
            const float NegTao = (b - d) / a;
            const float PosTao = (b + d) / a;
            const float Tao = (PosTao < NegTao) && (PosTao > 0) ? PosTao : NegTao;

            if(Tao > 0)
            {
                const float MinTao = Tao + DELTA_TIME;
                const glm::vec3 MinP1 = OutAgent1.Position + MinTao * V1;
                const glm::vec3 MinP2 = OutAgent2.Position + MinTao * V2;

                float MinTaoDistance = glm::distance(MinP1, MinP2);
                d = MinTaoDistance;
                float Penetration = d - CollisionMargin;

                // Penetration
                if(Penetration < 0 && d > _EPSILON)
                {
                    const float ClampTao = exp(-MinTao * MinTao / 5.0f);
                    const float k = InStiffness;

                    ContactNormal = (MinP1 - MinP2) / d;

                    Correction1 = -k * ClampTao * WeightedCoefficient1 * ContactNormal * Penetration;
                    Correction2 = -k * ClampTao * WeightedCoefficient2 * ContactNormal * Penetration;

                    const glm::vec3 TaoP1 = OutAgent1.Position + Tao * V1;
                    const glm::vec3 TaoP2 = OutAgent2.Position + Tao * V2;

                    PredictedWeightedDelta1 = Correction1 + MinP1;
                    PredictedWeightedDelta2 = Correction2 + MinP2;

                    float WeightedDeltaDistance = distance(PredictedWeightedDelta1, PredictedWeightedDelta2);
                    ContactNormal.x = -(PredictedWeightedDelta1.z - PredictedWeightedDelta2.z) / WeightedDeltaDistance;
                    ContactNormal.z = (PredictedWeightedDelta1.x - PredictedWeightedDelta2.x) / WeightedDeltaDistance;

                    CollisionTangent = (PredictedWeightedDelta1 - TaoP1) - (PredictedWeightedDelta2 - TaoP2);

                    ProjectOnVec3(CollisionTangent, ContactNormal, Out);

                    float OutLength = glm::length(Out);

                    if (OutLength >= FRICTION_MUI_STATIC * d)
                    {
                        float Coefficient = FRICTION_MUI_KINEMATIC * d / OutLength;
                        Coefficient = std::min(1.0f, Coefficient);
                        Out *= Coefficient;
                    }

                    Correction1 += -Out * WeightedCoefficient1;
                    Correction2 += -Out * WeightedCoefficient2;

                    ClampVec3(Correction1, FRICTION_LimitAccel);
                    ClampVec3(Correction2, FRICTION_LimitAccel);

                    OutAgent1.DeltaPosition += Correction1;
                    OutAgent2.DeltaPosition += Correction2;

                    OutAgent1.DeltaPositionCounter++;
                    OutAgent2.DeltaPositionCounter++;
                }
            }
        }
    }

}

void AvoidConstraint(Agent& OutAgent1, Agent& OutAgent2)
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
    const float CollisionMargin = RadiusSum * 1.05f;
    
    float RadiusSumSq = pow(RadiusSum, 2);
    if(Distance < RadiusSum)
    {
        float Penetration = RadiusSum - Distance;
        RadiusSumSq = pow(Penetration, 2);
    }

    // Avoidance Model (Weiss)
    glm::vec3 V = ( (OutAgent1.PredictedPosition - OutAgent1.Position) / DELTA_TIME ) - ( (OutAgent2.PredictedPosition - OutAgent2.Position) / DELTA_TIME );
    glm::vec3 P = OutAgent1.Position - OutAgent2.Position;

    // Root Equation
    float a = glm::dot(V, V);
    float b = glm::dot(-V, P);
    float c = pow(Distance, 2) - RadiusSumSq;
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
            const float TaoHat = floorf(Tao / DELTA_TIME) * DELTA_TIME;
            const float TaoDia = TaoHat + DELTA_TIME;

            const glm::vec3 Hat1 = OutAgent1.Position + TaoHat * OutAgent1.Velocity;
            const glm::vec3 Hat2 = OutAgent2.Position + TaoHat * OutAgent2.Velocity;

            const glm::vec3 Dia1 = OutAgent1.Position + TaoDia * OutAgent1.Velocity;
            const glm::vec3 Dia2 = OutAgent2.Position + TaoDia * OutAgent2.Velocity;

            const float ConstraintValue = glm::distance(Dia1, Dia2) - CollisionMargin;

            if(ConstraintValue < 0)
            {
                glm::vec3 Normal = (Dia1 - Dia2) / glm::distance(Dia1, Dia2);

                const glm::vec3 Gradient1 = Normal;
                const glm::vec3 Gradient2 = -Normal;

                const float Stiffness = AVOIDANCE_STIFFNESS * exp( -(TaoHat * TaoHat) / AVOIDANCE_TAO0 );

                const float s = ConstraintValue / 
                ( 
                    OutAgent1.InverseMass * static_cast<double>( pow(glm::length(Gradient1), 2) ) + 
                    OutAgent2.InverseMass * static_cast<double>( pow(glm::length(Gradient2), 2) )
                );
                
                Correction1 = Gradient1 * (-s) * OutAgent1.InverseMass * Stiffness;
                Correction2 = Gradient2 * (-s) * OutAgent2.InverseMass * Stiffness;

                if(ON_AVOIDANCE_MODEL)
                {
                    glm::vec3 AvoidanceDir1 = Dia1 + Correction1;
                    glm::vec3 AvoidanceDir2 = Dia2 + Correction2;

                    glm::vec3 AvoidanceUnit = glm::normalize(AvoidanceDir1 - AvoidanceDir2);

                    glm::vec3 AvoidanceDelta = (AvoidanceDir1 - Hat1) - (AvoidanceDir2 - Hat2);
                    glm::vec3 AvoidanceDeltaNormal = AvoidanceUnit * glm::dot(AvoidanceDelta, AvoidanceUnit);
                    glm::vec3 AvoidanceTangent = AvoidanceDelta - AvoidanceDeltaNormal;

                    Correction1 += AvoidanceTangent * OutAgent1.PreferedSpeed * DELTA_TIME;
                    Correction2 -= AvoidanceTangent * OutAgent2.PreferedSpeed * DELTA_TIME;

                    float Avoidance_Weight = 1.0f;
                    ClampVec3(Correction1, AVOIDANCE_LimitAccel * Avoidance_Weight);
                    ClampVec3(Correction2, AVOIDANCE_LimitAccel * Avoidance_Weight);

                    OutAgent1.DeltaPosition += Correction1;
                    OutAgent2.DeltaPosition += Correction2;

                    OutAgent1.DeltaPositionCounter++;
                    OutAgent2.DeltaPositionCounter++;
                }
            }
        }
    }
}

void StabilityConstraint(Agent& OutAgent1, Agent& OutAgent2)
{
    glm::vec3 Correction1 = VEC_ZERO;
    glm::vec3 Correction2 = VEC_ZERO;

    const float WeightedCoefficient1 = 0.5f;
    const float WeightedCoefficient2 = 0.5f;

    const float Distance = glm::distance(OutAgent1.Position, OutAgent2.Position);
    const float RadiusSum = OutAgent1.Radius + OutAgent2.Radius;
    const float CollisionMargin = RadiusSum * 1.05f;

    const float Penetration = Distance - CollisionMargin;  // Penetration < 0 -> Overlap

    if (Penetration < 0)
    {
        glm::vec3 ContactNormal = VEC_ZERO;
        glm::vec3 CollisionTangent = VEC_ZERO;

        glm::vec3 PredictedWeightedDelta1 = VEC_ZERO;
        glm::vec3 PredictedWeightedDelta2 = VEC_ZERO;

        glm::vec3 Out = VEC_ZERO;

        ContactNormal = (OutAgent1.Position - OutAgent2.Position) / Distance;

        OutAgent1.DeltaPosition = -WeightedCoefficient1 * ContactNormal * Penetration;
        OutAgent2.DeltaPosition = -WeightedCoefficient2 * ContactNormal * Penetration;
    }
}

void SRDConstraint(Agent& OutAgent)
{
    glm::vec3 Correction = VEC_ZERO;

    const float Coefficient = -0.5f;
    const float Distance = glm::distance(OutAgent.Position, OutAgent.SRD);
    const float SRD_STIFFNESS = 1.0f;

    if(Distance > SRD_HoldingDistance)
    {
        glm::vec3 ContactNormal = OutAgent.Position - OutAgent.SRD;
        Correction += ContactNormal * Coefficient * SRD_STIFFNESS;
        ClampVec3(Correction, SRD_LimitAccel);

        OutAgent.DeltaPosition += Correction;
        OutAgent.DeltaPositionCounter++;
    }
}