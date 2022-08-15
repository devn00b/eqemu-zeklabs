#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <iostream>
#include <iomanip>
#include "misc.h"

using namespace std;

vector<string> split(string data, string seps)
{
unsigned long next,offset=0;
vector<string> chunks;
string s;

	while ((next=data.find_first_of(seps,offset))!=string::npos) {
		s.assign(data,offset,next-offset);
		chunks.push_back(s);
		offset=next+1;
	}
	s.assign(data,offset,string::npos);
	chunks.push_back(s);

	return chunks;
}

vector<string> splitonstr(string data, string sep)
{
unsigned long next,offset=0;
vector<string> chunks;
string s;

	while ((next=data.find(sep,offset))!=string::npos) {
		s.assign(data,offset,next-offset);
		chunks.push_back(s);
		offset=next+sep.length();
	}
	s.assign(data,offset,string::npos);
	chunks.push_back(s);

	return chunks;
}

int SocketConnect(string host, unsigned short port, unsigned long utime)
{
struct sockaddr_in server;
struct hostent *hp=NULL;
int sock, retval, flags, size;
unsigned long addr;
fd_set readset,writeset;
struct timeval timeout,*tout;
 
	if (utime>=0) {
		timeout.tv_sec=utime/1000000;
		timeout.tv_usec=utime%1000000;
		tout=&timeout;
	} else
		tout=NULL;

	if ((hp = gethostbyname(host.c_str()))==NULL) {
		if ((long)(addr=inet_addr(host.c_str()))==-1) {
			return -1;
		} else {
			memcpy((char *)&server.sin_addr, &addr, sizeof(struct in_addr));
		}
	} else {
		memcpy((void *)&server.sin_addr,hp->h_addr_list[0], hp->h_length);
	}
	
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return -1;

	flags = fcntl(sock,F_GETFL,0);
	fcntl(sock, F_SETFL, flags|O_NONBLOCK);

	server.sin_port = htons((short)port);
	server.sin_family = AF_INET;

	if ((retval=connect(sock, (struct sockaddr *)&server, sizeof(struct sockaddr_in)))<0) {
		if (errno != EINPROGRESS) {
			close(sock);
			return -1;
		}

		FD_ZERO(&readset);
		FD_SET(sock,&readset);
		memcpy(&writeset,&readset,sizeof(fd_set));

		if ((retval=select(sock+1,&readset,&writeset,NULL,tout)) <=0) {
			if (!retval)
				errno=ETIMEDOUT;
			close(sock);
		
			return -1;
		} else {
			size=sizeof(int);
			if (!getsockopt(sock,SOL_SOCKET,SO_ERROR,(char *)&retval,(socklen_t *)&size) && retval) {
				close(sock);
				errno=retval;
				return -1;
			}
		}
	}

	fcntl(sock, F_SETFL, flags);

	return sock;
}

int SendTo(int fd, char *buffer, int length, sockaddr_in address)
{
	return sendto(fd,buffer,length,0,(struct sockaddr*)&address,sizeof(struct sockaddr_in));
}

int udp_listen(int port)
{
struct sockaddr_in address;
int sock;

/* Setup internet address information.  
This is used with the bind() call */
	memset((char *) &address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = htonl(INADDR_ANY);

	/* Setting up UDP port for new clients */
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		return -1;
	}

	if (bind(sock, (struct sockaddr *) &address, sizeof(address)) < 0) {
		close(sock);
		return -1;
	}

	fcntl(sock, F_SETFL, O_NONBLOCK);
	return sock;
}

void dump_message(unsigned char *buffer, int length, string leader)
{
int i;
	cout << leader;
	for(i=0;i<length && length<512;i++)
		printf(" %c ",isprint(buffer[i]) ? (char)buffer[i] : '.');
	cout << endl;

	cout << leader;
	for(i=0;i<length && length<512;i++)
		printf(" %02x",buffer[i]);
	cout << endl;
}

string pop_arg(string &s, string seps)
{
unsigned long offset;
string ret;
	if ((offset=s.find_first_of(seps))==string::npos) {
		ret=s;
		s="";
	} else {
		ret=s.substr(0,offset);
		s.erase(0,offset+1);
	}

	return ret;
}

string timestamp(time_t now)
{
	return string_from_time("[%Y%m%d.%H%M%S] ",now);
}

string string_from_time(string pattern, time_t now)
{
struct tm *now_tm;
char time_string[51];

	if (!now)
		time(&now);
	now_tm=localtime(&now);

	strftime(time_string,51,pattern.c_str(),now_tm);

	return string(time_string);
}


