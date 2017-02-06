#include "MessageBoard.h"

MessageBoard::MessageBoard()
{
	i_queueSize = 5;
	for (int i = 1; i <= i_queueSize; ++i)
	{
		freeID.push_back(i);
	}
}

MessageBoard::~MessageBoard()
{
}

int MessageBoard::AddMessageLog(MessageLog* messageLog)
{

	if (messageQueue.empty() == false)
	{
		if (messageLog->messageName == messageQueue.front()->messageName && messageLog->senderName == messageQueue.front()->senderName)
		{
			return -1;
		}
		if (messageQueue.size() > 1)
		{
			list<MessageLog*>::iterator it = messageQueue.begin();
			std::next(it);
			if (messageLog->messageName == (*it)->messageName && messageLog->senderName == (*it)->senderName)
			{
			return -1;
			}
		}
	}
	
	
	if (messageQueue.size() >= i_queueSize)
	{
		freeID.push_back(messageQueue.front()->logID);
		delete messageQueue.front();
		messageQueue.erase(messageQueue.begin());
	}
	int temp = GetFreeID();
	messageLog->logID = temp;
	messageQueue.push_back(messageLog);
	return temp;

}

int MessageBoard::AddMessageLog(string messageName, string senderName)
{
	MessageLog* newLog = new MessageLog(messageName, senderName);
	return AddMessageLog(newLog);
	
}

string MessageBoard::GetMessageLog(int index)
{
	return 0;
}

int MessageBoard::GetFreeID()
{
	if (freeID.empty())
	{
		return -1; 
	}

	int temp = freeID.front();
	freeID.erase(freeID.begin());
	return temp;
	//return 0;
}

void MessageBoard::UpdateLogReceiver(int logid, string name)
{
	if (logid == -1)
	{
		return;
	}
	for (list<MessageLog*>::iterator it = messageQueue.begin(); it != messageQueue.end(); ++it)
	{
		if ((*it)->logID == logid)
		{
			bool added = false;
			for (vector<string>::iterator it2 = (*it)->receiverTypes.begin(); it2 != (*it)->receiverTypes.end(); ++it2)
			{
				if ((*it2) == name)
				{
					added = true;
				}
			}
			if (added == false)
			{
				(*it)->receiverTypes.push_back(name);
			}
		}
	}
}