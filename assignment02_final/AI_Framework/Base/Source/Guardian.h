#ifndef GUARDIAN_H
#define GUARDIAN_H

#include "Character.h"
#include "Vector2.h"
#include "CharacterInfo.h"

class Guardian : public Character {

private:
	CharacterInfo info;

	int shield;
	int maxShield;	
	float shieldRadius;
	double shieldRechargeTimer;
	double buffTimer;
	float priorityRange;

	//Mesh
	Mesh* shieldMesh;
	Mesh* bulletMesh;
	Mesh* healthMesh;
	Mesh* redCircleMesh;
	Mesh* blueCircleMesh;

	double timeToShoot;
	double fireRate;
	int damage;

	float combatRange;

public:
	//Constructor(s) & Destructor
	Guardian();
	virtual ~Guardian();

	void SetShield(const int _shield);
	int GetShield() const;
	int GetMaxShield() const;

	int GetDamage() const;
	void Shoot();

	virtual void TakeDamage(const int _damage);	
	virtual void Buff();
	virtual void Heal(const int _health);
	
	virtual float GetRadius() const;

	float GetCombatRange() const;

	float GetPriorityRange() const;

	//Virtual Function(s)
	virtual void SetNextState(const string& _nextState);
	virtual void HandleMessage();
	virtual void Update(double _deltaTime);
	virtual void Render();
	virtual void RenderUI();

};

#endif