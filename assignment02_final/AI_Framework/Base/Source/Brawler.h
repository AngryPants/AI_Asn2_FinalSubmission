#include "Character.h"
#include <list>
#include <unordered_map>
#include "CharacterInfo.h"

using std::unordered_map;

using std::list;

class Brawler : public Character
{
private:
	int damage;
	
	float timeToAttack;
	float attackRate;

	float minRange;
	float maxRange;

	Mesh* healthMesh;
	
	string previousStateName, currentStateName;

	CharacterInfo info;

public:
	Brawler();
	virtual ~Brawler();

	int GetDamage();
	float GetMinRange();
	float GetMaxRange();

	const string& GetPreviousState() const;
	const string& GetCurrentState() const;

	void Attack(int _targetID);

	virtual void SetNextState(const string& nextState);
	virtual void HandleMessage();
	virtual void Update(double dt);
	virtual void Render();
};