#include "Character.h"
#include "GameManager.h"
#include "MeshBuilder.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"
#include "MessageBoard.h"

TileSystem Character::tileSystem;
AStarSearch Character::searcher;

//Constructor(s) & Destructor
Character::Character(const string& _name, int _type) : name(_name), type(_type) {
	GameManager::GetInstance()->AddCharacter(this, {});

	mesh = nullptr;
	if (!MeshBuilder::GetInstance()->HasMesh("Path Mesh")) {
		MeshBuilder::GetInstance()->GenerateQuad("Path Mesh", Color(0, 1, 1), 1.0f);
	}
	pathMesh = MeshBuilder::GetInstance()->GetMesh("Path Mesh");

	//Health
	maxHealth = 50;
	health = maxHealth;
	movementSpeed = 5.0f;
	teamNumber = 0;	
	influenceRadius = 0;
	currentState = nullptr;
	rotation = 0.0f;
	radius = 0.5f;
	midMovement = false;	
	destinationColumn = -1;
	destinationRow = -1;

	occupiedRow[0] = -1;	
	occupiedCol[0] = -1;
	occupiedRow[1] = -1;
	occupiedCol[1] = -1;
}

Character::~Character() {
	//When we die make sure to remove our influence and shit.
	RemoveInfluence();
	Unoccupy();
	GameManager::GetInstance()->RemoveCharacter(this, {});	
}

//Function(s)
void Character::Move(double _deltaTime, int _row, int _column) {
	//Unoccupy our previous thingies.
	Unoccupy();

	//Always chope the tile we are in, and the tile we wanna move to.
	int currentRow = static_cast<int>(position.y + 0.5f);
	int currentColumn = static_cast<int>(position.x + 0.5f);
	
	//Remember the place we occupied.
	occupiedRow[0] = currentRow;
	occupiedCol[0] = currentColumn;

	//Chope!
	tileSystem.GetTile(currentRow, currentColumn).occupant = GetID();

	bool destinationChange = false;
	if (_row >= 0 && _column >= 0) {
		if (destinationRow != _row || destinationColumn != _column) {
			destinationRow = _row;
			destinationColumn = _column;
			destinationChange = true;
		}
	}

	//If we don't have a path, or our destination changed find one.
	if (path.empty() || destinationChange) {
		int startRow = static_cast<int>(position.y + 0.5f);
		int startColumn = static_cast<int>(position.x + 0.5f);
		path = searcher.Search(&tileSystem, &influenceTable, &detectedIDs, GetID(), startRow, startColumn, destinationRow, destinationColumn);
	}
	
	//If our path is still empty, there is no path.
	if (path.empty()) {
		return;
	}

	midMovement = true;
	//This is where we're trying to go.
	Vector2 destination(static_cast<float>(path.begin()->second), static_cast<float>(path.begin()->first));
	//Make sure that where we wanna go is not choped by someone else.
	if (searcher.IsTraversable(&tileSystem, GetID(), path.begin()->first, path.begin()->second) == false) {
		//If it is, then the path is invalid. If it is invalid, throw it away, forget yesterday, we'll make the great escape!
		path.clear();
		return;
	}
	
	//Otherwise, we quickly put our tissue paper on it.
	//Remember where we chope.
	occupiedRow[1] = path.begin()->first;
	occupiedCol[1] = path.begin()->second;
	tileSystem.GetTile(path.begin()->first, path.begin()->second).occupant = GetID();

	//Remove our current influence
	RemoveInfluence();

	//Move towards the destination.
	if (position.y < destination.y) {
		position.y = Math::Min(position.y + static_cast<float>(_deltaTime) * movementSpeed, destination.y);
	} else if (position.y > destination.y) {
		position.y = Math::Max(position.y - static_cast<float>(_deltaTime) * movementSpeed, destination.y);
	}
	if (position.x < destination.x) {
		position.x = Math::Min(position.x + static_cast<float>(_deltaTime) * movementSpeed, destination.x);
	} else if (position.x > destination.x) {
		position.x = Math::Max(position.x - static_cast<float>(_deltaTime) * movementSpeed, destination.x);
	}

	//apply influence to new position
	SetInfluence();

	//In GPS Voice: "You have arrived at your destination.".
	if ((destination - position).LengthSquared() < Math::EPSILON) {
		//Remove the waypoint from the _path.
		//position = destination;
		path.erase(path.begin());
		midMovement = false;
	}
}

void Character::Move(double _deltaTime, int _row, int _column ,float movementSpeed) {
	//Unoccupy our previous thingies.
	Unoccupy();

	//Always chope the tile we are in, and the tile we wanna move to.
	int currentRow = static_cast<int>(position.y + 0.5f);
	int currentColumn = static_cast<int>(position.x + 0.5f);

	//Remember the place we occupied.
	occupiedRow[0] = currentRow;
	occupiedCol[0] = currentColumn;

	//Chope!
	tileSystem.GetTile(currentRow, currentColumn).occupant = GetID();

	bool destinationChange = false;
	if (_row >= 0 && _column >= 0) {
		if (destinationRow != _row || destinationColumn != _column) {
			destinationRow = _row;
			destinationColumn = _column;
			destinationChange = true;
		}
	}

	//If we don't have a path, or our destination changed find one.
	if (path.empty() || destinationChange) {
		int startRow = static_cast<int>(position.y + 0.5f);
		int startColumn = static_cast<int>(position.x + 0.5f);
		path = searcher.Search(&tileSystem, &influenceTable, &detectedIDs, GetID(), startRow, startColumn, destinationRow, destinationColumn);
	}

	//If our path is still empty, there is no path.
	if (path.empty()) {
		return;
	}

	midMovement = true;
	//This is where we're trying to go.
	Vector2 destination(static_cast<float>(path.begin()->second), static_cast<float>(path.begin()->first));
	//Make sure that where we wanna go is not choped by someone else.
	if (searcher.IsTraversable(&tileSystem, GetID(), path.begin()->first, path.begin()->second) == false) {
		//If it is, then the path is invalid. If it is invalid, throw it away, forget yesterday, we'll make the great escape!
		path.clear();
		return;
	}

	//Otherwise, we quickly put our tissue paper on it.
	//Remember where we chope.
	occupiedRow[1] = path.begin()->first;
	occupiedCol[1] = path.begin()->second;
	tileSystem.GetTile(path.begin()->first, path.begin()->second).occupant = GetID();

	//Remove our current influence
	RemoveInfluence();

	//Move towards the destination.
	if (position.y < destination.y) {
		position.y = Math::Min(position.y + static_cast<float>(_deltaTime)* movementSpeed, destination.y);
	}
	else if (position.y > destination.y) {
		position.y = Math::Max(position.y - static_cast<float>(_deltaTime)* movementSpeed, destination.y);
	}
	if (position.x < destination.x) {
		position.x = Math::Min(position.x + static_cast<float>(_deltaTime)* movementSpeed, destination.x);
	}
	else if (position.x > destination.x) {
		position.x = Math::Max(position.x - static_cast<float>(_deltaTime)* movementSpeed, destination.x);
	}

	//apply influence to new position
	SetInfluence();

	//In GPS Voice: "You have arrived at your destination.".
	if ((destination - position).LengthSquared() < Math::EPSILON) {
		//Remove the waypoint from the _path.
		//position = destination;
		path.erase(path.begin());
		midMovement = false;
	}
}


void Character::Unoccupy() {
	if (occupiedRow[0] >= 0 && occupiedRow[0] <= tileSystem.GetNumRows() - 1) {
		if (occupiedCol[0] >= 0 && occupiedCol[0] <= tileSystem.GetNumColumns() - 1) {
			tileSystem.GetTile(occupiedRow[0], occupiedCol[0]).occupant = -1;
		}			
	}	
	if (occupiedRow[1] >= 0 && occupiedRow[1] <= tileSystem.GetNumRows() - 1) {
		if (occupiedCol[1] >= 0 && occupiedCol[1] <= tileSystem.GetNumColumns() - 1) {
			tileSystem.GetTile(occupiedRow[1], occupiedCol[1]).occupant = -1;
		}
	}
}

void Character::Occupy(int row, int col) {
	if (row>= 0 && row <= tileSystem.GetNumRows() - 1) {
		if (col >= 0 && col <= tileSystem.GetNumColumns() - 1) {
			tileSystem.GetTile(row, col).occupant = GetID();
			occupiedRow[0] = row;
			occupiedRow[1] = row;
			occupiedCol[0] = col;
			occupiedCol[1] = col;
		}
	}	
}

bool Character::CancelMove() {
	if (midMovement == false) {
		Unoccupy();
		path.clear();
		return true;
	}
	return false;
}

bool Character::SetInfluence() {
	return SetInfluence(influenceRadius);
}

bool Character::SetInfluence(int radius) {
	if (tileSystem.GetTileMap() == nullptr) {
		return false;
	}

	//Do stuff here. Remember to make sure you're not outside the tiles.
	int currentRow = static_cast<int>(position.y + 0.5f);
	int currentColumn = static_cast<int>(position.x + 0.5f);

	int minRow = Math::Max(currentRow - radius, 0);
	int maxRow = Math::Min(currentRow + radius, tileSystem.GetNumRows());

	int minCol = Math::Max(currentColumn - radius, 0);
	int maxCol = Math::Min(currentColumn + radius, tileSystem.GetNumColumns());

	for (int row = minRow; row < maxRow; ++row) {
		for (int col = minCol; col < maxCol; ++col) {
			tileSystem.GetTile(col, row).ids.push_back(pair<int, int>(this->GetID(), this->GetType()));
		}
	}

	return true;
}

//Same thing as above but opposite.
bool Character::RemoveInfluence() {
	return RemoveInfluence(influenceRadius);
}

bool Character::RemoveInfluence(int radius) {
	if (tileSystem.GetTileMap() == nullptr) {
		return false;
	}

	int currentRow = static_cast<int>(position.y + 0.5f);
	int currentColumn = static_cast<int>(position.x + 0.5f);

	int minRow = Math::Max(currentRow - radius, 0);
	int maxRow = Math::Min(currentRow + radius, tileSystem.GetNumRows());

	int minCol = Math::Max(currentColumn - radius, 0);
	int maxCol = Math::Min(currentColumn + radius, tileSystem.GetNumColumns());

	for (int row = minRow; row < maxRow; ++row) {
		for (int col = minCol; col < maxCol; ++col) {
			tileSystem.GetTile(col, row).ids.remove(pair<int, int>(this->GetID(), this->GetType()));
		}
	}

	return true;
}

void Character::RemoveDetectedCharacter(int _enemyID) {
	for (vector<DetectedCharacterInfo>::iterator iter = detectedCharacters.begin(); iter != detectedCharacters.end(); ++iter) {
		if (iter->enemyID == _enemyID) {
			detectedCharacters.erase(iter);
			return;
		}
	}
}

void Character::DetectCharacter(int _enemyID, int _type, Vector2 _position) {
	for (vector<DetectedCharacterInfo>::iterator iter = detectedCharacters.begin(); iter != detectedCharacters.end(); ++iter) {
		if (iter->enemyID == _enemyID) {
			iter->type = _type;
			iter->position = _position;
			iter->lifeTime = 5.0f;
			return;
		}
	}
	detectedCharacters.push_back(DetectedCharacterInfo(_enemyID, _type, _position));
}

void Character::UpdateDetectedCharacters(double _deltaTime) {
	detectedIDs.clear();
	for (vector<DetectedCharacterInfo>::iterator iter = detectedCharacters.begin(); iter != detectedCharacters.end(); ) {
		iter->Update(_deltaTime);
		if (iter->lifeTime < 0.0) {
			iter = detectedCharacters.erase(iter);
		} else {
			detectedIDs.insert(iter->enemyID);
			++iter;
		}
	}
}

//Pretty standard stuff. These health functions are virtual so child classes can modify them, such as have shields or stuff.
int Character::GetHealth() const {
	return health;
}

int Character::GetMaxHealth() const {
	return maxHealth;
}

float Character::GetHealthPercentage() {
	return ((float)this->health / (float)this->maxHealth)*100.f;
}

void Character::TakeDamage(int _damage) {
	health -= _damage;
}

bool Character::IsAlive() {
	return health > 0;
}

void Character::Buff() {
}

void Character::Heal(const int _health) {
	health = Math::Min(maxHealth, health + _health);
}

float Character::GetRadius() const {
	return radius;
}

//How many squares can the character move i
float Character::GetMovementSpeed() const {
	return movementSpeed;
}

//Team Number, self-explainatory.
int Character::GetTeamNumber() const {
	return teamNumber;
}

int Character::GetType() const {
	return type;
}

//Virtual Function(s)
void Character::HandleMessage() {
	while (!messageQueue.empty()) {
		if (currentState != nullptr) {
			currentState->ReceiveMessage(messageQueue.front());
		}		
		delete messageQueue.front();
		messageQueue.pop();
	}
}

void Character::RenderUI() {
}

bool Character::PathEmpty() const {
	return path.empty();
}
bool Character::IsMidMovement() const {
	return midMovement;
}

void Character::PrintPath() {
	cout << "Path:" << endl;
	for (list<std::pair<int, int> >::iterator pathIter = path.begin(); pathIter != path.end(); ++pathIter) {
		cout << "Row: " << pathIter->first << " Column: " << pathIter->second << endl;
	}
}

void Character::RenderPath() {
	MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
	for (list<std::pair<int, int> >::iterator pathIter = path.begin(); pathIter != path.end(); ++pathIter) {
		modelStack.PushMatrix();
			modelStack.Translate(pathIter->second, pathIter->first, -9);
			RenderHelper::RenderMesh(pathMesh);
		modelStack.PopMatrix();
	}	
}

void Character::SendMessageToObservers(Message* messagePtr)
{
	EntityBase::SendMessageToObservers(messagePtr);
	messagePtr->messageLogID =  MessageBoard::GetInstance()->AddMessageLog(messagePtr->str, this->name);
	//messagePtr->messageLogID = MessageBoard::GetInstance()->GetFreeID();
}