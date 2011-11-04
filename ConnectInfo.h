// File: ConnectInfo.h
// 10/2011
// abenjamin@wustl.edu - Alex Benjamin
// linfeixb27@gmail.com - Yinfan Li
// mahesh.mahadevan@gmail.com - Mahesh M.

#ifndef CONNECTINFO_H
#define CONNECTINFO_H

#include "Const.h"
#include <ace/ACE.h>
#include <vector>
using namespace std;
class ConnectInfo {

public:
        ConnectInfo();
        ConnectInfo(ACE_TCHAR* pName);
	vector<const ACE_TCHAR*> games;
	//Lab2 - added new vector to hold gameTypes
	vector<const ACE_TCHAR*> gameTypes;
	int getPort() { return port; }
	const ACE_TCHAR* getAddress() { return address; }
	const ACE_TCHAR* getName() { return name; }
	void addGame(const ACE_TCHAR* gm) { games.push_back(gm); }
	//Lab2 - added new method to add gameTypes
	void addGameType(const ACE_TCHAR* gmType) {gameTypes.push_back(gmType);}
	void setAddress(ACE_TCHAR* addr) { address = addr; }
	void setPort(int p);
	void setName(ACE_TCHAR* inName) { name = inName; }
	bool isInit();
private:
	int port;
	const ACE_TCHAR* address;
	ACE_TCHAR* name;
};

#endif
