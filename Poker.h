// poker.h
// Names: Alex Benjamin
// Email: abenjamin@wustl.edu
//

#ifndef POKER_H
#define POKER_H

#include "Hand.h"
#include "Card.h"
#include <string>
#include <iostream>
using namespace std;

#define NO_RANK			 1
#define PAIR			 2
#define TWO_PAIR		 3
#define THREE_OF_A_KIND  4
#define STRAIGHT		 5
#define FLUSH		     6
#define FULL_HOUSE		 7
#define FOUR_OF_A_KIND	 8
#define STRAIGHT_FLUSH	 9

#define TOO_FEW_ARGS_ERROR		-1
#define BAD_CARD_INDEX_ERROR	-2
#define INVALID_PLAYER_INPUT	-3
#define NULL_POINTER			-4

bool is_alpha(const char * s);
void usageMessage(char * error_msg, char * program_name = "CSE532_Lab1");
bool isFlush(Card c1, Card c2, Card c3, Card c4, Card c5);
bool isStraight(Card c1, Card c2, Card c3, Card c4, Card c5);
bool isStraightFlush(Card c1, Card c2, Card c3, Card c4, Card c5);
bool isFourOfAKind(Card c1, Card c2, Card c3, Card c4, Card c5);
bool isFullHouse(Card c1, Card c2, Card c3, Card c4, Card c5);
bool isThreeOfAKind(Card c1, Card c2, Card c3, Card c4, Card c5);
bool isTwoPair(Card c1, Card c2, Card c3, Card c4, Card c5);
bool isPair(Card c1, Card c2, Card c3, Card c4, Card c5);
string printSuit(Card card);
string printRank(Card card);
bool interpretSuitStr(string suitStr, Card& nextCard);
bool interpretRankStr(string rankStr, Card& nextCard);
string toLowercase(string str);

bool poker_rank(const Hand &first,const Hand &second);
char * getRankString(const Hand &cards);
int getHandRank(const Hand &cards);
bool no_rank_tiebreak(const Hand &first,const Hand &second);
bool pair_tiebreak(const Hand &first,const Hand &second);
bool two_pair_tiebreak(const Hand &first,const Hand &second);
bool three_of_a_kind_tiebreak(const Hand &first,const Hand &second);
bool straight_tiebreak(const Hand &first,const Hand &second);
bool flush_tiebreak(const Hand &first,const Hand &second);
bool full_house_tiebreak(const Hand &first,const Hand &second);
bool four_of_a_kind_tiebreak(const Hand &first,const Hand &second);
bool straight_flush_tiebreak(const Hand &first,const Hand &second);

#endif
