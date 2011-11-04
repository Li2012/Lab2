// poker.cpp
// Names: Alex Benjamin
// Email: abenjamin@wustl.edu
//

#include "Poker.h"
#include "Hand.h"
#include "Card.h"
#include <cstring>
#include <string>
#include <iostream>
#include <algorithm>
using namespace std;

void debug(string s){
	bool DEBUG = false;
	if(DEBUG)
		cout << s << endl;
}

//true if (and only if) the first hand object ranks higher than the second hand
bool poker_rank(const Hand &first, const Hand &second){
	debug("Entered poker_rank");
	int firstRank = getHandRank(first);
	int secondRank = getHandRank(second);
	if(firstRank == secondRank){ //same poker rank type (both pair, both flush, etc.)
		switch(firstRank){
			case NO_RANK:
				return no_rank_tiebreak(first,second);break;
			case PAIR:
				return pair_tiebreak(first,second);break;
			case TWO_PAIR:
				return two_pair_tiebreak(first,second);break;
			case THREE_OF_A_KIND:
				return three_of_a_kind_tiebreak(first,second);break;
			case STRAIGHT:
				return straight_tiebreak(first,second);break;
			case FLUSH:
				return flush_tiebreak(first,second);break;
			case FULL_HOUSE:
				return full_house_tiebreak(first,second);break;
			case FOUR_OF_A_KIND:
				return four_of_a_kind_tiebreak(first,second);break;
			case STRAIGHT_FLUSH:
				return straight_flush_tiebreak(first,second);break;
		}
	}	
	//both have a distinct poker rank type
	return firstRank > secondRank;
}
char * getRankString(const Hand &cards){
  string msg;
	int rank = getHandRank(cards);
	if(rank == STRAIGHT_FLUSH)
		msg = "Straight Flush";
	else if(rank == FOUR_OF_A_KIND)
		msg = "Four of a Kind";
	else if(rank == FULL_HOUSE)
		msg = "Full House";
	else if(rank == FLUSH)
		msg = "Flush";
	else if(rank == STRAIGHT)
		msg = "Straight";
	else if(rank == THREE_OF_A_KIND)
		msg = "Three of a Kind";
	else if(rank == TWO_PAIR)
		msg = "Two Pair";
	else if(rank == PAIR)
		msg = "Pair";
	else//(rank == NO_RANK)
		msg = "High Card (no rank)";
	return (char*)msg.c_str();
}
int getHandRank(const Hand &cards){
	vector<Card> vec = cards.getHand();
	Card c1 = vec[0],c2 = vec[1],c3 = vec[2],c4 = vec[3],c5 = vec[4];
	int result;
	//Takes a hand of cards and checks the rank
	if(isStraightFlush(c1,c2,c3,c4,c5))
		result = STRAIGHT_FLUSH;
	else if(isFourOfAKind(c1,c2,c3,c4,c5))
		result = FOUR_OF_A_KIND;
	else if(isFullHouse(c1,c2,c3,c4,c5))
		result = FULL_HOUSE;
	else if(isFlush(c1,c2,c3,c4,c5))
		result = FLUSH;
	else if(isStraight(c1,c2,c3,c4,c5))
		result = STRAIGHT;
	else if(isThreeOfAKind(c1,c2,c3,c4,c5))
		result = THREE_OF_A_KIND;
	else if(isTwoPair(c1,c2,c3,c4,c5))
		result = TWO_PAIR;
	else if(isPair(c1,c2,c3,c4,c5))
		result = PAIR;
	else
		result = NO_RANK;
	return result;
}
bool no_rank_tiebreak(const Hand &first,const Hand &second){
	debug("Entered no_rank_tiebreak pointer");
	vector<Card> vec1 = first.getHand(), vec2 = second.getHand();
	for(int i = 4; i >= 0; --i){
		if(vec1[i] < vec2[i])
			return false;
		else if(vec2[i] < vec1[i])
			return true;
	}
	return false;
}
bool pair_tiebreak(const Hand &first,const Hand &second){
	debug("Entered pair_tiebreak pointer");
	vector<Card> vec1 = first.getHand(), vec2 = second.getHand();
	int rank1, rank2;
	//obtain which is the pair rank in each hand
	for(size_t i = 0; i < vec1.size()-1; ++i){
		if(vec1[i].c_rank == vec1[i+1].c_rank){
			rank1 = vec1[i].c_rank;
			break;
		}
	}
	for(size_t i = 0; i < vec2.size()-1; ++i){
		if(vec2[i].c_rank == vec2[i+1].c_rank){
			rank2 = vec2[i].c_rank;
			break;
		}
	}
	//Pair card ranks are not the same - return true if first pair is greater
	if(rank1 != rank2)
		return rank1 > rank2;

	//Collect the remaining three cards from each hand for comparison
	vector<int> firstRanks,secondRanks;
	for(size_t i = 0; i < vec1.size(); ++i){
		if(vec1[i].c_rank != rank1){
			firstRanks.push_back(vec1[i].c_rank);
		}
	}
	for(size_t i = 0; i < vec2.size(); ++i){
		if(vec2[i].c_rank != rank2){
			secondRanks.push_back(vec2[i].c_rank);
		}
	}
	sort(firstRanks.begin(),firstRanks.end());
	sort(secondRanks.begin(),secondRanks.end());

	for(int i = 2; i >= 0; --i){
		//Compare remaining cards
		if(firstRanks[i] != secondRanks[i])
		{
			if(firstRanks[i] > secondRanks[i])
				return true;
			else
				return false;
		}
	}
	return false;
}

bool two_pair_tiebreak(const Hand &first,const Hand &second){
	debug("Entered two_pair_tiebreak pointer");
	vector<Card> vec1 = first.getHand(), vec2 = second.getHand();
	int lp1 = -3, hp1 = -3, lp2 = -3, hp2 = -3;
	//Find the two pair ranks from first hand
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

	//Find the two pair ranks from second hand
	for(int i = 0; i < 4; ++i){
		if(vec2[i].c_rank == vec2[i+1].c_rank){
			if(lp2 == -3){
				lp2 = vec2[i].c_rank;
				i++;
			}
			else{
				hp2 = vec2[i].c_rank;
				break;
			}
		}
	}
	if(lp1 > hp1){
		//lp1 should be the smaller, swap if it isn't
		int swap = lp1;
		lp1 = hp1;
		hp1 = swap;
	}
	if(lp2 > hp2){
		//lp2 should be the smaller, swap if it isn't
		int swap = lp2;
		lp2 = hp2;
		hp2 = swap;
	}

	//compare highest pair from each hand
	if(hp1 != hp2){
		return hp1 > hp2;
	}
	//compare lower pair from each hand
	else if(lp1 != lp2){
		return lp1 > lp2;
	}

	//both pairs are the same, find remaining card from each hand
	int remaining1 = -1, remaining2 = -1;
	for(int i = 0; i < 5; ++i){
		if(vec1[i].c_rank != lp1 && vec1[i].c_rank != hp1)
			remaining1 = vec1[i].c_rank;
		if(vec2[i].c_rank != lp2 && vec2[i].c_rank != hp2)
			remaining2 = vec2[i].c_rank;
	}

	//compare remaining cards
	if(remaining1 != remaining2){
		return remaining1 > remaining2;
	}

	//hands have the same poker rank
	return false;
}
bool three_of_a_kind_tiebreak(const Hand &first,const Hand &second){
	debug("Entered TOAK_tiebreak pointer");
	vector<Card> vec1 = first.getHand(), vec2 = second.getHand();
	int rank1, rank2; //3oaK values from each hand
	//Find the 3oaK values from each hand
	for(int i = 0; i < 4; ++i){
		if(vec1[i].c_rank == vec1[i+1].c_rank)
			rank1 = vec1[i].c_rank;
		if(vec2[i].c_rank == vec2[i+1].c_rank)
			rank2 = vec2[i].c_rank;
	}

	//compare the 3oaK values
	if(rank1 != rank2)
		return rank1 > rank2;

	//3oaK values same, find remaining cards
	vector<int> firstRem,secondRem;
	for(int i = 0; i < 5; ++i){
		if(vec1[i].c_rank != rank1)
			firstRem.push_back(vec1[i].c_rank);
		if(vec2[i].c_rank != rank2)
			secondRem.push_back(vec2[i].c_rank);
	}

	sort(firstRem.begin(),firstRem.end());
	sort(secondRem.begin(),secondRem.end());

	for(int i = 1; i >= 0; --i){
		//Compare remaining cards
		if(firstRem[i] != secondRem[i])
		{
			if(firstRem[i] > secondRem[i])
				return true;
			else
				return false;
		}
	}

	return false;
}
bool straight_tiebreak(const Hand &first,const Hand &second){
	debug("Entered straight_tiebreak pointer");
	vector<Card> vec1 = first.getHand(), vec2 = second.getHand();
	//For a straight, we just need to compare the last card of each hand
	return vec1[4].c_rank > vec2[4].c_rank;
}
bool flush_tiebreak(const Hand &first,const Hand &second){
	debug("Entered flush_tiebreak pointer");
	vector<Card> vec1 = first.getHand(), vec2 = second.getHand();
	//Start at the end of each hand, and compare card values
	for(int i = 4; i >= 0; --i){
		if(vec1[i].c_rank != vec2[i].c_rank)
		{
			if(vec1[i].c_rank > vec2[i].c_rank)
				return true;
			else
				return false;
		}
	}
	return false;
}
bool full_house_tiebreak(const Hand &first,const Hand &second){
	debug("Entered full_houe_tiebreak pointer");
	vector<Card> vec1 = first.getHand(), vec2 = second.getHand();
	int three1 = -1, two1 = -1, three2 = -1, two2 = -1;
	
	//Find the three and two combinations in first hand
	if(vec1[0].c_rank == vec1[1].c_rank && vec1[1].c_rank == vec1[2].c_rank){
		three1 = vec1[0].c_rank;
		two1 = vec1[4].c_rank;
	}
	else{
		three1 = vec1[4].c_rank;
		two1 = vec1[0].c_rank;
	}
	//Find the three and two combinations in second hand
	if(vec2[0].c_rank == vec2[1].c_rank && vec2[1].c_rank == vec2[2].c_rank){
		three2 = vec2[0].c_rank;
		two2 = vec2[4].c_rank;
	}
	else{
		three2 = vec2[4].c_rank;
		two2 = vec2[0].c_rank;
	}

	if(three1 != three2) //Compare three of a kind in the hands
		return three1 > three2;
	else if(two1 != two2) //Compare two of a kind in the hands
		return two1 > two2;

	//Hands have same poker rank
	return false;
}
bool four_of_a_kind_tiebreak(const Hand &first,const Hand &second){
	debug("Entered FOAK_tiebreak pointer");
	vector<Card> vec1 = first.getHand(), vec2 = second.getHand();
	int rank1 = vec1[2].c_rank; //guaranteed to be part of the 4oaK
	int rank2 = vec2[2].c_rank; //guaranteed to be part of the 4oaK
	//Compare the FoaK card ranks
	if(rank1 != rank2)
		return rank1 > rank2;

	//FoaK card ranks are equal, remaining card will be on either front 
	//or end of each hand (because they are sorted)
	int other1 = (vec1[0].c_rank == vec1[1].c_rank) ? vec1[4].c_rank : vec1[0].c_rank;
	int other2 = (vec2[0].c_rank == vec2[1].c_rank) ? vec2[4].c_rank : vec2[0].c_rank;

	return other1 > other2;
}
bool straight_flush_tiebreak(const Hand &first,const Hand &second){
	debug("Entered straight_flush_tiebreak pointer");
	vector<Card> vec1 = first.getHand(), vec2 = second.getHand();
	//For straight flush, just need to check the last card
	return vec1[4].c_rank > vec2[4].c_rank;
}

bool isFlush(Card c1, Card c2, Card c3, Card c4, Card c5){
	if(c1.c_suit == c2.c_suit && c2.c_suit == c3.c_suit && c3.c_suit == c4.c_suit && c4.c_suit == c5.c_suit)
		return true;
	return false;
}
bool isStraight(Card c1, Card c2, Card c3, Card c4, Card c5){
	for(int i = 0; i < 8; ++i){
		if(c1.c_rank == i && c2.c_rank == i+1 && c3.c_rank == i+2 && c4.c_rank == i+3 && c5.c_rank == i+4)
			return true;
	}
	if(c1.c_rank == ace && c2.c_rank == two && c3.c_rank == three && c4.c_rank == four && c5.c_rank == five)
			return true;
	return false;
}
bool isStraightFlush(Card c1, Card c2, Card c3, Card c4, Card c5){
	if(isFlush(c1,c2,c3,c4,c5) && isStraight(c1,c2,c3,c4,c5))
		return true;
	return false;
}
bool isFourOfAKind(Card c1, Card c2, Card c3, Card c4, Card c5){
	int ranks[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	ranks[c1.c_rank]--;
	ranks[c2.c_rank]--;
	ranks[c3.c_rank]--;
	ranks[c4.c_rank]--;
	ranks[c5.c_rank]--;
	for(int i = 0; i < 13; ++i){
		if(ranks[i] == -4)
			return true;
	}
	return false;
}
bool isFullHouse(Card c1, Card c2, Card c3, Card c4, Card c5){
	int ranks[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	ranks[c1.c_rank]--;
	ranks[c2.c_rank]--;
	ranks[c3.c_rank]--;
	ranks[c4.c_rank]--;
	ranks[c5.c_rank]--;
	bool three = false, two = false;
	for(int i = 0; i < 13; ++i){
		if(ranks[i] == -3)
			three = true;
		if(ranks[i] == -2)
			two = true;
	}
	return three && two;
}
bool isThreeOfAKind(Card c1, Card c2, Card c3, Card c4, Card c5){
	int ranks[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	ranks[c1.c_rank]--;
	ranks[c2.c_rank]--;
	ranks[c3.c_rank]--;
	ranks[c4.c_rank]--;
	ranks[c5.c_rank]--;
	for(int i = 0; i < 13; ++i){
		if(ranks[i] == -3)
			return true;
	}
	return false;
}
bool isTwoPair(Card c1, Card c2, Card c3, Card c4, Card c5){
	int ranks[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	ranks[c1.c_rank]--;
	ranks[c2.c_rank]--;
	ranks[c3.c_rank]--;
	ranks[c4.c_rank]--;
	ranks[c5.c_rank]--;
	bool firstTwo = false, secondTwo = false;
	for(int i = 0; i < 13; ++i){
		if(ranks[i] == -2 && !firstTwo)
			firstTwo = true;
		else if(ranks[i] == -2)
			secondTwo = true;
	}
	return firstTwo && secondTwo;
}
bool isPair(Card c1, Card c2, Card c3, Card c4, Card c5){
	int ranks[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
	ranks[c1.c_rank] = -1;
	ranks[c2.c_rank] = -1;
	ranks[c3.c_rank] = -1;
	ranks[c4.c_rank] = -1;
	ranks[c5.c_rank] = -1;
	int different = 0;
	for(int i = 0; i < 13; ++i)
		if(ranks[i] == -1)
			++different;
	return different <= 4;
}
bool is_alphabet(const char s){
	if(s < 97 || s > 122)
		return false;
	else
		return true;
}
bool is_alpha(const char * s){
	for(size_t i = 0; i < strlen(s)-1; ++i){
		if(!is_alphabet(s[i]))
			return false;
	}
	if(!is_alphabet(s[strlen(s)-1]) && s[strlen(s)-1] != '*')
		return false;

	return true;

}
string toLowercase(string str) {
	for (size_t i = 0; i < str.length(); ++i)
		str[i] = tolower(str[i]);
	return str;
}
bool interpretRankStr(string rankStr, Card& nextCard){
	if(rankStr.compare("2") == 0)
		nextCard.c_rank = two;
	else if(rankStr.compare("3") == 0)
		nextCard.c_rank = three;
	else if(rankStr.compare("4") == 0)
		nextCard.c_rank = four;
	else if(rankStr.compare("5") == 0)
		nextCard.c_rank = five;
	else if(rankStr.compare("6") == 0)
		nextCard.c_rank = six;
	else if(rankStr.compare("7") == 0)
		nextCard.c_rank = seven;
	else if(rankStr.compare("8") == 0)
		nextCard.c_rank = eight;
	else if(rankStr.compare("9") == 0)
		nextCard.c_rank = nine;
	else if(rankStr.compare("10") == 0)
		nextCard.c_rank = ten;
	else if(rankStr.compare("j") == 0)
		nextCard.c_rank = jack;
	else if(rankStr.compare("q") == 0)
		nextCard.c_rank = queen;
	else if(rankStr.compare("k") == 0)
		nextCard.c_rank = king;
	else if(rankStr.compare("a") == 0)
		nextCard.c_rank = ace;
	else
		return false;
	return true;
}
bool interpretSuitStr(string suitStr, Card& nextCard){
	if(suitStr.compare("c") == 0)
		nextCard.c_suit = club;
	else if(suitStr.compare("d") == 0)
		nextCard.c_suit = diamond;
	else if(suitStr.compare("h") == 0)
		nextCard.c_suit = heart;
	else if(suitStr.compare("s") == 0)
		nextCard.c_suit = spade;
	else
		return false;
	return true;
}
/*
string printRank(Card card){
	string out;
	if(card.c_rank == card.two)
		out = "2";
	else if(card.c_rank == card.three)
		out = "3";
	else if(card.c_rank == card.four)
		out = "4";
	else if(card.c_rank == card.five)
		out = "5";
	else if(card.c_rank == card.six)
		out = "6";
	else if(card.c_rank == card.seven)
		out = "7";
	else if(card.c_rank == card.eight)
		out = "8";
	else if(card.c_rank == card.nine)
		out = "9";
	else if(card.c_rank == card.ten)
		out = "10";
	else if(card.c_rank == card.jack)
		out = "J";
	else if(card.c_rank == card.queen)
		out = "Q";
	else if(card.c_rank == card.king)
		out = "K";
	else if(card.c_rank == card.ace)
		out = "A";
	else {
		cout << "Error" << endl;
		out = "Error";
	}
	return out;
}
string printSuit(Card card){
	string out;
	if(card.c_suit == card.heart)
		out = "H";
	else if(card.c_suit == card.spade)
		out = "S";
	else if(card.c_suit == card.diamond)
		out = "D";
	else if(card.c_suit == card.club)
		out = "C";
	else{
		cout << "Error" << endl;
		out = "Error";
	}
	return out;
}
*/
string printRank(Card card){
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

string printSuit(Card card){
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



