#include "utility.h"
#include "constraint.h"
#include "simulation.h"
#include "particle.h"
#include "wall.h"

#pragma region BASE
Constraint::Constraint(Simulation* sim, int num_particles)
{
	this->sim = sim;
	this->num_particles = num_particles;
	this->delta_X = (glm::vec3*)malloc(num_particles * sizeof(glm::vec3));
	this->indicies = (int*)malloc(num_particles * (sizeof(int)));
	this->active = false;
	for (int i = 0; i < num_particles; i++)
	{
		delta_X[i] = glm::vec3(0., 0., 0.);
	}
}
Constraint::~Constraint()
{
	free(indicies);
	free(delta_X);
}
#pragma endregion

#pragma region Stability
Stability_Constraint::Stability_Constraint(Simulation* sim, int i, int j) : Constraint(sim, 2) {
	this->i = i;
	this->j = j;
	this->indicies[0] = i;
	this->indicies[1] = j;
	// TODO if seg fault happens, it is because the particles are set up after
	// the constraints
	this->w_i_coef =
		sim->particles[i]->inv_mass /
		(sim->particles[i]->inv_mass + sim->particles[j]->inv_mass);
	this->w_j_coef =
		-sim->particles[j]->inv_mass /
		(sim->particles[i]->inv_mass + sim->particles[j]->inv_mass);
	this->collision_margin =
		(sim->particles[i]->r + sim->particles[j]->r) * 1.05f;
	this->contact_normal = glm::vec3(0.0, 0.0, 0.0);
	this->tangential_displacement = glm::vec3(0.0, 0.0, 0.0);
	this->x_pred_w_delta1 = glm::vec3(0.0, 0.0, 0.0);
	this->x_pred_w_delta2 = glm::vec3(0.0, 0.0, 0.0);
	this->out = glm::vec3(0.0, 0.0, 0.0);
}
void Stability_Constraint::project(Particle** particles) 
{
	// we don't want to use the bad old values
	delta_X[0].x = 0.0;
	delta_X[0].y = 0.0;
	delta_X[0].z = 0.0;
	delta_X[1].x = 0.0;
	delta_X[1].y = 0.0;
	delta_X[1].z = 0.0;
	if (true) 
	{
		float d = distance(particles[i]->X, particles[j]->X);
		float f = d - collision_margin;
		if (f < 0) 
		{
			contact_normal.x = 0.;
			contact_normal.y = 0.;
			contact_normal.z = 0.;

			tangential_displacement.x = 0.;
			tangential_displacement.y = 0.;
			tangential_displacement.z = 0.;

			x_pred_w_delta1.x = 0.;
			x_pred_w_delta1.y = 0.;
			x_pred_w_delta1.z = 0.;

			x_pred_w_delta2.x = 0.;
			x_pred_w_delta2.y = 0.;
			x_pred_w_delta2.z = 0.;

			out.x = 0.;
			out.y = 0.;
			out.z = 0.;

			contact_normal.x = (particles[i]->X.x - particles[j]->X.x) / d;
			contact_normal.y = (particles[i]->X.y - particles[j]->X.y) / d;
			contact_normal.z = (particles[i]->X.z - particles[j]->X.z) / d;

			delta_X[0].x = -w_i_coef * contact_normal.x * f;
			delta_X[0].y = -w_i_coef * contact_normal.y * f;
			delta_X[0].z = -w_i_coef * contact_normal.z * f;

			delta_X[1].x = -w_j_coef * contact_normal.x * f;
			delta_X[1].y = -w_j_coef * contact_normal.y * f;
			delta_X[1].z = -w_j_coef * contact_normal.z * f;
			active = true;
		}
	}
}
#pragma endregion

#pragma region Powerlaw_Force
Powerlaw_Force_Constraint::Powerlaw_Force_Constraint(Simulation* sim, int i, int j)
	: Constraint(sim, 2) {
	this->i = i;
	this->j = j;
	this->indicies[0] = i;
	this->indicies[1] = j;
	this->w_i_coef = sim->particles[i]->inv_mass;
	this->w_j_coef = sim->particles[j]->inv_mass;
	this->out = glm::vec3(0., 0., 0.);
	this->collision_margin =
		(sim->particles[i]->r + sim->particles[j]->r) * 1.05f;
	this->radius_init = (sim->particles[i]->r + sim->particles[j]->r);
	this->radius_sq_init = radius_init * radius_init;
	this->delta_t = sim->time_step;
	this->dv_i = 1. / delta_t;
	this->dv_j = -1. / delta_t;
}
void Powerlaw_Force_Constraint::project(Particle * *particles) {
	// we don't want to use the bad old values
	delta_X[0].x = 0.0;
	delta_X[0].y = 0.0;
	delta_X[0].z = 0.0;
	delta_X[1].x = 0.0;
	delta_X[1].y = 0.0;
	delta_X[1].z = 0.0;
	glm::vec3 x_i = particles[i]->X;
	glm::vec3 x_j = particles[j]->X;
	const float dist = distance(particles[i]->X, particles[j]->X);
	float radius_sq = radius_sq_init;

	if (dist < radius_init) 
	{
		radius_sq = (radius_init - dist) * (radius_init - dist);
	}

	float v_x = particles[i]->V.x - particles[j]->V.x;
	float v_z = particles[i]->V.z - particles[j]->V.z;
	float x0 = x_i.x - x_j.x;
	float z0 = x_i.z - x_j.z;
	float v_sq = v_x * v_x + v_z * v_z;
	float x0_sq = x0 * x0;
	float z0_sq = z0 * z0;
	float x_sq = x0_sq + z0_sq;
	float a = v_sq;
	float b = -v_x * x0 - v_z * z0;
	float b_sq = b * b;
	float c = x_sq - radius_sq;
	float d_sq = b_sq - a * c;

	if (d_sq > 0 && (a < -_EPSILON || a > _EPSILON)) 
	{
		float d = sqrtf(d_sq);
		float tao = (b - d) / a;
		if (dist < NN_ACCEL && tao > 0) {
			float c_x_nom = (v_sq * x0 + b * v_x) / d;
			float c_x = v_x - c_x_nom;
			float c_z_nom = (v_sq * z0 + b * v_z) / d;
			float c_z = v_z - c_z_nom;
			float F_s =
				-k * exp(-tao / tao0) / (a * powf(tao, m)) * (m / tao + 1. / tao0);
			float F_x = c_x * F_s;
			float F_z = c_z * F_s;
			delta_X[0].x = F_x;
			delta_X[0].z = F_z;
			delta_X[1].x = -F_x;
			delta_X[1].z = -F_z;
			active = true;
		}
	}
}
const float Powerlaw_Force_Constraint::k = 1.5f; // stiffness
const float Powerlaw_Force_Constraint::m = 2.0f;
const float Powerlaw_Force_Constraint::tao0 = 3.f;
#pragma endregion

#pragma region Powerlaw
Powerlaw_Constraint::Powerlaw_Constraint(Simulation* sim, int i, int j) : Constraint(sim, 2) 
{
	this->i = i;
	this->j = j;
	this->indicies[0] = i;
	this->indicies[1] = j;
	// TODO if seg fault happens, it is because the particles are set up after
	// the constraints
	this->w_i_coef = sim->particles[i]->inv_mass;
	this->w_j_coef = sim->particles[j]->inv_mass;
	this->out = glm::vec3(0., 0., 0.);
	this->collision_margin =
		(sim->particles[i]->r + sim->particles[j]->r) * 1.05f;
	this->radius_init = (sim->particles[i]->r + sim->particles[j]->r);
	this->radius_sq_init = radius_init * radius_init;
	this->delta_t = sim->time_step;
	this->dv_i = 1.;  // 1./delta_t;
	this->dv_j = -1.; //-1./delta_t;
	this->max_acceleration = sim->time_step * MAX_ACCEL;
}
void Powerlaw_Constraint::project(Particle** particles)
{
	// we don't want to use the bad old values
	delta_X[0] = glm::vec3(0, 0, 0);
	delta_X[1] = glm::vec3(0, 0, 0);

	// 같은 그룹은 Long range 적용 x
	if (particles[i]->group_id == particles[j]->group_id)
	{
		return;
	}

	glm::vec3 x_i = particles[i]->X;
	glm::vec3 x_j = particles[j]->X;

	glm::vec3 x_pred_i = particles[i]->X_pred;
	glm::vec3 x_pred_j = particles[j]->X_pred;

	const float dist = distance(x_i, x_j);
	const float dist_pred = distance(x_pred_i, x_pred_j);
	float radius_sq = radius_sq_init;

	// 두 파티클이 접촉했을 때,
	if (dist < radius_init)
	{
		// 파고든 만큼의 거리
		radius_sq = (radius_init - dist) * (radius_init - dist);
	}

	// 두 파티클의 pred 위치 차이 / 시간
	float v_x = (x_pred_i.x - x_i.x) / delta_t - (x_pred_j.x - x_j.x) / delta_t;
	float v_z = (x_pred_i.z - x_i.z) / delta_t - (x_pred_j.z - x_j.z) / delta_t;

	// 두 파티클 사이의 현재 위치 차이
	float x0 = x_i.x - x_j.x;
	float z0 = x_i.z - x_j.z;

	float v_sq = v_x * v_x + v_z * v_z;

	float x0_sq = x0 * x0;
	float z0_sq = z0 * z0;
	float x_sq = x0_sq + z0_sq;  // x_i의 magnitude_sq

	// a
	float a = v_sq;
	//float a = (dist_pred * dist_pred) / (delta_t * delta_t);

	// b (x0, y0) * (v_x, v_y)
	float b = -v_x * x0 - v_z * z0;
	//float b = -dot((x_i - x_j), (x_pred_i - x_pred_j)) / delta_t;

	// c
	//float c_ = x_sq - radius_sq;
	float c = dist * dist - radius_sq;

	// d
	float d_sq = (b * b) - (a * c);
	float d = sqrtf(d_sq);

	if (d_sq > 0 && (a < -_EPSILON || a > _EPSILON))
	{
		// time
		float tao = (b - d) / a;

		if (tao > 0 && tao < tao0)
		{
			float tao_alt = (b + d) / a;

			// pick the min solution that is > 0 (작은 값 선택 -> 더 가까운 지점에서 충돌한다)
			tao = (tao_alt < tao) && (tao_alt > 0) ? tao_alt : tao;
			//printf("%f\n", tao);

			float tao_hat = delta_t * floorf(tao / delta_t);  // clamp tao
			float tao_dia = tao_hat + delta_t;

			float x_hat_i = x_i.x + tao_hat * particles[i]->V.x;
			float z_hat_i = x_i.z + tao_hat * particles[i]->V.z;
			float x_hat_j = x_j.x + tao_hat * particles[j]->V.x;
			float z_hat_j = x_j.z + tao_hat * particles[j]->V.z;
			glm::vec3 hat_i = glm::vec3(x_hat_i, 0, z_hat_i);
			glm::vec3 hat_j = glm::vec3(x_hat_j, 0, z_hat_j);

			float x_dia_i = x_i.x + tao_dia * particles[i]->V.x;
			float z_dia_i = x_i.z + tao_dia * particles[i]->V.z;
			float x_dia_j = x_j.x + tao_dia * particles[j]->V.x;
			float z_dia_j = x_j.z + tao_dia * particles[j]->V.z;
			glm::vec3 dia_i = glm::vec3(x_dia_i, 0, z_dia_i);
			glm::vec3 dia_j = glm::vec3(x_dia_j, 0, z_dia_j);

			float constraint_value = 0;
			constraint_value = (distance(dia_i, dia_j) - collision_margin);

			if (constraint_value < 0)
			{
				float normal_x = (x_dia_i - x_dia_j) / (distance(dia_i, dia_j));
				float normal_z = (z_dia_i - z_dia_j) / (distance(dia_i, dia_j));
				glm::vec2 nor = glm::vec2(normal_x, normal_z);

				float gradient_i_x = normal_x;
				float gradient_i_z = normal_z;
				float gradient_j_x = -gradient_i_x;
				float gradient_j_z = -gradient_i_z;
				glm::vec3 gradient_i = glm::vec3(gradient_i_x, 0, gradient_i_z);
				glm::vec3 gradient_j = glm::vec3(gradient_j_x, 0, gradient_j_z);

				float stiff = k * exp(-(tao_hat * tao_hat) / tao0);

				float s = constraint_value /
					(particles[i]->inv_mass * (double)(norm(gradient_i) * norm(gradient_i)) +
						particles[j]->inv_mass * (double)(norm(gradient_j) * norm(gradient_j)));

				delta_X[0] = gradient_i * (-s) * w_i_coef * stiff;
				delta_X[1] = gradient_j * (-s) * w_j_coef * stiff;

				/*clamp(delta_X[0], max_acceleration);
				clamp(delta_X[1], max_acceleration);*/

				active = true;

				if (AVOIDANCE)
				{
					glm::vec3 modified_i = dia_i + delta_X[0];  // long range collision 해소된 위치
					glm::vec3 modified_j = dia_j + delta_X[1];  // margin (5% 때문에 2보다 좀 더 큰 값을 가짐)

					glm::vec3 modified_nor = (modified_i - modified_j) / norm(modified_i - modified_j);

					glm::vec3 d = (modified_i - hat_i) - (modified_j - hat_j);
					glm::vec3 dn = modified_nor * dot(d, modified_nor);
					glm::vec3 dt = d - dn;

					delta_X[0] += dt * particles[i]->V_pref * delta_t;
					delta_X[1] -= dt * particles[j]->V_pref * delta_t;

					clamp(delta_X[0], max_acceleration);
					clamp(delta_X[1], max_acceleration);

					active = true;
				}
			}
		}
	}
}
const float Powerlaw_Constraint::k = 1.5; // stiffness
const float Powerlaw_Constraint::tao0 = 20.00;
const float Powerlaw_Constraint::maxValue = 0.2; // delta_t * pref_speed
#pragma endregion

#pragma region Friction
Friction_Constraint::Friction_Constraint(Simulation* sim, int i, int j) : Constraint(sim, 2) {
	this->i = i;
	this->j = j;
	this->indicies[0] = i;
	this->indicies[1] = j;
	// TODO if seg fault happens, it is because the particles are set up after
	// the constraints
	this->w_i_coef =
		sim->particles[i]->inv_mass /
		(sim->particles[i]->inv_mass + sim->particles[j]->inv_mass);
	this->w_j_coef =
		-sim->particles[j]->inv_mass /
		(sim->particles[i]->inv_mass + sim->particles[j]->inv_mass);
	this->collision_margin =
		(sim->particles[i]->r + sim->particles[j]->r) * 1.05f;
	this->contact_normal = glm::vec3(0.0, 0.0, 0.0);
	this->tangential_displacement = glm::vec3(0.0, 0.0, 0.0);
	this->x_pred_w_delta1 = glm::vec3(0.0, 0.0, 0.0);
	this->x_pred_w_delta2 = glm::vec3(0.0, 0.0, 0.0);
	this->out = glm::vec3(0.0, 0.0, 0.0);
	this->radius_init = (sim->particles[i]->r + sim->particles[j]->r);
	this->radius_sq_init = radius_init * radius_init;
	this->delta_t = sim->time_step;
}
void Friction_Constraint::project(Particle** particles) {
	// we don't want to use the bad old values
	delta_X[0].x = 0.0;
	delta_X[0].y = 0.0;
	delta_X[0].z = 0.0;
	delta_X[1].x = 0.0;
	delta_X[1].y = 0.0;
	delta_X[1].z = 0.0;
	float d = distance(particles[i]->X_pred, particles[j]->X_pred);
	float f = d - collision_margin;

	//충돌
	if (f < 0)
	{
		contact_normal.x = 0.;
		contact_normal.y = 0.;
		contact_normal.z = 0.;

		tangential_displacement.x = 0.;
		tangential_displacement.y = 0.;
		tangential_displacement.z = 0.;

		x_pred_w_delta1.x = 0.;
		x_pred_w_delta1.y = 0.;
		x_pred_w_delta1.z = 0.;

		x_pred_w_delta2.x = 0.;
		x_pred_w_delta2.y = 0.;
		x_pred_w_delta2.z = 0.;

		out.x = 0.;
		out.y = 0.;
		out.z = 0.;

		contact_normal.x = (particles[i]->X_pred.x - particles[j]->X_pred.x) / d;
		contact_normal.z = (particles[i]->X_pred.z - particles[j]->X_pred.z) / d;

		// kinetic frictional
		delta_X[0].x = -w_i_coef * contact_normal.x * f;
		delta_X[0].z = -w_i_coef * contact_normal.z * f;
		delta_X[1].x = -w_j_coef * contact_normal.x * f;
		delta_X[1].z = -w_j_coef * contact_normal.z * f;

		x_pred_w_delta1.x = delta_X[0].x + particles[i]->X_pred.x;
		x_pred_w_delta1.z = delta_X[0].z + particles[i]->X_pred.z;

		x_pred_w_delta2.x = delta_X[1].x + particles[j]->X_pred.x;
		x_pred_w_delta2.z = delta_X[1].z + particles[j]->X_pred.z;

		float n_norm = distance(x_pred_w_delta1, x_pred_w_delta2);
		contact_normal.z = (x_pred_w_delta1.x - x_pred_w_delta2.x) / n_norm;
		contact_normal.x = -(x_pred_w_delta1.z - x_pred_w_delta2.z) / n_norm;

		tangential_displacement.x = x_pred_w_delta1.x - particles[i]->X.x -
			(x_pred_w_delta2.x - particles[j]->X.x);
		tangential_displacement.z = x_pred_w_delta1.z - particles[i]->X.z -
			(x_pred_w_delta2.z - particles[j]->X.z);

		project_on_vector(tangential_displacement, contact_normal, out);

		float out_norm = norm(out);

		// friction model
		if (out_norm >= mui_static * d)
		{
			float coef = min(1., mui_kinematic * d / out_norm);
			out *= coef;
		}

		delta_X[0] += -out * w_i_coef;
		delta_X[1] += -out * w_j_coef;

		clamp(delta_X[0], sim->time_step * MAX_ACCEL);
		clamp(delta_X[1], sim->time_step * MAX_ACCEL);

		/*delta_X[0].x += -out.x * w_i_coef;
		delta_X[0].z += -out.z * w_i_coef;
		delta_X[1].x += -out.x * w_j_coef;
		delta_X[1].z += -out.z * w_j_coef;*/
		active = true;
	}
	// 비충돌
	else
	{
		glm::vec3 x_i = particles[i]->X;
		glm::vec3 x_j = particles[j]->X;
		const float dist = distance(particles[i]->X, particles[j]->X);
		float radius_sq = radius_sq_init;

		if (dist < radius_init)
		{
			radius_sq = (radius_init - dist) * (radius_init - dist);
		}

		const float v_ix = (particles[i]->X_pred.x - x_i.x) / delta_t;
		const float v_jx = (particles[j]->X_pred.x - x_j.x) / delta_t;
		const float v_x = v_ix - v_jx;
		const float v_iz = (particles[i]->X_pred.z - x_i.z) / delta_t;
		const float v_jz = (particles[j]->X_pred.z - x_j.z) / delta_t;
		const float v_z = v_iz - v_jz;

		float x0 = x_i.x - x_j.x;
		float z0 = x_i.z - x_j.z;
		float v_sq = v_x * v_x + v_z * v_z;
		float x0_sq = x0 * x0;
		float z0_sq = z0 * z0;
		float x_sq = x0_sq + z0_sq;
		float a = v_sq;
		float b = -v_x * x0 - v_z * z0;
		float b_sq = b * b;
		float c = x_sq - radius_sq;
		float d_sq = b_sq - a * c;

		if (d_sq > 0 && (a < -_EPSILON || a > _EPSILON))
		{
			float d = sqrtf(d_sq);
			float tao = (b - d) / a;
			float tao_alt = (b + d) / a;
			// pick the min solution that is > 0
			tao = tao_alt < tao&& tao_alt > 0 ? tao_alt : tao;
			// need to consider +- sign perhaps?
			if (tao > 0)
			{
				// const float min_tao_init=b/v_sq;
				const float min_tao =
					tao + delta_t; // min_tao_init;//(min_tao_init+tao)/2;
				const float x_i_min = x_i.x + min_tao * v_ix;
				const float z_i_min = x_i.z + min_tao * v_iz;
				const float x_j_min = x_j.x + min_tao * v_jx;
				const float z_j_min = x_j.z + min_tao * v_jz;
				float min_tao_dist = sqrtf((x_i_min - x_j_min) * (x_i_min - x_j_min) +
					(z_i_min - z_j_min) * (z_i_min - z_j_min));
				float d = min_tao_dist;
				float f = d - collision_margin;
				if (f < 0 && d > _EPSILON)
				{
					const float clamp_tao = exp(-min_tao * min_tao / 5.);
					const float k = sim->friction_constraint_stiffness; // 0.25;
					contact_normal.x = 0.;
					contact_normal.z = 0.;
					tangential_displacement.x = 0.;
					tangential_displacement.z = 0.;
					x_pred_w_delta1.x = 0.;
					x_pred_w_delta1.z = 0.;
					x_pred_w_delta2.x = 0.;
					x_pred_w_delta2.z = 0.;
					out.x = 0.;
					out.z = 0.;
					contact_normal.x = (x_i_min - x_j_min) / d;
					contact_normal.z = (z_i_min - z_j_min) / d;
					delta_X[0].x = -k * clamp_tao * w_i_coef * contact_normal.x * f;
					delta_X[0].z = -k * clamp_tao * w_i_coef * contact_normal.z * f;
					delta_X[1].x = -k * clamp_tao * w_j_coef * contact_normal.x * f;
					delta_X[1].z = -k * clamp_tao * w_j_coef * contact_normal.z * f;
					active = true;

					const float x_i_tao = x_i.x + tao * v_ix;
					const float z_i_tao = x_i.z + tao * v_iz;
					const float x_j_tao = x_j.x + tao * v_jx;
					const float z_j_tao = x_j.z + tao * v_jz;
					x_pred_w_delta1.x = delta_X[0].x + x_i_min;  // min -> hat
					x_pred_w_delta1.z = delta_X[0].z + z_i_min;
					x_pred_w_delta2.x = delta_X[1].x + x_j_min;
					x_pred_w_delta2.z = delta_X[1].z + z_j_min;

					float n_norm = distance(x_pred_w_delta1, x_pred_w_delta2);
					contact_normal.z = (x_pred_w_delta1.x - x_pred_w_delta2.x) / n_norm;
					contact_normal.x = -(x_pred_w_delta1.z - x_pred_w_delta2.z) / n_norm;

					tangential_displacement.x =
						x_pred_w_delta1.x - x_i_tao - (x_pred_w_delta2.x - x_j_tao);
					tangential_displacement.z =
						x_pred_w_delta1.z - z_i_tao - (x_pred_w_delta2.z - z_j_tao);

					project_on_vector(tangential_displacement, contact_normal, out);

					float out_norm = norm(out);

					if (out_norm >= mui_static * d)
					{
						float coef = min(1., mui_kinematic * d / out_norm);
						out.x *= coef;
						out.z *= coef;
					}
					delta_X[0].x += -out.x * w_i_coef;
					delta_X[0].z += -out.z * w_i_coef;
					delta_X[1].x += -out.x * w_j_coef;
					delta_X[1].z += -out.z * w_j_coef;

					active = true;
				}
			}
		}
	}
}
const float Friction_Constraint::mui_static = 0.00026;    // 0.021;
const float Friction_Constraint::mui_kinematic = 0.00023; // 0.02;
#pragma endregion

#pragma region Wall
Wall_Constraint::Wall_Constraint(Simulation* sim, int i, int wall_idx) : Constraint(sim, 1)
{
	this->i = i;
	this->indicies[0] = i;
	this->wall_idx = wall_idx;
	/*this->collision_margin =
		(sim->particles[i]->r + sim->walls[wall_idx]->width) * 1.05;*/
	this->collision_margin =
		(sim->particles[i]->r) * 1.05;
	this->contact_normal = glm::vec3(0., 0., 0.);
}
void Wall_Constraint::project(Particle** particles)
{
	delta_X[0].x = 0.0;
	delta_X[0].z = 0.0;

	// particle
	Particle* particle = particles[i];
	glm::vec3 p = particle->X_pred;
	glm::vec3 p_prev = particle->X;
	glm::vec3 dir = p - p_prev;
	float dir_length = norm(dir);

	// wall
	Wall* wall = sim->walls[wall_idx];
	glm::vec3 wall_anchor = wall->x0;
	glm::vec3 wall_wh = glm::vec3(wall->width, 0, wall->height);

	// intersection (line & Rect의 교차점은 최대 2개)
	glm::vec3 two_points[2] = {
		glm::vec3(INFINITE, 0, INFINITE),
		glm::vec3(INFINITE, 0, INFINITE) };
	glm::vec3 intersection = glm::vec3(0, 0, 0);  // 교차점 2개 중 가까운 위치를 선택

	// 현재위치 + 가고자 하는 곳
	glm::vec3 look_ahead = p_prev + dir * 2.0f;

	// short 충돌검사
	intersect_line_rect_f2(p_prev, p, wall_anchor, wall_wh, two_points);

	// 충돌 지점이 두 곳이라면 현재 위치에 더 가까운 지점 선택
	float dist1 = distance(two_points[0], p_prev);
	float dist2 = distance(two_points[1], p_prev);
	intersection = dist1 < dist2 ? two_points[0] : two_points[1];

	if (intersection.x != INFINITE && intersection.z != INFINITE)
	{
		glm::vec3 normal = wall->get_normal(intersection);
		glm::vec3 penetrate = intersection - look_ahead;  // wall을 파고든 vector
		float penetrate_length = norm(penetrate);

		//contact_normal = penetrate / penetrate_length;
		//delta_X[0] = contact_normal * penetrate_length;
		//active = true;

		contact_normal = (penetrate / penetrate_length) + normal * 2.0f;  // reflect
		delta_X[0] = (contact_normal / norm(contact_normal)) * penetrate_length;

		active = true;
	}
	//else
	//{
	//	// 고정 길이 만큼의 앞을 바라본다.
	//	look_ahead = p_prev + (dir / norm(dir)) * particle->look_length;

	//	// short 충돌검사
	//	intersect_line_rect_f2(p_prev, look_ahead, wall_anchor, wall_wh, two_points);

	//	// 충돌 지점이 두 곳이라면 현재 위치에 더 가까운 지점 선택
	//	float dist1 = distance(two_points[0], p_prev);
	//	float dist2 = distance(two_points[1], p_prev);
	//	intersection = dist1 < dist2 ? two_points[0] : two_points[1];

	//	if(intersection.x != INFINITE && intersection.y != INFINITE)
	//	{
	//		float2 normal = wall->get_normal(intersection);
	//		float2 penetrate = intersection - look_ahead;  // wall을 파고든 vector
	//		float penetrate_length = norm(penetrate);

	//		contact_normal = (penetrate / penetrate_length) + normal * 2.0f;  // reflect
	//		delta_X[0] = contact_normal * penetrate_length * sim->time_step;
	//		clamp(delta_X[0], MAX_ACCEL * sim->time_step);

	//		active = true;
	//	}
	//}
}
const float Wall_Constraint::kinematic_friction = 30000.7 * 9.81 * MS_PER_UPDATE * MS_PER_UPDATE;
#pragma endregion

#pragma region Ground
Ground_Constraint::Ground_Constraint(Simulation* sim, int i) : Constraint(sim, 1) {
	this->i = i;
	this->indicies[0] = i;
}
void Ground_Constraint::project(Particle** particles)
{
	// we don't want to use the bad old values
	delta_X[0] = VEC_ZERO;
	float f = distance_ground(particles[i]->X_pred, particles[i]->r, GROUND_HEIGHT + 30);

	if (f < 0)
	{
		float x_d = particles[i]->X_pred.x - particles[i]->X.x;

		if (x_d > kinematic_friction)
		{
			delta_X[0].x = -kinematic_friction;
		}
		else {
			delta_X[0].x = -x_d;
		}

		delta_X[0].z = f;
		active = true;
	}
}
const float Ground_Constraint::kinematic_friction = 30000.7 * 9.81 * MS_PER_UPDATE * MS_PER_UPDATE;
#pragma endregion

#pragma region Distance
Distance_Constraint::Distance_Constraint(Simulation* sim, int i, int j) : Constraint(sim, 2)
{
	this->i = i;
	this->j = j;
	this->indicies[0] = i;
	this->indicies[1] = j;
	this->w_i_coef =
		-sim->particles[i]->inv_mass /
		(sim->particles[i]->inv_mass + sim->particles[j]->inv_mass);
	this->w_j_coef =
		sim->particles[j]->inv_mass /
		(sim->particles[i]->inv_mass + sim->particles[j]->inv_mass);
	this->collision_margin =
		(sim->particles[i]->r + sim->particles[j]->r) * 1.05f;
	this->contact_normal = glm::vec3(0.0, 0.0, 0.0);
	this->radius_init = (sim->particles[i]->r + sim->particles[j]->r);
	this->radius_sq_init = radius_init * radius_init;
	this->delta_t = sim->time_step;
	this->dist = 3.0f;//(float)(sim->num_particles) / (_M_PI * 5.0f);
}
void Distance_Constraint::project(Particle** particles)
{
	//if (!IS_FORMATION)
		return;

	delta_X[0] = VEC_ZERO;
	delta_X[1] = VEC_ZERO;

	Particle* particle_i = particles[i];
	Particle* particle_j = particles[j];

	float d = distance(particle_i->X_pred, particle_j->X_pred);
	float f = d - collision_margin;
	float accel_limit = MAX_ACCEL * sim->time_step;

	if (particle_i->group_id != particle_j->group_id)
	{
		particles[i]->is_link = false;
		particles[j]->is_link = false;
	}

	return;

	/*if (particle_i->group_id == particle_j->group_id)
	{
		if (!particle_i->is_link || !particle_j->is_link)
			return;

		if (abs(particle_i->row_id - particle_j->row_id) < 2 && abs(particle_i->col_id - particle_j->col_id) < 2)
		{
			contact_normal = (particle_i->X_pred - particle_j->X_pred) / d;

			delta_X[0] = contact_normal * w_i_coef * (d - dist);
			delta_X[1] = contact_normal * w_j_coef * (d - dist);

			//clamp(delta_X[0], accel_limit);
			//clamp(delta_X[1], accel_limit);

			active = true;
		}
	}*/
}
#pragma endregion

#pragma region Mesh
Mesh_Constraint::Mesh_Constraint(Simulation* sim, int i) : Constraint(sim, 1)
{
	this->i = i;
	this->w_i_coef =
		-sim->particles[i]->inv_mass /
		(sim->particles[i]->inv_mass + sim->particles[i]->inv_mass);
	this->indicies[0] = i;
	this->dist = 0.2f;
	this->contact_normal = glm::vec3(0.0, 0.0, 0.0);
	this->delta_t = sim->time_step;
	this->accel_limit = MAX_ACCEL * delta_t * delta_t;
}
void Mesh_Constraint::project(Particle** particles)
{
	if (!IS_FORMATION)
		return;

	delta_X[0] = VEC_ZERO;

	Particle* particle = particles[i];

	if (particle->is_leader)
		return;

	/*if (!particle->is_link)
	{
		return;
	}*/

	float f = distance(particle->X, particle->offset);
	float k = 1.0f;

	if (f > dist)
	{
		contact_normal = (particle->X - particle->offset);
		delta_X[0] += contact_normal * w_i_coef * k;
		clamp(delta_X[0], accel_limit);

		active = true;
	}
}
#pragma endregion

