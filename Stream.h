// File: Game.cpp
// 10/2011
// abenjamin@wustl.edu - Alex Benjamin
// linfeixb27@gmail.com - Yinfan Li
// mahesh.mahadevan@gmail.com - Mahesh M.

#ifndef STREAM_H
#define STREAM_H

#include <ace/INET_Addr.h>
#include <string>
#include <sstream>
using namespace std;

//General inline function that returns a formatted
//string of the INET_Addr
inline string toString( const ACE_INET_Addr& addr ) {
  stringstream ss;
  ss << addr.get_host_addr() << ":" << addr.get_port_number();
  return ss.str();
}

#endif
