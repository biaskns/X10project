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
	//sei();
	////INT0 interupt sker ved rising edge
	//EICRA |= 0b00000011;
	////Enabler ekstern INT0 som er zero-cross.
	//EIMSK |= 0b00000001;
	
	
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

void X10transmitterApp::write2Buffer(char write)
{
	//if not ready to transmit, read new line into buffer
	if((TXindex_ <= 21) && (!transmitRdy_))
	{
		TXbuffer_[TXindex_] = write;
		TXindex_++;
	}
	
	//new line ready to transmit, reset index, transmit not done
	if(TXindex_ >= 21)
	{
		for (size_t i = 0; i <= 21; i++)
		{
			TXbuffer_[i+22] = TXbuffer_[i];
		}
		
		transmitDone_ = false;
		transmitRdy_ = true;
		TXindex_ = 0;
	}
	
}

void X10transmitterApp::shutTransmitter()
{
	//disabler alle interrupts (zero-cross)
	cli();
	
	//Stopper 120kHz HF-generator
	TCCR0B &= 0b11111000;
	
}
// default destructor
X10transmitterApp::~X10transmitterApp()
{
} //~X10transmitterApp