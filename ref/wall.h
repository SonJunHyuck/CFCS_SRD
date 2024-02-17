#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Wall
{
public:
	glm::vec3 x0;
	glm::vec3 x1;
	glm::vec3 center;

	float width;
	float height;

	glm::vec3 directions[4];
	glm::vec3 vertices[4];

	Wall(glm::vec3 x0, glm::vec3 x1, glm::vec3 n);

	glm::vec3 get_normal(glm::vec3 pos);

	bool is_intersect_point(glm::vec3 point);

private:
	void init_directions();
};