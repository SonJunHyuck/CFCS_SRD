#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Particle
{
public:
	glm::vec3 X;  // cur_pos
	glm::vec3 X_pred;  // predict (x*)

	glm::vec3 Delta_x;  // correction
	int Delta_x_ctr; // becomes Delta_buf_ctr

	glm::vec3 V;  // blended (vb)
	glm::vec3 V_prev;  // planner
	float V_pref;  // prefered

	glm::vec3 Accel;  // Only Force

	float mass;
	float inv_mass;

	int group_id;

	glm::vec3 goal;
	glm::vec3 final_goal;

	float r;

	// grid
	int cell_id;
	int cell_x;
	int cell_z;

	int path_ptr;

	float look_length;

	bool is_leader;
	bool is_link;

	glm::vec3 offset;

	glm::vec3 color;

	// not require
	glm::vec3 dummy_X;  // Draw Arrow �ϱ� ���ؼ� ��� (X ���� ���ϱ� ���� ������ �Ŀ� draw)
	Particle(glm::vec3 X, glm::vec3 V, float mass, float r, int group, glm::vec3 color, glm::vec3 goal);

	//void drawArrow();
};