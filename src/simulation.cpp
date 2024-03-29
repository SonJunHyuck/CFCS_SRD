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
	
	// Set Params
	std::vector<glm::vec3> GroupPositions;
	GroupPositions.push_back(glm::vec3(-10.0f, 0, 0));
	GroupPositions.push_back(glm::vec3( 10.0f, 0, 0));

	std::vector<glm::vec3> GroupColors;
	GroupColors.push_back(glm::vec3(1, 0, 0));
	GroupColors.push_back(glm::vec3(0, 0, 1));

	std::vector<Formation::Direction> FormationDirection;
	FormationDirection.push_back(Formation::Direction::RIGHT_TO_LEFT);
	FormationDirection.push_back(Formation::Direction::LEFT_TO_RIGHT);

	// Create Objects (Group, Agent)
	for (uint8_t GroupId = 0; GroupId < NumGroups; GroupId++)
	{
		uint32_t CreateAgentCount = InNumAgents[GroupId];

		// Create Group (Init Group SRD)
		float GroupSpeed = 1.0f;
		Groups.push_back(GroupFactory::Create(GroupId, GroupPositions[GroupId], GroupSpeed, GroupColors[GroupId]));
		FormationUPtr TempFormation = Formation::CreateRectFormation(CreateAgentCount, Groups[GroupId].GetPosition(), 8, GRID_DENSITY + 1.0f, FormationDirection[GroupId]);

		glm::vec3 NewGroupPosition = VEC_ZERO;
		for (uint32_t AgentId = NumAgents; AgentId < NumAgents + CreateAgentCount; AgentId++)
		{
			//Create Agents
			uint32_t RefAgentId = AgentId - NumAgents;  // (0 ~ )
			float Mass = 1.0f;
			float Radius = 1.0f;
			float PreferedSpeed = 1.0f;
			glm::vec3 Position = TempFormation->GetPositions()[RefAgentId];  // Group Position + Formation Position

			Agents.push_back(AgentFactory::Create(AgentId, GroupId, Mass, Radius, PreferedSpeed, Position));
			NewGroupPosition += Position;
		}

		// Set group position to centre
		NewGroupPosition /= CreateAgentCount;
		Groups[GroupId].SetPosition(NewGroupPosition);
		
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

#pragma region PUBLIC
void Simulation::DrawPath(const glm::vec3& Waypoint)
{
	Groups[DrawPathId].DrawPath(Waypoint);
}

void Simulation::SwitchSimulate()
{
	for(Group IterGroup : Groups)
	{
		if(!IterGroup.HasPath())
		{
			SPDLOG_ERROR("Fail Start Simulation : No Group {} Path", IterGroup.GetId());
			return;
		}
	}

	bIsSimulate = !bIsSimulate;
}

// GETS
glm::vec3 Simulation::GetAgentPosition(const uint32_t InAgentId)
{
	return Agents[InAgentId].Position;
}

glm::vec3 Simulation::GetAgentSRD(const uint32_t InAgentId)
{
	return Agents[InAgentId].SRD;
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
#pragma endregion

#pragma region PRIVATE
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
#pragma endregion

#pragma region PRIVATE_SCHEME
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
void Simulation::DetermineBehavior()
{
	for (Agent &IterAgent : Agents)
	{
		int32_t CellId = IterAgent.CellId;
		Grid::Cell::STATE State = static_cast<Grid::Cell::STATE>( GridField->GetCellState(CellId) );
		
		IterAgent.DetermineBehavior(State);
	}
}
void Simulation::TriggerAvoidanceConstraint()
{
	InitAgentDelta();

	for (Agent &HostAgent : Agents)
	{
		uint8_t SearchRange = 2;
		std::vector<int32_t> Neighbors;
		GridField->GetNeighborAgents(HostAgent, SearchRange, Neighbors);

		for (uint32_t IterNeighborId : Neighbors)
		{
			Agent &GuestAgent = Agents[IterNeighborId];
			AvoidConstraint(HostAgent, GuestAgent);
		}
	}

	// predict(Particle) <- correction(Constraint) 
	UpdatePredictedPosition();
}
void Simulation::TriggerFrictionConstraint()
{
	for (int i = 1; i < IterateCount + 1; i++)
	{
		CalcStiffness(i);

		// init to 0
		InitAgentDelta();

		for (Agent &IterAgent : Agents)
		{
			uint8_t SearchRange = 1;
			std::vector<int32_t> Neighbors;
			GridField->GetNeighborAgents(IterAgent, SearchRange, Neighbors);

			//SPDLOG_INFO("Neighbor Count : {} -> {}", IterAgent.Id, Neighbors.size());
			for (uint32_t IterNeighborId : Neighbors)
			{
				if (IterAgent.Id < IterNeighborId)
				{
					Agent &OutGuestAgent = Agents[IterNeighborId];
					FrictionConstraint(IterAgent, OutGuestAgent, CollisionConstraintStiffness);
				}
			}
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

	// 2-1. 
	DetermineBehavior();

	// 3. long_range constraint (Avoidance) (4.4, 4.5)
	TriggerAvoidanceConstraint();

	// 4. Collision (Penetration) Constraint
    TriggerFrictionConstraint();

	// 4-1. SRD Constraint
	TriggerSRDConstraint();

    // 5. Real Translate Agent Position
	UpdateFinalPosition();

	StepNo++;
}