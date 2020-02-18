/*
* X10transmitterApp.cpp
*
* Created: 04/04/2019 17.41.08
* Author: Tobias
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "X10transmitter.h"

// default constructor
X10transmitterApp::X10transmitterApp()
{
	
	//fill array with start bit and nothing
	
	for(size_t i = 0; i <= 49; i++)
	{
		TXbuffer_[i] = 0;
	}
	
	TXindex_ = 0;
	
	//Transmit not done, new line to transmit not ready
	transmitDone_ = false;
	transmitRdy_ = false;
	
}

void X10transmitterApp::initTransmitter()
{
	sei();
	//INT0 interupt sker ved rising edge
	EICRA |= 0b00000011;
	//Enabler ekstern INT0 som er zero-cross.
	EIMSK |= 0b00000001;
	
	
	//initiering af timer0 i CTC-mode til 120kHz signal
	
	//Toggler efter 66 cycles, herved genereres 120kHz
	OCR0A = 66;
	
	//CTC mode, update at OCR0, toggle on compare match OC0A (PB7)
	TCCR0A |= 0b01000010;
	//No prescaler. Starter 120kHz HF-generator.
	TCCR0B |= 0b00000001;
}

void X10transmitterApp::transmit(char value)
{
	//forhindre forstyrelse i at transmitte
	cli();
	
	//16000 clockcycles for 1ms. Derfor loades 65536-16000 på TCNT1.
	TCNT1 = 49536;
	
	TCCR1A &= 0b11111100;
	TCCR1B &= 0b11100111;
	
	//prescaler 1, timer1 startes
	TCCR1B |= 0b00000001;
	
	if(value)
	{
		//Der transmitteres et logisk 1 for PB7 (OC0A), som er 120kHz-signalet
		DDRB |= 0b10000000;
	}
	
	//Vent på overflow flag
	while((TIFR1 & (1 << 0)) == 0)
	{}
	
	//Luk for 120kHz signalet
	DDRB &= 0b01111111;
	
	// No clock, stop af timer1.
	TCCR1B &= 0b11111000;
	
	//nulstiller overflow flag
	TIFR1 = 0b00000001;
	
	//tillad global interupts igen
	sei();
	
}


void X10transmitterApp::transmitLine()
{
	
	//if(!transmitRdy_)
	//{
	////if new line not ready, do nothing (transmit 0)
	//transmit(1);
	//}

	if(transmitRdy_ && (TXindex_ <= 49))
	{
		//transmit buffer if not done and ready to transmit
		transmit(TXbuffer_[TXindex_]);
		TXindex_++;
	}
	
	//else transmit nothing
	if(TXindex_ >= 49)
	{
		//fill array with start bit and nothing
		
		//for(size_t i = 3; i <= 50; i++)
		//{
		//buffer_[i] = 0;
		//}
		
		for(size_t i = 0; i <= 49; i++)
		{
			TXbuffer_[i] = 0;
		}
		//reset index.
		TXindex_ = 0;
		//transmit is done
		transmitDone_ = true;
		//not ready to transmit new line aka. ready to read new line
		transmitRdy_ = false;
	}
	
}

void X10transmitterApp::write2Buffer(char* cmd)
{
	
	translate(cmd);
	
	//**Startcode**//
	
	for (size_t i = 0; i <= 2; i++)
	{
		TXbuffer_[i] = 1;
	}
	
	TXbuffer_[3] = 0;
	
	
	//**fill buffer with translated cmd**//
	
	for(size_t i = 4; i <= 21; i++)
	{
		TXbuffer_[i] = translated[i-4];
	}
	
	//new line ready to transmit, reset index, transmit not done
	
	for(size_t i = 0; i <= 21; i++)
	{
		TXbuffer_[i+22] = TXbuffer_[i];
	}
	
	transmitDone_ = false;
	transmitRdy_ = true;
	TXindex_ = 0;
	
}

void X10transmitterApp::translate(char* msg)
{
	
	switch(msg[0])
	{
		//<< A-commands >>//
		case 'A':
		{
			int letterArray [] = {0, 1, 1, 0, 1, 0, 0, 1}; // letter: A
			switch(msg[1])
			{
				case '0':
				switch(msg[2])
				{
					case '1':
					{
						int numArray [] = {0, 1, 1, 0, 1, 0, 0, 1, 0, 1}; // number: 1
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '2':
					{
						int numArray [] = {1, 0, 1, 0, 1, 0, 0, 1, 0, 1}; // number: 2
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '3':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 0, 1, 0, 1}; // number: 3
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '4':
					{
						int numArray [] = {1, 0, 0, 1, 1, 0, 0, 1, 0, 1}; // number: 4
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '5':
					{
						int numArray [] = {0, 1, 0, 1, 0, 1, 1, 0, 0, 1}; // number: 5
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '6':
					{
						int numArray [] = {1, 0, 0, 1, 0, 1, 1, 0, 0, 1}; // number: 6
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '7':
					{
						int numArray [] = {0, 1, 1, 0, 0, 1, 1, 0, 0, 1}; // number: 7
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '8':
					{
						int numArray [] = {1, 0, 1, 0, 0, 1, 1, 0, 0, 1}; // number: 8
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '9':
					{
						int numArray [] = {0, 1, 1, 0, 1, 0, 1, 0, 0, 1}; // number: 9
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
				}
				break;
				
				case '1':
				switch(msg[2])
				{
					case '0':
					{
						int numArray [] = {1, 0, 1, 0, 1, 0, 1, 0, 0, 1}; // number: 10
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '1':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 1, 0, 0, 1}; // number: 11
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '2':
					{
						int numArray [] = {1, 0, 0, 1, 1, 0, 1, 0, 0, 1}; // number: 12
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '3':
					{
						int numArray [] = {0, 1, 0, 1, 0, 1, 0, 1 ,0, 1}; // number 13
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '4':
					{
						int numArray [] = {1, 0, 0, 1, 0, 1, 0, 1, 0, 1}; // number: 14
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '5':
					{
						int numArray [] = {0, 1, 1, 0, 0, 1, 0, 1, 0, 1}; // number: 15
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '6':
					{
						int numArray [] = {1, 0, 1, 0, 0, 1, 0, 1, 0, 1}; // number: 16
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
				}
				break;
				
				case  'O':
				switch(msg[2])
				{
					case 'N':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 0, 1, 1, 0}; // on-command
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case 'F':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 1, 0, 1, 0}; // off-command
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
				}
				break;

			}
			break;
		}
		
		
		
		
		//<< B-commands >>//
		case 'B':
		{
			int letterArray [] = {1, 0, 1, 0, 1, 0, 0, 1}; // letter: B
			switch(msg[1])
			{
				case '0':
				switch(msg[2])
				{
					case '1':
					{
						int numArray [] = {0, 1, 1, 0, 1, 0, 0, 1, 0, 1}; // number: 1
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '2':
					{
						int numArray [] = {1, 0, 1, 0, 1, 0, 0, 1, 0, 1}; // number: 2
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '3':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 0, 1, 0, 1}; // number: 3
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '4':
					{
						int numArray [] = {1, 0, 0, 1, 1, 0, 0, 1, 0, 1}; // number: 4
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '5':
					{
						int numArray [] = {0, 1, 0, 1, 0, 1, 1, 0, 0, 1}; // number: 5
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '6':
					{
						int numArray [] = {1, 0, 0, 1, 0, 1, 1, 0, 0, 1}; // number: 6
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '7':
					{
						int numArray [] = {0, 1, 1, 0, 0, 1, 1, 0, 0, 1}; // number: 7
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '8':
					{
						int numArray [] = {1, 0, 1, 0, 0, 1, 1, 0, 0, 1}; // number: 8
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '9':
					{
						int numArray [] = {0, 1, 1, 0, 1, 0, 1, 0, 0, 1}; // number: 9
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
				}
				break;
				
				case '1':
				switch(msg[2])
				{
					case '0':
					{
						int numArray [] = {1, 0, 1, 0, 1, 0, 1, 0, 0, 1}; // number: 10
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '1':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 1, 0, 0, 1}; // number: 11
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '2':
					{
						int numArray [] = {1, 0, 0, 1, 1, 0, 1, 0, 0, 1}; // number: 12
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '3':
					{
						int numArray [] = {0, 1, 0, 1, 0, 1, 0, 1 ,0, 1}; // number 13
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '4':
					{
						int numArray [] = {1, 0, 0, 1, 0, 1, 0, 1, 0, 1}; // number: 14
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '5':
					{
						int numArray [] = {0, 1, 1, 0, 0, 1, 0, 1, 0, 1}; // number: 15
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '6':
					{
						int numArray [] = {1, 0, 1, 0, 0, 1, 0, 1, 0, 1}; // number: 16
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
				}
				break;
				
				case  'O':
				switch(msg[2])
				{
					case 'N':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 0, 1, 1, 0}; // on-command
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case 'F':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 1, 0, 1, 0}; // off-command
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
				}
				break;

			}
			break;
		}
		
		
		
		//<< C-commands >>//
		case 'C':
		{
			int letterArray [] = {0, 1, 0, 1, 1, 0, 0, 1}; // letter: C
			switch(msg[1])
			{
				case '0':
				switch(msg[2])
				{
					case '1':
					{
						int numArray [] = {0, 1, 1, 0, 1, 0, 0, 1, 0, 1}; // number: 1
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '2':
					{
						int numArray [] = {1, 0, 1, 0, 1, 0, 0, 1, 0, 1}; // number: 2
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '3':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 0, 1, 0, 1}; // number: 3
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '4':
					{
						int numArray [] = {1, 0, 0, 1, 1, 0, 0, 1, 0, 1}; // number: 4
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '5':
					{
						int numArray [] = {0, 1, 0, 1, 0, 1, 1, 0, 0, 1}; // number: 5
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '6':
					{
						int numArray [] = {1, 0, 0, 1, 0, 1, 1, 0, 0, 1}; // number: 6
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '7':
					{
						int numArray [] = {0, 1, 1, 0, 0, 1, 1, 0, 0, 1}; // number: 7
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '8':
					{
						int numArray [] = {1, 0, 1, 0, 0, 1, 1, 0, 0, 1}; // number: 8
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '9':
					{
						int numArray [] = {0, 1, 1, 0, 1, 0, 1, 0, 0, 1}; // number: 9
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
				}
				break;
				
				case '1':
				switch(msg[2])
				{
					case '0':
					{
						int numArray [] = {1, 0, 1, 0, 1, 0, 1, 0, 0, 1}; // number: 10
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '1':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 1, 0, 0, 1}; // number: 11
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '2':
					{
						int numArray [] = {1, 0, 0, 1, 1, 0, 1, 0, 0, 1}; // number: 12
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '3':
					{
						int numArray [] = {0, 1, 0, 1, 0, 1, 0, 1 ,0, 1}; // number 13
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '4':
					{
						int numArray [] = {1, 0, 0, 1, 0, 1, 0, 1, 0, 1}; // number: 14
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '5':
					{
						int numArray [] = {0, 1, 1, 0, 0, 1, 0, 1, 0, 1}; // number: 15
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '6':
					{
						int numArray [] = {1, 0, 1, 0, 0, 1, 0, 1, 0, 1}; // number: 16
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
				}
				break;
				
				case  'O':
				switch(msg[2])
				{
					case 'N':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 0, 1, 1, 0}; // on-command
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case 'F':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 1, 0, 1, 0}; // off-command
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
				}
				break;

			}
			break;
		}
		
		
		
		
		
		//<< D-commands >>//
		case 'D':
		{
			int letterArray [] = {1, 0, 0, 1, 1, 0, 0, 1}; // letter: D
			switch(msg[1])
			{
				case '0':
				switch(msg[2])
				{
					case '1':
					{
						int numArray [] = {0, 1, 1, 0, 1, 0, 0, 1, 0, 1}; // number: 1
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '2':
					{
						int numArray [] = {1, 0, 1, 0, 1, 0, 0, 1, 0, 1}; // number: 2
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '3':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 0, 1, 0, 1}; // number: 3
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '4':
					{
						int numArray [] = {1, 0, 0, 1, 1, 0, 0, 1, 0, 1}; // number: 4
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '5':
					{
						int numArray [] = {0, 1, 0, 1, 0, 1, 1, 0, 0, 1}; // number: 5
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '6':
					{
						int numArray [] = {1, 0, 0, 1, 0, 1, 1, 0, 0, 1}; // number: 6
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '7':
					{
						int numArray [] = {0, 1, 1, 0, 0, 1, 1, 0, 0, 1}; // number: 7
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '8':
					{
						int numArray [] = {1, 0, 1, 0, 0, 1, 1, 0, 0, 1}; // number: 8
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '9':
					{
						int numArray [] = {0, 1, 1, 0, 1, 0, 1, 0, 0, 1}; // number: 9
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
				}
				break;
				
				case '1':
				switch(msg[2])
				{
					case '0':
					{
						int numArray [] = {1, 0, 1, 0, 1, 0, 1, 0, 0, 1}; // number: 10
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '1':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 1, 0, 0, 1}; // number: 11
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '2':
					{
						int numArray [] = {1, 0, 0, 1, 1, 0, 1, 0, 0, 1}; // number: 12
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '3':
					{
						int numArray [] = {0, 1, 0, 1, 0, 1, 0, 1 ,0, 1}; // number 13
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '4':
					{
						int numArray [] = {1, 0, 0, 1, 0, 1, 0, 1, 0, 1}; // number: 14
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '5':
					{
						int numArray [] = {0, 1, 1, 0, 0, 1, 0, 1, 0, 1}; // number: 15
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '6':
					{
						int numArray [] = {1, 0, 1, 0, 0, 1, 0, 1, 0, 1}; // number: 16
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
				}
				break;
				
				case  'O':
				switch(msg[2])
				{
					case 'N':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 0, 1, 1, 0}; // on-command
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case 'F':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 1, 0, 1, 0}; // off-command
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
				}
				break;

			}
			break;
		}
		
		
		
		
		//<< E-commands >>//
		case 'E':
		{
			int letterArray [] = {0, 1, 0, 1, 0, 1, 1, 0}; // letter: E
			switch(msg[1])
			{
				case '0':
				switch(msg[2])
				{
					case '1':
					{
						int numArray [] = {0, 1, 1, 0, 1, 0, 0, 1, 0, 1}; // number: 1
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '2':
					{
						int numArray [] = {1, 0, 1, 0, 1, 0, 0, 1, 0, 1}; // number: 2
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '3':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 0, 1, 0, 1}; // number: 3
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '4':
					{
						int numArray [] = {1, 0, 0, 1, 1, 0, 0, 1, 0, 1}; // number: 4
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '5':
					{
						int numArray [] = {0, 1, 0, 1, 0, 1, 1, 0, 0, 1}; // number: 5
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '6':
					{
						int numArray [] = {1, 0, 0, 1, 0, 1, 1, 0, 0, 1}; // number: 6
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '7':
					{
						int numArray [] = {0, 1, 1, 0, 0, 1, 1, 0, 0, 1}; // number: 7
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '8':
					{
						int numArray [] = {1, 0, 1, 0, 0, 1, 1, 0, 0, 1}; // number: 8
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '9':
					{
						int numArray [] = {0, 1, 1, 0, 1, 0, 1, 0, 0, 1}; // number: 9
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
				}
				break;
				
				case '1':
				switch(msg[2])
				{
					case '0':
					{
						int numArray [] = {1, 0, 1, 0, 1, 0, 1, 0, 0, 1}; // number: 10
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '1':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 1, 0, 0, 1}; // number: 11
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '2':
					{
						int numArray [] = {1, 0, 0, 1, 1, 0, 1, 0, 0, 1}; // number: 12
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '3':
					{
						int numArray [] = {0, 1, 0, 1, 0, 1, 0, 1 ,0, 1}; // number 13
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '4':
					{
						int numArray [] = {1, 0, 0, 1, 0, 1, 0, 1, 0, 1}; // number: 14
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '5':
					{
						int numArray [] = {0, 1, 1, 0, 0, 1, 0, 1, 0, 1}; // number: 15
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '6':
					{
						int numArray [] = {1, 0, 1, 0, 0, 1, 0, 1, 0, 1}; // number: 16
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
				}
				break;
				
				case  'O':
				switch(msg[2])
				{
					case 'N':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 0, 1, 1, 0}; // on-command
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case 'F':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 1, 0, 1, 0}; // off-command
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
				}
				break;

			}
			break;
		}
		
		
		
		//<< F-commands >>//
		case 'F':
		{
			int letterArray [] = {1, 0, 0, 1, 0, 1, 1, 0}; // letter: F
			switch(msg[1])
			{
				case '0':
				switch(msg[2])
				{
					case '1':
					{
						int numArray [] = {0, 1, 1, 0, 1, 0, 0, 1, 0, 1}; // number: 1
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '2':
					{
						int numArray [] = {1, 0, 1, 0, 1, 0, 0, 1, 0, 1}; // number: 2
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '3':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 0, 1, 0, 1}; // number: 3
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '4':
					{
						int numArray [] = {1, 0, 0, 1, 1, 0, 0, 1, 0, 1}; // number: 4
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '5':
					{
						int numArray [] = {0, 1, 0, 1, 0, 1, 1, 0, 0, 1}; // number: 5
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '6':
					{
						int numArray [] = {1, 0, 0, 1, 0, 1, 1, 0, 0, 1}; // number: 6
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '7':
					{
						int numArray [] = {0, 1, 1, 0, 0, 1, 1, 0, 0, 1}; // number: 7
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '8':
					{
						int numArray [] = {1, 0, 1, 0, 0, 1, 1, 0, 0, 1}; // number: 8
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '9':
					{
						int numArray [] = {0, 1, 1, 0, 1, 0, 1, 0, 0, 1}; // number: 9
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
				}
				break;
				
				case '1':
				switch(msg[2])
				{
					case '0':
					{
						int numArray [] = {1, 0, 1, 0, 1, 0, 1, 0, 0, 1}; // number: 10
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '1':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 1, 0, 0, 1}; // number: 11
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '2':
					{
						int numArray [] = {1, 0, 0, 1, 1, 0, 1, 0, 0, 1}; // number: 12
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '3':
					{
						int numArray [] = {0, 1, 0, 1, 0, 1, 0, 1 ,0, 1}; // number 13
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '4':
					{
						int numArray [] = {1, 0, 0, 1, 0, 1, 0, 1, 0, 1}; // number: 14
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '5':
					{
						int numArray [] = {0, 1, 1, 0, 0, 1, 0, 1, 0, 1}; // number: 15
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '6':
					{
						int numArray [] = {1, 0, 1, 0, 0, 1, 0, 1, 0, 1}; // number: 16
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
				}
				break;
				
				case  'O':
				switch(msg[2])
				{
					case 'N':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 0, 1, 1, 0}; // on-command
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case 'F':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 1, 0, 1, 0}; // off-command
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
				}
				break;

			}
			break;
		}
		
		
		
		
		//<< G-commands >>//
		case 'G':
		{
			int letterArray [] = {0, 1, 1, 0, 0, 1, 1, 0}; // letter: G
			switch(msg[1])
			{
				case '0':
				switch(msg[2])
				{
					case '1':
					{
						int numArray [] = {0, 1, 1, 0, 1, 0, 0, 1, 0, 1}; // number: 1
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '2':
					{
						int numArray [] = {1, 0, 1, 0, 1, 0, 0, 1, 0, 1}; // number: 2
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '3':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 0, 1, 0, 1}; // number: 3
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '4':
					{
						int numArray [] = {1, 0, 0, 1, 1, 0, 0, 1, 0, 1}; // number: 4
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '5':
					{
						int numArray [] = {0, 1, 0, 1, 0, 1, 1, 0, 0, 1}; // number: 5
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '6':
					{
						int numArray [] = {1, 0, 0, 1, 0, 1, 1, 0, 0, 1}; // number: 6
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '7':
					{
						int numArray [] = {0, 1, 1, 0, 0, 1, 1, 0, 0, 1}; // number: 7
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '8':
					{
						int numArray [] = {1, 0, 1, 0, 0, 1, 1, 0, 0, 1}; // number: 8
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '9':
					{
						int numArray [] = {0, 1, 1, 0, 1, 0, 1, 0, 0, 1}; // number: 9
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
				}
				break;
				
				case '1':
				switch(msg[2])
				{
					case '0':
					{
						int numArray [] = {1, 0, 1, 0, 1, 0, 1, 0, 0, 1}; // number: 10
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '1':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 1, 0, 0, 1}; // number: 11
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '2':
					{
						int numArray [] = {1, 0, 0, 1, 1, 0, 1, 0, 0, 1}; // number: 12
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '3':
					{
						int numArray [] = {0, 1, 0, 1, 0, 1, 0, 1 ,0, 1}; // number 13
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '4':
					{
						int numArray [] = {1, 0, 0, 1, 0, 1, 0, 1, 0, 1}; // number: 14
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '5':
					{
						int numArray [] = {0, 1, 1, 0, 0, 1, 0, 1, 0, 1}; // number: 15
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '6':
					{
						int numArray [] = {1, 0, 1, 0, 0, 1, 0, 1, 0, 1}; // number: 16
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
				}
				break;
				
				case  'O':
				switch(msg[2])
				{
					case 'N':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 0, 1, 1, 0}; // on-command
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case 'F':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 1, 0, 1, 0}; // off-command
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
				}
				break;

			}
			break;
		}
		
		
		
		
		//<< H-commands >>//
		case 'H':
		{
			int letterArray [] = {1, 0, 1, 0, 0, 1, 1, 0}; // letter: H
			switch(msg[1])
			{
				case '0':
				switch(msg[2])
				{
					case '1':
					{
						int numArray [] = {0, 1, 1, 0, 1, 0, 0, 1, 0, 1}; // number: 1
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '2':
					{
						int numArray [] = {1, 0, 1, 0, 1, 0, 0, 1, 0, 1}; // number: 2
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '3':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 0, 1, 0, 1}; // number: 3
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '4':
					{
						int numArray [] = {1, 0, 0, 1, 1, 0, 0, 1, 0, 1}; // number: 4
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '5':
					{
						int numArray [] = {0, 1, 0, 1, 0, 1, 1, 0, 0, 1}; // number: 5
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '6':
					{
						int numArray [] = {1, 0, 0, 1, 0, 1, 1, 0, 0, 1}; // number: 6
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '7':
					{
						int numArray [] = {0, 1, 1, 0, 0, 1, 1, 0, 0, 1}; // number: 7
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '8':
					{
						int numArray [] = {1, 0, 1, 0, 0, 1, 1, 0, 0, 1}; // number: 8
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '9':
					{
						int numArray [] = {0, 1, 1, 0, 1, 0, 1, 0, 0, 1}; // number: 9
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
				}
				break;
				
				case '1':
				switch(msg[2])
				{
					case '0':
					{
						int numArray [] = {1, 0, 1, 0, 1, 0, 1, 0, 0, 1}; // number: 10
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '1':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 1, 0, 0, 1}; // number: 11
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '2':
					{
						int numArray [] = {1, 0, 0, 1, 1, 0, 1, 0, 0, 1}; // number: 12
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '3':
					{
						int numArray [] = {0, 1, 0, 1, 0, 1, 0, 1 ,0, 1}; // number 13
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '4':
					{
						int numArray [] = {1, 0, 0, 1, 0, 1, 0, 1, 0, 1}; // number: 14
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '5':
					{
						int numArray [] = {0, 1, 1, 0, 0, 1, 0, 1, 0, 1}; // number: 15
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '6':
					{
						int numArray [] = {1, 0, 1, 0, 0, 1, 0, 1, 0, 1}; // number: 16
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
				}
				break;
				
				case  'O':
				switch(msg[2])
				{
					case 'N':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 0, 1, 1, 0}; // on-command
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case 'F':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 1, 0, 1, 0}; // off-command
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
				}
				break;

			}
			break;
		}
		
		
		
		
		
		//<< I-commands >>//
		case 'I':
		{
			int letterArray [] = {0, 1, 1, 0, 1, 0, 1, 0}; // letter: I
			switch(msg[1])
			{
				case '0':
				switch(msg[2])
				{
					case '1':
					{
						int numArray [] = {0, 1, 1, 0, 1, 0, 0, 1, 0, 1}; // number: 1
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '2':
					{
						int numArray [] = {1, 0, 1, 0, 1, 0, 0, 1, 0, 1}; // number: 2
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '3':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 0, 1, 0, 1}; // number: 3
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '4':
					{
						int numArray [] = {1, 0, 0, 1, 1, 0, 0, 1, 0, 1}; // number: 4
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '5':
					{
						int numArray [] = {0, 1, 0, 1, 0, 1, 1, 0, 0, 1}; // number: 5
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '6':
					{
						int numArray [] = {1, 0, 0, 1, 0, 1, 1, 0, 0, 1}; // number: 6
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '7':
					{
						int numArray [] = {0, 1, 1, 0, 0, 1, 1, 0, 0, 1}; // number: 7
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '8':
					{
						int numArray [] = {1, 0, 1, 0, 0, 1, 1, 0, 0, 1}; // number: 8
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '9':
					{
						int numArray [] = {0, 1, 1, 0, 1, 0, 1, 0, 0, 1}; // number: 9
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
				}
				break;
				
				case '1':
				switch(msg[2])
				{
					case '0':
					{
						int numArray [] = {1, 0, 1, 0, 1, 0, 1, 0, 0, 1}; // number: 10
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '1':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 1, 0, 0, 1}; // number: 11
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '2':
					{
						int numArray [] = {1, 0, 0, 1, 1, 0, 1, 0, 0, 1}; // number: 12
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '3':
					{
						int numArray [] = {0, 1, 0, 1, 0, 1, 0, 1 ,0, 1}; // number 13
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '4':
					{
						int numArray [] = {1, 0, 0, 1, 0, 1, 0, 1, 0, 1}; // number: 14
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '5':
					{
						int numArray [] = {0, 1, 1, 0, 0, 1, 0, 1, 0, 1}; // number: 15
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '6':
					{
						int numArray [] = {1, 0, 1, 0, 0, 1, 0, 1, 0, 1}; // number: 16
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
				}
				break;
				
				case  'O':
				switch(msg[2])
				{
					case 'N':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 0, 1, 1, 0}; // on-command
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case 'F':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 1, 0, 1, 0}; // off-command
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
				}
				break;

			}
			break;
		}
		
		
		
		
		//<< J-commands >>//
		case 'J':
		{
			int letterArray [] = {1, 0, 1, 0, 1, 0, 1, 0}; // letter: J
			switch(msg[1])
			{
				case '0':
				switch(msg[2])
				{
					case '1':
					{
						int numArray [] = {0, 1, 1, 0, 1, 0, 0, 1, 0, 1}; // number: 1
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '2':
					{
						int numArray [] = {1, 0, 1, 0, 1, 0, 0, 1, 0, 1}; // number: 2
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '3':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 0, 1, 0, 1}; // number: 3
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '4':
					{
						int numArray [] = {1, 0, 0, 1, 1, 0, 0, 1, 0, 1}; // number: 4
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '5':
					{
						int numArray [] = {0, 1, 0, 1, 0, 1, 1, 0, 0, 1}; // number: 5
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '6':
					{
						int numArray [] = {1, 0, 0, 1, 0, 1, 1, 0, 0, 1}; // number: 6
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '7':
					{
						int numArray [] = {0, 1, 1, 0, 0, 1, 1, 0, 0, 1}; // number: 7
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '8':
					{
						int numArray [] = {1, 0, 1, 0, 0, 1, 1, 0, 0, 1}; // number: 8
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '9':
					{
						int numArray [] = {0, 1, 1, 0, 1, 0, 1, 0, 0, 1}; // number: 9
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
				}
				break;
				
				case '1':
				switch(msg[2])
				{
					case '0':
					{
						int numArray [] = {1, 0, 1, 0, 1, 0, 1, 0, 0, 1}; // number: 10
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '1':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 1, 0, 0, 1}; // number: 11
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '2':
					{
						int numArray [] = {1, 0, 0, 1, 1, 0, 1, 0, 0, 1}; // number: 12
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '3':
					{
						int numArray [] = {0, 1, 0, 1, 0, 1, 0, 1 ,0, 1}; // number 13
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '4':
					{
						int numArray [] = {1, 0, 0, 1, 0, 1, 0, 1, 0, 1}; // number: 14
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '5':
					{
						int numArray [] = {0, 1, 1, 0, 0, 1, 0, 1, 0, 1}; // number: 15
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '6':
					{
						int numArray [] = {1, 0, 1, 0, 0, 1, 0, 1, 0, 1}; // number: 16
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
				}
				break;
				
				case  'O':
				switch(msg[2])
				{
					case 'N':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 0, 1, 1, 0}; // on-command
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case 'F':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 1, 0, 1, 0}; // off-command
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
				}
				break;

			}
			break;
		}
		
		
		
		
		
		
		//<< K-commands >>//
		case 'K':
		{
			int letterArray [] = {0, 1, 0, 1, 1, 0, 1, 0}; // letter: K
			switch(msg[1])
			{
				case '0':
				switch(msg[2])
				{
					case '1':
					{
						int numArray [] = {0, 1, 1, 0, 1, 0, 0, 1, 0, 1}; // number: 1
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '2':
					{
						int numArray [] = {1, 0, 1, 0, 1, 0, 0, 1, 0, 1}; // number: 2
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '3':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 0, 1, 0, 1}; // number: 3
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '4':
					{
						int numArray [] = {1, 0, 0, 1, 1, 0, 0, 1, 0, 1}; // number: 4
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '5':
					{
						int numArray [] = {0, 1, 0, 1, 0, 1, 1, 0, 0, 1}; // number: 5
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '6':
					{
						int numArray [] = {1, 0, 0, 1, 0, 1, 1, 0, 0, 1}; // number: 6
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '7':
					{
						int numArray [] = {0, 1, 1, 0, 0, 1, 1, 0, 0, 1}; // number: 7
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '8':
					{
						int numArray [] = {1, 0, 1, 0, 0, 1, 1, 0, 0, 1}; // number: 8
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '9':
					{
						int numArray [] = {0, 1, 1, 0, 1, 0, 1, 0, 0, 1}; // number: 9
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
				}
				break;
				
				case '1':
				switch(msg[2])
				{
					case '0':
					{
						int numArray [] = {1, 0, 1, 0, 1, 0, 1, 0, 0, 1}; // number: 10
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '1':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 1, 0, 0, 1}; // number: 11
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '2':
					{
						int numArray [] = {1, 0, 0, 1, 1, 0, 1, 0, 0, 1}; // number: 12
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '3':
					{
						int numArray [] = {0, 1, 0, 1, 0, 1, 0, 1 ,0, 1}; // number 13
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '4':
					{
						int numArray [] = {1, 0, 0, 1, 0, 1, 0, 1, 0, 1}; // number: 14
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '5':
					{
						int numArray [] = {0, 1, 1, 0, 0, 1, 0, 1, 0, 1}; // number: 15
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '6':
					{
						int numArray [] = {1, 0, 1, 0, 0, 1, 0, 1, 0, 1}; // number: 16
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
				}
				break;
				
				case  'O':
				switch(msg[2])
				{
					case 'N':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 0, 1, 1, 0}; // on-command
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case 'F':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 1, 0, 1, 0}; // off-command
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
				}
				break;

			}
			break;
		}
		
		
		
		
		//<< L-commands >>//
		case 'L':
		{
			int letterArray [] = {1, 0, 0, 1, 1, 0, 1, 0}; // letter: L
			switch(msg[1])
			{
				case '0':
				switch(msg[2])
				{
					case '1':
					{
						int numArray [] = {0, 1, 1, 0, 1, 0, 0, 1, 0, 1}; // number: 1
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '2':
					{
						int numArray [] = {1, 0, 1, 0, 1, 0, 0, 1, 0, 1}; // number: 2
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '3':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 0, 1, 0, 1}; // number: 3
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '4':
					{
						int numArray [] = {1, 0, 0, 1, 1, 0, 0, 1, 0, 1}; // number: 4
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '5':
					{
						int numArray [] = {0, 1, 0, 1, 0, 1, 1, 0, 0, 1}; // number: 5
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '6':
					{
						int numArray [] = {1, 0, 0, 1, 0, 1, 1, 0, 0, 1}; // number: 6
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '7':
					{
						int numArray [] = {0, 1, 1, 0, 0, 1, 1, 0, 0, 1}; // number: 7
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '8':
					{
						int numArray [] = {1, 0, 1, 0, 0, 1, 1, 0, 0, 1}; // number: 8
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '9':
					{
						int numArray [] = {0, 1, 1, 0, 1, 0, 1, 0, 0, 1}; // number: 9
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
				}
				break;
				
				case '1':
				switch(msg[2])
				{
					case '0':
					{
						int numArray [] = {1, 0, 1, 0, 1, 0, 1, 0, 0, 1}; // number: 10
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '1':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 1, 0, 0, 1}; // number: 11
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '2':
					{
						int numArray [] = {1, 0, 0, 1, 1, 0, 1, 0, 0, 1}; // number: 12
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '3':
					{
						int numArray [] = {0, 1, 0, 1, 0, 1, 0, 1 ,0, 1}; // number 13
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '4':
					{
						int numArray [] = {1, 0, 0, 1, 0, 1, 0, 1, 0, 1}; // number: 14
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '5':
					{
						int numArray [] = {0, 1, 1, 0, 0, 1, 0, 1, 0, 1}; // number: 15
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '6':
					{
						int numArray [] = {1, 0, 1, 0, 0, 1, 0, 1, 0, 1}; // number: 16
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
				}
				break;
				
				case  'O':
				switch(msg[2])
				{
					case 'N':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 0, 1, 1, 0}; // on-command
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case 'F':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 1, 0, 1, 0}; // off-command
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
				}
				break;

			}
			break;
		}
		
		
		
		
		
		//<< M-commands >>//
		case 'M':
		{
			int letterArray [] = {0, 1, 0, 1, 0, 1, 0, 1}; // letter: M
			switch(msg[1])
			{
				case '0':
				switch(msg[2])
				{
					case '1':
					{
						int numArray [] = {0, 1, 1, 0, 1, 0, 0, 1, 0, 1}; // number: 1
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '2':
					{
						int numArray [] = {1, 0, 1, 0, 1, 0, 0, 1, 0, 1}; // number: 2
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '3':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 0, 1, 0, 1}; // number: 3
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '4':
					{
						int numArray [] = {1, 0, 0, 1, 1, 0, 0, 1, 0, 1}; // number: 4
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '5':
					{
						int numArray [] = {0, 1, 0, 1, 0, 1, 1, 0, 0, 1}; // number: 5
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '6':
					{
						int numArray [] = {1, 0, 0, 1, 0, 1, 1, 0, 0, 1}; // number: 6
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '7':
					{
						int numArray [] = {0, 1, 1, 0, 0, 1, 1, 0, 0, 1}; // number: 7
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '8':
					{
						int numArray [] = {1, 0, 1, 0, 0, 1, 1, 0, 0, 1}; // number: 8
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '9':
					{
						int numArray [] = {0, 1, 1, 0, 1, 0, 1, 0, 0, 1}; // number: 9
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
				}
				break;
				
				case '1':
				switch(msg[2])
				{
					case '0':
					{
						int numArray [] = {1, 0, 1, 0, 1, 0, 1, 0, 0, 1}; // number: 10
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '1':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 1, 0, 0, 1}; // number: 11
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '2':
					{
						int numArray [] = {1, 0, 0, 1, 1, 0, 1, 0, 0, 1}; // number: 12
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '3':
					{
						int numArray [] = {0, 1, 0, 1, 0, 1, 0, 1 ,0, 1}; // number 13
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '4':
					{
						int numArray [] = {1, 0, 0, 1, 0, 1, 0, 1, 0, 1}; // number: 14
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '5':
					{
						int numArray [] = {0, 1, 1, 0, 0, 1, 0, 1, 0, 1}; // number: 15
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '6':
					{
						int numArray [] = {1, 0, 1, 0, 0, 1, 0, 1, 0, 1}; // number: 16
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
				}
				break;
				
				case  'O':
				switch(msg[2])
				{
					case 'N':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 0, 1, 1, 0}; // on-command
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case 'F':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 1, 0, 1, 0}; // off-command
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
				}
				break;

			}
			break;
		}
		
		
		
		
		//<< N-commands >>//
		case 'N':
		{
			int letterArray [] = {1, 0, 0, 1, 0, 1, 0, 1}; // letter: N
			switch(msg[1])
			{
				case '0':
				switch(msg[2])
				{
					case '1':
					{
						int numArray [] = {0, 1, 1, 0, 1, 0, 0, 1, 0, 1}; // number: 1
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '2':
					{
						int numArray [] = {1, 0, 1, 0, 1, 0, 0, 1, 0, 1}; // number: 2
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '3':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 0, 1, 0, 1}; // number: 3
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '4':
					{
						int numArray [] = {1, 0, 0, 1, 1, 0, 0, 1, 0, 1}; // number: 4
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '5':
					{
						int numArray [] = {0, 1, 0, 1, 0, 1, 1, 0, 0, 1}; // number: 5
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '6':
					{
						int numArray [] = {1, 0, 0, 1, 0, 1, 1, 0, 0, 1}; // number: 6
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '7':
					{
						int numArray [] = {0, 1, 1, 0, 0, 1, 1, 0, 0, 1}; // number: 7
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '8':
					{
						int numArray [] = {1, 0, 1, 0, 0, 1, 1, 0, 0, 1}; // number: 8
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '9':
					{
						int numArray [] = {0, 1, 1, 0, 1, 0, 1, 0, 0, 1}; // number: 9
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
				}
				break;
				
				case '1':
				switch(msg[2])
				{
					case '0':
					{
						int numArray [] = {1, 0, 1, 0, 1, 0, 1, 0, 0, 1}; // number: 10
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '1':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 1, 0, 0, 1}; // number: 11
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '2':
					{
						int numArray [] = {1, 0, 0, 1, 1, 0, 1, 0, 0, 1}; // number: 12
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '3':
					{
						int numArray [] = {0, 1, 0, 1, 0, 1, 0, 1 ,0, 1}; // number 13
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '4':
					{
						int numArray [] = {1, 0, 0, 1, 0, 1, 0, 1, 0, 1}; // number: 14
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '5':
					{
						int numArray [] = {0, 1, 1, 0, 0, 1, 0, 1, 0, 1}; // number: 15
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '6':
					{
						int numArray [] = {1, 0, 1, 0, 0, 1, 0, 1, 0, 1}; // number: 16
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
				}
				break;
				
				case  'O':
				switch(msg[2])
				{
					case 'N':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 0, 1, 1, 0}; // on-command
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case 'F':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 1, 0, 1, 0}; // off-command
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
				}
				break;

			}
			break;
		}
		
		
		
		
		
		//<< O-commands >>//
		case 'O':
		{
			int letterArray [] = {0, 1, 1, 0, 0, 1, 0, 1}; // letter: O
			switch(msg[1])
			{
				case '0':
				switch(msg[2])
				{
					case '1':
					{
						int numArray [] = {0, 1, 1, 0, 1, 0, 0, 1, 0, 1}; // number: 1
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '2':
					{
						int numArray [] = {1, 0, 1, 0, 1, 0, 0, 1, 0, 1}; // number: 2
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '3':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 0, 1, 0, 1}; // number: 3
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '4':
					{
						int numArray [] = {1, 0, 0, 1, 1, 0, 0, 1, 0, 1}; // number: 4
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '5':
					{
						int numArray [] = {0, 1, 0, 1, 0, 1, 1, 0, 0, 1}; // number: 5
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '6':
					{
						int numArray [] = {1, 0, 0, 1, 0, 1, 1, 0, 0, 1}; // number: 6
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '7':
					{
						int numArray [] = {0, 1, 1, 0, 0, 1, 1, 0, 0, 1}; // number: 7
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '8':
					{
						int numArray [] = {1, 0, 1, 0, 0, 1, 1, 0, 0, 1}; // number: 8
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '9':
					{
						int numArray [] = {0, 1, 1, 0, 1, 0, 1, 0, 0, 1}; // number: 9
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
				}
				break;
				
				case '1':
				switch(msg[2])
				{
					case '0':
					{
						int numArray [] = {1, 0, 1, 0, 1, 0, 1, 0, 0, 1}; // number: 10
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '1':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 1, 0, 0, 1}; // number: 11
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '2':
					{
						int numArray [] = {1, 0, 0, 1, 1, 0, 1, 0, 0, 1}; // number: 12
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '3':
					{
						int numArray [] = {0, 1, 0, 1, 0, 1, 0, 1 ,0, 1}; // number 13
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '4':
					{
						int numArray [] = {1, 0, 0, 1, 0, 1, 0, 1, 0, 1}; // number: 14
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '5':
					{
						int numArray [] = {0, 1, 1, 0, 0, 1, 0, 1, 0, 1}; // number: 15
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case '6':
					{
						int numArray [] = {1, 0, 1, 0, 0, 1, 0, 1, 0, 1}; // number: 16
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
				}
				break;
				
				case  'O':
				switch(msg[2])
				{
					case 'N':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 0, 1, 1, 0}; // on-command
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
					case 'F':
					{
						int numArray [] = {0, 1, 0, 1, 1, 0, 1, 0, 1, 0}; // off-command
						
						for (int i = 0; i < 8 ; i++)
						{
							translated [i] = letterArray[i];
						}
						for (int i = 8; i < 18; i++)
						{
							translated [i] = numArray[i];
						}
					}
					break;
					
				}
				break;

			}
			break;
		}
		
		
		
				
		//<< P-commands >>//
		case 'P':
		{
			switch(msg[1])
			{
				case '0':
				switch(msg[2])
				{
					case '1':
					{
						int newArray [] = {1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1};
						
						for (int i = 0; i < 18; i++)
						{
							translated [i] = newArray[i];
						}
					}
					break;
					
					case '2':
					{
						int newArray [] = {1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1};
						
						for (int i = 0; i < 18; i++)
						{
							translated [i] = newArray[i];
						}
					}
					break;
					
					case '3':
					{
						int newArray [] = {1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1};
						
						for (int i = 0; i < 18; i++)
						{
							translated [i] = newArray[i];
						}
					}
					break;
					
					case '4':
					{
						int newArray [] = {1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1};
						
						for (int i = 0; i < 18; i++)
						{
							translated [i] = newArray[i];
						}
					}
					break;
					
					case '5':
					{
						int newArray [] = {1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1};
						
						for (int i = 0; i < 18; i++)
						{
							translated [i] = newArray[i];
						}
					}
					break;
					
					case '6':
					{
						int newArray [] = {1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1};
						
						for (int i = 0; i < 18; i++)
						{
							translated [i] = newArray[i];
						}
					}
					break;
					
					case '7':
					{
						int newArray [] = {1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1};
						
						for (int i = 0; i < 18; i++)
						{
							translated [i] = newArray[i];
						}
					}
					break;
					
					case '8':
					{
						int newArray [] = {1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1};
						
						for (int i = 0; i < 18; i++)
						{
							translated [i] = newArray[i];
						}
					}
					break;
					
					case '9':
					{
						int newArray [] = {1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1};
						
						for (int i = 0; i < 18; i++)
						{
							translated [i] = newArray[i];
						}
					}
					break;
				}
				break;
				
				case '1':
				switch(msg[2])
				{
					case '1':
					{
						int newArray [] = {1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1};
						
						for (int i = 0; i < 18; i++)
						{
							translated [i] = newArray[i];
						}
					}
					break;
					
					case '2':
					{
						int newArray [] = {1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1};
						
						for (int i = 0; i < 18; i++)
						{
							translated [i] = newArray[i];
						}
					}
					break;
					
					case '3':
					{
						int newArray [] = {1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
						
						for (int i = 0; i < 18; i++)
						{
							translated [i] = newArray[i];
						}
					}
					break;
					
					case '4':
					{
						int newArray [] = {1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1};
						
						for (int i = 0; i < 18; i++)
						{
							translated [i] = newArray[i];
						}
					}
					break;
					
					case '5':
					{
						int newArray [] = {1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1};
						
						for (int i = 0; i < 18; i++)
						{
							translated [i] = newArray[i];
						}
					}
					break;
					
					case '6':
					{
						int newArray [] = {1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1};
						
						for (int i = 0; i < 18; i++)
						{
							translated [i] = newArray[i];
						}
					}
					break;
				}
				break;
				
				case  'O':
				switch(msg[2])
				{
					case 'N':
					{
						int newArray [] = {1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0};
						
						for (int i = 0; i < 18; i++)
						{
							translated [i] = newArray[i];
						}
					}
					break;
					
					case 'F':
					{
						int newArray [] = {1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0};
						
						for (int i = 0; i < 18; i++)
						{
							translated [i] = newArray[i];
						}
					}
					break;
					
				}
				break;

			}
		}
		break;
	}
}


void X10transmitterApp::shutTransmitter()
{
	//disabler alle interrupts (zero-cross)
	cli();
	
	//Stopper 120kHz HF-generator
	TCCR0B &= 0b11111000;
	
}

void X10transmitterApp::enableX10()
{
	EIMSK |= 0b00000001;	
}
void X10transmitterApp::disableX10()
{
	EIMSK &= 0b11111110;
}
// default destructor
X10transmitterApp::~X10transmitterApp()
{
} //~X10transmitterApp