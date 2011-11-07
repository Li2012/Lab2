// File: Discard.cpp
//Lab2
// 11/2011
// abenjamin@wustl.edu - Alex Benjamin
// linfeixb27@gmail.com - Yinfan Li
// mahesh.mahadevan@gmail.com - Mahesh M.

#include <iostream>
#include <iterator>
#include "Discard.h"
#include "Poker.h"
#include "Card.h"
#include <algorithm>
#include <functional>
#include <numeric>
using namespace std;

struct isSuit:  unary_function<Card,bool>
{
	Suit s;
	isSuit(Suit val): s(val) {}
	bool operator()(const Card& x) const
	{
		return x.c_suit == s;
	}
};

struct isRank : unary_function<Card,bool>
{
	Rank r;
	isRank(Rank val) : r(val) {}
	bool operator()(const Card& x) const
	{
		return x.c_rank == r;
	}

};

struct isCard
{
	Card c;
	isCard(const Card& val) : c(val) {}
	bool operator()(const card_bool_pair& x) const
	{
		return x.first == c;
	}

};

Rank getRank (Card c) {return c.c_rank;}

Suit getSuit (Card c) {return c.c_suit;}

struct minusRank : binary_function<Rank,Rank,int>
{
	int operator()(const Rank& r1,const Rank& r2) const
	{
		return (int)(r1 - r2);
	}
};

bool isOne (int i, int j) {
  return (i==1 && j==1);
}

//two pair: keep both pairs, and replace the other card.
void auto_discardTP(Hand &cards)
{
	vector<Card> vec1 = cards.getHand();
	//get vector of Ranks,
	//find which Rank is in Pair
	vector<Rank> vecRank;
	transform(vec1.begin(), vec1.end(), back_inserter<vector<Rank> >(vecRank),
			getRank);
	//Sort Ranks
	sort(vecRank.begin(), vecRank.end());
	vector<Rank>::iterator pair1Rank = adjacent_find(vecRank.begin(),
			vecRank.end());
	vector<Rank>::iterator pair2Rank = adjacent_find(++pair1Rank,
			vecRank.end());

	for (vector<Card>::const_iterator it2 = vec1.begin(); it2 != vec1.end();
			++it2) {
		Card c = *it2;
		if (c.c_rank != *(pair1Rank) && c.c_rank != *(pair2Rank)) {
			vector<card_bool_pair>::iterator itr = find_if(
					cards.getHandBoolPair()->begin(),
					cards.getHandBoolPair()->end(), isCard(c));
			itr->second = true;
			cout << "Discarding unmatched card " << c << "in hand" << endl;
		}
	}
	cout << endl;
	return;
}
//four of a kind: if and only if there are more ranks
//(not counting the rank of the four cards) above the
//unmatched card than below it, replace that one card
//with a new card from the deck.
void auto_discardFOAK(Hand &cards)
{
	vector<Card> vec1 = cards.getHand();
	//get vector of Ranks,
	//find which Rank is in Pair
	vector<Rank> vecRank;
	transform(vec1.begin(), vec1.end(), back_inserter<vector<Rank> >(vecRank),getRank);
	//Sort Ranks
	sort(vecRank.begin(), vecRank.end());
	vector<Rank>::iterator foakRank = adjacent_find(vecRank.begin(),vecRank.end());
	//find the card which is not foak
	vector<Card>::iterator it = find_if(vec1.begin(),vec1.end(),not1(isRank(*(foakRank))));
	//Replace this card only if there are more ranks above this card
	Card c = *it;
	if(c.c_rank < seven)
	{
		vector<card_bool_pair>::iterator itr = find_if(cards.getHandBoolPair()->begin(),
							cards.getHandBoolPair()->end(), isCard(c));
		itr->second = true;
		cout << "Discarding unmatched card " << c << "in hand" << endl;
	}
	cout << endl;
	return;


}

//three of a kind: keep the three cards of the same rank, and replace the other two.
void auto_discardTOAK(Hand &cards)
{
	vector<Card> vec1 = cards.getHand();
	//get vector of Ranks,
	//find which Rank is in Pair
	vector<Rank> vecRank;
	transform(vec1.begin(), vec1.end(), back_inserter<vector<Rank> >(vecRank),
			getRank);
	//Sort Ranks
	sort(vecRank.begin(), vecRank.end());
	vector<Rank>::iterator toakRank = adjacent_find(vecRank.begin(),vecRank.end());

	cout << "Discarding two unmatched cards ";
	for (vector<Card>::const_iterator it2 = vec1.begin(); it2 != vec1.end();++it2)
	{
		Card c = *it2;
		if (c.c_rank != *(toakRank))
		{
			vector<card_bool_pair>::iterator itr = find_if(cards.getHandBoolPair()->begin(),
					cards.getHandBoolPair()->end(), isCard(c));
			itr->second = true;
			cout << c << " ";
		}
	}
	cout << " in hand" <<endl;
	return;

}

//if the pair is jacks or higher or if none of the other three cards is an ace,
//keep the pair and replace all three of the other cards;
//otherwise, keep the pair and the ace and replace the other two cards.
void auto_discardPair(Hand &cards)
{
	vector<Card> vec1 = cards.getHand();
	//get vector of Ranks,
	//find which Rank is in Pair
	vector<Rank> vecRank;
	transform(vec1.begin(),vec1.end(),back_inserter< vector<Rank> >(vecRank),getRank);
	//Sort Ranks
	sort(vecRank.begin(),vecRank.end());
	vector<Rank>::iterator pairRank = adjacent_find(vecRank.begin(),vecRank.end());

	//Look for Ace
	//vector<Rank>::iterator aceIt = find(vecRank.begin(),vecRank.end(),ace);

	cout << "Discarding following unmatched cards ";
	for(vector<Card>::const_iterator it2 = vec1.begin();it2!=vec1.end();++it2)
	{
		Card c = *it2;
		if(c.c_rank != *(pairRank) && c.c_rank!= ace)
		{
			vector<card_bool_pair>::iterator itr = find_if(cards.getHandBoolPair()->begin(),cards.getHandBoolPair()->end(),isCard(c));
			itr->second = true;
			cout << c << " ";
		}
	}

	cout << " in hand" << endl;
	return;


}

//(1) if there are four cards of the same suit or that have consecutive ranks
//(not including an ace) replace the one other card; otherwise if
//(2) one of the cards is an ace, replace the other four cards;
//otherwise replace all five cards.
void auto_discardNoRank(Hand &cards)
{

		vector<Card> vec1 = cards.getHand();

		cout << "Discarding following unmatched cards ";

		//check if there are 4 cards of same suit
		//checking if option for flush
		for(int i = 0 ;i<=spade;i++)
		{
			Suit s = (Suit) i;

			int c = count_if(vec1.begin(),vec1.end(),isSuit(s));
			if(c==4)
			{
				vector<Card>::const_iterator it = find_if(vec1.begin(),vec1.end(),not1(isSuit(s)));
				Card c = *it;
				if(it != vec1.end())
				{
					vector<card_bool_pair>::iterator itr = find_if(cards.getHandBoolPair()->begin(),cards.getHandBoolPair()->end(),isCard(c));
					//set bool isDiscarded to true
					itr->second = true;
					cout << c <<  " " ;
					return;
				}

			}
		}

		//check there are 4 cards with consecutive ranks
		//if there is an ace it will be at the last.
		//checking for straight
		bool aceFound(false);
		Card aceCard;
		//create a vector for card Ranks
		vector<Rank> vec2;
		transform(vec1.begin(),vec1.end(),back_inserter< vector<Rank> >(vec2),getRank);
		//remove if ace is found
		vector<Rank>::iterator aceIt = find(vec2.begin(),vec2.end(),ace);
		if(aceIt != vec2.end())
		{
			vector<Card>::const_iterator it = find_if(vec1.begin(),vec1.end(),isRank(ace));
			aceCard = *it;
			aceFound = true;
			vec2.erase(aceIt);
		}
		//Sort other Ranks
		sort(vec2.begin(),vec2.end());
		//find adjacent difference between adjacent ranks , if there are 4 adjacent differences of 1 then we have 4 consecutive cards
		vector<int> vint;
		adjacent_difference(vec2.begin(),vec2.end(),back_inserter< vector<int> >(vint),minusRank());
		// count if there are 4 consecutive cards
		//vint will have difference of rank from position 2 to end ,
		//we need ajacent_find to check if there are three 1s ajacent to each other.
		int cnt(0);
		vector<int>::iterator one = adjacent_find(vint.begin()+1,vint.end(),isOne);

		// Find how many ajacent 1s are registerd in vint vector
		while(one!= vint.end())
		{
			++cnt;
			one = adjacent_find(++one,vint.end(),isOne);
		}

		//if count of adjacent_finds is 2 we had three 1s in vector<int>, so 4 cards with consecutive ranks
		if(cnt == 2)
		{
			Card c;
			if(!aceFound)
			{
				//find which card to discard the first or the last
				vector<int>::iterator notOne = find_if(vint.begin()+1,vint.end(),bind2nd(not_equal_to<int>(),1));
				vector<int>::difference_type pos = notOne - vint.begin();
				int i = pos == 1 ? 0 : 4;
				vector<Card>::const_iterator it = find_if(vec1.begin(),vec1.end(),isRank(vec2[i]));
				if(it != vec1.end())
					c = *it;
			}
			else
			{
				c= aceCard;
			}
			vector<card_bool_pair>::iterator itr = find_if(cards.getHandBoolPair()->begin(),cards.getHandBoolPair()->end(),isCard(c));
			itr->second = true;
			cout << c << " " ;
			return;

		}

		//check if one of the cards is ace
		//if ace is found dont discard it
		vector<Card>::iterator it1 = find_if(vec1.begin(),vec1.end(),isRank(ace));
		for(vector<Card>::const_iterator it2 = vec1.begin();it2!=vec1.end();++it2)
		{
			Card c = *it2;
			if(it1 == vec1.end() || (it1!= vec1.end() && it2 != it1))
			{
				vector<card_bool_pair>::iterator itr = find_if(cards.getHandBoolPair()->begin(),cards.getHandBoolPair()->end(),isCard(c));
				itr->second = true;
				cout << c << " ";
			}
		}
		cout << " in the hand" << endl;
		return;

}

int auto_discard(Hand &cards)
{
	//
		// The auto_discard method gets the rank of the hand for auto_players
		// and then calls an appropriate helper method which will find the
		// appropriate cards to be discarded depending on the type of the hand.
		//
		int rank = getHandRank(cards);
		if(rank == STRAIGHT_FLUSH || rank == FLUSH
			|| rank == FULL_HOUSE || rank == STRAIGHT)
			cout << "Not discarding any cards." << endl;
		else if(rank == FOUR_OF_A_KIND){
			auto_discardFOAK(cards);
		}
		else if(rank == TWO_PAIR){
			auto_discardTP(cards);
		}
		else if(rank == THREE_OF_A_KIND){
			auto_discardTOAK(cards);
		}
		else if(rank == PAIR){
			auto_discardPair(cards);
		}
		else if(rank == NO_RANK){
			auto_discardNoRank(cards);
		}


		return 0;
}
