#include "CharacterCreated.h"

CharacterCreated::CharacterCreated(const int _senderID) : Message(_senderID) {
	teamNumber = -1;
	str = "Character created.";
}

CharacterCreated::CharacterCreated(const CharacterCreated& _rhs) : Message(_rhs.senderID) {
	this->teamNumber = _rhs.teamNumber;
	this->name = _rhs.name;
	this->str = _rhs.str;
}

CharacterCreated::~CharacterCreated() {
}

CharacterCreated* CharacterCreated::Copy() const {
	return new CharacterCreated(*this);
}