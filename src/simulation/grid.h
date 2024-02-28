#pragma once

#include "../common.h"

class Agent;

CLASS_PTR(Grid)
class Grid
{
public:
    const static uint16_t MaxPerCell = 10;

    Grid();
    Grid(float InCellSize, glm::vec3 InMinBound, glm::vec3 InMaxBound);
    ~Grid();

    void GetNeighborAgents(const Agent& InAgent, const uint8_t InRange, std::vector<int32_t>& OutNeighbors);

    void Update(std::vector<Agent>& Agents);

public:
    struct Cell
    {
        enum STATE
        {
            EMPTY = -128,
            MIXED = 127
        };

        uint32_t Counter;
        int32_t Guests[MaxPerCell];
        int32_t Congestion;

        Cell()
        {
            Counter = 0;
            Congestion = STATE::EMPTY;
            std::fill_n(Guests, MaxPerCell, -1);
        }
    };

private:
    uint32_t NumParticles;
    uint32_t NumCells;
    uint32_t NumRows;
    uint32_t NumCols;
    
    float CellSize;
    glm::vec3 MinBound;
    glm::vec3 MaxBound;

    std::vector<Cell> Cells;

    bool IsConquerableCell(uint32_t InCellId, uint8_t InGroupId);
};

