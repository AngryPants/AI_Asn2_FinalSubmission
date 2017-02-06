#ifndef TB_ATTACKER_H
#define TB_ATTACKER_H

#include "TB_Character.h"
#include <list>
#include <unordered_map>

using std::unordered_map;

using std::list;

class TB_Attacker : public TB_Character {

private:
	int	maxBullets; //Mag Size
	int numBullets;
	int damage;
	float minRange; //To Enter Combat
	float maxRange; //To Leave Combat

	Mesh* healthMesh;
	Mesh* ammoMesh;
	Mesh* detectionRadiusMesh;
	Mesh* combatRadiusMesh;
	list<std::pair<int, int> > path;

public:
	TB_Attacker();
	virtual ~TB_Attacker();

	//Interface Function(s)
	virtual int GetType() const;
	virtual void SetNextState(const string& _nextState);
	virtual void HandleMessage();
	virtual void Update(double _deltaTime);
	virtual void Render();
	virtual void RenderUI();

};

#endif