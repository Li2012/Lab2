// File: Discard.h
//Lab2
// 11/2011
// abenjamin@wustl.edu - Alex Benjamin
// linfeixb27@gmail.com - Yinfan Li
// mahesh.mahadevan@gmail.com - Mahesh M.

#include "Hand.h"
using namespace std;

#ifndef DISCARD_H_
#define DISCARD_H_

int auto_discard(Hand &cards);
void auto_discardTP(Hand &cards);
void auto_discardFOAK(Hand &cards);
void auto_discardTOAK(Hand &cards);
void auto_discardPair(Hand &cards);
void auto_discardNoRank(Hand &cards);


#endif /* DISCARD_H_ */
