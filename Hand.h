// hand.h
// Names: Alex Benjamin
// Email: abenjamin@wustl.edu
//

#ifndef HAND_H
#define HAND_H

#include <vector>
#include <ostream>
#include <iostream>
#include <string>
#include "Card.h"
using namespace std;

//Lab2 - define card & bool pair
typedef bool isDiscarded;
typedef pair<Card,isDiscarded> card_bool_pair;

class Hand {
public:
	Hand();
	Hand(const Hand &);
	//default constructor is fine since we use STL container
	Hand & operator= (const Hand &);
	//Lab2 - update return value to card_bool pair
	//Card operator[] (const size_t) const;
	card_bool_pair operator[] (const size_t) const;
	bool operator== (const Hand &) const;
	bool operator< (const Hand &);
	int remove_card(const size_t);
	//Lab2 - update to card_bool_pair
	//int add_card(Card c);
	int add_card(card_bool_pair c);
	string asString() const;
	size_t size() const;
	//Lab2 new method to get card bool pair
	vector<card_bool_pair>* getHandBoolPair();
	vector<Card> getHand() const;
	//vector<card_bool_pair> getHand() const;
	friend ostream & operator<< (ostream &, const Hand &);
private:
	//Lab2
	//vector<Card> handVec;
	vector<card_bool_pair> handVec;

};

ostream & operator<< (ostream &, const Hand &);


#endif
