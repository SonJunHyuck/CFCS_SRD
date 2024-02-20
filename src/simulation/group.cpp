#include "group.h"
#include "waypoint.h"

Group::Group(uint16_t InGroupId)
{
    Id = InGroupId;

    Init();
}

Group::~Group()
{

}

void Group::Init()
{
    
}

void Group::DrawPath(const glm::vec3& Waypoint)
{
    Path.push_back(Waypoint);
}

void Group::FollowPath()
{
    glm::vec3 CurrentWaypointPos = Path[CurrentWaypointId];
    Velocity = CurrentWaypointPos - Position;
    float Distance = glm::length(Velocity);
    
    if(Distance > _EPSILON)
    {
        float MinPreferedSpeed = glm::min(Distance, PreferedSpeed);

        Velocity /= Distance;  // Normalize;
        Velocity *= MinPreferedSpeed * DELTA_TIME;

        UpdateSRDs();

        Position += Velocity;
    }

    float DensityWeight = 0.2f;
    if(Distance < GRID_DENSITY - DensityWeight)
    {
        if(CurrentWaypointId < Path.size() - 1)
            CurrentWaypointId++;
    }
}

void Group::UpdateSRDs()
{
    for(auto& IterSRD : SRDs)
    {
        IterSRD.second += Velocity;
    }
}

// void Group::PlanAgentVelocity()
// {
//     for (Agent& agent : Agents)
//     {
//         agent.PlanVelocity();
//     }
// }

// void Group::CorrectAgentPosition()
// {
//     for (Agent &agent : Agents)
//     {
//         agent.CorrectPosition();
//     }
// }