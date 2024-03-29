#pragma once

#include <memory>
#include <string>
#include <optional>
#include <vector>

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <spdlog/spdlog.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// ## : 은 klass랑 변수이름(UPtr, Ptr, WPtr)을 붙여줌
#define CLASS_PTR(klassName) \
class klassName; \
using klassName ## UPtr = std::unique_ptr<klassName>; \
using klassName ## Ptr = std::shared_ptr<klassName>; \
using klassName ## WPtr = std::weak_ptr<klassName>;

#define Formation_t std::vector<glm::vec3>

#define _M_PI 3.14159265358979323846f
#define _EPSILON 0.00001f

#define VEC_ZERO glm::vec3(0, 0, 0)
#define VEC_ONE glm::vec3(1, 1, 1)

extern float DPI_WEIGHT;
extern float DELTA_TIME;
extern float MAX_ACCEL;
extern float GRID_BOUND_X;
extern float GRID_BOUND_Z;
extern float GRID_DENSITY;
extern bool ON_AVOIDANCE_MODEL;

// optional : 포인터의 메모리 누수 위험 방지 장치
std::optional<std::string> LoadTextFile(const std::string& filename);

glm::vec3 GetAttenuationCoeff(float distance);

void ClampVec3(glm::vec3 &InVec3, const float MaxValue);

void ProjectOnVec3(const glm::vec3 &InVec3, const glm::vec3 &InUnitVec3, glm::vec3 &OutVec3);

float Deg2Rad(float InDegree);

void UniqueVertices(Formation_t& OutVertices);