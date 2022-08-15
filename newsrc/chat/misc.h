#ifndef _MISC_H

#define _MISC_H

#include <string>
#include <vector>
#include <sys/types.h>
#include <netinet/in.h>

using namespace std;

vector<string> split(string data, string seps=" ");
vector<string> splitonstr(string data, string sep);
int SocketConnect(string host, unsigned short port, unsigned long utime=50000000);
int SendTo(int fd, char *buffer, int length, sockaddr_in address);
int udp_listen(int port);
void dump_message(unsigned char *buffer, int length, string leader="");
string pop_arg(string &s, string seps=" ");
string string_from_time(string pattern, time_t now=0);
string timestamp(time_t now=0);

template <class InputIterator>
string join(InputIterator first, InputIterator last, char c=',')
{
ostringstream s;
InputIterator temp;
	for(temp=first;temp!=last;temp++) {
		if (temp!=first)
			s << c;
		s << *temp;
	}

	return s.str();
}

#endif
