// File: Player.h
// 10/2011
// abenjamin@wustl.edu - Alex Benjamin
// linfeixb27@gmail.com - Yinfan Li
// mahesh.mahadevan@gmail.com - Mahesh M.

#ifndef PLAYER_H
#define PLAYER_H

#include "ClientHandler.h"
#include <list>
using namespace std;

class Dealer;
class Game;

class Player : public ClientHandler
{
public:
	Player(Dealer* dealerIn, ACE_Reactor* r) : ClientHandler(r), dealer(dealerIn) {}

private:
	friend class Dealer;
	friend class Game;
	int playerConnected();
	void disconnectPlayer();
	int respondMessageToClient(int code, int status);
	int respondMessageToClient(int code, int status, const ACE_TCHAR* body);
	//lab2
	int respondMessageToClient(int code, int status, const ACE_TCHAR* body1,const ACE_TCHAR* body2);
	int sendMessageToClient(int code, const ACE_TCHAR* body);
	int sendMessageToClient(int code);
	int handleMessageReceived(ACE_InputCDR &cdr);
	int initPlayer(ACE_InputCDR &cdr);
	int joinGame(ACE_InputCDR &cdr);
	int countScore(ACE_InputCDR &cdr);
	//Lab2 - method to send new cards to Player
	int sendNewCards(ACE_InputCDR &cdr);

	Dealer*	dealer;
	string	name;
	list<Game*>	pGameList;
};

#endif 

