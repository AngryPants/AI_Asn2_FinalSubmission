#include "Attacker.h"
#include "MeshBuilder.h"
#include "GraphicsManager.h"
#include "EntityManager.h"
#include "RenderHelper.h"
#include "LoadTGA.h"
#include "Bullet.h"
#include "GameManager.h"

//Include Messages
#include "CharacterCreated.h"
#include "ExistingCharacterCheck.h"
#include "ExistingCharacterReply.h"

//Include States
#include "Attacker_Attack.h"
#include "Attacker_Move.h"
#include "Attacker_Run.h"
#include "Attacker_Reload.h"

Attacker::Attacker() : Character("Attacker", CharacterType::ATTACKER), info(GetID()) {
	//Init Meshes
	if (!MeshBuilder::GetInstance()->HasMesh("Attacker Blue")) {
		MeshBuilder::GetInstance()->GenerateQuad("Attacker Blue", Color(1, 0, 0), 1.0f);
		MeshBuilder::GetInstance()->GetMesh("Attacker Blue")->textureID = LoadTGA("Image//Attacker_Blue.tga");
	}	
	if (!MeshBuilder::GetInstance()->HasMesh("Attacker Red")) {
		MeshBuilder::GetInstance()->GenerateQuad("Attacker Red", Color(1, 0, 0), 1.0f);
		MeshBuilder::GetInstance()->GetMesh("Attacker Red")->textureID = LoadTGA("Image//Attacker_Red.tga");
	}
	if (!MeshBuilder::GetInstance()->HasMesh("Attacker Purple")) {
		MeshBuilder::GetInstance()->GenerateQuad("Attacker Purple", Color(1, 0, 0), 1.0f);
		MeshBuilder::GetInstance()->GetMesh("Attacker Purple")->textureID = LoadTGA("Image//Attacker_Purple.tga");
	}
	if (!MeshBuilder::GetInstance()->HasMesh("Attacker Green")) {
		MeshBuilder::GetInstance()->GenerateQuad("Attacker Green", Color(1, 0, 0), 1.0f);
		MeshBuilder::GetInstance()->GetMesh("Attacker Green")->textureID = LoadTGA("Image//Attacker_Green.tga");
	}
	if (!MeshBuilder::GetInstance()->HasMesh("Attacker Yellow")) {
		MeshBuilder::GetInstance()->GenerateQuad("Attacker Yellow", Color(1, 0, 0), 1.0f);
		MeshBuilder::GetInstance()->GetMesh("Attacker Yellow")->textureID = LoadTGA("Image//Attacker_Yellow.tga");
	}

	switch (teamNumber) {
	case 0:
		mesh = MeshBuilder::GetInstance()->GetMesh("Attacker Blue");
		break;
	case 1:
		mesh = MeshBuilder::GetInstance()->GetMesh("Attacker Red");
		break;
	case 2:
		mesh = MeshBuilder::GetInstance()->GetMesh("Attacker Green");
		break;
	case 3:
		mesh = MeshBuilder::GetInstance()->GetMesh("Attacker Purple");
		break;
	default:
		mesh = MeshBuilder::GetInstance()->GetMesh("Attacker Yellow");
	}

	if (MeshBuilder::GetInstance()->HasMesh("Blue Bullet") == false) {
		MeshBuilder::GetInstance()->GenerateSphere("Blue Bullet",Color(0,0,1),12,12,0.5f);
	}
	if (MeshBuilder::GetInstance()->HasMesh("Green Bullet") == false) {
		MeshBuilder::GetInstance()->GenerateSphere("Green Bullet",Color(0,0,1),12,12,0.5f);
	}
	if (MeshBuilder::GetInstance()->HasMesh("Purple Bullet") == false) {
		MeshBuilder::GetInstance()->GenerateSphere("Purple Bullet",Color(0,0,1),12,12,0.5f);
	}
	if (MeshBuilder::GetInstance()->HasMesh("Red Bullet") == false) {
		MeshBuilder::GetInstance()->GenerateSphere("Red Bullet",Color(0,0,1),12,12,0.5f);
	}
	if (MeshBuilder::GetInstance()->HasMesh("Yellow Bullet") == false) {
		MeshBuilder::GetInstance()->GenerateSphere("Yellow Bullet",Color(1,1,0),12,12,0.5f);
	}

	if (!MeshBuilder::GetInstance()->HasMesh("HP Bar")) {
		MeshBuilder::GetInstance()->GenerateQuad("HP Bar", Color(1, 0, 0), 1.0f);
	}
	healthMesh = MeshBuilder::GetInstance()->GetMesh("HP Bar"); 
	if(!MeshBuilder::GetInstance()->HasMesh("Ammo Bar")) {
		MeshBuilder::GetInstance()->GenerateQuad("Ammo Bar",Color(0,0,1),1.0f);
	}
	ammoMesh = MeshBuilder::GetInstance()->GetMesh("Ammo Bar");
	if (!MeshBuilder::GetInstance()->HasMesh("Detection Radius")) {
		MeshBuilder::GetInstance()->GenerateRing("Detection Radius", Color(1, 0, 0), 32, 1.0f, 0.999f);
	}
	detectionRadiusMesh = MeshBuilder::GetInstance()->GetMesh("Detection Radius");
	if(!MeshBuilder::GetInstance()->HasMesh("Combat Radius")) {
		MeshBuilder::GetInstance()->GenerateRing("Combat Radius", Color(0, 0, 1),32, 1.0f, 0.999f);
	}
	combatRadiusMesh = MeshBuilder::GetInstance()->GetMesh("Combat Radius");

	//Bullets
	maxBullets = 15;
	numBullets = maxBullets;
	damage = 2;
	timeToShoot = 0.0f;
	fireRate = 2.0f;

	minCombatRange = 5.0f;
	maxCombatRange = 10.0f;
	optimalRange = (maxCombatRange + minCombatRange) * 0.5f;

	influenceRadius = 3;

	//States
	currentState = new Attacker_Attack(*this);	
	states.insert(std::pair<string, State*>(currentState->name, currentState));
	currentState = new Attacker_Run(*this);	
	states.insert(std::pair<string, State*>(currentState->name, currentState));
	currentState = new Attacker_Reload(*this);	
	states.insert(std::pair<string, State*>(currentState->name, currentState));
	currentState = new Attacker_Move(*this);
	states.insert(std::pair<string, State*>(currentState->name, currentState));

	previousStateName = currentState->name;
	currentStateName = currentState->name;
	
	//Send creation message.
	CharacterCreated creationMessage(GetID());
	creationMessage.teamNumber = teamNumber;
	creationMessage.name = name;
	EntityManager::GetInstance()->BroadcastMessage(&creationMessage);

	influenceTable.insert(std::pair<int, int>(CharacterType::SUPPORT, -2)); //Go closer to supports.
	influenceTable.insert(std::pair<int, int>(CharacterType::GUARDIAN, -2)); //Go closer to guardians.
	influenceTable.insert(std::pair<int, int>(CharacterType::TURRET, -10)); //Hunt down those turrets
	influenceTable.insert(std::pair<int, int>(CharacterType::BRAWLER, -10)); //and brawlers
	influenceTable.insert(std::pair<int, int>(CharacterType::GUNNER, 10)); //Avoid Gunners
}

Attacker::~Attacker() {
}

//Getters
int Attacker::GetNumBullets() {
	return numBullets;
}

int Attacker::GetMaxBullets() {
	return maxBullets;
}

int Attacker::GetDamage() {
	return damage;
}

float Attacker::GetMinCombatRange() {
	return minCombatRange;
}

float Attacker::GetMaxCombatRange() {
	return maxCombatRange;
}

float Attacker::GetOptimalRange() {
	return optimalRange;
}

void Attacker::Shoot(float _bulletSpread) {
	if (_bulletSpread > 20.0f) {
		_bulletSpread = 20.0f;
	} else if (_bulletSpread < -20.0f) {
		_bulletSpread = -20.0f;
	}

	if (timeToShoot <= 0.0f && numBullets > 0) {
		timeToShoot = 1.0 / fireRate;
		--numBullets;
		Bullet* bullet = GameManager::GetInstance()->GetBullet();
		bullet->SetActive();
		bullet->damage = damage;
		bullet->teamNumber = this->teamNumber;
		bullet->position = this->position;

		switch (this->teamNumber) {
			case 0:
				bullet->mesh = MeshBuilder::GetInstance()->GetMesh("Blue Bullet");
				break;
			case 1:
				bullet->mesh = MeshBuilder::GetInstance()->GetMesh("Red Bullet");
				break;
			case 2:
				bullet->mesh = MeshBuilder::GetInstance()->GetMesh("Green Bullet");
				break;
			case 3:
				bullet->mesh = MeshBuilder::GetInstance()->GetMesh("Purple Bullet");
				break;
			default:
				bullet->mesh = MeshBuilder::GetInstance()->GetMesh("Yellow Bullet");
		}

		Vector3 direction(1, 0, 0);
		Mtx44 mat;
		mat.SetToRotation(rotation + Math::RandFloatMinMax(-_bulletSpread, _bulletSpread), 0, 0, 1);
		Vector3 bulletDirection = mat * direction;
		bullet->direction.Set(bulletDirection.x, bulletDirection.y);
	}
}

void Attacker::Reload() {
	if (numBullets < maxBullets) {
		++numBullets;
	}
}

void Attacker::Buff() {
	fireRate = 10.0;
}

//Interface Function(s)
void Attacker::SetNextState(const string& _nextState) {
	cout << "Attacker State: " << _nextState << endl;
	for (map<string, State*>::iterator iter = states.begin(); iter != states.end(); ++iter) {
		if (iter->first == _nextState) {
			currentState = iter->second;
			break;
		}
	}
	previousStateName = currentStateName;
	currentStateName = currentState->name;	
}

const string& Attacker::GetPreviousStateName() const {
	return previousStateName;
}

const string& Attacker::GetCurrentStateName() const {
	return currentStateName;
}

void Attacker::HandleMessage() {
	while(!messageQueue.empty()) {
		bool messageResolved = false;
		if (!messageResolved) {
			const CharacterCreated* creationMesssage = dynamic_cast<const CharacterCreated*>(messageQueue.front());
			if (creationMesssage != nullptr && creationMesssage->teamNumber != this->teamNumber) {
				EntityBase* entityPtr = EntityManager::GetInstance()->GetEntityByID(creationMesssage->senderID);
				if (entityPtr != nullptr) {
					AddSubject(*entityPtr);
				}
				messageResolved = true;
			}
		}

		if (!messageResolved) {
			const ExistingCharacterCheck* checkMessage = dynamic_cast<ExistingCharacterCheck*>(messageQueue.front());
			if (checkMessage != nullptr) {
				EntityBase* entityPtr = EntityManager::GetInstance()->GetEntityByID(checkMessage->senderID);
				if(entityPtr != nullptr) {
					ExistingCharacterReply reply(GetID());
					reply.teamNumber = teamNumber;
					entityPtr->ReceiveMessage(&reply);
				}
				messageResolved = true;
			}
		}

		if(!messageResolved) {
			const ExistingCharacterReply* checkMessage = dynamic_cast<ExistingCharacterReply*>(messageQueue.front());
			if(checkMessage != nullptr && checkMessage->teamNumber != teamNumber) {
				EntityBase* entityPtr = EntityManager::GetInstance()->GetEntityByID(checkMessage->senderID);
				if(entityPtr != nullptr) {
					AddSubject(*entityPtr);
				}
				messageResolved = true;
			}
		}

		currentState->ReceiveMessage(messageQueue.front());
		delete messageQueue.front();
		messageQueue.pop();
	}
}

void Attacker::Update(double _deltaTime) {
	if (timeToShoot > 0.0) {
		timeToShoot = Math::Max(0.0, timeToShoot - _deltaTime);
	}
	if (currentState == nullptr) {
		cout << "Attacker's currentState is NULL!" << endl;
		return;
	}

	UpdateDetectedCharacters(_deltaTime);

	HandleMessage();
	currentState->Update(_deltaTime);

	info.name = name;
	info.position = position;
	info.health = GetHealth();
	info.maxHealth = GetMaxHealth();
	info.isAlive = IsAlive();
	info.currentState = currentState->name;
	info.teamNumber = teamNumber;
	EntityManager::GetInstance()->BroadcastMessage(&info);
	fireRate = 2.0f;
}

void Attacker::Render() {
	switch (teamNumber) {
	case 0:
		mesh = MeshBuilder::GetInstance()->GetMesh("Attacker Blue");
		break;
	case 1:
		mesh = MeshBuilder::GetInstance()->GetMesh("Attacker Red");
		break;
	case 2:
		mesh = MeshBuilder::GetInstance()->GetMesh("Attacker Green");
		break;
	case 3:
		mesh = MeshBuilder::GetInstance()->GetMesh("Attacker Purple");
		break;
	default:
		mesh = MeshBuilder::GetInstance()->GetMesh("Attacker Yellow");
	}

	MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
	modelStack.PushMatrix();
		modelStack.Translate(position.x, position.y, 0);
		//Render the main body.
		modelStack.PushMatrix();
			modelStack.Rotate(rotation, 0, 0, 1);
			modelStack.Scale(0.8f, 0.8f, 1);
			RenderHelper::RenderMesh(mesh);
		modelStack.PopMatrix();

		//Render the Health
		modelStack.PushMatrix();
			modelStack.Translate(0, 0.5f, 1);
			modelStack.Scale(Math::Max(0.01f, static_cast<float>(health) / static_cast<float>(maxHealth)), 0.2f, 1);
			RenderHelper::RenderMesh(healthMesh);
		modelStack.PopMatrix();

		//Render the Ammo
		modelStack.PushMatrix();
			modelStack.Translate(0, 0.7f, 1);
			modelStack.Scale(Math::Max(0.01f, static_cast<float>(numBullets) / static_cast<float>(maxBullets)), 0.2f, 1);
			RenderHelper::RenderMesh(ammoMesh);
		modelStack.PopMatrix();

		string state = currentState->name;
		modelStack.PushMatrix();
			float textScale = 0.3f;
			modelStack.Translate(-textScale * (static_cast<float>(state.length())* 0.5f), -GetRadius() - 0.5f, -3);
			modelStack.Scale(textScale, textScale, 1);
			RenderHelper::RenderText(MeshBuilder::GetInstance()->GetMesh("text"), state, Color(1, 1, 1));
		modelStack.PopMatrix();

	modelStack.PopMatrix();
}

void Attacker::RenderUI() {
}