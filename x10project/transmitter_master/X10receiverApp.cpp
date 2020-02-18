/*
* X10receiverApp.cpp
*
* Created: 08-04-2019 21:28:37
* Author: Tobias
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "X10receiverApp.h"

// default constructor
X10receiverApp::X10receiverApp()
{
	
	//transmitter part//
	
	
	//receiver part
	RXvalue_ = 0;
	startRead_ = false;
	readIndex_ = 0;
	lineIndex_ = 0;
	lineRead_ = false;
	received_ = false;
	listen_ = false;
	ack_ = false;
	handle_ = false;
}

void X10receiverApp::initReceiver()
{
	//INT0 interrupt at rising edge
	EICRA |= 0b00000011;
	//Enable extern INT0 (zero-cross)
	EIMSK |= 0b00000001;
	//Enable global interrupt
	
	DDRF &= 0b11111110;	//using PF0 as read-input
	DDRB |= 0b00000111; //for testing on/off
	DDRA |= 0b11111111; //for testing
	DDRG |= 0b00000010;
	
	for (size_t i = 0; i <= 3; i++)
	{
		startBuffer_[i] = 0;
	}
	
	sei();
}

void X10receiverApp::read()
{
	cli();
			
	TCNT3 = 65055; //to count 
	//TCNT3 = 64000; //to count 
	
	TCCR3B |= 0b00000001; //no prescaler, starting time
	
	
	while((TIFR3 & (1 << 0)) == 0)
	{}
		
	
	// No clock, stop af timer3.
	TCCR3B &= 0b11111000;
	
	//nulstiller overflow flag
	TIFR3 = 0b00000001;
	
	//****************************** Read half a bit and check if the same ******************************//
	//read input
	RXvalue_ = PINF & (1 << 0);
	
	//setup timer0 to count for 0,7ms
	TCNT0 = 81;
	//TCNT0 = 150;
	//prescaler 64, start clock
	TCCR0B |= 0b00000011;
	
	//Wait for overflow
	while((TIFR0 & (1 << 0)) == 0)
	{}
			
	
	
	//stop timer
	TCCR0B &= 0b11111000;
	//reset overflow flag
	TIFR0 = 0b00000001;
	
	
	//If not same level between 700ms, read a 0 (error or noise)
	if(RXvalue_ != (PINF & (1 << 0)))
	{
		RXvalue_ = 0;
	}
	//else read value
	else
	{
		RXvalue_ = PINF & (1 << 0);
		//PORTA = RXvalue_;
		//PORTA |= (1 << 0);
	}
	
	//****************************** Before reading a full line, read a start code (1110) *************************************//
	
	if(startRead_ == false)
	{
		
		for(size_t i = 1; i <= 3; i++)
		{
			startBuffer_[i-1] = startBuffer_[i];
		}
		
		startBuffer_[3] = RXvalue_;
		
				
		//if (startBuffer_[0] == 1) PORTA |= 0b00000001; //turn on PB0;
		//else PORTA &= 0b11111110;
		//if (startBuffer_[1] == 1) PORTA |= 0b00000010; //turn on PB1;
		//else PORTA &= 0b11111101;
		//if (startBuffer_[2] == 1) PORTA |= 0b00000100; //turn on PB2;
		//else PORTA &= 0b11111011;
		//if (startBuffer_[3] == 1) PORTA |= 0b00001000; //turn on PB3;
		//else PORTA &= 0b11110111;
		
		//PORTB |= 0b00000001; //turn on PB0;
		
		if((startBuffer_[0] == 1) && (startBuffer_[1] == 1) && (startBuffer_[2] == 1) && (startBuffer_[3] == 0))
		{
			startRead_ = true;
			return;
		}
		else
		{
			startRead_ = false;
		}
		
	}
	
	
	//if(startRead_ == false) //if no start code has been read, handle start code
	//{
		//if(startIndex_ <= 3)
		//{
			//startBuffer_[startIndex_] = RXvalue_;
			//
			//if(startIndex_ == 3) //if 4 chars read, check if start bit
			//{
				//if((startBuffer_[0] == 1) && (startBuffer_[1] == 1) && (startBuffer_[2] == 1) && (startBuffer_[3] == 0))
				//{
					//startRead_ = true;
				//}
				//else
				//{
					//startRead_ = false;
				//}
				//startIndex_ = 0; //reset startindex
				//
			//} // end check
			//
			//else //else get ready to read next bit in startcode
			//{
				//startIndex_++;
			//}
		//} //end read of start code
	//} //end handle of start code
	
	
	//****************************** If start code has been read, read a full line and repeat until checkbuffer is full ******************************//
	
	if(startRead_ == true)
	{
		if(readIndex_ <= 1) //handle readbuffer
		{
			readBuffer_[readIndex_] = RXvalue_;
			//PORTA = (readBuffer_[readIndex_] << 2);
			
			if(readIndex_ == 1) //handle read bit
			{
				if((readBuffer_[0] == 1) && (readBuffer_[1] == 0)) //if read bit is 1
				{
					//PORTB = (1 << 1);
					if(lineIndex_ <= 8 ) //read 1 into linebuffer
					{
						lineBuffer_[lineIndex_] = 1;
						
						if(lineIndex_ >= 8) //if first line has been read, read a new start code
						{
							startRead_ = false;
						}
						
						lineIndex_++;
						
					} //end read 1 to linebuffer
					
					else if((lineIndex_ > 8) && (lineIndex_ <= 17)) //read 1 to checkbuffer
					{
						checkBuffer_[lineIndex_-9] = 1;
						
						if(lineIndex_ >= 17) //if read into last char of linebuffer, reset
						{
							lineRead_ = true; //a full line is read
							startRead_ = false; //get ready to read new start code
							lineIndex_ = 0;
						}
						else
						{
							lineRead_ = false; //a full line is not read yet
							lineIndex_++;
						}
					} // end read 1 to checkbuffer
				} //end handle a 1
				
				else //else it must be a 0
				{
					//PORTB = (0 << 1);
					if(lineIndex_ <= 8) //read 0 to linebuffer
					{
						lineBuffer_[lineIndex_] = 0;
						
						if(lineIndex_ >= 8) //if full line has been read, read a new start code
						{
							startRead_ = false;
						}
						
						lineIndex_++;
					} // end read 0 to linebuffer
					
					else if((lineIndex_ > 8) && (lineIndex_ <= 17)) //read 0 to checkbuffer
					{
						checkBuffer_[lineIndex_-9] = 0;
						
						if(lineIndex_ >= 17) //if read into last char of checkbuffer, reset index
						{
							lineRead_ = true; //a full line is read
							startRead_ = false; //read a new start code
							lineIndex_ = 0;
						}
						
						else
						{
							lineRead_ = false; //a full line is not read yet
							lineIndex_++;
						}
					} //end read 0 to checkbuffer
				} //end handle a 0
				
				readIndex_ = 0; //reset readIndex to read new bit
				for (size_t i = 0; i <= 1; i++)
				{
					readBuffer_[i]= 0;
				}
			} //end handle of read bit if readindex is 1
			
			else //if not 1, last half bit needs to be read, before handing to line- or checkbuffer
			{
				readIndex_++;
			}
		} //end handle readbuffer
				
	}
	
	//****************************** If a full line has been read, check if linebuffer is repeated correctly ******************************//
	
	if(lineRead_)
	{
		//PORTA = (lineBuffer_[8] << 0) | (lineBuffer_[7] << 1) | (lineBuffer_[6] << 2) | (lineBuffer_[5] << 3) | (lineBuffer_[4] << 4) | (lineBuffer_[3] << 5) | (lineBuffer_[2] << 6) | (lineBuffer_[1] << 7);
		//PORTG = (lineBuffer_[0] << 1);
		
		//PORTA = (checkBuffer_[8] << 0) | (checkBuffer_[7] << 1) | (checkBuffer_[6] << 2) | (checkBuffer_[5] << 3) | (checkBuffer_[4] << 4) | (checkBuffer_[3] << 5) | (checkBuffer_[2] << 6) | (checkBuffer_[1] << 7);
		//PORTG = (checkBuffer_[0] << 1);
		
		//PORTB |= (1 << 1);
		
		if(checkBuffers()) //if repeated correctly
		{
			for (size_t i = 0; i < 9; i++)
			{
				lineReceived_[i] = lineBuffer_[i];
				PORTA = (lineReceived_[8] << 0) | (lineReceived_[7] << 1) | (lineReceived_[6] << 2) | (lineReceived_[5] << 3) | (lineReceived_[4] << 4) | (lineReceived_[3] << 5) | (lineReceived_[2] << 6) | (lineReceived_[1] << 7);
				PORTG = (lineReceived_[0] << 1);
			}
			
			received_ = true; //a command has been received and needs to be handled
			lineRead_ = false; //Ready to read new command
		}
		
		else
		{
			received_ = false; //a command has not been recieved and error needs to be handled
			lineRead_ = false; //Ready to read new command
		}
	}

	//****************************** if command received correctly, check address ******************************//

	if(received_)
	{
		
		if(lineReceived_[8] == 0) //if line received is an address (ending with 0)
		{
			
			
			if(checkAddress()) //if this address, listen
			{				
				listen_ = true;
				received_ = false; //ready to read new líne
				return;
			}
			else
			{
				listen_ = false;
				received_ = false; //ready to read new line
			}
		}
	}
	
	//****************************** if this address, listen and execute ******************************//
	
	if(listen_ && (lineReceived_[8] == 1)) //if line received is cmd and listening
	{
		if(execute()) //if command understood and executed, acknowledge
		{
			
			ack_ = true;	//if executed succesfully (cmd not found)
			listen_ = false; //stop listening, handle instead
			handle_ = true; //needs to be handled and answered
		}
		
		else //if command not understood and not executed, not-acknowledge
		{
			//PORTB |= (1 << 1);
			ack_ = false;	//if not executed succesfully (cmd not found)
			listen_ = false; //stop listening, handle instead
			handle_ = true; //need to be handled and answered
			
		}
	}
	
	//****************************** outside read-function, wait for handle ******************************//
	
	sei();
}

bool X10receiverApp::checkBuffers()
{
	for(size_t i = 0; i <= 8; i++)
	{
		if(lineBuffer_[i] != checkBuffer_[i]) //if not repeated, return false.
		{
			return false;
		}
	}
	return true;
}

bool X10receiverApp::checkAddress()
{
	for (size_t i = 0; i <= 8; i++)
	{
		if(lineReceived_[i] != address_[i])
		{
			return false;
		}
	}
	
	return true;
}

bool X10receiverApp::execute()
{
	for(size_t i = 0; i <= 3; i++) //check adress for correct letter
	{
		if(lineReceived_[i] != address_[i])
		{
			return false;
			
		}
	}
	
	bool next = false; //to run through available commands
	
	//****************************** if adressing correct, check for available commands ******************************//
	
	//****************************** Check for ON-cmd ******************************//
	for (size_t i = 4; i <= 8; i++)
	{
		if (lineReceived_[i] != ONcmd_[i-4])
		{
			next = true;
		}
	}
	
	if(next == false) //if ON-cmd received, turn test lamp on and return true.
	{
		PORTB |= 0b00000001; //turn on PB0;
		
		return true;
	}
	
	//****************************** Check for OFF-cmd ******************************//
	
	next = false; //set to false, to check again
	
	for (size_t i = 4; i < 9; i++)
	{
		if (lineReceived_[i] != OFFcmd_[i-4])
		{
			next = true;
		}
	}
	
	if(next == false) //if OFF-cmd received, turn test lamp off and return true.
	{
		PORTB &= 0b11111110; //turn off PB0;
		
		return true;
	}
	
	//****************************** else, next must be true and no matching cmds was found ******************************//
	
	return false;

}

// default destructor
X10receiverApp::~X10receiverApp()
{
}