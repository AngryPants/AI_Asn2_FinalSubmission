#ifndef CHARACTER_INFO_H
#define CHARACTER_INFO_H

#include "Message.h"
#include "Vector2.h"
#include <string>

using std::string;

struct CharacterInfo : public Message {

public:
	string name;
	string currentState;
	Vector2 position;
	int health;
	int maxHealth;
	int teamNumber;
	bool isAlive;	

	CharacterInfo(const int senderID) : Message(senderID) {
		str = "Character information.";
		position.Set(-1, -1);
		health = -1;
		maxHealth = -1;
		teamNumber = -1;
		isAlive = false;
	}
	virtual ~CharacterInfo() {}

	virtual CharacterInfo* Copy() const {
		CharacterInfo* copyPtr = new CharacterInfo(*this);
		return copyPtr;
	}

};

#endif