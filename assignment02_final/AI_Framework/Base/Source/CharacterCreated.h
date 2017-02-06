#ifndef CHARACTER_CREATED_H
#define CHARACTER_CREATED_H

#include "Message.h"
#include <string>

using std::string;

struct CharacterCreated : public Message {

public:
	int teamNumber;
	string name;
	
	CharacterCreated(const int _senderID);
	CharacterCreated(const CharacterCreated& _rhs);
	virtual ~CharacterCreated();
	
	virtual CharacterCreated* Copy() const;

};

#endif