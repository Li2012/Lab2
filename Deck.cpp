// File: Deck.cpp
// 10/2011
// abenjamin@wustl.edu - Alex Benjamin
// linfeixb27@gmail.com - Yinfan Li
// mahesh.mahadevan@gmail.com - Mahesh M.

#include "Deck.h"
#include "Card.h"
#include "Lib.h"
#include <algorithm>
using namespace std;

//Make space in the deck container and then put in one
//of each card. Afterward, shuffle
Deck::Deck()
{
	cards.reserve(52);
	for(Suit suit = club; suit <= spade; suit=(Suit)(suit+1))
	  for(Rank rank = two; rank <= ace; rank=(Rank)(rank+1))
			cards.push_back(CardPair(rank, suit));
	shuffle();
}

//Advances the deck iterator once without actually putting
//the card anywhere. Basically acts as a 'burn card'
CardPair Deck::dealCard()
{
	if(nextCard == cards.end()){
		cout << "ERROR: Tried to deal when deck was out of cards." << endl;
		//This shouldn't ever happen!
		return *nextCard;
	}
	CardPair card = *nextCard;
	++nextCard;
	return card;
}

//Puts the enum values for Rank/Suit into variables
//passed by reference and advances the deck iterator
void Deck::dealCard(Rank& rank, Suit& suit)
{
	if(nextCard == cards.end()){
		cout << "ERROR: Tried to deal when deck was out of cards." << endl;
		return;
	}
	rank = nextCard->first;
	suit = nextCard->second;
	++nextCard;
}

//Randomly shuffles the deck container using an STL algorithm
//and resets the deck iterator
void Deck::shuffle()
{
	random_shuffle(cards.begin(), cards.end());
	nextCard = cards.begin();
}

//Prints out a formatted card string
void Deck::print(ostream& os)
{
	for(vector<CardPair>::iterator iter = cards.begin(); iter!=cards.end(); iter++)
		os << "[" << iter->second << " " << iter->first << "], ";
	os << endl;

}

//Lab2 - try to find the card in deck in range of dealed cards
// if found erase it and push_back to the end of the cards vector ,
// once all discarded cards are pushed , shuffle only the cards that
// are not yet dealed to the PLayers
void Deck::addCardsToDeck(vector<CardPair> c)
{
	if(c.size() == 0)
		return;
	vector<CardPair>::difference_type pos = nextCard - cards.begin();

	for(unsigned int i = 0 ; i < c.size() ; i ++)
	{

		vector<CardPair>::iterator result = find(cards.begin(),nextCard,c[i]);
		if(result == nextCard)
		{
			Card tmp;
			tmp.c_rank = c[i].first;
			tmp.c_suit = c[i].second;
			//Should not happen
			cout << "Error - Could not find card " << tmp << " in deck of dealed cards" << endl;

			return;
		}

		// if card is found then erase and push_back
		cards.erase(result);
		cards.push_back(c[i]);
		// reinitialize nextcard as push back might have invalidated the pointers
		nextCard = cards.begin() + pos-1 - i ;

	}
	//Shuffle the deck of cards not dealed to Players yet
	random_shuffle(nextCard,cards.end());

}
