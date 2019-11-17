

#ifndef TCPSOCKET_H_
#define TCPSOCKET_H_

#include <netinet/in.h>
#include <inttypes.h>
#include <strings.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

namespace npl{

class TCPSocket{
	struct sockaddr_in serverAddr;
	struct sockaddr_in peerAddr;
	int socket_fd;
	int connectSock;


private:
	/**
	 * private constructor to create a secondary server socket to communicate with a remote peer
	 */
	TCPSocket(int connected_sock,struct sockaddr_in serverAddr,struct sockaddr_in peerAddr);

public:
	/**
	 * Constructor create a TCP server socket
	 */
	TCPSocket(int port);

	/**
	 * Constructor creates TCP client socket
	 */
	TCPSocket(const string& peerIp, int port);

	/**
	 * Perform listen and accept on server socket
	 */
	TCPSocket* listenAndAccept();

	/**
	 * Read from socket into the given buffer up to the buffer given length.
	 * return the number of bytes read
	 */
	int recv(char* buffer, int length);

	/**
	 * send the given buffer to the socket
	 */
	int send(const string& msg);

	int send(const char *msg, int length);

	/**
	 * close the socket and free all resources
	 */
	void close();

	/**
	 * return the address of the connected peer
	 */
	string fromAddr();
	string destIpAndPort();
	int getSocket_FD(){
		return socket_fd;
	}


};
}

#endif
