// File: ConnectInfo.cpp
// 10/2011
// abenjamin@wustl.edu - Alex Benjamin
// linfeixb27@gmail.com - Yinfan Li
// mahesh.mahadevan@gmail.com - Mahesh M.

#include "ConnectInfo.h"

ConnectInfo::ConnectInfo() : port(DEFAULT_PORT), address(DEFAULT_ADDR) { }
ConnectInfo::ConnectInfo(ACE_TCHAR* pName) : port(DEFAULT_PORT), address(DEFAULT_ADDR), name(pName) { }
void ConnectInfo::setPort(int p) {   if(port > MIN_PORT) port = p; }
bool ConnectInfo::isInit() { return name && address && port>MIN_PORT && games.size()>0; }
