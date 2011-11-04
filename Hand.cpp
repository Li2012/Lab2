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
			if(handVec[i] == otherHand.handVec[j]) 
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
		hand += printRank(handVec[i]);
		hand += printSuit(handVec[i]);
		hand += " ";
	}
	return hand;
}

int Hand::add_card(Card c) {
	handVec.push_back(c);
	return 0;
}

int Hand::remove_card(const size_t index) {
	if(index >= size())
		throw BAD_CARD_INDEX_ERROR;
	handVec.erase(handVec.begin()+(index));
	return 0;
}
Card Hand::operator[] (const size_t index) const {
	if(index >= size())
		throw BAD_CARD_INDEX_ERROR;
	return handVec[index];
}

ostream & operator<< (ostream &o, const Hand &otherHand) {
	o << otherHand.asString();
	return o;
}

vector<Card> Hand::getHand() const{
	return handVec;
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
		else if(handVec[i] < otherHand.handVec[i])
			return true;
		else if(otherHand.handVec[i] < handVec[i])
			return false;
	}

	//at this point the hands are the same
	//in order to establish "strict weak ordering"
	//we add some consistency to the decision
	return false;
}
