/*
 * FileSystem.h
 *
 * Created: 27-03-2019 18:14:57
 *  Author: viksk
 */ 


#ifndef UART_H_
#define UART_H_
class UART
{
	public:
	UART(int baudrate = 9600, char databits = 8, char stopbits = 2, char parity = 0, char interrupts = 1);
	
	void write(char data);
	char read();
	bool received_;
	void disableUART();
	//void setBaudrate(int baudrate);
	//void setDatabits(char databits);
	//void setParity(char parity);
	//void setInterrupts(char interrupts);


	private:
	int baudrate_;
	char databits_;
	char stopbits_;
	char parity_;
	char interrupts_;
	
};


#endif /* FILESYSTEM_H_ */