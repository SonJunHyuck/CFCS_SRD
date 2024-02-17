#pragma once

#include "common.h"

class Agent
{
public:
Agent() {}
~Agent() {}

float GetInverseMass() { return 1 / Mass; }

private:
void Init();

public:
    glm::vec3 Position;
    glm::vec3 PredictedPosition;

    glm::vec3 Velocity;
    glm::vec3 PreviousVelocity;
    float PreferedSpeed;

    glm::vec3 DeltaPosition;
    int DeltaPositionControl;  // MUST be change

	glm::vec3 SRD;

	glm::vec3 Accel;  // Only Force

	float Mass;

	uint8_t GroupId;

	glm::vec3 CurrentWaypoint;
    glm::vec3 FinalDestination;

	float Radius;

	// grid
	int CellId;
	int CellX;
	int CellZ;

    int PathPtr

	float LookLength;

	uint8_t bIsLeader;
	uint8_t bIsConnected;
};