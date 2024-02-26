#include "group.h"
#include "agent.h"

Group::Group(uint16_t InGroupId)
{
    Id = InGroupId;
    CurrentWaypointId = 0;
}

Group::~Group()
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

        Position += Velocity;
    }

    float DensityWeight = 0.2f;
    if(Distance < GRID_DENSITY - DensityWeight)
    {
        if(CurrentWaypointId < Path.size() - 1)
            CurrentWaypointId++;
    }
}

void Group::GetWaypoints(std::vector<glm::vec3> &OutPath)
{
    OutPath = Path;
}

void Group::Init(const glm::vec3& InPosition, const float InPreferedSpeed, const glm::vec3& InColor)
{   
    Position = InPosition;
    PreferedSpeed = InPreferedSpeed;
    Color = InColor;
}

Group GroupFactory::Create(const uint8_t& InGroupId, const glm::vec3& InPosition, const float InPreferedSpeed, const glm::vec3& InColor)
{
    Group OutGroup = Group(InGroupId);

    OutGroup.Init(InPosition, InPreferedSpeed, InColor);

    return OutGroup;
}