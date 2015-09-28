#ifndef OSCILLOSCOPE_H
	#define OSCILLOSCOPE_H
//including device specific header file
#include <lpc214x.h>
//for complex numbers
#include "complex.h"
#include "FFT.h"
#include "fft_fix.h"			//for fixed point fft
// for lcd
#include "ILI9225.h"
extern ILI9225 lcd;
// define Global Variables
		#ifndef _PI_ 
		#define _PI_ 3.14159265
		#endif

extern const int samples;			// preferred 256 ( but can 2^i : i is integer)
extern int AD0_raw[];		// storing raw values from AD0
extern int AD1_raw[];		// storing raw values from AD1
extern float AD0_val[];	// storing actual calculated values for voltage on ADC 1
extern float AD1_val[];	// storing actual calculated values for voltage on ADC 1
extern int indexADC;				//index for current ADC data being buffered for both ADC's
extern bool isADC_Buffered;
extern bool isTriggerSet;
extern bool isTriggered;
extern const int DAC_Period;	// period of dac waveform
extern int indexDAC;		// index for current DAC data being outputted
extern unsigned int DAC_raw[];		// storing DAC value to output continuously and periodically
extern int phaseBins0[];		// for holding phase vs frequency  (in fix fft) for AD0
extern int phaseBins1[];		// for holding  phase vs frequency ( in fix fft) for AD1
//NOTE:
	//using pins A0.2 and A1.2 for ADC0 and ADC1 resp.
	//
	// Vmeasured = Voffset+ Gain * Vreal
		// Vmeasured wrt voltage ADC assuming AVCC 
		// Voffset is voltage offset after amplifying 
		// Gain is external Hardware Gain
	// DAC raw array holds 10 bit output value  wrt AVCC



//interrupt function declarations
__irq void Timer1_Interrupt(void);
__irq void Timer0_Interrupt(void);
__irq void ADC0_Interrupt(void);

/**
 * Some Variables
 */

/**
 * main oscilloscpe function + helper functions
 */
 
void oscilloscope(void);
void initADC(void);
void initTimer0(void);
void disableTimer0(void);		// disable timer 0  ( do not use in ISR)
void disableTimer1(void);		// disable timer 1 
void initDACout(void);			// uses timer1 for DAC out
void initDACoutBuffer(void); // initialise raw values for dac output in DAC raw
void EnableInterruptSampling(void); //enable interrupt for Sampling : match 0 register timer0
void EnableInterruptWaveGen(void);	// enable interrupt for dac out ( match 0 register) timer1
void displayGraph(int * graphData,int len,int color);
void displayXY_Plot(int * graphData1,int* graphData2,int size,int color);
void displayFFT_Plot(int * graphData,int len, int color,bool isBackground=false);
void displayFFT_PlotMix(int * graphData1,int * graphData2,int len, int color1,int color2);
void displayFFT_fix(int*graphData1,int * graphData2, int len, int color1,int color2);
void displayDrawBackgroundGraph(void);
void displayDrawBackgroundFFT(void);
void displayDrawBackgroundXY_Plot(void);

#endif
