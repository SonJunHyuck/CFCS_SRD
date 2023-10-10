#pragma once
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "particle.h"
#include "station.h"

class Group
{
public:
	int id;

	// status
	glm::vec3 position;
	glm::vec3 velocity;

	// station 
	glm::vec3 start;
	glm::vec3 end;

	int station_start_id;
	int station_end_id;

	// path
	std::vector<Station> path;
	int path_ptr;
	int path_counter;
	glm::vec3 color;

	// short_rage_goal
	std::map<int, glm::vec3> short_goals;

	// particle
	int num_particles;
	std::map<int, Particle*> map_particles;  // int array

	// leader
	int leader_id;
	Particle* leader;
	bool is_linked;

	Group();
	Group(int id, glm::vec3 start, glm::vec3 end);
	Group(int id, glm::vec3 start, glm::vec3 end, glm::vec3 color);
	void init();

	void set_leader(int id);

	void set_center();

	void select_leader_closest(glm::vec3 pos);

	void update_start_end();

	void update_short_range_goal();

	void update_short_range_goal(glm::vec3 vel);

	void update_path();
};