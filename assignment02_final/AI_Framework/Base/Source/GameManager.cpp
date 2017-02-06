#include "GameManager.h"
#include "Character.h"
#include "Bullet.h"

//Constructor(s) & Destructor
GameManager::GameManager() {
}

GameManager::~GameManager() {
}

//Function(s)
void GameManager::AddCharacter(Character* _character, PassKey<Character> _key) {
	characterList.push_back(_character);
}

bool GameManager::RemoveCharacter(Character* _character, PassKey<Character> _key) {
	for (list<Character*>::iterator iter = characterList.begin(); iter != characterList.end(); ++iter) {
		if (_character == *iter) {
			characterList.erase(iter);
			return true;
		}
	}

	return false;
}

void GameManager::AddBullet(Bullet* _bullet, PassKey<Bullet> _key) {
	bulletList.push_back(_bullet);
}

bool GameManager::RemoveBullet(Bullet* _bullet, PassKey<Bullet> _key) {
	for (list<Bullet*>::iterator iter = bulletList.begin(); iter != bulletList.end(); ++iter) {
		if (_bullet == *iter) {
			bulletList.erase(iter);
			return true;
		}
	}

	return false;
}

Bullet* GameManager::GetBullet() {
	for (list<Bullet*>::iterator bulletIter = bulletList.begin(); bulletIter != bulletList.end(); ++bulletIter) {
		if((*bulletIter)->IsActive() == false) {
			return (*bulletIter);
		}
	}
	Bullet* ptr;
	for(int i = 0; i < 10; ++i) {
		ptr = new Bullet();
	}
	return ptr;
}

void GameManager::Update(double _deltaTime) {
	for (list<Bullet*>::iterator bulletIter = bulletList.begin(); bulletIter != bulletList.end(); ++bulletIter) {
		Bullet* bulletPtr = *bulletIter;
		if (bulletPtr->IsActive() == false) {
			continue;
		}
		for (list<Character*>::iterator characterIter = characterList.begin(); characterIter != characterList.end(); ++characterIter) {
			Character* characterPtr = *characterIter;
			if (characterPtr->teamNumber == bulletPtr->teamNumber) {
				continue;
			}
			float distanceSquared = (bulletPtr->position - characterPtr->position).LengthSquared();
			if (distanceSquared < (bulletPtr->radius + characterPtr->GetRadius()) * (bulletPtr->radius + characterPtr->GetRadius())) {
				characterPtr->TakeDamage(bulletPtr->damage);
				if (characterPtr->IsAlive() == false) {
					characterPtr->SetIsDone();
				}
				bulletPtr->SetInactive();
				break;
			}
		}
	}

	for (list<Character*>::iterator characterIter = characterList.begin(); characterIter != characterList.end(); ++characterIter) {
		Character* characterPtr = *characterIter;
		if (characterPtr->IsAlive() == false) {
			characterPtr->SetIsDone();
		}
	}
}