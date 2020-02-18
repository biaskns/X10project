/*
 * CPPFile1.cpp
 *
 * Created: 27-03-2019 18:18:52
 *  Author: viksk
 */ 

#include "UART.h"
#include <avr/io.h>

#define F_CPU 16000000

UART::UART(int baudrate, char databits, char stopbits, char parity, char interrupts)
: baudrate_(baudrate == 110 ||baudrate == 600 || baudrate == 2400 || baudrate == 4800 || baudrate == 9600 || baudrate == 14400 ? baudrate : 9600),
 databits_(databits == (5 || 6 || 7 || 8) ? databits : 8),
 stopbits_(stopbits == (1 || 2) ? stopbits : 1),
 parity_(parity == ('e' || 'o' || 0) ? parity : 0),
 interrupts_(interrupts == 0 ? 0 : 1)
{

	// Basic //
	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (interrupts_ == 1 ? (1 << RXCIE0) : 0);
	
	// BaudRate //
	#define BAUDRATE ((F_CPU)/(baudrate_*16UL)-1);
	UBRR0 = BAUDRATE;
	//UBRR0 = ((16000000 / (16 * 9600)) - 1);
	// Set UBBR according to Baud Rate
	//UBRR0 = 103;
	
	// Stopbits //
	UCSR0C = (stopbits_ == 2 ? 1 << USBS0 : 0);
	
	// Databits // 8 bits
	UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);
	
	// Parity //
	if (parity_ != 0)
	{
		if (parity_ == 'e') UCSR0C |= (1 << 5);
		else UCSR0C |= (1 << 5) | (1 << 4);	
	}
	
}

void UART::write(char data)
{
	while ( (UCSR0A & (1<<5)) == 0 )
	{}
		
	UDR0 = data;								//Write data to UDR register
	
}

char UART::read()
{
	  // Wait for new character received
	  while ( (UCSR0A & (1<<RXC0)) == 0 )
	  {}
	  // Then return it
	  return UDR0;	
}

void UART::disableUART()
{
	UCSR0B &= 0b11110111;
	
}