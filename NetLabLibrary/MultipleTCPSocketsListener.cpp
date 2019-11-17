

#include "MultipleTCPSocketsListener.h"

using namespace std;


/*
 * Add the given socket to the socket list to be listen on
 */
void MultipleTCPSocketsListener::addSocket(TCPSocket* socket){
	//TODO: add he given socket to the socket list
	sockets.push_back(socket);
}

/*
 * Set the given sockets to the socket list to be listen on
 */
void MultipleTCPSocketsListener::addSockets(vector<TCPSocket*> socketVec){
	//TODO: set the given sockets as the socket list to selct from
	sockets.insert(sockets.end(),socketVec.begin(),socketVec.end());
}

/*
 * This method checks to see if any of the previous given sockets are ready for reading
 * It returns one of the Sockets that are ready.
 */
TCPSocket* MultipleTCPSocketsListener::listenToSocket(long timev){
	//TODO: create local set for the select function (fd_set)
     fd_set fset;
     struct timeval ftime;
	//TODO: fill the set with file descriptors from the socket list using (FD_SET macro)
     FD_ZERO(&fset);
     int maxFD=0;
     for(int i=0;i<sockets.size();i++)
     {
    	 FD_SET(sockets[i]->getSocket_FD(),&fset);
    	 if(sockets[i]->getSocket_FD()>maxFD)
    		 maxFD=sockets[i]->getSocket_FD();
     }


    	 ftime.tv_sec=timev;
    	 ftime.tv_usec=0;

	//TODO: perform the select
     select(maxFD+1,&fset,NULL,NULL,&ftime);
	//TODO: check the returned value from the select to find the socket that is ready
     for(int i=0;i<sockets.size();i++)
          {
         	 if(FD_ISSET(sockets[i]->getSocket_FD(),&fset))
         		 return sockets[i];
          }
	//TODO: if select return a valid socket return the matching TCPSocket object otherwise return NULL

	return NULL;
}

