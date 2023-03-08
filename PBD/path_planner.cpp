#include "path_planner.h"

PathPlanner::PathPlanner(int num_particles, Particle** particles)
{
	this->num_particles = num_particles;
	this->particles = particles;
	this->velocity_buffer = (glm::vec3*)malloc(sizeof(glm::vec3) * num_particles);

	for (int i = 0; i < num_particles; i++)
	{
		this->velocity_buffer[i] = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	map_groups = std::map<int, Group*>();
}
PathPlanner::~PathPlanner()
{
	free(velocity_buffer);
}

// normalize(now, next)
void PathPlanner::calc_pref_v_force(const int& particle_id) // returns velocity
{
	const Particle* p = this->particles[particle_id];
	glm::vec3 goal = p->goal;
	this->velocity_buffer[particle_id].x = goal.x - p->X.x;
	this->velocity_buffer[particle_id].y = goal.y - p->X.y;
	this->velocity_buffer[particle_id].z = goal.z - p->X.z;
	const float length = norm(velocity_buffer[particle_id]);

	if (length != 0)
	{
		this->velocity_buffer[particle_id].x /= length;
		this->velocity_buffer[particle_id].y /= length;
		this->velocity_buffer[particle_id].z /= length;
		this->velocity_buffer[particle_id].x *= p->V_pref;
		this->velocity_buffer[particle_id].y *= p->V_pref;
		this->velocity_buffer[particle_id].z *= p->V_pref;
	}
}

// normalize + clamping
void PathPlanner::calc_velocity(const int& particle_id) // returns velocity
{
	// CORE Velocity
	Particle* p = this->particles[particle_id];
	Group* unit_group = map_groups[p->group_id];

	if (IS_FORMATION)
	{
		p->goal = p->final_goal;
		
		if (p->is_leader)
		{
			// Virtual Pointer 갱신
			//p->goal = p->final_goal;

			// station은 std::vector로 가지고 있고, top을 계속해서 꺼내면서 final_goal에 집어넣어주기
			// unit_group->check_goal_arrive();  // p->final_goal change
			// 모든 파티클이 short-range-goal에 위치한지 파악한 후 다음 위치로 이동
		}
		else
		{
			//p->goal = p->is_link ? p->offset : p->final_goal;
		}
	}

	// 현재 위치에서 goal 까지 거리
	this->velocity_buffer[particle_id] = p->goal - p->X;
	float length = norm(velocity_buffer[particle_id]);

	// 최종목적지에 도달하지 못했다면? -> Normalize
	if (length != 0)
	{
		// 목적지에서 관성 줄이기
		float V_pref;
		V_pref = length > p->V_pref ? p->V_pref : length;

		this->velocity_buffer[particle_id] /= length;  // normalize
		this->velocity_buffer[particle_id] *= V_pref;
	}
}