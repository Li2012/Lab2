// Names: Alex Benjamin
// Email: abenjamin@wustl.edu
//
#include "stdafx.h"
#include "fivecarddraw.h"
#include "player.h"
#include "card.h"
#include "poker.h"
#include "deck.h"
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <functional>
using namespace std;

FiveCardDraw::FiveCardDraw() : dealer(0) {
	Card c;
	//Fill up our deck with one of each card in a standard deck
	for(unsigned int suit = Card::club; suit <= Card::spade; ++suit){
		c.c_suit = (Card::suit)suit;
		for(unsigned int rank = Card::two; rank <= Card::ace; ++rank){
			c.c_rank = (Card::rank)rank;
			gameDeck.add_card(c);
		}
	}
	//reset PRNG
	gameDeck.reseed();
	//shuffle deck
	gameDeck.shuffle();
}
void FiveCardDraw::auto_discardFOAK(Hand &cards){
	//Find the card not in the FOAK and get rid of it
	vector<Card> vec1 = cards.getHand();
	if(vec1[0].c_rank == vec1[1].c_rank){
		discardDeck.add_card(cards[0]);
		cards.remove_card(4);
	}
	else{
		discardDeck.add_card(cards[0]);
		cards.remove_card(0);
	}
}
void FiveCardDraw::auto_discardTP(Hand &cards){
	//Find the card not in the two pair and get rid of it
	vector<Card> vec1 = cards.getHand();
	int lp1 = -3, hp1 = -3;
	//Find the two pair ranks from hand
	for(int i = 0; i < 4; ++i){
		if(vec1[i].c_rank == vec1[i+1].c_rank){
			if(lp1 == -3){
				lp1 = vec1[i].c_rank;
				++i;
			}
			else{
				hp1 = vec1[i].c_rank;
				break;
			}
		}
	}

	//find remaining card in hand
	int i = 0;
	for(; i < 5; ++i){
		if(vec1[i].c_rank != lp1 && vec1[i].c_rank != hp1)
			break;
	}
	discardDeck.add_card(cards[i]);
	cards.remove_card(i);
}
void FiveCardDraw::auto_discardTOAK(Hand &cards){
	//Find the two cards not in the TOAK and get rid of them
	vector<Card> vec1 = cards.getHand();
	int rank1;
	//Find the 3oaK value
	for(int i = 0; i < 4; ++i){
		if(vec1[i].c_rank == vec1[i+1].c_rank)
			rank1 = vec1[i].c_rank;
	}

	//find remaining card indexes
	int rem1 = -3, rem2 = -3;
	for(int i = 0; i < 5; ++i){
		if(vec1[i].c_rank != rank1){
			if(rem1 == -3)
				rem1 = i;
			else
				rem2 = i;
		}
	}
	if(rem1 < rem2){
		int swap = rem1;
		rem1 = rem2;
		rem2 = swap;
	}//now rem2 is < rem1. Need this because vector index will change 
	//on erase so we should delete from the highest index to lowest
	//so that indexes will be correct
	discardDeck.add_card(cards[rem1]);
	cards.remove_card(rem1);
	discardDeck.add_card(cards[rem2]);
	cards.remove_card(rem2);
	
}
void FiveCardDraw::auto_discardPair(Hand &cards){
	//Find the three cards not in the pair and get rid of them
	vector<Card> vec1 = cards.getHand();
	int rank1;
	//obtain which is the pair rank
	for(size_t i = 0; i < vec1.size()-1; ++i){
		if(vec1[i].c_rank == vec1[i+1].c_rank){
			rank1 = vec1[i].c_rank;
			break;
		}
	}
	//Collect the remaining three cards from each hand for discard
	vector<int> firstRanks;
	for(size_t i = 0; i < vec1.size(); ++i){
		if(vec1[i].c_rank != rank1){
			firstRanks.push_back(i);
			//store the indexes of the hand which we will delete
		}
	}
	sort(firstRanks.begin(),firstRanks.end());
	//Need this because vector index will change 
	//on erase so we should delete from the highest index to lowest
	//so that indexes will be correct
	
	discardDeck.add_card(cards[firstRanks[2]]);
	cards.remove_card(firstRanks[2]);
	discardDeck.add_card(cards[firstRanks[1]]);
	cards.remove_card(firstRanks[1]);
	discardDeck.add_card(cards[firstRanks[0]]);
	cards.remove_card(firstRanks[0]);

}
void FiveCardDraw::auto_discardNoRank(Hand &cards){
	//Toss all the cards, except if there is an ace, keep that
	vector<Card> vec1 = cards.getHand();
	bool foundAce(false);
	for(size_t i = 0; i < 5; ++i){
		if(vec1[i].c_rank == Card::ace)
			foundAce = true;
	}
	if(foundAce){
		//delete all cards except the ace
		for(size_t i = 0; i < 4; ++i){
			if(cards[0].c_rank == Card::ace){
				discardDeck.add_card(cards[1]);
				cards.remove_card(1);				
			}
			else{
				discardDeck.add_card(cards[0]);
				cards.remove_card(0);
			}
		}
		//all cards but ace deleted now
	}
	else{
		//delete 3 lowest cards
		discardDeck.add_card(cards[0]);
		cards.remove_card(0);
		discardDeck.add_card(cards[0]);
		cards.remove_card(0);
		discardDeck.add_card(cards[0]);
		cards.remove_card(0);
	}
}
int FiveCardDraw::auto_discard(Player &p){
	//
	// The auto_discard method gets the rank of the hand for auto_players
	// and then calls an appropriate helper method which will find the
	// appropriate cards to be discarded depending on the type of the hand.
	//
	cout << "Player " << p.pName << " on auto-play. Current hand: " << getRankString(p.pHand) << endl;
	int rank = getHandRank(p.pHand);
	if(rank == STRAIGHT_FLUSH || rank == FLUSH 
		|| rank == FULL_HOUSE || rank == STRAIGHT)
		cout << "Not discarding any cards." << endl;
	else if(rank == FOUR_OF_A_KIND){
		cout << "Discarding unmatched card in hand." << endl;
		auto_discardFOAK(p.pHand);
	}
	else if(rank == TWO_PAIR){
		cout << "Discarding unmatched card in hand." << endl;
		auto_discardTP(p.pHand);
	}
	else if(rank == THREE_OF_A_KIND){
		cout << "Discarding two unmatched cards in hand." << endl;
		auto_discardTOAK(p.pHand);
	}
	else if(rank == PAIR){
		cout << "Discarding three unmatched cards in hand." << endl;
		auto_discardPair(p.pHand);
	}
	else if(rank == NO_RANK){
		auto_discardNoRank(p.pHand);
	}
	cout << endl;

	return 0;
}
int FiveCardDraw::before_turn(Player &p){
	//
	// The before_turn method lets players discard any of the cards
	// in the hand that has been dealt to them. This is done by entering
	// card indexes one at a time as prompted. When no more cards should
	// be discarded, "none" is entered and the method exits.
	//
	// Appropriate checking is made on inputs to the before_turn method.
	// Given that it is a five card hand, the size of input should only be 1
	// for each card discarded. Also, the index specified must be valid.
	//
	// The before_turn method also checks for auto-players. In the case of
	// an auto-player, the before_turn method simply returns the result
	// of the auto_discard method which handles that case.
	//
	cout << "Name: " << p.pName << "   Hand: " << p.pHand << " (" << getRankString(p.pHand) << ")" << endl;
	if(p.autopilot)
		return auto_discard(p);
	string s = "";
	bool done = false;
	while(true){
		size_t dispSize = p.pHand.size()-1;
		cout << "Which one of " << p.pName << "'s cards would you like to discard? (Specify a hand position 0-" << dispSize << ", or none)" << endl;
		cin >> s;
		if(s.compare("") == 0 || s.empty())
			continue; //if no input, simply prompt again
		string done = "none";
		s = toLowercase(s); //keep the string lowercase so we can recognize "none"
		if(s.compare(done) == 0) //if "none", we are done discarding
			break;
		if(s.length() > 1) //we're only discarding one card at a time, so
			continue;	   //the string should only be one character
		vector<size_t> posVec;
		int pos = atoi(s.substr(0,1).c_str());
		int handSize = static_cast<int>(p.pHand.size());
		if(pos < 0 || pos >= handSize)
		{ //Make sure the index is actually in the hand
			cout << "Not a valid position." << endl;
			continue;
		}
		posVec.push_back(static_cast<size_t>(pos));
		sort(posVec.begin(),posVec.end(),greater<size_t>());
		for(size_t i = 0; i < posVec.size(); ++i){
			discardDeck.add_card(p.pHand[posVec[i]]);
			cout << "Discarded: " << p.pHand[posVec[i]];
			p.pHand.remove_card(posVec[i]);
			cout << "  Current Hand: " << p.pHand << endl;
		}
	}
	return 0;
}
int FiveCardDraw::turn(Player &p){
	//
	// The turn method basically fills out each player's hand to
	// five cards after discards have happened. It draws first
	// from the game deck until that is depleted. Then, it will
	// try to use the discard deck to deal cards (after shuffling it).
	// If the discard deck runs out, there isn't much we can do,
	// so we return an error value
	//
	while(p.pHand.size() < 5){
		if(gameDeck.size() > 0){
			p.pHand << gameDeck;
		}
		else {
			if(discardDeck.size() < 1){
				return OUT_OF_CARDS;
			}
			else {
				discardDeck.shuffle();		
				p.pHand << discardDeck;
			}
		}
	}
	return 0;
}
int FiveCardDraw::after_turn(Player &p){
	//Display the state of a player's hand
	cout << "(after_turn) Name: " << p.pName << "   Hand: " << p.pHand << " (" << getRankString(p.pHand) << ")" << endl;
	return 0;
}
int FiveCardDraw::before_round(){
	//
	// The before_round method is responsible for dealing out five
	// cards to each player. A rudimentary check is done at first to
	// make sure we don't have more players than cards will allow for.
	// If we do, we return an error value as the game cannot continue.
	//
	// Dealing is done in an order starting just past the dealer. Then,
	// the method calls the before_turn method for each player in the same
	// order.
	//
	gameDeck.shuffle();
	if(gameDeck.size() < playerVec.size()*5)
		return TOO_MANY_PLAYERS;

	unsigned int index = dealer+1;
	for(size_t i = 0; i < 5*playerVec.size(); ++i){
		if(index >= playerVec.size())
			index = 0;
		Player * p = playerVec[index++];
		(*p).pHand << gameDeck;

	}

	index = dealer+1;
	int count = playerVec.size();
	while(count > 0){
		if(index >= playerVec.size())
			index = 0;
		Player * p = playerVec[index++];
		before_turn(*p); //check this	
		--count;
	}
	return 0;
}

int FiveCardDraw::round(){
	//
	// The round method calls the turn and after_turn methods for each
	// player in order starting just past the dealer. If an error is returned
	// from one of those methods, it returns the same error.
	//
	unsigned int index = dealer+1;
	int count = playerVec.size();
	int error = 0;
	while(count > 0){
		if(index >= playerVec.size())
			index = 0;
		Player * p = playerVec[index++];
		error = turn(*p);		
		if(error != 0)
			return error;
		after_turn(*p);
		--count;
	}
	
	return 0;
}

int FiveCardDraw::after_round(){
	//
	// The after_round method first creates a copy of the vector of pointers
	// to players. This vector is sorted based upon hand rank, and the winner
	// and losers of the round are either given a win or loss on their record.
	//
	// We do some additional work in order to store highest/lowest hands to 
	// calculate the probabilities for auto-players leaving or staying in the game.
	// 
	// If an auto-player has the lowest hand, he has only a 10% chance of staying.
	// If he has the highest hand, he has a 90% chance of staying. Otherwise, he has
	// a 50% chance.
	//
	// Next, we put all of the cards from the discard deck and the players' hands back
	// into the game deck for the next round.
	// 
	// Finally, we see if any players want to leave or join the game.
	//
	vector<Player *> playerVecCopy;	
	for(size_t index = 0; index < playerVec.size(); ++index){
		Player * p = playerVec[index];
		playerVecCopy.push_back(p);
	}
	sort(playerVecCopy.begin(),playerVecCopy.end(),poker_rank_pointer);
	vector<string> winners,losers;
	
	//Collect the highest hands for later
	for(size_t index = 0; index < playerVecCopy.size(); ++index){
		Player * p = playerVecCopy[index];
		if(poker_rank_pointer(p,playerVecCopy[0]) 
			== poker_rank_pointer(playerVecCopy[0],p)){
			(*p).gamesWon++;
			winners.push_back((*p).pName);
		}
		else
			(*p).gamesLost++;
		cout << "(after_round) Name: " << (*p).pName << "  Wins: " << (*p).gamesWon << "  Losses: " << (*p).gamesLost << "  Hand: " << (*p).pHand << " (" << getRankString((*p).pHand) << ")" << endl;
	}


	//Collect the lowest hand for later
	for(size_t index = 0; index < playerVecCopy.size(); ++index){
		Player * p = playerVecCopy[index];
		if(poker_rank_pointer(p,playerVecCopy[playerVecCopy.size()-1]) 
			== poker_rank_pointer(playerVecCopy[playerVecCopy.size()-1],p)){
				losers.push_back((*p).pName);
		}
	}

	//Put all of the cards from the discard deck back into game deck for next round
	while(discardDeck.size() > 0){
		gameDeck.add_card(discardDeck[0]);
		discardDeck.eraseCard(0);
	}
	//Put all the cards from player's hands back into game deck for next round
	for(size_t index = 0; index < playerVec.size(); ++index){
		Player * p = playerVec[index];
		while((*p).pHand.size() > 0){
			gameDeck.add_card((*p).pHand[0]);
			(*p).pHand.remove_card(0);
		}
	}

	//check on autoplay players for leaving
	vector<size_t> leavingPlayers;
	//we'll store leaving players and delete at the end
	//so as not to invalidate our iterator
	for(size_t index = 0; index < playerVec.size(); ++index){
		Player * p = playerVec[index];
		if((*p).autopilot){
			int probability = 50;
			size_t sizeSearch = winners.size() > losers.size() ? winners.size() : losers.size();
			for(size_t i = 0; i < sizeSearch; ++i){
				if(i < winners.size() && winners[i].compare((*p).pName) == 0){
					cout << "Auto-Player " << (*p).pName << " was a winner last round. 90% chance of staying in game." << endl;
					probability = 90;
				}
				else if(i < losers.size() && losers[i].compare((*p).pName) == 0){
					cout << "Auto-Player " << (*p).pName << " was a loser last round. 10% chance of staying in game." << endl;
					probability = 10;
				}
			}
			if( (rand() % 100) >= probability ){
				//player should leave game
				leavingPlayers.push_back(index);
			}
		}
	}

	sort(leavingPlayers.begin(),leavingPlayers.end(),greater<size_t>());
	//Need this because playerVec indexes will change 
	//on erase so we should delete from the highest index to lowest
	//so that indexes will be correct

	//go through and delete autoplay players that should be
	for(size_t index = 0; index < leavingPlayers.size(); ++index){
		size_t position = leavingPlayers[index];
		Player * p = playerVec[position];
		cout << "Auto-Player " << (*p).pName << " has left the game." << endl;
		ofstream ofs((*p).pName.c_str());
		ofs << *p;
		ofs.close();
		delete p;
		playerVec.erase(playerVec.begin()+position);
	}

	string in;
	while(true){
		cout << "Would any players like to leave the game? (enter a name or no)" << endl;
		cin >> in;
		if(in.empty())
			continue;
		in = toLowercase(in);
		if(in == "no")
			break; 
		Player * p;
		size_t index = 0;
		for(; index < playerVec.size(); ++index){
			p = playerVec[index];
			if(in.compare((*p).pName) == 0)
				break;
		}
		if(index >= playerVec.size())
			continue;
		else {
			cout << (*p).pName << " has left the game." << endl;
			ofstream ofs((*p).pName.c_str());
			ofs << *p;
			ofs.close();
			delete p;
			playerVec.erase(playerVec.begin()+index);
		}
	}

	while(true){
		cout << "Would any players like to join the game? (enter a name or no)" << endl;
		cin >> in;
		if(in.empty())
			continue;
		in = toLowercase(in);
		if(in == "no")
			break;
		try{
			(*g).add_player(in);
			cout << in << " has joined the game." << endl;
		}
		catch(int &exception){
			if(exception == ALREADY_PLAYING)
				cout << in << " is already in the game." << endl;
			else if(exception == INVALID_PLAYER_INPUT)
				cout << in << " is not a valid name. Must be alphabetic with the optional auto-play flag (*) at the end" << endl;
		}
	}
	++dealer;
	if(dealer >= playerVec.size())
		dealer = 0;
	return 0;
}