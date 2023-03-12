#include "group.h"
#include "utility.h"

Group::Group() {};
Group::Group(int id, glm::vec3 start, glm::vec3 end)
{
	this->id = id;
	this->start = start;
	this->end = end;

	this->path = std::vector<Station>();
	this->path_ptr = 0;
	this->path_counter = 0;

	this->pos = start;
	this->short_goals = std::vector<glm::vec3>();

	this->is_linked = false;
}

void Group::init_group()
{
	num_particles = map_particles.size();

	int counter = 1;

	for (auto particle : map_particles)
	{
		glm::vec3 short_range_goal_pos = particle.second->X;
		particle.second->offset = short_range_goal_pos;
		short_goals.push_back(short_range_goal_pos);

		counter--;
		if (counter == 0)
		{
			int particle_id = particle.first;
			set_leader(particle_id);
		}
	}
}

void Group::init_particles_goal()
{
	if (!IS_FORMATION)
		return;

	glm::vec3 goal_pos = path[0].pos;

	for (auto particle : map_particles)
	{
		particle.second->goal = goal_pos;
	}
}

void Group::set_leader(int id)
{
	auto particle = map_particles[id];

	if (leader)
	{
		leader->color = particle->color;
		leader->goal = leader->offset;
		leader->is_leader = false;
		//std::cout << "existing leader" << std::endl;
	}

	particle->is_leader = true;
	particle->color = glm::vec3(1, 1, 1);
	particle->goal = particle->final_goal;
	leader_id = id;
	leader = particle;
	pos = leader->X;
}

void Group::set_center()
{
	glm::vec3 center = VEC_ZERO; //  = map_particles.begin()->second->X;
	
	for (auto p : map_particles)
	{
		center += p.second->X;
	}

	center /= num_particles;

	glm::vec3 diff = end - start;
	start = center;
	pos = center;
	end = start + diff;
}

void Group::re_select_leader()
{
	float min = distance(leader->X, path[path_ptr].pos);
	int new_id = leader_id;

	// 파티클들이 이동할 거리 갱신
	for (auto particle : map_particles)
	{
		Particle* p = particle.second;
		float dist = distance(p->X, path[path_ptr].pos);
		
		if (min - dist > 0.1f)
		{
			min = dist;
			new_id = particle.first;
		}
	}

	if (leader_id != new_id)
	{
		set_leader(new_id);
		
		std::cout << "Switch Leader : " << new_id << std::endl;
	}
}

void Group::update_short_range_goal()
{
	// 그룹의 미래위치 : Leader 현재 위치
	// 그룹의 과거위치 : group 현재 위치
	// Leader pos - group pos = 이동한 방향
	glm::vec3 offset = leader->X - pos;

	// 파티클들이 이동할 거리 갱신
	for (auto particle : map_particles)
	{
		particle.second->offset += offset;
	}

	// 위치 갱신
	pos = leader->X;
}