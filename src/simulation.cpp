#include "simulation.h"
#include "simulation/agent.h"
#include "simulation/group.h"
#include "simulation/grid.h"
#include "simulation/constraint.h"

Simulation::Simulation(int num_particles, int num_constraints)
{
	CollisionConstraintStiffness = 0.22f;
	this->num_particles = IS_SINGLE ? num_particles / GROUP_COUNT : num_particles;
	this->num_particles_divided = num_particles / GROUP_COUNT;
	this->particles = (Particle**)malloc(sizeof(void*) * num_particles);

	this->grid = new Grid(num_particles, CELL_SIZE, glm::vec3(GRID_MIN_X, 0, GRID_MIN_Z), glm::vec3(GRID_MAX_X, 0, GRID_MAX_Z));

	this->num_groups = IS_SINGLE ? 1 : GROUP_COUNT;
	this->groups = (Group**)malloc(sizeof(void*) * num_groups);

	this->stations = std::vector<Station>();
	this->start = GetSeconds();
}

Simulation::~Simulation() {
	for (int i = 0; i < num_particles; i++) 
	{
		delete particles[i];
	}
	for (int i = 0; i < num_constraints; i++) 
	{
		delete constraints[i];
	}

	if (walls != NULL) {
		for (int i = 0; i < num_walls; i++) {
			delete walls[i];
		}
	}

	int trig_len = 1 + (num_particles * (num_particles + 1) / 2);
	for (int i = 0; i < num_particles; i++) {
		for (int j = 0; j < num_particles; j++) {
			if (i < j) {
				int r = i;
				int c = j;
				int t_idx = (num_particles * r) + c - (r * (r + 1) * 0.5);
				if (collision_upper_trig_arr != NULL) {
					delete collision_upper_trig_arr[t_idx];
				}
				if (powerlaw_upper_trig_arr != NULL) {
					delete powerlaw_upper_trig_arr[t_idx];
				}
				if (stability_upper_trig_arr != NULL) {
					delete stability_upper_trig_arr[t_idx];
				}
			}
		}
	}

	free(constraints);
	free(particles);
	free(collision_upper_trig_arr);
	free(powerlaw_upper_trig_arr);
	delete planner;
	delete grid;
}

void Simulation::InitAgentDelta()
{
	for(Agent& IterAgent : Agents)
	{
		IterAgent.DeltaPosition = VEC_ZERO;
		IterAgent.DeltaPositionCounter = 0;
	}
}

void Simulation::CalcStiffness(int n)
{
    CollisionConstraintStiffness = 1.0f - powf(1.0f - CollisionConstraintStiffness, (1.0f / n));
}

void Simulation::UpdatePredictedPosition()
{
	for(Group& group : Groups)
	{
		group.CorrectAgentPosition();
	}
}

void Simulation::ProjectConstraints()
{
	// init to 0
	InitAgentDelta();

	for (Agent &IterAgent : Agents)
	{
		uint8_t SearchRange = 2;
		std::vector<uint32_t> Neighbors = GridField.GetNeighborAgents(IterAgent, SearchRange);

		for (uint32_t IterNeighborId : Neighbors)
		{
			if (IterAgent.Id < IterNeighborId)
			{
				Agent& OutGuestAgent = Agents[IterNeighborId];
				CollisionConstraint(IterAgent, OutGuestAgent, CollisionConstraintStiffness);
			}
		}
	}
	UpdatePredictedPosition();

	// SRD constraints
	for (Agent &IterAgent : Agents)
	{
		SRDConstraint(IterAgent);
	}
	UpdatePredictedPosition();
}

#pragma region Scheme
void Simulation::PathFinding()
{
	for(Group& IterGroup : Groups)
	{
		IterGroup.FollowPath();
	}
}
void Simulation::CalcPredictedPosition()
{
	for(Group& IterGroup : Groups)
	{
		IterGroup.PlanAgentVelocity();
	}
}
void Simulation::UpdateLocalInformation()
{	
	GridField.Update(Agents);
}
void Simulation::TriggerCollisionAvoidance()
{
	InitAgentDelta();

	for (Agent &OutAgent : Agents)
	{
		uint8_t SearchRange = 2;
		std::vector<uint32_t> Neighbors = GridField.GetNeighborAgents(OutAgent, SearchRange);

		for (uint32_t IterNeighborId : Neighbors)
		{
			if (OutAgent.Id < IterNeighborId)
			{
				Agent& OutGuestAgent = Agents[IterNeighborId];
				AvoidConstraint(OutAgent, OutGuestAgent);
			}
		}
	}

	// predict(Particle) <- correction(Constraint) 
	UpdatePredictedPosition();
}
void Simulation::TriggerPenetrateAvoidance()
{
	for (int i = 1; i < IterateCount; i++)
	{
		CalcStiffness(i);
		ProjectConstraints();  // 4.2 (short)
	}
}
void Simulation::UpdateFinalPosition()
{
    // 5. Update Velocity
	for(Agent& IterAgent : Agents)
	{
		glm::vec3 DummyPosition = IterAgent.Position;
		glm::vec3 DeltaPos = IterAgent.PredictedPosition - IterAgent.Position;
		
		IterAgent.Velocity = DeltaPos / DELTA_TIME;

		// if (bIsFTL)
		// {
		// 	// Follow The Leader
		// 	glm::vec3 ftl = glm::vec3(0, 0, 0);

		// 	glm::vec3 d_next = Groups[IterAgent.GroupId].Leader.DeltaPosition;
		// 	float s_damping = 0.8f;

		// 	ftl = (groups[particles[i]->group_id]->leader->Delta_x * s_damping) / time_step;

		// 	particles[i]->V += ftl;
		// }

		if (bIsSteering)
		{
			// Steering Behavior
			float Speed = glm::length(IterAgent.Velocity);
			glm::vec3 DesiredVelocity = (IterAgent.SRD - IterAgent.Position) / DELTA_TIME;
			glm::vec3 Steering = (DesiredVelocity - IterAgent.Velocity);
			ClampVec3(Steering, MAX_ACCEL * DELTA_TIME);

			IterAgent.Velocity += Steering;

			ClampVec3(IterAgent.Velocity, Speed);
		}

		if (bIsCohesion)
		{
			// XSPH viscosity 4.3
			float h = 7.0f;
			float c = 217.0f;
			float weight = 0;
			glm::vec3 cohesion = glm::vec3(0, 0, 0);

			for(Agent& IterAgent2 : Agents)
			{
				if(IterAgent.GroupId == IterAgent2.GroupId)
				{
					float r = glm::distance(IterAgent.Position, IterAgent2.Position);
					if (r <= h && r >= 0)
					{
						weight = (c * pow((h * h - r * r), 3)) / (double)(64.0f * _M_PI * pow(h, 9));

						cohesion += (IterAgent.Velocity - IterAgent2.Velocity) * weight;
					}
				}
			}

			IterAgent.Velocity -= cohesion;
		}

        // Final Decision
		IterAgent.Position = IterAgent.PredictedPosition;
	}
}
#pragma endregion

void Simulation::Update()
{
    // 0. 
    PathFinding();

	// 1.
	CalcPredictedPosition();

	// 2. searching neighboring
    UpdateLocalInformation();

	// 3. long_range constraint (4.4, 4.5)
	TriggerCollisionAvoidance();

	// 4. Short range Destination
    TriggerPenetrateAvoidance();

    // 5. Real Translate Agent Position
	UpdateFinalPosition();

	StepNo++;
}