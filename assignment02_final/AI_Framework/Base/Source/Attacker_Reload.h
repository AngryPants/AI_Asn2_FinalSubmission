#ifndef ATTACK_RELOAD_H
#define ATTACK_RELOAD_H

#include "Attacker_State.h"

class Attacker_Reload : public Attacker_State {
	
private:
	double timeToReload;

public:
	//Constructor(s) & Destructor
	Attacker_Reload(Attacker&  _owner);
	virtual ~Attacker_Reload();

	//Interface Function(s)
	virtual void Update(double _deltaTime);
	virtual void ReceiveMessage(const Message* _messagePtr);

};

#endif