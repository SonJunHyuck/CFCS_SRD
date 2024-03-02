#pragma once

#include "../common.h"

// extern float DELTA_TIME;

CLASS_PTR(Agent);
class Agent
{
private:
	Agent(const uint32_t& InAgentId, const uint8_t& InGroupId) : Id(InAgentId), GroupId(InGroupId) { }
	
	void Init(const float InMass, const float InRadius, const float InPreferedSpeed, const glm::vec3& InPosition);

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

	glm::vec3 SRD;  // Short Range Destination

	glm::vec3 CurrentWaypoint;
    glm::vec3 FinalDestination;

	// grid
	uint32_t CellId;
	uint32_t CellX;
	uint32_t CellZ;

	bool bIsConnected;

	void PlanVelocity();
	void DetermineBehavior(const int8_t InState);
	void CorrectPosition();
};

class AgentFactory
{
	public:
	static Agent Create(const uint32_t& InAgentId, const uint8_t& InGroupId, const float InMass, const float InRadius, const float InPreferedSpeed, const glm::vec3& InPosition);
};