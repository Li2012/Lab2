// File: Player_Svc_Handler.h
// 10/2011
// abenjamin@wustl.edu - Alex Benjamin
// linfeixb27@gmail.com - Yinfan Li
// mahesh.mahadevan@gmail.com - Mahesh M.

#ifndef PLAYER_SVC_HANDLER_H
#define PLAYER_SVC_HANDLER_H

#include <ace/ACE.h>
#include <ace/Svc_Handler.h>
#include <ace/String_Base.h>
#include <ace/CDR_Stream.h>
#include <ace/SOCK_Connector.h>
#include <ace/INET_Addr.h>
#include <string>
#include <vector>
#include "DataWrapper.h"
#include "ConnectInfo.h"
//#include "PlayerConnection.h"
#include "Const.h"
#include "Codes.h"
#include "Hand.h"
using namespace std;

class PlayerConnection;

class Player_Svc_Handler : public ACE_Svc_Handler<ACE_SOCK_Stream,ACE_NULL_SYNCH>
{
public:

	Player_Svc_Handler() {}
	~Player_Svc_Handler() {}
	//Lab2 - changed games to vector of gametype - game tuple
	Player_Svc_Handler(const ACE_INET_Addr& addr,const ACE_TCHAR* inName,vector<gametype_game_pair> &inGames) : address(addr),pName(inName),games(inGames) { theHand.getHandBoolPair()->clear(); }
	int handle_input( ACE_HANDLE=ACE_INVALID_HANDLE );
	int handle_close( ACE_HANDLE=ACE_INVALID_HANDLE, ACE_Reactor_Mask=0 );

	inline ACE_HANDLE get_handle() const {
	  return dataWrapper.getStream().get_handle();
	}
	inline ACE_SOCK_Stream& peer() { return dataWrapper.getStream(); }

	int open( void* x= 0 );
	void setPlayerConnection(PlayerConnection * conn);
	typedef ACE_Svc_Handler<ACE_SOCK_Stream,ACE_NULL_SYNCH>	serviceBase;
private:
	int processRead(ACE_InputCDR &cdr);
	//Lab2 method to send score to Dealer
	int sendScore(const ACE_TCHAR* gameType,const ACE_TCHAR* gameName);
	//Lab2 method to receive new cards
	int receiveNewCards(ACE_InputCDR &cdr);
	//Lab2 method to discard cards
	int discardCards(const ACE_TCHAR* gameType,const ACE_TCHAR* gameName);
	int printScore(ACE_InputCDR &cdr);

	int playerEntry();
	int printEnter(ACE_InputCDR &cdr);
	int printLeave(ACE_InputCDR &cdr);
	int printEntry(ACE_InputCDR &cdr);
	int handleEnterGame();

	int printError(ACE_InputCDR &cdr);
	int printWinner(ACE_InputCDR &cdr);
	int printHand(ACE_InputCDR &cdr);

	int initSignin(const ACE_TCHAR* playerName);
	int printSignin(ACE_InputCDR &cdr);

	int writeToOutput(ACE_OutputCDR &cdr);

	int connect();
	int disconnect();

	DataWrapper dataWrapper;
	ACE_INET_Addr address;
	ACE_CString pName;
	ACE_CString dealerName;
	//Lab2 - Update vector to hold gametype and game tuple
	//vector<const ACE_TCHAR*> games;
	vector<gametype_game_pair> games;
	PlayerConnection *pConn;
	//Lab2 - include hand as member variable
	Hand theHand;
};


#endif
