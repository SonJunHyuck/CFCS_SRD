#include <vector>
#include <map>

#include "../common.h"

class Group
{
public:
    Group();
    Group(uint16_t InGroupId);
    ~Group();

    void CorrectAgentPosition();

private:
    uint16_t Id;

    glm::vec3 Position;
    glm::vec3 Velocity;
    glm::vec3 Color;

    std::vector<class Agent> Agents;
    std::map<uint32_t, glm::vec3> SRDs;  // Short Range Destination

    std::vector<class Waypoint> Path;
    uint32_t CurrentWaypointId;

    // Init
    void Init();

    // Update
    void FollowPath();  // (before) Update Path
    void UpdateSRDs();
    void PlanAgentVelocity();  
};