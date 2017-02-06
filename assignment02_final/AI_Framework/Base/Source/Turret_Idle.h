#ifndef TURRET_IDLE_H
#define TURRET_IDLE_H

#include "Turret_State.h"
#include "EnemyDetected.h"
#include "Vector2.h"
#include <vector>

using std::vector;

class CharacterInfo;

class Turret_Idle : public Turret_State
{
public:
	Turret_Idle(Turret& owner);
	virtual ~Turret_Idle();

	virtual void Update(double dt);
	virtual void ReceiveMessage(const Message *message);

private:
	bool enemyNearby;
	vector<EnemyDetected> detectedEnemies;
	double directionResetTimer;

	bool Think();
	void Act(double dt);

	void ResetDirection();

	Vector2 targetPosition;
	Vector2 direction;

};

#endif