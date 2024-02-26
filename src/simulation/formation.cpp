#include "formation.h"

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