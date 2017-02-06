#ifndef ATTACKER_ATTACK_H
#define ATTACKER_ATTACK_H

#include "Attacker_State.h"
#include "Vector2.h"
#include <vector>

using std::vector;

class CharacterInfo;

class Attacker_Attack : public Attacker_State {

public:
	enum ATTACK_MODE {
		ATTACK_FURTHEST,
		ATTACK_CLOSEST,
		ATTACK_HIGHEST_HP,
		ATTACK_LOWEST_HP,		

		NUM_ATTACK_MODE,
	};

private:
	//Variable(s)
	bool hasSupport;
	ATTACK_MODE currentMode;
	int currentTargetID;
	Vector2 targetPosition;
	vector<CharacterInfo> enemyInfos;

	//Private Function(s)
	bool Think();
	void Act(double _deltaTime);

	bool TargetCurrent();
	void TargetClosest();
	void TargetFurthest();
	void TargetLowestHP();
	void TargetHighestHP();

public:
	//Constructor(s) & Destructor
	Attacker_Attack(Attacker& _owner);
	virtual ~Attacker_Attack();

	//Interface Function(s)
	virtual void Update(double _deltaTime);
	virtual void ReceiveMessage(const Message* _messagePtr);

	ATTACK_MODE GetAttackMode() const;

};

#endif