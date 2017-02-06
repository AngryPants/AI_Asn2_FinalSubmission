#ifndef TB_CHARACTER_H
#define TB_CHARACTER_H

#include "EntityBase.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Mesh.h"
#include <string>
#include <map>
#include "State.h"
#include "AStar.h"
#include "TileSystem.h"

using namespace std;

//Hardcode All Da Shieeet
struct CharacterType {
private:
	CharacterType() {}
	~CharacterType() {}
public:
	static const int ATTACKER = 1;
	static const int GUARDIAN = 2;
	static const int SUPPORT = 3;
	static const int GUNNER = 4;
	static const int TURRET = 5;
	static const int BRAWLER = 6;
};

//Turn based Character
class TB_Character : public EntityBase {
	
protected:
	//Variable(s)
	//Health and stuff
	int health;
	int maxHealth;

	float movementSpeed;	
	
	//Which team are we in?
	int teamNumber;

	//Influence
	int influenceRadius;
	unordered_map<int, int> influenceTable; //First: Type, Second: Influence Value.
	unordered_set<int> detectedIDs; //A List of ids of stuff that we've detected.

	//States
	State* currentState;
	map<string, State*> states;
	
	//What to render on the screen.
	Mesh* mesh;

	//Move the character using this.
	void Move(double _deltaTime, list<std::pair<int, int> >& _path);
	bool SetInfluence();
	bool SetInfluence(int radius);
	bool RemoveInfluence();
	bool RemoveInfluence(int radius);

public:
	//Static Variable(s)
	static TileSystem tileSystem;
	static AStarSearch searcher;

	//Variable(s)
	const string name;
	Vector2 position;
	float rotation;
	
	//Constructor & Destructor
	TB_Character(const string& _name);
	virtual ~TB_Character();

	//Function(s)
	virtual int GetHealth() const;
	virtual int GetMaxHealth() const;
	virtual float GetHealthPercentage();
	virtual void TakeDamage(int _damage);
	virtual bool IsAlive();

	float GetSpeed() const;
	int GetTeamNumber() const;

	//Get the type of this thingy. Hardcoded. So yeah.
	virtual int GetType() const = 0;

	//Virtual Function(s)
	virtual void SetNextState(const string& _nextState) = 0;
	
	virtual void HandleMessage();
	virtual void Update(double _deltaTime) = 0;
	virtual void Render() = 0;
	virtual void RenderUI();
};

#endif