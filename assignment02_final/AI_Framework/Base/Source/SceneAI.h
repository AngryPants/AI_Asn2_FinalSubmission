#ifndef SCENE_AI_H
#define SCENE_AI_H

#include "Scene.h"
#include "Camera2D.h"
#include "Mtx44.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "ShaderProgram.h"
#include "TileSystem.h"
#include "Character.h"
#include <string>

using std::string;

class Attacker;
class TestChar;
class Gunner;
class Guardian;

class SceneAI : public Scene {

private:
	//Variable(s)
	Camera2D camera;
	ShaderProgram* currProg;
	double frameRate;
	Mesh* textMesh;

	int spawnType;
	string spawnString;

	//Updating
	void UpdateCamera(double _deltaTime);

	//Rendering
	void RenderMap();
	void RenderFrameRate();	

public:
	//Constructor(s) & Destructor
	SceneAI();
	virtual ~SceneAI();

	//Virtual Function(s)
	virtual void Init();
	virtual void Update(double _deltaTime);
	virtual void Render();
	virtual void Exit();

};

#endif