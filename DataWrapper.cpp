// File: DataWrapper.cpp
// 10/2011
// abenjamin@wustl.edu - Alex Benjamin
// linfeixb27@gmail.com - Yinfan Li
// mahesh.mahadevan@gmail.com - Mahesh M.

#include <ace/Log_Msg.h>
#include "DataWrapper.h"
#include <iostream>
using namespace std;

int DataWrapper::updateState()
{
	ssize_t	numBytes;
	numBytes = theStream.recv(theData->wr_ptr(), remainingBytes);
	//Receive didn't work, we return -1 (and probably close the connection)
	if(numBytes == -1)
		ACE_ERROR_RETURN((LM_ERROR, "%s, %p\n",toString(theAddress).c_str(),"System failed while updating state"), -1);
	if(numBytes == 0)
		return -1;
	theData->wr_ptr(numBytes);
	//however many bytes we still need to get
	remainingBytes -= numBytes;

	//0 if still waiting for data, otherwise 1 if finished
	if(remainingBytes > 0)
		return 0;
	else
		return 1;
}

int DataWrapper::write(const ACE_OutputCDR& cdr)
{
	//This method writes to an ace socket stream. First, it will collect
	//the appropriate header which includes the message length. It will then
	//combine ACE blocks that hold the header and message, and send it.
	//If preparing the block or sending fails, it returns an appropriate error.
	//If the actual socket send fails, the connection will be closed (not sure what
	//else to do, since it shouldn't fail under a good connection)

	if(!cdr.good_bit())
		ACE_ERROR_RETURN((LM_ERROR, "%s, %p\n",toString(theAddress).c_str(),"Failed while sending CDR"), -1);

	ACE_OutputCDR aceHead(headerLength);
	//Put header info into a CDR
	if(!(aceHead << (ACE_CDR::ULong)bitsForward) || !(aceHead << (ACE_CDR::ULong)cdr.length()))
		ACE_ERROR_RETURN( (LM_ERROR, "%s, %p\n",toString(theAddress).c_str(),"Failed while sending, CDR header"), -1 );

	//Create a block with the header and message
	const_cast<ACE_Message_Block*>(aceHead.begin())->cont(const_cast<ACE_Message_Block*>(cdr.begin()));
	const ssize_t msgLength = headerLength + ACE_CDR::total_length(cdr.begin(), cdr.end());

	//	cout << toString(theAddress) << endl;

	//Send the message
	const ssize_t bytesSent = theStream.send_n(aceHead.begin());

	// Clear the block
	const_cast<ACE_Message_Block*>(aceHead.begin())->cont(NULL);

	//Check whether the send succeeded (sent all the bytes). If not, we assume the socket has disconnected.
	//	cout << "Bytes sent: " << bytesSent << " | msgLength: " << msgLength << endl;
	if( bytesSent != msgLength ) {
		theStream.close_writer();
		ACE_ERROR_RETURN((LM_ERROR, "%s - %p\n",toString(theAddress).c_str(),"Socket failed while sending"), -1);
	}

	//Everything sent successfully
	return 0;
}

int DataWrapper::read()
{
	//This method will read data in from the socket connection. First, we check 
	//if we're waiting on more bytes. If we are, then we need to complete reading
	//a previous message. Otherwise, we are beginning to read a new message

	if( !header && remainingBytes == 0 ) {
		//There are no bytes remaining on the previous message,
		//so clear the buffers/flags and start reading the new one
		theData->reset();
		header = true;
		remainingBytes = headerLength;
	}

	if(header) {
		int	state = updateState();
		//Waiting on data and it wasn't there
		if( state != 1 )
			return state;

		state = getHeader();
		//Waiting on data and it wasn't there
		if( state == -1 )
			return -1;
	}

	return updateState();
}

int DataWrapper::getHeader()
{
	ACE_InputCDR cdr(theData);
	ACE_CDR::ULong bitOrder;
	ACE_CDR::ULong msgLength;

	//This method does the work of checking the initial parameters
	//including the bit order of data we're about to receive, and
	//also sets up header information

	//Check ordering of data
	cdr >> bitOrder;
	if(bitOrder != bitsForward) { //Endian bit ordering
		if(bitOrder != bitsBackward) //Reverse bit ordering
			ACE_ERROR_RETURN((LM_ERROR, "%s, %s\n",toString(theAddress).c_str(),"Bit order did not match"), -1);

		//Our endian order needs to be switched
		swapEndian = true;
		cdr.reset_byte_order( !ACE_CDR_BYTE_ORDER );
	}
	else
		swapEndian = false;

	//Get information about the actual message
	cdr >> msgLength;
	if(!cdr.good_bit())
		ACE_ERROR_RETURN((LM_ERROR, "%s, %s\n",toString(theAddress).c_str(),"Header is corrupted (process header)"), -1);
	
	theData->reset();
	int	checkVal = ACE_CDR::grow(theData,msgLength+ACE_CDR::MAX_ALIGNMENT);
	if(checkVal == -1)
		ACE_ERROR_RETURN((LM_ERROR, "%s, %s\n",toString(theAddress).c_str(),"packet buffer location failed"),-1);

	header = false;
	remainingBytes = msgLength;

	return 0;
}

//Called upon creation, sets initial parameters
int DataWrapper::init(const ACE_INET_Addr& address)
{
	theAddress = address;
	header = false;
	remainingBytes = 0;
	ACE_NEW_RETURN(theData,ACE_Message_Block(ACE_DEFAULT_CDR_BUFSIZE),-1);

	return 0;
}

//Called on destruction, closes socket and cleans up memory
int DataWrapper::close()
{
	if(theStream.close() == -1)	return -1;
	if(theData)	delete theData;
	return 0;
}
