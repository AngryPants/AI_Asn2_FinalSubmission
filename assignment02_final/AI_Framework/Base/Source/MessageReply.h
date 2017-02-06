#ifndef MESSAGE_REPLY_H
#define MESSAGE_REPLY_H

#include "Message.h"

struct MessageReply : public Message {

public:
	//Variable(s)
	int destinationID;

	//Constructor(s) & Destructor
	MessageReply(const int _sender) : Message(_sender) {
		str = "Message Reply";
		destinationID = -1;
	}
	MessageReply(const MessageReply& _rhs) : Message(_rhs.senderID) {
		this->destinationID = _rhs.destinationID;
		this->str = _rhs.str;
	}
	virtual ~MessageReply() {}

	virtual MessageReply* Copy() const {
		return new MessageReply(*this);
	}

};

#endif