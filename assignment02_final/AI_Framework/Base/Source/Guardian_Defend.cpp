#include "Guardian_Defend.h"
#include "Guardian.h"
#include "MessageReply.h"
#include "EntityManager.h"
#include "MessageBoard.h"

Guardian_Defend::Guardian_Defend(Guardian& _owner) : Guardian_State("Defend", _owner) {
	refreshTimer = 0.0f;
}

Guardian_Defend::~Guardian_Defend() {
}

bool Guardian_Defend::Think() {
	if (GetOwner().IsMidMovement() == false) {
		//If we are low shield
		if (static_cast<float>(GetOwner().GetShield()) / static_cast<float>(GetOwner().GetMaxShield()) <= 0.1f) {
			GetOwner().SetNextState("Follow");
			return false;
		}

		//If there's nobody left to defend.
		if (helpRequests.empty() && attackerInfos.empty() && supportInfos.empty()) {
			GetOwner().SetNextState("Attack");
			return false;
		}

		//Check if anybody is even needs help.
		if (helpRequests.empty()) {
			bool needHelp = false;
			for (unsigned int i = 0; i < attackerInfos.size(); ++i) {
				if (attackerInfos[i].currentState == "Attack" || static_cast<float>(attackerInfos[i].health) / static_cast<float>(attackerInfos[i].maxHealth) <= 0.5f) {
					needHelp = true;
					needsGuarding.push_back(attackerInfos[i]);
				}
			}
			for (unsigned int i = 0; i < supportInfos.size(); ++i) {
				if (static_cast<float>(supportInfos[i].health) / static_cast<float>(supportInfos[i].maxHealth) <= 0.5f) {
					needHelp = true;
					needsGuarding.push_back(supportInfos[i]);
				}
			}
			if (!needHelp) {
				GetOwner().SetNextState("Follow");
				return false;
			}			
		}
	}

	return true;
}

void Guardian_Defend::Act(double _deltaTime) {
	//Let's defend those who need help the most first.
	Vector2 destination;
	if (helpRequests.empty() == false) {
		//Let's help the one closest to us.
		float closestDistance = (helpRequests[0].position - GetOwner().position).LengthSquared();
		Vector2 closestPosition = helpRequests[0].position;
		for (unsigned int i = 1; i < helpRequests.size(); ++i) {
			float distance = (helpRequests[i].position - GetOwner().position).LengthSquared();
			if (distance < closestDistance) {
				closestDistance = distance;
				closestPosition = helpRequests[i].position;
			}
		}
		destination = closestPosition;
	} else {
		if (needsGuarding.empty() == false) {
			//Let's priorities those that are low health and within a reasonable range first.
			//If nobody is within the priorityRange, then go to the one with the lowest health.
			float priorityRange = GetOwner().GetPriorityRange();
			float lowestHealthPercentage = static_cast<float>(needsGuarding[0].health) / static_cast<float>(needsGuarding[0].maxHealth);
			Vector2 lowestHealthPosition = needsGuarding[0].position;

			bool hasPriority = false; //Is there anyone within our priority range?
			float priorityLowestHealthPercentage = 0.4f;
			Vector2 priorityPosition;
			
			for (unsigned int i = 1; i < needsGuarding.size(); ++i) {
				//Find the distance.
				float distance = (needsGuarding[i].position - GetOwner().position).LengthSquared();

				//Find the health Percentage
				float healthPercentage = static_cast<float>(needsGuarding[i].health) / static_cast<float>(needsGuarding[i].maxHealth);
				if(healthPercentage < lowestHealthPercentage) {
					lowestHealthPercentage = healthPercentage;
					priorityPosition = needsGuarding[i].position;
				}

				//Check if the distance is within our priority range.
				if (distance < priorityRange * priorityRange) {				
					if (healthPercentage < priorityLowestHealthPercentage) {
						priorityLowestHealthPercentage = healthPercentage;
						priorityPosition = needsGuarding[i].position;
						hasPriority = true;
					}
				}
			}
			if (hasPriority) {
				destination = priorityPosition - GetOwner().position;
			} else {
				destination = lowestHealthPosition - GetOwner().position;
			}
		} else {
			//cout << "Guardian::Defend() Error!" << endl;
		}
	}
	
	if (refreshTimer < 0.0f) {
		int row = static_cast<int>(destination.y) + Math::RandIntMinMax(-1, 1);
		int col = static_cast<int>(destination.x) + Math::RandIntMinMax(-1, 1);
		GetOwner().Move(_deltaTime, row, col);
		refreshTimer = 0.5f;
	} else {
		GetOwner().Move(_deltaTime);
	}
	
}

void Guardian_Defend::Update(double _deltaTime) {
	if (Think()) {
		Act(_deltaTime);
	}
	attackerInfos.clear();
	supportInfos.clear();
	helpRequests.clear();
	needsGuarding.clear();
}

void Guardian_Defend::ReceiveMessage(const Message* _messagePtr) {
	//Does anyone need help?
	const RequestHelp* helpMessage = dynamic_cast<const RequestHelp*>(_messagePtr);
	if (helpMessage != nullptr) {
		if (helpMessage->teamNumber == GetOwner().teamNumber && helpMessage->name != "Guardian") {
			helpRequests.push_back(*helpMessage);
			/*MessageReply replyMessage(GetOwner().GetID());
			replyMessage.destinationID = helpMessage->senderID;
			replyMessage.str = "Received Help Request.";
			EntityManager::GetInstance()->SendMessage(helpMessage->senderID, &replyMessage);*/
			MessageBoard::GetInstance()->UpdateLogReceiver(_messagePtr->messageLogID, GetOwner().name);
		}
	}

	//Get information about the characters
	const CharacterInfo* charInfo = dynamic_cast<const CharacterInfo*>(_messagePtr);
	if (charInfo != nullptr && charInfo->isAlive) {
		if (charInfo->teamNumber == GetOwner().teamNumber) {
			if (charInfo->name == "Attacker") {
				attackerInfos.push_back(*charInfo);
				MessageBoard::GetInstance()->UpdateLogReceiver(_messagePtr->messageLogID, GetOwner().name);
			} else if (charInfo->name == "Support") {
				supportInfos.push_back(*charInfo);
				MessageBoard::GetInstance()->UpdateLogReceiver(_messagePtr->messageLogID, GetOwner().name);
			}
		}
	}
}