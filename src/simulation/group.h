#include <vector>
#include <map>

#include "../common.h"

CLASS_PTR(Group)
class Group
{
public:
    ~Group();

    // void CorrectAgentPosition();
    
    void DrawPath(const glm::vec3& Waypoint);
    void FollowPath();  // (before) Update Path
    // void PlanAgentVelocity();  

private:
    Group();
    Group(uint16_t InGroupId);
    uint16_t Id;

    glm::vec3 Position;
    glm::vec3 Velocity;
    glm::vec3 Color;
    float PreferedSpeed;

    std::map<uint32_t, glm::vec3> SRDs;  // Short Range Destination

    std::vector<glm::vec3> Path;
    uint32_t CurrentWaypointId;

    // Init
    void Init(const std::vector<std::pair<uint32_t, glm::vec3>> InFormation);
    
    void UpdateSRDs();

    friend class GroupFactory;
};

class GroupFactory
{
public:
    static Group Create(const uint8_t& InGroupId, const std::vector<std::pair<uint32_t, glm::vec3>> InFormation);
};