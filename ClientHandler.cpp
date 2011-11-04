// File: ClientHandler.cpp
// 10/2011
// abenjamin@wustl.edu - Alex Benjamin
// linfeixb27@gmail.com - Yinfan Li
// mahesh.mahadevan@gmail.com - Mahesh M.

#include "ClientHandler.h"

//This method implements the ACE_Event_Handler
//handle_input method and reactively demultiplexes
//messages received over the data stream
int ClientHandler::handle_input(ACE_HANDLE)
{
	//Try to read over the socket into the data stream
	int	error = dataWrapper.read();

	//Couldn't read for some reason, throw an error
	if(error != 1)
		return error;

	//Create a data stream with the message block we received
	ACE_InputCDR cdr(dataWrapper.getData());

	//Make sure that our endian is correct
	cdr.reset_byte_order(dataWrapper.getEndian());

	error = handleMessageReceived(cdr);
	//Error check to make sure we did something with
	//the message
	if(error == -1)
		return error;
	return 0;
}

//This method tries to open up a connection with the
//client on a new socket. If successful, it registers
//the client with the reactor and begins a handshake
int ClientHandler::open()
{
	//Initialize our socket stream wrapper with the
	//INET_Address of the client
	int	error = dataWrapper.init(peerAddr);
	
	//Couldn't connect, return error value
	if(error == -1)
		return error;
	
	//Try to connect
	error = playerConnected();

	//If an error is returned here, the socket failed
	//to open properly and we just end
	if(error == -1) {
		dataWrapper.close();
		return 0;
	}

	//Everything seems ok, try to register with the
	//reactors event loop
	error = reactor()->register_handler(this,ACE_Event_Handler::READ_MASK);

	//Registering with the reactor failed, cannot
	//interact with this client
	if(error == -1) {
		disconnectPlayer();
		dataWrapper.close();
		return error;
	}

	return 0;
}

// This is the sending point for all data from the server
// to a client. It receives a parameter in the form of a
// data stream, and then uses a socket stream wrapper to
// write out to the client.
int ClientHandler::send(const ACE_OutputCDR &cdr)
{
	if(dataWrapper.write(cdr) == -1) {
		//Could not send to the client, close the socket connection
		//and return an error value
		dataWrapper.getStream().close_writer();
		return -1;
	}
	return 0;
}

// This implements the ACE_Event_Handler handle_close and is
// called when the client disconnects/is unregistered with
// the reactor. Releases resources and calls on the Player
// to remove the player from containers. Finally, closes the
// socket stream
int ClientHandler::handle_close(ACE_HANDLE, ACE_Reactor_Mask)
{
	disconnectPlayer();
	dataWrapper.close();
	delete this;
	return 0;
}




