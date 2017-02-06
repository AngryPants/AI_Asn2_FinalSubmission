#ifndef REQUEST_HELP_H
#define REQUEST_HELP_H

#include "Message.h"
#include "Vector2.h"
#include <string>

using std::string;

struct RequestHelp : public Message {

public:
	int teamNumber;
	Vector2 position;
	string name;

	RequestHelp(const int _senderID) : Message(_senderID) {	
		teamNumber = -1;
		str = "Request Help.";
	}
	RequestHelp(const RequestHelp& _rhs) : Message(_rhs.senderID) {
		this->teamNumber = _rhs.teamNumber;
		this->position = _rhs.position;
		this->name = _rhs.name;
	}
	virtual ~RequestHelp() {}

	virtual RequestHelp* Copy() const {
		return new RequestHelp(*this);
	}

};

#endif