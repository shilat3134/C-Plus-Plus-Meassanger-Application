

#include "UDPSocket.h"
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>

using namespace npl;

UDPSocket::UDPSocket(int port){
	/**
	 * int socket(int domain, int type, int protocol);
	 * creates a UDP socket
	 * AF_INET - IPv4 Internet protocols
	 * SOCK_DGRAM - UDP
	 * 0 - default protocol type fo UDP
	 */
	socket_fd = socket (AF_INET, SOCK_DGRAM, 0);
	if(socket_fd<0)
		perror("Socket");

	struct sockaddr_in s_in;
	bzero((char*)&s_in,sizeof(s_in));

	s_in.sin_family = (short)AF_INET;
	s_in.sin_addr.s_addr=htonl(INADDR_ANY);
	s_in.sin_port = htons(port);
	if (bind(socket_fd, (struct sockaddr *)&s_in, sizeof(s_in))<0)
	 perror("Bind");


	// clear the s_in struct

	//sets the sin address

	//in case a port is given, bind the socket on the specified address

}

int UDPSocket::recv(char* buffer, int length){
	//printf("UDP server receive ...\n");
	bzero((char*)&from,sizeof(from));
	fsize = sizeof from;
	int n=recvfrom(socket_fd,buffer,100,0,(struct sockaddr*)&from,(socklen_t*)&fsize);
	//ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
	//					struct sockaddr *src_addr, socklen_t *addrlen);

	char *ip=inet_ntoa(from.sin_addr);
	if(n<=0)
    	perror("RecvFrom");
    return n;
}

int UDPSocket::sendTo(const string& msg,const string& ip, int port){
	struct sockaddr_in s_to;
	bzero((char*)&s_to,sizeof(s_to));
	s_to.sin_family=(short)AF_INET;
	s_to.sin_addr.s_addr=inet_addr(ip.data());
	s_to.sin_port=htons(port);

    if(sendto(socket_fd, msg.data(), msg.length(), 0,(struct sockaddr*)&s_to, sizeof(s_to))<=0)
    	perror("SendTo");
	return -1;
}

int UDPSocket::reply(const string& msg){
//	char ip[15]={'1','9','2','.','1','6','8','.','7','6','.','1','6','5','\0'};
//		int port=3344;
//		bzero((char*)&from,sizeof(from));
//			from.sin_family=(short)AF_INET;
//			from.sin_addr.s_addr=inet_addr(ip);
//			from.sin_port=htons(port);
	char *ip=inet_ntoa(from.sin_addr);
		cout<<"InReply "<<ip<<endl;
	if(sendto(socket_fd, msg.data(), msg.length(), 0,(struct sockaddr*)&from,sizeof(from))<=0)
	    	perror("Replyshelanu");
	return -1;
}

void UDPSocket::close(){
	cout<<"closing udp socket"<<endl;
	shutdown(socket_fd,SHUT_RDWR);
	::close(socket_fd);
}

string UDPSocket::fromAddr(){
	return inet_ntoa(from.sin_addr);
}



