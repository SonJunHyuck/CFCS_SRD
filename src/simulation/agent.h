#pragma once

#include "../common.h"

class Agent
{
private:
Agent() {}
~Agent() {}

private:
void Init();

public:
	uint32_t Id;

    glm::vec3 Position;
    glm::vec3 PredictedPosition;

    glm::vec3 Velocity;
    glm::vec3 PreviousVelocity;
    float PreferedSpeed;

    glm::vec3 DeltaPosition;
    int DeltaPositionCounter;  // MUST be change

	glm::vec3 SRD;

	float Mass;
	float InverseMass;

	uint8_t GroupId;

	glm::vec3 CurrentWaypoint;
    glm::vec3 FinalDestination;

	float Radius;

	// grid
	int CellId;
	int CellX;
	int CellZ;

	uint8_t bIsLeader;
	uint8_t bIsConnected;

	void PlanVelocity();
	void CorrectPosition();
};