// File: Player.cpp
// 10/2011
// abenjamin@wustl.edu - Alex Benjamin
// linfeixb27@gmail.com - Yinfan Li
// mahesh.mahadevan@gmail.com - Mahesh M.

#include <iostream>
#include <ace/Log_Msg.h>
#include "Lib.h"
#include "Codes.h"
#include "Dealer.h"
#include "Player.h"
#include "Const.h"
using namespace std;

//Called by super class when the socket has been opened
//Simply prints out a message
int Player::playerConnected()
{
	cout << "Connected successfully with " << toString(peerAddr) << endl;
	return 0;
}

//Called by super class when the socket connection is closed
void Player::disconnectPlayer()
{
	cout << "Disconnected from " << toString(peerAddr) << endl;

	//This shouldn't ever really happen, but if
	//this Player object hasn't successfully been initialized,
	//they aren't in the game, so don't need to do anything
	if(name.empty())
		return;

	//Iterate through the list of games this player is connected to
	//and have the dealer remove from each one
	for(list<Game*>::iterator itr=pGameList.begin(); itr != pGameList.end(); ++itr)
		dealer->removePlayerFromGame(*itr, this);

	//Remove the player with the dealer's container
	dealer->players.erase(name);

	cout << "Player (" << name << ") disconnected and has been removed from all games." << endl;
}

//Sends a basic message packet to a client via it's
//super class. This method is basically a status update, so
//it sends only a command id and state, with no message body
int Player::respondMessageToClient(int code, int state)
{
	ACE_OutputCDR cdr(ACE_DEFAULT_CDR_BUFSIZE);	

	//Fill our data stream with the type identifier (code) and state
	if(!( cdr << (ACE_CDR::ULong)code) || !(cdr << (ACE_CDR::ULong)state))
		return -1;
	
	//Let the super class handle the actual send via our Data Wrapper
	return send(cdr);
}

//Sends a message packet to a client via it's
//super class. This method is a generic messsage communication
//that takes any C-style message string
int Player::respondMessageToClient(int code, int state, const ACE_TCHAR* body)
{
	ACE_OutputCDR cdr( ACE_DEFAULT_CDR_BUFSIZE );
	
	//Fill our data stream with the type identifier (code), state, and generic message body
	if(!(cdr << (ACE_CDR::ULong)code) || !(cdr << (ACE_CDR::ULong)state) || !(cdr.write_string(body)))
		return -1;

	//Let the super class handle the actual send via our Data Wrapper
	return send(cdr);
}

//Lab2 - overload for sending 2 strings
//Sends a message packet to a client via it's
//super class. This method is a generic messsage communication
//that takes any C-style message string
int Player::respondMessageToClient(int code, int state, const ACE_TCHAR* body1,const ACE_TCHAR* body2)
{
	ACE_OutputCDR cdr( ACE_DEFAULT_CDR_BUFSIZE );

	//Fill our data stream with the type identifier (code), state, and generic message body
	if(!(cdr << (ACE_CDR::ULong)code) || !(cdr << (ACE_CDR::ULong)state) || !(cdr.write_string(body1)) || !(cdr.write_string(body2)))
		return -1;

	//Let the super class handle the actual send via our Data Wrapper
	return send(cdr);
}

//Sends a message to the client containing just
//a code which tells the client to do something
int Player::sendMessageToClient(int code)
{
	ACE_OutputCDR cdr( ACE_DEFAULT_CDR_BUFSIZE );

	//Data stream in this case only holds the message type
	if(!(cdr << (ACE_CDR::ULong)code))
		return -1;

	//Let the super class handle the actual send via our Data Wrapper
	return send(cdr);
}

//Generic send method to communicate a code and any
//C-style message body over the socket.
int Player::sendMessageToClient(int code, const ACE_TCHAR* body)
{
	ACE_OutputCDR cdr( ACE_DEFAULT_CDR_BUFSIZE );

	//Data stream holds a message type (code) folllowed by string body
	if(!(cdr << (ACE_CDR::ULong)code) || !(cdr.write_string(body)))
		return -1;

	//Let the super class handle the actual send via our Data Wrapper
	return send(cdr);
}

//This method is the handler that is called whenever a message
//is sent to a player. The input stream holds an identifier code
//and then some sort of message body that is passed along to
//the appropriate method
int Player::handleMessageReceived(ACE_InputCDR &cdr)
{
	ACE_CDR::ULong code;

	//First extract the message identifier code. If it isn't there,
	//our data is corrupted and there isn't much we can do
	if(!(cdr >> code))
		ACE_ERROR_RETURN((LM_ERROR, "%s, %s\n",toString(peerAddr).c_str(),"The message did not contain a type code."),-1);

	//Demultiplex the rest of the message out to the appropriate
	//handler using the message id code
	switch(code)
	{
	//Called once per player, addds the Player object
	//to the dealer's container
	case CMD_SIGN_IN:
		return initPlayer( cdr );
	//Called once per player, adds the players list of games
	//they want to join
	case CMD_ENTER_GAME:
		return joinGame( cdr );
	//Called when the player's hand is scored
	case CMD_CARD_SCORE:
		return countScore( cdr );
	//Lab2 - Called when player discards cards for game type - Draw
	case CMD_DISCARD_CARDS:
		return sendNewCards( cdr );
	default:
		ACE_ERROR_RETURN((LM_ERROR, "%s, %s\n",toString(peerAddr).c_str(),"The message type code was not recognized."),-1);
	}
	return 0;
}

//This method reads in data from the Client asking for
//the client's score. It then calls the dealers scoreHand
//method to return the appropriate card score
int Player::countScore(ACE_InputCDR &cdr)
{
	//Lab2 declare game type
	ACE_CDR::Char* gameName,*gameType;
	ACE_CDR::Long score;


	//Read in the game name and score from stream and make sure
	//that both are there
	//Lab2 - read game type with game name and score
	if(!cdr.read_string(gameType) || !cdr.read_string(gameName) || !(cdr >> score))
		ACE_ERROR_RETURN((LM_ERROR, "%s, %s\n",toString(peerAddr).c_str(),"Missing parameter for score count."),-1);

	//Ask the dealer to score the hand for this particular game/player
	return dealer->scoreHand(gameName, this, score);
}

//Called once when a player is first connected. Registers
//the new player object with the dealer
int Player::joinGame(ACE_InputCDR &cdr)
{
	//Lab2 adding gameType
	ACE_CDR::Char* gameType;
	ACE_CDR::Char* gameName;

	//Make sure we can read in the game's name 
	//Lab2 - Making sure we can read gameType as well
	if(!cdr.read_string(gameType) || !cdr.read_string(gameName))
		ACE_ERROR_RETURN((LM_ERROR, "%s, %s\n",toString(peerAddr).c_str(),"protocol error (enter-game: no game name or game type)"),-1 );

	//Ask the dealer to add the player to the requested game
	return dealer->addPlayerToGame(gameName,gameType,this);
}

//This method handles the initial setup and registration
//when a client (player) first connects to the server. First
//checks that the player isn't already present, and if not,
//adds it to the dealer containers
int Player::initPlayer(ACE_InputCDR &cdr)
{
	//If the name for this Player object is not empty,
	//then initPlayer has already been called, and we don't
	//want to initialize them again or we end up with duplicates
	//in the dealer containers
	if(!name.empty())
		return respondMessageToClient(CMD_SIGN_IN,STATUS_FORBIDDEN);

	ACE_CDR::Char* playerName;
	if(!cdr.read_string(playerName))
		ACE_ERROR_RETURN((LM_ERROR, "%s - %s\n",toString(peerAddr).c_str(),"initPlayer: player name missing"),-1);

	//As with above, we don't allow duplicate player entries, so
	//make sure that the player's name isn't already registered
	if(dealer->players.count(playerName) > 0)
		return respondMessageToClient(CMD_SIGN_IN,STATUS_CONFLICT);

	// compare the player's name with the dealer's name
	if(ACE_OS::strcmp(dealer->name.c_str(),playerName) == 0)
		return respondMessageToClient(CMD_SIGN_IN,STATUS_CONFLICT);

	//At this point, everything should be ok and we've verified that
	//this is actually a new player
	name = playerName;
	dealer->players.insert(make_pair(name,this));

	cout << "Player (" << name << ") has succesfully signed in at: " << toString(peerAddr) << endl;
	
	//We send back to the client that the connection was successful
	//and continue the handshake by sending the Dealer name
	ACE_OutputCDR sendCDR(ACE_DEFAULT_CDR_BUFSIZE);
	
	//First stream in the message identifier code
	sendCDR << (ACE_CDR::ULong)CMD_SIGN_IN;

	//Next the status message
	sendCDR << (ACE_CDR::ULong)STATUS_OK;

	//Finally the dealer's name
	if(!sendCDR.write_string(dealer->name.c_str()))
		return -1;

	//Let the super class handle the data stream via Data Wrapper
	return send(sendCDR);
}

//Lab2
//This method handles the discarded cards from Player.
//it will receive from Player number of cards to be discarded
// and also the cards that are to be discarded. This method will
// check if the game type allows cards to be discarded , then will
//send those many cards back to Player.
int Player::sendNewCards(ACE_InputCDR &cdr)
{

		ACE_CDR::Char* gameType;
		ACE_CDR::Char* gameName;
		ACE_CDR::ULong numCards;
		ACE_CDR::Char rank, suit;

		//Make sure we can read in the game's name
		//Lab2 - Making sure we can read gameType as well
		if(!cdr.read_string(gameType) || !cdr.read_string(gameName) || !(cdr >> numCards))
			ACE_ERROR_RETURN((LM_ERROR, "%s, %s\n",toString(peerAddr).c_str(),"Missing parameters for discard cards"),-1 );


		//if game type for the game is not of type Draw then
		//we do not want to accept discarded cards for this game type
		if(strcmp(gameType,GAME_TYPE_DRAW.c_str()) != 0)
			return respondMessageToClient(CMD_DISCARD_CARDS,STATUS_FORBIDDEN);

		//if number of cards discarded is  greater than 5
		//should return error
		if(numCards > 5)
			return respondMessageToClient(CMD_DISCARD_CARDS,STATUS_BAD_REQUEST);

		//Read the discarded cards from the stream
		cout << "Player (" << name << ") @ "<< toString(peerAddr)<<" has discarded following cards: ";

		vector<CardPair> discardedCards;
		for(unsigned int i = 0; i < numCards ; i++)
		{
			CardPair c;
			Card c1;
			//Read in the card rank from data stream
			if(!(cdr >> rank))
			{
				cout << "Error: sendNewCards: Failed to read the card rank." << endl;
				return -1;
			}
			//Read in the card suit from data stream
			if(!(cdr >> suit))
			{
				cout << "Error: sendNewCards: Failed to read the card suit." << endl;
				return -1;
			}
			c.first = (Rank)rank;
			c.second = (Suit)suit;
			//Lab2 - temporary solution , should look for another way to print this
			c1.c_rank = (Rank)rank;
			c1.c_suit = (Suit)suit;
			cout << c1 << " ";
			discardedCards.push_back(c);
		}
		cout << endl;
		//Check if  size of vector is same numCards
		//if not return error status
		if(discardedCards.size()!=numCards)
			return respondMessageToClient(CMD_DISCARD_CARDS,STATUS_BAD_REQUEST);




		//Ask dealer to replace the cards sent by Player
		return dealer->replaceCards(gameName,discardedCards,this);

}

