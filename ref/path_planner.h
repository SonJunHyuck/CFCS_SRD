#pragma once
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Particle;
class Group;

class PathPlanner
{
	// should be constructed once for each scenerio

public:
	int num_particles;
	Particle** particles;
	glm::vec3* velocity_buffer;

	std::map<int, Group*> map_groups;

	// get current velocity, adjust predicted particle accordinfly for smoothness
	PathPlanner(int num_particles, Particle** particles);
	~PathPlanner();

	// normalize(now, next)
	void calc_pref_v_force(const int& particle_id);

	// normalize + clamping
	void calc_velocity(const int& particle_id);

};