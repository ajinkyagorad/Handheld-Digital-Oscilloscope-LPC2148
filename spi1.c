#include "spi1.h"

void initSPI1(void)
{
		//use pins for SSP
		PINSEL1=(PINSEL1&0xFFFFFFFF)|0x000002A8;
		//initialise Clock (clock divide by 2 <minimum>)
		SSPCPSR=0x02;	
		//initialise Control Register 0
		SSPCR0 = (16-1)/*dataLength-1*/;
		//initialise Control Register 1
		SSPCR1 = (1<<1);
		//no interrupts
		SSPIMSC = 0;	
}