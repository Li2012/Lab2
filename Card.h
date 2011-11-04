// File: Card.h
// 10/2011
// abenjamin@wustl.edu - Alex Benjamin
// linfeixb27@gmail.com - Yinfan Li
// mahesh.mahadevan@gmail.com - Mahesh M.

#ifndef CARD_H
#define CARD_H

#include <iostream>
#include <ostream>
using namespace std;

enum Suit {club/*=1*/, diamond, heart, spade};
enum Rank {two/*=2*/, three, four, five, six, seven, eight, nine, ten, jack, queen, king, ace};

class Card
{
public:
	bool operator< (const Card &) const;
	bool operator== (const Card &) const;
	Suit c_suit;
	Rank c_rank;
};

ostream & operator<< (ostream & o, const Card & c);

const unsigned int HAND_SIZE = 5;

/*
inline Rank operator++(Rank&rs, int) { 
	return rs = (Rank)(rs+1); 
}
inline Suit operator++(Suit&rs, int) { 
	return rs = (Suit)(rs+1); 
}
*/

#endif
