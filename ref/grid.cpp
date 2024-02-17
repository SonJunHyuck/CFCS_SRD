#include "grid.h"

#include "common.h"
#include "particle.h"

Grid::Grid(int num_particles, float dummy_cell_size, glm::vec3 min_, glm::vec3 max_)
{
	this->num_particles = num_particles;
	this->cell_size = dummy_cell_size;
	this->min_ = min_;
	this->max_ = max_;
	this->num_cells = (max_.x - min_.x) * (max_.z - min_.z);
	this->num_cols = (max_.x - min_.x) / cell_size;
	this->num_rows = (max_.z - min_.z) / cell_size;
	this->grid_counters = (int*)malloc(num_cells * (sizeof(int)));
	for (int i = 0; i < num_cells; i++)
	{
		this->grid_counters[i] = 0;
	}
	this->grid_cells = (int**)malloc(num_cells * (sizeof(int*)));
	for (int i = 0; i < num_cells; i++)
	{
		int* particle_indices = (int*)malloc(max_per_cell * (sizeof(int)));
		for (int j = 0; j < max_per_cell; j++)
		{
			particle_indices[j] = 0;
		}
		this->grid_cells[i] = particle_indices;
	}
	this->num_threads = min(BLOCK_SIZE, num_particles);
	this->num_blocks = iDivUp(num_particles, this->num_threads);
	this->grid_safty = (int*)malloc(num_cells * (sizeof(int)));
}
Grid::~Grid() 
{
	free(grid_counters);
	for (int i = 0; i < num_cells; i++) {
		free(grid_cells[i]);
	}
	free(grid_cells);
}

void Grid::update_stability(Particle** particles) // this is a kernel function
{
	// reset\update grid counters
	for (int i = 0; i < num_cells; i++)
	{
		grid_counters[i] = 0;
		for (int j = 0; j < max_per_cell; j++)
		{
			grid_cells[i][j] = K_NOT_USED;
		}
	}

	// adding particles to grid
	for (int i = 0; i < num_particles; i++)
	{
		glm::vec3 X = particles[i]->X;
		int x = (X.x - min_.x) / cell_size;
		int z = (X.z - min_.z) / cell_size;
		int cell_id = z * num_rows + x;
		particles[i]->cell_id = cell_id;
		particles[i]->cell_x = x;
		particles[i]->cell_z = z;
		int tmp = grid_counters[cell_id];
		grid_cells[cell_id][tmp] = i;
		grid_counters[cell_id] += 1;
	}
}

void Grid::update(Particle** particles) // this is a kernel function
{
	int empty = -2;

	// reset\update grid counters
	for (int i = 0; i < num_cells; i++)
	{
		grid_counters[i] = 0;
		grid_safty[i] = empty;

		for (int j = 0; j < max_per_cell; j++)
		{
			grid_cells[i][j] = K_NOT_USED;
		}
	}

	// adding particles to grid
	for (int i = 0; i < num_particles; i++)
	{
		glm::vec3 X = particles[i]->X_pred;
		int x = (X.x - min_.x) / cell_size;
		int z = (X.z - min_.z) / cell_size;
		int cell_id = z * num_rows + x;
		particles[i]->cell_id = cell_id;
		particles[i]->cell_x = x;
		particles[i]->cell_z = z;
		int tmp = grid_counters[cell_id];
		grid_cells[cell_id][tmp] = i;
		grid_counters[cell_id] += 1;

		// to search particle mixed zone;
		grid_safty[cell_id] =
			(grid_safty[cell_id] == empty || grid_safty[cell_id] == particles[i]->group_id) ? 
			particles[i]->group_id : MIXED_AREA;
	}
}