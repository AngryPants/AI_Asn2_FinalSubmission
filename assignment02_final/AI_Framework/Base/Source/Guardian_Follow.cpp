#include "Guardian_Follow.h"
#include "Guardian.h"
#include "CharacterInfo.h"
#include "RequestHelp.h"
#include "EnemyDetected.h"
#include "MessageBoard.h"
#include "MyMath.h"

Guardian_Follow::Guardian_Follow(Guardian& _owner) : Guardian_State("Follow", _owner) {
	refreshTimer = 0.0;
}

Guardian_Follow::~Guardian_Follow() {
}

bool Guardian_Follow::Think() {
	if (GetOwner().IsMidMovement() == false) {
		//If we are low HP & Shield, run.
		if (static_cast<float>(GetOwner().GetHealth()) / static_cast<float>(GetOwner().GetMaxHealth()) <= 0.3f
			&& static_cast<float>(GetOwner().GetShield()) / static_cast<float>(GetOwner().GetMaxShield()) <= 0.1f) {
			GetOwner().SetNextState("Run");
			return false;
		}

		//Check if there's anyone to follow, or is everyone dead.
		if (attackerInfos.empty() && supportInfos.empty()) {
			GetOwner().SetNextState("Attack");
			return false;
		}

		if (static_cast<float>(GetOwner().GetShield()) / static_cast<float>(GetOwner().GetMaxShield()) > 0.5f) {
			//Does any attacker need our help?
			for (vector<CharacterInfo>::iterator vectorIter = attackerInfos.begin(); vectorIter != attackerInfos.end(); ++vectorIter) {
				if ((*vectorIter).currentState == "Run" || (*vectorIter).currentState == "Attack") {
					GetOwner().SetNextState("Defend");
					return false;
				}
			}
			//Does any support need our help?
			for (vector<CharacterInfo>::iterator vectorIter = supportInfos.begin(); vectorIter != supportInfos.end(); ++vectorIter) {
				if((*vectorIter).currentState == "Run") {
					GetOwner().SetNextState("Defend");
					return false;
				}
			}
		}
	}

	return true;
}

void Guardian_Follow::Act(double _deltaTime) {
	int row, col;
	Vector2 destination = GetOwner().position;

	//Find the closest guy and follow him.
	bool move = false;
	if (attackerInfos.empty() == false) { //If there's an attacker we follow him.
		float closestDistance = (attackerInfos[0].position - GetOwner().position).LengthSquared();
		Vector2 closestPosition = attackerInfos[0].position;
		for (unsigned int i = 1; i < attackerInfos.size(); ++i) {
			float distance = (attackerInfos[i].position - GetOwner().position).LengthSquared();
			if (distance < closestDistance) {
				closestDistance = distance;
				closestPosition = attackerInfos[i].position;
			}
		}
		destination = closestPosition;
		move = true;
	} else if (supportInfos.empty() == false) { //Otherwise we follow a support.
		float closestDistance = (supportInfos[0].position - GetOwner().position).LengthSquared();
		Vector2 closestPosition = supportInfos[0].position;
		for (unsigned int i = 1; i < supportInfos.size(); ++i) {
			float distance = (supportInfos[i].position - GetOwner().position).LengthSquared();
			if(distance < closestDistance) {
				closestDistance = distance;
				closestPosition = supportInfos[i].position;
			}
		}
		destination = closestPosition;
		move = true;
	}

	if (move) {
		row = static_cast<int>(destination.y + 0.5f) + Math::RandIntMinMax(-1, 1);
		col = static_cast<int>(destination.x + 0.5f) + Math::RandIntMinMax(-1, 1);
		if (refreshTimer < 0.0) {
			GetOwner().Move(_deltaTime, row, col);
			if (!GetOwner().PathEmpty()) {
				refreshTimer = 1.0;
			} else {
			}
		} else {
			GetOwner().Move(_deltaTime);
		}
	}
}

void Guardian_Follow::Update(double _deltaTime) {
	if (Think()) {
		Act(_deltaTime);
	}
	refreshTimer -= _deltaTime;
	attackerInfos.clear();
	supportInfos.clear();
}

void Guardian_Follow::ReceiveMessage(const Message* _messagePtr) {
	const CharacterInfo* charInfo = dynamic_cast<const CharacterInfo*>(_messagePtr);
	if (charInfo != nullptr && charInfo->isAlive) {
		Guardian* owner = &GetOwner();
		if (charInfo->teamNumber == owner->teamNumber) {
			if (charInfo->name == "Attacker") {
				this->attackerInfos.push_back(*charInfo->Copy());
				MessageBoard::GetInstance()->UpdateLogReceiver(_messagePtr->messageLogID, GetOwner().name);
			} else if (charInfo->name == "Support") {
				this->supportInfos.push_back(*charInfo->Copy());
				MessageBoard::GetInstance()->UpdateLogReceiver(_messagePtr->messageLogID, GetOwner().name);
			}
		}
	}

	const EnemyDetected* enemyDetectedPtr = dynamic_cast<const EnemyDetected*>(_messagePtr);
	if (enemyDetectedPtr != nullptr) {
		GetOwner().DetectCharacter(enemyDetectedPtr->enemyID, enemyDetectedPtr->type, enemyDetectedPtr->position);
		MessageBoard::GetInstance()->UpdateLogReceiver(_messagePtr->messageLogID, GetOwner().name);
	}
}