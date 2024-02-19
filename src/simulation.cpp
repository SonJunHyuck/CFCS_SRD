#include "simulation.h"
#include "simulation/agent.h"
#include "simulation/group.h"
#include "simulation/grid.h"
#include "simulation/constraint.h"

Simulation::Simulation(int num_particles, int num_constraints)
{
	this->num_particles = IS_SINGLE ? num_particles / GROUP_COUNT : num_particles;
	this->num_particles_divided = num_particles / GROUP_COUNT;
	this->particles = (Particle**)malloc(sizeof(void*) * num_particles);

	this->num_constraints = 0;
	this->constraints = NULL;
	this->collision_upper_trig_arr = NULL;
	this->powerlaw_upper_trig_arr = NULL;
	this->friction_constraint_stiffness = 0.22f;

	this->grid = new Grid(num_particles, CELL_SIZE, glm::vec3(GRID_MIN_X, 0, GRID_MIN_Z), glm::vec3(GRID_MAX_X, 0, GRID_MAX_Z));
	this->stability_grid = new Grid(num_particles, CELL_SIZE, glm::vec3(GRID_MIN_X, 0, GRID_MIN_Z), glm::vec3(GRID_MAX_X, 0, GRID_MAX_Z));

	this->planner = new PathPlanner(num_particles, this->particles);

	this->num_walls = 0;
	this->walls = NULL;

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
    for (int i = 0; i < num_particles; i++)
	{
		particles[i]->Delta_x = VEC_ZERO;
		particles[i]->Delta_x_ctr = 0;
	}
}

void Simulation::CalcStiffness(int n)
{
    friction_constraint_stiffness = 1.0f - powf(1.0f - friction_constraint_stiffness, (1.0f / n));
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

	// friction constraints (projection)
	for (int i = 0; i < num_particles; i++) {
		// iterate over adjacent cells
		for (int x = -1; x <= 1; x++) {
			int cur_x = particles[i]->cell_x + x;
			if (cur_x >= 0 && cur_x < grid->num_cols) {
				for (int z = -1; z <= 1; z++) {
					int cur_z = particles[i]->cell_z + z;
					if (cur_z >= 0 && cur_z < grid->num_rows) {
						int cell_id = particles[i]->cell_id + x + (z * grid->num_rows);
						if (grid->grid_counters[cell_id] > 0) {
							for (int idx = 0; idx < grid->grid_counters[cell_id]; idx++) {
								int j = grid->grid_cells[cell_id][idx];
								if (i < j) // so only do collision once
								{
									int t_idx = (num_particles * i) + j - (i * (i + 1) * 0.5);
									collision_upper_trig_arr[t_idx]->project(particles);
								}
							}
						}
					}
				}
			}
		}
	}

	// traverse friction constraints to accumalte deltas
	for (int i = 0; i < num_particles; i++)
	{
		// iterate over adjacent cells
		for (int x = -1; x <= 1; x++)
		{
			int cur_x = particles[i]->cell_x + x;
			if (cur_x >= 0 && cur_x < grid->num_cols)
			{
				for (int z = -1; z <= 1; z++)
				{
					int cur_z = particles[i]->cell_z + z;
					if (cur_z >= 0 && cur_z < grid->num_rows)
					{
						int cell_id = particles[i]->cell_id + x + (z * grid->num_rows);
						if (grid->grid_counters[cell_id] > 0)
						{
							for (int idx = 0; idx < grid->grid_counters[cell_id]; idx++)
							{
								int j = grid->grid_cells[cell_id][idx];
								if (i < j) // so only do collision once
								{
									int t_idx = (num_particles * i) + j - (i * (i + 1) * 0.5);
									if (collision_upper_trig_arr[t_idx]->active)
									{
										for (int ctr = 0;
											ctr < collision_upper_trig_arr[t_idx]->num_particles;
											ctr++)
										{
											int p_idx =
												collision_upper_trig_arr[t_idx]->indicies[ctr];
											particles[p_idx]->Delta_x.x +=
												collision_upper_trig_arr[t_idx]->delta_X[ctr].x;
											particles[p_idx]->Delta_x.z +=
												collision_upper_trig_arr[t_idx]->delta_X[ctr].z;
											particles[p_idx]->Delta_x_ctr++;
										}
										collision_upper_trig_arr[t_idx]->active = false;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	update_predicted_position();

	// wall + ground + mesh constraints
	for (int i = 0; i < num_constraints; i++) 
	{
		constraints[i]->project(particles);
		if (constraints[i]->active) 
		{
			for (int j = 0; j < constraints[i]->num_particles; j++) 
			{
				int idx = constraints[i]->indicies[j];
				particles[idx]->Delta_x.x += constraints[i]->delta_X[j].x;
				particles[idx]->Delta_x.z += constraints[i]->delta_X[j].z;
				particles[idx]->Delta_x_ctr++;
			}
			constraints[i]->active = false;
		}
	}
	update_predicted_position();
}

#pragma region Scheme
void Simulation::PathFinding()
{
	for (int i = 0; i < NumGroups; i++)
	{
		Groups[i]->FollowPath();
	}
}
void Simulation::CalcPredictedPosition()
{
	for (int i = 0; i < NumGroups; i++)
	{
		Groups[i]->PlanAgentVelocity();
	}
}
void Simulation::UpdateLocalInformation()
{	
	for(Group group : Groups)
	{
		GridField.Update(group.Agents);
	}
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
				ConstraintAvoide(OutAgent, OutGuestAgent);
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
	for (int i = 0; i < num_particles; i++)
	{
		particles[i]->dummy_X = particles[i]->X;
		float dx = particles[i]->X_pred.x - particles[i]->X.x;
		float dz = particles[i]->X_pred.z - particles[i]->X.z;

		particles[i]->V.x = dx / time_step;
		particles[i]->V.z = dz / time_step;

		if (FTL)
		{
			// Follow The Leader
			glm::vec3 ftl = glm::vec3(0, 0, 0);

			glm::vec3 d_next = groups[particles[i]->group_id]->leader->Delta_x;
			float s_damping = 0.8f;

			ftl = (groups[particles[i]->group_id]->leader->Delta_x * s_damping) / time_step;

			particles[i]->V += ftl;
		}

		if (STEERING)
		{
			// Steering Behavior
			float v_length = norm(particles[i]->V);
			glm::vec3 desired_V = (particles[i]->offset - particles[i]->X) / time_step;
			glm::vec3 steering = (desired_V - particles[i]->V);
			clamp(steering, MAX_ACCEL * time_step);

			particles[i]->V += steering;

			clamp(particles[i]->V, v_length);
		}

		if (COHESION)
		{
			// XSPH viscosity 4.3
			float h = 7.0f;
			float c = 217.0f;
			float weight = 0;
			glm::vec3 cohesion = glm::vec3(0, 0, 0);

			for (int j = 0; j < num_particles; j++)
			{
				if (particles[i]->group_id == particles[j]->group_id)
				{
					float r = distance(particles[i]->X, particles[j]->X);

					if (r <= h && r >= 0)
					{
						weight = (c * pow((h * h - r * r), 3)) / (double)(64.0f * _M_PI * pow(h, 9));

						cohesion += (particles[i]->V - particles[j]->V) * weight;
					}
				}
			}

			particles[i]->V -= cohesion;
		}

        // Final Decision
		particles[i]->X = particles[i]->X_pred;
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

	step_no++;
}