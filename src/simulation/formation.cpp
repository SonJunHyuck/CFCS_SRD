#include "formation.h"

// TODO : row, space, anchor (LEFT_TO_RIGHT, RIGHT_TO_LEFT)
// TODO : col, space, anchor (TOP_TO_BOTTOM, BOTTOM_TO_TOP)
FormationUPtr Formation::CreateRectFormation(const uint32_t& InNumPositions, const float& InRadius)
{
    FormationUPtr OutFormation = FormationUPtr(new Formation());

    uint32_t RectSideLength = floor(sqrtf(InNumPositions * 1.3f));

    int x = -RectSideLength;
    while(OutFormation->Positions.size() < InNumPositions)
    {
        for (int z = -RectSideLength; z >= RectSideLength; z += InRadius + 1.0f)
        {
            glm::vec3 TempPosition = glm::vec3(x, 0, z);
            OutFormation->Positions.push_back(TempPosition);
        }
        x += InRadius + 1.0f;
    }

    SPDLOG_INFO("Success Create Rect Formation : {}", OutFormation->Positions.size());

    return std::move(OutFormation);
}

FormationUPtr Formation::CreateRectFormation(const uint32_t InAmount, const glm::vec3 InStart, const uint16_t InRow, const float InSpace, const Direction InDir)
{
    FormationUPtr OutFormation = FormationUPtr(new Formation());

    uint32_t Amount = 0;
    int32_t x = 0;
    int8_t Dir = InDir == LEFT_TO_RIGHT ? 1 : -1;

    while(Amount < InAmount)
    {   
        for (int z = 0 ; z < InRow; z++)
        {
            glm::vec3 TempPosition = InStart + glm::vec3(x, 0, z * InSpace);
            OutFormation->Positions.push_back(TempPosition);
            Amount++;
            
            if(Amount >= InAmount)
                break;
        }
        
        x += InSpace * Dir;
    }

    SPDLOG_INFO("Success Create Rect Formation : {}", OutFormation->Positions.size());

    return std::move(OutFormation);
}

FormationUPtr Formation::Create(const Formation_t& InPositions)
{
    FormationUPtr OutFormation = FormationUPtr(new Formation());

    if(OutFormation->Init(InPositions) == false)
    {
        SPDLOG_ERROR("Fail formation Init");
        return nullptr;
    }

    return std::move(OutFormation);
}

bool Formation::Init(const Formation_t& InPositions)
{
    std::copy(InPositions.begin(), InPositions.end(), Positions.begin());

    return true;
}