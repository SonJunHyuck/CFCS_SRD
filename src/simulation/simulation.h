#pragma once

#include <vector>

#include "common.h"

class Constraint;
class Particle;
class PathPlanner;
class Station;

class Simulation
{
public:
	Simulation(int num_particles, int num_constraints);
	~Simulation();

	void Update();

public:
	uint64_t StepNo { 1 };

	// Simulation Parameters
	uint8_t IterateCount { 2 };
	bool bIsAvoidance = true;
	bool bIsFormation = true;
	bool bIsFTL = false;
	bool bIsSteering = true;
	bool bIsCohesion = true;
	bool bSimulate = false;

private:
	// NumGroups -> Group 1 Agents -> Group 2 Agents, ...m Group N Agents; -> NumAgents
	int NumGroups;  // MUST : input
	std::vector<class Group> Groups;
	
	class Grid GridField;

	void InitAgentDelta();
	void CalcStiffness(int n);
	void UpdatePredictedPosition();
	void ProjectConstraints();

	void PathFinding();
	void CalcPredictedPosition();
	void UpdateLocalInformation();
	void TriggerCollisionAvoidance();
	void TriggerPenetrateAvoidance();
	void UpdateFinalPosition();
};