// File: ClientHandler.h
// 10/2011
// abenjamin@wustl.edu - Alex Benjamin
// linfeixb27@gmail.com - Yinfan Li
// mahesh.mahadevan@gmail.com - Mahesh M.

#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <ace/Event_Handler.h>
#include <ace/Reactor.h>
#include "DataWrapper.h"

class ClientHandler : public ACE_Event_Handler
{
public:
	ClientHandler(ACE_Reactor* r) : ACE_Event_Handler(r) { }
	virtual ~ClientHandler() {}	
	ACE_HANDLE get_handle() const { return dataWrapper.getStream().get_handle(); }
	int handle_close(ACE_HANDLE=ACE_INVALID_HANDLE, ACE_Reactor_Mask=0);
	int handle_input(ACE_HANDLE=ACE_INVALID_HANDLE);
	virtual int playerConnected() = 0;
	virtual void disconnectPlayer() = 0;
	virtual int handleMessageReceived(ACE_InputCDR &cdr) = 0;
	int send(const ACE_OutputCDR &cdr);
	ACE_INET_Addr peerAddr;
private:
	friend class ServerHandler;	
	int open();
	ACE_SOCK_Stream& peer() { return dataWrapper.getStream(); }
	DataWrapper dataWrapper;	
};

#endif 
