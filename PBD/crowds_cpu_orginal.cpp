// Position-Based Real-Time Simulation of Large Crowds
// Copyright (c) 2020, Tomer Weiss
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other materials
//    provided with the distribution.
// THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR  A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
// OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
// IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Original author: Tomer Weiss <http://www.cs.ucla.edu/~tweiss>

//#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GL_Window.h"

#include "utility.h"

#include "wall.h"
#include "station.h"
#include "particle.h"
#include "group.h"
#include "grid.h"
#include "path_planner.h"
#include "constraint.h"
#include "simulation.h"
#include "particle_tuple.h"

#include "Shader.h"
#include "Camera.h"
#include "Plane.h"
#include "Model.h"

GLFWwindow* window;
GL_Window* gl_window;

Shader* shader;
Shader* shader_plane;

Camera camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f));

std::string const model_path = "C:/Users/ahkff/Desktop/Tutorial/OpenGL/gltut/Tut 00 Window/Tut00/Release/sphere.obj";
Model model;
Plane plane;

bool stop = true;

bool mouse_button_down_left;
bool mouse_button_down_right;
bool mouse_button_down_center;
double last_mouse_X;
double last_mouse_Y;
double cx, cy;


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		stop = !stop;
}
void window_size_callback(GLFWwindow* window, int width, int height)
{
	gl_window->set_size(width, height);
}
static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	cx = xpos;
	cy = ypos;
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		last_mouse_X = xpos;
		last_mouse_Y = ypos;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (GLFW_PRESS == action)
			mouse_button_down_left = true;
		else if (GLFW_RELEASE == action)
			mouse_button_down_left = false;
	}

	else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (GLFW_PRESS == action)
			mouse_button_down_right = true;
		else if (GLFW_RELEASE == action)
			mouse_button_down_right = false;
	}

	else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
		if (GLFW_PRESS == action)
			mouse_button_down_center = true;
		else if (GLFW_RELEASE == action)
			mouse_button_down_center = false;
	}
}
void mouseDragging(double width, double height)
{
	if (mouse_button_down_left) {
		float fractionChangeX = static_cast<float>(cx - last_mouse_X) / static_cast<float>(width);
		float fractionChangeY = static_cast<float>(last_mouse_Y - cy) / static_cast<float>(height);
		gl_window->viewer->rotate(fractionChangeX, fractionChangeY);
	}
	else if (mouse_button_down_center) {
		float fractionChangeX = static_cast<float>(cx - last_mouse_X) / static_cast<float>(width);
		float fractionChangeY = static_cast<float>(last_mouse_Y - cy) / static_cast<float>(height);
		gl_window->viewer->zoom(fractionChangeY);
	}
	else if (mouse_button_down_right) {
		float fractionChangeX = static_cast<float>(cx - last_mouse_X) / static_cast<float>(width);
		float fractionChangeY = static_cast<float>(last_mouse_Y - cy) / static_cast<float>(height);
		gl_window->viewer->translate(-fractionChangeX, -fractionChangeY, 1);
	}
	last_mouse_X = cx;
	last_mouse_Y = cy;
}

static void error_callback(int error, const char* description)
{
	//fprintf(stderr, "Error %d: %s\n", error, description);
}

void draw_particles(Simulation* sim) {
	for (int i = 0; i < sim->num_particles; i++)
	{
		if (sim->particles[i]->is_leader)
			sim->particles[i]->color = glm::vec3(1.0f, 1.0f, 1.0f);

		drawCircle(sim->particles[i]->X.x, sim->particles[i]->X.y, 1, 15, sim->particles[i]->color);
		/*
		drawDirection(sim->particles[i]->X.x,sim->particles[i]->X.y,
				 sim->planner->velocity_buffer[i].x,sim->planner->velocity_buffer[i].y);
		*/
		//sim->particles[i]->drawArrow();

		//drawLine(sim->particles[i]->X.x, sim->particles[i]->X.y, sim->particles[i]->goal.x, sim->particles[i]->goal.y);
	}

	for (int i = 0; i < sim->num_walls; i++)
	{
		Wall* w = sim->walls[i];
		//drawGround(w->x0.x, w->x0.y, w->x1.x, w->x1.y);
		drawRect(w->center, w->width, w->height, glm::vec3(0, 0, 0));
	}

	drawGround(-1000.0, GROUND_HEIGHT + 30, 1000.0, GROUND_HEIGHT + 30);
	drawGround(-1000.0, GRID_UP_BOUND, 1000.0, GRID_UP_BOUND);
	drawGround(LEFT_BOUND_X, GROUND_HEIGHT, LEFT_BOUND_X, GROUND_HEIGHT - 1000);
	drawGround(RIGHT_BOUND_X, GROUND_HEIGHT, RIGHT_BOUND_X, GROUND_HEIGHT - 1000);

	for (int i = 0; i < sim->num_groups; i++)
	{
		//drawCircle(sim->groups[i]->pos.x, sim->groups[i]->pos.y, sim->groups[i]->center_to_edge, GROUP_SEGMENT, make_float3(1, 1, 0));

		/*for (auto pos : sim->groups[i]->virtual_points)
		{
			drawCircle(pos.x, pos.y, 1, 10, make_float3(1, 0, 1));
		}*/
	}

	/*for (int i = 0; i < sim->stations.size(); i++)
	{
		float3 color = make_float3(0, 0, 0);

		if (i < 4)
			color = make_float3(1, 1, 1);

		drawCircle(sim->stations[i].pos.x, sim->stations[i].pos.y, 1, 15, color);
	}*/

	/*float cell_size = sim->grid->cell_size;
	for (int i = 0; i < sim->grid->num_rows; i++)
	{
		for (int j = 0; j < sim->grid->num_cols; j++)
		{
			drawSquare(GRID_MIN_X + (i * cell_size), 
				GRID_MIN_Y + (j * cell_size), cell_size, make_float3(sim->grid->grid_counters[j * sim->grid->num_rows + i], 0, 0));
		}
	}*/
}

int render(Simulation* sim)
{
	int display_w, display_h;
	double prev_time = 0.0;
	double elapsed = 0.0;
	double cur_time = 0.0;
	double lag = 0.0;
	BYTE pixels[3 * WIDTH * HEIGHT];  // 여기서 다 사용

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		return 1;

	GLFWwindow* window =
		glfwCreateWindow(WIDTH, HEIGHT, "ImGui Crowd Sim", NULL, NULL);

	glfwMakeContextCurrent(window);
	glEnable(GL_DEPTH_TEST); // Depth Testing
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glfwGetWindowSize(window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	float3 clear_color = make_float3(0.827f, 0.827f, 0.827f);

	prev_time = GetSeconds();
	// Main loop
	int i = 0;
	while (!glfwWindowShouldClose(window))
	{
		glLoadIdentity();
		cur_time = GetSeconds();
		elapsed = cur_time - prev_time;
		prev_time = cur_time;
		lag += elapsed;

		/*while (lag >= MS_PER_UPDATE) {
		  lag -= MS_PER_UPDATE;
		  update(sim);
		}*/

		sim->do_time_step();  // update -> do_time_step
		//update(sim);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		draw_particles(sim);
		set_camera(display_w, display_h, 0.0 /*rotate_camera+=0.2*/);
		glfwSwapBuffers(window);
		glfwPollEvents();
		/*
				glReadPixels(0,0,display_w,display_h, GL_BGR, GL_UNSIGNED_BYTE,
		pixels); save_file(pixels,"hello",display_h,display_w); i++; if(i>10) break;
				*/
	}
	glfwTerminate();
	return 0;
}

void update(Simulation* sim)
{
	sim->do_time_step();
}

void dummy_init(Simulation* s)
{
	srand(time(NULL));

#pragma region INIT_PARTICLE & GROUP

	int i = 0;
	// Blue
	float rad = DENSITY;
	float row_init = RIGHT_BOUND_X - 400 * 0.5f;  // LEFT_BOUND_X + 3.8 * rad * 50 * 0.5;
	float height_init = -10.0f; //GROUND_HEIGHT - 30;
	int group_id = 0;

	Group* group = new Group(group_id, glm::vec2(row_init, height_init),
		glm::vec2(RIGHT_BOUND_X - 500.0f, height_init));
	s->groups[0] = group;

	// ROWS / 2
	/*int row_count = IS_SINGLE ? ROWS : ROWS / 2;

	for (int i_ = 0; i_ < row_count; i_++)
	{
		for (int j_ = 0; j_ < COLS; j_++)
		{
			float y = height_init - HEIGHT_INTERVAL * i_ + rand_interval(-0.4, 0.4) - HEIGHT_DIFF;
			float x = row_init + j_ * rad + rand_interval(-0.4, 0.4);

			Particle* particle = new Particle(glm::vec2(x, y), glm::vec2(0, 0.0), 1.0, 1.0, group_id,
				make_float3(0.0f, 0.0f, 1.0f),
				glm::vec2(RIGHT_BOUND_X - 2 * 8.2 * 50 * 0.5, y));

			particle->row_id = i_;
			particle->col_id = j_;

			s->particles[i] = particle;
			i++;
		}
	}*/

	// Circle
	int num_particles_half = (ROWS / 2) * COLS;
	float D2R = _M_PI / 180;
	float r_weight = 0;
	for (int i_ = 0; i_ < num_particles_half;)
	{
		for (int j_ = 0; j_ < 360; j_ += 12)
		{
			float radius = 10 + 3 * r_weight;
			float z = height_init + radius * sin(j_ * D2R);
			float x = row_init + radius * cos(j_ * D2R);

			Particle* particle = new Particle(glm::vec3(x, 0, z), glm::vec3(0, 0, 0.0), 1.0, 1.0, group_id,
				glm::vec3(0, 0, 1),
				glm::vec3(RIGHT_BOUND_X - 2 * 8.2 * 50 * 0.5, 0, z));

			s->particles[i] = particle;
			i++;
			i_++;
		}
		r_weight++;
	}

	if (!IS_SINGLE)
	{
		// Red
		rad = 4.5;
		height_init = -10.0f;
		row_init = LEFT_BOUND_X + 200.0f; // LEFT_BOUND_X + 200 * 0.5f;
		rad = DENSITY - 0.1f;
		group_id = 1;

		group = new Group(group_id, glm::vec2(row_init, height_init),
			glm::vec2(LEFT_BOUND_X + 500.0f, height_init));
		s->groups[1] = group;

		for (int i_ = 0; i_ < ROWS / 2; i_++)
		{
			for (int j_ = 0; j_ < COLS; j_++)
			{
				float height_weight = 0;
				height_weight = i_ > ROWS / 4 ? -MIDDLE_HOLE : MIDDLE_HOLE;

				float interval_weight = 1.2f;  // 간격 조절
				float z = height_init - HEIGHT_INTERVAL * i_ * interval_weight + rand_interval(-0.4, 0.4) + height_weight;
				float x = row_init - j_ * rad * interval_weight + rand_interval(-0.4, 0.4);
				Particle* particle = new Particle(glm::vec3(x, 0, z), glm::vec3(0, 0, 0.0), 1.0, 1.0, group_id,
					glm::vec3(1, 0, 0),
					glm::vec3(LEFT_BOUND_X + 4 * 3.2 * 50 * 0.5 + x, 0, z));


				particle->row_id = i_;
				particle->col_id = j_;

				s->particles[i] = particle;
				i++;
			}
		}
	}
	

	for (int i = 0; i < s->num_groups; i++)
	{
		Group* g = s->groups[i];

		for (int j = 0; j < s->num_particles; j++)
		{
			Particle* p = s->particles[j];

			if (g->id == p->group_id)
				g->map_particles.insert({ j, p });
		}

		g->init_group();
	}
#pragma endregion

#pragma region INIT_CONSTRAINT
	std::vector<particle_tuple*> friction_pairs = get_tuples(s->num_particles);
	int trig_len = 1 + (s->num_particles * (s->num_particles + 1) / 2);

	s->stability_upper_trig_arr =
		(Constraint**)malloc(sizeof(void*) * trig_len);
	s->collision_upper_trig_arr =
		(Constraint**)malloc(sizeof(void*) * trig_len);
	s->powerlaw_upper_trig_arr =
		(Constraint**)malloc(sizeof(void*) * trig_len);
	s->distance_trig_arr =
		(Constraint**)malloc(sizeof(void*) * trig_len);

	for (std::vector<particle_tuple*>::iterator it = friction_pairs.begin();
		it != friction_pairs.end(); ++it)
	{
		Stability_Constraint* stab = new Stability_Constraint(s, (*it)->i, (*it)->j);
		Friction_Constraint* fc = new Friction_Constraint(s, (*it)->i, (*it)->j);
		Powerlaw_Constraint* pl = new Powerlaw_Constraint(s, (*it)->i, (*it)->j);
		Distance_Constraint* ds = new Distance_Constraint(s, (*it)->i, (*it)->j);

		// 해당 tuple 전에 관계는 다 적립된 상태
		if ((*it)->i < (*it)->j)
		{
			s->collision_map[(*it)->i * s->num_particles + (*it)->j] = fc;
			int r = (*it)->i;
			int c = (*it)->j;
			int t_idx = (s->num_particles * r) + c - (r * (r + 1) * 0.5);
			s->collision_upper_trig_arr[t_idx] = fc;
			s->powerlaw_upper_trig_arr[t_idx] = pl;
			s->stability_upper_trig_arr[t_idx] = stab;
			s->distance_trig_arr[t_idx] = ds;
		}
	}
#pragma endregion

#pragma region INIT_WALL
	s->num_walls = 2;
	s->walls = (Wall**)malloc(sizeof(void*) * s->num_walls);
	s->walls[0] =
		new Wall(glm::vec3(-170, 0, GROUND_HEIGHT - 20),
			glm::vec3(150., 0, GROUND_HEIGHT - 10), glm::vec3(0., 0, 1.));
	s->walls[1] =
		new Wall(glm::vec3(-170, 0, GROUND_HEIGHT - 110),
			glm::vec3(150., 0, GROUND_HEIGHT - 110), glm::vec3(0., 0, -1.));
	//s->walls[2] =
	//	new Wall(glm::vec2(-10, GROUND_HEIGHT - 110),
	//		glm::vec2(10, GROUND_HEIGHT - 50), glm::vec2(0., -1.));
	//s->walls[3] =
	//	new Wall(glm::vec2(-10, GROUND_HEIGHT - 45),
	//		glm::vec2(10, GROUND_HEIGHT - 10), glm::vec2(0., -1.));

	s->num_constraints =
		s->num_particles * 2 + s->num_particles * s->num_walls; // ground + walls
	s->constraints = (Constraint**)malloc(sizeof(void*) * s->num_constraints);
	int constraint_ctr = 0;

	// ground
	for (int i = 0; i < s->num_particles; i++)
	{
		s->constraints[i] = new Ground_Constraint(s, i);
		constraint_ctr++;
	}

	// wall
	for (int i = 0; i < s->num_particles; i++)
	{
		for (int j = 0; j < s->num_walls; j++)
		{
			s->constraints[constraint_ctr] = new Wall_Constraint(s, i, j);
			constraint_ctr++;
		}
	}

	// mesh
	for (int i = 0; i < s->num_particles; i++)
	{
		s->constraints[constraint_ctr] = new Mesh_Constraint(s, i);
		constraint_ctr++;
	}

#pragma endregion

#pragma region INIT_STATION
	int station_id = 0;
	Station station;
	PathPlanner* planner = s->planner;

	// 0. Planner 안에 있는 Group 초기화
	for (int i = 0; i < s->num_groups; i++)
	{
		Group* g = s->groups[i];
		planner->map_groups.insert({ i, g });
	}

	// 1. Start Station, End Station ref Group
	for (auto group : planner->map_groups)
	{
		station = Station(station_id, group.second->start);
		s->stations.push_back(station);
		group.second->station_start_id = station_id;
		station_id++;

		station = Station(station_id, group.second->end);
		s->stations.push_back(station);
		group.second->station_end_id = station_id;
		station_id++;
	}

	// 2. Wall Stations (모든 벽은 4개의 vertices을 가지고 있다.)
	for (int i = 0; i < s->num_walls; i++)
	{
		// 4개의 vertices
		for (int j = 0; j < 4; j++)
		{
			station = Station(station_id, s->walls[i]->vertices[j]);
			s->stations.push_back(station);
			station_id++;
		}
	}

	// 3-1. Random Stations
	/*
	{
	for (int i = 0; i < STATION_RANDOM_COUNT; i++)
	{
		int shrink_x = 200;
		int shrink_y = 200;
		glm::vec2 pos_random = rand_glm::vec2(
			GRID_MIN_X + shrink_x, GRID_MAX_X - shrink_x,
			GRID_MIN_Y + shrink_y * 2, GRID_MAX_Y - shrink_y * 0.5f);

		// 장애물과 겹치면 다시 생성
		for (int j = 0; j < s->num_walls; j++)
		{
			if (s->walls[j]->is_intersect_point(pos_random))
			{
				j--;
				pos_random = rand_glm::vec2(
					GRID_MIN_X + shrink_x, GRID_MAX_X - shrink_x,
					GRID_MIN_Y + shrink_y * 2, GRID_MAX_Y - shrink_y * 0.5f);
			}
		}

		station = Station(station_id, pos_random);
		s->stations.push_back(station);
		station_id++;
	}
	}*/

	// 3-2. Grid 형태로 Station 생성
	auto list = s->grid->insert_station();
	for (auto i : list)
	{
		bool is_intersect = false;
		for (int j = 0; j < s->num_walls; j++)
		{
			if (s->walls[j]->is_intersect_point(i))
			{
				is_intersect = true;
				break;
			}
		}

		if (!is_intersect)
		{
			station = Station(station_id, i);
			s->stations.push_back(station);
			station_id++;
		}
	}

	// 4. Link each Station
	for (int i = 0; i < s->stations.size(); i++)
	{
		s->stations[i].search(s->stations, s->walls, s->num_walls);
	}

	// 5. a_star
	for (auto group : planner->map_groups)
	{
		std::vector<int> path = a_star(s->stations, group.second->station_start_id, group.second->station_end_id);

		for (int j = 0; j < path.size(); j++)
		{
			int station_index = path[j];
			station = s->stations[station_index];
			group.second->path.push_back(station);
		}
	}

	//for (int i = 0; i < planner->groups.size(); i++)
	//{
	//	auto path = a_star(s->stations, planner->groups[i].station_start_id, planner->groups[i].station_end_id);

	//	for (int j = 0; j < path.size(); j++)
	//	{
	//		int station_index = path[j];
	//		station = s->stations[station_index];
	//		planner->groups[i].path.push_back(station);
	//	}
	//}

	// 6. A* 적용, 각 agent가 path를 가짐
	for (auto group : planner->map_groups)
	{
		group.second->init_particles_goal();
	}
#pragma endregion

#pragma region INIT_PARTICLE_VELOCITY
	for (int i = 0; i < s->num_particles; i++) 
	{
		s->particles[i]->V_pref = V_PREF_ACCEL;

		float u;
		do {
			u = (float)rand() / (float)RAND_MAX;
		} while (u >= 1.0);

		s->particles[i]->V_pref +=
			sqrtf(-2.f * logf(1.f - u)) * 0.1f *
			cosf(2.f * _M_PI * (float)rand() / (float)RAND_MAX);

		s->planner->calc_pref_v_force(i);
		s->particles[i]->V.x = s->planner->velocity_buffer[i].x;
		s->particles[i]->V.y = s->planner->velocity_buffer[i].y;
	}
#pragma endregion
}

void init() {
	glfwInit();

	// using glfw 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// GLFW를 활용해 window 생성
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		abort();
	}
	// window의 context를 현재 스레드의 주 context로 지정
	glfwMakeContextCurrent(window);

	// glfw에 콜백함수 등록
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetKeyCallback(window, key_callback);

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// GLAD Loader를 활용하여, OpenGL의 함수를 관리
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initilize GLAD" << std::endl;

		abort();
	}

	printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
}

int main(int argc, char** argv) 
{
	init();

	gl_window = new GL_Window(WINDOW_WIDTH, WINDOW_HEIGHT);

	double previousTime = glfwGetTime();
	int frameCount = 0;
	double lastTime;

	// 0.03 sec - time step, 30 fr/sec
	int num_particles = ROWS * COLS;
	char* output = (char*)"blender.txt";

	Simulation sim(num_particles, 0, MS_PER_UPDATE, output);

	dummy_init(&sim);

	while (!glfwWindowShouldClose(window))
	{
		int display_w, display_h;

		// 버퍼 사이즈 가져오기
		glfwGetFramebufferSize(window, &display_w, &display_h);

		if (!stop)
			sim.do_time_step();

		// 그리기
		gl_window->draw(&sim);
		gl_window->draw_particle(&sim);

		// 버퍼 교체 (버퍼 돌려쓰기)
		glfwSwapBuffers(window);
		glfwPollEvents();

		mouseDragging(display_w, display_h);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	render(&sim);

	return 0;
}
