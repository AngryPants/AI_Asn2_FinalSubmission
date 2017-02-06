#ifndef GUARDIAN_FOLLOW_H
#define GUARDIAN_FOLLOW_H

#include "Guardian_State.h"
#include <vector>
#include "Vector2.h"

using std::vector;

class Guardian;
struct CharacterInfo;

class Guardian_Follow : public Guardian_State {

private:
	double refreshTimer;
	vector<CharacterInfo> attackerInfos;
	vector<CharacterInfo> supportInfos;

	bool Think();
	void Act(double _deltaTime);

public:
	Guardian_Follow(Guardian& _owner);
	virtual ~Guardian_Follow();

	virtual void Update(double _deltaTime);
	virtual void ReceiveMessage(const Message* _messagePtr);

};

#endif