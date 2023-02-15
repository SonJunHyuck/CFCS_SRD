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

	glm::vec3 start;
	glm::vec3 end;

	glm::vec3 pos;
	std::vector<glm::vec3> short_goals;

	int station_start_id;
	int station_end_id;

	std::map<int, Particle*> map_particles;
	int num_particles;
	int leader_id;
	Particle* leader;

	std::vector<Station> path;
	int path_ptr;  // 일괄 조정용 변수, 모든 파티클이 이 ptr을 지나야 다음 ptr로 이동 가능
	int path_counter;

	bool is_linked;

	Group();
	Group(int id, glm::vec3 start, glm::vec3 end);

	void init_group();

	void init_particles_goal();

	void set_leader(int id);

	void update_short_range_goal();
};