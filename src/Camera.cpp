#include "camera.h"

void Camera::Translate(glm::vec3 direction)
{
    if(!isControl)
        return;

    position += direction * translateSpeed;
}

void Camera::Rotate(double x, double y)
{   
    if(!isControl)
        return; 

    auto newMousePos = glm::vec2((float)x, (float)y);
    auto deltaPos = newMousePos - prevMousePos;

    yaw -= deltaPos.x * rotateSpeed;
    pitch -= deltaPos.y * rotateSpeed;

    // Yaw
    if (yaw < 0.0f)
        yaw += 360.0f;
    if (yaw > 360.0f)
        yaw -= 360.0f;

    // Pitch
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    prevMousePos = newMousePos;
}