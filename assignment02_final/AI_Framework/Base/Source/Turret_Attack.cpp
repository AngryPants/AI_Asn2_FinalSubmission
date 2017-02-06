#include "Turret_Attack.h"
#include "Turret.h"
#include "CharacterInfo.h"
#include "MessageBoard.h"

Turret_Attack::Turret_Attack(Turret& owner) : Turret_State("Attack", owner)
{
}

Turret_Attack::~Turret_Attack()
{
}

bool Turret_Attack::Think()
{
	if (GetOwner().GetNumBullet() == 0)
	{
		GetOwner().SetNextState("Reload");
		return false;
	}
	else if (enemyInfos.empty())
	{
		GetOwner().SetNextState("Idle");
		return false;
	}

	return true;
}

void Turret_Attack::Act(double dt)
{
	if (TargetCurrent() == false)
	{
		TargetClosest();
	}

	Vector2 direction = targetPosition - GetOwner().position;
	float distance = direction.Length();
	if (distance > Math::EPSILON) {
		direction.Normalize();
		GetOwner().rotation = Math::RadianToDegree(atan2(direction.y, direction.x));
	}

	GetOwner().Shoot();
}

void Turret_Attack::Update(double dt)
{
	if (Think())
	{
		Act(dt);
	}
	enemyInfos.clear();
}

bool Turret_Attack::TargetCurrent() {
	if (enemyInfos.empty()) {
		return false;
	}

	for (unsigned int i = 0; i < enemyInfos.size(); ++i) {
		if (enemyInfos[i].senderID == currentTargetID) {
			targetPosition = enemyInfos[i].position;
			return true;
		}
	}

	return false;
}

void Turret_Attack::TargetClosest() {
	if (enemyInfos.empty()) {
		return;
	}

	targetPosition = enemyInfos[0].position;
	float closestDistanceSquared = (targetPosition - GetOwner().position).LengthSquared();
	currentTargetID = enemyInfos[0].senderID;
	for (unsigned int i = 1; i < enemyInfos.size(); ++i) {
		float distanceSquared = (enemyInfos[i].position - GetOwner().position).LengthSquared();
		if (distanceSquared < closestDistanceSquared) {
			closestDistanceSquared = distanceSquared;
			targetPosition = enemyInfos[i].position;
			currentTargetID = enemyInfos[i].senderID;
		}
	}
}

void Turret_Attack::ReceiveMessage(const Message* messagePtr)
{
	const CharacterInfo* charInfo = dynamic_cast<const CharacterInfo*>(messagePtr);
	if (charInfo != nullptr && charInfo->isAlive)
	{
		float distanceSquared = (charInfo->position - GetOwner().position).LengthSquared();
		if (distanceSquared < GetOwner().GetMaxRange() * GetOwner().GetMaxRange())
		{
				enemyInfos.push_back(*charInfo);
				MessageBoard::GetInstance()->UpdateLogReceiver(messagePtr->messageLogID, GetOwner().name);
		}
	}
}