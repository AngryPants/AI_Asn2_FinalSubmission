#ifndef GUARDIAN_ATTACK_H
#define GUARDIAN_ATTACK_H

#include "Guardian_State.h"
#include "CharacterInfo.h"
#include <vector>

using std::vector;

class Guardian_Attack : public Guardian_State {

private:
	//Variable(s)
	bool switchState;
	vector<CharacterInfo> enemyInfos;

	bool Think();
	void Act(double _deltaTime);

public:
	Guardian_Attack(Guardian& _owner);
	virtual ~Guardian_Attack();

	virtual void Update(double _deltaTime);
	virtual void ReceiveMessage(const Message* _messagePtr);

};

#endif