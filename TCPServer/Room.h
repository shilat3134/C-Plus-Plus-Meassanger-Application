

#ifndef ROOM_H_
#define ROOM_H_

#include <strings.h>
#include <string>
#include <map>

using namespace std;

class Room {
	string roomName;
	string creator;
	//string ip:port (default - TCP),string port - UDP 2001...
	map<string,string> roomMembers;
public:

	Room(string roomName,string creator);
	map<string,string>::iterator getBeginMemberRoom();
	map<string,string>::iterator getEndMemberRoom();
    bool isInTheRoom(string ipAndPort);
    string getRoomName();
    string getCreator();
    void addMemberToRoom(string ipAndPort,string udpPort);
    void deleteMemberFromRoom(string ipAndPort);
    string getValueByKey(string ipAndPort);
	virtual ~Room();
};

#endif /* ROOM_H_ */
