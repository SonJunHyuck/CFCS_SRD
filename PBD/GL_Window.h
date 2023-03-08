#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Loader.h"
#include "Viewer.h"

#include "CheckerdBoard.h"
#include "Sphere.h"
#include "ColorCube.h"
#include "Line.h"
#include "Mesh.h"

#include "utility.h"
#include "simulation.h"
#include "constraint.h"
#include "particle.h"
#include "path_planner.h"
#include "wall.h"
#include "particle_tuple.h"

static float DEFAULT_VIEW_POINT[3] = { 0, 15, 1 };
static float DEFAULT_VIEW_CENTER[3] = { 0, 0, 0 };
static float DEFAULT_UP_VECTOR[3] = { 0, 1, 0 };

class GL_Window
{
public:
	enum class shape_type 
	{
		SQUARE = 0, 
		CIRCLE, 
		MESH
	};

	Viewer* viewer;
	ColorCube* cube;
	Sphere* sphere;
	Sphere* sphere_formation;
	CheckerdBoard* checkerd_board;
	Line* line;

	Mesh* mesh;

	float last_X;
	float last_Z;

	GL_Window();
	GL_Window(int w, int h);

	void set_size(int w, int h);

	void draw(Simulation* sim);

	glm::mat4 Perspective(float fovy, float aspect, float near, float far);
	glm::mat4 LookAt(glm::vec3 campos, glm::vec3 look, glm::vec3 up);

	std::vector<glm::vec3> set_mesh(int num_particle, int r_x, int r_y, int r_z, float s);
	void set_formation(Simulation* sim);
	void change_particle_color(Simulation* sim);

	void dummy_init(Simulation* sim);

	void create_station(glm::vec3 pos);
	void send_ray(glm::vec4 ray_clip);

private:
	int width;
	int height;

	int tracking_particle;
	glm::vec3 tracking_particle_last_pos;
	
	ShaderProgram* shader_cube;
	ShaderProgram* shader_sphere;
	ShaderProgram* shader_checkerd_board;
	ShaderProgram* shader_mesh;

	GLuint VBO_mat_sphere_instancing;
	GLuint VBO_ka_instancing;
	glm::mat4* mat_sphere_instancing;
	glm::vec4* ka_instancing;

	void setup_buffer();
	void setup_instance_buffer(int num_particles);
};