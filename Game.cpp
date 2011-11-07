// File: Game.cpp
// 10/2011
// abenjamin@wustl.edu - Alex Benjamin
// linfeixb27@gmail.com - Yinfan Li
// mahesh.mahadevan@gmail.com - Mahesh M.

#include "Player.h"
#include "Const.h"
#include "Codes.h"
#include "Game.h"
#include <algorithm>

using namespace std;

//Adds a player to the game. Steps are:
//  1) Verify that player isn't already in the game
//  2) Send the player a list of other players in the game
//  3) Add the player to the game containers
//  4) Tell the other players in the game that someone has joined
//  5) Deal cards to the new player
int Game::joinPlayer(Player* player)
{
	//Check that the player isn't already in the game. If they are, 
	//let the client know that they cannot join again
	if(players.find(player) != players.end())
		return player->respondMessageToClient(CMD_ENTER_GAME,STATUS_FORBIDDEN,this->name.c_str());

	// We have a valid player

	//Send the list of all current players to the person who
	//is now joining
	ACE_OutputCDR cdr( ACE_DEFAULT_CDR_BUFSIZE );
	
	//Add appropriate message identifier
	cdr << (ACE_CDR::ULong)CMD_ENTER_GAME;
	//Add state code
	cdr << (ACE_CDR::ULong)STATUS_OK;
	//Add number of players in the game already
	cdr << (ACE_CDR::ULong)players.size();
	//Add new player's name
	//Lab2 - Add game name and game type
	cdr.write_string( type.c_str() );
	cdr.write_string( name.c_str() );
	//Iterate through player list and add everyone else's name as well
	for(set<Player*>::iterator itr=players.begin(); itr != players.end(); ++itr) 
		cdr.write_string((*itr)->name.c_str());

	//DataWrapper will handle streaming over the socket
	if(player->send(cdr) == -1)
		return -1;

	//Record the new player in this games containers
	player->pGameList.push_back(this);
	players.insert(player);

	cout << player->name << " has joined game (" << name << ")" << endl;

	//Inform all the other players that are connected that a new player
	//has joined the game
	for(set<Player*>::iterator itr=players.begin(); itr != players.end(); ++itr)
	{
		if(player == *itr)
			continue;
		ACE_OutputCDR oCDR(ACE_DEFAULT_CDR_BUFSIZE);

		//Appropriate message identifier
		oCDR << (ACE_CDR::ULong)PRINT_GAME_ENTRY;

		//Lab2 - Type of the Game
		oCDR.write_string( type.c_str() );
		//Name of this game
		oCDR.write_string( name.c_str() );

		//Name of the player
		oCDR.write_string( player->name.c_str() );

		//Sending and error checking taken care of by Player -> DataWrapper
		(*itr)->send(oCDR);
	}

	// Deal if there are enough players
	if(players.size()>=MIN_PLAYER && !waitingBetweenGames)
	{	
		if(!gameInProgress())
			startDealing();
		//We're constrained by the number of cards we have in the deck,
		//so we enforce a max number of players (any others will be spectators)
		else if(players.size() < MAX_PLAYER)
			dealToPlayer(player);
	}
	return 0;
}

//This method handles removing a player from a specific game.
//It sends out notifications to the other players, and if the
//round has started already (ie, the player has cards), 
//then the round must end.
void Game::removePlayer(Player* player)
{
	//Quick error check on if the player is actually here.
	//If not, print a message and don't do anything else
	if(players.find(player) == players.end()){
		cout << "ERROR: Trying to remove a player that is not present!" << endl;
		return;
	}
	else {
		cout << "Player (" << player->name << ") has left game (" << name << ")" << endl;
		players.erase(player);
	}

	//Inform all of the other players of this game of who is leaving
	for(set<Player*>::iterator itr=players.begin(); itr != players.end(); ++itr)
	{
		ACE_OutputCDR notify_cdr(ACE_DEFAULT_CDR_BUFSIZE);
		//Add the appropriate message identifier code
		notify_cdr << (ACE_CDR::ULong)PRINT_LEAVE_GAME;

		//Lab2 - Type of Game they are leaving
		notify_cdr.write_string(type.c_str());
		//The name of the game they are leaving
		notify_cdr.write_string(name.c_str());

		//The name of the player who is leaving
		notify_cdr.write_string(player->name.c_str());
		
		//Sending and error checking taken care of by Player -> DataWrapper
		(*itr)->send(notify_cdr);
	}

	//If the player is in the middle of a game when they leave, we need to
	//do some additional cleanup. If a hand has already been dealt to the player
	//that is leaving, the game ends. Otherwise, it's fine to continue without them.
	if(gameInProgress())
	{
		//Find the player in our score list (if present)
		vector<score_player_pair>::iterator itr = findPlayerScore(player);

		//Player doesn't have any cards yet, so it's fine to just remove
		//them and continue the game
		if(itr == scores.end())
			return;

		cout << "Player (" << player->name << ") left in the middle of game (" << name << "). Round ending." << endl;

		//Let the rest of the players know that the round is ending because
		//a player left mid-round
		for(vector<score_player_pair>::iterator iter=scores.begin(); iter != scores.end(); ++iter)
			iter->second->sendMessageToClient(PRINT_GAME_ERROR, name.c_str());

		//Clean up the residual round information and set the wait cycle before
		//the dealer starts the next round
		scores.clear();
		waitBetweenGames();
	}
}

//The game runs through everyone who is connected and 
//deals them five cards at the beginning of the round.
void Game::startDealing()
{
	if(gameInProgress()){
		cout << "ERROR: Called startDealing when game was already in progress. Ignoring." << endl;
		return;
	}
	else if(players.size() < MIN_PLAYER){
		cout << "ERROR: Called startDealing when not enough players were present. Ignoring." << endl;
	}

	//Reset the game's record for the new round
	numScoresReported = 0;
	
	//Shuffle the deck and make sure it has all of the cards
	deck.shuffle();

	cout << "Game (" << name << "): of Type (" << type << "): beginning a new game:" << endl;

	//We only have a set number of cards in the deck, so we enforce a maximum
	//number of players (Deck size / Hand size) that can be playing the game 
	//at once. Any extra players will be spectators but will not have hands dealt
	//to them.
	set<Player*>::iterator itr = players.begin();
	for( unsigned int plyNum=0; itr != players.end() && plyNum<MAX_PLAYER; ++itr,++plyNum)
		dealToPlayer(*itr);
}

//This method deals five cards to the player and then
//sends the game name and hand to the client
void Game::dealToPlayer(Player* player)
{
	cout << "Game (" << name << "): of Type (" << type << "): Dealing to player: " << player->name << endl;

	//Whenever we deal cards to a player, we also want to score
	//their hand at the end.
	scores.push_back(score_player_pair(-1, player));

	//Send the client a message with the hand that was dealt
	ACE_OutputCDR cdr( ACE_DEFAULT_CDR_BUFSIZE );

	//Preface with appropriate message identifier
	cdr << (ACE_CDR::ULong)PRINT_CARD_HAND;

	//Lab2 - Add type of the game
	cdr.write_string(type.c_str());
	//Add the name of the game
	cdr.write_string(name.c_str());

	// Deal out five cards and add them to the message to be sent out
	for(unsigned int cardNum=0; cardNum<HAND_SIZE; ++cardNum)
	{
		Rank rank;
		Suit suit;
		deck.dealCard(rank, suit);
		cdr << (ACE_CDR::Char)rank;
		cdr << (ACE_CDR::Char)suit;
	}

	//Sending and error checking taken care of by Player -> DataWrapper
	player->send(cdr);
}

//This game handles the cleanup after a game is over. It sends
//out the scores to all of the players and then cleans up memory.
void Game::finishGame()
{
	cout << "Scores for game (" << name << "):" << endl;
	
	//Game has now ended, so we want to send out the scores to the clients
	ACE_OutputCDR cdr(ACE_DEFAULT_CDR_BUFSIZE);
	//First add the appropriate message identifier
	cdr << (ACE_CDR::ULong)PRINT_WINNER;

	//Lab2 Add type of the game
	cdr.write_string( type.c_str());
	//Add the name of the game
	cdr.write_string( name.c_str() );
	//How many players/scores we have
	cdr << (ACE_CDR::ULong)scores.size();

	//When finishGame is called, all scores should have been reported.
	//We sort with a functor based on the integer
	sort(scores.begin(),scores.end(),greater<score_player_pair>());

	//Iterate over the range of te scores, print them out in order of rank,
	//and add them to the message block
	unsigned int rank=1;
	for(vector<score_player_pair>::iterator itr=scores.begin(); itr != scores.end(); ++itr, ++rank) {
		cout << "Player: " << itr->second->name << ", Score: " << itr->first << ", Rank: " << rank << endl;
		cdr.write_string(itr->second->name.c_str());
	}
	
	//Make sure that the data stream is ok
	//If not, we have to shut down and release resources (shouldn't happen)
	if(!cdr.good_bit()) {
		reactor()->end_reactor_event_loop();
		return;
	}
	
	//Send out the message block to those who are connected
	for(set<Player*>::iterator itr = players.begin(); itr != players.end(); ++itr)
		(*itr)->send(cdr); //Sending and error checking taken care of by Player -> DataWrapper
	
	//Clean up the residual round information and set the wait cycle before
	//the dealer starts the next round
	scores.clear();
	waitBetweenGames();
}

//After a game ends, we want to wait for a bit before the
//next game starts. Gives players a chance to join/leave.
void Game::waitBetweenGames()
{
	long timeout;
	timeout = reactor()->schedule_timer(this,0,ACE_Time_Value(timeBetweenGames));

	//Something bad happened...
	if(timeout == -1)
	{
		//Shut down and release resources
		reactor()->end_reactor_event_loop();
		return;
	}
	//Set the flag that we're now waiting
	waitingBetweenGames = true;
	//Lab2 print game type
	cout << "Game (" << name << ") of Type (" << type << ") waiting for " << timeBetweenGames << " seconds before next game." << endl;
}

// Update score and send it to the particular client
int Game::calculateHandScore(Player* player, int score)
{
	//Check a couple error cases:
	// 1) Player is not actually a member of this game
	// 2) Game is currently in a waiting period
	//In either case, inform the client that it has asked for info at
	//the wrong time.
	//Lab2 send game type with game names
	if(players.find(player)==players.end() || !gameInProgress())
		return player->respondMessageToClient(CMD_CARD_SCORE,STATUS_FORBIDDEN,type.c_str(),name.c_str());

	//Get an iterator positioned at this players place in the score
	//container
	vector<score_player_pair>::iterator itr = findPlayerScore(player);

	// Make sure that the player actually has a score entry. If
	// number of players is greater than MAX_PLAYER, we will have some
	// spectators who don't have scores. Alternately, if the player hasn't
	// recorded a score yet, the value will be -1
	if(itr == scores.end() || itr->first != -1)
		return player->respondMessageToClient(CMD_CARD_SCORE,STATUS_FORBIDDEN,type.c_str(),name.c_str());

	//Everything is ok at this point -> player is in the game and has a score
	int	ret = player->respondMessageToClient(CMD_CARD_SCORE,STATUS_OK,type.c_str(),name.c_str());
	//Something happened with sending
	if(ret == -1)
		return -1;

	// Sync up our local container with the player's score
	itr->first = score;
	numScoresReported++;
	//Lab2 printing game type
	cout << "Game (" << name << ") of Type (" << type << ") received score from " << player->name << endl;
	
	// If all scores have been run through this method, then the game
	// is ready to be cleaned up
	if(scores.size() == numScoresReported)
		finishGame();	

	return 0;
}

//Tries to find a certain players score. Returns the iterator position
//at that player if present, else the end() iterator
vector<score_player_pair>::iterator Game::findPlayerScore(Player* player) {
	for( vector<score_player_pair>::iterator itr=scores.begin(); itr != scores.end(); ++itr)
		if(itr->second == player)
			return itr;
	return scores.end();
}

//This event handler is called at the expiration of
//the timeBetweenGames period. It checks if there are enough
//players and if so begins a new game.
int Game::handle_timeout(const ACE_Time_Value&, const void*)
{
	if(gameInProgress()){
		cout << "ERROR: Tried to end waiting period while game was still going! Ignoring." << endl;
		return -1;
	}
	//Lab2 print game type
	cout << "Game (" << name << ") of Type (" << type << ") trying to begin next round: ";

	//Reset the flag for next time
	waitingBetweenGames = false;

	//If there are enough players in the game, we can begin
	//the next round
	if( players.size() >= MIN_PLAYER ){
		cout << "success!" << endl;
		startDealing();
	}
	else
		cout << "not enough players, waiting." << endl;

	return 0;
}


//Lab2 This method will do actual replace of discarded
// cards with new cards from the deck and will send back to Player
int Game::swapCards(vector<CardPair> discardedCards,Player* player)
{
	//Check for game type if it is of type Draw
	if(type.compare(GAME_TYPE_DRAW) != 0)
		return player->respondMessageToClient(CMD_DISCARD_CARDS,STATUS_FORBIDDEN);

	ACE_OutputCDR cdr( ACE_DEFAULT_CDR_BUFSIZE );

	//Add appropriate message identifier
	cdr << (ACE_CDR::ULong)CMD_DISCARD_CARDS;
	//Add state code
	cdr << (ACE_CDR::ULong)STATUS_OK;
	// Add type of the game
	cdr.write_string(type.c_str());
	//Add the name of the game
	cdr.write_string(name.c_str());
	//Add number of cards that are being replaced
	cdr << (ACE_CDR::ULong)discardedCards.size();

	// Deal out cards to Player as many as discarded by Player
	//and add them to the message to be sent out
	for(unsigned int cardNum=0; cardNum<discardedCards.size(); ++cardNum)
	{
		Rank rank;
		Suit suit;
		deck.dealCard(rank, suit);
		cdr << (ACE_CDR::Char)rank;
		cdr << (ACE_CDR::Char)suit;
	}

	//Sending and error checking taken care of by Player -> DataWrapper
	player->send(cdr);

	// Now should add back the discarded cards to the deck and shuffle
	//need to ensure only to shuffle the cards that remain with dealer
	deck.addCardsToDeck(discardedCards);

}
