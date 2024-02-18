#include "common.h"
#include "simulation.h"
#include "constraint.h"
#include "particle.h"
#include "path_planner.h"
#include "grid.h"
#include "wall.h"
#include "group.h"
#include "station.h"

Simulation::Simulation(int num_particles, int num_constraints, float time_step, char* out_path)
{
	this->time_step = time_step;
	this->step_no = 1;

	this->num_particles = IS_SINGLE ? num_particles / GROUP_COUNT : num_particles;
	this->num_particles_divided = num_particles / GROUP_COUNT;
	this->particles = (Particle**)malloc(sizeof(void*) * num_particles);

	this->num_constraints = 0;
	this->constraints = NULL;
	this->collision_map =
		std::unordered_map<unsigned long long, Constraint*>();
	this->collision_upper_trig_arr = NULL;
	this->powerlaw_upper_trig_arr = NULL;
	this->distance_trig_arr = NULL;
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

void Simulation::calc_constraint_stiffness(int n) {
	// 1.-(1.-0.25)**(4./6)
	friction_constraint_stiffness = 1.0f - powf(1.0f - friction_constraint_stiffness, (1.0f / n));
}
 
void Simulation::project_longrange_constraints()
{
	// init to 0
	for (int i = 0; i < num_particles; i++)
	{
		particles[i]->Delta_x = VEC_ZERO;
		particles[i]->Delta_x_ctr = 0;
	}

	// grid  ڽ ϴ cell  cell ִ ƼŬ鿡  constraint
	for (int i = 0; i < num_particles; i++)
	{
		// iterate over adjacent cells ( cell Ȯ)
		for (int x = -2; x <= 2; x++)
		{
			int cur_x = particles[i]->cell_x + x;
			if (cur_x >= 0 && cur_x < grid->num_cols)
			{
				for (int z = -2; z <= 2; z++)
				{
					int cur_z = particles[i]->cell_z + z;
					if (cur_z >= 0 && cur_z < grid->num_rows)
					{
						int cell_id = particles[i]->cell_id + x + (z * grid->num_rows);
						// ش cell particle ϰ ִٸ
						if (grid->grid_counters[cell_id] > 0)
						{
							// ش cell ϴ ƼŬ  ŭ
							for (int idx = 0; idx < grid->grid_counters[cell_id]; idx++)
							{
								int j = grid->grid_cells[cell_id][idx];
								if (i < j) // so only do collision once
								{
									int t_idx = (num_particles * i) + j - (i * (i + 1) * 0.5);
									powerlaw_upper_trig_arr[t_idx]->project(particles);
								}
							}
						}
					}
				}
			}
		}
	}

	// traverse friction constraints to accumalte deltas
	for (int i = 0; i < num_particles; i++) {
		// iterate over adjacent cells
		for (int x = -2; x <= 2; x++)
		{
			int cur_x = particles[i]->cell_x + x;
			if (cur_x >= 0 && cur_x < grid->num_cols)
			{
				for (int z = -2; z <= 2; z++)
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
									if (powerlaw_upper_trig_arr[t_idx]->active)
									{
										for (int ctr = 0;
											ctr < powerlaw_upper_trig_arr[t_idx]->num_particles;
											ctr++)
										{
											int p_idx =
												powerlaw_upper_trig_arr[t_idx]->indicies[ctr];
											particles[p_idx]->Delta_x.x +=
												powerlaw_upper_trig_arr[t_idx]->delta_X[ctr].x;
											particles[p_idx]->Delta_x.z +=
												powerlaw_upper_trig_arr[t_idx]->delta_X[ctr].z;
											particles[p_idx]->Delta_x_ctr++;
										}
										powerlaw_upper_trig_arr[t_idx]->active = false;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// 5. predict(Particle) <- correction(Constraint) 
	update_predicted_position();
}

void Simulation::project_constraints()
{
	// init to 0
	for (int i = 0; i < num_particles; i++) {
		particles[i]->Delta_x.x = 0.0;
		particles[i]->Delta_x.z = 0.0;
		particles[i]->Delta_x_ctr = 0;
	}

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
									// collision_map[i * num_particles + j]->project(particles);
									int t_idx = (num_particles * i) + j - (i * (i + 1) * 0.5);
									collision_upper_trig_arr[t_idx]->project(particles);
									// powerlaw_upper_trig_arr[t_idx]->project(particles);
									// stability_upper_trig_arr[t_idx]->project(particles);
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

									/*
									if(powerlaw_upper_trig_arr[t_idx]->active)
									{
										for(int ctr=0;
											ctr<powerlaw_upper_trig_arr[t_idx]->num_particles
											;ctr++)
										{
											int
									p_idx=powerlaw_upper_trig_arr[t_idx]->indicies[ctr];
											particles[p_idx]->Delta_x.x +=
									powerlaw_upper_trig_arr[t_idx]->delta_X[ctr].x;
											particles[p_idx]->Delta_x.y +=
									powerlaw_upper_trig_arr[t_idx]->delta_X[ctr].y;
											particles[p_idx]->Delta_x_ctr++;
										}
										powerlaw_upper_trig_arr[t_idx]->active=false;
									}
									*/
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
	for (int i = 0; i < num_constraints; i++) {
		constraints[i]->project(particles);
		if (constraints[i]->active) {
			for (int j = 0; j < constraints[i]->num_particles; j++) {
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

void Simulation::project_distance_constraints()
{
	// init to 0
	for (int i = 0; i < num_particles; i++)
	{
		particles[i]->Delta_x.x = 0.;
		particles[i]->Delta_x.z = 0.;
		particles[i]->Delta_x_ctr = 0;

		//  뿭 
		if (grid->grid_safty[particles[i]->cell_id] == MIXED_AREA)
		{
			particles[i]->is_link = false;
		}
		else
		{
			particles[i]->is_link = true;
		}
	}

	// grid  ڽ ϴ cell  cell ִ ƼŬ鿡  constraint
	for (int i = 0; i < num_particles; i++)
	{
		// iterate over adjacent cells ( cell Ȯ)
		for (int x = -2; x <= 2; x++)
		{
			int cur_x = particles[i]->cell_x + x;
			if (cur_x >= 0 && cur_x < grid->num_cols)
			{
				for (int z = -2; z <= 2; z++)
				{
					int cur_z = particles[i]->cell_z + z;
					if (cur_z >= 0 && cur_z < grid->num_rows)
					{
						int cell_id = particles[i]->cell_id + x + (z * grid->num_rows);
						// ش cell particle ϰ ִٸ
						if (grid->grid_counters[cell_id] > 0)
						{
							// ش cell ϴ ƼŬ  ŭ
							for (int idx = 0; idx < grid->grid_counters[cell_id]; idx++)
							{
								int j = grid->grid_cells[cell_id][idx];
								if (i < j) // so only do collision once
								{
									int t_idx = (num_particles * i) + j - (i * (i + 1) * 0.5);
									distance_trig_arr[t_idx]->project(particles);
								}
							}
						}
					}
				}
			}
		}
	}

	// traverse friction constraints to accumalte deltas
	for (int i = 0; i < num_particles; i++) {
		// iterate over adjacent cells
		for (int x = -2; x <= 2; x++)
		{
			int cur_x = particles[i]->cell_x + x;
			if (cur_x >= 0 && cur_x < grid->num_cols)
			{
				for (int z = -2; z <= 2; z++)
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
									if (distance_trig_arr[t_idx]->active)
									{
										for (int ctr = 0;
											ctr < distance_trig_arr[t_idx]->num_particles;
											ctr++)
										{
											int p_idx =
												distance_trig_arr[t_idx]->indicies[ctr];
											particles[p_idx]->Delta_x.x +=
												distance_trig_arr[t_idx]->delta_X[ctr].x;
											particles[p_idx]->Delta_x.z +=
												distance_trig_arr[t_idx]->delta_X[ctr].z;
											particles[p_idx]->Delta_x_ctr++;
										}
										distance_trig_arr[t_idx]->active = false;
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void Simulation::do_time_step()
{

	// 4. Short range Destination
	for (int i = 1; i < (ITER_COUNT + 1); i++)
	{
		calc_constraint_stiffness(i);
		project_constraints();  // 4.2 (short)
	}
}
