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

//Lab2 - pair to store GameType and Game mapping. First - GameType , Second - Game
typedef pair<const ACE_TCHAR*, const ACE_TCHAR*>gametype_game_pair;

class ConnectInfo {

public:
        ConnectInfo();
        ConnectInfo(ACE_TCHAR* pName);
    //Lab2 - change the vector to type of pair tuple handling both game type and games
    //vector<const ACE_TCHAR*> games;
    vector<gametype_game_pair> games;
	int getPort() { return port; }
	const ACE_TCHAR* getAddress() { return address; }
	const ACE_TCHAR* getName() { return name; }
	//Lab2 - update addGame method to push vector of pair
	//void addGame(const ACE_TCHAR* gm) { games.push_back(gm); }
	void addGame(gametype_game_pair gm) { games.push_back(gm); }
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
