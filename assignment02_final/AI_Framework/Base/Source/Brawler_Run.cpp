#include "Brawler_Run.h"
#include "Brawler.h"
#include "CharacterInfo.h"
#include "MessageBoard.h"

Brawler_Run::Brawler_Run(Brawler& owner) : Brawler_State("Run", owner)
{
	canRun = false;
}

Brawler_Run::~Brawler_Run()
{
}

bool Brawler_Run::Think()
{
	if (GetOwner().GetHealthPercentage() > 50.f)
	{
		GetOwner().SetNextState("Move");
		return false;
	}
	else if (!canRun)
	{
		GetOwner().SetNextState("Move");
		return false;
	}
	return true;
}

void Brawler_Run::Act(double dt)
{
	GetOwner().Move(dt, static_cast<int>(GetOwner().position.y + 1.f), static_cast<int>(GetOwner().position.x));
}

void Brawler_Run::Update(double dt)
{
}

void Brawler_Run::ReceiveMessage(const Message* messagePtr)
{
	const CharacterInfo* charInfo = dynamic_cast<const CharacterInfo*>(messagePtr);

	if (charInfo != nullptr)
	{
		if (!canRun)
		{
			canRun = true;
			destination = charInfo->position;
			MessageBoard::GetInstance()->UpdateLogReceiver(messagePtr->messageLogID, GetOwner().name);
		}
		else
		{
			if ((destination - GetOwner().position).LengthSquared() > (charInfo->position - GetOwner().position).LengthSquared()) {
				destination = charInfo->position;
				MessageBoard::GetInstance()->UpdateLogReceiver(messagePtr->messageLogID, GetOwner().name);
			}
		}
	}
}