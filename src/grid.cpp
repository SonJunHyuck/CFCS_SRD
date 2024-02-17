#include "grid.h"
#include "agent.h"

Grid::Grid(float InCellSize, glm::vec3 InMinBound, glm::vec3 InMaxBound)
{
    CellSize = InCellSize;
    MinBound = InMinBound;
    MaxBound = InMaxBound;

    NumRows = (MaxBound.x - MinBound.x) / CellSize;
    NumCols = (MaxBound.z - MinBound.z) / CellSize;
    NumCells = NumRows * NumCols;

    Cells = std::vector<Cell>(NumCells);
}

Grid::~Grid()
{

}

bool Grid::IsConquerableCell(uint32_t InCellId, uint8_t InGroupId)
{
    if(Cells[InCellId].Congestion == Cell::STATE::EMPTY)
        return true;
     
    if(Cells[InCellId].Congestion == InGroupId)
        return true;

    return false;
}

void Grid::UpdateStability()
{
}

void Grid::Update(std::vector<Agent>& Agents)
{

    for(Cell& Iter : Cells)
    {
        Iter.Counter = 0;
        Iter.Congestion = Cell::STATE::EMPTY;

        for(uint16_t& Guest : Iter.Guests)
        {
            Guest = -1;  // Nobody agents
        }
    }

	// adding particles to grid
	for (int i = 0; i < Agents.size(); i++)
	{
		glm::vec3 Position = Agents[i].PredictedPosition;
		uint32_t x = (Position.x - MinBound.x) / CellSize;
		uint32_t z = (Position.z - MinBound.z) / CellSize;
		uint32_t CellId = z * NumRows + x;
		Agents[i].CellId = CellId;
		Agents[i].CellX = x;
		Agents[i].CellZ = z;

        uint16_t Counter = Cells[CellId].Counter;
		Cells[CellId].Guests[Counter] = i;  // Counter is number of guests
		Cells[CellId].Counter += 1;

		// to search particle mixed zone;
        if(IsConquerableCell(CellId, Agents[i].GroupId))
        {
            Cells[CellId].Congestion = Agents[i].GroupId;
        }
        else
        {
            Cells[CellId].Congestion = Cell::STATE::MIXED;
        }
	}
}