// File: DataWrapper.h
// 10/2011
// abenjamin@wustl.edu - Alex Benjamin
// linfeixb27@gmail.com - Yinfan Li
// mahesh.mahadevan@gmail.com - Mahesh M.

#ifndef DATAWRAPPER_H
#define DATAWRAPPER_H

#include <ace/SOCK_Stream.h>
#include <ace/Message_Block.h>
#include <ace/INET_Addr.h>
#include <ace/CDR_Base.h>
#include <ace/CDR_Stream.h>
#include <string>
#include <sstream>
#include "Stream.h"
using namespace std;

class DataWrapper {

public:
	//Initialization and Close
        DataWrapper() : theData(0) {}
	int init(const ACE_INET_Addr &address);
	int close();

	//Data transaction
	int read();
	int write(const ACE_OutputCDR &out);
	
	//Accessors
	ACE_Message_Block* getData() { return theData; }
	int getEndian() { return swapEndian ? !ACE_CDR_BYTE_ORDER: ACE_CDR_BYTE_ORDER; }
	const ACE_SOCK_Stream& getStream() const { return theStream; }
	ACE_SOCK_Stream& getStream() { return theStream; }

private:
	int getHeader();
	int updateState();

	ACE_INET_Addr theAddress;
	ACE_SOCK_Stream theStream;
	ACE_Message_Block *theData;
	ssize_t remainingBytes;
	bool header;
	bool swapEndian;
};

const int headerLength = (ACE_CDR::LONG_SIZE * 2);
const unsigned int bitsForward = (unsigned int)0xaabbccdd;
const unsigned int bitsBackward	= (unsigned int)0xddccbbaa;

#endif
