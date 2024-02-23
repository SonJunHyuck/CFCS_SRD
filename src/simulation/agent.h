#pragma once

#include "../common.h"

CLASS_PTR(Agent);
class Agent
{
private:
	Agent(const uint32_t& InAgentId, const uint8_t& InGroupId) : Id(InAgentId), GroupId(InGroupId) { }
	
	void Init();

	friend class AgentFactory;

public:
	~Agent() {}
	
	const uint32_t Id;  // Get
	const uint8_t GroupId;  // Get

	float Mass;  // Get
	float InverseMass;  // Get

	float Radius;  // Get

    float PreferedSpeed;

    glm::vec3 Position;
    glm::vec3 PredictedPosition;

    glm::vec3 Velocity;
    glm::vec3 PreviousVelocity;

    glm::vec3 DeltaPosition;
    uint16_t DeltaPositionCounter;

	glm::vec3 SRD;  // private

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
	static Agent Create(const uint32_t& InAgentId, const uint8_t& InGroupId);
};