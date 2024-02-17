#pragma once

#include <vector>

#include "common.h"

class Constraint;
class Particle;
class PathPlanner;
class Grid;
class Group;
class Station;

class Simulation
{
public:
	Simulation(int num_particles, int num_constraints, float time_step, char* out_path);
	~Simulation();

	uint32_t GetDividedParticleNum() { return num_particles / num_groups; }

	void InitAgentDelta();
	void CalcStiffness(int n);
	void UpdatePredictedPosition();
	void ProjectConstraints();

	void PathFinding();
	void CalcPredictedPosition();
	void UpdateLocalInformation();
	void TriggerCollisionAvoidance();
	void TriggerPenetrateAvoidance();

	void UpdatePosition();

	void Update();

public:
	float time_step;
	int step_no;

	uint32_t num_particles;
	uint32_t num_particles_divided;

	int num_constraints;
	Constraint** constraints;
	
	Particle** particles;
	PathPlanner* planner;

	Grid* grid;
	Grid* stability_grid;

	Constraint** collision_upper_trig_arr;
	Constraint** powerlaw_upper_trig_arr;
	Constraint** stability_upper_trig_arr;
	Constraint** distance_trig_arr;
	float friction_constraint_stiffness;

	int num_groups;
	Group** groups;

	std::vector<Station> stations;

	double start = 0;
	double end;

	uint8_t bStop = true;
};