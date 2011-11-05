// File: Codes.h
// 10/2011
// abenjamin@wustl.edu - Alex Benjamin
// linfeixb27@gmail.com - Yinfan Li
// mahesh.mahadevan@gmail.com - Mahesh M.

#ifndef CODES_H
#define CODES_H


#include <ace/CDR_Base.h>

// These ranges obviously need to be mutually exclusive
const unsigned int CODE_START	= 0;
const unsigned int PRINT_START		= 100;	

//This is the handshake codes
enum Commands {
	CMD_SIGN_IN = CODE_START,
	CMD_ENTER_GAME,		
	CMD_CARD_SCORE,
	//Lab2 - new command to send discarded cards
	CMD_DISCARD_CARDS
};

//At various stages of the protocol we receive codes from server
//to print out certain information
enum Notifications {
	PRINT_GAME_ENTRY = PRINT_START,
	PRINT_LEAVE_GAME,					
	PRINT_CARD_HAND,					
	PRINT_WINNER,						
	PRINT_GAME_ERROR,
};

enum State {	// modeled after HTTP status codes
	STATUS_OK = 200,

	STATUS_BAD_REQUEST = 400,	// improper request (invalid parameter, etc.)
	STATUS_FORBIDDEN,			// not allowed unconditionally
	STATUS_CONFLICT,			// doesn't add up (change parameters, etc.)
	STATUS_NOT_FOUND,

	STATUS_SERVER_ERROR = 500,	// any of unknown server-side problem

	STATUS_UNKNOWN_ERROR = 600,	// Every unknown errors
	STATUS_NETWORK_ERROR,		// Network errors (failed to check good_bits etc..)
	STATUS_UNEXPECTED_DATA,		// Unexpected data error --> Usually unexpected packet
	STATUS_INVALID_DATA,		// Invalid Data Error
};

ACE_TCHAR* errorMessage(State code);

#endif 
