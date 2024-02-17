#pragma once
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Viewer;
class ShaderProgram;

class Sphere;
class CheckerdBoard;
class Line;
class Mesh; 

class Simulation;

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

	void create_station(glm::vec3 pos, Simulation* sim, int group_id);
	void send_ray(glm::vec4 ray_clip, Simulation* sim, int group_id);

private:
	int width;
	int height;

	int tracking_target;
	glm::vec3 tracking_particle_last_pos;
	
	ShaderProgram* shader_simple;
	ShaderProgram* shader_sphere;
	ShaderProgram* shader_checkerd_board;
	ShaderProgram* shader_mesh;

	GLuint VBO_mat_sphere_instancing;
	GLuint VBO_ka_instancing;
	glm::mat4* mat_sphere_instancing;
	glm::vec4* ka_instancing;

	void setup_shader();
	void setup_instance_buffer(int num_particles);
};