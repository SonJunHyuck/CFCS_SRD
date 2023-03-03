#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
	glm::vec3 pos;
	glm::vec3 target;
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;

	float yaw;
	float pitch;
	float fov;

	float speed;

	Camera(glm::vec3 pos, glm::vec3 target);

	glm::mat4 LookAt();
	void Move(float deltaTime, char key);
	void Rotate(double offsetX, double offsetY);
};

