#pragma once

#include "../common.h"

CLASS_PTR(Grid)
class Grid
{
public:
    const static uint16_t MaxPerCell = 10;

    Grid(float InCellSize, glm::vec3 InMinBound, glm::vec3 InMaxBound);
    ~Grid();

    void Update(std::vector<class Agent>& Agents);

public:
    struct Cell
    {
        enum STATE
        {
            EMPTY = -128,
            MIXED = 127
        };

        uint32_t Counter;
        std::vector<uint16_t> Guests;
        uint32_t Congestion;

        Cell()
        {
            Counter = 0;
            Guests = std::vector<uint16_t>(MaxPerCell);
            Congestion = STATE::EMPTY;
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

