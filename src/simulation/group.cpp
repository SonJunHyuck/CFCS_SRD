#include "group.h"

Group::Group(uint16_t InGroupId)
{
    Id = InGroupId;
}

Group::~Group()
{

}

void Group::Init(const std::vector<std::pair<uint32_t, glm::vec3>> InFormation)
{
    for(auto AgentPosition : InFormation)
    {
        SRDs.emplace(AgentPosition.first, AgentPosition.second);
    }
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

Group GroupFactory::Create(const uint8_t& InGroupId, const std::vector<std::pair<uint32_t, glm::vec3>> InFormation)
{
    Group OutGroup = Group(InGroupId);

    OutGroup.Init(InFormation);

    return OutGroup;
}