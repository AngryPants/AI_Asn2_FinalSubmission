#ifndef GUARDIAN_STATE_H
#define GUARDIAN_STATE_H

#include "State.h"

class Guardian;

class Guardian_State : public State {

private:
	Guardian* owner;

public:
	Guardian_State(const string& _name, Guardian& _owner) : State(_name) {
		owner = &_owner;
	}
	virtual ~Guardian_State() {}

	Guardian& GetOwner() {
		return *owner;
	}

};

#endif