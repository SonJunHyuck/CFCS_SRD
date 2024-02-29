#pragma once

#include "../common.h"

// extern float DELTA_TIME;
// extern float MAX_ACCEL;
// extern bool ON_AVOIDANCE_MODEL;

class Agent;

const float COLLISION_MUI_STATIC = 0.00026f;
const float COLLISION_MUI_KINEMATIC = 0.00023f;
const float COLLISION_LimitAccel = MAX_ACCEL * DELTA_TIME;
void CollisionConstraint(Agent& OutAgent1, Agent& OutAgent2, const float& InStiffness);

const float AVOIDANCE_STIFFNESS = 1.5f;
const float AVOIDANCE_TAO0 = 10.0f;
const float AVOIDANCE_LimitAccel = MAX_ACCEL * DELTA_TIME;
void AvoidConstraint(Agent& OutAgent1, Agent& OutAgent2);

const float SRD_STIFFNESS = 1.0f;
const float SRD_HoldingDistance = 2.2f;
const float SRD_LimitAccel = MAX_ACCEL * DELTA_TIME * DELTA_TIME;
void SRDConstraint(Agent& OutAgent);