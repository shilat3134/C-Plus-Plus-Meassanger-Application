

#include "Session.h"

Session::Session(int port) {
	// TODO Auto-generated constructor stub
	this->udpSocket=new UDPSocket(port);
	running=false;
     this->start();
}

void Session::run(){
	char *buff = new char[1024];
	running=true;
	while(running){
		int n=udpSocket->recv(buff,1024);
		buff[n]='\0';


		cout<<buff<<endl;
	}
		//udpSocket->close();
		//delete udpSocket;
		// receiver thread...
}

bool Session::send(const string& msg,const string& ip, int port)
{
	this->udpSocket->sendTo(msg, ip, port);
	return true;
}

void Session::setRunning(bool running){
	this->running=running;
}

Session::~Session() {
	// TODO Auto-generated destructor stub
	udpSocket->close();
	delete udpSocket;
}


