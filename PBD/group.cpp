#include "group.h"
#include "utility.h"

Group::Group() {};
Group::Group(int id, glm::vec3 start, glm::vec3 end)
{
	this->id = id;
	this->start = start;
	this->end = end;

	this->path = std::vector<Station>();
	this->path_ptr = 1;
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

	particle->is_leader = true;

	leader_id = id;
	leader = particle;
	//particle->color = glm::vec3(1 ,1 ,1);
	pos = leader->X;
}

void Group::update_short_range_goal()
{
	// �׷��� �̷���ġ : Leader ���� ��ġ
	// �׷��� ������ġ : group ���� ��ġ
	// Leader pos - group pos = �̵��� ����
	glm::vec3 offset = leader->X - pos;

	// ��ġ ����
	pos = leader->X;

	// ������ �� ����
	//for (auto& g_pos : short_goals)
	//{
	//	g_pos += offset;
	//}

	// ��ƼŬ���� �̵��� �Ÿ� ����
	for (auto particle : map_particles)
	{
		particle.second->offset += offset;
	}
}