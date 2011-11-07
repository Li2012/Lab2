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
#include "Card.h"
//#include "DataWrapper.h"
#include <iostream>
#include "Discard.h"
#include<algorithm>
#include <functional>
using namespace std;



struct isCard
{
	Card c;
	isCard(const Card& val) : c(val) {}
	bool operator()(const card_bool_pair& x) const
	{
		return x.first == c;
	}

};

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
	//Lab2 updated output message
	cout << "Client sending game types and game names to dealer (" << dealerName << ") @ " << toString(address) << endl;
	//Lab2 updated vector
	for(vector<gametype_game_pair>::iterator itr = games.begin(); itr != games.end(); ++itr)
	{
		ACE_OutputCDR cdr(ACE_DEFAULT_CDR_BUFSIZE);
		cdr << (ACE_CDR::ULong)CMD_ENTER_GAME;

		//Pack the game name to CDR data and write it to the stream,
		//check if everything sent ok
		//Lab2- Send both GameType and Game Name
		if(!cdr.write_string(itr->first) || !cdr.write_string(itr->second) || writeToOutput(cdr) == -1)
			return -1;

		//Output to the console with some formatting
		//Lab2 - Updated Formatting to display game type and game name
		//if(itr != games.begin())
		cout << "Game Type : " << itr->first << " :: Game Name : " << itr->second;
		//cout << (*itr);
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
	//Lab2 - declare game type
	ACE_CDR::Char* gameName, *playerName ,*gameType;
	//Try to read in both the new player and which game is being joined
	//If we don't have both, there was bad data
	//Lab2 - try to read game type
	if( !(cdr >> gameType) || !(cdr >> gameName) || !(cdr >> playerName) ){
		cout << "Failed connecting to: " << toString(address) << ", Error: " << errorMessage(STATUS_UNEXPECTED_DATA) << endl;;
		return -1;
	}
	//Print out the basic data of who has entered and the dealer for the game entered
	//Lab2 - print game type
	cout << "Dealer: " << dealerName << "  Address: " << toString(address) << ", Player: " << playerName << " joining game " << gameName << " of type " << gameType <<endl;
	return 0;
}
//This method just handles the message output when a player leaves a game
int Player_Svc_Handler::printLeave(ACE_InputCDR &cdr)
{
	//Lab2 - declare game type
	ACE_CDR::Char* gameName, *playerName ,*gameType;
	//Try to read in both the new player and which game is being joined
	//If we don't have both, there was bad data
	//Lab2 - try to read game type
	if( !(cdr >> gameType) || !(cdr >> gameName) || !(cdr >> playerName) ){
		cout << "Failed connecting to: " << toString(address) << ", Error: " << errorMessage(STATUS_UNEXPECTED_DATA) << endl;;
		return -1;
	}
	//Print out the basic data of who has entered and the dealer for the game entered
	//Lab2 - print game type
	cout << "Dealer: " << dealerName << "  Address: " << toString(address) << ", Player: " << playerName << " leaving game " << gameName << " of type " << gameType << endl;
	return 0;
}

int Player_Svc_Handler::printEntry(ACE_InputCDR& cdr)
{
	ACE_CDR::ULong status, numUsers;
	ACE_CDR::Char* gameName;
	//Lab2 - read game type
	ACE_CDR::Char* gameType;

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
	//Lab2 - try to read game type
	if(!(cdr >> numUsers) || !(cdr >> gameType) || !(cdr >> gameName)){
		cout << "Failed connecting to: " << toString(address) << ", Error: " << errorMessage(STATUS_UNEXPECTED_DATA) << endl;
		return -1;
	}

	//We got all the appropriate data, send out the information to our player
	//Lab2 - print gameType along with game name
	cout << "Dealer: " << dealerName << ", Address: " << toString(address) << ", Entering: " << gameName << " of Type : " << gameType;

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
	ACE_CDR::Char* gameName, *gameType;

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
	//Make sure that our data contains the game name and (Lab2) gametype
	else if(!(cdr >> gameType) || !(cdr >> gameName)){
		cout << "Failed connecting to: " << toString(address) << ", Error: " << errorMessage(STATUS_UNEXPECTED_DATA) << endl;
		return -1;
	}
	//Lab2 - added game type to print
	cout << "Dealer: " << dealerName << ", sent score for game (" << gameName << ") of type ("<< gameType <<")"  << endl;

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
		//Lab2 - switch for discarded cards
		case CMD_DISCARD_CARDS:
			gameState = receiveNewCards(cdr);
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
	//Lab2 - declare game type
	ACE_CDR::Char* gameName, *gameType;
	//Read in the game name that this is for
	//Lab2 - read game type
	if(!(cdr >> gameType) || !(cdr >> gameName)){
		cout << "printHand: Did not receive game name. (" << toString(address) << ")" << endl;
		return -1;
	}
	// Check that the received game name is valid
	bool isGame = false;
	//Lab2 - Updated vector
	for(vector<gametype_game_pair>::iterator itr=games.begin(); itr != games.end(); ++itr)
	{
		//We've found the game, can quit searching
		//Lab2 updated *itr to itr->second , compare both gameType and gameName
		if(ACE_OS::strcmp( itr->first, gameType)== 0 && ACE_OS::strcmp( itr->second, gameName)== 0)
		{
			isGame = true;
			break;
		}
	}

	//It's possible that we got to the end of the previous
	//loop without finding the game. In that case, we don't
	//have a record of this game and return an error
	//Lab2 - update to print game type
	if(!isGame){
		cout << "printHand: Client has no record of game: " << gameName << " type : "<< gameType << endl;
		return -1;
	}
	// We've found the game, proceed to print out hands
	//Lab2 - Included theHand as member variable --

	Hand tmpHand;
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
		//Lab2 create variable for card and boolean pair
		card_bool_pair cp;
		cp.first=c;
		//By default card is set to be not discarded.
		cp.second = false;
		tmpHand.add_card(cp);
		//theHand.getHandBoolPair()->push_back(cp);
	}
	//Lab2 - assign reference of tmp variable to member pointer
	theHand = tmpHand;
	cout << "Dealer (" << dealerName << ") dealt following for game (" << gameName << "): " << " of type (" << gameType << "): ";
	cout << theHand << endl;

	//Lab2 Need to add method to discard cards if game type is of draw
	//else if game is of type stud call the method to send score
	if(strcmp(gameType,GAME_TYPE_DRAW.c_str())==0)
	{
		return discardCards(gameType,gameName);
	}
	else
	{
		return sendScore(gameType,gameName);
	}

}

int Player_Svc_Handler::printWinner(ACE_InputCDR &cdr)
{
	//Lab2 declare gameType
	ACE_CDR::Char* gameName, *playerN, *gameType;
	ACE_CDR::ULong numUsers;

	//Make sure that the data received holds both the
	//name of the game and how many players
	//Lab2 read game type
	if(!(cdr >> gameType) || !(cdr >> gameName) || !(cdr >> numUsers)){
		cout << "Failed printing winner with error: Unexpected Data" << " @ address: " << toString(address) << endl;
		return -1;
	}
	//Lab2 print game type
	cout << "Dealer: " << dealerName << " @ address: " << toString(address) << ", game: " << gameName << " of type : " << gameType << endl;
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

//Lab2 method to send score to Dealer
int Player_Svc_Handler::sendScore(const ACE_TCHAR* gameType,const ACE_TCHAR* gameName)
{
	int theScore = getHandRank(theHand);
	string handRankString(getRankString(theHand));
	cout << " (" << handRankString << ") - Score: " << theScore << endl;

	// Build a data stream to send to the server with the score
	ACE_OutputCDR ocdr(ACE_DEFAULT_CDR_BUFSIZE);
	//Appropriate message identifier
	ocdr << (ACE_CDR::ULong)CMD_CARD_SCORE;

	//Add in the name of the game and player score respectively
	//Lab2 - write game type
	if(!(ocdr.write_string(gameType)) || !(ocdr.write_string(gameName)) || !(ocdr << theScore)){
		cout << "Error: sendScore: Could not write the game name or score." << endl;
		return -1;
	}
	if(writeToOutput(ocdr) == -1){
		cout << "Error: sendScore: Could not write out the score to server." << endl;
		return -1;
	}
	return 0;
}
//Lab2 method to receive new cards
int Player_Svc_Handler::receiveNewCards(ACE_InputCDR &cdr)
{
	ACE_CDR::Long status,newCards;
	ACE_CDR::Char rank, suit;
	ACE_CDR::Char* gameName, *gameType;

	//Make sure that our data contains a status
	if (!(cdr >> status)) {
		cout << "receiveNewCards:Received unexpected data from Server , :Error: "
				<< errorMessage(STATUS_UNEXPECTED_DATA) << endl;
		return -1;
	}
	//Make sure that the status is good
	else if (((State) status) != STATUS_OK) {
		cout << "receiveNewCards: Received error from Server"
				<< ", Error: (" << status << ") "
				<< errorMessage((State) status) << endl;
		return -1;
	}
	//Make sure that our data contains the game name and (Lab2) gametype and num of new cards
	else if (!(cdr >> gameType) || !(cdr >> gameName) || !(cdr >> newCards)) {
		cout <<  "receiveNewCards:Received unexpected data from Server , :Error: "
				<< errorMessage(STATUS_UNEXPECTED_DATA) << endl;
		return -1;
	}

	vector<Card> discardedCards;

	for (vector<card_bool_pair>::iterator it =theHand.getHandBoolPair()->begin();it != theHand.getHandBoolPair()->end(); ++it)
	{
		if (it->second)
		{
			discardedCards.push_back(it->first);
		}
	}

	if(newCards!=discardedCards.size())
	{
		cout <<  "receiveNewCards:Received unexpected data from Server , :Error: "
						<< errorMessage(STATUS_UNEXPECTED_DATA) << endl;
		return -1;
	}

	cout << "Dealer: " << dealerName << " sent new cards for game: (" << gameName << ") of type : (" << gameType << ")" << endl;
	copy(discardedCards.begin(),discardedCards.end(),ostream_iterator<Card>(cout," "));
	cout << " cards were replaced with ";
	//Update the new cards received from Dealer

	for (vector<Card>::const_iterator it2 = discardedCards.begin(); it2 != discardedCards.end();++it2)
	{
		Card dCard = *it2;
		Card nCard;
		//Read in the card rank from data stream
		if (!(cdr >> rank)) {
			cout << "Error: printHand: Failed to read the card rank." << endl;
			return -1;
		}
		//Read in the card suit from data stream
		if (!(cdr >> suit)) {
			cout << "Error: printHand: Failed to read the card suit." << endl;
			return -1;
		}
		nCard.c_rank = (Rank) rank;
		nCard.c_suit = (Suit) suit;
		vector<card_bool_pair>::iterator itr = find_if(theHand.getHandBoolPair()->begin(),theHand.getHandBoolPair()->end(),isCard(dCard));
		itr->first = nCard;
		itr->second = false;
		cout << nCard << " ";

	}

	cout << endl;

	cout << "Dealer (" << dealerName << ") dealt new cards following for game (" << gameName << "): " << " of type (" << gameType << "): ";
	cout << theHand << endl;


	//Everything was done successfully , send score to dealer
	return sendScore(gameType,gameName);


}

//Lab2 method to discard cards
int  Player_Svc_Handler::discardCards(const ACE_TCHAR* gameType,const ACE_TCHAR* gameName)
{
	auto_discard(theHand);
	// Build a data stream to send to the server with the score
	ACE_OutputCDR ocdr(ACE_DEFAULT_CDR_BUFSIZE);
	//Appropriate message identifier
	ocdr << (ACE_CDR::ULong)CMD_DISCARD_CARDS;

	vector<Card> discardedCards;

	for(vector<card_bool_pair>::iterator it = theHand.getHandBoolPair()->begin(); it != theHand.getHandBoolPair()->end(); ++it)
	{
		if(it->second)
		{
			discardedCards.push_back(it->first);
		}
	}

	//Add in the type of game , name of the game and number of cards discarded respectively
	if(!(ocdr.write_string(gameType)) || !(ocdr.write_string(gameName)) || !(ocdr << discardedCards.size())){
		cout << "Error: discardCards: Could not write the game type or game name or number of discarded cards." << endl;
		return -1;
	}

	//send discarded cards to Dealer
	for(unsigned int cardNum=0; cardNum<discardedCards.size(); ++cardNum)
	{
		ocdr << (ACE_CDR::Char)discardedCards[cardNum].c_rank;
		ocdr << (ACE_CDR::Char)discardedCards[cardNum].c_suit;
	}

	if(writeToOutput(ocdr) == -1){
		cout << "Error: discardCards: Could not write out the discarded cards to server." << endl;
		return -1;
	}
	return 0;
}
