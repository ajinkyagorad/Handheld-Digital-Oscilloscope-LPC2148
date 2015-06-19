//library to access gpio pins in different way Header File


#ifndef PIN_H
	#define PIN_H
#include <lpc214x.h>

class pin{
	volatile unsigned long * pinsel;
	volatile unsigned int *iodir,*ioset,*ioclr;	//some how make them constants :: NEEDS update
	int pinNum,pinIndex;
	public:
		pin();	//use only for passing function arguments
		pin(int _pinNum);
		void high();
		void low();
		void input();
		void output();
	//	void read();
		
};
#endif


