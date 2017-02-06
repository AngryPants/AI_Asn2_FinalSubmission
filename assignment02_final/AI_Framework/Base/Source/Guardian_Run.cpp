#include "Guardian_Run.h"
#include "Guardian.h"
#include "MessageBoard.h"

//Messages
#include "RequestHelp.h"
#include "CharacterInfo.h"

Guardian_Run::Guardian_Run(Guardian& _owner) : Guardian_State("Run", _owner) {
	canFollow = false;
}

Guardian_Run::~Guardian_Run() {

}

bool Guardian_Run::Think() {
	if (GetOwner().IsMidMovement() == false) {
		if (canFollow) {
			//If we are high enough HP, and there's actually people to follow, go back to following.
			if (static_cast<float>(GetOwner().GetHealth()) / static_cast<float>(GetOwner().GetMaxHealth()) >= 0.7f) {
				GetOwner().SetNextState("Follow");
				return false;
			}
			//If we are high enough Shield, and there's actually people to follow, go back to following.
			if (static_cast<float>(GetOwner().GetShield()) / static_cast<float>(GetOwner().GetMaxShield()) >= 0.6f) {
				GetOwner().SetNextState("Follow");
				return false;
			}
		} else {
			GetOwner().SetNextState("Attack");
			return false;
		}
	}

	return true;
}

void Guardian_Run::Act(double _deltaTime) {
	//Ask for help!
	RequestHelp requestHelp(GetOwner().GetID());
	requestHelp.name = GetOwner().name;
	requestHelp.position = GetOwner().position;
	requestHelp.teamNumber = GetOwner().teamNumber;
	GetOwner().SendMessageToObservers(&requestHelp);
	
	//Look for the closest support and run like crazy to it.
	//We need to know if there's a support to run to.
	if (destinations.empty()) { //No there isn't.
		if (GetOwner().PathEmpty()) {
			//Move to a random place
			GetOwner().Move(_deltaTime, Math::RandIntMinMax(0, Character::tileSystem.GetNumRows() - 1), Math::RandIntMinMax(0, Character::tileSystem.GetNumColumns() - 1));
		} else {
			GetOwner().Move(_deltaTime);
		}
	} else { //Yes there is.
		//Find the closest one.
		Vector2 closestPosition = destinations[0];
		float closestDistance = (closestPosition - GetOwner().position).LengthSquared();
		for (unsigned int i = 1; i < destinations.size(); ++i) {
			float distance = (destinations[i] - GetOwner().position).LengthSquared();
			if(distance < closestDistance) {
				closestDistance = distance;
				closestPosition = destinations[i];
			}
		}		
		if (GetOwner().PathEmpty()) {
			GetOwner().Move(_deltaTime, (int)(closestPosition.y + 0.5f), (int)(closestPosition.x + 0.5f));
		} else {
			GetOwner().Move(_deltaTime);
		}
	}
}

void Guardian_Run::Update(double _deltaTime) {
	if (Think()) {
		Act(_deltaTime);
	}
	canFollow = false;
	destinations.clear();
}

void Guardian_Run::ReceiveMessage(const Message* _messagePtr) {
	const CharacterInfo* charInfo = dynamic_cast<const CharacterInfo*>(_messagePtr);
	if (charInfo != nullptr) {
		if (charInfo->teamNumber == GetOwner().teamNumber && charInfo->isAlive) {
			if (charInfo->name == "Support") {
				destinations.push_back(charInfo->position);
				canFollow = true;
				MessageBoard::GetInstance()->UpdateLogReceiver(_messagePtr->messageLogID, GetOwner().name);
			} else if (charInfo->name == "Attacker") {
				canFollow = true;
				MessageBoard::GetInstance()->UpdateLogReceiver(_messagePtr->messageLogID, GetOwner().name);
			}
		}
	}
}