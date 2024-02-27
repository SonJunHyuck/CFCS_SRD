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
    enum Direction
    {
        LEFT_TO_RIGHT,
        RIGHT_TO_LEFT,
        TOP_TO_BOTTOM,
        BOTTOM_TO_TOP
    };

public:
    static FormationUPtr CreateRectFormation(const uint32_t InAmount, const glm::vec3 InStart, const uint16_t InRow, const float InSpace, const Direction InDir = LEFT_TO_RIGHT);
    static FormationUPtr Create(const Formation_t& InPositions);
    ~Formation() { Positions.clear(); }

    Formation_t GetPositions() { return Positions; }
};