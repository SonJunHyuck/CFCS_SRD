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

// normalize + clamping
void PathPlanner::calc_velocity(const int& particle_id)
{
	Particle* p = this->particles[particle_id];
	Group* g = map_groups[p->group_id];

	float blending = p->is_link ? 1.0f : 0.5f;
	glm::vec3 pred = p->offset - p->X;
	
	this->velocity_buffer[particle_id] = blending * pred + (1.0f - blending) * (p->X_pred * p->V_pref);

	float length = norm(velocity_buffer[particle_id]);

	if (length != 0)
	{
		float V_pref = length > p->V_pref ? p->V_pref : length;

		this->velocity_buffer[particle_id] /= length;  // normalize
		this->velocity_buffer[particle_id] *= V_pref;
	}
}