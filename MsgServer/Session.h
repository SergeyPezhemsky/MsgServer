#pragma once

struct Session
{
	int m_ID;
	string m_Name;
	std::chrono::system_clock::time_point time;

	queue<Message> m_Messages;
	vector<Message> m_AllMessages;
	CCriticalSection m_CS;

	Session(int ID, string Name)
		:m_ID(ID), m_Name(Name)
	{
		time = std::chrono::system_clock::now();
	}

	void Add(Message& m)
	{
		CSingleLock sl(&m_CS, TRUE);
		m_AllMessages.push_back(m);
		m_Messages.push(m);
	}

	void Send(CSocket& s)
	{
		CSingleLock sl(&m_CS, TRUE);
		if (m_Messages.empty())
		{
			Message::Send(s, m_ID, M_BROKER, M_NODATA);
		}
		else
		{
			m_Messages.front().Send(s);
			m_Messages.pop();
		}
	}

	void SendAll(CSocket& s)
	{
		CSingleLock sl(&m_CS, TRUE);
		if (m_AllMessages.empty())
		{
			Message::Send(s, m_ID, M_BROKER, M_NODATA);
		}
		else
		{
			Message message = m_AllMessages.at(0);
			message.m_Data = "";
			message.m_Header.m_Size = 0;
			for (Message m : m_AllMessages) {
				message.m_Data += m.m_Data + ":::::::";
				message.m_Header.m_Size += m.m_Header.m_Size + 7;
			}
			message.Send(s);
		}
	}

};