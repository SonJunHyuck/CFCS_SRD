#include "Camera.h"

#define KEY_UP 'w'
#define KEY_DOWN 's'
#define KEY_LEFT 'a'
#define KEY_RIGHT 'd'

Camera::Camera(glm::vec3 pos, glm::vec3 target)
{
	this->pos = pos;
	this->target = target;

	front = glm::normalize(pos - target);
	right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), front));
	up = glm::cross(front, right);

	yaw = -90.0f;
	pitch = 0.0f;
	fov = 45.0f;

	speed = 2.5f;
}

glm::mat4 Camera::LookAt()
{
	glm::mat4 matrixView = glm::identity<glm::mat4>();

	matrixView = glm::lookAt(pos, pos + front, up);

	return matrixView;
}

void Camera::Move(float deltaTime, char key)
{
	float speed = this->speed * deltaTime;

	switch (key)
	{
	case KEY_UP:
		pos += speed * front;
		break;
	case KEY_DOWN:
		pos -= speed * front;
		break;
	case KEY_LEFT:
		pos -= speed * right;
		break;
	case KEY_RIGHT:
		pos += speed * right;
		break;
	}
}
void Camera::Rotate(double offsetX, double offsetY)
{
	float sensitivity = 0.05f;
	offsetX *= sensitivity;
	offsetY *= sensitivity;

	yaw += offsetX;
	pitch += offsetY;

	glm::clamp(pitch, 89.0f, -89.0f);

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	this->front = glm::normalize(front);
}