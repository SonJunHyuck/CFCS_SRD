#pragma once
#include <iostream>
#include <vector>

#include <glad/glad.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace std;

class CheckerdBoard
{
private:
	GLuint vaoHandle;
	GLuint vbo_vertex_cell_positions;
	GLuint vbo_vertex_cell_colors;
	GLuint vbo_vertex_cell_normals;
	GLuint ibo_cell_elements;
	
	int verticesCount;

	void setup(float size, int nSquares);
	void setup(glm::vec2 size, std::vector<glm::vec3> colors, glm::vec2 slices);

public:
	CheckerdBoard();
	CheckerdBoard(float, int);
	CheckerdBoard(glm::vec2);
	CheckerdBoard(glm::vec2 size, std::vector<glm::vec3> colors, glm::vec2 slices);
	~CheckerdBoard();
	void draw();
};

