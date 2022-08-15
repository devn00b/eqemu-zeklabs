#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "eq_connection.h"
#include "eq_data.h"
#include "misc.h"

using namespace std;

void EQ_Connection::SendDisconnect()
{
eq_data eqdata;
	eqdata.sequenced=true;
	memset(eqdata.buffer,0,sizeof(eqdata.buffer));
	eqdata.buffer[1]=0x1d;
	*((unsigned long *)&(eqdata.buffer[2]))=htonl(Session);
	eqdata.length=6;
}

void EQ_Connection::Message(string from, string to, string message)
{
eq_data eqdata;
	eqdata.sequenced=false;
	memset(eqdata.buffer,0,sizeof(eqdata.buffer));
	eqdata.buffer[1]=0x03;
	eqdata.length=2;

	strcpy((char *)&eqdata.buffer[eqdata.length],to.c_str());
	eqdata.length+=to.length()+1;

	strcpy((char *)&eqdata.buffer[eqdata.length],from.c_str());
	eqdata.length+=from.length()+1;

	strcpy((char *)&eqdata.buffer[eqdata.length],message.c_str());
	eqdata.length+=message.length()+1;
	Out.push_back(eqdata);
}

int EQ_Connection::Process(eq_data data)
{
eq_data out_data;
int length;
unsigned char *buffer;
	if (Debug>1) {
		cout << timestamp() << "Got:" << endl;
		dump_message(data.buffer,data.length,timestamp());
		cout << timestamp() << "------------" << endl;
	}
	CheckRemoveCRC16(data);
	Decode(data);
	buffer=data.buffer;
	length=data.length;
	memset(out_data.buffer,0,512);
	if (buffer[0]==0x00) {
		switch(buffer[1]) {
			case OP_SessionInit: {
				if (!Session) {
					session_request *sesreq=(session_request *)(buffer+2);
					Session=ntohl(sesreq->Session);
					MaxLen=ntohs(sesreq->MaxLength);

					out_data.buffer[0]=0x00;
					out_data.buffer[1]=OP_SessionInitResp;
					session_response *sesres=(session_response *)(&out_data.buffer[2]);
					sesres->Session=htonl(Session);
					sesres->Key=htonl(Key);
					sesres->MaxLength=htons(MaxLen);
					out_data.length=sizeof(session_response)+2;

					Out.push_back(out_data);

					OnSessionRequest();
				} else {
					cout << timestamp() << "*** Duplicate session request. Dropping" << endl;
				}
			}
			break;
			case OP_SequenceAckResp: { //0x15
				OnSequenceAckResp(ntohs(*(unsigned short *)(buffer+2)));
			}
			break;
			case OP_Packet: { //0x09
				short Seq=htons(*(unsigned short *)(buffer+2));
				if (Seq>=NextInSeq) {
					if (Seq>NextInSeq) {
						cout << timestamp() << "*** Out of sync! Server=" << NextInSeq << ", Client=" << Seq << "!  Adjusting" << endl;
						NextInSeq=Seq;
					}
					NextInSeq++;
					Process_SubOP(buffer+4,length-4);
				} else if (Seq<NextInSeq) {
					cout << timestamp() << "*** Duplicate packet " << Seq << " should have been " << NextInSeq << endl;
				}
			}
			break;
			case OP_Disconnect: { //0x05
				OnSessionDisconnect();
			}
			break;
			case OP_KeepAlive: { //0x05
				OnKeepAlive();
			}
			break;
			default:
				cout << timestamp() << "*** Unknown opcode 0x" << hex << setw(2) << (int)buffer[1] << dec << endl;
				dump_message(data.buffer,data.length,timestamp());
				cout << timestamp() << "----------------" << endl;
				break;
		}
	} else {
		cout << timestamp() << "*** Got Message without leading 0x00" << endl;
		dump_message(data.buffer,data.length,timestamp());
		cout << timestamp() << "----------------" << endl;
	}

	OnPacket(data);

	return 1;
}

void EQ_Connection::Process_SubOP(unsigned char *buffer, int length)
{
eq_data out_data;
	memset(out_data.buffer,0,512);
	switch(buffer[0]) {
		case SUBOP_ClientLogin: { //0x001
			char *server, *name, *pass, *lang;
			server=(char *)(buffer+2);
			name=strchr(server,'.');
			*(name++)=0;
			pass=name+strlen(name)+1;
			lang=pass+strlen(pass)+1;
			*(lang+2)=0;

			Server=server;
			Name=name;
			Pass=pass;
			Lang=lang;

			// Do response
			out_data.buffer[1]=0x03;
			out_data.buffer[2]=0x04;
			out_data.buffer[3]=0x00;
			out_data.buffer[4]=0x15;
			out_data.buffer[5]=0x00;
			out_data.buffer[6]=0x00;
			out_data.buffer[7]=0x06; // Length;
			out_data.buffer[8]=0x00;
			out_data.buffer[9]=0x09;
			out_data.buffer[10]=0x00;
			out_data.buffer[11]=0x00;
			out_data.buffer[12]=0x01;
			out_data.buffer[13]=0x01;
			out_data.sequenced=true;
			out_data.length=14;

			Out.push_back(out_data);

			NextOutSeq++;
			OnClientLogin();
		}
		break;
		case SUBOP_ClientCommand: { //0x02
			out_data.buffer[1]=0x15;
			*(unsigned short *)&out_data.buffer[2]=htons(NextInSeq-1);
			out_data.sequenced=true;
			out_data.length=4;
			Out.push_back(out_data);

			OnClientCommand((char *)buffer+2);
		}
		break;
		default:
			out_data.buffer[1]=0x15;
			*(unsigned short *)&out_data.buffer[2]=htons(NextInSeq-1);
			out_data.sequenced=true;
			out_data.length=4;
			Out.push_back(out_data);
			cout << timestamp() << "*** Unknown sub opcode 0x" << hex << setw(2) << (int)buffer[0] << dec << endl;
			dump_message(buffer,length,timestamp());
			cout << timestamp() << "----------------" << endl;
		break;
	}
}

void EQ_Connection::Write(int eq_fd)
{
eq_data data;
eq_data out_data;
int offset=0;
	memset(data.buffer,0,512);
	memset(out_data.buffer,0,512);
	while (Out.size()) {
		if (Out[0].length>255) {
			cout << timestamp() << "*** Next message is too long (" << Out[0].length << "), skipping it!  Get this part fixed!" << endl;
			Out.erase(Out.begin());
			continue;
		}

		offset=0;
		if (Out[0].sequenced==false) {
			if (Out[0].length<=248 && Out.size()>1) {
				out_data.buffer[offset++]=0x00;
				out_data.buffer[offset++]=0x03;
				out_data.buffer[offset++]=0x04;
				out_data.buffer[offset++]=0x00;
				out_data.buffer[offset++]=0x15;
				out_data.buffer[offset++]=0x00;
				out_data.buffer[offset++]=(unsigned char)NextInSeq;
				out_data.buffer[offset++]=0x00;
				out_data.buffer[offset++]=0x00;
				out_data.buffer[offset++]=0x09;
				*(unsigned short *)&out_data.buffer[offset]=htons(NextOutSeq++);
				offset+=2;
				out_data.buffer[offset++]=0x00;
				out_data.buffer[offset++]=0x19;

				out_data.buffer[7]=6;

				while(Out.size() && (Out[0].length+out_data.buffer[7])<248) {
					data=Out[0];
					Out.erase(Out.begin());

					if (Debug>1)
						cout << timestamp() << "Out of queue (" << data.length << "):" << endl;
					if (data.length > 255 || data.length < 0) {
						cerr << "*** Bad length on outgoing EQ packet!" << endl;
						continue;
					}
					if (Debug>1)
						dump_message(data.buffer,data.length,timestamp());
					if (data.length==0)
						continue;

					if (offset<0 || offset>510) {
						cerr << "*** Bad size of EQ packet! offset=" << offset << endl;
						continue;
					}

					data.buffer[0]=data.length-1;
					memcpy(&out_data.buffer[offset],data.buffer,data.length);
					offset+=data.length;
					out_data.length=offset;

					out_data.buffer[7]+=data.length;
					if (Debug>1) {
						cout << timestamp() << "Current packet (" << out_data.length << "):" << endl;
						dump_message(out_data.buffer,offset,timestamp());
						cout << timestamp() << "----------------" << endl;
					}
				}

				out_data.length=offset;
			} else {
				out_data=Out[0];
				Out.erase(Out.begin());

				memmove(&out_data.buffer[3],out_data.buffer,out_data.length);
				out_data.buffer[0]=0x00;
				out_data.buffer[1]=0x09;
				*(unsigned short *)(&out_data.buffer[2])=htons(NextOutSeq++);
				out_data.length+=3;
			}
		} else {
			out_data=Out[0];
			Out.erase(Out.begin());
		}
		Encode(out_data);
		AddCRC16(out_data);
		if (Debug>1) {
			unsigned long ip=address.sin_addr.s_addr;
			cout << timestamp() << "Sending to: "
				<< (int)*(unsigned char *)&ip
				<< "." << (int)*((unsigned char *)&ip+1)
				<< "." << (int)*((unsigned char *)&ip+2)
				<< "." << (int)*((unsigned char *)&ip+3)
				<< "," << (int)ntohs(address.sin_port) << endl;

			dump_message(out_data.buffer,out_data.length,timestamp());
			cout << timestamp() << "-------------" << endl;
		}
		SendTo(eq_fd,(char *)out_data.buffer,out_data.length,address);

	}
}

void EQ_Connection::Encode(eq_data &data)
{
	//Do encoding
}

void EQ_Connection::Decode(eq_data &data)
{
	//Do decoding
}

void EQ_Connection::AddCRC16(eq_data &data)
{
	// Do real CRC here
	data.buffer[data.length++]=0x00;
	data.buffer[data.length++]=0x00;
}

int EQ_Connection::CheckRemoveCRC16(eq_data &data)
{
	// Do real CRC here
	data.length-=2;

	return 1;
}

void EQ_Connection::OnSessionRequest()
{
	if (Debug) {
		cout << timestamp() << "[" << Address << "] Got SessionRequest:" << ", Session=" << hex << setw(8) << setfill('0') << Session << ",  MaxLen=" << dec << MaxLen << ":  Setting key=" << hex << setw(8) << setfill('0') << Key << dec << endl;
	}
}

void EQ_Connection::OnSessionDisconnect()
{
	if (Debug) {
		cout << timestamp() << "Got SessionDisconnect" << endl;
	}
}

void EQ_Connection::OnKeepAlive()
{
	if (Debug>1) {
		cout << timestamp() << "[" << Address << "] Got KeepAlive" << endl;
	}
}

void EQ_Connection::OnSequenceAckResp(unsigned short seq)
{
	if (Debug>1) {
		cout << timestamp() << "Got ack on #" << seq << endl;
	}
}

void EQ_Connection::OnClientLogin()
{
	if (Debug) {
		cout << timestamp() << "[" << Address << "] Client logging in:" << " Server=" << Server << ", Name=" << Name << ", Pass=" << Pass << ", Lang=" << Lang << endl;
	}
}

void EQ_Connection::OnClientCommand(string command)
{
	if (Debug) {
		cout << timestamp() << "Command: " << command << endl;
	}
}

int EQ_Connection::Read(int eq_fd, sockaddr_in *from, eq_data &data)
{
int socklen;

	socklen=sizeof(sockaddr);
	data.length=recvfrom(eq_fd,data.buffer,512,0,(struct sockaddr*)from,(socklen_t *)&socklen);

	return data.length;
}

int EQ_Connection::Listen(unsigned short port)
{
	return udp_listen(port);
}
