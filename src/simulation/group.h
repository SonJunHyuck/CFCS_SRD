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
    glm::vec3 GetColor() { return Color; }
    void GetWaypoints(std::vector<glm::vec3>& OutPath);

private:
    Group();
    Group(uint16_t InGroupId);
    uint16_t Id;

    glm::vec3 Position { VEC_ZERO };
    glm::vec3 Velocity { VEC_ZERO };
    glm::vec3 Color { VEC_ZERO };
    float PreferedSpeed { 0 };

    // Formation_t SRDs;  // Short Range Destination

    std::vector<glm::vec3> Path;
    uint32_t CurrentWaypointId;

    // Init
    void Init(const glm::vec3& InPosition, const float InPreferedSpeed, const glm::vec3& InColor);
    
    friend class GroupFactory;
};

class GroupFactory
{
public:
    static Group Create(const uint8_t& InGroupId, const glm::vec3& InPosition, const float InPreferedSpeed, const glm::vec3& InColor);
};