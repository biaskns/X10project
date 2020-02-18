/*
* X10transmitterApp.h
*
* Created: 04/04/2019 17.41.08
* Author: Tobias
*/


#ifndef __X10TRANSMITTERAPP_H__
#define __X10TRANSMITTERAPP_H__

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

class X10transmitterApp
{

	public:
	X10transmitterApp();
	~X10transmitterApp();
	//starter timer 1 normal mode, no prescaler, til at tælle i 1ms.
	void initTransmitter();
	void transmitLine();
	void transmit(char value);
	void shutTransmitter();
	void write2Buffer(char*);
	void translate(char*);
	void enableX10();
	void disableX10();
	
	
	bool transmitDone_;
	bool transmitRdy_;
	private:
	char TXbuffer_[50];
	char translated[18] = {0};
	size_t TXindex_;

}; //X10transmitterApp

#endif //__X10TRANSMITTERAPP_H__
