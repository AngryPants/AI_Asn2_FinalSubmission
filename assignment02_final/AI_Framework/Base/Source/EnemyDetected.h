#ifndef ENEMY_DETECTED_H
#define ENEMY_DETECTED_H

#include "Message.h"
#include "Vector2.h"
#include <string>

using std::string;

struct EnemyDetected : public Message {

public:
	int enemyID; //What's the enemy's id?
	int type; //What type is the enemy?
	Vector2 position;

	EnemyDetected(const int senderID) : Message(senderID) {
		enemyID = -1;
		type = -1;
		str = "Enemy detected.";
	}
	virtual ~EnemyDetected() {}

	virtual EnemyDetected* Copy() const {
		EnemyDetected* copyPtr = new EnemyDetected(*this);
		return copyPtr;
	}

};

#endif