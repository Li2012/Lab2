// File: Server.cpp
// 10/2011
// abenjamin@wustl.edu - Alex Benjamin
// linfeixb27@gmail.com - Yinfan Li
// mahesh.mahadevan@gmail.com - Mahesh M.

#include <ace/ACE.h>
#include <ace/Log_Msg.h>
#include <iostream>
#include <string>
#include "Lib.h"
#include "Dealer.h"
#include "Const.h"
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
int parseCommandLine(int argc,char** argv, string &name, unsigned int &port)
{
	bool mandatoryOpsN = false;
	for(int i = 1,j=2; i < argc && j < argc; i+=2,j+=2)
	{
		string in(argv[j]);
		if(strcmp(argv[i],"-n") != 0 && strcmp(argv[i],"-p") != 0){
			cout << "Error: Invalid command line flag: " << argv[i] << endl;
			cout << "Syntax: Server -n <name> [-p <port num>]" << endl;
			return -1;
		}
		else if(strcmp(argv[i],"-n") == 0){
			mandatoryOpsN = true;
			name = formatLowercase(in);
		}
		else if(strcmp(argv[i],"-p") == 0){
		  port = atoi(in.c_str());
		}
	}
	//We're missing a required parameter
	if(!mandatoryOpsN){
		cout << "Error: Missing mandatory -n flag." << endl;
		cout << "Server -n <name> [-p <port num>]" << endl;
		return -1;
		//		return ERR_MISSING_N_FLAG;
	}

	//Parameters are valid
	return 0;	
}

int ACE_TMAIN(int argc, ACE_TCHAR* argv[])
{
	int error = 0;
	string dealerName;
	unsigned int dealerPort=DEFAULT_PORT;

	if(argc%2 != 1){
	  cout << "Invalid number of command line arguments." << endl;
	  cout << "Syntax: Server -n <name> [-p <port num>]" << endl;
	  return -1;
	}

	//Parse in the command line to our variables
	if((error = parseCommandLine(argc,argv,dealerName,dealerPort)) != 0)
		return error; //Message printing is handled by the function called

	cout << "Dealer Name: " << dealerName << endl;
	cout << "Port: " << dealerPort << endl;

	ACE_INET_Addr	addr;
	const char* ipAddress = addr.get_host_name();
	
	// Get the host of this server
	hostent* host = ACE_OS::gethostbyname(ipAddress);

	//Create a socket address struct to store our host information
	struct   sockaddr_in socketAddress;

	for(unsigned int num=0; host->h_addr_list[num]; ++num)
	{
		// Duplicate the socket address information to a sockaddr_in structure
		memcpy ( &socketAddress.sin_addr.s_addr, host->h_addr_list[num], host->h_length);

		cout << "Server Name: " << host->h_name << ", IP: " << ACE_OS::inet_ntoa(socketAddress.sin_addr) << endl;
	}
	
	//Attempt to set the INET_Address/port
	error = addr.set(dealerPort, (ACE_UINT32)INADDR_ANY );
	if(error == -1) {
		cout << "Server failed to set the INET_Address. Cannot recover." << endl;
		return error;
	}

	//Seed the PRNG
	srand(time(NULL));
	
	//Create a dealer with the reactor singleton
	Dealer dealer(dealerName.c_str(),ACE_Reactor::instance());

	//Attempt to open a connection at the specified address
	if((error = dealer.open(addr)) != 0)
	{
		cout << "Server failed to open a connection at: " << toString(addr) << endl;
		return error;
	}
	cout << "Dealer created and opened connection." << endl;

	//Everything should be set up, let the reactor handle things
	if(ACE_Reactor::instance()->run_reactor_event_loop() == -1)
		ACE_ERROR_RETURN( (LM_ERROR, "%p\n","Server: Reactor event loop failed."),1);
	
	ACE_Reactor::instance()->close();

	//End normally
	return 0;
}
