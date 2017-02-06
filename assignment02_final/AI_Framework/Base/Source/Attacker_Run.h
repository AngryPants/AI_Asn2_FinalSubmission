#ifndef ATTACKER_RUN_H
#define ATTACKER_RUN_H

#include "Attacker_State.h"
#include "Vector2.h"

class Attacker_Run : public Attacker_State {

private:
	bool canRun;
	Vector2 destination;
	double refreshTimer;

	bool Think();
	void Act(double _deltaTime);

public:
	//Constructor(s) & Destructor
	Attacker_Run(Attacker& _owner);
	virtual ~Attacker_Run();

	//Interface Function(s)
	virtual void Update(double _deltaTime);
	virtual void ReceiveMessage(const Message* _messagePtr);

};

#endif