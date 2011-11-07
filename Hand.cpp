// hand.cpp
// Names: Alex Benjamin
// Email: abenjamin@wustl.edu
//

#include "Hand.h"
#include "Card.h"
#include "Poker.h"
#include <string>
#include <ostream>
#include <iostream>
#include <algorithm>
using namespace std;

Hand::Hand(){
	handVec.clear();
	//Lab2
	handVec.reserve(5);
}
Hand::Hand(const Hand & otherHand){
	for(size_t i = 0; i < otherHand.size(); i++)
		handVec.push_back(otherHand.handVec[i]);
}
//DESTRUCTOR?????

Hand & Hand::operator=(const Hand &otherHand){
	if(this != &otherHand){
		handVec.clear();
		for(size_t i = 0; i < otherHand.size(); i++)
			handVec.push_back(otherHand.handVec[i]);
	}
	return *this;
};

size_t Hand::size() const {
	return handVec.size();
}

bool Hand::operator==(const Hand &otherHand) const{
	if(otherHand.size() != size())
		return false;
	//Works even if Hands are not in sorted orders
	for(size_t i = 0; i < size(); ++i){
		bool foundCard = false;
		for(size_t j = 0; j < otherHand.size(); ++j){
			//Lab2 - update comparison for boolean
			if(handVec[i].first == otherHand.handVec[j].first && handVec[i].second == otherHand.handVec[j].second)
			{
				foundCard = true;
				break;
			}	
		}
		if(!foundCard)
			return false;
	}
	return true;
}

string Hand::asString() const {
	string hand;
	for(size_t i = 0; i < handVec.size(); ++i){
		//Lab2 - print first of pair
		hand += printRank(handVec[i].first);
		hand += printSuit(handVec[i].first);
		hand += " ";
	}
	return hand;
}

//Lab2 Replace with card bool pair
int Hand::add_card(card_bool_pair c) {
	handVec.push_back(c);
	return 0;
}

int Hand::remove_card(const size_t index) {
	if(index >= size())
		throw BAD_CARD_INDEX_ERROR;
	handVec.erase(handVec.begin()+(index));
	return 0;
}
//Lab2 update to card bool pair
card_bool_pair Hand::operator[] (const size_t index) const {
	if(index >= size())
		throw BAD_CARD_INDEX_ERROR;
	return handVec[index];
}

ostream & operator<< (ostream &o, const Hand &otherHand) {
	o << otherHand.asString();
	return o;
}

//Lab2 Update the method to return only vector of Cards.
vector<Card> Hand::getHand() const{
	vector<Card> cards;
	//Comment - Need const_iterator here as handVec.begin() return const_iterator
	for(vector<card_bool_pair>::const_iterator itr = handVec.begin();itr!=handVec.end();++itr)
	{
		cards.push_back(itr->first);
	}
	return cards;
}

//Lab2 new method to return hand and bool pair
vector<card_bool_pair>* Hand::getHandBoolPair() {
return &handVec;
}

bool Hand::operator< (const Hand &otherHand) {
	size_t thisSize = size(), otherSize = otherHand.size();
	size_t length = otherSize > thisSize ? otherSize : thisSize;
	//length is the size of the longer of the two hands.

	//essentially compare each card at a time until
	//one hand runs out of cards, or a comparison is
	//not equal	
	for(size_t i = 0; i < length; ++i){
		if(i >= thisSize && i < otherSize)
			//If first string has run out, first comes before the second string
			// i.e. "9H" < "9H 10H"
			return true;
		else if(i >= otherSize && i < thisSize)
			//Converse of previous case
			return false;
		//Lab2 - print first of pair
		else if(handVec[i].first < otherHand.handVec[i].first)
			return true;
		else if(otherHand.handVec[i].first  < handVec[i].first)
			return false;
	}

	//at this point the hands are the same
	//in order to establish "strict weak ordering"
	//we add some consistency to the decision
	return false;
}
