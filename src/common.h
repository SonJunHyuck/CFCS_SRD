#pragma once

#include <iostream>
#include <stdio.h>
#include <time.h>
#include <ctime>
#include <complex>
#include <vector>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
//#include <GL/freeglut.h>
#endif

#define INFINITE 0xffff

#define FLOAT_ZERO 0.0f
#define FLOAT_ONE 1.0f

#define VEC_ZERO glm::vec3(0.0f, 0.0f, 0.0f)
#define VEC_ONE glm::vec3(1.0f, 1.0f, 1.0f)
#define VEC_UP glm::vec3(0.0f, 1.0f, 0.0f)

#define AXIS_X glm::vec3(1.0f, 0.0f, 0.0f)
#define AXIS_Y glm::vec3(0.0f, 1.0f, 0.0f)
#define AXIS_Z glm::vec3(0.0f, 0.0f, 1.0f)

/* ========================= */
/* Simulation Engine Params: */
#define BLOCK_SIZE 512
#define _M_PI 3.14159265358979323846f
#define K_NOT_USED -1
#define _EPSILON 0.00001f
#define EPS 0.0000097
#define MS_PER_UPDATE 0.072 //0.0208
#define KSI 0.01f // 0.0093  // 0.005/0.54 -> alpha

#define ALPHA 1.2
#define ITER_COUNT 1
#define MAX_ACCEL 0.25f
#define V_PREF_ACCEL 1.8f
#define KSI_ACCEL 0.54f
#define NN_ACCEL 10.0f
#define LONG_RANGE_STIFFNESS 0.05
/* ========================= */

/* ========================= */
/* Scenario Params: */
#define WIDTH 1280
#define HEIGHT 720
//#define ROWS 30
//#define COLS 40
// 
#define ROWS 30  // for Circle (Large)
#define COLS 25

//#define ROWS 2  // for Circle (Small)
//#define COLS 11

//#define ROWS 3  // 3
//#define COLS 375
#define GROUND_HEIGHT 45.0
#define GRID_UP_BOUND -436.0
#define GRID_LOW_BOUND GROUND_HEIGHT + 50
#define LEFT_BOUND_X -285.0
#define RIGHT_BOUND_X 285.0

/* Particle Group Params: */
#define GROUP_COUNT 2

#define IS_SINGLE GROUP_COUNT < 2
#define IS_FORMATION true
#define IS_AVOIDANCE false
/* ========================= */

/* ========================= */
#define IS_DRAW_LINE true
#define IS_DRAW_SRD false
#define IS_DRAW_SRD_LINE false
/* ========================= */

/* ========================= */
/* Custom Group Params: */
#define CAMERA_TRACKING true
#define CAMERA_VIEW 84.0f
#define COHESION true
#define FTL false
#define STEERING false
#define AVOIDANCE true
#define DENSITY 2.2f // 3.2f
#define HEIGHT_INTERVAL 3.2f
#define HEIGHT_DIFF 0.0f
#define MIDDLE_HOLE 0.0f
#define ROW_INTERVAL 400
/* ========================= */


/* ========================= */

/* ========================= */
/* Grid Params: */
#define GRID_MIN_X (LEFT_BOUND_X - 50)
#define GRID_MIN_Z (GRID_UP_BOUND - 10)
#define GRID_MAX_X (RIGHT_BOUND_X + 50)
#define GRID_MAX_Z (GRID_LOW_BOUND)
#define MIXED_AREA -1

static float CELL_SIZE = floor(DENSITY + 3);
/* ========================= */

#define __device static inline

typedef unsigned char BYTE;
typedef unsigned int uint;

// Vec2
struct float2 {
	float x, y;
	float operator[](int i) const { return *(&x + i); }
	float& operator[](int i) { return *(&x + i); }
	float2 operator+ (float2 a)
	{
		return { x + a.x, y + a.y };
	}
	float2& operator+= (const float2& a)
	{
		*this = *this + a;
		return *this;
	}
	float2 operator- (float2 a)
	{
		return { x - a.x, y - a.y };
	}
	float2& operator-= (const float2& a)
	{
		*this = *this - a;
		return *this;
	}
	float2 operator* (float t)
	{
		return { x * t, y * t };
	}
	float operator* (float2 a)
	{
		return { x * a.x + y };
	}
	float2 operator/ (float t)
	{
		return { x / t, y / t };
	}
};

__device float2 make_float2(float x, float y)
{
	float2 a = { x, y };
	return a;
}

// Vec3
struct float3 {
	float x, y, z;
#ifdef WITH_OPENCL
	float w;
#endif
	float operator[](int i) const { return *(&x + i); }
	float& operator[](int i) { return *(&x + i); }
};

__device float3 make_float3(float x, float y, float z)
{
#ifdef WITH_OPENCL
	float3 a = { x, y, z, 0.0f };
#else
	float3 a = { x, y, z };
#endif
	return a;
}

static uint iDivUp(uint a, uint b)
{
	return (a % b != 0) ? (a / b + 1) : (a / b);
}

static float min(const float& a, const float& b) { return (a < b) ? a : b; }

static float norm(const glm::vec3& a) { return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z); }

static glm::vec3 normalize(const glm::vec3& a)
{
	return (a / norm(a));
}

static float distance(const glm::vec3& a, const glm::vec3& b) 
{
	return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
}

static float distance_ground(const glm::vec3& a, const float& rad, const float& ground_z) {
	float res = ground_z - a.z - rad;
	return res;
}

static float dot(const glm::vec3& a, const glm::vec3& b) 
{ 
	return a.x * b.x + a.y * b.y + a.z * b.z; 
}

static void project_on_vector(const glm::vec3& a, const glm::vec3& b_normalized, glm::vec3& out)
{
	float d = dot(a, b_normalized);
	out.x = b_normalized.x * d;
	out.y = 0;
	out.z = b_normalized.z * d;
}

static void clamp(glm::vec3& v, float maxValue) {
	float lengthV = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	
	if (lengthV > maxValue) 
	{
		float mult = (maxValue / lengthV);
		v.x *= mult;
		v.y *= mult;
		v.z *= mult;
	}
}

static void printv(glm::vec3& v)
{
	printf("vec : %f, %f, %f\n", v.x, v.y, v.z);
}

static glm::vec3 get_center_point(std::vector<glm::vec3> pos)
{
	int pos_size = pos.size();

	if (pos_size == 1)
		return pos[0];

	glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
	float area = 0;

	glm::vec3  first_point;
	glm::vec3  second_point;

	float factor = 0;
	for (int i = 0; i < pos_size; i++)
	{
		int next_i = (i + 1) % pos_size;

		first_point = pos[i];
		second_point = pos[next_i];

		factor = ((first_point.x * second_point.z) - (second_point.x * first_point.z));
		area += factor;
		center += glm::vec3(
			(first_point.x + second_point.x) * factor,
			0,
			(first_point.z + second_point.z) * factor);
	}

	area /= 2.0f;
	area *= 6.0f;

	factor = (1.0f / area);

	center = glm::vec3(center.x * factor, 0, center.z * factor);

	return center;
}

static  glm::vec3 position_to_index(glm::vec3 pos)
{
	float x = (pos.x - GRID_MIN_X) / CELL_SIZE;
	float z = (pos.z - GRID_MIN_Z) / CELL_SIZE;

	glm::vec3 index = glm::vec3(floor(x), 0, floor(z));

	return index;
}

static  glm::vec3 index_to_position(glm::vec3 index)
{
	glm::vec3 min = glm::vec3(GRID_MIN_X, 0, GRID_MIN_Z);

	float x = (index.x * CELL_SIZE) + min.x;
	float z = (index.z * CELL_SIZE) + min.z;

	glm::vec3 pos = glm::vec3(x, 0, z);

	return pos;
}

static int index_to_cell_id(int x, int y, int num_rows)
{
	return y * num_rows + x;
}

static  double GetSeconds() 
{
	static time_t last_time;
	static time_t last_utime;
	static double sim_time = 0.0f;
	time_t now_time;
	time_t now_utime;

	now_time = std::time(NULL);
	now_utime = std::time(NULL) * 1000;

	if(last_time == 0)
	{
		last_time = now_time;
		last_utime = now_utime;
	}

	float dt = (float)(now_time - last_time) + 1.0e-6 * (now_utime - last_utime);
	sim_time += dt;
	last_time = now_time;
	last_utime = now_utime;

	return sim_time;
}

static  float rand_interval(float min, float max)
{
	return min + (max - min) * rand() / RAND_MAX;
}

static  glm::vec3 rand_float3(float minx, float maxx, float miny, float maxy)
{
	return glm::vec3(rand_interval(minx, maxx), 0, rand_interval(miny, maxy));
}

static  float deg_to_rad(float deg)
{
	const float PI = 3.1415;
	while (deg >= 360.) {
		deg -= 360.;
	}
	while (deg <= 0.) {
		deg += 360.;
	}
	return PI / 180. * deg;
}

static  bool intersect_point_rect(glm::vec3 point, glm::vec3 rect_pivot, glm::vec3 rect_wh)
{
	// min > point (x)
	if (point.x < rect_pivot.x)
		return false;

	// max < point (x)
	if (point.x > rect_pivot.x + rect_wh.x)
		return false;

	// min > point (y)
	if (point.z < rect_pivot.z)
		return false;

	// max < point (y)
	if (point.z > rect_pivot.z + rect_wh.z)
		return false;

	return true;
}
static  bool intersect_line_line(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4)
{
	// calculate the distance to intersection point
	float uA = ((p4.x - p3.x) * (p1.z - p3.z) - (p4.z - p3.z) * (p1.x - p3.x)) /
		((p4.z - p3.z) * (p2.x - p1.x) - (p4.x - p3.x) * (p2.z - p1.z));

	float uB = ((p2.x - p1.x) * (p1.z - p3.z) - (p2.z - p1.z) * (p1.x - p3.x)) /
		((p4.z - p3.z) * (p2.x - p1.x) - (p4.x - p3.x) * (p2.z - p1.z));

	// if uA and uB are between 0-1, lines are colliding
	if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1)
	{
		// optionally, draw a circle where the lines meet
		float intersectionX = p1.x + (uA * (p2.x - p1.x));
		float intersectionZ = p1.z + (uA * (p2.z - p1.z));

		return true;
	}

	return false;
}
static  bool intersect_line_rect(glm::vec3 p1, glm::vec3 p2, glm::vec3 rect_pivot, glm::vec3 rect_wh)
{
	glm::vec3 to_right = glm::vec3(rect_wh.x, 0, 0);
	glm::vec3 to_bottom = glm::vec3(0, 0, rect_wh.z);

	bool left = intersect_line_line(p1, p2, rect_pivot, (rect_pivot + to_bottom));
	bool right = intersect_line_line(p1, p2, (rect_pivot + to_right), rect_pivot + rect_wh);
	bool top = intersect_line_line(p1, p2, rect_pivot, (rect_pivot + to_right));
	bool bottom = intersect_line_line(p1, p2, (rect_pivot + to_bottom), (rect_pivot + rect_wh));

	// if ANY of the above are true, the line
	// has hit the rectangle
	if (left || right || top || bottom)
	{
		return true;
	}

	return false;
}
static  bool intersect_line_line_f2(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, glm::vec3* out)
{
	// calculate the distance to intersection point
	float uA = ((p4.x - p3.x) * (p1.z - p3.z) - (p4.z - p3.z) * (p1.x - p3.x)) /
		((p4.z - p3.z) * (p2.x - p1.x) - (p4.x - p3.x) * (p2.z - p1.z));

	float uB = ((p2.x - p1.x) * (p1.z - p3.z) - (p2.z - p1.z) * (p1.x - p3.x)) /
		((p4.z - p3.z) * (p2.x - p1.x) - (p4.x - p3.x) * (p2.z - p1.z));

	// if uA and uB are between 0-1, lines are colliding
	if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1)
	{
		// optionally, draw a circle where the lines meet
		float intersectionX = p1.x + (uA * (p2.x - p1.x));
		float intersectionZ = p1.z + (uA * (p2.z - p1.z));

		*out = glm::vec3(intersectionX, 0, intersectionZ);
		return true;
	}

	return false;
}
static  void intersect_line_rect_f2(glm::vec3 p1, glm::vec3 p2, glm::vec3 rect_pivot, glm::vec3 rect_wh, glm::vec3* out)
{
	glm::vec3 to_right = glm::vec3(rect_wh.x, 0, 0);
	glm::vec3 to_bottom = glm::vec3(0, 0, rect_wh.z);
	std::vector<glm::vec3> vec_intersect = std::vector<glm::vec3>();
	glm::vec3 out_ = glm::vec3(INFINITE, 0, INFINITE);

	bool left, right, top, bottom;
	left = intersect_line_line_f2(p1, p2, rect_pivot, (rect_pivot + to_bottom), &out_);
	if (left)
		vec_intersect.push_back(out_);

	right = intersect_line_line_f2(p1, p2, (rect_pivot + to_right), (rect_pivot + rect_wh), &out_);
	if (right)
		vec_intersect.push_back(out_);

	top = intersect_line_line_f2(p1, p2, rect_pivot, (rect_pivot + to_right), &out_);
	if (top)
		vec_intersect.push_back(out_);

	bottom = intersect_line_line_f2(p1, p2, (rect_pivot + to_bottom), (rect_pivot + rect_wh), &out_);
	if (bottom)
		vec_intersect.push_back(out_);

	if (left || right || top || bottom)
	{
		int count = 0;

		for (auto i : vec_intersect)
		{
			out[count] = i;
			count++;
		}
	}
}

static std::vector<glm::vec3> unique_vec3(std::vector<glm::vec3> a)
{
	// Ҽ 
	for (auto pos = a.begin(); pos != a.end(); pos++)
	{
		pos->x = floor(pos->x);
		pos->y = floor(pos->y);
		pos->z = floor(pos->z);
	}

	//  (xǥ  ,  zǥ  )
	std::sort(a.begin(), a.end(), [](glm::vec3 b, glm::vec3 c) 
		{
			if (b.x == c.x)
			{
				if (b.y == c.y)
				{
					return b.z <= c.z;
				}
				else
				{
					return b.y < c.y;
				}
			}

			return b.x < c.x;
		});

	// ߺ 
	a.erase(std::unique(a.begin(), a.end()), a.end());

	return a;
}