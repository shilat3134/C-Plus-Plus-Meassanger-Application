

#ifndef TCPMESSENGERPROTOCOL_H_
#define TCPMESSENGERPROTOCOL_H_

/**
 * TCP Messenger protocol:
 * all messages are of the format [Command 4 byte int]
 * and optionally data follows in the format [Data length 4 byte int][ Data ]
 */
#define MSNGR_PORT 3346

#define LOGIN 						1
#define LOGIN_APPROVED				2
#define LOGIN_REFUSED 				3
#define LIST_ROOMS  				4
#define ROOMS  						5
#define LIST_CONNECTED_USERS		6
#define CONNECTED_USERS				7
#define OPEN_SESSION				8
#define SESSION_REFUSED				9
#define SESSION_ESTABLISHED			10
#define CLOSE_SESSION				11
#define SEND_MSG_TO_PEER			12
#define ENTER_ROOM					13
#define ENTERED_ROOM				14
#define USER_ADDED_TO_ROOM			15
#define EXIT_ROOM					16
#define ROOM_DELETED				17
#define PEER_EXIT					18
#define USER_EXIT_ROOM              19


#endif /* TCPMESSENGERPROTOCOL_H_ */
