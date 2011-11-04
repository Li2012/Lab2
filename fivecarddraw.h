// Names: Alex Benjamin
// Email: abenjamin@wustl.edu
//
#ifndef FIVECARDDRAW_H
#define FIVECARDDRAW_H

#include "game.h"
#include "deck.h"
#include "player.h"

//class Game;

#define OUT_OF_CARDS		-200
#define TOO_MANY_PLAYERS	-201

class FiveCardDraw : public Game {
protected:
	size_t dealer;
	Deck discardDeck;
public:
	FiveCardDraw();
	virtual int before_turn(Player &p);
	virtual int turn(Player &p);
	virtual int after_turn(Player &p);
	virtual int before_round();
	virtual int round();
	virtual int after_round();
private:
	int auto_discard(Player &p);
	void auto_discardTP(Hand &cards);
	void auto_discardFOAK(Hand &cards);
	void auto_discardTOAK(Hand &cards);
	void auto_discardPair(Hand &cards);
	void auto_discardNoRank(Hand &cards);
};

#endif FIVECARDDRAW_H