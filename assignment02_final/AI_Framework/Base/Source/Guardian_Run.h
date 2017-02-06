#ifndef GUARDIAN_RUN_H
#define GUARDIAN_RUN_H

#include "Guardian_State.h"
#include "Vector2.h"
#include <vector>

using std::vector;

class Guardian;

class Guardian_Run : public Guardian_State {

private:
	vector<Vector2> destinations;
	bool canFollow;

	bool Think();
	void Act(double _deltaTime);

public:
	Guardian_Run(Guardian& _owner);
	virtual ~Guardian_Run();

	virtual void Update(double _deltaTime);
	virtual void ReceiveMessage(const Message* _messagePtr);

};

#endif