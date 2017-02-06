#ifndef TURRET_H
#define TURRET_H

#include "Character.h"
#include <list>
#include <unordered_map>
#include "CharacterInfo.h"

using std::unordered_map;
using std::list;

class Turret : public Character
{
private:
	int maxBullet;
	int numBullet;
	int damage;

	float timeToShoot;
	float fireRate;

	float minRange;
	float maxRange;

	Mesh* healthMesh;
	Mesh* ammoMesh;

	string previousState, currentStateName;

	CharacterInfo info;

public:
	Turret();
	virtual ~Turret();

	int GetMaxBullet();
	int GetNumBullet();
	int GetDamage();

	float GetMinRange();
	float GetMaxRange();

	const string& GetPreviousState() const;
	const string& GetCurrentState() const;

	void Shoot();
	void Reload();

	virtual void SetNextState(const string& nextState);
	virtual void HandleMessage();
	virtual void Update(double dt);
	virtual void Render();
	virtual void RenderUI();
};

#endif