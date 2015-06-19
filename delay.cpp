//library for delay
#include "delay.h"
// initialise delay object 
delayClass::delayClass(unsigned long _fcpu)
{
	fdelay=_fcpu/4.92;//fdelay=fcpu/k ;find k by experiment
}

//delay for x clocks
void delayClass::clocks(unsigned long clk)
{
	while(clk--);
}
//delay for milliseconds
void delayClass::ms(unsigned long t)
{
	clocks((fdelay/1000)*t);
}
void delayClass::us(unsigned long t)
{
	clocks((fdelay/1000000)*t);
}void delayClass::s(float t)
{
	clocks(fdelay*t);
}
