#ifndef TCPMESSENGERDISPATCHER_H__
#define TCPMESSENGERDISPATCHER_H__

#include <strings.h>
#include <map>
#include <vector>
#include <set>
#include "MThread.h"
#include "TCPSocket.h"
#include "MultipleTCPSocketsListener.h"
#include "TCPMessengerProtocol.h"
#include <string.h>
#include "Room.h"

using namespace std;
using namespace npl;


/**
 * The dispatcher server reads incoming commands from open peers and performs the required operations
 */
class MsnDispatcher: public MThread{
	typedef map<string, TCPSocket*> tOpenedPeers;
	tOpenedPeers openedPeers;	// opened but not yet logged in
	tOpenedPeers loggedInPeers;	// logged in peers
	map<string,string> activeSessions;
    map<string,Room*> activeRooms;

	bool running;

public:
	/**
	 * constructor
	 */
	MsnDispatcher();
	~MsnDispatcher();

	/**
	 * The Dispatcher main loop
	 */
	void run();

	void addPeer(TCPSocket* peerSocket);

	void listPeers();

	void close();
	string getLoggedInPeers();
	void printSessionMap();
private:

	vector<TCPSocket*> getPeersVec();

	TCPSocket* getAvailablePeerByUserName(const string &userName);

	//OUR Adding
	string getRooms();
	bool isLoginValid(string userPass);
	bool isLoggedIn(string userName);
	Room* isInRoom(string ipAndPort);
	map<string,string>::iterator isInSession(string ipAndPort);
	string getUserPassByPeer(TCPSocket* socket);
	string getUdpAddress(string username,string tcpAddress);
	void establishSession(TCPSocket* readyPeer,TCPSocket* requestedPeer,string userReadyPeer="",string userRequestedPeer="");
	void closeSession(string ipAndPort);
	void updateRoomMembers(Room* currRoom,string udpAddress,int command);//update all members in room that new member enter or member get out
	string getRoomMembers(Room* currRoom);
	void exitRoom(Room* room,string ipAndPort);
	void disconnect(TCPSocket* readyPeer);
};

#endif
