#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include "SingletonTemplate.h"
#include "PassKey.h"
#include <list>
#include <vector>
#include <type_traits>
#include <iostream>

using namespace std;

class EntityBase;
struct Message;

class EntityManager : public Singleton<EntityManager>
{
	friend Singleton<EntityManager>;
public:
	void Update(double _deltaTime);
	void Render();
	void RenderUI();

	void AddEntity(EntityBase* _newEntity, PassKey<EntityBase> _key);
	void BroadcastMessage(const Message* _messagePtr);
	bool SendMessage(int _destinationID, const Message* _messagePtr);
	EntityBase* GetEntityByID(int _id);

private:
	EntityManager();
	virtual ~EntityManager();

	std::list<EntityBase*> entityList;
};

#endif // ENTITY_MANAGER_H