/*
 * X10receiverApp.cpp
 *
 * Created: 08-04-2019 21:28:11
 * Author : Tobias
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "X10receiverApp.h"
#include "X10transmitter.h"
#include "UART.h"
#include "wakeupRoutine.h"

X10receiverApp receiver;
X10transmitterApp transmitter;


ISR(INT0_vect)
{
	
	if(!receiver.handle_) //if nothing ack'ed or nack'ed, listen for start-bit and read
	{

		receiver.read();
		
	}
	
	else if(receiver.handle_)
	{
		
		if(receiver.ack_ && !transmitter.transmitRdy_) //if acknowledged fill buffer with ACK
		{
			for (size_t i = 0; i <= 2; i++)
			{
				transmitter.write2Buffer(1);
			}
			transmitter.write2Buffer(0); //start code
			
			for (size_t i = 4; i <= 21; i++)
			{
				transmitter.write2Buffer(1);
			}	
		}
		
		else if(!receiver.ack_ && !transmitter.transmitRdy_) //if acknowledged fill buffer with NACK
		{
			for (size_t i = 0; i <= 2; i++)
			{
				transmitter.write2Buffer(1);
			}
			transmitter.write2Buffer(0); //start code
			
			for (size_t i = 4; i <= 21; i++)
			{
				transmitter.write2Buffer(0);
			}
		}
		
		if(transmitter.transmitRdy_) //transmit ACK or NACK
		{
			
			transmitter.transmitLine();
			
			if(transmitter.transmitDone_) //if handled and answered
			{
				//PORTB |= (1 << 1);
				receiver.handle_ = false; //after answer, read again
			}
			
			
		}
		
		//else if(transmitter.transmitDone_) //if handled and answered
		//{
			//PORTB |= (1 << 1);
			//receiver.handle_ = false; //after answer, read again	
		//}
	}
	
}


int main(void)
{
		
	receiver.initReceiver();
	transmitter.initTransmitter();
	
    while (1) 
    {

    }
}