#pragma once
#include <vector>
#include <glad/glad.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Loader.h"

class Line
{
private:
	GLuint vaoHandle;
	GLuint vbo_vertex_positions;

	void setup(glm::vec3 pos1, glm::vec3 pos2);

public :
	Line(glm::vec3 pos1, glm::vec3 pos2);
	~Line();
	void draw();

	int verticesCount;
};

