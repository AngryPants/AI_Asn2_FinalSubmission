#include "Attacker_Attack.h"
#include "Attacker.h"

#include "CharacterInfo.h"
#include "MessageBoard.h"

//Constructor(s) & Destructor
Attacker_Attack::Attacker_Attack(Attacker& _owner) : Attacker_State("Attack", _owner) {
	hasSupport = true;
	currentMode = ATTACK_CLOSEST;
	currentTargetID = -1;
}

Attacker_Attack::~Attacker_Attack() {
}

//Private Function(s)
bool Attacker_Attack::Think() {	
	if (hasSupport && static_cast<float>(GetOwner().GetHealth()) / static_cast<float>(GetOwner().GetMaxHealth()) <= 0.3f) {
		int randomInt = Math::RandInt() % 10;
		GetOwner().SetNextState("Run");
		return false;
	}

	//If there are no enemies nearby, move.
	if (enemyInfos.empty()) {
		GetOwner().SetNextState("Move");
		return false;
	}

	//If no bullets, reload.
	if (GetOwner().GetNumBullets() == 0) {
		GetOwner().SetNextState("Reload");
		return false;
	}

	return true;
}

//Shoot
void Attacker_Attack::Act(double _deltaTime) {
	//If we no longer have our current target, randomly select an attack mode.
	if (TargetCurrent() == false) {
		currentMode = static_cast<ATTACK_MODE>(Math::RandIntMinMax(0, static_cast<int>(NUM_ATTACK_MODE) - 1));
		switch (currentMode) {
			case ATTACK_CLOSEST:
				TargetClosest();
				break;
			case ATTACK_FURTHEST:
				TargetFurthest();
				break;
			case ATTACK_LOWEST_HP:
				TargetLowestHP();
				break;
			case ATTACK_HIGHEST_HP:
				TargetHighestHP();
				break;
		}
	}

	Vector2 direction = targetPosition - GetOwner().position;
	float distance = direction.Length();
	if (distance > Math::EPSILON) {
		direction.Normalize();
		GetOwner().rotation = Math::RadianToDegree(atan2(direction.y, direction.x));
	}
	GetOwner().Shoot(10.0f);
}

bool Attacker_Attack::TargetCurrent() {
	if(enemyInfos.empty()) {
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

void Attacker_Attack::TargetClosest() {
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

void Attacker_Attack::TargetFurthest() {
	if(enemyInfos.empty()) {
		return;
	}

	targetPosition = enemyInfos[0].position;
	float closestDistanceSquared = (targetPosition - GetOwner().position).LengthSquared();
	currentTargetID = enemyInfos[0].senderID;
	for(unsigned int i = 1; i < enemyInfos.size(); ++i) {
		float distanceSquared = (enemyInfos[i].position - GetOwner().position).LengthSquared();
		if(distanceSquared > closestDistanceSquared) {
			closestDistanceSquared = distanceSquared;
			targetPosition = enemyInfos[i].position;
			currentTargetID = enemyInfos[i].senderID;
		}
	}
}

//Work on this one.
void Attacker_Attack::TargetLowestHP() {
	if(enemyInfos.empty()) {
		return;
	}

	targetPosition = enemyInfos[0].position;
	float lowestHP = static_cast<float>(enemyInfos[0].health) / static_cast<float>(enemyInfos[0].maxHealth);
	currentTargetID = enemyInfos[0].senderID;
	for (unsigned int i = 1; i < enemyInfos.size(); ++i) {
		float hp = static_cast<float>(enemyInfos[i].health) / static_cast<float>(enemyInfos[i].maxHealth);
		if (hp < lowestHP) {
			lowestHP = hp;
			this->targetPosition = enemyInfos[i].position;
			currentTargetID = enemyInfos[i].senderID;
		}
	}
}

//And this
void Attacker_Attack::TargetHighestHP() {
	if(enemyInfos.empty()) {
		return;
	}

	targetPosition = enemyInfos[0].position;
	float highestHP = static_cast<float>(enemyInfos[0].health) / static_cast<float>(enemyInfos[0].maxHealth);
	currentTargetID = enemyInfos[0].senderID;
	for(unsigned int i = 1; i < enemyInfos.size(); ++i) {
		float hp = static_cast<float>(enemyInfos[i].health) / static_cast<float>(enemyInfos[i].maxHealth);
		if(hp > highestHP) {
			highestHP = hp;
			this->targetPosition = enemyInfos[i].position;
			currentTargetID = enemyInfos[i].senderID;
		}
	}
}

//Interface Function(s)
void Attacker_Attack::Update(double _deltaTime) {
	if (Think()) {
		Act(_deltaTime);
	}
	hasSupport = false;
	enemyInfos.clear();
}

void Attacker_Attack::ReceiveMessage(const Message* _messagePtr) {
	const CharacterInfo* charInfo = dynamic_cast<const CharacterInfo*>(_messagePtr);
	if (charInfo != nullptr && charInfo->isAlive) {
		if (charInfo->teamNumber == GetOwner().GetTeamNumber()){
			if (charInfo->name == "Support") {
				hasSupport = charInfo->isAlive;
				MessageBoard::GetInstance()->UpdateLogReceiver(_messagePtr->messageLogID, GetOwner().name);
			}
		} else {
			float distanceSquared = (charInfo->position - GetOwner().position).LengthSquared();
			if (distanceSquared < GetOwner().GetMaxCombatRange() * GetOwner().GetMaxCombatRange()) {
				enemyInfos.push_back(*charInfo);
				MessageBoard::GetInstance()->UpdateLogReceiver(_messagePtr->messageLogID, GetOwner().name);
			}
		}
	}
}

Attacker_Attack::ATTACK_MODE Attacker_Attack::GetAttackMode() const {
	return currentMode;
}