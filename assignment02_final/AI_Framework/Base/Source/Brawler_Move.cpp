#include "Brawler_Move.h"
#include "Character.h"
#include "Brawler.h"
#include "EntityManager.h"
#include "MessageBoard.h"

Brawler_Move::Brawler_Move(Brawler& owner) : Brawler_State("Move", owner)
{
	enemyNearby = false;
}

Brawler_Move::~Brawler_Move()
{
}

bool Brawler_Move::Think()
{
	if (GetOwner().IsMidMovement() == false)
	{
		if (enemyNearby)
		{
			GetOwner().CancelMove();
			GetOwner().SetNextState("Attack");
			return false;
		}
	}
	return true;
}

void Brawler_Move::Act(double dt)
{
	if (GetOwner().PathEmpty())
	{
		if (detectedEnemies.empty())
		{
			GetOwner().Move(dt, Math::RandIntMinMax(0, GetOwner().tileSystem.GetNumRows() - 1), Math::RandIntMinMax(0, GetOwner().tileSystem.GetNumColumns() - 1));
		}
		else
		{
			Vector2 closestPosition = detectedEnemies[0].position;
			float closestDistance = (closestPosition - GetOwner().position).LengthSquared();
			for (int i = 1; i < detectedEnemies.size(); ++i)
			{
				float distance = (detectedEnemies[i].position - GetOwner().position).LengthSquared();
				if (distance < closestDistance)
				{
					closestDistance = distance;
					closestPosition = detectedEnemies[i].position;
				}
			}

			int row = static_cast<int>(closestPosition.y + 0.5f);
			int col = static_cast<int>(closestPosition.x + 0.5f);
		}
	}
	else
	{
		GetOwner().Move(dt);
	}
}

void Brawler_Move::Update(double dt)
{
	if (Think())
	{
		Act(dt);
	}
	enemyNearby = false;
	detectedEnemies.clear();
}

void Brawler_Move::ReceiveMessage(const Message* messagePtr)
{
	const CharacterInfo* infoPtr = dynamic_cast<const CharacterInfo*>(messagePtr);
	if (infoPtr != nullptr) {
		if (infoPtr->isAlive && infoPtr->teamNumber != GetOwner().GetTeamNumber())
		{
			if (enemyNearby == false)
			{
				float distanceSquared = (infoPtr->position - GetOwner().position).LengthSquared();
				if (distanceSquared < GetOwner().GetMinRange() * GetOwner().GetMinRange())
				{
					enemyNearby = true;
					EnemyDetected enemyDetected(GetOwner().GetID());
					EntityManager::GetInstance()->BroadcastMessage(&enemyDetected);

					MessageBoard::GetInstance()->UpdateLogReceiver(messagePtr->messageLogID, GetOwner().name);
				}
			}
		}
	}

	const EnemyDetected* enemyDetectedPtr = dynamic_cast<const EnemyDetected*>(messagePtr);
	if (enemyDetectedPtr != nullptr)
	{
		GetOwner().DetectCharacter(enemyDetectedPtr->enemyID, enemyDetectedPtr->type, enemyDetectedPtr->position);
		detectedEnemies.push_back(*enemyDetectedPtr);
	}
}