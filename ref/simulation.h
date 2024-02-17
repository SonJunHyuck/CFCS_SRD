#pragma once
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Constraint;
class Particle;
class PathPlanner;
class Grid;
class Wall;
class Group;
class Station;

class Simulation
{
public:
	int num_particles;
	int num_particles_divided;
	int num_constraints;
	float time_step;
	Constraint** constraints;
	Particle** particles;
	PathPlanner* planner;
	Grid* grid;
	Grid* stability_grid;
	std::unordered_map<unsigned long long, Constraint*> collision_map;
	Constraint** collision_upper_trig_arr;
	Constraint** powerlaw_upper_trig_arr;
	Constraint** stability_upper_trig_arr;
	Constraint** distance_trig_arr;
	int step_no;
	float friction_constraint_stiffness;
	int num_walls;
	Wall** walls;
	int num_groups;
	Group** groups;

	std::vector<Station> stations;

	double start = 0;
	double end;

	bool stop = true;

	Simulation(int num_particles, int num_constraints, float time_step, char* out_path);

	~Simulation();

	void calc_constraint_stiffness(int n);

	void update_predicted_position();

	void stabilization();

	void project_longrange_constraints();

	void project_constraints();

	void project_distance_constraints();

	void do_time_step();
};