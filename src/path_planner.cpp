#include "path_planner.h"

#include "common.h"
#include "particle.h"
#include "group.h"

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
void PathPlanner::calc_velocity(const int& particle_id)
{
	Particle* p = this->particles[particle_id];
	Group* g = map_groups[p->group_id];

	float blending = p->is_link ? 1.0f : 0.5f;
	glm::vec3 pred = p->offset - p->X;

	// ���� ��ġ���� goal ���� �Ÿ�
	//this->velocity_buffer[particle_id] = p->offset - p->X;
	//this->velocity_buffer[particle_id] = p->is_link ? p->offset - p->X : p->V;
	
	//if (IS_FORMATION)
		this->velocity_buffer[particle_id] = blending * pred + (1.0f - blending) * (p->X_pred * p->V_pref);
	//else
		//this->velocity_buffer[particle_id] = p->final_goal - p->X;

	float length = norm(velocity_buffer[particle_id]);

	// ������������ �������� ���ߴٸ�? -> Normalize
	if (length != 0)
	{
		// ���������� ���� ���̱�
		float V_pref = length > p->V_pref ? p->V_pref : length;

		this->velocity_buffer[particle_id] /= length;  // normalize
		this->velocity_buffer[particle_id] *= V_pref;
	}
}