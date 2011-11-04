// File: Client.cpp
// 10/2011
// abenjamin@wustl.edu - Alex Benjamin
// linfeixb27@gmail.com - Yinfan Li
// mahesh.mahadevan@gmail.com - Mahesh M.

#include "Player_Svc_Handler.h"
#include "Const.h"
#include "ConnectInfo.h"
#include "PlayerConnection.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <istream>
#include <sstream>
using namespace std;

//First letter uppercase, everything else lowercase
string formatLowercase(string str) {
	
    for (unsigned int i=0;i<strlen(str.c_str());i++) 
        if (str[i] >= 'A' && str[i] <= 'Z') 
            str[i] = str[i] + 'a' - 'A';

	str[0] = str[0] - ('a' - 'A');

    return str;
}

// Check the parameters from the user and make sure they are all recognized
int parseCommandLine(int argc,char** argv, string &name, string &file)
{
	bool mandatoryOpsN = false, mandatoryOpsF = false;
	for(int i = 1,j=2; i < argc && j < argc; i+=2,j+=2)
	{
		string in(argv[j]);
		//		in = formatLowercase(in);
		if(strcmp(argv[i],"-n") != 0 && strcmp(argv[i],"-f") != 0)
			return ERROR_CMD_LN_SYNTAX;
		else if(strcmp(argv[i],"-n") == 0){
			mandatoryOpsN = true;
			name = formatLowercase(in);
		}
		else if(strcmp(argv[i],"-f") == 0){
			mandatoryOpsF = true;	
			file = in;
		}
	}
	//We're missing a required parameter
	if(!mandatoryOpsN)
		return ERR_MISSING_N_FLAG;
	if(!mandatoryOpsF)
		return ERR_MISSING_F_FLAG;
	
	if(argc != 5)
	  return ERR_INVALID_ARG_NUM;

	//Parameters are valid
	return 0;	
}

//Central error handling utility - prints out message and returns error code
int errorHandler(int error){
	const string usageMessage = "client -n <client name> -f <definition file>";
	cout << "Error: ";
	switch(error){
		case ERROR_CMD_LN_SYNTAX:
			cout << "Invalid command line syntax." << endl << usageMessage << endl;
			break;
		case ERR_MISSING_N_FLAG:
			cout << "Missing required -n flag in command line." << endl;
			break;
		case ERR_MISSING_F_FLAG:
			cout << "Missing required -f flag in command line." << endl;
			break;
		case ERR_INVALID_ARG_NUM:
			cout << "Invalid number of command line arguments." << endl << usageMessage << endl;
			break;
		case ERR_INVALID_FILE:
			cout << "Unable to open definition file suppled." << endl;
			break;
		case ERR_NO_VALID_DEFS:
			cout << "None of the definition file lines were valid, cannot proceed." << endl;
			break;
		case ERR_OPEN_CONNECTION:
			cout << "Could not open up reactor connections." << endl;
			break;
		case ERR_INET_ADDR_SET:
			cout << "Failed to connect to server while setting INET Address." << endl;
			break;
		case ERR_REACTOR_LOOP_FAILED:
			cout << "Client failed during reactor event loop." << endl;
			break;
	}

	return error;
}

//This interprets one line from the player definition file and
//stores it in a ConnectInfo object
int interpretDefinition(ConnectInfo &ci,string line){
	vector<string> tokens;
	istringstream iss(line);
	//Tokenize the line with an istringstream
	copy(istream_iterator<string>(iss),istream_iterator<string>(),back_inserter<vector<string> >(tokens));
	string flag;
	string value;
	const int MIN_PORT = 1024;
	//Lab2 - Moving the increment of i and j inside the loop based on what flag was encountered.
	//Will increment by 3 if -g , else by 2
	for(size_t i=0,j=1; i < tokens.size() && j < tokens.size(); /*Lab2-i+=2,j+=2*/)
	{
		flag = tokens[i];
		value = tokens[j];
		if(flag.compare("-g") != 0 && flag.compare("-i") != 0 && flag.compare("-p") != 0)
			return -1;
		if(flag.compare("-g") == 0){
		  //Lab2- first string after g is considered as gameType and the one after as game name.
		  //Need error check for if 2 arguments were received after -g flag.
		  //Need error check for game type either being draw or stud
		  if(j+1 >= tokens.size())
		  {
			  cout << "Invalid number of arguments passed after -g. Either GameType or GameName is missing" << endl;
			  return -1;
		  }
		  if(formatLowercase(value).compare(GAME_TYPE_DRAW)!=0 && formatLowercase(value).compare(GAME_TYPE_STUD)!=0)
		  {
			  cout << "Invalid game type. Game type should be one of these (" << GAME_TYPE_DRAW << "," <<GAME_TYPE_STUD<< ")" << endl;
			  return -1;
		  }
		  char* v=new char[value.size()];
		  strcpy(v,formatLowercase(value).c_str());
		  value = tokens[j+1];
		  char* vv=new char[value.size()];
		  strcpy(vv,value.c_str());
		  gametype_game_pair game(v,vv);
		  ci.addGame(game);
		  i+=3;
		  j+=3;
		  //Lab2
		}
		if(flag.compare("-i") == 0){
		  char* v=new char[value.size()];
		  strcpy(v,value.c_str());
		  ci.setAddress(v);
		  //Lab2
		  i+=2;
		  j+=2;
		  //Lab2
		}
		if(flag.compare("-p") == 0){
			if(atoi(value.c_str()) <= MIN_PORT)
				cout << "Invalid port. Port must be greater than " << MIN_PORT << "." << endl;
			else
				ci.setPort(atoi(value.c_str()));
			//Lab2
			i+=2;
			j+=2;
			//Lab2
		}
	}

	//Parameters are valid
	return 0;	
}

int main(int argc, char* argv[])
{
	// client -n <client name> -f <definition file>
	string clientName;
	string clientFile;
	int error = 0;

	if( (error = parseCommandLine(argc,argv,clientName,clientFile)) != 0)
		return errorHandler(error);

	//Read in our definition file to vector defLines
	ifstream ifs(clientFile.c_str());
	if(!ifs) //make sure we can find the file
		return errorHandler(ERR_INVALID_FILE);
	vector<string> defLines;
	string inLine;
	while(!ifs.eof()){
		getline(ifs,inLine);
		if(!inLine.empty())
			defLines.push_back(inLine);
	}


	//We have all lines, parse each one
	vector<ConnectInfo> pInfo;
	  for(vector<string>::iterator it=defLines.begin(); it != defLines.end(); ++it){
	    ConnectInfo ci((char*)clientName.c_str());
	      if(interpretDefinition(ci,*it)==0 && ci.isInit())
		pInfo.push_back(ci);
	      else
		cout << "Invalid definition line. Skipping: " << *it << endl;
		}

	//None of the lines were valid
	if(pInfo.size() < 1)
		return errorHandler(ERR_NO_VALID_DEFS);

	//Print out all valid ConnectInfos
	for(vector<ConnectInfo>::iterator it=pInfo.begin(); it != pInfo.end(); ++it){
	  cout << "Name: " << (*it).getName() << ", Port: " << (*it).getPort() << ", Addr: " << (*it).getAddress() << endl /*<< "Games: "*/;
	  //Lab2 - Updated the Print statement to print both GameType and Game Name
	  	for(vector<gametype_game_pair>::iterator itg = (*it).games.begin();itg!= (*it).games.end();++itg)
			cout << "Game Type : " << itg->first << " :: Game : " << itg-> second;
	 //Lab2
	  cout << endl;
	}

	//Time to create our reactor and get things going
	ACE_Reactor* reactor = ACE_Reactor::instance();

	PlayerConnection pConn;
	//We want to open up however many connections are in pInfo
	if( (error = pConn.open(pInfo.size(),reactor,0)) != 0 )
		return errorHandler(ERR_OPEN_CONNECTION);

	//Last step, we will set up a service handler for each valid player
	//game, and then let the reactor handle things from there
	vector<Player_Svc_Handler *> thePlayers;
	for(vector<ConnectInfo>::iterator it=pInfo.begin(); it != pInfo.end(); ++it){
		ACE_INET_Addr inetAddr;
		Player_Svc_Handler *pHandler;
		//Time to actually connect to something
		  if(inetAddr.set((*it).getPort(),(*it).getAddress()) < 0)
			return errorHandler(ERR_INET_ADDR_SET);

		//Set up the pointer to a new instance
		    ACE_NEW_NORETURN(pHandler,Player_Svc_Handler(inetAddr,(*it).getName(),(*it).games));
		if(pHandler){
		  (*pHandler).setPlayerConnection(&pConn);

		    if(pConn.connect(pHandler,inetAddr) == -1)
		      cout << "Problem connecting to address/port. Skipping ConnectInfo entry." << endl;
		    
		    thePlayers.push_back(pHandler);
		}
	}

	//Everything is set -> start the event loop
	if(reactor->run_reactor_event_loop() == -1)
		return errorHandler(ERR_REACTOR_LOOP_FAILED);

	//Everything is done, clean up our pointers
	for(vector<Player_Svc_Handler *>::iterator it=thePlayers.begin(); it != thePlayers.end(); ++it)
		delete *it;

	ACE_Reactor::instance()->close();

	return 0;
}

