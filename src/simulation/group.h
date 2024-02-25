#include <vector>

#include "../common.h"

CLASS_PTR(Group)
class Group
{
public:
    ~Group();

    void DrawPath(const glm::vec3& Waypoint);
    void FollowPath();  // (before) Update Path
    glm::vec3 GetPosition() { return Position; }
    glm::vec3 GetVelocity() { return Velocity; }

private:
    Group();
    Group(uint16_t InGroupId);
    uint16_t Id;

    glm::vec3 Position;
    glm::vec3 Velocity;
    glm::vec3 Color;
    float PreferedSpeed;

    Formation_t SRDs;  // Short Range Destination

    std::vector<glm::vec3> Path;
    uint32_t CurrentWaypointId;

    // Init
    void Init(const glm::vec3& InPosition, const float InPreferedSpeed);
    
    friend class GroupFactory;
};

class GroupFactory
{
public:
    static Group Create(const uint8_t& InGroupId, const glm::vec3& InPosition, const float InPreferedSpeed);
};