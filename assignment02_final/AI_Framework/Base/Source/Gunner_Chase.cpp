#include "Gunner_Chase.h"
#include "Gunner.h"

//include messages
#include "EntityManager.h"

Gunner_Chase::Gunner_Chase(Gunner& owner) :State("Gunner_Chase")
{
	this->owner = &owner;
	f_distanceToTarget = FLT_MAX;

}

Gunner_Chase::~Gunner_Chase()
{
}

void Gunner_Chase::ReceiveMessage(const Message* message)
{
	//const CharacterInfo* ch = dynamic_cast<const CharacterInfo*>(message);
	//if (ch)
	//{
	//	if (ch->isAlive == false && ch->inParty != owner->InParty)
	//	{
	//		if (f_closestEnemyDistance * f_closestEnemyDistance > (ch->position - owner->position).LengthSquared)
	//		{
	//			//i_targetID = ch->senderID;
	//			f_closestEnemyDistance = (ch->position - owner->position).Length();
	//		}

	//	}
	//}
	return;
}
void Gunner_Chase::Reset()
{
	f_distanceToTarget = FLT_MAX;
}

bool Gunner_Chase::Think()
{
	if (f_distanceToTarget < owner->cf_AttackRange)
	{
		Reset();
		owner->SetNextState("Gunner_Attack");
		return false;
	}
	if (f_distanceToTarget > owner->cf_detectionRange)
	{
		Reset();
		owner->SetNextState("Gunner_Patrol");
		return false;
	}
	return true;
}

void Gunner_Chase::Act(double dt)
{
	Vector3 direction = (targetPos - Vector3(owner->position.x, owner->position.y)).Normalize();
	float rotation = Math::RadianToDegree(atan2(direction.y, direction.x));
	owner->rotation = rotation;
	owner->Move(dt,targetPos.y,targetPos.x-1,owner->f_movSpeed);
	//owner->SetPosition(owner->GetPosition() + direction * owner->f_movSpeed * dt);
}

void Gunner_Chase::Update(double dt)
{
	Character* ch = dynamic_cast<Character*>(EntityManager::GetInstance()->GetEntityByID(owner->i_targetID));
	if (ch != nullptr && ch->IsAlive())
	{
		f_distanceToTarget = (ch->position - owner->position).Length();
	}
	else
	{
		f_distanceToTarget = FLT_MAX;
	}
	if (Think() == true)
	{
		targetPos = ch->position.ChangeToVec3();
		Act(dt);
	}
}