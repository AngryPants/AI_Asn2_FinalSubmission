#include "Guardian_Attack.h"
#include "Guardian.h"
#include "MessageBoard.h"

//Include Messages
#include "RequestHelp.h"
#include "CharacterInfo.h"
#include "MessageReply.h"

//Become a sentry turret.
Guardian_Attack::Guardian_Attack(Guardian& _owner) : Guardian_State("Attack", _owner) {
	switchState = false;
}

Guardian_Attack::~Guardian_Attack() {
}

bool Guardian_Attack::Think() {
	if (switchState) {
		GetOwner().SetNextState("Follow");
		return false;
	}
	return true;
}

void Guardian_Attack::Act(double _deltaTime) {
	if (enemyInfos.empty() == false) {
		Vector2 targetPosition = enemyInfos[0].position;
		float closestDistanceSquared = (targetPosition - GetOwner().position).LengthSquared();
		for (unsigned int i = 1; i < enemyInfos.size(); ++i) {
			float distanceSquared = (enemyInfos[i].position - GetOwner().position).LengthSquared();
			if (distanceSquared < closestDistanceSquared) {
				closestDistanceSquared = distanceSquared;
				targetPosition = enemyInfos[i].position;
			}
		}

		Vector2 direction = targetPosition - GetOwner().position;
		float distance = direction.Length();
		if (distance > Math::EPSILON) {
			direction.Normalize();
			GetOwner().rotation = Math::RadianToDegree(atan2(direction.y, direction.x));
		}
		GetOwner().Shoot();
	}
}

void Guardian_Attack::Update(double _deltaTime) {
	if (Think()) {
		Act(_deltaTime);
	}
	enemyInfos.clear();
	switchState = false;
}

void Guardian_Attack::ReceiveMessage(const Message* _messagePtr) {
	//Get information about the characters
	const CharacterInfo* charInfo = dynamic_cast<const CharacterInfo*>(_messagePtr);
	if (charInfo != nullptr && charInfo->isAlive) {
		if (charInfo->teamNumber == GetOwner().teamNumber) {
			if (charInfo->name == "Attacker" || charInfo->name == "Support") {
				switchState = true;
				MessageBoard::GetInstance()->UpdateLogReceiver(_messagePtr->messageLogID, GetOwner().name);
			}
		} else {
			float distance = (charInfo->position - GetOwner().position).LengthSquared();
			if (distance < GetOwner().GetCombatRange() * GetOwner().GetCombatRange()) {
				enemyInfos.push_back(*charInfo);
				MessageBoard::GetInstance()->UpdateLogReceiver(_messagePtr->messageLogID, GetOwner().name);
			}
		}
	}
}