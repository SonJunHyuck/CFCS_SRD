#include <vector>

#include "../common.h"

// extern float DELTA_TIME;
// extern float GRID_DENSITY;

CLASS_PTR(Group)
class Group
{
public:
    ~Group();

    void DrawPath(const glm::vec3& Waypoint);
    void FollowPath();  // (before) Update Path
    uint8_t GetId() { return Id; }
    glm::vec3 GetPosition() { return Position; }
    void SetPosition(const glm::vec3 InPosition) { Position = InPosition; }
    glm::vec3 GetVelocity() { return Velocity; }
    glm::vec3 GetColor() { return Color; }
    void GetWaypoints(std::vector<glm::vec3>& OutPath);
    bool HasPath() { return Path.size() > 0; }

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