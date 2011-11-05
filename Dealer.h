// File: Dealer.h
// 10/2011
// abenjamin@wustl.edu - Alex Benjamin
// linfeixb27@gmail.com - Yinfan Li
// mahesh.mahadevan@gmail.com - Mahesh M.

#ifndef	DEALER_H
#define	DEALER_H

#include <map>
#include <string>
#include "ServerHandler.h"
#include "Deck.h"
#include "Player.h"

class Game;

using namespace std;

class Dealer : public ServerHandler
{
public:
	Dealer(const ACE_TCHAR* dName, ACE_Reactor* r) : ServerHandler(r), name(dName) {}
	~Dealer();

	string name;
	map<string, Player*> players; //Holds all of the players
	map<string, Game*> games; //Holds all of the existing games

	//Adds a player to the map<string,Player*> container by name
	int addPlayerToGame(const ACE_TCHAR* nameIn,const ACE_TCHAR* typeIn, Player* player);

	//Removes a player from the map<string,Player*> container by Player ptr
	void removePlayerFromGame(Game* game, Player* player);

	//Retrieve the score for a certain player. The dealer will
	//call the scoring function for the appropriate game (if it exists)
	//and return the result
	int scoreHand(const ACE_TCHAR* gameName, Player* player, int score);

	//Lab2 replaces discarded cards received from Player
	int replaceCards(const ACE_TCHAR* gameName,vector<CardPair> discardedCards,Player* player);

	//Whenever a new client connects to the server, a new instance of Player
	//must be registered with the reactor
	ClientHandler* createClient() { return new (nothrow) Player( this, reactor() ); }
};

#endif
