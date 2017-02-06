#ifndef ATTACKER_MOVE_H
#define ATTACKER_MOVE_H

#include "Attacker_State.h"
#include "EnemyDetected.h"
#include <vector>

using std::vector;

class Attacker_Move : public Attacker_State {

private:
	bool enemyNearby;
	vector<EnemyDetected> detectedEnemies;

	bool Think();
	void Act(double _deltaTime);

public:
	//Constructor(s)
	Attacker_Move(Attacker& _owner);
	virtual ~Attacker_Move();

	//Interface Function(s)
	virtual void Update(double _deltaTime);
	virtual void ReceiveMessage(const Message* _messagePtr);

};

#endif