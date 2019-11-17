

#include "TCPSocket.h"
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <sstream>

using namespace std;
using namespace npl;

TCPSocket::TCPSocket(int connected_sock,struct sockaddr_in serverAddr,struct sockaddr_in peerAddr){
	this->serverAddr =  serverAddr;
	this->peerAddr =  peerAddr;
	socket_fd = connected_sock;
	//connectSock=connected_sock;
}

/* to be used by a server */
TCPSocket::TCPSocket(int port){
	/**
	 * int socket(int domain, int type, int protocol);
	 * creates a TCP socket
	 * AF_INET - IPv4 Internet protocols
	 * SOCK_STREAM - TCP
	 * 0 - default protocol type
	 */
	socket_fd = socket (AF_INET, SOCK_STREAM, 0);
	if(socket_fd<0)
		perror("Socket");

	// clear the s_in struct
	bzero((char *) &serverAddr, sizeof(serverAddr));  /* They say you must do this    */
	serverAddr.sin_family = (short)AF_INET;
	serverAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	serverAddr.sin_port = htons(port);
	//bind the socket on the specified address
	printf("TCP server binding...\n");
	
	//TODO: binding
	if(bind(socket_fd,(struct sockaddr*)&serverAddr,sizeof(serverAddr))<0)
		perror("Bind");
}

/* to be used by a client */
TCPSocket::TCPSocket(const string &peerIp, int port){
	cout<<"openning new client socket"<<endl;

	/**
	 * int socket(int domain, int type, int protocol);
	 * creates a TCP socket
	 * AF_INET - IPv4 Internet protocols
	 * SOCK_STREAM - TCP
	 * 0 - default protocol type
	 */
	socket_fd = socket (AF_INET, SOCK_STREAM, 0);

	// clear the s_in struct
	bzero((char *) &peerAddr, sizeof(peerAddr));  /* They say you must do this    */

	//sets the sin address
	peerAddr.sin_family = (short)AF_INET;
	peerAddr.sin_addr.s_addr = inet_addr(peerIp.data());
	peerAddr.sin_port = htons((u_short)port);

	if (connect(socket_fd, (struct sockaddr *)&peerAddr, sizeof(peerAddr)) < 0)
	{
		perror ("Error establishing communications");
		::close(socket_fd);
	}
}


TCPSocket* TCPSocket::listenAndAccept(){
	//TODO:
	listen(socket_fd,1);
	unsigned int len = sizeof(serverAddr);
	connectSock = accept(socket_fd,(struct sockaddr*)&peerAddr,&len);
	 TCPSocket *g=new TCPSocket(connectSock,serverAddr,peerAddr);
	    return g;
}


int TCPSocket::recv(char* buffer, int length){
	//TODO:
     //read(connectSock,buffer,length);
	int n=read(socket_fd,buffer,length);
	return n;
}


int TCPSocket::send(const string& msg){

	//TODO:
  // write(connectSock,msg.data(),sizeof(msg));msg.length()
	write(socket_fd,msg.data(),msg.length());
}

int TCPSocket::send(const char *msg, int length){

	//TODO:
  // write(connectSock,msg.data(),sizeof(msg));msg.length()
	write(socket_fd,msg, length);
}


void TCPSocket::close(){
	cout<<"Closing tcp socket"<<endl;
	shutdown(socket_fd,SHUT_RDWR);
	::close(socket_fd);
}


string TCPSocket::fromAddr(){
	return inet_ntoa(peerAddr.sin_addr);
}

string TCPSocket::destIpAndPort(){
    stringstream s;
    s << fromAddr() << ":" << peerAddr.sin_port;
    return s.str();





	/*string s_ip=inet_ntoa(peerAddr.sin_addr);
    int port=peerAddr.sin_port;
    char * p =new char[5];
    p[4]='\0';
    for(int i=0;i<4;i++){
    	p[i]=port%10+'0';
    	port=port/10;
    }
   string s_port(p);
	return s_ip+":"+s_port;*/
}


