
#include <strings.h>
#include <string.h>
#include <map>
#include <vector>
#include "MThread.h"
#include "TCPSocket.h"
#include "UDPSocket.h"
#include "Session.h"
#include "TCPMessengerProtocol.h"
#include <stdio.h>
#include <sstream>

using namespace std;
using namespace npl;

/**
 * The TCP Messenger client class
 */

struct Peer{
	string userName;
	string sessionIp;
	int sessionPort;
};

class TCPMessengerClient: public MThread{
	map<string,Peer*> connectedPeers;
	TCPSocket* socket;
	//UDPSocket* uSocket;
	Session* sessionObj=NULL;
	bool running;
	bool connected; //indicate that there is an active connection to the server
	string serverAddress; // the connected server address
	bool sessionActive; // indicate that a session with a peer is active
	//string peerAddress; // the session peer address
	string myUser,myPass;
	string roomName;
    int myPort;
    bool isRoom;
    bool loggedIn;
public:
	/**
	 * initialize all properties
	 */
	TCPMessengerClient();

	/**
	 * client receive loop, activated once a connection to the server is established
	 */
	void run();

	/**
	 * connect to the given server ip (the port is defined in the protocol header file)
	 */
	bool connect(string ip);

	/**
	 * close active session
	 */
	bool closeActiveSession();

	/**
	 * disconnect from messenger server
	 */
	bool disconnect();

	/**
	 * send the given message to the connected peer
	 */
	bool send(string msg);

	/**
	 * return true if a session is active
	 */
	bool isActiveClientSession();

	/**
	 * return true if connected to the server
	 */
	bool isConnected();

	/** Our addition
	 * return true if user is logged in
	 */
	bool isLoggedIn();

	/** Our addition
		 * return true if user is logged in
		 */
		bool isInRoom();

	/** Our addition
	 * set the username of me
	 */
	void setMyUser(string username);

	/** Our addition
	 * set the password of me
	 */
	void setMyPass(string pass);

    /** Our addition
     * Login to messenger
     */
    void login(string userAndPass);

    /**Our addition
     * Open session with one peer
     */
    void openSession(string userName);

    /**
     * Open room
     */
    void openRoom(string rName);

    /**
     * Print all users in room
     */
    void printRoom();

    /**
     * Ask rooms list from
     */
     void askRooms();

     /**
      * Ask for connected peers
      */
     void askLoggedPeers();
private:

	/**
	 * read incoming command
	 */
	int readCommand();

	/**
	 * read incoming data
	 */
	string readDataFromPeer();

	/**
	 * send given command
	 */
	void sendCommand(int command);

	/**
	 * send given message
	 */
	void sendData(string msg);

	/** Our addition
	 * set port by user name
	 */
	void setMyPort(string username);

	/**Our addition
     * Add peer to connected map
	 */
	 void addPeerToMap(string peersAdress);

	 /**Our addition
	  * delete peer from connected map
	  */
	 void deletePeerFromMap(string peersAdress);

	 /**Our addition
	   * clear all members from map
	  */
	  void clearMap();



};

