#ifndef BRAWLER_STATE_H
#define BRAWLER_STATE_H

#include "State.h"

class Brawler;

class Brawler_State : public State
{
private:
	Brawler* owner;

public:
	Brawler_State(const string& name, Brawler& owner) : State(name)
	{
		this->owner = &owner;
	}

	virtual ~Brawler_State()
	{
	}

	Brawler& GetOwner()
	{
		return *owner;
	}
};

#endif