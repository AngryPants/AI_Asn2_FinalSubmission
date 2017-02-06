#include "TB_Attacker.h"
#include "MeshBuilder.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"
#include "LoadTGA.h"

TB_Attacker::TB_Attacker() : TB_Character("Attacker") {
	//Init Meshes
	if (!MeshBuilder::GetInstance()->HasMesh("Attacker")) {
		MeshBuilder::GetInstance()->GenerateQuad("Attacker", Color(1, 0, 0), 1.0f);
		MeshBuilder::GetInstance()->GetMesh("Attacker")->textureID = LoadTGA("Image//Attacker.tga");
	}
	mesh = MeshBuilder::GetInstance()->GetMesh("Attacker");
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

	maxBullets = 4;
	numBullets = maxBullets;



}

TB_Attacker::~TB_Attacker() {
}

//Interface Function(s)
int TB_Attacker::GetType() const {
	return CharacterType::ATTACKER;
}

void TB_Attacker::SetNextState(const string& _nextState) {
	//Do Stuff.
}

void TB_Attacker::HandleMessage() {
	//Do Stuff.
}

void TB_Attacker::Update(double _deltaTime) {
	while (path.empty()) {
		int startRow = static_cast<int>(position.y + 0.5f);
		int startColumn = static_cast<int>(position.x + 0.5f);
		int endRow = Math::RandIntMinMax(0, tileSystem.GetNumRows() - 1);
		int endColumn = Math::RandIntMinMax(0, tileSystem.GetNumColumns() - 1);

		//cout << "Start Row: " << startRow << endl; cout << "Start Column: " << startColumn << endl;
		//cout << "End Row: " << endRow << endl; cout << "End Column: " << endColumn << endl;
		path = searcher.Search(&tileSystem, &influenceTable, &detectedIDs, GetID(), startRow, startColumn, endRow, endColumn);
		/*if (path.empty()) {
			cout << "Search Failed." << endl;
			cout << "ID: " << GetID() << endl;
		}*/

		/*cout << "Printing Path:" << endl;
		for (list<std::pair<int, int> >::iterator iter = path.begin(); iter != path.end(); ++iter) {			
			cout << iter->first << ", " << iter->second << endl;
		}
		cout << "End of Path" << endl;*/
	}

	Move(_deltaTime, path);
}

void TB_Attacker::Render() {
	MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
	modelStack.PushMatrix();
		modelStack.Translate(position.x, position.y, 10);
		//Render the main body.
		modelStack.PushMatrix();
			modelStack.Rotate(rotation, 0, 0, 1);
			RenderHelper::RenderMesh(mesh);
		modelStack.PopMatrix();
	modelStack.PopMatrix();
}

void TB_Attacker::RenderUI() {
}