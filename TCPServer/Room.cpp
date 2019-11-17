

#include "Room.h"

Room::Room(string roomName,string creator) {
	// TODO Auto-generated constructor stub
      this->roomName=roomName;
      this->creator=creator;
}

map<string,string>::iterator Room::getBeginMemberRoom(){
	return roomMembers.begin();
}

map<string,string>::iterator Room::getEndMemberRoom(){
	return roomMembers.end();
}

bool Room::isInTheRoom(string ipAndPort){
	map<string,string>::iterator iter=roomMembers.find(ipAndPort);
	if(iter!=roomMembers.end())
		return true;
	return false;
}

string Room::getRoomName(){
	return roomName;
}

string Room::getCreator(){
	return creator;
}

void Room::addMemberToRoom(string ipAndPort,string udpPort){
   roomMembers.insert(pair<string,string>(ipAndPort,udpPort));
}

void Room::deleteMemberFromRoom(string ipAndPort){
   roomMembers.erase(ipAndPort);
}

string Room::getValueByKey(string ipAndPort){
	map<string,string>::iterator it=roomMembers.find(ipAndPort);
	if(it==roomMembers.end())
		return "";
	return (*it).second;
}

Room::~Room() {
	roomMembers.clear();
	// TODO Auto-generated destructor stub
}

