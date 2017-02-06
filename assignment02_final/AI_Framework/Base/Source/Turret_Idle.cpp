#include "Turret_Idle.h"
#include "Turret.h"
#include "CharacterInfo.h"
#include "EntityManager.h"
#include "MessageBoard.h"

Turret_Idle::Turret_Idle(Turret &owner) : Turret_State("Idle", owner)
{
	enemyNearby = false;
	directionResetTimer = 0.0;
}

Turret_Idle::~Turret_Idle()
{
}

bool Turret_Idle::Think()
{
	if (GetOwner().GetNumBullet() == 0)
	{
		GetOwner().SetNextState("Reload");
		return false;
	}
	else if (enemyNearby)
	{
		GetOwner().SetNextState("Attack");
		return false;
	}
	return true;
}

void Turret_Idle::Act(double dt)
{
	GetOwner().rotation = Math::RadianToDegree(atan2(direction.y, direction.x));
}

void Turret_Idle::Update(double dt)
{
	directionResetTimer -= dt;
	if (directionResetTimer < 0.0)
	{
		ResetDirection();
		directionResetTimer = 3.0;
	}

	if (Think())
	{
		Act(dt);
	}
	enemyNearby = false;
	detectedEnemies.clear();
}

void Turret_Idle::ResetDirection() {
	do {
		direction.Set(Math::RandFloatMinMax(-1.0f, 1.0f), Math::RandFloatMinMax(-1.0f, 1.0f));
	} while (direction.LengthSquared() < Math::EPSILON);
	direction.Normalize();
}

void Turret_Idle::ReceiveMessage(const Message* messagePtr)
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