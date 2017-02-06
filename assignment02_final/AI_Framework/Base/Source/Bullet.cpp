#include "Bullet.h"
#include "Mesh.h"
#include "GameManager.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"

Bullet::Bullet() {
	GameManager::GetInstance()->AddBullet(this, {});
	damage = 0;
	teamNumber = -1;
	lifeTime = 5.0;
	isActive = false;
	mesh = nullptr;
	radius = 0.2f;
	speed = 5.0f;
}

Bullet::~Bullet() {
}

void Bullet::Update(double deltaTime) {
	if (isActive == false) {
		return;
	}
	if (lifeTime > 0.0f) {
		lifeTime -= deltaTime;	
	} else {
		isActive = false;
	}
	if (direction.LengthSquared() > Math::EPSILON) {
		position += speed * direction.Normalize() * deltaTime;
	}
}

void Bullet::Render() {
	if (mesh == nullptr || isActive == false) {
		return;
	}
	MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
	modelStack.PushMatrix();
		modelStack.Translate(position.x, position.y, 0);
		modelStack.Scale(radius * 2.0f, radius * 2.0f, radius * 2.0f);
		RenderHelper::RenderMesh(mesh);
	modelStack.PopMatrix();
}

void Bullet::RenderUI() {
}