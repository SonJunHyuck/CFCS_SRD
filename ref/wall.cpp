#include "wall.h"
#include "common.h"

Wall::Wall(glm::vec3 x0, glm::vec3 x1, glm::vec3 n)
{
	this->x0 = x0;
	this->x1 = x1;

	this->center = x0 + (x1 - x0) * 0.5f;
	this->width = abs(x1.x - x0.x);
	this->height = abs(x1.y - x0.y);

	init_directions();
}

glm::vec3 Wall::get_normal(glm::vec3 pos)
{
	glm::vec3 normal = glm::vec3(0, 0, 0);

	if (pos.x == x0.x)
		normal += glm::vec3(-1, 0, 0);

	if (pos.y == x0.y)
		normal += glm::vec3(0, 0, -1);

	if (pos.x == x1.x)
		normal += glm::vec3(1, 0, 0);

	if (pos.y == x1.y)
		normal += glm::vec3(0, 0, 1);

	return normal;
}

bool Wall::is_intersect_point(glm::vec3 point)
{
	glm::vec3 wh = glm::vec3(width, 0, height);

	return intersect_point_rect(point, x0, wh);
}

void Wall::init_directions()
{
	enum Side { LEFT_TOP, LEFT_BOTTOM, RIGHT_TOP, RIGHT_BOTTOM };

	glm::vec3 dir_height = glm::vec3(0, 0, height);

	this->directions[Side::LEFT_TOP] = x0 - center;
	this->directions[Side::LEFT_BOTTOM] = (x0 + dir_height) - center;
	this->directions[Side::RIGHT_TOP] = (x1 - dir_height) - center;
	this->directions[Side::RIGHT_BOTTOM] = x1 - center;

	for (int i = 0; i < 4; i++)
	{
		float length = norm(directions[i]);
		this->vertices[i] = center + directions[i] + (directions[i] / length) * 5.0f;
	}
}