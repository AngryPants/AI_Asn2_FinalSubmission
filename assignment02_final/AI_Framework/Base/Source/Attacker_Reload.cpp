#include "Attacker_Reload.h"
#include "Attacker.h"

//Constructor(s) & Destructor
Attacker_Reload::Attacker_Reload(Attacker& _owner) : Attacker_State("Reload", _owner) {
	timeToReload = 0.0f;
}

Attacker_Reload::~Attacker_Reload() {
}

//Interface Function(s)
void Attacker_Reload::Update(double _deltaTime) {
	if(GetOwner().GetNumBullets() >= GetOwner().GetMaxBullets()) {
		timeToReload = 0.0;
		GetOwner().SetNextState(GetOwner().GetPreviousStateName());
		return;
	}

	timeToReload -= _deltaTime;
	if (timeToReload <= 0.0f) {
		GetOwner().Reload();
		timeToReload = 0.1f;
	}
}

void Attacker_Reload::ReceiveMessage(const Message* _messagePtr) {
}