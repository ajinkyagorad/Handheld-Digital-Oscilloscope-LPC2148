//pll setup header file
#include "defines.h"

#ifndef PLL_H
	#define PLL_H

//include required libraries

#include <lpc214x.h>
//namespace PLL{
	
	//#define FOSC 12000000u //frequency of Xtal
/*
	#define FCCO  //frequency of pll Current Controlled Oscillator
	#define CCLK  //PLL output frequency (CPU Clock)
	#define PLL_MUL //PLL Multiplier Value (MSEL Bits in PLLCFG register)
	
	#define PLL_DIV //PLL Divider Value (PSEL Bits in the PLLCFG register)
	#define  PCLK //Peripheral Clock :derived from CCLK

*///CCLK = PLL_MUL * FOSC	//cpu clock
//FCCO = CCLK * 2 * PLL_DIV
	
//FOSC is in the range of 10 M to 25 M
//CCLK in the range of 10 M to Fmax (60M:LPC2148)
//FCCO in the range 156 M to 320 M
	//following class must be declared global
class pll
{
	public: 
		unsigned long pclk,fosc;
	private: 
		unsigned long fcco,cclk;
		unsigned char pllMul,fccoDiv,pclkDiv;//pllMul stores pll clock multiplier //fccoDiv : val fcco divided to remain in range
		void feedSequence();		//applys feed sequence
		bool isLocked();					//checks if pll is locked return true if locked
		void setPll();
		int getP();
	public:
		
		unsigned long getCpuClock();
		unsigned long getPeripheralClock();
		void setCpuClock(int M);
		void setPeripheralClock(int D);
		pll(unsigned long xtalFreq,int cpuClockMul,int pClockDiv=0x01);	//take input clock frequency 
		// such that declaring pll object is enough
};
//};	
#endif
