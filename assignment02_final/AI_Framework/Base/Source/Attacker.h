#ifndef ATTACKER_H
#define ATTACKER_H

#include "Character.h"
#include <list>
#include <unordered_map>
#include "CharacterInfo.h"

using std::unordered_map;

using std::list;

class Attacker : public Character {

private:
	int	maxBullets; //Mag Size
	int numBullets;
	int damage;
	double timeToShoot;
	double fireRate;

	float minCombatRange; //To Enter Combat
	float maxCombatRange; //To Leave Combat
	float optimalRange; //Best Range

	Mesh* healthMesh;
	Mesh* ammoMesh;
	Mesh* detectionRadiusMesh;
	Mesh* combatRadiusMesh;

	string previousStateName, currentStateName;

	CharacterInfo info;

public:
	Attacker();
	virtual ~Attacker();

	//Getters
	int GetNumBullets();
	int GetMaxBullets();
	int GetDamage();
	float GetMinCombatRange();
	float GetMaxCombatRange();
	float GetOptimalRange();

	const string& GetPreviousStateName() const;
	const string& GetCurrentStateName() const;

	virtual void Buff();
	void Shoot(float _bulletSpread);
	void Reload();

	//Interface Function(s)
	virtual void SetNextState(const string& _nextState);
	virtual void HandleMessage();
	virtual void Update(double _deltaTime);
	virtual void Render();
	virtual void RenderUI();

};

#endif