#ifndef BRAWLER_MOVE_H
#define BRAWLER_MOVE_H

#include "Brawler_State.h"
#include "EnemyDetected.h"
#include <vector>

using std::vector;

class Brawler_Move : public Brawler_State
{
private:
	bool enemyNearby;
	vector<EnemyDetected> detectedEnemies;

	bool Think();
	void Act(double dt);

public:
	Brawler_Move(Brawler& owner);
	virtual ~Brawler_Move();

	virtual void Update(double dt);
	virtual void ReceiveMessage(const Message* messagePtr);
};

#endif