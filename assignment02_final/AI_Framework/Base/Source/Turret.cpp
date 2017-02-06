#include "Turret.h"
#include "MeshBuilder.h"
#include "GraphicsManager.h"
#include "EntityManager.h"
#include "RenderHelper.h"
#include "LoadTGA.h"
#include "Bullet.h"
#include "GameManager.h"

// Include Messages
#include "CharacterCreated.h"
#include "ExistingCharacterCheck.h"
#include "ExistingCharacterReply.h"

// Include States
#include "Turret_Attack.h"
#include "Turret_Reload.h"
#include "Turret_Idle.h"

Turret::Turret() : Character("Turret", CharacterType::TURRET), info(GetID())
{
	MeshBuilder::GetInstance()->GenerateQuad("Turret", Color(1, 0, 0), 1.0f);
	MeshBuilder::GetInstance()->GetMesh("Turret")->textureID = LoadTGA("Image//Turret.tga");

	if (!MeshBuilder::GetInstance()->HasMesh("HP Bar")) {
		MeshBuilder::GetInstance()->GenerateQuad("HP Bar", Color(1, 0, 0), 1.0f);
	}
	healthMesh = MeshBuilder::GetInstance()->GetMesh("HP Bar");

	if (!MeshBuilder::GetInstance()->HasMesh("Ammo Bar")) {
		MeshBuilder::GetInstance()->GenerateQuad("Ammo Bar", Color(0, 0, 1), 1.0f);
	}
	ammoMesh = MeshBuilder::GetInstance()->GetMesh("Ammo Bar");

	// Bullet
	maxBullet = 30;
	numBullet = maxBullet;
	damage = 1;
	timeToShoot = 0.0f;
	fireRate = 1.5f;

	minRange = 6.0f;
	maxRange = 8.0f;

	influenceRadius = 3;

	currentState = new Turret_Attack(*this);
	states.insert(std::pair<string, State*>(currentState->name, currentState));
	currentState = new Turret_Reload(*this);
	states.insert(std::pair<string, State*>(currentState->name, currentState));
	currentState = new Turret_Idle(*this);
	states.insert(std::pair<string, State*>(currentState->name, currentState));

	previousState = currentState->name;
	currentStateName = currentState->name;

	//Send creation message.
	CharacterCreated creationMessage(GetID());
	creationMessage.teamNumber = teamNumber;
	creationMessage.name = name;
	EntityManager::GetInstance()->BroadcastMessage(&creationMessage);

	influenceTable.insert(std::pair<int, int>(CharacterType::ATTACKER, 3)); //Go closer to supports.
	influenceTable.insert(std::pair<int, int>(CharacterType::SUPPORT, 4)); //Go closer to supports.
	influenceTable.insert(std::pair<int, int>(CharacterType::GUARDIAN, -1)); //Go closer to guardians.
	influenceTable.insert(std::pair<int, int>(CharacterType::BRAWLER, 2)); //and brawlers
	influenceTable.insert(std::pair<int, int>(CharacterType::GUNNER, -2)); //Avoid Gunners
}

Turret::~Turret()
{
}

int Turret::GetNumBullet()
{
	return numBullet;
}

int Turret::GetMaxBullet()
{
	return maxBullet;
}

int Turret::GetDamage()
{
	return damage;
}

float Turret::GetMinRange()
{
	return minRange;
}

float Turret::GetMaxRange()
{
	return maxRange;
}

void Turret::Shoot()
{
	if (timeToShoot <= 0.0f && numBullet > 0)
	{
		timeToShoot = 1.0 / fireRate;
		--numBullet;
		Bullet* bullet = GameManager::GetInstance()->GetBullet();
		bullet->SetActive();
		bullet->damage = damage;
		bullet->position = this->position;
		bullet->teamNumber = teamNumber;
		bullet->mesh = MeshBuilder::GetInstance()->GetMesh("Red Bullet");
		bullet->direction.Set(cos(Math::DegreeToRadian(rotation)), sin(Math::DegreeToRadian(rotation)));
	}
}

void Turret::Reload()
{
	if (numBullet < maxBullet)
	{
		++numBullet;
	}
}

void Turret::SetNextState(const string& nextState)
{
	cout << "Turret State: " << nextState << endl;
	for (map<string, State*>::iterator iter = states.begin(); iter != states.end(); ++iter) {
		if (iter->first == nextState) {
			currentState = iter->second;
			break;
		}
	}
	previousState = currentStateName;
	currentStateName = currentState->name;

}

const string& Turret::GetPreviousState() const
{
	return previousState;
}

const string& Turret::GetCurrentState() const
{
	return currentStateName;
}

void Turret::HandleMessage()
{
	while (!messageQueue.empty()) {
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
				if (entityPtr != nullptr) {
					ExistingCharacterReply reply(GetID());
					reply.teamNumber = teamNumber;
					entityPtr->ReceiveMessage(&reply);
				}
				messageResolved = true;
			}
		}

		if (!messageResolved) {
			const ExistingCharacterReply* checkMessage = dynamic_cast<ExistingCharacterReply*>(messageQueue.front());
			if (checkMessage != nullptr && checkMessage->teamNumber != teamNumber) {
				EntityBase* entityPtr = EntityManager::GetInstance()->GetEntityByID(checkMessage->senderID);
				if (entityPtr != nullptr) {
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

void Turret::Update(double dt)
{
	if (currentState == nullptr)
	{
		cout << "Turret's current state is NULL" << endl;
		return;
	}

	UpdateDetectedCharacters(dt);

	HandleMessage();
	currentState->Update(dt);

	info.name = name;
	info.position = position;
	info.health = GetHealth();
	info.maxHealth = GetMaxHealth();
	info.isAlive = IsAlive();
	info.currentState = currentState->name;
	info.teamNumber = teamNumber;
	EntityManager::GetInstance()->BroadcastMessage(&info);
	timeToShoot -= dt;
}

void Turret::Render()
{
	mesh = MeshBuilder::GetInstance()->GetMesh("Turret");

	MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
	modelStack.PushMatrix();
		modelStack.Translate(position.x, position.y, 10);
		//Render the main body.
		modelStack.PushMatrix();
			modelStack.Rotate(rotation, 0, 0, 1);
			RenderHelper::RenderMesh(mesh);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
			modelStack.Translate(0, 0.5f, 1);
			modelStack.Scale(Math::Max(0.01f, static_cast<float>(health) / static_cast<float>(maxHealth)), 0.2f, 1);
			RenderHelper::RenderMesh(healthMesh);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
			modelStack.Translate(0, 0.7f, 1);
			modelStack.Scale(Math::Max(0.01f, static_cast<float>(numBullet) / static_cast<float>(maxBullet)), 0.2f, 1);
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

void Turret::RenderUI()
{
}