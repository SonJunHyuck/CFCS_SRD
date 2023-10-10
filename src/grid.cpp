#include "grid.h"

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

// 현재 위치 기반
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

bool Grid::is_colliding_stability(Particle** particles, int i, int j) const {

	glm::vec3 X = particles[i]->X;
	int xi = (X.x - min_.x) / cell_size;
	int zi = (X.z - min_.z) / cell_size;
	int cell_id_i = zi * num_rows + xi;
	X = particles[j]->X;
	int xj = (X.x - min_.x) / cell_size;
	int zj = (X.z - min_.z) / cell_size;
	int cell_id_j = zj * num_rows + xj;
	int is_x_neighbour = xi - xj;
	int is_z_neighbour = zi - zj;
	bool res = is_x_neighbour >= -3 && is_x_neighbour <= 3 &&
		is_z_neighbour >= -3 && is_z_neighbour <= 3;
	return res;
}

bool Grid::is_colliding(Particle** particles, int i, int j) const {

	glm::vec3 X = particles[i]->X_pred;
	int xi = (X.x - min_.x) / cell_size;
	int zi = (X.z - min_.z) / cell_size;
	int cell_id_i = zi * num_rows + xi;

	X = particles[j]->X_pred;
	int xj = (X.x - min_.x) / cell_size;
	int zj = (X.z - min_.z) / cell_size;
	int cell_id_j = zj * num_rows + xj;

	int is_x_neighbour = xi - xj;
	int is_z_neighbour = zi - zj;

	// 이웃한 3x3에 위치하지 않는다면 충돌할 일이 없다.
	bool res = is_x_neighbour >= -3 && is_x_neighbour <= 3 &&
		is_z_neighbour >= -3 && is_z_neighbour <= 3;

	return res;
}

// Pred_X Based + grid 에 해당하는 각 cell에 어떤 파티클이 몇개나 존재하는지 update
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
		int tmp = grid_counters[cell_id];  // 해당 counter 칸에 몇개의 파티클이 있는지
		grid_cells[cell_id][tmp] = i;  // 해당 grid_cell에 Particle들의 index를 저장
		grid_counters[cell_id] += 1;  // counter increment

		// 비어있지 않고, 다른 그룹의 particle이 이미 점령했을 때
		grid_safty[cell_id] =
			(grid_safty[cell_id] == empty || grid_safty[cell_id] == particles[i]->group_id) ? 
			particles[i]->group_id : MIXED_AREA;
	}
}

// Station 생성 (Grid 형태로)
std::vector<glm::vec3> Grid::insert_station()
{
	std::vector<glm::vec3> out = std::vector<glm::vec3>();

	// Cell 크기 배수
	float multi = 3.0f;  // 3.0f;
	float new_cell_size = cell_size * multi;
	float new_cell_half = new_cell_size * 0.5f;

	int cols = (max_.x - min_.x) / new_cell_size;
	int rows = (max_.z - min_.z) / new_cell_size;

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			glm::vec3 station_pos = glm::vec3(
				min_.x + (float)i * new_cell_size + new_cell_half,
				0,
				min_.z + (float)j * new_cell_size + new_cell_half);
			out.push_back(station_pos);
		}
	}

	return out;
}