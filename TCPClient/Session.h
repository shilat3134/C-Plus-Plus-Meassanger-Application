

#ifndef SESSION_H_
#define SESSION_H_

#include "MThread.h"
#include "TCPSocket.h"
#include "UDPSocket.h"

using namespace std;
using namespace npl;

class Session: public MThread {
	bool running;
	UDPSocket* udpSocket;
public:

	Session(int port);
	void setRunning(bool running);
	void run();
	bool send(const string& msg,const string& ip, int port);
	virtual ~Session();
};

#endif /* SESSION_H_ */
