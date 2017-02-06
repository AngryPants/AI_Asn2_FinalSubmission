#ifndef BRAWLER_RUN_H
#define BRAWLER_RUN_H

#include "Brawler_State.h"
#include "Vector2.h"

class Brawler_Run : public Brawler_State
{
private:
	bool canRun;
	Vector2 destination;

	bool Think();
	void Act(double dt);

public:
	Brawler_Run(Brawler& owner);
	virtual ~Brawler_Run();

	virtual void Update(double dt);
	virtual void ReceiveMessage(const Message* messagePtr);
};

#endif