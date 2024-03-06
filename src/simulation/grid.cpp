#include "grid.h"
#include "agent.h"

Grid::Grid() { }

Grid::Grid(float InCellSize, glm::vec3 InMinBound, glm::vec3 InMaxBound)
{
    CellSize = InCellSize;
    MinBound = InMinBound;
    MaxBound = InMaxBound;

    NumRows = (MaxBound.z - MinBound.z) / CellSize;
    NumCols = (MaxBound.x - MinBound.x) / CellSize;
    NumCells = NumRows * NumCols;

    SPDLOG_INFO("GRID ROW : {}, GRID COL : {}, GRID CELL : {}", NumRows, NumCols, NumCells);

    Cells = std::vector<Cell>(NumCells);
}

Grid::~Grid()
{

}

void Grid::GetNeighborAgents(const Agent& InAgent, const uint8_t InRange, std::vector<int32_t>& OutNeighbors)
{
    // iterate over adjacent cells
    for (int X = -InRange; X <= InRange; X++)
    {
        const int CurX = InAgent.CellX + X;
        if (CurX >= 0 && CurX < NumCols)
        {
            for (int Z = -2; Z <= 2; Z++)
            {
                const int CurZ = InAgent.CellZ + Z;
                if (CurZ >= 0 && CurZ < NumRows)
                {
                    const uint32_t CurCellId = InAgent.CellId + X + (Z * NumRows);
                    const Cell CurCell = Cells[CurCellId];

                    if (CurCell.Counter > 0)
                    {
                        for(const int32_t Guest : CurCell.Guests)
                        {
                            // uint32_t vs int32_t -> (uint32_t)-1 == 4294967295
                            if( (int32_t)InAgent.Id < Guest )
                            {
                                OutNeighbors.push_back(Guest);
                            }
                        }
                    }
                }
            }
        }
    }
}

int8_t Grid::GetCellState(const uint32_t InCellId)
{
    return Cells[InCellId].Congestion;
}

bool Grid::IsConquerableCell(uint32_t InCellId, uint8_t InGroupId)
{
    if(Cells[InCellId].Congestion == Cell::STATE::EMPTY)
        return true;
     
    if(Cells[InCellId].Congestion == InGroupId)
        return true;

    return false;
}

void Grid::Update(std::vector<Agent>& InAgents)
{
    for(Cell& Iter : Cells)
    {
        Iter.Counter = 0;
        Iter.Congestion = Cell::STATE::EMPTY;

        for(int32_t& Guest : Iter.Guests)
        {
            Guest = -1;  // Nobody agents
        }
    }

	// adding particles to grid
	for (int i = 0; i < InAgents.size(); i++)
	{
		glm::vec3 Position = InAgents[i].PredictedPosition;
		const uint32_t x = (Position.x - MinBound.x) / CellSize;
		const uint32_t z = (Position.z - MinBound.z) / CellSize;
		const uint32_t CellId = z * NumRows + x;
		InAgents[i].CellId = CellId;
		InAgents[i].CellX = x;
		InAgents[i].CellZ = z;

        uint16_t Counter = Cells[CellId].Counter;
		Cells[CellId].Guests[Counter] = InAgents[i].Id;  // Counter is number of guests
		Cells[CellId].Counter += 1;

		// to search particle mixed zone;
        if(IsConquerableCell(CellId, InAgents[i].GroupId))
        {
            Cells[CellId].Congestion = InAgents[i].GroupId;
        }
        else
        {
            Cells[CellId].Congestion = Cell::STATE::MIXED;
        }
	}

    // After Cell Update Complete
    for(Agent& agent : InAgents)
    {
        if(Cells[agent.CellId].Congestion == Cell::STATE::MIXED)
        {
            agent.bIsConnected = true;
        }
        else
        {
            agent.bIsConnected = false;
        }
    }
}