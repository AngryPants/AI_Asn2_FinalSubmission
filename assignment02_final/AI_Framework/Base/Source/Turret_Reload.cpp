#include "Turret_Reload.h"
#include "Turret.h"
#include "MessageBoard.h"

Turret_Reload::Turret_Reload(Turret& owner) : Turret_State("Reload", owner)
{
	timeToReload = 0.0f;
}

Turret_Reload::~Turret_Reload()
{
}

void Turret_Reload::Update(double dt)
{
	if (GetOwner().GetNumBullet() >= GetOwner().GetMaxBullet())
	{
		timeToReload = 0.0f;
		GetOwner().SetNextState(GetOwner().GetPreviousState());
		return;
	}

	timeToReload -= dt;
	if (timeToReload <= 0.0f)
	{
		GetOwner().Reload();
		timeToReload = 0.2f;
	}
}

void Turret_Reload::ReceiveMessage(const Message* messagePtr)
{
	MessageBoard::GetInstance()->UpdateLogReceiver(messagePtr->messageLogID, GetOwner().name);
}