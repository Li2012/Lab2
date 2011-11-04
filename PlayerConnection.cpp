// File: PlayerConnection.cpp
// 10/2011
// abenjamin@wustl.edu - Alex Benjamin
// linfeixb27@gmail.com - Yinfan Li
// mahesh.mahadevan@gmail.com - Mahesh M.

#include "PlayerConnection.h"

int PlayerConnection::open(int connectionCount, ACE_Reactor * reactor, int flag)
{
	if( connectionCount == 0 )
		return -1;

	//Check super class
	if( connectorBase::open( reactor, flag ) != 0 )
		return -1;

	serverCount = connectionCount;

	return 0;
}

void PlayerConnection::unregisterReactor()
{
	//Decrease number connected by one
	--serverCount;

	//If nobody is connected, start shutting things down
	if( serverCount == 0 )
		ACE_Reactor::instance()->end_reactor_event_loop();
}

