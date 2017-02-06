#include "Brawler_Attack.h"
#include "Brawler.h"
#include "CharacterInfo.h"
#include "MessageBoard.h"

Brawler_Attack::Brawler_Attack(Brawler& owner) : Brawler_State("Attack", owner)
{
	currentTargetID = -1;
}

Brawler_Attack::~Brawler_Attack()
{
}

bool Brawler_Attack::Think()
{
	if (static_cast<float>(GetOwner().GetHealth()) / static_cast<float>(GetOwner().GetMaxHealth()) <= 0.2f)
	{
		int random = Math::RandInt() % 10;
		if (random > 2)
		{
			GetOwner().SetNextState("Run");
			return false;
		}
	}

	if (enemyInfos.empty()) {
		GetOwner().SetNextState("Move");
		return false;
	}

	return true;
}

void Brawler_Attack::Act(double dt)
{
	if (TargetCurrent() == false)
	{
		TargetClosest();
	}

	Vector2 direction = targetPosition - GetOwner().position;
	float distance = direction.Length();
	if (distance > Math::EPSILON)
	{
		direction.Normalize();
		GetOwner().rotation = Math::RadianToDegree(atan2(direction.y, direction.x));
	}

	GetOwner().Attack(currentTargetID);
}
bool Brawler_Attack::TargetCurrent() {
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
void Brawler_Attack::TargetClosest() {
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

void Brawler_Attack::Update(double dt)
{
	if (Think())
	{
		Act(dt);
	}
	enemyInfos.clear();
}

void Brawler_Attack::ReceiveMessage(const Message* messagePtr)
{
	const CharacterInfo* charInfo = dynamic_cast<const CharacterInfo*>(messagePtr);
	if (charInfo != nullptr && charInfo->isAlive && charInfo->teamNumber != GetOwner().teamNumber)
	{
		float distanceSquared = (charInfo->position - GetOwner().position).LengthSquared();
		if (distanceSquared < GetOwner().GetMaxRange() * GetOwner().GetMaxRange())
		{
			enemyInfos.push_back(*charInfo);

			MessageBoard::GetInstance()->UpdateLogReceiver(messagePtr->messageLogID, GetOwner().name);
		}
	}
}
