#pragma once

#include <vector>

#include "common.h"

CLASS_PTR(Simulation);
class Simulation
{
public:
	Simulation(const std::vector<uint8_t>& InNumGroups, const std::vector<uint32_t>& InNumAgents);
	~Simulation();

	void Update();

public:
	uint64_t StepNo { 1 };

	// Simulation Parameters
	uint8_t IterateCount { 2 };
	bool bIsAvoidance = true;
	bool bIsFormation = true;
	//bool bIsFTL = false;
	bool bIsSteering = true;
	bool bIsCohesion = true;
	bool bIsSimulate = false;

private:
	// NumGroups -> Group 1 Agents -> Group 2 Agents, ...m Group N Agents; -> NumAgents
	uint8_t NumGroups;  // MUST : input
	std::vector<class Group> Groups;

	uint32_t NumAgents;
	std::vector<class Agent> Agents;
	
	class Grid GridField;

	float CollisionConstraintStiffness;

	void Init();  // Agent Position

	void InitAgentDelta();
	void CalcStiffness(int n);
	void UpdatePredictedPosition();

	void PathFinding();
	void CalcPredictedPosition();
	void UpdateLocalInformation();
	void TriggerAvoidanceConstraint();
	void TriggerCollisionConstraint();
	void TriggerSRDConstraint();
	void UpdateFinalPosition();
};