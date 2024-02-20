#pragma once
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Particle;

class Grid
{
public:
	static const int max_per_cell = 10;
	int num_particles;
	int num_cells;
	int num_rows;
	int num_cols;
	float cell_size;
	glm::vec3 min_;
	glm::vec3 max_;
	int* grid_counters; // stores num of particles in each cell
	int** grid_cells;   // stores the particles indicies for each cell
						// has a maximum number of particles per cell
	int num_blocks;
	int num_threads;

	int* grid_safty;

	Grid(int num_particles, float dummy_cell_size, glm::vec3 min_, glm::vec3 max_);
	~Grid();

	void update_stability(Particle** particles);

	bool is_colliding_stability(Particle** particles, int i, int j) const;

	bool is_colliding(Particle** particles, int i, int j) const;

	void update(Particle** particles);
};