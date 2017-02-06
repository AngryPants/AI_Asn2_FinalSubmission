#ifndef TURRET_STATE_H
#define TURRET_STATE_H

#include "State.h"

class Turret;

class Turret_State : public State
{
private:
	Turret* owner;

public:
	Turret_State(const string& name, Turret& owner) : State(name)
	{
		this->owner = &owner;
	}

	virtual ~Turret_State()
	{
	}

	Turret& GetOwner()
	{
		return *owner;
	}
};

#endif