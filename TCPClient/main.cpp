

#ifndef MAIN_CPP_
#define MAIN_CPP_

#include <iostream>
#include <string.h>
#include "TCPMessengerClient.h"


using namespace std;

void printInstructions(){
	cout<<"To open connect to server type: c <server ip>"<<endl;
	cout<<"To Login the system: login<username><pass>"<<endl;
	cout<<"To open session type with peer: o <username>"<<endl;
	cout<<"To open a room or get into the room: or"<<endl;
	cout<<"To see all loggedIn users: lcu"<<endl;
	cout<<"To see all rooms: lr"<<endl;
	cout<<"To see all users in your room: lru"<<endl;
	cout<<"Opening session will close any previously active sessions"<<endl;
	cout<<"After the session is opened, to send message type: s <message>"<<endl;
	// TODO: extend to support all commands
	cout<<"To close opened session type: cs"<<endl;
	cout<<"To disconnect from server type: d"<<endl;
	cout<<"To exit type: x"<<endl;
	cout<<endl;
}

int main(){

	cout<<"Welcome to TCP messenger"<<endl;
	printInstructions();
	TCPMessengerClient* messenger = new TCPMessengerClient();
	while(true){
		string msg;
		string command;
		cin >> command;
		if(command == "c"){
			string ip;
			cin >> ip;
			messenger->connect(ip);
		}else if(command=="lcu"){
			if(messenger->isConnected())
			    messenger->askLoggedPeers();
		    else
			    cout<<"Fail: You should connect first"<<endl<<endl;
		}else if(command=="lr"){
             if(messenger->isConnected())
            	 messenger->askRooms();
             else
            	 cout<<"Fail: You should connect first"<<endl<<endl;
		}else if(command=="lru"){
            if(messenger->isInRoom())
            	messenger->printRoom();
		}else if(command == "o"){
			string username;
			cin >> username;
			if(!messenger->isLoggedIn())
				cout<<"Fail: You should login first"<<endl<<endl;
			else{
				//if i in one session and i want to open new session - should close the first session
				if(messenger->isActiveClientSession())
					messenger->closeActiveSession();
				messenger->openSession(username);
			}

		}else if(command == "or"){
			string roomname;
			cin >> roomname;
			if(!messenger->isLoggedIn())
				cout<<"Fail: You should login first"<<endl<<endl;
			else{
				if(messenger->isActiveClientSession())
					messenger->closeActiveSession();
				messenger->openRoom(roomname);
			}
		}else if(command == "s"){
			if (!messenger->isActiveClientSession()){
			cout<<"Fail: No client session is active"<<endl<<endl;
			printInstructions();
			}else{
			char msg[256];
			cin.getline(msg,sizeof(msg));
			//getline(std::cin,msg);
			//cin >> msg;
			//if(msg.sizeo()>0 && msg[0] == ' ') msg.erase(0,1);
			messenger->send(msg);
			}
		}else if(command == "cs"){
			if(messenger->isActiveClientSession())
			   messenger->closeActiveSession();
			else
				cout<<"You are not in any active session"<<endl<<endl;
		}else if(command == "d"){
			if(messenger->isConnected())
			   messenger->disconnect();
			else
				cout<<"You arn't connected to server"<<endl<<endl;


			// TODO: extend functionality to support all commands

		}else if(command == "login"){
			if(!messenger->isConnected()){
				cout<<"Fail: No server connection is active"<<endl<<endl;
				printInstructions();
			}
			else{
			string userAndPass;
			cin>>userAndPass;
			messenger->login(userAndPass);
			}
		}else if(command == "x"){
			break;
		}else{
			cout<<"wrong input"<<endl<<endl;
			printInstructions();
		}
	}
	if(messenger->isConnected())
	  messenger->disconnect();
	delete messenger;
	cout<<"messenger was closed"<<endl;
	return 0;
}



#endif /* MAIN_CPP_ */
