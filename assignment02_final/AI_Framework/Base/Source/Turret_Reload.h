#ifndef TURRET_RELOAD_H
#define TURRET_RELOAD_H

#include "Turret_State.h"

class Turret_Reload : public Turret_State
{
private:
	float timeToReload;
public:
	Turret_Reload(Turret& owner);
	virtual ~Turret_Reload();

	virtual void Update(double dt);
	virtual void ReceiveMessage(const Message* _messagePtr);
};

#endif