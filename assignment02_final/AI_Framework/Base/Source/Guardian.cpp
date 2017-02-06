#include "Guardian.h"
#include "EntityManager.h"
#include "MeshBuilder.h"
#include "LoadTGA.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"
#include "Bullet.h"
#include "GameManager.h"

//Include States
#include "Guardian_Defend.h"
#include "Guardian_Follow.h"
#include "Guardian_Run.h"
#include "Guardian_Attack.h"

//Include Messages
#include "ExistingCharacterCheck.h"
#include "ExistingCharacterReply.h"
#include "CharacterCreated.h"

Guardian::Guardian() : Character("Guardian", CharacterType::GUARDIAN), info(GetID()) {
	//Initialise Mesh
	if (!MeshBuilder::GetInstance()->HasMesh("Guardian Green")) {
		MeshBuilder::GetInstance()->GenerateQuad("Guardian Green", Color(1,0,0),1.0f);
		MeshBuilder::GetInstance()->GetMesh("Guardian Green")->textureID = LoadTGA("Image//Guardian_Green.tga");
	}
	if (!MeshBuilder::GetInstance()->HasMesh("Guardian Purple")) {
		MeshBuilder::GetInstance()->GenerateQuad("Guardian Purple", Color(1,0,0),1.0f);
		MeshBuilder::GetInstance()->GetMesh("Guardian Purple")->textureID = LoadTGA("Image//Guardian_Purple.tga");
	}
	if (!MeshBuilder::GetInstance()->HasMesh("Guardian Red")) {
		MeshBuilder::GetInstance()->GenerateQuad("Guardian Red", Color(1,0,0),1.0f);
		MeshBuilder::GetInstance()->GetMesh("Guardian Red")->textureID = LoadTGA("Image//Guardian_Red.tga");
	}
	if (!MeshBuilder::GetInstance()->HasMesh("Guardian Blue")) {
		MeshBuilder::GetInstance()->GenerateQuad("Guardian Blue", Color(1,0,0),1.0f);
		MeshBuilder::GetInstance()->GetMesh("Guardian Blue")->textureID = LoadTGA("Image//Guardian_Blue.tga");
	}
	if (!MeshBuilder::GetInstance()->HasMesh("Guardian Yellow")) {
		MeshBuilder::GetInstance()->GenerateQuad("Guardian Yellow", Color(1,0,0),1.0f);
		MeshBuilder::GetInstance()->GetMesh("Guardian Yellow")->textureID = LoadTGA("Image//Guardian_Yellow.tga");
	}

	switch (teamNumber) {
	case 0:
		mesh = MeshBuilder::GetInstance()->GetMesh("Guardian Blue");
		break;
	case 1:
		mesh = MeshBuilder::GetInstance()->GetMesh("Guardian Red");
		break;
	case 2:
		mesh = MeshBuilder::GetInstance()->GetMesh("Guardian Green");
		break;
	case 3:
		mesh = MeshBuilder::GetInstance()->GetMesh("Guardian Purple");
		break;
	default:
		mesh = MeshBuilder::GetInstance()->GetMesh("Guardian Yellow");
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

	if (!MeshBuilder::GetInstance()->HasMesh("Shield Mesh")) {
		MeshBuilder::GetInstance()->GenerateQuad("Shield Mesh", Color(1, 1, 1));
		MeshBuilder::GetInstance()->GetMesh("Shield Mesh")->textureID = LoadTGA("Image//Shield.tga");
	}
	shieldMesh = MeshBuilder::GetInstance()->GetMesh("Shield Mesh");

	if(!MeshBuilder::GetInstance()->HasMesh("HP Bar")) {
		MeshBuilder::GetInstance()->GenerateQuad("HP Bar",Color(1,0,0),1.0f);
	}
	healthMesh = MeshBuilder::GetInstance()->GetMesh("HP Bar");

	if(!MeshBuilder::GetInstance()->HasMesh("Red Circle")) {
		MeshBuilder::GetInstance()->GenerateRing("Red Circle",Color(1,0,0),32,1.0f,0.999f);
	}
	redCircleMesh = MeshBuilder::GetInstance()->GetMesh("Red Circle");

	if(!MeshBuilder::GetInstance()->HasMesh("Blue Circle")) {
		MeshBuilder::GetInstance()->GenerateRing("Blue Circle",Color(0,0,1),32,1.0f,0.999f);
	}
	blueCircleMesh = MeshBuilder::GetInstance()->GetMesh("Blue Circle");

	maxShield = 6;
	shield = maxShield;
	radius = 0.5f;
	shieldRadius = 1.0f;
	shieldRechargeTimer = 0.0f;
	buffTimer = 0.0;
	priorityRange = 6.0f;
	movementSpeed = 6.0f;
	
	combatRange = 8.0f;	
	timeToShoot = 0.0;
	fireRate = 1.0;
	damage = 2;
	
	//Initialise States
	currentState = new Guardian_Attack(*this);
	states.insert(std::pair<string,State*>(currentState->name, currentState));
	currentState = new Guardian_Defend(*this);
	states.insert(std::pair<string,State*>(currentState->name, currentState));
	currentState = new Guardian_Run(*this);
	states.insert(std::pair<string,State*>(currentState->name, currentState));
	currentState = new Guardian_Follow(*this);
	states.insert(std::pair<string,State*>(currentState->name, currentState));
	
	//Send creation message.
	CharacterCreated creationMessage(GetID());
	creationMessage.teamNumber = teamNumber;
	creationMessage.name = name;
	EntityManager::GetInstance()->BroadcastMessage(&creationMessage);

	influenceTable.insert(std::pair<int, int>(CharacterType::SUPPORT, -10)); //Go closer to supports.
	influenceTable.insert(std::pair<int, int>(CharacterType::GUARDIAN, -10)); //Go closer to guardians.
	influenceTable.insert(std::pair<int, int>(CharacterType::TURRET, 5)); //Avoid down those turrets
	influenceTable.insert(std::pair<int, int>(CharacterType::BRAWLER, 10)); //and brawlers
	influenceTable.insert(std::pair<int, int>(CharacterType::GUNNER, 10)); //and Gunners
}

Guardian::~Guardian() {
}

void Guardian::SetNextState(const string& _nextState) {
	cout << "Guardian State: " << _nextState << endl;
	for (map<string, State*>::iterator iter = states.begin(); iter != states.end(); ++iter) {
		if (iter->first == _nextState) {
			currentState = iter->second;
			break;
		}
	}
}

void Guardian::HandleMessage() {
	while(!messageQueue.empty()) {
		bool messageResolved = false;

		if (!messageResolved) {
			const CharacterCreated* creationMessage = dynamic_cast<CharacterCreated*>(messageQueue.front());
			if (creationMessage != nullptr) {
				if (creationMessage->teamNumber == teamNumber && creationMessage->name == "Support") {
					AddSubject(*EntityManager::GetInstance()->GetEntityByID(creationMessage->senderID));
				}
			}
		}

		if(!messageResolved) {
			const ExistingCharacterCheck* checkMessage = dynamic_cast<ExistingCharacterCheck*>(messageQueue.front());
			if(checkMessage != nullptr) {
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
			const ExistingCharacterReply* replyMessage = dynamic_cast<ExistingCharacterReply*>(messageQueue.front());
			if(replyMessage != nullptr) {
				EntityBase* entityPtr = EntityManager::GetInstance()->GetEntityByID(replyMessage->senderID);
				if(entityPtr != nullptr) {
					Character* characterPtr = dynamic_cast<Character*>(entityPtr);
					if (characterPtr != nullptr && characterPtr->name == "Support" && replyMessage->teamNumber == teamNumber) {
						AddSubject(*entityPtr);
					}					
				}
				messageResolved = true;
			}
		}

		currentState->ReceiveMessage(messageQueue.front());
		delete messageQueue.front();
		messageQueue.pop();
	}
}

void Guardian::SetShield(const int _shield) {
	if (shield < 0) {
		this->shield = _shield;
	} else {
		this->shield = _shield;
	}
}

int Guardian::GetShield() const {
	return shield;
}

int Guardian::GetMaxShield() const {
	return maxShield;
}

void Guardian::Buff() {
	if (buffTimer <= 0.0) {
		shield += 5;
		buffTimer = 1.0;
	}
}

void Guardian::Heal(const int _health) {
	this->health = Math::Min(maxHealth, this->health + _health);
}

int Guardian::GetDamage() const {
	return damage;
}

void Guardian::Shoot() {
	if (timeToShoot <= 0.0f) {
		timeToShoot = 1.0 / fireRate;
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
		mat.SetToRotation(rotation, 0, 0, 1);
		Vector3 bulletDirection = mat * direction;
		bullet->direction.Set(bulletDirection.x, bulletDirection.y);
	}
}

void Guardian::TakeDamage(const int _damage) {
	if (this->shield >= _damage) {
		this->shield -= _damage;
	} else {
		this->health -= (_damage - shield);
		shield = 0;
	}
}

float Guardian::GetRadius() const {
	return radius + shieldRadius;
}

float Guardian::GetCombatRange() const {
	return combatRange;
}

float Guardian::GetPriorityRange() const {
	return priorityRange;
}

void Guardian::Update(double _deltaTime) {
	if (IsAlive() == false) {
		return;
	}

	if (currentState == nullptr) {
		cout << "Guardian's currentState is NULL!" << endl;
		return;
	}

	buffTimer -= _deltaTime;
	shieldRadius = (1.0f * static_cast<float>(shield) / static_cast<float>(maxShield));
	shieldRechargeTimer -= _deltaTime;
	if (shieldRechargeTimer <= 0.0) {
		shield = Math::Min(maxShield, shield + 2);
		shieldRechargeTimer = 2.0;
	}

	timeToShoot -= _deltaTime;

	UpdateDetectedCharacters(_deltaTime);

	HandleMessage();
	currentState->Update(_deltaTime);

	//Character Info
	info.name = name;
	info.position = position;
	info.health = GetHealth();
	info.maxHealth = GetMaxHealth();
	info.isAlive = IsAlive();
	info.teamNumber = teamNumber;
	info.currentState = currentState->name;
	EntityManager::GetInstance()->BroadcastMessage(&info);
}

void Guardian::Render() {
	switch (teamNumber) {
	case 0:
		mesh = MeshBuilder::GetInstance()->GetMesh("Guardian Blue");
		break;
	case 1:
		mesh = MeshBuilder::GetInstance()->GetMesh("Guardian Red");
		break;
	case 2:
		mesh = MeshBuilder::GetInstance()->GetMesh("Guardian Green");
		break;
	case 3:
		mesh = MeshBuilder::GetInstance()->GetMesh("Guardian Purple");
		break;
	default:
		mesh = MeshBuilder::GetInstance()->GetMesh("Guardian Yellow");
	}

	MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
	modelStack.PushMatrix();
	modelStack.Translate(position.x, position.y, 0);

		//Render the main body.
		modelStack.PushMatrix();
			modelStack.Rotate(rotation, 0, 0, 1);
			RenderHelper::RenderMesh(mesh);
		modelStack.PopMatrix();

		//Render the shield
		if (shield > 0) {
			modelStack.PushMatrix();
				modelStack.Translate(0, 0, -1);
				modelStack.Scale(GetRadius() * 2.0f, GetRadius() * 2.0f, 1);
				RenderHelper::RenderMesh(shieldMesh);
			modelStack.PopMatrix();
		}	

		//Render the Health
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

	//RenderPath();
}

void Guardian::RenderUI() {
}