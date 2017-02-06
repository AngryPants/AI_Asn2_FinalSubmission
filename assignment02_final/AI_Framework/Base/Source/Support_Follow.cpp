#include "Support_Follow.h"
#include "Support.h"

//Include Messages
#include "CharacterInfo.h"
#include "MessageBoard.h"


//Constructor(s) & Destructor
Support_Follow::Support_Follow(Support& owner) :State("Follow") {
	this->owner = &owner;
}

Support_Follow::~Support_Follow() {
}

//Private Function(s)
bool Support_Follow::Think(double deltaTime) {

	if (static_cast<float>(owner->GetHealthPercentage()<30.f)) {
		owner->SetNextState("Run");
		return false;
	}

	//Check if there's anyone to follow, or is everyone dead.


	else 
	{	
		if (attackerInfos.empty() && guardianInfos.empty()) {
			owner->SetNextState("Run");
			return false;
		}
		if (!attackerInfos.empty())
		{
			//Does any attacker need our help?
			for (vector<CharacterInfo>::iterator it = attackerInfos.begin();it!= attackerInfos.end();++it) {
				if ((*it).currentState == "Run") {
					owner->targetID = (*it).senderID;
					owner->SetNextState("Heal");
					return false;
				}
			}
		}
		
		if (!guardianInfos.empty())
		{
			for (vector<CharacterInfo>::iterator it = guardianInfos.begin();it!= guardianInfos.end();++it) 
			{
				if ((*it).currentState == "Run") {
					owner->targetID = (*it).senderID;
					owner->SetNextState("Heal");
					return false;
				}
			}
			//Does any guardians need our help?
			for(vector<CharacterInfo>::iterator it = guardianInfos.begin();it!= guardianInfos.end();++it)
			{
				if ((*it).currentState == "Defend") {
					owner->targetID = (*it).senderID;
					owner->SetNextState("Buff");
					return false;
				}
			}
		}
		
		if (!attackerInfos.empty())
		{
			//is any attackers in combat?
			for (vector<CharacterInfo>::iterator it = attackerInfos.begin();it!= attackerInfos.end();++it) 
			{
				if ((*it).currentState == "Attack") {
					owner->targetID = (*it).senderID;
					owner->SetNextState("Buff");
					return false;
				}
			}
		}
		
	}



	return true;
}

void Support_Follow::Act(double deltaTime) {
	direction.SetZero();
	Vector3 closestPosition;
	//Find the closest guy and follow him. //If there's an attacker we follow him.
	if (attackerInfos.empty() == false) {
		float closestDistance = (attackerInfos[0].position - owner->position).LengthSquared();
		closestPosition = attackerInfos[0].position.ChangeToVec3();
		for (unsigned int i = 1; i < attackerInfos.size(); ++i) {
			float distance = (attackerInfos[i].position - owner->position).LengthSquared();
			if (distance < closestDistance) {
				closestDistance = distance;
				closestPosition = attackerInfos[i].position.ChangeToVec3();
			}
		}
		/*if (closestDistance > 3 * 3)
		{
			direction = closestPosition - owner->position.ChangeToVec3();
		}
		*/
	}

	else if (guardianInfos.empty() == false) { //Otherwise we follow a guardian.
		float closestDistance = (guardianInfos[0].position - owner->position).LengthSquared();
		closestPosition = guardianInfos[0].position.ChangeToVec3();
		for (unsigned int i = 1; i < guardianInfos.size(); ++i) {
			float distance = (guardianInfos[i].position - owner->position).LengthSquared();
			if (distance < closestDistance) {
				closestDistance = distance;
				closestPosition = guardianInfos[i].position.ChangeToVec3();
			}
		}
		/*if (closestDistance > 3 * 3)
		{
			direction = closestPosition - owner->position.ChangeToVec3();
		}*/

	}
	owner->Move(deltaTime, closestPosition.y, closestPosition.x-1);
}

//Interface Function(s)
void Support_Follow::Update(double deltaTime) {
	if (Think(deltaTime)) {
		Act(deltaTime);
	}
	attackerInfos.clear();
	guardianInfos.clear();
}

void Support_Follow::ReceiveMessage(const Message* messagePtr) {
	//cout << "Support_Follow received a message." << endl;
	const CharacterInfo* charInfo = dynamic_cast<const CharacterInfo*>(messagePtr);
	if (charInfo != nullptr && charInfo->isAlive) {
		if (charInfo->teamNumber == owner->GetTeamNumber()) {
			if (charInfo->name == "Attacker") {
				MessageBoard::GetInstance()->UpdateLogReceiver(messagePtr->messageLogID, owner->name);
				this->attackerInfos.push_back(*charInfo->Copy());
			}
			else if (charInfo->name == "Guardian") {
				MessageBoard::GetInstance()->UpdateLogReceiver(messagePtr->messageLogID, owner->name);
				this->guardianInfos.push_back(*charInfo->Copy());
			}
		}
	}
}