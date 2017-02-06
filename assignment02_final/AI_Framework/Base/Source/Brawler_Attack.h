#ifndef BRAWLER_ATTACK_H
#define BRAWLER_ATTACK_H

#include "Brawler_State.h"
#include "Vector2.h"
#include <vector>

using std::vector;

class CharacterInfo;

class Brawler_Attack : public Brawler_State
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
	Brawler_Attack(Brawler& owner);
	virtual ~Brawler_Attack();

	virtual void Update(double dt);
	virtual void ReceiveMessage(const Message* messagePtr);
};

#endif