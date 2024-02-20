#include "simulation.h"
#include "simulation/agent.h"
#include "simulation/group.h"
#include "simulation/grid.h"
#include "simulation/constraint.h"

Simulation::Simulation(const std::vector<uint8_t>& InNumGroups, const std::vector<uint32_t>& InNumAgents)
{
	if(InNumGroups.size() != InNumAgents.size())
	{
		SPDLOG_ERROR("Diff Size : Groups : {}, Agents : {}", InNumGroups.size(), InNumAgents.size());
		return;
	}
	
	// Create Groups, Agents
	NumGroups = InNumGroups.size();
	NumAgents = 0;

	for(uint8_t GroupId = 0; GroupId < NumGroups; GroupId++)
	{
		for (const uint32_t CreateAgentCount : InNumAgents)
		{
			for (uint32_t AgentId = NumAgents; AgentId < NumAgents + CreateAgentCount; AgentId++)
			{
				Agents.push_back(AgentFactory::CreateAgent(AgentId, GroupId));
			}
			NumAgents += CreateAgentCount;
		}

		// Should Ref Created Agent Id, After have to create Group (with SRD)
	}

	// Grid
	glm::vec3 MinBound = glm::vec3(-400, 0, -400);
	glm::vec3 MaxBound = glm::vec3(400, 0, 400);
	GridField = Grid(GRID_DENSITY, MinBound, MaxBound);

	// Stiffness
	CollisionConstraintStiffness = 0.22f;
}

Simulation::~Simulation() 
{
	
}

void Simulation::DrawPath(const glm::vec3& Waypoint)
{
	Groups[DrawPathGroupId].DrawPath(Waypoint);
}

void Simulation::InitAgentDelta()
{
	for(Agent& IterAgent : Agents)
	{
		IterAgent.DeltaPosition = VEC_ZERO;
		IterAgent.DeltaPositionCounter = 0;
	}
}

void Simulation::CalcStiffness(int n)
{
    CollisionConstraintStiffness = 1.0f - powf(1.0f - CollisionConstraintStiffness, (1.0f / n));
}

void Simulation::UpdatePredictedPosition()
{
	for(Agent& IterAgent : Agents)
	{
		IterAgent.CorrectPosition();
	}
}

#pragma region Scheme
void Simulation::PathFinding()
{
	for(Group& IterGroup : Groups)
	{
		IterGroup.FollowPath();
	}
}
void Simulation::CalcPredictedPosition()
{
	for (Agent &IterAgent : Agents)
	{
		IterAgent.PlanVelocity();
	}
}
void Simulation::UpdateLocalInformation()
{	
	GridField.Update(Agents);
}
void Simulation::TriggerAvoidanceConstraint()
{
	InitAgentDelta();

	for (Agent &OutAgent : Agents)
	{
		uint8_t SearchRange = 2;
		std::vector<uint32_t> Neighbors = GridField.GetNeighborAgents(OutAgent, SearchRange);

		for (uint32_t IterNeighborId : Neighbors)
		{
			if (OutAgent.Id < IterNeighborId)
			{
				Agent& OutGuestAgent = Agents[IterNeighborId];
				AvoidConstraint(OutAgent, OutGuestAgent);
			}
		}
	}

	// predict(Particle) <- correction(Constraint) 
	UpdatePredictedPosition();
}
void Simulation::TriggerCollisionConstraint()
{
	for (int i = 1; i < IterateCount; i++)
	{
		CalcStiffness(i);

		// init to 0
		InitAgentDelta();

		for (Agent &IterAgent : Agents)
		{
			uint8_t SearchRange = 2;
			std::vector<uint32_t> Neighbors = GridField.GetNeighborAgents(IterAgent, SearchRange);

			for (uint32_t IterNeighborId : Neighbors)
			{
				if (IterAgent.Id < IterNeighborId)
				{
					Agent &OutGuestAgent = Agents[IterNeighborId];
					CollisionConstraint(IterAgent, OutGuestAgent, CollisionConstraintStiffness);
				}
			}
		}
		UpdatePredictedPosition();

		// SRD constraints
		for (Agent &IterAgent : Agents)
		{
			SRDConstraint(IterAgent);
		}
		UpdatePredictedPosition();
	}
}
void Simulation::TriggerSRDConstraint()
{
	for (int i = 1; i < IterateCount; i++)
	{
		// init to 0
		InitAgentDelta();

		// SRD constraints
		for (Agent &IterAgent : Agents)
		{
			SRDConstraint(IterAgent);
		}
		UpdatePredictedPosition();
	}
}
void Simulation::UpdateFinalPosition()
{
    // 5. Update Velocity
	for(Agent& IterAgent : Agents)
	{
		glm::vec3 DummyPosition = IterAgent.Position;
		glm::vec3 DeltaPos = IterAgent.PredictedPosition - IterAgent.Position;
		
		IterAgent.Velocity = DeltaPos / DELTA_TIME;

		// if (bIsFTL)
		// {
		// 	// Follow The Leader
		// 	glm::vec3 ftl = glm::vec3(0, 0, 0);

		// 	glm::vec3 d_next = Groups[IterAgent.GroupId].Leader.DeltaPosition;
		// 	float s_damping = 0.8f;

		// 	ftl = (groups[particles[i]->group_id]->leader->Delta_x * s_damping) / time_step;

		// 	particles[i]->V += ftl;
		// }

		if (bIsSteering)
		{
			// Steering Behavior
			float Speed = glm::length(IterAgent.Velocity);
			glm::vec3 DesiredVelocity = (IterAgent.SRD - IterAgent.Position) / DELTA_TIME;
			glm::vec3 Steering = (DesiredVelocity - IterAgent.Velocity);
			ClampVec3(Steering, MAX_ACCEL * DELTA_TIME);

			IterAgent.Velocity += Steering;

			ClampVec3(IterAgent.Velocity, Speed);
		}

		if (bIsCohesion)
		{
			// XSPH viscosity 4.3
			float h = 7.0f;
			float c = 217.0f;
			float weight = 0;
			glm::vec3 cohesion = glm::vec3(0, 0, 0);

			for(Agent& IterAgent2 : Agents)
			{
				if(IterAgent.GroupId == IterAgent2.GroupId)
				{
					float r = glm::distance(IterAgent.Position, IterAgent2.Position);
					if (r <= h && r >= 0)
					{
						weight = (c * pow((h * h - r * r), 3)) / (double)(64.0f * _M_PI * pow(h, 9));

						cohesion += (IterAgent.Velocity - IterAgent2.Velocity) * weight;
					}
				}
			}

			IterAgent.Velocity -= cohesion;
		}

        // Final Decision
		IterAgent.Position = IterAgent.PredictedPosition;
	}
}
#pragma endregion

void Simulation::Update()
{
	if(bIsSimulate == false)
		return;

    // 0. 
    PathFinding();

	// 1.
	CalcPredictedPosition();

	// 2. searching neighboring
    UpdateLocalInformation();

	// 3. long_range constraint (Avoidance) (4.4, 4.5)
	TriggerAvoidanceConstraint();

	// 4. Collision (Penetration) Constraint
    TriggerCollisionConstraint();

	// 4-1. SRD Constraint
	TriggerSRDConstraint();

    // 5. Real Translate Agent Position
	UpdateFinalPosition();

	StepNo++;
}