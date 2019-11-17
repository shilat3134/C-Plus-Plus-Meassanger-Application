
#include "TCPMessengerServer.h"
#include "TCPMessengerProtocol.h"


TCPMessengerServer::TCPMessengerServer(){
	dispatcher = new MsnDispatcher();
	tcpServerSocket = new TCPSocket(MSNGR_PORT);
	running = false;
	start();
}

void TCPMessengerServer::run(){
	running = true;
	while(running){
		TCPSocket* peerSocket = tcpServerSocket->listenAndAccept();
		if (peerSocket!= NULL){
			cout<<"new peer connected: "<<peerSocket->destIpAndPort()<<endl;
			//add to connected map
			dispatcher->addPeer(peerSocket);
		}
	}
	cout<<"close server"<<endl;
}

void TCPMessengerServer::close(){
	running = false;
	//==============================================================================================================================
	//waitForThread();


	tcpServerSocket->close();
	waitForThread();
	dispatcher->close();
	dispatcher = NULL;
	delete tcpServerSocket;
	tcpServerSocket = NULL;
}

TCPMessengerServer::~TCPMessengerServer(){
}

int TCPMessengerServer::readCommandFromPeer(TCPSocket* peer){
	int command = 0;
	int rt = peer->recv((char*)&command,4);
	if (rt < 1)
		return rt;
	command = ntohl(command);
	return command;
}

string TCPMessengerServer::readDataFromPeer(TCPSocket* peer){
	string msg;
	char buff[1500];
	int msgLen;
	peer->recv((char*)&msgLen,4);
	msgLen = ntohl(msgLen);
	int totalrc = 0;
	int rc;
	while (totalrc < msgLen){
		rc = peer->recv((char*)&buff[totalrc],msgLen-totalrc);
		if (rc>0)
			totalrc += rc;
		else
			break;
	}
	if (rc > 0 && totalrc == msgLen){
		buff[msgLen] = 0;
		msg = buff;
	}
	else {
		peer->close();
	}
	return msg;
}

void TCPMessengerServer::sendCommandToPeer(TCPSocket* peer, int command){
	command = htonl(command);
	peer->send((char*)&command, 4);
}

void TCPMessengerServer::sendDataToPeer(TCPSocket* peer, string msg){
	int msgLen = msg.length();
	msgLen = htonl(msgLen);
	peer->send((char*)&msgLen,4);
	peer->send(msg.data());
}

void TCPMessengerServer::listPeers(){
	dispatcher->listPeers();
}
void TCPMessengerServer::printLoggedInMap(){
	cout<<dispatcher->getLoggedInPeers()<<endl;
}

