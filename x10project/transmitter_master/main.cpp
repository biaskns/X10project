/*
 * X10transmitterApp.cpp
 *
 * Created: 04/04/2019 17.40.13
 * Author : Tobias
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "UART.h"
#include "wakeupRoutine.h"
#include "wakeupUnit.h"
#include "X10transmitter.h"

#define F_CPU 16000000
#define MAX_DATASIZE 200


X10transmitterApp X1;

wakeupRoutine firstRoutine;


UART arduino(9600,8,2,0,1);
char dataArray[MAX_DATASIZE];
char state = 0;
char activeUnit = 0;


ISR(TIMER5_COMPA_vect)
{
	cli();
	firstRoutine.secondsCounter++;
	if (firstRoutine.secondsCounter >= 15)
	{
		firstRoutine.minutesSinceBoot++;
		firstRoutine.secondsCounter = 0;
	}
	sei();
}

ISR(INT0_vect)
{
	cli();
	X1.transmitLine();
	sei();
}

ISR (USART0_RX_vect)
{
	cli();
	size_t index = 0;
	
	dataArray[index] = UDR0;
	index++;


	while (dataArray[index-1] != '\r' && index < MAX_DATASIZE)
	{
		dataArray[index] = arduino.read();
		index++;
	}
	
	//PORTB ^= 0b01111111;

	firstRoutine.parseData(dataArray);
	
	arduino.received_ = true;
	
	
	sei();
	
	
	//PORTB = 0x00;
}

int main(void)
{
	while(1)
	{
		
		switch (state)
		{
			case 0: //Init
				{
					firstRoutine.initClock();
					PORTB |= 0b00000001;
					
					DDRB = 0b01111111;
					X1.initTransmitter();
					state = 1;
				}
				break;
			
			case 1: //Wait for UART
				{
					PORTB |= 0b00000010;
					X1.disableX10(); //Disable INT0 interrupt
					
					while(arduino.received_ == false)
					{
						
					}
					
					arduino.received_ = false;
					state = 2;
				}
				break;
			
			
			case 2: // check time
				{
					PORTB |= 0b00000100;
					
					while (activeUnit = firstRoutine.compare() == 0)
					{
						
					}
					
					state = 3;
				}
				break;
				
				
			case 3: //Transmit
				{
					PORTB |= 0b00001000;
					arduino.disableUART();
					X1.enableX10();
					if (activeUnit < 16) X1.write2Buffer(firstRoutine.unitArr[activeUnit].adress_);
					else X1.write2Buffer(firstRoutine.unitArr[activeUnit - 16].adress_);
					
					
					while(!X1.transmitDone_ && X1.transmitRdy_);
					if (activeUnit < 16)
					{
						char command[] = "-ON";
						command[0] = firstRoutine.unitArr[activeUnit].adress_[0];
						X1.write2Buffer(command);
					} else
					{
						char command[] = "-OF";
						command[0] = firstRoutine.unitArr[activeUnit - 16].adress_[0];
						X1.write2Buffer(command);
					}
					
					while(!X1.transmitDone_ && X1.transmitRdy_);
					
					
					state = 2;
				}
				break;
			
			case 4: //Receive
				{
					
				}
				break;
			
		}
		
		
	}

}

