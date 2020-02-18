/*
* X10receiverApp.h
*
* Created: 08-04-2019 21:28:37
* Author: Tobias
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#ifndef __X10RECEIVERAPP_H__
#define __X10RECEIVERAPP_H__


class X10receiverApp
{

	//functions
	public:
	X10receiverApp();
	void initReceiver();
	void read();
	bool checkBuffers();
	bool checkAddress();
	bool execute();
	~X10receiverApp();
	bool handle_;	//if msg received and ack/nack
	bool ack_;		//true for ack, false for nack
	private:
	bool startRead_;
	unsigned char RXvalue_; //value read at zero-cross
	unsigned char startBuffer_[4]; //startbit buffer
	unsigned char readBuffer_[2]; //buffer to read a bit
	unsigned char lineBuffer_[15]; //Read first line
	unsigned char checkBuffer_[15]; //Read repeated line to check
	unsigned char lineReceived_[9];	//buffer for repeated line
	bool lineRead_;	//if a full line has been read
	bool received_;	//if repeated line received
	bool listen_;	//if this adress
	unsigned char address_[9] = {1, 1, 0, 0, 1, 1, 0, 0, 0}; //address P16 until changed
	unsigned char ONcmd_[5] = {0, 0, 1, 0, 1};	//cmd for turning ON
	unsigned char OFFcmd_[5] = {0, 0, 1, 1, 1}; //cmd for turning OFF
	unsigned char startIndex_;
	unsigned char readIndex_;
	unsigned char lineIndex_;
}; //X10receiverApp

#endif //__X10RECEIVERAPP_H__