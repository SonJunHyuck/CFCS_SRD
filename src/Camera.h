#pragma once

#include "common.h"

class Camera
{
public:
    bool isControl{ false };

    void Translate(glm::vec3 direction);
    void Rotate(double x, double y);

    void SetTranslationSpeed(float speed) { translateSpeed = speed; }
    void SetRotationSpeed(float speed) { rotateSpeed = speed; }

private:
    friend class Context;  // context 클래스 이외에는 변수 접근 어렵게

    float translateSpeed {0.02f};
    float rotateSpeed {0.1f};

    glm::vec2 prevMousePos{glm::vec2(0.0f)};

    float pitch{-20.0f};
    float yaw{0.0f};

    glm::vec3 position{glm::vec3(0.0f, 2.5f, 8.0f)};
    glm::vec3 front{glm::vec3(0.0f, 0.0f, -1.0f)};
    glm::vec3 up{glm::vec3(0.0f, 1.0f, 0.0f)};
};