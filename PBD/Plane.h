#pragma once
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

class Plane
{
public :
	std::vector<glm::vec4> positions;
	std::vector<glm::vec3> colors;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;

	Plane();
	Plane(float size, int squares);
	void set_up(float size, int squares);
	void draw();

private:
	GLuint vaoHandle;
	GLuint vbo_vertex_cell_positions;
	GLuint vbo_vertex_cell_colors;
	GLuint vbo_vertex_cell_normals;
	GLuint ibo_cell_elements;
};

