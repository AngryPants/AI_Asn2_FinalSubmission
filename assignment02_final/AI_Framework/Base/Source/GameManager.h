#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "SingletonTemplate.h"
#include "PassKey.h"
#include <list>

using std::list;

class Character;
class Bullet;

class GameManager : public Singleton<GameManager> {

	friend Singleton<GameManager>;

private:
	list<Character*> characterList;
	list<Bullet*> bulletList;

	//Constructor(s) & Destructor
	GameManager();
	virtual ~GameManager();

public:
	//Function(s)
	void AddCharacter(Character* _character, PassKey<Character> _key);
	bool RemoveCharacter(Character* _character, PassKey<Character> _key);
	void AddBullet(Bullet* _bullet, PassKey<Bullet> _key);
	bool RemoveBullet(Bullet* _bullet, PassKey<Bullet> _key);
	Bullet* GetBullet();

	void Update(double _deltaTime);
};

#endif