#pragma once
#include <vector>
#include <glad/glad.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Loader.h"

class ColorCube
{
private:
	GLuint vaoHandle;
	GLuint vaoHandle2;
	GLuint vbo_vertex_cube_positions;
	GLuint vbo_vertex_cube_normals;
	GLuint ibo_cube_elements;
	GLuint ibo_cube_elements2;

	void setup();
	void setup(float width, float height, float length);

public:
	ColorCube();
	ColorCube(float width, float height, float length);
	~ColorCube();
	void draw();
	glm::vec3 ComputeNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);

	int verticesCount;
};

