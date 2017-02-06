#ifndef EXISTING_CHARACTER_REPLY_H
#define EXISTING_CHARACTER_REPLY_H

#include "Message.h"

struct ExistingCharacterReply : public Message {

public:
	int teamNumber;

	ExistingCharacterReply(int _senderID) : Message(_senderID) {
		teamNumber = -1;
		str = "Character check reply.";
	}
	virtual ~ExistingCharacterReply() {}

	ExistingCharacterReply* Copy() const {
		ExistingCharacterReply* copyPtr = new ExistingCharacterReply(*this);
		return copyPtr;
	}

};

#endif