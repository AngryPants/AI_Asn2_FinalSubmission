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

struct DetectedCharacterInfo {
	int enemyID;
	int type;
	Vector2 position;
	double lifeTime; //Delete this once lifetime is 0.
	DetectedCharacterInfo(int _enemyID, int _type, Vector2 _position) {
		enemyID = _enemyID;
		type = _type;
		position = _position;
		lifeTime = 5.0;
	}
	~DetectedCharacterInfo() {}
	void Update(double _deltaTime) {
		lifeTime -= _deltaTime;
	}
};

class Character : public EntityBase {
	
private:
	bool SetInfluence(int radius);
	int destinationRow, destinationColumn;
	
	vector<DetectedCharacterInfo> detectedCharacters;
	unordered_set<int> detectedIDs; //A List of ids of stuff that we've detected.

protected:
	//Variable(s)
	//Health and stuff
	int health;
	int maxHealth;
	const int type;

	//Movement
	float movementSpeed;	
	
	//Radius to take damage
	float radius;

	//Influence
	int influenceRadius;
	unordered_map<int, int> influenceTable; //First: Type, Second: Influence Value.	

	//States
	State* currentState;
	map<string, State*> states;
	
	//What to render on the screen.
	Mesh* mesh;
	Mesh* pathMesh;

	//The path
	list<std::pair<int, int> > path;

	int occupiedRow[2];
	int occupiedCol[2];
		
	void Unoccupy();
	
	bool midMovement; //Are we nicely in the middle of a square?

	//Move the character using this.	
	bool SetInfluence();	
	bool RemoveInfluence();
	bool RemoveInfluence(int radius);
		
	void UpdateDetectedCharacters(double _deltaTime);

public:
	//Static Variable(s)
	static TileSystem tileSystem;
	static AStarSearch searcher;

	//Variable(s)
	const string name;
	Vector2 position;
	float rotation;

	//Which team are we in?
	int teamNumber;	

	//Constructor & Destructor
	Character(const string& _name, int _type);
	virtual ~Character();

	//Function(s)
	virtual int GetHealth() const;
	virtual int GetMaxHealth() const;
	virtual float GetHealthPercentage();
	virtual void TakeDamage(int _damage);
	virtual bool IsAlive();
	virtual void Buff();
	virtual void Heal(const int _health);

	void Occupy(int row, int col);

	//The radius at which a bullet can hurt us.
	virtual float GetRadius() const;

	bool PathEmpty() const;
	bool IsMidMovement() const;

	void RemoveDetectedCharacter(int enemyID);
	void DetectCharacter(int _id, int _type, Vector2 _position);

	//If the last 2 param is < 0, then it will move along the last set destination.
	void Move(double _deltaTime, int _row = -1, int _column = -1);
	void Move(double _deltaTime, int _row, int _column, float moveSpeed);
	bool CancelMove(); //Cannot cancel when not properly in the middle of a tile.
	
	float GetMovementSpeed() const;
	int GetTeamNumber() const;

	//Get the type of this thingy. Hardcoded. So... yeah.
	int GetType() const;

	void PrintPath();
	void RenderPath();
	
	//Virtual Function(s)
	virtual void SetNextState(const string& _nextState) = 0;
	virtual void HandleMessage();
	virtual void Update(double _deltaTime) = 0;
	virtual void Render() = 0;
	virtual void RenderUI();

	virtual void SendMessageToObservers(Message* messagePtr);
};

#endif