

#ifndef UDPSOCKET_H_
#define UDPSOCKET_H_

#include <netinet/in.h>
#include <inttypes.h>
#include <strings.h>
#include <stdio.h>
#include <iostream>

using namespace std;
namespace npl{

class UDPSocket{
	struct sockaddr_in from;
	int socket_fd;
	size_t fsize;
	//int fsize;

public:
	UDPSocket(int port = 9999);
	int recv(char* buffer, int length);
	int sendTo(const string& msg, const string& ip, int port);
	int reply(const string& msg);
	void close();
	string fromAddr();

};

}
#endif /* UDPSOCKET_H_ */
