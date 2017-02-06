#include "SceneAI.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "MeshBuilder.h"
#include "Application.h"
#include "Utility.h"
#include <sstream>
#include "KeyboardController.h"
#include "SceneManager.h"
#include "EntityManager.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"
#include "LoadTGA.h"
#include "GameManager.h"
#include "MessageBoard.h"

#include "TextEntity.h"
#include "SpriteEntity.h"
#include "Light.h"

//Include Characters
#include "Attacker.h"
#include "Guardian.h"
#include "Support.h"
#include "Gunner.h"
#include "Turret.h"
#include "Brawler.h"

//Constructor(s) & Destructor
SceneAI::SceneAI() {
}

SceneAI::~SceneAI() {
}

//Function(s)
void SceneAI::Init() {
	currProg = GraphicsManager::GetInstance()->LoadShader("default", "Shader//Texture.vertexshader", "Shader//Text.fragmentshader");
	
	// Tell the shader program to store these uniform locations
	currProg->AddUniform("MVP");
	currProg->AddUniform("MV");
	currProg->AddUniform("MV_inverse_transpose");
	currProg->AddUniform("material.kAmbient");
	currProg->AddUniform("material.kDiffuse");
	currProg->AddUniform("material.kSpecular");
	currProg->AddUniform("material.kShininess");
	currProg->AddUniform("lightEnabled");
	currProg->AddUniform("numLights");
	currProg->AddUniform("lights[0].type");
	currProg->AddUniform("lights[0].position_cameraspace");
	currProg->AddUniform("lights[0].color");
	currProg->AddUniform("lights[0].power");
	currProg->AddUniform("lights[0].kC");
	currProg->AddUniform("lights[0].kL");
	currProg->AddUniform("lights[0].kQ");
	currProg->AddUniform("lights[0].spotDirection");
	currProg->AddUniform("lights[0].cosCutoff");
	currProg->AddUniform("lights[0].cosInner");
	currProg->AddUniform("lights[0].exponent");
	currProg->AddUniform("lights[1].type");
	currProg->AddUniform("lights[1].position_cameraspace");
	currProg->AddUniform("lights[1].color");
	currProg->AddUniform("lights[1].power");
	currProg->AddUniform("lights[1].kC");
	currProg->AddUniform("lights[1].kL");
	currProg->AddUniform("lights[1].kQ");
	currProg->AddUniform("lights[1].spotDirection");
	currProg->AddUniform("lights[1].cosCutoff");
	currProg->AddUniform("lights[1].cosInner");
	currProg->AddUniform("lights[1].exponent");
	currProg->AddUniform("colorTextureEnabled");
	currProg->AddUniform("colorTexture");
	currProg->AddUniform("textEnabled");
	currProg->AddUniform("textColor");
	
	// Tell the graphics manager to use the shader we just loaded
	GraphicsManager::GetInstance()->SetActiveShader("default");
	
	currProg->UpdateInt("numLights", 0);
	currProg->UpdateInt("textEnabled", 0);
	currProg->UpdateInt("lightEnabled", 0);

	// Create and attach the camera to the scene
	GraphicsManager::GetInstance()->AttachCamera(&camera);
	
	// Load all the meshes
	MeshBuilder::GetInstance()->GenerateAxes("reference");

	MeshBuilder::GetInstance()->GenerateText("text", 16, 16);
	MeshBuilder::GetInstance()->GetMesh("text")->textureID = LoadTGA("Image//Fonts//Calibri.tga");
	MeshBuilder::GetInstance()->GetMesh("text")->material.kAmbient.Set(1, 0, 0);

	MeshBuilder::GetInstance()->GenerateSphere("Red Bullet",Color(1,0,0),12,12,0.5f);
	MeshBuilder::GetInstance()->GenerateSphere("Green Bullet",Color(0,1,0),12,12,0.5f);
	MeshBuilder::GetInstance()->GenerateSphere("Blue Bullet",Color(0,0,1),12,12,0.5f);
	MeshBuilder::GetInstance()->GenerateSphere("Purple Bullet",Color(1,0,1),12,12,0.5f);
	MeshBuilder::GetInstance()->GenerateSphere("Yellow Bullet",Color(1,1,0),12,12,0.5f);

	MeshBuilder::GetInstance()->GenerateQuad("Tile Empty", Color(1, 1, 1), 1.0f);
	MeshBuilder::GetInstance()->GetMesh("Tile Empty")->textureID = LoadTGA("Image//Tile_Empty.tga");
	MeshBuilder::GetInstance()->GenerateQuad("Wall", Color(0, 0, 0), 1.0f);
	MeshBuilder::GetInstance()->GenerateQuad("Tile Occupied", Color(1, 0, 1), 1.0f);
	
	Character::tileSystem.LoadFile("TileMap//level0.csv", true);

	for (int i = 0; i < 5; i += 2) {
		Character* testChar = new Attacker();
		testChar->position.Set(Math::RandIntMinMax(0, Character::tileSystem.GetNumColumns() - 1), Math::RandIntMinMax(0, Character::tileSystem.GetNumRows() - 1));
		testChar->Occupy(testChar->position.y, testChar->position.x);
		testChar->teamNumber = 0;
	}

	for (int i = 0; i < 5; i += 2) {
		Character* testChar = new Guardian();
		testChar->position.Set(Math::RandIntMinMax(0, Character::tileSystem.GetNumColumns() - 1), Math::RandIntMinMax(0, Character::tileSystem.GetNumRows() - 1));
		testChar->Occupy(testChar->position.y, testChar->position.x);
		testChar->teamNumber =0;
	}

	for (int i = 0; i < 5; i += 2) {
		Character* testChar = new Support();
		testChar->position.Set(Math::RandIntMinMax(0, Character::tileSystem.GetNumColumns() - 1), Math::RandIntMinMax(0, Character::tileSystem.GetNumRows() - 1));
		testChar->Occupy(testChar->position.y, testChar->position.x);
		testChar->teamNumber = 0;
	}

	for (int i = 0; i < 5; i += 5) {
		Gunner* testChar = new Gunner();
		testChar->InitPos(Vector3(Math::RandIntMinMax(0, Character::tileSystem.GetNumColumns() - 1), Math::RandIntMinMax(0, Character::tileSystem.GetNumRows() - 1)));
		testChar->Occupy(testChar->position.y, testChar->position.x);
		testChar->teamNumber = 1;
	}

	for (int i = 0; i < 5; ++i) {
		Character* testChar = new Turret();
		testChar->teamNumber = 1;
		testChar->position.Set(Math::RandIntMinMax(0, Character::tileSystem.GetNumColumns() - 1), Math::RandIntMinMax(0, Character::tileSystem.GetNumRows() - 1));
		testChar->Occupy(testChar->position.y, testChar->position.x);
	}

	for (int i = 0; i < 5; ++i) {
		Character* testChar = new Brawler();
		testChar->teamNumber = 1;
		testChar->position.Set(Math::RandIntMinMax(0, Character::tileSystem.GetNumColumns() - 1), Math::RandIntMinMax(0, Character::tileSystem.GetNumRows() - 1));
		testChar->Occupy(testChar->position.y, testChar->position.x);
	}

	camera.position.Set(0, 0, 80);
	camera.width = 16.0f * Application::GetInstance().GetWindowAspectRatio();
	camera.height = 12.0f;

	frameRate = 0.0f;
	textMesh = MeshBuilder::GetInstance()->GenerateText("Consolas", 16, 16);
	textMesh->textureID = LoadTGA("Image//Fonts//Consolas.tga");

	spawnType = 1;
	spawnString = "Attacker";
}

void SceneAI::UpdateCamera(double _deltaTime) {
	float camSpeed = 10.0f;
	if (KeyboardController::GetInstance()->IsKeyDown('A')) {
		camera.position.x -= camSpeed * static_cast<float>(_deltaTime);
	}
	if (KeyboardController::GetInstance()->IsKeyDown('D')) {
		camera.position.x += camSpeed * static_cast<float>(_deltaTime);
	}
	if (KeyboardController::GetInstance()->IsKeyDown('W')) {
		camera.position.y += camSpeed * static_cast<float>(_deltaTime);
	}
	if (KeyboardController::GetInstance()->IsKeyDown('S')) {
		camera.position.y -= camSpeed * static_cast<float>(_deltaTime);
	}

	camera.height = 12.0f;
	camera.width = camera.height * Application::GetInstance().GetWindowAspectRatio();	

	float mapLeft = -0.5f;
	float mapRight = static_cast<float>(Character::tileSystem.GetNumColumns()) + mapLeft;
	float mapBottom = -0.5f;
	float mapTop = static_cast<float>(Character::tileSystem.GetNumRows()) + mapBottom;

	if (camera.GetRightBorder() > mapRight) {
		camera.position.x = mapRight - camera.width*0.5f;
	}
	if (camera.GetLeftBorder() < mapLeft) {
		camera.position.x = mapLeft + camera.width*0.5f;
	}
	if (camera.GetTopBorder() > mapTop) {
		camera.position.y = mapTop - camera.height*0.5f;
	}
	if (camera.GetBottomBorder() < mapBottom) {
		camera.position.y = mapBottom + camera.height*0.5f;
	}
}

void SceneAI::Update(double _deltaTime) {	
	frameRate = 1.0 / _deltaTime;

	// Update our entities
	EntityManager::GetInstance()->Update(_deltaTime);	
	GameManager::GetInstance()->Update(_deltaTime);

	UpdateCamera(_deltaTime);

	if (KeyboardController::GetInstance()->IsKeyDown(VK_F1)) {
		glEnable(GL_CULL_FACE);
	}		
	if (KeyboardController::GetInstance()->IsKeyDown(VK_F2)) {
		glDisable(GL_CULL_FACE);
	}		
	if(KeyboardController::GetInstance()->IsKeyDown(VK_F3)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}		
	if(KeyboardController::GetInstance()->IsKeyDown(VK_F4)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}		

	if (KeyboardController::GetInstance()->IsKeyDown('1')) {
		spawnType = 1;
		spawnString = "Attacker";
	}
	if (KeyboardController::GetInstance()->IsKeyDown('2')) {
		spawnType = 2;
		spawnString = "Guardian";
	}
	if (KeyboardController::GetInstance()->IsKeyDown('3')) {
		spawnType = 3;
		spawnString = "Support";
	}
	if (KeyboardController::GetInstance()->IsKeyDown('4')) {
		spawnType = 4;
		spawnString = "Gunner";
	}
	if (KeyboardController::GetInstance()->IsKeyDown('5')) {
		spawnType = 5;
		spawnString = "Turret";
	}
	if (KeyboardController::GetInstance()->IsKeyDown('6')) {
		spawnType = 6;
		spawnString = "Brawler";
	}


	if (KeyboardController::GetInstance()->IsKeyDown('F')) {
		switch (spawnType) {
		case 1: {
			Character* testChar = new Attacker();
			testChar->position.Set(Math::RandIntMinMax(0, Character::tileSystem.GetNumColumns() - 1), Math::RandIntMinMax(0, Character::tileSystem.GetNumRows() - 1));
			testChar->Occupy(testChar->position.y, testChar->position.x);
			testChar->teamNumber = 0;
		}
			break;
		case 2: {
			Character* testChar = new Guardian();
			testChar->position.Set(Math::RandIntMinMax(0, Character::tileSystem.GetNumColumns() - 1), Math::RandIntMinMax(0, Character::tileSystem.GetNumRows() - 1));
			testChar->Occupy(testChar->position.y, testChar->position.x);
			testChar->teamNumber = 0;
		}
			break;
		case 3: {
			Character* testChar = new Support();
			testChar->position.Set(Math::RandIntMinMax(0, Character::tileSystem.GetNumColumns() - 1), Math::RandIntMinMax(0, Character::tileSystem.GetNumRows() - 1));
			testChar->Occupy(testChar->position.y, testChar->position.x);
			testChar->teamNumber = 0;
		}
			break;
		case 4: {
			Gunner* testChar = new Gunner();
			testChar->InitPos(Vector3(Math::RandIntMinMax(0, Character::tileSystem.GetNumColumns() - 1), Math::RandIntMinMax(0, Character::tileSystem.GetNumRows() - 1)));
			testChar->Occupy(testChar->position.y, testChar->position.x);
			testChar->teamNumber = 1;
		}
			break;
		case 5: {
			Character* testChar = new Turret();
			testChar->teamNumber = 1;
			testChar->position.Set(Math::RandIntMinMax(0, Character::tileSystem.GetNumColumns() - 1), Math::RandIntMinMax(0, Character::tileSystem.GetNumRows() - 1));
			testChar->Occupy(testChar->position.y, testChar->position.x);
		}
			break;
		case 6: {
			Character* testChar = new Brawler();
			testChar->teamNumber = 1;
			testChar->position.Set(Math::RandIntMinMax(0, Character::tileSystem.GetNumColumns() - 1), Math::RandIntMinMax(0, Character::tileSystem.GetNumRows() - 1));
			testChar->Occupy(testChar->position.y, testChar->position.x);
		}
			break;
		default:
			break;
		}
	}
}

void SceneAI::RenderMap() {
	MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();	
	Mesh* tileEmpty = MeshBuilder::GetInstance()->GetMesh("Tile Empty");
	Mesh* wall = MeshBuilder::GetInstance()->GetMesh("Wall");
	Mesh* tileOccupied = MeshBuilder::GetInstance()->GetMesh("Tile Occupied");

	modelStack.PushMatrix();
		modelStack.Translate(0, 0, -10);
		for (int row = 0; row < Character::tileSystem.GetNumRows(); ++row) {
			modelStack.PushMatrix();
				for (int column = 0; column < Character::tileSystem.GetNumColumns(); ++column) {
					//No need to render what we can't see.
					if (row < camera.GetBottomBorder() -0.5f || row > camera.GetTopBorder() + 0.5f ||
						column < camera.GetLeftBorder() - 0.5f || column > camera.GetRightBorder() + 0.5f) {
						modelStack.Translate(1, 0, 0);
						continue;
					}

					int index = row * Character::tileSystem.GetNumColumns() + column;
					if (Character::tileSystem.GetTileMap()[index].occupant >= 0) {
						RenderHelper::RenderMesh(tileOccupied);
					} else if (Character::tileSystem.GetTileMap()[index].value == 0) {
						RenderHelper::RenderMesh(tileEmpty);
					} else {
						RenderHelper::RenderMesh(wall);
					}
					modelStack.Translate(1, 0, 0);
				}				
			modelStack.PopMatrix();
			modelStack.Translate(0, 1, 0);
		}
	modelStack.PopMatrix();
}

void SceneAI::RenderFrameRate() {
	MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();	
	modelStack.PushMatrix();
		modelStack.Translate(-1, 0.95, 0);
		modelStack.Scale(0.05f, 0.05f, 0.05f);
		RenderHelper::RenderText(textMesh, "FPS: " + to_string(static_cast<int>(frameRate)), Color(0, 1, 0));
	modelStack.PopMatrix();
}

void SceneAI::Render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GraphicsManager::GetInstance()->UpdateLightUniforms();

	//Setup 3D pipeline then render 3D
	//GraphicsManager::GetInstance()->SetPerspectiveProjection(45.0f, 4.0f / 3.0f, 0.1f, 1000.0f);
	GraphicsManager::GetInstance()->SetOrthographicProjection(-camera.width * 0.5f, camera.width * 0.5f, -camera.height * 0.5f, camera.height * 0.5f, -100, 100);
	GraphicsManager::GetInstance()->AttachCamera(&camera);
	EntityManager::GetInstance()->Render();

	//Render Map
	RenderMap();

	// Setup 2D pipeline then render 2D
	GraphicsManager::GetInstance()->SetOrthographicProjection(-1, 1, -1, 1, -1, 1);
	GraphicsManager::GetInstance()->DetachCamera();
	EntityManager::GetInstance()->RenderUI();
	RenderFrameRate();

	MS& modelstack = GraphicsManager::GetInstance()->GetModelStack();
	modelstack.PushMatrix();
		modelstack.Translate(-0.9f, 0.8f, 0);
		modelstack.Scale(0.05, 0.05, 0.1);
		string instructionString = "Press F to spawn: ";
		RenderHelper::RenderText(textMesh, instructionString + spawnString, Color(1, 0, 0));
	modelstack.PopMatrix();

	int i = 0;
	for (list<MessageLog*>::iterator it = MessageBoard::GetInstance()->messageQueue.begin(); it != MessageBoard::GetInstance()->messageQueue.end();++it)
	{
		modelstack.PushMatrix();
		modelstack.Translate(-1,-0.3-i*0.1,0);
		modelstack.Scale(0.025, 0.05, 0.1);
		RenderHelper::RenderText(textMesh,(*it)->GetLogString(),Color(0,1,0) );
		modelstack.PopMatrix();
		++i;
	}
}

void SceneAI::Exit() {
}