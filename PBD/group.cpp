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

	this->position = start;
	this->short_goals = std::map<int, glm::vec3>();

	this->is_linked = false;
}

void Group::init()
{
	num_particles = map_particles.size();

	int counter = 1;

	for (auto particle : map_particles)
	{
		glm::vec3 short_range_goal_pos = particle.second->X;
		particle.second->offset = short_range_goal_pos;
		short_goals.insert({ particle.first, short_range_goal_pos });

		counter--;
		if (counter == 0)
		{
			int particle_id = particle.first;
			set_leader(particle_id);
		}
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
	//particle->color = glm::vec3(1, 1, 1);
	particle->goal = particle->final_goal;
	leader_id = id;
	leader = particle;
	position = leader->X;
}

void Group::set_center()
{
	glm::vec3 center = VEC_ZERO;
	
	for (auto p : map_particles)
	{
		center += p.second->X;
	}

	center /= num_particles;

	select_leader_closest(center);
}

void Group::select_leader_closest(glm::vec3 pos)
{
	float min = distance(leader->X, pos);
	int new_id = leader_id;

	// pos와 가장 가까운 파티클이 리더
	for (auto particle : map_particles)
	{
		Particle* p = particle.second;

		float dist = distance(p->X, pos);
		
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

void Group::update_start_end()
{
	start = path[0].pos;
	end = path[path_counter - 1].pos;
}

void Group::update_short_range_goal()
{
	// 그룹의 미래위치 : Leader 현재 위치
	// 그룹의 과거위치 : group 현재 위치
	// Leader pos - group pos = 이동한 방향
	glm::vec3 diff = leader->X - position;

	// 파티클들이 이동할 거리 갱신
	for (auto particle : map_particles)
	{
		particle.second->offset += diff;
	}

	// 위치 갱신
	position = leader->X;
}

void Group::update_short_range_goal(glm::vec3 vel)
{
	// 파티클들이 이동할 거리 갱신
	for (auto particle : map_particles)
	{
		particle.second->offset += vel;
	}
}

void Group::update_path()
{
	// CORE Velocity
	glm::vec3 goal = path[path_ptr].pos;

	// 현재 위치에서 goal 까지 거리
	glm::vec3 velocity = goal - position;
	float dist = norm(velocity);

	if (dist != 0)
	{
		float V_pref = dist > V_PREF_ACCEL ? V_PREF_ACCEL : dist;
		float blending = leader->is_link ? 0.5f : 0.1f;

		velocity /= dist;
		velocity *= V_pref;

		velocity = blending * leader->V + (1.0f - blending) * velocity;
		velocity *= MS_PER_UPDATE;

		update_short_range_goal(velocity);

		position += velocity;
	}

	if (dist < DENSITY)
	{
		if (path_ptr < path_counter - 1)
			path_ptr++;
	}
}