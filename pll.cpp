//pll Setup 
#include "pll.h"
/*
Setting up pll 
1.Setup PLL
2.Apply Feed Sequence
3.Wait for PLL to lock and then Connect PLL
4.Apply Feed Seqence
*/

/*pll : 
xtalFreq : Freqency of Xtal oscillator
cpuClockMul : Multiplier  value of cpu clock cpu clock = M*fXtal
pClockDiv 	: Divider value for peripheral clock = fCPU/D	
							by default it is 1
*/
pll::pll(unsigned long xtalFreq,int cpuClockMul,int pClockDiv)
{
	pllMul=cpuClockMul;	//must be in between 1 - 32
	pclkDiv=pClockDiv;
	fosc=xtalFreq;
	cclk=fosc*pllMul;	// save cclk value
	pclk=cclk/pClockDiv;
	//assume fccoDiv to be  2
	fccoDiv=getP();	//M and Fosc must be saved
	//value to written to MSEL bits in PLLCFG is M-1
	//find pllDiv to make FCCO in 156 M to 320 M
	//pllDiv should have values 1,2,4,or8
	
	setPll();	//future modifications
	
	
}
int pll::getP()
{
		//determine P for CCO such that fCCO [156,320] M
		for(int i=0;i<=3;i++)
	{
			unsigned long x=(1<<(i+1))*2*pllMul*fosc;
			if((x>156000000)&&(x<320000000))return i;
	}
	return 2;	//to return  a valid values
	
}
unsigned long pll::getCpuClock()
{
	return cclk;
}
unsigned long pll::getPeripheralClock()
{
	return pclk;
}
void pll::setCpuClock(int M)
{
	cclk=M*fosc;	//update cpu clock (ie save it)
	pllMul=M;	//save multiplier
	setPll();	//apply changes
}
void pll::setPeripheralClock(int D)
{
	switch(D)
	{
		case 4 : VPBDIV=0x00;break;
		default : VPBDIV=D;
	}
}
void pll::setPll()
{
	PLL0CON=0x01;		//set PLLE bit
	PLL0CFG=16*fccoDiv+pllMul-1;
	feedSequence();
	while(!isLocked());//wait until pll stabilises
	//now pll is locked connect to it
	PLL0CON=0x03;		//set PLLE bit and PLLC bit
	feedSequence();
	VPBDIV=pclkDiv;//pClockDiv;
}	
bool pll::isLocked()
{
	return PLL0STAT&(1<<10);
}
void pll::feedSequence()
{
		PLL0FEED=0xAA;
		PLL0FEED=0x55;
}

