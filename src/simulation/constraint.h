#pragma once

#include "../common.h"

class Agent;

const float FRICTION_MUI_STATIC = 0.00026f;
const float FRICTION_MUI_KINEMATIC = 0.00023f;
const float FRICTION_LimitAccel = MAX_ACCEL * DELTA_TIME;
void FrictionConstraint(Agent& OutAgent1, Agent& OutAgent2, const float& InStiffness);

const float AVOIDANCE_STIFFNESS = 1.5f;
const float AVOIDANCE_TAO0 = 10.0f;
const float AVOIDANCE_LimitAccel = MAX_ACCEL * DELTA_TIME;
void AvoidConstraint(Agent& OutAgent1, Agent& OutAgent2);

void StabilityConstraint(Agent& OutAgent1, Agent& OutAgent2);

const float SRD_STIFFNESS = 1.0f;
const float SRD_HoldingDistance = 0.1f;
const float SRD_LimitAccel = MAX_ACCEL * DELTA_TIME * DELTA_TIME;
void SRDConstraint(Agent& OutAgent);