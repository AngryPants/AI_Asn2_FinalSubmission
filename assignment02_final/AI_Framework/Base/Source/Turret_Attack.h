#ifndef TURRET_ATTACK_H
#define TURRET_ATTACK_H

#include "Turret_State.h"
#include "Vector2.h"
#include <vector>

using std::vector;

class CharacterInfo;

class Turret_Attack : public Turret_State
{
private:
	Vector2 targetPosition;
	vector<CharacterInfo> enemyInfos;

	bool Think();
	void Act(double dt);

	int currentTargetID;
	bool TargetCurrent();
	void TargetClosest();

public:
	Turret_Attack(Turret& owner);
	virtual ~Turret_Attack();

	virtual void Update(double dt);
	virtual void ReceiveMessage(const Message* messagePtr);

};

#endif