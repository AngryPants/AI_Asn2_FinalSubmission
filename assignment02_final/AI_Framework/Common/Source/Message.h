#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

using std::string;

struct Message {

protected:
	//Constructor(s)
	Message(const int _senderID) : senderID(_senderID) {
		messageLogID = -1;
	}

public:	
	const int senderID;
	string str;
	int messageLogID;

	//Destructor
	virtual ~Message() {}
	virtual Message* Copy() const = 0;

};

#endif