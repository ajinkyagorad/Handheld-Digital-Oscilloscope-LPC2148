//library to access gpio pins in different way
#include "pin.h"
pin::pin()
{
}
pin::pin(int _pinNum)	//ADAPT for FAST ports
{	//speed=1 : fast
	//speed=0 : slow

	//using  fast version of io ports
	pinNum=_pinNum;
	SCS=0x03;
	
	if( (pinNum>=0) && (pinNum<16) )
	{
		pinsel=&PINSEL0;
		iodir=(unsigned int *)&FIO0DIR;
		ioset=(unsigned int *)&FIO0SET;
		ioclr=(unsigned int *)&FIO0CLR;
	}
	else if( (pinNum>=16) && (pinNum<32) )
	{
		pinsel=&PINSEL1;
		iodir=(unsigned int *)&FIO0DIR+2;
		ioset=(unsigned int *)&FIO0SET+2;		//account for the offset
		ioclr=(unsigned int *)&FIO0CLR+2;
	}
	else if( (pinNum>=100) && (pinNum<132)) 
		{
		pinsel=&PINSEL2;
		iodir=(unsigned int *)&FIO1DIR;
		ioset=(unsigned int *)&FIO1SET;
		ioclr=(unsigned int *)&FIO1CLR;
	}
	
	pinIndex=(pinNum%100);
	if(pinNum<100)*pinsel=*pinsel&(~(0x3<<pinIndex));//make respective pin a gpio pin
  else *pinsel=0;	//making all pins on port (1) as gpio
	output();
	
}

void pin::high()
{
	*ioset=(1<<pinIndex);
}
void pin::low()
{	
	*ioclr=(1<<pinIndex);
}
void pin::input()
{
	*iodir&=~(1<<pinIndex);
}
void pin::output()
{
	*iodir|=(1<<pinIndex);
}
