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

glm::vec3 Camera::GetWorldPos(float width, float height, double x, double y)
{
    float DPI_WEIGHT = 2.0f;
    x *= DPI_WEIGHT;
    y *= DPI_WEIGHT;
    double NDC_X = ((2.0f * x) / width) - 1.0f;
    double NDC_Y = 1.0f - ((2.0f * y) / height);
    double NDC_Z = -1.0f;
    glm::vec4 Cord_NDC = glm::vec4(NDC_X, NDC_Y, NDC_Z, 0.0f);

    auto projection = glm::perspective(glm::radians(45.0f), width / height, 0.01f, 400.0f);
    auto view = glm::lookAt(position, position + front, up);
    
    glm::vec4 Cord_Eye = glm::inverse(projection) * Cord_NDC;
    Cord_Eye = glm::vec4(Cord_Eye.x, Cord_Eye.y, -1.0f, 0.0f);
    glm::vec3 Cord_World = (glm::vec3)(glm::inverse(view) * Cord_Eye);
    Cord_World = glm::normalize(Cord_World);

    float t;
    float plane_distance = 0.0f;

    t = -(dot(position, up) - plane_distance) / (dot(Cord_World, up));
    glm::vec3 FinalPos = position + Cord_World * t;

    return FinalPos;
}