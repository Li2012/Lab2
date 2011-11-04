// File: Player_Svc_Handler.cpp
// 10/2011
// abenjamin@wustl.edu - Alex Benjamin
// linfeixb27@gmail.com - Yinfan Li
// mahesh.mahadevan@gmail.com - Mahesh M.

#include "Player_Svc_Handler.h"
//#include "Codes.h"
//#include "Const.h"
#include "PlayerConnection.h"
#include "Stream.h"
#include "Poker.h"
#include "Hand.h"
#include "Card.h"
//#include "DataWrapper.h"
#include <iostream>
using namespace std;


void Player_Svc_Handler::setPlayerConnection(PlayerConnection * conn)
{
	pConn = conn;
}

int Player_Svc_Handler::handle_input(ACE_HANDLE)
{
	//Read in data from the socket connection
	int	checkVal = dataWrapper.read();

	//Check if still waiting for rest of message
	if(checkVal != 1)
		return checkVal;

	ACE_InputCDR cdr(dataWrapper.getData());
	//Switch endian if needed
	cdr.reset_byte_order(dataWrapper.getEndian());

	if(processRead( cdr ) == -1)
		//Error occured processing input
		return -1;
	else
		//Everything is fine, input handled
		return 0;
}

int Player_Svc_Handler::open(void* x/* = 0  */)
{
	if( serviceBase::open(x) == -1 )
		return -1;

	connect();

	return 0;
}

//The connect() method is called once when the service handler first
//opens a socket connection (via its base class). It opens up the data
//exchange dataWrapper and also does the initial sign in handshake
int Player_Svc_Handler::connect()
{
	cout << "Socket open and connected." << endl;

	//Open up our data exchange vehicle
	dataWrapper.init(address);
	if(initSignin(pName.c_str()) == -1)
	{
		cout << "Signing in failed with address: " << toString(address) << endl;
		return -1;
	}
	return 0;
}

//This method is called on connect, begins the handshake
//by sending through the player's name.
int Player_Svc_Handler::initSignin(const ACE_TCHAR* playerName)
{
	ACE_OutputCDR cdr(ACE_DEFAULT_CDR_BUFSIZE);
	cdr << (ACE_CDR::ULong)CMD_SIGN_IN;

	cout << "Player: (" << playerName << ") signing in to address: " << toString(address) << endl;

	//Write the player's name into our data container
	if(!cdr.write_string( playerName ))
		return -1;

	//If we can't send the player's name, something is wrong with
	//the connection, and attempt to start this player up will fail
	return writeToOutput( cdr );
}

int Player_Svc_Handler::printSignin(ACE_InputCDR &cdr)
{
	ACE_CDR::ULong status;
	ACE_CDR::Char* dealer;

	if(cdr >> status)
	{
		if(status != STATUS_OK){
			cout << "Error: " << errorMessage((State)status) << " @ address: " << toString(address) << endl;
			return -1;
		}
	}
	//Data container did not have a status code
	else{
		cout << "Error: " << errorMessage(STATUS_NETWORK_ERROR) << " @ address: " << toString(address) << endl;
		return -1;
	}

	//Data received did not have the dealer name
	if(!(cdr >> dealer)){
		cout << "Error: " << errorMessage(STATUS_NETWORK_ERROR) << " @ address: " << toString(address) << endl;
		return -1;
	}

	dealerName = dealer;
	cout << "Successfully signed into dealer: " << dealerName << ", @ " << toString(address) << endl;

	//Once we know what the dealer's name is, the
	//next part of the handshake is to send the list
	//of games that we have
	if(handleEnterGame() != 0){
		cout << "Error: " << errorMessage(STATUS_NETWORK_ERROR) << " @ address: " << toString(address) << endl;
		return -1;
	}

	return 0;
}

int Player_Svc_Handler::handleEnterGame()
{
	cout << "Client sending game names to dealer (" << dealerName << ") @ " << toString(address) << endl;

	for(vector<const ACE_TCHAR*>::iterator itr = games.begin(); itr != games.end(); ++itr)
	{
		ACE_OutputCDR cdr(ACE_DEFAULT_CDR_BUFSIZE);
		cdr << (ACE_CDR::ULong)CMD_ENTER_GAME;

		//Pack the game name to CDR data and write it to the stream,
		//check if everything sent ok
		if(!cdr.write_string(*itr) || writeToOutput(cdr) == -1)
			return -1;

		//Output to the console with some formatting
		if(itr != games.begin())
			cout << ", ";
		cout << (*itr);
	}
	cout << endl;

	return 0;
}

//General purpose method for sending data through our dataWrapper
int Player_Svc_Handler::writeToOutput(ACE_OutputCDR &cdr)
{
	if(dataWrapper.write(cdr) == -1)
		dataWrapper.getStream().close_writer();
	return 0;
}

//This method just handles the message output when player has joined a game
int Player_Svc_Handler::printEnter(ACE_InputCDR &cdr)
{
	ACE_CDR::Char* gameName, *playerName;
	//Try to read in both the new player and which game is being joined
	//If we don't have both, there was bad data
	if( !(cdr >> gameName) || !(cdr >> playerName) ){
		cout << "Failed connecting to: " << toString(address) << ", Error: " << errorMessage(STATUS_UNEXPECTED_DATA) << endl;;
		return -1;
	}
	//Print out the basic data of who has entered and the dealer for the game entered
	cout << "Dealer: " << dealerName << "  Address: " << toString(address) << ", Player: " << playerName << " joining game " << gameName << endl;
	return 0;
}
//This method just handles the message output when a player leaves a game
int Player_Svc_Handler::printLeave(ACE_InputCDR &cdr)
{
	ACE_CDR::Char* gameName, *playerName;
	//Try to read in both the new player and which game is being joined
	//If we don't have both, there was bad data
	if( !(cdr >> gameName) || !(cdr >> playerName) ){
		cout << "Failed connecting to: " << toString(address) << ", Error: " << errorMessage(STATUS_UNEXPECTED_DATA) << endl;;
		return -1;
	}
	//Print out the basic data of who has entered and the dealer for the game entered
	cout << "Dealer: " << dealerName << "  Address: " << toString(address) << ", Player: " << playerName << " leaving game " << gameName << endl;
	return 0;
}

int Player_Svc_Handler::printEntry(ACE_InputCDR& cdr)
{
	ACE_CDR::ULong status, numUsers;
	ACE_CDR::Char* gameName;

	//If we couldn't read in the state, something bad happened
	if(!(cdr >> status)){
		cout << "Failed connecting to: " << toString(address) << ", Error: " << errorMessage(STATUS_UNEXPECTED_DATA) << endl;
		return -1;
	}
	//Check if we've received any code other than ok
	if(STATUS_OK != ((State)status)){
		cout << "printEntry: Failed connecting to: " << toString(address) << ", Error: (" << status << ") " << errorMessage((State)status) << endl;
		return -1;
	}
	//If we can't read in the number of users / game name, something bad happened
	if(!(cdr >> numUsers) || !(cdr >> gameName)){
		cout << "Failed connecting to: " << toString(address) << ", Error: " << errorMessage(STATUS_UNEXPECTED_DATA) << endl;
		return -1;
	}

	//We got all the appropriate data, send out the information to our player
	cout << "Dealer: " << dealerName << ", Address: " << toString(address) << ", Entering: " << gameName;

	for(ACE_CDR::ULong i=0; i<numUsers; ++i)
	{
		if(i == 0)
			cout << " with ";
		else
			cout << ", ";

		ACE_CDR::Char* userStr;
		if( !(cdr >> userStr ) ){
			cout << "Failed with error: " << errorMessage(STATUS_UNEXPECTED_DATA) << ", Address: " << toString(address) << endl;
			return -1;
		}
		cout << userStr;
	}
	cout << endl;

	cout << "Entered successfully" << endl;

	return 0;
}

int Player_Svc_Handler::printScore(ACE_InputCDR &cdr)
{
	ACE_CDR::Long status;
	ACE_CDR::Char* gameName;

	//Make sure that our data contains a status
	if(!(cdr >> status)){
		cout << "Failed connecting to: " << toString(address) << ", Error: " << errorMessage(STATUS_UNEXPECTED_DATA) << endl;
		return -1;
	}
	//Make sure that the status is good
	else if(((State)status) != STATUS_OK){
		cout << "printScore: Failed connecting to: " << toString(address) << ", Error: (" << status << ") " << errorMessage((State)status) << endl;
		return -1;
	}
	//Make sure that our data contains the game name
	else if(!(cdr >> gameName)){
		cout << "Failed connecting to: " << toString(address) << ", Error: " << errorMessage(STATUS_UNEXPECTED_DATA) << endl;
		return -1;
	}

	cout << "Dealer: " << dealerName << ", sent score for game (" << gameName << ")" << endl;

	return 0;
}

//NOTE TO SELF: THIS METHOD NEEDS REFACTORING!
int Player_Svc_Handler::processRead(ACE_InputCDR &cdr)
{
	ACE_CDR::ULong code;

	//This is the main handler method for reading incoming data. It
	//reads the 'code' of the data stream which tells the client which
	//state the handshake is in. Depending on the code, the processRead
	//method will call the appropriate step in the handshake chain

	if(!(cdr >> code))
	{
		cout << toString(address) << " - No state code in data received." << endl;
		return -1;
	}

	int gameState = 0;
	if( code >= PRINT_START )
	{
		switch(code)
		{
		case PRINT_GAME_ENTRY:
			gameState = printEnter(cdr);
			break;
		case PRINT_LEAVE_GAME:
			gameState = printLeave(cdr);
			break;
		case PRINT_CARD_HAND:
			gameState = printHand(cdr);
			break;
		case PRINT_WINNER:
			gameState = printWinner(cdr);
			break;
		case PRINT_GAME_ERROR:
			gameState = printError(cdr);
			break;
		default:			
			cout << toString(address) << " - Couldn't recognize game state code." << endl;
			return -1;
		}
	}
	else 
	{
		switch(code)
		{
		case CMD_SIGN_IN:
			gameState = printSignin(cdr);
			break;
		case CMD_ENTER_GAME:
			gameState = printEntry(cdr);
			break;
		case CMD_CARD_SCORE:
			gameState = printScore(cdr);
			break;
		default:
			cout << toString(address) << " - Couldn't recognize game state code." << endl;
			return -1;
		}
	}

	return gameState;
}

int Player_Svc_Handler::printHand(ACE_InputCDR &cdr)
{
	ACE_CDR::Char rank, suit;
	ACE_CDR::Char* gameName;
	//Read in the game name that this is for
	if(!(cdr >> gameName)){
		cout << "printHand: Did not receive game name. (" << toString(address) << ")" << endl;
		return -1;
	}
	// Check that the received game name is valid
	bool isGame = false;
	for(vector<const ACE_TCHAR*>::iterator itr=games.begin(); itr != games.end(); ++itr)
	{
		//We've found the game, can quit searching
		if(ACE_OS::strcmp( *itr, gameName)== 0)
		{
			isGame = true;
			break;
		}
	}

	//It's possible that we got to the end of the previous
	//loop without finding the game. In that case, we don't
	//have a record of this game and return an error
	if(!isGame){
		cout << "printHand: Client has no record of game: " << gameName << endl;
		return -1;
	}
	// We've found the game, proceed to print out hands
	Hand theHand;
	for(unsigned int cardNum=0; cardNum<HAND_SIZE; ++cardNum)
	{
		Card c;
		//Read in the card rank from data stream
		if(!(cdr >> rank)){
			cout << "Error: printHand: Failed to read the card rank." << endl;
			return -1;
		}
		//Read in the card suit from data stream
		if(!(cdr >> suit)){
			cout << "Error: printHand: Failed to read the card suit." << endl;
			return -1;
		}
		c.c_rank = (Rank)rank;
		c.c_suit = (Suit)suit;
		theHand.add_card(c);
	}
	cout << "Dealer (" << dealerName << ") dealt following for game (" << gameName << "): ";
	cout << theHand;

	int theScore = getHandRank(theHand);
	string handRankString(getRankString(theHand));
	cout << " (" << handRankString << ") - Score: " << theScore << endl;

	// Build a data stream to send to the server with the score
	ACE_OutputCDR ocdr(ACE_DEFAULT_CDR_BUFSIZE);
	//Appropriate message identifier
	ocdr << (ACE_CDR::ULong)CMD_CARD_SCORE;

	//Add in the name of the game and player score respectively
	if(!(ocdr.write_string(gameName)) || !(ocdr << theScore)){
		cout << "Error: printHand: Could not write the game name or score." << endl;
		return -1;
	}
	if(writeToOutput(ocdr) == -1){
		cout << "Error: printHand: Could not write out the score to server." << endl;
		return -1;
	}
	return 0;
}

int Player_Svc_Handler::printWinner(ACE_InputCDR &cdr)
{
	ACE_CDR::Char* gameName, *playerN;
	ACE_CDR::ULong numUsers;

	//Make sure that the data received holds both the
	//name of the game and how many players
	if(!(cdr >> gameName) || !(cdr >> numUsers)){
		cout << "Failed printing winner with error: Unexpected Data" << " @ address: " << toString(address) << endl;
		return -1;
	}

	cout << "Dealer: " << dealerName << " @ address: " << toString(address) << ", game: " << gameName << endl;
	cout << "Order of player scores is: ";

	//Read in the players names from data one at a time
	//and print out with formatting. Names should be in
	//order of rank
	for(ACE_CDR::ULong i=0; i<numUsers; ++i)
	{
		if(!(cdr >> playerN)){
			cout << "Failed printing winner with error: Unexpected Data" << " @ address: " << toString(address) << endl;
			return -1;
		}

		cout << playerN;		
		if( i+1 != numUsers )
			cout << ", ";
	}
	cout << endl;

	return 0;
}

int Player_Svc_Handler::handle_close(ACE_HANDLE, ACE_Reactor_Mask)
{
	disconnect();
	dataWrapper.close();
	return 0;
}

//Called once when the player is done with the game
//Removes the player handler from the reactor
int Player_Svc_Handler::disconnect()
{
	ACE_Reactor::instance()->remove_handler(this,ACE_Event_Handler::WRITE_MASK|ACE_Event_Handler::DONT_CALL);
	pConn->unregisterReactor();
	return 0;
}

//IF we get a status code that the server side failed
int Player_Svc_Handler::printError(ACE_InputCDR &cdr)
{
	ACE_CDR::Char* gameName;
	if(!(cdr >> gameName)){
		cout << "Game failed with error: " << errorMessage(STATUS_UNEXPECTED_DATA) << " @ address: " << toString(address) << endl;
		return -1;
	}
	cout << "Game failed because player left mid-round." << endl;
	return 0;
}

