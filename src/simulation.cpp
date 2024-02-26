#include "simulation.h"
#include "simulation/agent.h"
#include "simulation/group.h"
#include "simulation/grid.h"
#include "simulation/constraint.h"
#include "simulation/formation.h"

SimulationUPtr Simulation::Create(const uint8_t& InNumGroups, const std::vector<uint32_t>& InNumAgents)
{
	SimulationUPtr OutSimulation = SimulationUPtr(new Simulation());

	if(!OutSimulation->Init(InNumGroups, InNumAgents))
	{
		return nullptr;
	}

	SPDLOG_INFO("Success Create Simulation");

	return std::move(OutSimulation);
}

bool Simulation::Init(const uint8_t& InNumGroups, const std::vector<uint32_t>& InNumAgents)
{
	if(InNumGroups != InNumAgents.size())
	{
		SPDLOG_ERROR("Diff Size : Groups : {}, Agents : {}", InNumGroups, InNumAgents.size());
		return false;
	}

	// Groups, Agents
	NumGroups = InNumGroups;
	NumAgents = 0;
	
	// Set Base Position
	std::vector<glm::vec3> GroupPositions;
	GroupPositions.push_back(glm::vec3(-200.0f, 0, 0));
	GroupPositions.push_back(glm::vec3( 200.0f, 0, 0));

	std::vector<glm::vec3> GroupColors;
	GroupColors.push_back(glm::vec3(1, 0, 0));
	GroupColors.push_back(glm::vec3(0, 0, 1));

	for (uint8_t GroupId = 0; GroupId < NumGroups; GroupId++)
	{
		uint32_t CreateAgentCount = InNumAgents[GroupId];

		// Create Group (Init Group SRD)
		float GroupSpeed = 1.0f;
		Groups.push_back(GroupFactory::Create(GroupId, GroupPositions[GroupId], GroupSpeed, GroupColors[GroupId]));

		// Create Formation
		FormationUPtr TempFormation = Formation::CreateRectFormation(CreateAgentCount, 3.0f);

		for (uint32_t AgentId = NumAgents; AgentId < NumAgents + CreateAgentCount; AgentId++)
		{
			//Create Agents
			uint32_t RefAgentId = AgentId - NumAgents;  // (0 ~ )
			float Mass = 1.0f;
			float Radius = 1.0f;
			float PreferedSpeed = 1.0f;
			glm::vec3 Position = Groups[GroupId].GetPosition() + TempFormation->GetPositions()[RefAgentId];  // Group Position + Formation Position

			Agents.push_back(AgentFactory::Create(AgentId, GroupId, Mass, Radius, PreferedSpeed, Position));
		}
		
		NumAgents += CreateAgentCount;
	}
	SPDLOG_INFO("Success Set Agent & Group");

	// Grid
	glm::vec3 MinBound = glm::vec3(-GRID_BOUND_X, 0, -GRID_BOUND_Z);
	glm::vec3 MaxBound = glm::vec3(GRID_BOUND_X, 0, GRID_BOUND_Z);
	GridField = GridUPtr(new Grid(GRID_DENSITY, MinBound, MaxBound));
	SPDLOG_INFO("Success Set Grid");

	return true;
}

void Simulation::SetFormation(const uint8_t& InGroupId, const glm::vec3& InRotateAxis, const float& InScale)
{
	// Find Agent in Group
	std::vector<Agent*> AgentsInGroup;
	for(Agent& IterAgent : Agents)
	{
		if(IterAgent.GroupId == InGroupId)
		{
			AgentsInGroup.push_back(&IterAgent);
		}
	}
}

// void Simulation::SetFormation(const uint8_t& InGroupId, const glm::vec3& InRotateAxis, const float& InScale)
// {
// 	Formation_t DummyPositions = InFormation;
// 	// std::copy(InFormation.begin(), InFormation.end(), DummyPositions.begin());

// 	//SPDLOG_INFO("FormationSize : {}, GroupId : {}, Scale : {}", InFormation.size(), InGroupId, InScale);

// 	// Find Agent in Group
// 	std::vector<Agent*> AgentsInGroup;
// 	for(Agent& IterAgent : Agents)
// 	{
// 		if(IterAgent.GroupId == InGroupId)
// 		{
// 			AgentsInGroup.push_back(&IterAgent);
// 		}

// 	FormationUPtr DummyPosition = Formation::CreateRectFormation(AgentsInGroup.size(), 3.0f);

// 	// Rodate, Scale
// 	glm::mat4 MatRotateX = glm::rotate(glm::identity<glm::mat4>(), Deg2Rad(90.0f * InRotateAxis.x), glm::vec3(1, 0, 0));
// 	glm::mat4 MatRotateY = glm::rotate(glm::identity<glm::mat4>(), Deg2Rad(90.0f * InRotateAxis.y), glm::vec3(0, 1, 0));
// 	glm::mat4 MatRotateZ = glm::rotate(glm::identity<glm::mat4>(), Deg2Rad(90.0f * InRotateAxis.z), glm::vec3(0, 0, 1));
// 	glm::mat4 MatScale = glm::scale(glm::identity<glm::mat4>(), VEC_ONE * InScale * (GRID_DENSITY / 3.2f));

// 	// Projection to bottom (y = 0)
// 	for(glm::vec3& IterDummyPosition : DummyPositions)
// 	{
// 		glm::vec4 TransformedPos = glm::vec4(IterDummyPosition, 0);

// 		TransformedPos = MatScale * TransformedPos;

// 		TransformedPos = MatRotateX * TransformedPos;
// 		TransformedPos = MatRotateY * TransformedPos;
// 		TransformedPos = MatRotateZ * TransformedPos;

// 		IterDummyPosition.x = TransformedPos.x;
// 		IterDummyPosition.y = 0;
// 		IterDummyPosition.z = TransformedPos.z;
// 	}

// 	// Remove overlapping position
// 	UniqueVertices(DummyPositions);

// 	// Agents < DummyPositions
// 	uint32_t NumVertices = DummyPositions.size();
// 	uint32_t Div = std::max((int)(NumVertices / AgentsInGroup.size()), 1);

// 	uint32_t AgentIndex = 0;
// 	for (int i = 0; i < NumVertices; i += Div)
// 	{
// 		glm::vec3 FormationPosition= glm::vec3(DummyPositions[i]);

// 		if ( (i / Div) < NumVertices )
// 		{
// 			AgentsInGroup[AgentIndex]->Position = FormationPosition;
			
// 			AgentIndex++;
// 		}
// 	}
// }

Simulation::Simulation()
{
	// Stiffness
	CollisionConstraintStiffness = 0.22f;
}

Simulation::~Simulation() 
{
	Groups.clear();
	Agents.clear();
	GridField.reset();
}

void Simulation::DrawPath(const glm::vec3& Waypoint)
{
	Groups[DrawPathId].DrawPath(Waypoint);
}

glm::vec3 Simulation::GetAgentPosition(const uint32_t InAgentId)
{
	return Agents[InAgentId].Position;
}

glm::vec3 Simulation::GetGroupColor(const uint8_t InGroupId)
{
	return Groups[InGroupId].GetColor();
}

glm::vec3 Simulation::GetGroupColor(const uint32_t InAgentId)
{
	return Groups[ Agents[InAgentId].GroupId ].GetColor();
}

void Simulation::GetWaypoints(std::vector<glm::vec3>& OutPath)
{
	Groups[DrawPathId].GetWaypoints(OutPath);
}

void Simulation::CalcStiffness(int n)
{
    CollisionConstraintStiffness = 1.0f - powf(1.0f - CollisionConstraintStiffness, (1.0f / n));
}

void Simulation::InitAgentDelta()
{
	for(Agent& IterAgent : Agents)
	{
		IterAgent.DeltaPosition = VEC_ZERO;
		IterAgent.DeltaPositionCounter = 0;
	}
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
		IterAgent.SRD += Groups[IterAgent.GroupId].GetVelocity();
		IterAgent.PlanVelocity();
	}
}
void Simulation::UpdateLocalInformation()
{	
	GridField->Update(Agents);
}
void Simulation::TriggerAvoidanceConstraint()
{
	InitAgentDelta();

	for (Agent &OutAgent : Agents)
	{
		uint8_t SearchRange = 2;
		std::vector<uint32_t> Neighbors = GridField->GetNeighborAgents(OutAgent, SearchRange);

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
			std::vector<uint32_t> Neighbors = GridField->GetNeighborAgents(IterAgent, SearchRange);

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

	// // 2. searching neighboring
    // UpdateLocalInformation();

	// // 3. long_range constraint (Avoidance) (4.4, 4.5)
	// TriggerAvoidanceConstraint();

	// // 4. Collision (Penetration) Constraint
    // TriggerCollisionConstraint();

	// // 4-1. SRD Constraint
	// TriggerSRDConstraint();

    // // 5. Real Translate Agent Position
	UpdateFinalPosition();

	StepNo++;
}