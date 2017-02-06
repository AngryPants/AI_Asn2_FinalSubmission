#include "Attacker_Run.h"
#include "Attacker.h"
#include "CharacterInfo.h"
#include "RequestHelp.h"
#include "MessageBoard.h"

//Constructor(s) & Destructor
Attacker_Run::Attacker_Run(Attacker& _owner) : Attacker_State("Run", _owner) {
	canRun = false;
	refreshTimer = 0.0;
}

Attacker_Run::~Attacker_Run() {
}

bool Attacker_Run::Think() {
	if (GetOwner().GetHealthPercentage() > 60.f) {
		GetOwner().SetNextState("Move");
		return false;
	} else if (!canRun) {
		GetOwner().SetNextState("Move");
		return false;
	}
	return true;
}

void Attacker_Run::Act(double _deltaTime) {
	//Ask for help!
	RequestHelp requestHelp(GetOwner().GetID());
	requestHelp.name = GetOwner().name;
	requestHelp.position = GetOwner().position;
	requestHelp.teamNumber = GetOwner().teamNumber;
	GetOwner().SendMessageToObservers(&requestHelp);
	cout << "Running" << endl;
	if (GetOwner().IsMidMovement() == false && refreshTimer < 0.0f) {
		GetOwner().Move(_deltaTime, static_cast<int>(destination.y + 0.5f), static_cast<int>(destination.x + 0.5f));
		if (GetOwner().PathEmpty() == false) {
			refreshTimer = 1.0;
		}			
	} else {
		cout << "Running 2" << endl;
		GetOwner().Move(_deltaTime);
	}	
}

//Interface Function(s)
void Attacker_Run::Update(double _deltaTime) {
	if (Think()) {
		Act(_deltaTime);
	}
	refreshTimer -= _deltaTime;
}

void Attacker_Run::ReceiveMessage(const Message* _messagePtr) {
	const CharacterInfo* charInfo = dynamic_cast<const CharacterInfo*>(_messagePtr);
	if (charInfo!= nullptr) {
		if (charInfo->name == "Support" && charInfo->isAlive) {
			if (!canRun) {
				canRun = true;
				destination = charInfo->position;
				MessageBoard::GetInstance()->UpdateLogReceiver(_messagePtr->messageLogID, GetOwner().name);
			} else {
				if ((destination - GetOwner().position).LengthSquared() > (charInfo->position - GetOwner().position).LengthSquared()) {
					destination = charInfo->position;
					MessageBoard::GetInstance()->UpdateLogReceiver(_messagePtr->messageLogID, GetOwner().name);
				}
			}
		}
	}
}