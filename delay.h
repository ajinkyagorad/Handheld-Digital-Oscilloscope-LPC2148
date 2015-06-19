//library for delay Header File
#ifndef DELAY_H
	#define DELAY_H
	
	//following needs to be calibrated for clock:
	//clocks used is not exactly the clock
class delayClass{
			unsigned long fdelay;//hypothetical delay frequency
			
	public:
		delayClass(unsigned long _fcpu);
		void clocks(unsigned long clk);
		void ms(unsigned long t);
		void us(unsigned long t);
		void s(float t);
	//maximum delay with 60M is ~ 71 sec (k=1)
};

#endif
