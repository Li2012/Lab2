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
	//Lab2 - maintain the last dealed card in deck
	lastDealedCard = nextCard;
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
	//Lab2 - maintain the last dealed card in deck
	lastDealedCard = nextCard;
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

//Lab2 Method would first find the cards in the range of dealed
//cards , then it swaps the cards with last dealed card on the deck,
//reposition the nextCard iterator to appropriately point to beginning
//of new set of cards added , shuffle
void Deck::addCardsToDeck(vector<CardPair> c)
{
	if(c.size() == 0)
		return;

	for(unsigned int i = 0 ; i < c.size() ; i ++)
	{
		cards.push_back(c[i]);
	}
		//try to find the card in deck in range of dealed cards
		/*cout << "Printing dealed cards" << endl;
		for(vector<CardPair>::iterator iter = cards.begin(); iter!=nextCard; iter++)
				cout << "[" << iter->second << " " << iter->first << "], ";
			cout << endl;

		cout << "Printing discarded cards" << endl;
		for (vector<CardPair>::iterator iter2 = c.begin(); iter2 != c.end();
				iter2++)
			cout << "[" << iter2->second << " " << iter2->first << "], ";
		cout << endl;

		vector<CardPair>::iterator result = find(cards.begin(),nextCard,c[i]);
		if(result == nextCard)
		{
			Card tmp;
			tmp.c_rank = c[i].first;
			tmp.c_suit = c[i].second;
			cout << "Error - Could not find card " << tmp << " in deck of dealed cards" << endl;
			return;
		}

		// if card is found then swap it was lastDealedCard
		swap(result,lastDealedCard);
		nextCard = lastDealedCard;
		--lastDealedCard;
	}*/
	//shuffle
	//random_shuffle(nextCard,cards.end());
}
