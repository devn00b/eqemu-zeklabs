/*  EQEMu:  Everquest Server Emulator
    Copyright (C) 2001-2002  EQEMu Development Team (http://eqemu.org)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; version 2 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY except by those people which sell it, which
	are required to give you total support for your newly bought product;
	without even the implied warranty of MERCHANTABILITY or FITNESS FOR
	A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <string.h>
using namespace std;
#ifndef WIN32
#include "unix.h"
#include <ctype.h>

void Sleep(unsigned int x) {
	if (x > 0)
		usleep(x*1000);
}

char* strupr(char* tmp) {
	int l = strlen(tmp);
	for (int x = 0; x < l; x++) {
		tmp[x] = toupper(tmp[x]);
	}
	return tmp;
}

char* strlwr(char* tmp) {
	int l = strlen(tmp);
	for (int x = 0; x < l; x++) {
		tmp[x] = tolower(tmp[x]);
	}
	return tmp;
}

string dns_lookup(char* host_name, int ipv)
{
    struct addrinfo hints, *res, *p;
    int status, ai_family;
    char ip_address[INET6_ADDRSTRLEN];
 
    ai_family = ipv==6 ? AF_INET6 : AF_INET; //v4 vs v6?
    ai_family = ipv==0 ? AF_UNSPEC : ai_family; // AF_UNSPEC (any), or chosen
    memset(&hints, 0, sizeof hints);
    hints.ai_family = ai_family; 
    hints.ai_socktype = SOCK_STREAM;
 
    if ((status = getaddrinfo(host_name, NULL, &hints, &res)) != 0) {
        cout << "getaddrinfo: "<< gai_strerror(status) << " for " << host_name << endl;
        return std::string(host_name);
    }
 
    cout << "DNS Lookup: " << host_name << " ipv:" << ipv << endl;
    void *addr = NULL;
    for(p = res;p != NULL; p = p->ai_next) {
        if (p->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
        } else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
		}
        inet_ntop(p->ai_family, addr, ip_address, sizeof ip_address);
		cout << "IP Address Resolved: " << ip_address << " ipv:" << ipv << endl;
		freeaddrinfo(res);
		break;
	}
 
	if ( addr )
	{
        return ip_address;
    }
	
	cout << "Could not resolve, returning: " << host_name << " ipv:" << ipv << endl;

    return host_name;
}

#else
	 int joe = 1;
#endif /* !WIN32 */