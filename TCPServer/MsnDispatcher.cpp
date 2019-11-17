#include "MsnDispatcher.h"
#include "TCPMessengerProtocol.h"
#include "TCPMessengerServer.h"

/**
 * The dispatcher server reads incoming commands from open peers and performs the required operations
 */
MsnDispatcher::MsnDispatcher(){
	running = false;
	start();
}

MsnDispatcher::~MsnDispatcher(){
}

void MsnDispatcher::run(){
	cout<<"dispatcher started"<<endl;
	running = true;
	while (running){
		MultipleTCPSocketsListener msp;
		msp.addSockets(getPeersVec());
		TCPSocket* readyPeer = msp.listenToSocket(2);
		if(readyPeer == NULL) 	// timeout
			continue;

		int command = TCPMessengerServer::readCommandFromPeer(readyPeer);
		string data;
		string userPass;
		string userName;
		string udpAddr;
		map<string,Room*>::iterator itR;
		TCPSocket* tcpTmp;
		Room * room;
		switch (command) {
		case LOGIN:
			data = TCPMessengerServer::readDataFromPeer(readyPeer);
			cout<<"got login command:"<<readyPeer->destIpAndPort()<<"->"<< data<<endl;

			// TODO: parse the user and password //DONE
			// TODO: check login info, if all is OK send LOGIN_APPROVED to peer and add the peer to the logged in map //DONE
			if(isLoginValid(data)&&!isLoggedIn(data))
			{
			    TCPMessengerServer::sendCommandToPeer(readyPeer, LOGIN_APPROVED);
				loggedInPeers.insert(std::pair<string,TCPSocket *>(data,readyPeer));

			}
			// TODO: if login is wrong or already logged in, send LOGIN_REFUSED to peer //DONE
			else
				TCPMessengerServer::sendCommandToPeer(readyPeer, LOGIN_REFUSED);
            break;
		case OPEN_SESSION:
			data = TCPMessengerServer::readDataFromPeer(readyPeer);
			cout<<"got OPEN_SSESION command:"<<readyPeer->destIpAndPort()<<"->"<< data<<endl;
			if(data!="User1"&&data!="User2"&&data!="User3")
				TCPMessengerServer::sendCommandToPeer(readyPeer,SESSION_REFUSED);
			else
			{
			tcpTmp=getAvailablePeerByUserName(data);
			if(tcpTmp!=NULL)
			{
				//the peer is not in room && the peer is not in activeSessions map
				if((isInRoom(tcpTmp->destIpAndPort())==NULL )&& (isInSession(tcpTmp->destIpAndPort())==activeSessions.end()))
                    establishSession(readyPeer, tcpTmp,"", data);
				else{
					TCPMessengerServer::sendCommandToPeer(readyPeer,SESSION_REFUSED);
				}
			}
			}
			break;
		case CLOSE_SESSION:
			cout<<"got CLOSE_SSESION command:"<<readyPeer->destIpAndPort()<<endl;
             closeSession(readyPeer->destIpAndPort());
             printSessionMap();
			break;
		case ENTER_ROOM:
			data=TCPMessengerServer::readDataFromPeer(readyPeer);
			cout<<"got ENTER_ROOM command:"<<readyPeer->destIpAndPort()<<"->"<< data<<endl;
			itR=activeRooms.find(data);
			userPass=getUserPassByPeer(readyPeer);
			userName=userPass.substr(0, userPass.find(":"));
            udpAddr=getUdpAddress(userName, readyPeer->destIpAndPort());
			if(itR==activeRooms.end()){
				room=new Room(data,readyPeer->destIpAndPort());//In case the requested room isnt exist - create mew room
				activeRooms.insert(pair<string,Room*>(data,room));
			}
			else{
				room=(*itR).second;
				updateRoomMembers(room,udpAddr,USER_ADDED_TO_ROOM);
			}
			data=getRoomMembers(room);
			room->addMemberToRoom(readyPeer->destIpAndPort(),udpAddr);
			TCPMessengerServer::sendCommandToPeer(readyPeer,ENTERED_ROOM);
			TCPMessengerServer::sendDataToPeer(readyPeer,data);
			break;
		case EXIT_ROOM:
			cout<<"got EXIT_ROOM command:"<<readyPeer->destIpAndPort()<<endl;
            room=isInRoom(readyPeer->destIpAndPort());
            exitRoom(room,readyPeer->destIpAndPort());

			break;
		case PEER_EXIT:
			cout<<"got EXIT command:"<<readyPeer->destIpAndPort()<<endl;
			// TODO: disconnect the socket, and remove from all data structures//DONE
            disconnect(readyPeer);
			break;
		case LIST_CONNECTED_USERS:
            TCPMessengerServer::sendCommandToPeer(readyPeer, CONNECTED_USERS);
            TCPMessengerServer::sendDataToPeer(readyPeer, getLoggedInPeers());
			break;
		case LIST_ROOMS:
              TCPMessengerServer::sendCommandToPeer(readyPeer, ROOMS);
              TCPMessengerServer::sendDataToPeer(readyPeer,getRooms());
			break;
		default:
			cout<<"got command:"<<command<<" from peer:"<<readyPeer->destIpAndPort()<<endl;
            disconnect(readyPeer);
			//break;
		}
	}
	cout<<"dispatcher ended"<<endl;
}

void MsnDispatcher::listPeers(){
	// TODO: print all peer's ip + port
	tOpenedPeers::iterator iter = openedPeers.begin();;
	tOpenedPeers::iterator iter2 = openedPeers.end();
		if(openedPeers.empty()){
			cout<<"No one is connected"<<endl;
		}
		else{
			cout<<"Connected peers:"<<endl;
			for(;iter!=iter2;iter++)
			{
			string tmp=(*iter).first;
			cout<<tmp<<endl;
			}
		}
}

void MsnDispatcher::close(){
	cout<<"closing dispatcher"<< endl;
	running = false;

	// TODO: close all peers
	tOpenedPeers::iterator iter = openedPeers.begin();
	tOpenedPeers::iterator iter2 = openedPeers.end();
	for(;iter!=iter2;iter++)
	{
	  (*iter).second->close();
	}

	waitForThread();

	// TODO: delete all peers
	for(iter=openedPeers.begin();iter!=iter2;iter++)
	{
		  delete (*iter).second;
	}

	map<string,Room*>::iterator it = activeRooms.begin();
	map<string,Room*>::iterator it2 = activeRooms.end();

	for(;it!=it2;it++)
	{
	   delete (*it).second;
	}

	openedPeers.clear();
	loggedInPeers.clear();
    activeRooms.clear();
    activeSessions.clear();

	cout<<"dispatcher closed"<< endl;
}

vector<TCPSocket*> MsnDispatcher::getPeersVec(){
	vector<TCPSocket*> vec;
	tOpenedPeers::iterator iter = openedPeers.begin();
	tOpenedPeers::iterator endIter = openedPeers.end();
	for(;iter != endIter;iter++){
		vec.push_back((*iter).second);
	}
	return vec;
}

TCPSocket* MsnDispatcher::getAvailablePeerByUserName(const string &userName){
	// TODO: return a socket of the given username
	string key=userName+":"+userName;
	tOpenedPeers::iterator it=loggedInPeers.find(key);
	if(it!=loggedInPeers.end())
		return (*it).second;
	return NULL;
}

string MsnDispatcher::getUserPassByPeer(TCPSocket* socket){
	string ipAndPort=socket->destIpAndPort();
	tOpenedPeers::iterator iterBegin=loggedInPeers.begin();
	tOpenedPeers::iterator iterEnd=loggedInPeers.end();
	for(;iterBegin!=iterEnd;iterBegin++){
		TCPSocket* tmp=(*iterBegin).second;
		if(tmp->destIpAndPort()==ipAndPort)
			return (*iterBegin).first;
	}
	return "";
}

string MsnDispatcher::getUdpAddress(string username,string tcpAddress){
	string port;
	if(username=="User1")
		port="2001";
	if(username=="User2")
			port="2002";
	if(username=="User3")
			port="2003";
	size_t pos= tcpAddress.find(":");
	string ip=tcpAddress.substr(0, pos);
	return ip+":"+port;
}

void MsnDispatcher::addPeer(TCPSocket* peer){
	openedPeers[peer->destIpAndPort()] = peer;
}
bool MsnDispatcher::isLoginValid(string userPass){
	size_t pos=userPass.find(":");
	string name=userPass.substr(0, pos);
	string password=userPass.substr(pos+1);
    if(name==password&&(name=="User1"||name=="User2"||name=="User3"))
    	return true;
    return false;
}
bool MsnDispatcher::isLoggedIn(string userPass)
{
//	   tOpenedPeers::iterator iter = openedPeers.begin();
//	   tOpenedPeers::iterator endIter = openedPeers.end();
//
//		char name[10];
//		const char* len,*key;
//		for(;iter != endIter;iter++){
//			key=((*iter).first).data();
//			len=strchr(key,':');
//			strncpy(name, key,len-key);
//			if(strcmp(name,userName.data())==0)
//				return true;
//        }
//	return false;
	tOpenedPeers::iterator iter = loggedInPeers.find(userPass);
	if(iter!=loggedInPeers.end())
		return true;
	return false;
}
Room* MsnDispatcher::isInRoom(string ipAndPort){
	map<string,Room*>::iterator it=activeRooms.begin();
	map<string,Room*>::iterator iter=activeRooms.end();
	for(;it != iter;it++){
        Room* tmp=(*it).second;
        if(tmp->isInTheRoom(ipAndPort))
        	return tmp;
	}
	return NULL;
}
map<string,string>::iterator MsnDispatcher::isInSession(string ipAndPort){
	map<string,string>::iterator it=activeSessions.begin();
	map<string,string>::iterator iter=activeSessions.end();
	for(;it != iter;it++){
		if((*it).first==ipAndPort||(*it).second==ipAndPort)
			return it;
	}
	return activeSessions.end();
}

void MsnDispatcher::establishSession(TCPSocket* readyPeer,TCPSocket* requestedPeer,string userReadyPeer,string userRequestedPeer){
	string data;
	string username;
	if(userReadyPeer==""){
		data=getUserPassByPeer(readyPeer);
		size_t pos=data.find(":");
		username=data.substr(0, pos);
	}
	else
		username=userReadyPeer;
	string readyData=">"+getUdpAddress(username, readyPeer->destIpAndPort());

	if(userRequestedPeer==""){
			data=getUserPassByPeer(requestedPeer);
			size_t pos=data.find(":");
			username=data.substr(0,pos);
		}
		else
			username=userRequestedPeer;

	string requestData=">"+getUdpAddress(username, requestedPeer->destIpAndPort());//add > because of the problem in empty string

    TCPMessengerServer::sendCommandToPeer(readyPeer,SESSION_ESTABLISHED);
    TCPMessengerServer::sendDataToPeer(readyPeer,requestData);

    TCPMessengerServer::sendCommandToPeer(requestedPeer,SESSION_ESTABLISHED);
    TCPMessengerServer::sendDataToPeer(requestedPeer, readyData);

    activeSessions.insert(pair<string,string>(readyPeer->destIpAndPort(),requestedPeer->destIpAndPort()));

}

string MsnDispatcher::getLoggedInPeers(){
	string str="";
	tOpenedPeers::iterator iter = loggedInPeers.begin();;
	tOpenedPeers::iterator iter2 = loggedInPeers.end();
	if(loggedInPeers.empty()){
		return "No one is logged in";
	}
	else{
		for(;iter!=iter2;iter++)
		{
		string tmp=(*iter).first;
		size_t pos= tmp.find(":");
		tmp=tmp.substr(0,pos);
		str+=" "+tmp;
		}
	}
 return str;
}

void MsnDispatcher::printSessionMap(){
	cout<<"sessions Map:"<<endl;
	map<string,string>::iterator iter=activeSessions.begin();
	map<string,string>::iterator iter2=activeSessions.end();
	for(;iter!=iter2;iter++)
		{
			cout<<(*iter).first<<" - "<<(*iter).second<<endl;
		}
}

void MsnDispatcher::closeSession(string ipAndPort){
//	string ipPort;//the ip:port of the user that someone ask to close the session with him
//	map<string,string>::iterator it=activeSessions.begin();
//	map<string,string>::iterator iter=activeSessions.end();
//	for(;it != iter;it++){
//		if((*it).first==ipAndPort||(*it).second==ipAndPort)
//			if(ipAndPort==(*it).first)
//				ipPort=(*it).second;
//			else
//				ipPort=(*it).first;
//			activeSessions.erase((*it).first);
//		    break;
//		}


	map<string,string>::iterator it=isInSession(ipAndPort);
	if(it!=activeSessions.end()){
		string ipPort;//the ip:port of the user that someone ask to close the session with him
		if(ipAndPort==(*it).first)
		   ipPort=(*it).second;
	    else
		   ipPort=(*it).first;
	    activeSessions.erase((*it).first);
	    tOpenedPeers::iterator con = openedPeers.find(ipPort);
	    TCPMessengerServer::sendCommandToPeer((*con).second,CLOSE_SESSION);
	}

}

void MsnDispatcher::updateRoomMembers(Room* currRoom,string udpAddress,int command){
    map<string,string>::iterator begin=currRoom->getBeginMemberRoom();
    map<string,string>::iterator end=currRoom->getEndMemberRoom();
    if(command==USER_ADDED_TO_ROOM)
    	udpAddress=">"+udpAddress;
    for(;begin!=end;begin++){
    	TCPSocket* currTcp=(*(openedPeers.find((*begin).first))).second;
    	TCPMessengerServer::sendCommandToPeer(currTcp,command);
    	if(command!=ROOM_DELETED){
    		TCPMessengerServer::sendDataToPeer(currTcp,udpAddress);
    	}
    }
}

string MsnDispatcher::getRoomMembers(Room* currRoom){
	 map<string,string>::iterator begin=currRoom->getBeginMemberRoom();
	 map<string,string>::iterator end=currRoom->getEndMemberRoom();
	 string roomMembers=">";
	 for(;begin!=end;begin++){
	    	roomMembers+=(*begin).second+">";
	    }
	 if(roomMembers!=">")
	    roomMembers=roomMembers.substr(0,roomMembers.size()-1);
	 return roomMembers;
}

void MsnDispatcher::exitRoom(Room* room,string ipAndPort){
	string udpAddress="";
	if(room->getCreator()!=ipAndPort){
	   udpAddress=room->getValueByKey(ipAndPort);
	   room->deleteMemberFromRoom(ipAndPort);
	   updateRoomMembers(room, udpAddress,USER_EXIT_ROOM);
	 }
	else{
		room->deleteMemberFromRoom(ipAndPort);
		updateRoomMembers(room, udpAddress, ROOM_DELETED);
        activeRooms.erase(room->getRoomName());
        delete room;
	}
}

void MsnDispatcher::disconnect(TCPSocket* readyPeer){
	Room* room;
	string userPass;
	room=isInRoom(readyPeer->destIpAndPort());
	if(room!=NULL)
		exitRoom(room, readyPeer->destIpAndPort());
	else
		closeSession(readyPeer->destIpAndPort());
	readyPeer->close();
	userPass=getUserPassByPeer(readyPeer);
	openedPeers.erase(readyPeer->destIpAndPort());
	if(userPass!="")
		loggedInPeers.erase(userPass);
	delete readyPeer;
}
string MsnDispatcher::getRooms(){
	string str="";
		map<string,Room*>::iterator iter = activeRooms.begin();;
		map<string,Room*>::iterator iter2 = activeRooms.end();
		if(activeRooms.empty()){
			str= "No rooms";
		}
		else{
		for(;iter!=iter2;iter++)
		   str+=" "+(*iter).first;
		}
	 return str;
}




