#ifndef	MESSAGE_LOG_H
#define MESSAGE_LOG_H

#include <string>
#include <vector>

using std::string;
using std::vector;

class MessageLog
{
public:

	string messageName;
	string senderName;

	int logID;
	vector<string> receiverTypes;

	MessageLog(){};

	MessageLog(string messageName, string senderName)
	{
		this->messageName = messageName;
		this->senderName = senderName;
	};

	string GetLogString(){
		string temp = senderName + " sent a " + messageName + " message to ";
		if (receiverTypes.empty() == true)
		{
			temp += "everybody";
		}
		else
		{
			for (vector<string>::iterator it = receiverTypes.begin(); it != receiverTypes.end(); ++it)
			{
				temp += (*it) + ", ";
			}
		}
		return temp;
	};

	~MessageLog(){};

private:

protected:


};

#endif