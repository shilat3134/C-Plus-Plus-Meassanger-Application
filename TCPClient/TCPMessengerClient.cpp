
#include "TCPMessengerClient.h"
#include "TCPMessengerProtocol.h"
#include <stdlib.h>

/**
 * initialize all properties
 */
TCPMessengerClient::TCPMessengerClient(){
	socket = NULL;
	running = false;
	//peerAddress = "";
	sessionActive = false;
	connected = false;
	serverAddress = "";
	isRoom=false;
	roomName="";
	loggedIn=false;
	myUser="";
	myPass="";
}

/**
 * client receive loop, activated once a connection to the server is established
 */
void TCPMessengerClient::run(){
	running = true;

	while(running){
		int command = readCommand();
		string msg;
		string data;
		switch (command){
		case SEND_MSG_TO_PEER:
			msg = readDataFromPeer();
			cout<<">>"<<msg<<endl;
			break;
		case CLOSE_SESSION:
			cout<<"Session was closed by remote peer"<<endl;
			sessionActive = false;
			clearMap();
			break;
		case SESSION_ESTABLISHED:
			cout<<"Session was established with "<<endl;
			sessionActive = true;
			data=readDataFromPeer();
			addPeerToMap(data);
			break;
		case SESSION_REFUSED:
			cout<<"Session was refused, requested peer not available"<<endl;
			sessionActive = false;
			break;
		case ENTERED_ROOM:
			cout<<"You entered to requested room"<<endl;
			sessionActive = true;
			isRoom=true;
			data=readDataFromPeer();
			if(data!=">")
			{
			  cout<<"The room members are: "<<endl;
			  addPeerToMap(data);
			}
			else
			  cout<<"You create a new empty room"<<endl;
			break;
		case USER_ADDED_TO_ROOM:
            cout<<"New member added to the room: "<<endl;
            data=readDataFromPeer();
            addPeerToMap(data);
			break;
		case USER_EXIT_ROOM:
		    cout<<"One member get out of the room: "<<endl;
		    data=readDataFromPeer();
		    deletePeerFromMap(data);
			break;
		case ROOM_DELETED:
		    cout<<"The room is deleted"<<endl;
		    sessionActive=false;
		    isRoom=false;
		    clearMap();
		    break;
		case ROOMS:
			data=readDataFromPeer();
			cout<<"The rooms are: "<<endl;
			cout<<data<<endl;
			break;
		case CONNECTED_USERS:
			data=readDataFromPeer();
			cout<<"The LoggedIn users are: "<<endl;
			cout<<data<<endl;
			break;
		case LOGIN_APPROVED:
			cout<<"Login success"<<endl;
			loggedIn=true;
			setMyPort(myUser);
		    sessionObj=new Session(myPort);
			 //uSocket=new UDPSocket(myPort);
			 break;
		case LOGIN_REFUSED:
			myUser="";
			myPass="";
			cout<<"Login refused"<<endl;
			break;
		default:
			if(socket!=NULL)
			{
			cout<<"communication with server was interrupted - connection closed"<<endl;
			clearMap();
			running = false;
			socket->close();
			connected = false;
			loggedIn=false;
			serverAddress = "";
			sessionActive = false;
			//peerAddress = "";
			delete socket;
			socket=NULL;
			if(sessionObj!=NULL)
			{
			sessionObj->setRunning(false);
			delete sessionObj;

			}
			}
			break;
		}
	}
}

/**
 * connect to the given server ip (the port is defined in the protocol header file)
 */
bool TCPMessengerClient::connect(string ip){
	if(connected) disconnect();
	socket = new TCPSocket(ip,MSNGR_PORT);
	if (socket == NULL) return false;
	connected = true;
	serverAddress = ip;
	start();
	return true;
}

void TCPMessengerClient::openSession(string userName){
	if(userName==myUser)
		cout<<"You cant talk to yourself"<<endl;
	else{
		sendCommand(OPEN_SESSION);
		sendData(userName);
	}
}

void TCPMessengerClient::openRoom(string rName){
	if(isRoom && this->roomName==rName)
		cout<<"You already exist in this room"<<endl;
	else
	{
	this->roomName=rName;
	isRoom=true;
	sendCommand(ENTER_ROOM);
	sendData(rName);
	}
}



/**
 * close active session
 */
bool TCPMessengerClient::closeActiveSession(){
	clearMap();
	if(!isRoom)
	   cout<<"The session is closed"<<endl;
	else
		cout<<"The room is closed"<<endl;
	int command=isRoom?EXIT_ROOM:CLOSE_SESSION;
	sendCommand(command);
	sessionActive = false;
	isRoom=false;
	roomName="";

	return true;
}

/**
 * disconnect from messenger server
 */
bool TCPMessengerClient::disconnect(){
	running = false;
	sendCommand(PEER_EXIT);
	connected=false;
	loggedIn=false;
	sessionObj->setRunning(false);
	delete sessionObj;
	if (socket != NULL){
		if(sessionActive) closeActiveSession();
		socket->close();
		delete socket;
		socket=NULL;
		this->waitForThread();
	}
	return true;
}

/**
 * send the given message to the connected peer
 */
bool TCPMessengerClient::send(string msg){
	//if(!sessionActive) return false;
	//sendCommand(SEND_MSG_TO_PEER);
	//sendData(msg);
	map<string,Peer*>::iterator iter = connectedPeers.begin();
	map<string,Peer*>::iterator endIter = connectedPeers.end();
	string msgForSend;
	for(;iter!=endIter;iter++)
	{
	Peer * current=(*iter).second;
	msgForSend=">";
	msgForSend+=current->userName+" "+msg;
	//uSocket->sendTo(msgForSend, current->sessionIp, current->sessionPort);
	sessionObj->send(msgForSend, current->sessionIp, current->sessionPort);
	}
	return true;
}

/**
 * return true if a session is active
 */
bool TCPMessengerClient::isActiveClientSession(){
	return sessionActive;
}

/**
 * return true if connected to the server
 */
bool TCPMessengerClient::isConnected(){
	return connected;
}

bool TCPMessengerClient::isLoggedIn(){
	return loggedIn;
}
bool TCPMessengerClient::isInRoom(){
	return isRoom;
}

void TCPMessengerClient::setMyUser(string username){
	this->myUser=username;
}

void TCPMessengerClient::setMyPass(string pass){
	this->myPass=pass;
}

void TCPMessengerClient::addPeerToMap(string peersAdress){
    size_t pos1,pos2;
    peersAdress=peersAdress.substr(1);
    do {
    	pos1=peersAdress.find(">");
    	Peer *p= new Peer();
    	string currPeer=peersAdress.substr(0, pos1);
    	string tmp=currPeer;
         pos2=currPeer.find(":");
       	 p->sessionIp=currPeer.substr(0,pos2);
       	 string port=currPeer.substr(pos2+1);
       	 p->sessionPort=atoi(port.c_str());
       	switch (p->sessionPort) {
       			case 2001:
       				p->userName="User1";
       				break;
       			case 2002:
       				p->userName="User2";
       				break;
       			case 2003:
       				p->userName="User3";
       				break;
       			default:
       				break;
       		}
       	connectedPeers.insert(pair<string,Peer *>(tmp,p));
        cout<<p->userName<<" ";
         if (pos1!=string::npos)
        	 peersAdress= peersAdress.substr(pos1+1);
	} while (pos1!=string::npos);
   cout<<endl;
}

void TCPMessengerClient::deletePeerFromMap(string peersAdress){
	map<string,Peer*>::iterator iter=connectedPeers.find(peersAdress);
	cout<<(*iter).second->userName<<endl;
	delete (*iter).second;
	connectedPeers.erase(peersAdress);
}

void TCPMessengerClient::clearMap(){
	map<string,Peer*>::iterator iter = connectedPeers.begin();
	map<string,Peer*>::iterator endIter = connectedPeers.end();
	for(;iter!=endIter;iter++)
	  delete (*iter).second;
	connectedPeers.clear();
}

void TCPMessengerClient::login(string userAndPass){

	size_t pos=userAndPass.find("_");

	myUser=userAndPass.substr(0, pos) ;

	myPass=userAndPass.substr(pos + 1);

	string tmp = myUser + ":" +myPass;
	sendCommand(LOGIN);
	sendData(tmp);
}


void TCPMessengerClient::setMyPort(string username){
	if (username=="User1")
		myPort=2001;
	if (username=="User2")
		myPort=2002;
	if (username=="User3")
		myPort=2003;
}

void TCPMessengerClient::printRoom(){
	map<string,Peer*>::iterator iter = connectedPeers.begin();
	map<string,Peer*>::iterator endIter = connectedPeers.end();
	cout<<"The room members are: ";
	for(;iter!=endIter;iter++)
	  cout<<(*iter).second->userName<<" ";
	cout<<myUser<<endl;
}

void TCPMessengerClient::askRooms(){
	sendCommand(LIST_ROOMS);
}

void TCPMessengerClient::askLoggedPeers(){
	sendCommand(LIST_CONNECTED_USERS);
}

/**
 * read incoming command
 */
int TCPMessengerClient::readCommand(){
	int command = 0;
	socket->recv((char*)&command,4);
	command = ntohl(command);
	return command;
}

/**
 * read incoming data
 */
string TCPMessengerClient::readDataFromPeer(){
	string msg;
	char buff[1500];
	int msgLen;
	socket->recv((char*)&msgLen,4);
	msgLen = ntohl(msgLen);
	int totalrc = 0;
	int rc;
	while (totalrc < msgLen){
		rc = socket->recv((char*)&buff[totalrc],msgLen-totalrc);
		if (rc>0){
			totalrc += rc;
		}else{
			break;
		}
	}
	if (rc > 0 && totalrc == msgLen){
		buff[msgLen] = 0;
		msg = buff;
	}
	else{
		socket->close();
	}
	return msg;
}

/**
 * send given command
 */
void TCPMessengerClient::sendCommand(int command){
	command = htonl(command);
	socket->send((char*)&command,4);
}

/**
 * send given message
 */
void TCPMessengerClient::sendData(string msg){
	int msgLen = msg.length();
	msgLen = htonl(msgLen);
	socket->send((char*)&msgLen, 4);
	socket->send(msg.data());
}


