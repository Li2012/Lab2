// File: ServerHandler.h
// 10/2011
// abenjamin@wustl.edu - Alex Benjamin
// linfeixb27@gmail.com - Yinfan Li
// mahesh.mahadevan@gmail.com - Mahesh M.

#ifndef SERVERHANDLER_H
#define SERVERHANDLER_H

#include <ace/SOCK_Acceptor.h>
#include <ace/Event_Handler.h>
#include "ClientHandler.h"

class ServerHandler : public ACE_Event_Handler
{
public:
	ServerHandler(ACE_Reactor* r) : ACE_Event_Handler(r) { }
	virtual ~ServerHandler() {}
	virtual ClientHandler* createClient() = 0;
	int open(ACE_INET_Addr &addr);

private:
	int handle_input(ACE_HANDLE = ACE_INVALID_HANDLE);
	int handle_close(ACE_HANDLE = ACE_INVALID_HANDLE, ACE_Reactor_Mask = 0);	
	ACE_SOCK_Acceptor acceptor;
	ACE_HANDLE get_handle() const { return acceptor.get_handle(); }
};

#endif
