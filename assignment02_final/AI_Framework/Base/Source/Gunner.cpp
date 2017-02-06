#include "Gunner.h"
#include "MeshBuilder.h"
#include "GL\glew.h"
#include "LoadTGA.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"
#include "EntityManager.h"
#include "GameManager.h"

#include "Bullet.h"

//include states
#include "Gunner_Chase.h"
#include "Gunner_Reload.h"
#include "Gunner_Patrol.h"
#include "Gunner_Attack.h"
#include "Gunner_Run.h"

//Include Messages
#include "CharacterCreated.h"
#include "ExistingCharacterCheck.h"
#include "ExistingCharacterReply.h"

//Constructor(s) & Destructor
Gunner::Gunner() : Character("Gunner",4) 
, cf_fireRate(2.f)
, cf_reloadTime(2.0f)
, cf_detectionRange(10.f)
, cf_AttackRange(5.f)
, info(GetID())
{
	//Initialise Mesh
	if (!MeshBuilder::GetInstance()->HasMesh("Gunner")) {
		MeshBuilder::GetInstance()->GenerateQuad("Gunner", Color(1, 1, 1), 1.0f);
		MeshBuilder::GetInstance()->GetMesh("Gunner")->textureID = LoadTGA("Image//Gunner.tga");
	}
	mesh = MeshBuilder::GetInstance()->GetMesh("Gunner");
	radius = 0.5f;

	if(!MeshBuilder::GetInstance()->HasMesh("HP Bar")) {
		MeshBuilder::GetInstance()->GenerateQuad("HP Bar",Color(1,0,0),1.0f);
	}
	healthMesh = MeshBuilder::GetInstance()->GetMesh("HP Bar");

	if(!MeshBuilder::GetInstance()->HasMesh("Ammo Bar")) {
		MeshBuilder::GetInstance()->GenerateQuad("Ammo Bar",Color(0,0,1),1.0f);
	}
	ammoMesh = MeshBuilder::GetInstance()->GetMesh("Ammo Bar");

	//Initialise Variables	
	//Character Stuff
	teamNumber = 1;

	//Movement
	f_movSpeed = 5.0f;

	//Combat
	i_magSize = 10;
	i_ammo = i_magSize;
	i_damage = 5;

	//Initialise States
	currentState = new Gunner_Run(*this);
	states.insert(std::pair<string, State*>(currentState->name, currentState));

	currentState = new Gunner_Chase(*this);
	states.insert(std::pair<string, State*>(currentState->name, currentState));

	currentState = new Gunner_Reload(*this);
	states.insert(std::pair<string, State*>(currentState->name, currentState));

	currentState = new Gunner_Attack(*this);
	states.insert(std::pair<string, State*>(currentState->name, currentState));

	currentState = new Gunner_Patrol(*this);
	states.insert(std::pair<string, State*>(currentState->name, currentState));

	CharacterCreated creationMessage(GetID());
	creationMessage.teamNumber= teamNumber;
	creationMessage.name = name;
	EntityManager::GetInstance()->BroadcastMessage(&creationMessage);
}

Gunner::~Gunner() {
}

//Interface Function(s)
void Gunner::SetNextState(const string& nextState) {
	map<string, State*>::iterator mapIter = states.find(nextState);
	if (mapIter != states.end()) {
		//info.previousState = currentState->name;
		currentState = mapIter->second;
		//info.currentState = currentState->name;
	}
}

void Gunner::Reset() {
	i_ammo = i_magSize;
}

void Gunner::Update(double deltaTime) {

	if (currentState == nullptr) {
		cout << "Gunner's currentState is NULL!" << endl;
		return;
	}

	HandleMessage();
	currentState->Update(deltaTime);

	info.name = name;
	info.teamNumber = GetTeamNumber();
	info.position = position;
	info.health = GetHealth();
	info.maxHealth = GetMaxHealth();
	info.isAlive = IsAlive();	
	SendMessageToObservers(&info);
}

void Gunner::Render() {
	MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
	modelStack.PushMatrix();
	modelStack.Translate(position.x,position.y,0);

	//Render the main body.
	modelStack.PushMatrix();
	modelStack.Rotate(rotation,0,0,1);
	modelStack.Scale(radius * 2.0f,radius * 2.0f,1.0f);
	RenderHelper::RenderMesh(mesh);
	modelStack.PopMatrix();

	//Render the State
	string state = currentState->name.substr(7);
	modelStack.PushMatrix();
	float textScale = 0.3f;
	modelStack.Translate(-textScale * (static_cast<float>(state.length())* 0.5f), -GetRadius() - 1, -3);
	modelStack.Scale(textScale,textScale,1);
	//strcpy(&state, currentState->name.c_str()[7]);
	RenderHelper::RenderText(MeshBuilder::GetInstance()->GetMesh("text"), state, Color(1, 1, 1));
	modelStack.PopMatrix();

	//Render the Health
	modelStack.PushMatrix();
	modelStack.Translate(0,GetRadius() + 0.2f,0);
	float healthBarScale = Math::Max((static_cast<float>(health) / static_cast<float>(maxHealth)),0.001f);
	modelStack.Scale(healthBarScale,0.2f,1);
	RenderHelper::RenderMesh(healthMesh);
	modelStack.PopMatrix();

	//Render the Ammo		
	float ammoBarSize = 0.1f;
	for(unsigned int i = 0; i < i_magSize; ++i) {
		modelStack.PushMatrix();
		modelStack.Translate(-static_cast<float>(i_magSize - 1) * ammoBarSize * 0.5f + (i * ammoBarSize),GetRadius() + 0.3f,0);
		modelStack.Scale(ammoBarSize * 0.8f,0.2f,1);
		RenderHelper::RenderMesh(ammoMesh);
		modelStack.PopMatrix();
	}


	modelStack.PopMatrix();
}

void Gunner::RenderUI() {
}


int Gunner::GetAmmo()
{
	return this->i_ammo;
}

void Gunner::Reload()
{
	i_ammo = i_magSize;
}

void Gunner::Shoot(Vector3& direction)
{
	Bullet* bullet = GameManager::GetInstance()->GetBullet();
	bullet->teamNumber = this->teamNumber;
	bullet->SetActive();
	bullet->damage = i_damage;
	bullet->direction = Vector2(direction.x,direction.y);
	bullet->position = this->position;
	//bullet->mesh = MeshBuilder::GetInstance()->GetMesh("Enemy_Bullet");

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


	this->i_ammo--;
}

void Gunner::InitPos(Vector3& pos)
{
	this->position = Vector2(pos.x,pos.y);
	this->initialPos = pos;
	map<string, State*>::iterator mapIter = states.find("Gunner_Patrol");
	if (mapIter != states.end()) 
	{
		Gunner_Patrol* s = dynamic_cast<Gunner_Patrol*>(mapIter->second);
		if (s)
		{
			s->SetPatrolPoint(pos);
		}
	}
}

void Gunner::HandleMessage() {
	while(!messageQueue.empty()) {
		bool messageResolved = false;;
		if(!messageResolved) {
			const ExistingCharacterCheck* checkMessage = dynamic_cast<ExistingCharacterCheck*>(messageQueue.front());
			if(checkMessage != nullptr) {
				EntityBase* entityPtr = EntityManager::GetInstance()->GetEntityByID(checkMessage->senderID);
				if(entityPtr != nullptr) {
					ExistingCharacterReply reply(GetID());
					reply.teamNumber = this->teamNumber;
					entityPtr->ReceiveMessage(&reply);
				}
				messageResolved = true;
			}
		}

		if(currentState != nullptr) {
			currentState->ReceiveMessage(messageQueue.front());
		}
		delete messageQueue.front();
		messageQueue.pop();
	}
}