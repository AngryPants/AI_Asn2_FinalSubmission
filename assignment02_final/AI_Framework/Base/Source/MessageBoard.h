#ifndef	MESSAGE_BOARD_H
#define MESSAGE_BOARD_H

#include "MessageLog.h"
#include "SingletonTemplate.h"
#include <list>

using std::list;

class MessageBoard:public Singleton<MessageBoard>
{
	friend Singleton<MessageBoard>;
public:

	list<MessageLog*> messageQueue;// stores the message logs to be displayed.
	int i_queueSize; // store the max size of the queue

	int AddMessageLog(MessageLog* messageLog);
	int AddMessageLog(string messageName, string senderName); // more parameters to be added as message logs member vairiables increases
	
	string GetMessageLog(int index);
	void UpdateLogReceiver(int ID,string name);


	int GetFreeID();

	list<int> freeID;

private:

protected:
	MessageBoard();
	virtual ~MessageBoard();

};

#endif