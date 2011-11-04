// File: ServerHandler.cpp
// 10/2011
// abenjamin@wustl.edu - Alex Benjamin
// linfeixb27@gmail.com - Yinfan Li
// mahesh.mahadevan@gmail.com - Mahesh M.

#include "ServerHandler.h"
#include <ace/Log_Msg.h>

//Initializes the ACE_Acceptor at the given peer address
//and registers the event handler subclass with the server
//reactor instance
int ServerHandler::open(ACE_INET_Addr &addr)
{
	//If we can't open on the socket, there isn't much we can do
	if(acceptor.open(addr) == -1)
		return -1;

	//Register the ACE_Event_Handler with the reactor instance
	return reactor()->register_handler(this,ACE_Event_Handler::ACCEPT_MASK);
}

// Implements handle_input method for ACE_Event_Handler
// and tries to create a new ClientHandler
int ServerHandler::handle_input(ACE_HANDLE)
{
	int error = -1;
	
	//Try to create a client handler which will reactively
	//handle events
	ClientHandler* theClient = createClient();

	//Creating the ClientHandler failed (shouldn't happen?)
	if( theClient == NULL )
		return error;

	// Try to accept a socket connection on our ACE_SOCK_Stream
	// at the given peer address
	error = acceptor.accept(theClient->peer(),&theClient->peerAddr);
	if(error == -1){
		if(theClient){
			delete theClient;
			theClient = 0;
		}
		ACE_ERROR_RETURN((LM_ERROR,"%p\n","Acceptor failed in handle_input!"),-1);
	}

	//Open a socket connection
	error = theClient->open();

	//Couldn't open a connection, cannot communicate
	//and return error value
	if(error == -1)
		return error;

	return 0;
}

int ServerHandler::handle_close(ACE_HANDLE, ACE_Reactor_Mask)
{
	acceptor.close();
	return 0;
}

