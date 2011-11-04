// card.cpp
// Names: Alex Benjamin
// Email: abenjamin@wustl.edu
//

#include "Card.h"
#include "Poker.h"

bool Card::operator< (const Card &p) const {
  return (c_rank < p.c_rank) || ((c_rank == p.c_rank) && (c_suit < p.c_suit));
}

bool Card::operator== (const Card &otherCard) const{
	return c_rank == otherCard.c_rank && c_suit == otherCard.c_suit;
}

string printCRank(Card card){
  string out;
  if(card.c_rank == two)
    out = "2";
  else if(card.c_rank == three)
    out = "3";
  else if(card.c_rank == four)
    out = "4";
  else if(card.c_rank == five)
    out = "5";
  else if(card.c_rank == six)
    out = "6";
  else if(card.c_rank == seven)
    out = "7";
  else if(card.c_rank == eight)
    out = "8";
  else if(card.c_rank == nine)
    out = "9";
  else if(card.c_rank == ten)
    out = "10";
  else if(card.c_rank == jack)
    out = "J";
  else if(card.c_rank == queen)
    out = "Q";
  else if(card.c_rank == king)
    out = "K";
  else if(card.c_rank == ace)
    out = "A";
  else {
    cout << "Error" << endl;
    out = "Error";
  }
  return out;
}

string printCSuit(Card card){
  string out;
  if(card.c_suit == heart)
    out = "H";
  else if(card.c_suit == spade)
    out = "S";
  else if(card.c_suit == diamond)
    out = "D";
  else if(card.c_suit == club)
    out = "C";
  else{
    cout << "Error" << endl;
    out = "Error";
  }
  return out;
}

ostream & operator<< (ostream & o, const Card & c) {
  o << printCRank(c) << printCSuit(c);
	return o;
}
