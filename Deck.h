// File: Deck.h
// 10/2011
// abenjamin@wustl.edu - Alex Benjamin
// linfeixb27@gmail.com - Yinfan Li
// mahesh.mahadevan@gmail.com - Mahesh M.

#ifndef DECK_H
#define DECK_H

#include "Card.h"
#include <vector>
#include <ostream>
#include <iostream>
using namespace std;

typedef pair<Rank,Suit> CardPair;

class Deck
{
public:
	Deck();
	~Deck(){}
	void shuffle();
	pair<Rank, Suit> dealCard();
	void dealCard(Rank& rank, Suit& suit);
	void print(ostream& os);
	//Lab2 - Method to add cards back into non- dealed cards
	void addCardsToDeck(vector<CardPair> c);
private:
	vector<CardPair> cards;
	vector<CardPair>::iterator nextCard;

};

#endif
