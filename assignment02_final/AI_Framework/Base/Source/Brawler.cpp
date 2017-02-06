#include "Brawler.h"
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
#include "Brawler_Attack.h"
#include "Brawler_Run.h"
#include "Brawler_Move.h"

Brawler::Brawler() : Character("Brawler", CharacterType::BRAWLER), info(GetID())
{
	MeshBuilder::GetInstance()->GenerateQuad("Brawler", Color(1, 0, 0), 1.0f);
	MeshBuilder::GetInstance()->GetMesh("Brawler")->textureID = LoadTGA("Image//Brawler.tga");

	if (!MeshBuilder::GetInstance()->HasMesh("HP Bar")) {
		MeshBuilder::GetInstance()->GenerateQuad("HP Bar", Color(1, 0, 0), 1.0f);
	}
	healthMesh = MeshBuilder::GetInstance()->GetMesh("HP Bar");

	maxHealth = 30;
	health = maxHealth;

	damage = 1;
	timeToAttack = 0.0f;
	attackRate = 1.f;

	minRange = 2.0f;
	maxRange = 2.0f;

	influenceRadius = 3;

	currentState = new Brawler_Attack(*this);
	states.insert(std::pair<string, State*>(currentState->name, currentState));
	currentState = new Brawler_Run(*this);
	states.insert(std::pair<string, State*>(currentState->name, currentState));
	currentState = new Brawler_Move(*this);
	states.insert(std::pair<string, State*>(currentState->name, currentState));

	previousStateName = currentState->name;
	currentStateName = currentState->name;

	CharacterCreated creationMessage(GetID());
	creationMessage.teamNumber = teamNumber;
	creationMessage.name = name;
	EntityManager::GetInstance()->BroadcastMessage(&creationMessage);

	influenceTable.insert(std::pair<int, int>(CharacterType::SUPPORT, 3)); 
	influenceTable.insert(std::pair<int, int>(CharacterType::GUARDIAN, 4));
	influenceTable.insert(std::pair<int, int>(CharacterType::TURRET, -1));
	influenceTable.insert(std::pair<int, int>(CharacterType::ATTACKER, 2));
	influenceTable.insert(std::pair<int, int>(CharacterType::GUNNER, -2));
}

Brawler::~Brawler()
{
}

int Brawler::GetDamage()
{
	return damage;
}

float Brawler::GetMinRange()
{
	return minRange;
}

float Brawler::GetMaxRange()
{
	return maxRange;
}

void Brawler::Attack(int _targetID)
{
	if (timeToAttack <= 0.0f)
	{
		timeToAttack = 1.0 / attackRate;
		Character* target = dynamic_cast<Character*>(EntityManager::GetInstance()->GetEntityByID(_targetID));
		if (target != nullptr) {
			target->TakeDamage(damage);
		}
	}
}

void Brawler::SetNextState(const string& nextState)
{
	cout << "Brawler State: " << nextState << endl;
	for (map<string, State*>::iterator iter = states.begin(); iter != states.end(); ++iter) {
		if (iter->first == nextState) {
			currentState = iter->second;
			break;
		}
	}
	previousStateName = currentStateName;
	currentStateName = currentState->name;
}

const string& Brawler::GetPreviousState() const {
	return previousStateName;
}

const string& Brawler::GetCurrentState() const {
	return currentStateName;
}

void Brawler::HandleMessage()
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

void Brawler::Update(double dt)
{
	if (currentState == nullptr)
	{
		cout << "Brawler's current state is NULL" << endl;
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
	timeToAttack -= dt;
}

void Brawler::Render()
{
	mesh = MeshBuilder::GetInstance()->GetMesh("Brawler");

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

		string state = currentState->name;
		modelStack.PushMatrix();
		float textScale = 0.3f;
		modelStack.Translate(-textScale * (static_cast<float>(state.length())* 0.5f), -GetRadius() - 0.5f, -3);
		modelStack.Scale(textScale, textScale, 1);
		RenderHelper::RenderText(MeshBuilder::GetInstance()->GetMesh("text"), state, Color(1, 1, 1));
		modelStack.PopMatrix();
	modelStack.PopMatrix();

}