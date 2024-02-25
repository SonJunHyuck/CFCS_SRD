#pragma once

#include "../common.h"

CLASS_PTR(Formation)
class Formation
{
private:
    Formation() { Positions = Formation_t(); }
    bool Init(const Formation_t& InPositions);
    
    std::vector<glm::vec3> Positions;

public:
    static FormationUPtr CreateRectFormation(const uint32_t& InNumPositions, const float& InRadius);
    static FormationUPtr Create(const Formation_t& InPositions);
    ~Formation() { Positions.clear(); }

    Formation_t GetPositions() { return Positions; }
};