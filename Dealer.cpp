// File: Dealer.cpp
// 10/2011
// abenjamin@wustl.edu - Alex Benjamin
// linfeixb27@gmail.com - Yinfan Li
// mahesh.mahadevan@gmail.com - Mahesh M.

#include "Lib.h"
#include "Game.h"
#include "Dealer.h"
#include "Codes.h"
using namespace std;

Dealer::~Dealer()
{
	//Since we're holding onto pointers for each game, we need to release
	//the memory at the end
	for(map<string, Game*>::iterator itr = games.begin(); itr != games.end(); ++itr)
		delete itr->second;
}

int Dealer::addPlayerToGame(const ACE_TCHAR* nameIn, Player* player)
{
	string gameName(nameIn);

	//Check if the game is already present
	map<string, Game*>::iterator itr = games.find(gameName);

	Game* gameJoined = NULL;
	//If the iterator found an entry, then the game 
	//that the player wants to join exists already
	if( itr != games.end() )
	{
		gameJoined = itr->second;
	}
	else //This is a game name we haven't seen before, we need to
	{	 //create a new game instance and register with the reactor
		Game* newGame;
		ACE_NEW_RETURN(newGame, Game(gameName.c_str(),reactor()), -1);
		//Add to the game container
		games.insert(make_pair(gameName, newGame));
		gameJoined = newGame;
	}

	//When a player joins a game, we call enter to initialize
	//the player, notify the other clients (players), and then
	//deal the player cards
	return gameJoined->joinPlayer(player);
}

//This method handles unregistering a player from the server
//and cleaning up the appropriate data. If the game specified
//doesn't exist, print a message and ignore the command
void Dealer::removePlayerFromGame(Game* game, Player* player)
{
	if(games.find(game->name.c_str()) != games.end()){
		//We found the game, so try to remove the player now
		game->removePlayer(player);

		// Check if the game this player has just left is now empty
		// If so, we remove that game since nobody is playing
		if(game->players.empty())
			games.erase(game->name);
	}
}

//This method allows the dealer to request that the Game
//calculate the score for a players hand. Includes check for
//bad input.
int Dealer::scoreHand(const ACE_TCHAR* gameName, Player* player,int score)
{
	//Find which game we are scoring the hand for
	map<string, Game*>::iterator iter = games.find(gameName);

	//If we didn't find the game specified (game_name), we send back
	//the appropriate status and end here
	if(iter == games.end())
		return player->respondMessageToClient(CMD_CARD_SCORE, STATUS_NOT_FOUND, gameName);

	//Found the game, try to actually score the hand and report the score
	return iter->second->calculateHandScore(player, score);
}
