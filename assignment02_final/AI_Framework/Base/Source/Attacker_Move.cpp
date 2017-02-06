#include "Attacker_Move.h"
#include "CharacterInfo.h"
#include "Attacker.h"
#include "EntityManager.h"
#include "MessageBoard.h"

Attacker_Move::Attacker_Move(Attacker& _owner) : Attacker_State("Move", _owner) {
	enemyNearby = false;
}

Attacker_Move::~Attacker_Move() {
}

bool Attacker_Move::Think() {
	if (GetOwner().IsMidMovement() == false) {
		if (enemyNearby) {
			GetOwner().CancelMove();
			GetOwner().SetNextState("Attack");
			return false;
		} else if (GetOwner().GetNumBullets() < GetOwner().GetMaxBullets()) {
			GetOwner().CancelMove();
			GetOwner().SetNextState("Reload");
			return false;
		}
	}

	return true;
}

//Move to a randomspot
void Attacker_Move::Act(double _deltaTime) {
	if (GetOwner().PathEmpty()) {
		//If we haven't detected any enemies, move randomly.
		if (detectedEnemies.empty()) {
			GetOwner().Move(_deltaTime, Math::RandIntMinMax(0, GetOwner().tileSystem.GetNumRows() - 1), Math::RandIntMinMax(0, GetOwner().tileSystem.GetNumColumns() - 1));
		} else {
			//Otherwise go towards the commotion.
			Vector2 closestPosition = detectedEnemies[0].position;
			float closestDistance = (closestPosition - GetOwner().position).LengthSquared();
			for (int i = 1; i < detectedEnemies.size(); ++i) {
				float distance = (detectedEnemies[i].position - GetOwner().position).LengthSquared();
				if (distance < closestDistance) {
					closestDistance = distance;
					closestPosition = detectedEnemies[i].position;
				}
			}

			int row = static_cast<int>(closestPosition.y + 0.5f);
			int col = static_cast<int>(closestPosition.x + 0.5f);
		}
	} else {
		GetOwner().Move(_deltaTime);
	}	
}

void Attacker_Move::Update(double _deltaTime) {
	if (Think()) {
		Act(_deltaTime);
	}
	enemyNearby = false;
	detectedEnemies.clear();
}

void Attacker_Move::ReceiveMessage(const Message* _messagePtr) {
	//Check for nearby enemies.
	const CharacterInfo* infoPtr = dynamic_cast<const CharacterInfo*>(_messagePtr);
	if (infoPtr != nullptr) {
		if (infoPtr->isAlive && infoPtr->teamNumber != GetOwner().GetTeamNumber()) {
			if (enemyNearby == false) {
				float distanceSquared = (infoPtr->position - GetOwner().position).LengthSquared();
				if (distanceSquared < GetOwner().GetMinCombatRange() * GetOwner().GetMinCombatRange()) {
					enemyNearby = true;

					//Imperials above us!
					EnemyDetected enemyDetected(GetOwner().GetID());
					EntityManager::GetInstance()->BroadcastMessage(&enemyDetected);

					MessageBoard::GetInstance()->UpdateLogReceiver(_messagePtr->messageLogID, GetOwner().name);
				}
			}
		}
	}

	//Thanks.
	const EnemyDetected* enemyDetectedPtr = dynamic_cast<const EnemyDetected*>(_messagePtr);
	if (enemyDetectedPtr != nullptr) {
		GetOwner().DetectCharacter(enemyDetectedPtr->enemyID, enemyDetectedPtr->type, enemyDetectedPtr->position);
		detectedEnemies.push_back(*enemyDetectedPtr);
		MessageBoard::GetInstance()->UpdateLogReceiver(_messagePtr->messageLogID, GetOwner().name);
	}
}