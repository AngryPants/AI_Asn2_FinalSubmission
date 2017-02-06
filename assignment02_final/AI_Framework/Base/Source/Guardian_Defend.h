#ifndef GUARDIAN_DEFEND_H
#define GUARDIAN_DEFEND_H

#include "Guardian_State.h"
#include <vector>

#include "CharacterInfo.h"
#include "RequestHelp.h"

using std::vector;

class Guardian_Defend : public Guardian_State {

private:
	//Variable(s)
	vector<CharacterInfo> attackerInfos;
	vector<CharacterInfo> supportInfos;
	vector<RequestHelp> helpRequests;
	vector<CharacterInfo> needsGuarding;
	bool Think();
	void Act(double _deltaTime);

	float refreshTimer;

public:
	Guardian_Defend(Guardian& _owner);
	virtual ~Guardian_Defend();

	virtual void Update(double _deltaTime);
	virtual void ReceiveMessage(const Message* _messagePtr);

};

#endif