#include "TB_Character.h"
#include "GameManager.h"

TileSystem TB_Character::tileSystem;
AStarSearch TB_Character::searcher;

//Constructor(s) & Destructor
TB_Character::TB_Character(const string& _name) : name(_name) {
	GameManager::GetInstance()->AddCharacter(this, {});

	maxHealth = 10;
	health = maxHealth;
	movementSpeed = 5.0f;
	teamNumber = 0;	
	influenceRadius = 0;
	currentState = nullptr;
	mesh = nullptr;
	rotation = 0.0f;

}

TB_Character::~TB_Character() {
	GameManager::GetInstance()->RemoveCharacter(this, {});

	//When we die make sure to remove our influence and shit.
}

//Function(s)
void TB_Character::Move(double _deltaTime, list<std::pair<int, int> >& _path) {
	//Always chope the tile we are in, and the tile we wanna move to.
	int currentRow = static_cast<int>(position.y + 0.5f);
	int currentColumn = static_cast<int>(position.x + 0.5f);
	//Chope!
	tileSystem.GetTile(currentRow, currentColumn).occupant = GetID();
	
	//If got nowhere to go, then fuck it.
	if (_path.empty()) {
		return;
	}
	
	//This is where we're trying to go.
	Vector2 destination(static_cast<float>(_path.begin()->second), static_cast<float>(_path.begin()->first));
	//Make sure that where we wanna go is not choped by someone else.
	if (searcher.IsTraversable(&tileSystem, GetID(), _path.begin()->first, _path.begin()->second) == false) {
		//If it is, then the path is invalid. If it is invalid, throw it away, forget yesterday, we'll make the great escape!
		_path.clear();
		return;
	} else {
		//Otherwise, we quickly put our tissue paper on it.
		tileSystem.GetTile(_path.begin()->first, _path.begin()->second).occupant = GetID();
		//remove our current influencce
		RemoveInfluence();
	}

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
		_path.erase(_path.begin());
		//Also, unchope our previous tile.
		tileSystem.GetTile(currentRow, currentColumn).occupant = -1;
	}

	//cout << "Position: " << position.x << "," << position.y << endl;
}

bool TB_Character::SetInfluence() {
	return SetInfluence(influenceRadius);
}

bool TB_Character::SetInfluence(int radius) {
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

	for (int row = minRow; row < maxRow; ++row)
	{
		for (int col = minCol; col < maxCol; ++col)
		{
			tileSystem.GetTile(col, row).ids.push_back(pair<int, int>(this->GetID(), this->GetType()));
		}
	}
	//Chope!



	return true;
}

//Same thing as above but opposite.
bool TB_Character::RemoveInfluence() {
	return RemoveInfluence(influenceRadius);
}

bool TB_Character::RemoveInfluence(int radius) {
	if (tileSystem.GetTileMap() == nullptr) {
		return false;
	}

	//Do stuff here.

	int currentRow = static_cast<int>(position.y + 0.5f);
	int currentColumn = static_cast<int>(position.x + 0.5f);

	int minRow = Math::Max(currentRow - radius, 0);
	int maxRow = Math::Min(currentRow + radius, tileSystem.GetNumRows());

	int minCol = Math::Max(currentColumn - radius, 0);
	int maxCol = Math::Min(currentColumn + radius, tileSystem.GetNumColumns());

	for (int row = minRow; row < maxRow; ++row)
	{
		for (int col = minCol; col < maxCol; ++col)
		{
			tileSystem.GetTile(col, row).ids.remove(pair<int, int>(this->GetID(), this->GetType()));
		}
	}

	return true;
}


//Pretty standard stuff. These health functions are virtual so child classes can modify them, such as have shields or stuff.
int TB_Character::GetHealth() const {
	return health;
}

int TB_Character::GetMaxHealth() const {
	return maxHealth;
}

float TB_Character::GetHealthPercentage() {
	return ((float)this->health / (float)this->maxHealth)*100.f;
}

void TB_Character::TakeDamage(int _damage) {
	health -= _damage;
}

bool TB_Character::IsAlive() {
	return health > 0;
}

//How many squares can the character move i
float TB_Character::GetSpeed() const {
	return movementSpeed;
}

//Team Number, self-explainatory.
int TB_Character::GetTeamNumber() const {
	return teamNumber;
}

//Virtual Function(s)
void TB_Character::HandleMessage() {
	while (!messageQueue.empty()) {
		if (currentState != nullptr) {
			currentState->ReceiveMessage(messageQueue.front());
		}		
		delete messageQueue.front();
		messageQueue.pop();
	}
}

void TB_Character::RenderUI() {
}