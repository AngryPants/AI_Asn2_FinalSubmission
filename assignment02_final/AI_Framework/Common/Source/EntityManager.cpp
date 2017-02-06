#include "EntityManager.h"
#include "EntityBase.h"
#include "Message.h"

void EntityManager::Update(double _deltaTime)
{
	// Update all entities
	std::list<EntityBase*>::iterator it, end;
	end = entityList.end();
	for (it = entityList.begin(); it != end; ++it)
	{
		(*it)->Update(_deltaTime);
	}

	// Clean up entities that are done
	it = entityList.begin();
	while (it != end)
	{
		if ((*it)->IsDone())
		{
			// Delete if done
			delete *it;
			it = entityList.erase(it);
		}
		else
		{
			// Move on otherwise
			++it;
		}
	}
}

void EntityManager::Render()
{
	// Render all entities
	for (std::list<EntityBase*>::iterator it = entityList.begin(); it != entityList.end(); ++it)
	{
		(*it)->Render();
	}
}

void EntityManager::RenderUI()
{
	// Render all entities
	std::list<EntityBase*>::iterator it, end;
	end = entityList.end();
	for (it = entityList.begin(); it != end; ++it)
	{
		(*it)->RenderUI();
	}
}

void EntityManager::AddEntity(EntityBase* _newEntity, PassKey<EntityBase> _key)
{
	entityList.push_back(_newEntity);
}

EntityManager::EntityManager()
{
}

EntityManager::~EntityManager()
{
}

EntityBase* EntityManager::GetEntityByID(int _id) {
	for (list<EntityBase*>::iterator listIter = entityList.begin(); listIter != entityList.end(); ++listIter) {
		EntityBase* entityPtr = *listIter;
		if (entityPtr->GetID() == _id) {
			return entityPtr;
		}
	}
	return nullptr;
}

void EntityManager::BroadcastMessage(const Message* _messagePtr) {
	for (list<EntityBase*>::iterator listIter = entityList.begin(); listIter != entityList.end(); ++listIter) {
		EntityBase* entityPtr = *listIter;
		if (entityPtr->GetID() != _messagePtr->senderID) {
			entityPtr->ReceiveMessage(_messagePtr);
		}		
	}
}

bool EntityManager::SendMessage(int _destinationID, const Message* _messagePtr) {
	for (list<EntityBase*>::iterator listIter = entityList.begin(); listIter != entityList.end(); ++listIter) {
		EntityBase* entityPtr = *listIter;
		if (entityPtr->GetID() == _destinationID) {
			entityPtr->ReceiveMessage(_messagePtr);
			return true;
		}
	}

	return false;
}