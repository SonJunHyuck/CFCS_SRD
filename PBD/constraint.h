#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Simulation;
class Particle;

class Constraint
{ 
public:
	const Simulation* sim;
	int* indicies;
	int num_particles;
	glm::vec3* delta_X;
	bool active;

	Constraint(Simulation* sim, int num_particles);

	virtual void project(Particle** particles) = 0; // forcing implemntation in base class
	virtual ~Constraint();
};

class Stability_Constraint : public Constraint {
public:
	int i;
	int j;
	float w_i_coef;
	float w_j_coef;
	glm::vec3 contact_normal;
	glm::vec3 tangential_displacement;
	glm::vec3 x_pred_w_delta1;
	glm::vec3 x_pred_w_delta2;
	glm::vec3 out;
	float collision_margin;

	Stability_Constraint(Simulation* sim, int i, int j);

	virtual void project(Particle** particles);
};

class Powerlaw_Force_Constraint : public Constraint {
public:
	static const float k;// = 1.5f; // stiffness
	static const float m;// = 2.0f;
	static const float tao0;// = 3.f;
	int i;
	int j;
	float w_i_coef;
	float w_j_coef;
	glm::vec3 out;
	float collision_margin;
	float radius_init;
	float radius_sq_init;
	float delta_t;
	float dv_i;
	float dv_j;

	Powerlaw_Force_Constraint(Simulation* sim, int i, int j);

	virtual void project(Particle** particles);
};

// long-range -> avoidance
class Powerlaw_Constraint : public Constraint {
public:
	static const float k;// = 1.5; // stiffness
	static const float tao0;// = 4.;
	static const float maxValue;// = 0.2; // delta_t * pref_speed
	int i;
	int j;
	float w_i_coef;
	float w_j_coef;
	glm::vec3 out;
	float collision_margin;
	float radius_init;
	float radius_sq_init;
	float delta_t;
	float dv_i;
	float dv_j;
	float max_acceleration;

	Powerlaw_Constraint(Simulation* sim, int i, int j);

	virtual void project(Particle** particles);
};

// contact (충돌했을 때, 마찰을 일으킨 후 위치이동) -> 파티클이 겹치지 않음
class Friction_Constraint : public Constraint {
public:
	// usually easier to keep object moving than start movement, so mu_s>mu_k
	// some friction results
	// http://hypertextbook.com/facts/2007/TabraizRasul.shtml
	static const float mui_static;// = 0.00026;    // 0.021;
	static const float mui_kinematic;// = 0.00023; // 0.02;
	/*typical values:
	http://spiff.rit.edu/classes/phys211/lectures/fric/fric_all.html smooth 0.05
	medium 0.3
	rough 1.0
	*/
	int i;
	int j;
	float w_i_coef;
	float w_j_coef;
	glm::vec3 contact_normal;
	glm::vec3 tangential_displacement;
	glm::vec3 x_pred_w_delta1;
	glm::vec3 x_pred_w_delta2;
	glm::vec3 out;
	float collision_margin;
	float radius_sq_init;
	float radius_init;
	float delta_t;

	Friction_Constraint(Simulation* sim, int i, int j);

	virtual void project(Particle** particles);
};

class Wall_Constraint : public Constraint {
public:
	// static const float ground_height=GROUND_HEIGHT;
	int i;
	// F=mui Fn = mui mg .
	// static const float
	// kinematic_friction=30.7*9.81*MS_PER_UPDATE*MS_PER_UPDATE;
	static const float kinematic_friction;// =  30000.7 * 9.81 * MS_PER_UPDATE * MS_PER_UPDATE;
	int wall_idx;
	float collision_margin;
	glm::vec3 contact_normal;
	Wall_Constraint(Simulation* sim, int i, int wall_idx);

	virtual void project(Particle** particles);
};

// Groud Friction (y축으로는 못나가게, x축으로는 마찰력으로 느리게)
class Ground_Constraint : public Constraint {
public:
	// static const float ground_height=GROUND_HEIGHT;
	int i;
	// F=mui Fn = mui mg .
	// static const float
	// kinematic_friction=30.7*9.81*MS_PER_UPDATE*MS_PER_UPDATE;
	static const float kinematic_friction; //=30000.7 * 9.81 * MS_PER_UPDATE * MS_PER_UPDATE;

	Ground_Constraint(Simulation* sim, int i);

	virtual void project(Particle** particles);
};

// CURRENT Distance Constraint
class Distance_Constraint : public Constraint
{
public:
	int i;
	int j;
	float w_i_coef;
	float w_j_coef;
	glm::vec3 contact_normal;
	float collision_margin;
	float radius_sq_init;
	float radius_init;
	float delta_t;

	float dist;

	Distance_Constraint(Simulation* sim, int i, int j);

	virtual void project(Particle** particles);
};

// CURRENT Mesh
class Mesh_Constraint : public Constraint {
public:
	int i;
	float w_i_coef;
	float dist;
	glm::vec3 contact_normal;
	float delta_t;
	float accel_limit;

	Mesh_Constraint(Simulation* sim, int i);

	virtual void project(Particle** particles);
};