#pragma once

#include "../common.h"

class Agent
{
private:
	Agent(const uint32_t& InAgentId, const uint8_t& InGroupId) : Id(InAgentId), GroupId(InGroupId) { }
	~Agent() {}
	
	void Init();

	friend class AgentFactory;

public:
	const uint32_t Id;
	const uint8_t GroupId;

	float Mass;
	float InverseMass;

	float Radius;

    glm::vec3 Position;
    glm::vec3 PredictedPosition;

    glm::vec3 Velocity;
    glm::vec3 PreviousVelocity;
    float PreferedSpeed;

    glm::vec3 DeltaPosition;
    int DeltaPositionCounter;

	glm::vec3 SRD;

	glm::vec3 CurrentWaypoint;
    glm::vec3 FinalDestination;

	// grid
	int CellId;
	int CellX;
	int CellZ;

	bool bIsConnected;

	void PlanVelocity();
	void CorrectPosition();
};

class AgentFactory
{
	public:
	static Agent CreateAgent(const uint32_t& InAgentId, const uint8_t& InGroupId);
};