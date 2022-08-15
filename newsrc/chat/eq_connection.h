#ifndef _EQ_CONNECTION_H

#define _EQ_CONNECTION_H

#include <string>
#include <vector>
#include <iostream>
#include <netinet/in.h>
#include "eq_data.h"

using namespace std;

class EQ_Connection {
	protected:
		int NextInSeq;
		int NextOutSeq;
		string Server;
		string Name;
		string Pass;
		string Lang;
		sockaddr_in address;
		unsigned long Session, Key;
		unsigned short MaxLen;
		vector<eq_data> Out;
		vector<eq_data> In;
		int Debug;
	public:
		string Address;
		inline EQ_Connection() {  Reset(); }
		inline EQ_Connection(sockaddr_in from) { Reset(); SetAddress(from); }
		virtual ~EQ_Connection() { }
		inline void Reset() { Session=0; Key=0; MaxLen=0; NextInSeq=0; NextOutSeq=0; Debug=0; }
		inline void SetDebug(int level) { Debug=level; }
		inline void SetAddress(sockaddr_in from) { memcpy(&address,&from,sizeof(sockaddr_in)); }
		inline bool HasOutgoingData() { return Out.size() > 0; }
		void Write(int fd);
		int Process(eq_data data);
		void Process_SubOP(unsigned char *data, int length);
		void Send(eq_data);
		int EQ_GetPacket(eq_data &out_data);
		void Encode(eq_data &data);
		void Decode(eq_data &data);
		void AddCRC16(eq_data &data);
		int CheckRemoveCRC16(eq_data &data);
		void Message(string from, string to, string message);
		void SendDisconnect();

		static int Read(int eq_fd, sockaddr_in *from, eq_data &data);
		static int Listen(unsigned short port);

		virtual void OnSessionRequest();
		virtual void OnSessionDisconnect();
		virtual void OnSequenceAckResp(unsigned short seq);
		virtual void OnClientLogin();
		virtual void OnKeepAlive();
		virtual void OnClientCommand(string command);
		virtual void OnPacket(eq_data data)=0;
};

#endif
