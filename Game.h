// File: Game.h
// 10/2011
// abenjamin@wustl.edu - Alex Benjamin
// linfeixb27@gmail.com - Yinfan Li
// mahesh.mahadevan@gmail.com - Mahesh M.

#ifndef GAME_H
#define GAME_H

#include <ace/Event_Handler.h>
#include <set>
#include <vector>

#include "Deck.h"

using namespace std;

class Player;

typedef pair<int, Player*>score_player_pair;

class Game : public ACE_Event_Handler
{
public:
	Game(const ACE_TCHAR* nameIn,const ACE_TCHAR* typeIn, ACE_Reactor* r) : ACE_Event_Handler(r), name(nameIn),type(typeIn), waitingBetweenGames(false) { }
	int joinPlayer(Player* player);
	void removePlayer(Player* player);
	int calculateHandScore(Player* player, int score);
	//Lab2 method to swap discarded cards by Player
	int swapCards(vector<CardPair> discardedCards,Player* player);

private:
	friend class Dealer;

	bool gameInProgress() { return !scores.empty(); }
	vector<score_player_pair>::iterator findPlayerScore(Player* player);

	//Deal cards to all players connected to the game
	void startDealing();

	//Deal cards to player
	void dealToPlayer(Player* player );

	//Send out the scores once a game is over
	void finishGame();

	//Sets a wait on our event handler before cycling to the next game
	void waitBetweenGames();

	int handle_timeout(const ACE_Time_Value& now, const void *arg=0);

	string name;
	//Lab2 - Add variable to store game type
	string type;
	set<Player*> players;	
	Deck deck;
	vector<score_player_pair> scores;			// holds any scores that have been reported
	unsigned int numScoresReported;	// how many players have sent in a score
	bool waitingBetweenGames; //true when we are on hold between games
};

#endif
