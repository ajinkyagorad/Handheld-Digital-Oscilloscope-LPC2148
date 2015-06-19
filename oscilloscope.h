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

//NOTE:
	//using pins A0.2 and A1.2 for ADC0 and ADC1 resp.
	//
	// Vmeasured = Voffset+ Gain * Vreal
		// Vmeasured wrt voltage ADC assuming AVCC 
		// Voffset is voltage offset after amplifying 
		// Gain is external Hardware Gain
	// DAC raw array holds 10 bit output value  wrt AVCC
	
//defines for mode
#define MODE_GRAPH  0x01
#define MODE_FFT		0x02
#define MODE_XY			0x04


//Element Objects Defines
struct Window_Properties{
	int x0;int y0;		// coordinates for initial vertex of Window (rectangle)
	int x1;int y1;		// coordinates for diagonally vertex of Window (rectangle)
	int colorBackground;	// default  background color 
	int colorBorder;			// border Color 
	// Border Width : 1px
};

struct WindowGrid_Properties{
	int DX;		//grid delta-x spacing
	int DY;		//grid delta-y spacing
	int color;//grid color
};

//following for both channels
struct GraphNormal_Properties{		//Normal VvsT plot
	int color;		//two colors for both channels
	//data points
	float* data;		// array of pointers for retriving data to display
	
	//Time Scale Properties
	int colorTextTime;		//text color to display
	float TimePerGrid;// Time per grid Division
	
	//Voltage Scale Properties
	int colorText;			// text color to display
	float VoltPerGrid;	// real voltage per grid division 
	int x,y;						//position for 
};	

struct XY_Plot_Properties{				// XY plot
	int  color;						// color of X-Y plot
	int xOffset,yOffset;	// offset for centre of plot
	int colorText;
	float xScalePerGrid,yScalePerGrid;
};

struct FFT_Plot_Properties{		//Normal FFT ( linear span of frequency)
	int color;
	int colorText;
	float HzPerGrid;
};


// following depends on external input hardware
// Default values to set
//  Voffset =0;
// Gain = 1;
struct Input_Parameters{
	float refVoltage;
	float Voffset;			// offset after amplifying the original signal
	float Gain;					// gain of the amplifier
};

struct Trigger_Properties{
	int VoltageThresholdRaw;	//voltage  threshold in terms of ADC sampling ; to be calculated 
	float VoltageThreshold;		// actual voltage threshold Vreal
	bool isRising;						// is the trigger is for rising or falliing edge
	//int SampleTrigger;			// for maintaining trigger in between
	int colorMarker;					// color for marker
	int colorLine;								//color for TriggerLine
	
};



//interrupt function declarations
__irq void Timer1_Interrupt(void);
__irq void Timer0_Interrupt(void);
__irq void ADC0_Interrupt(void);

/**
 * Some Variables
 */
extern Window_Properties			  WindowProperties;
extern WindowGrid_Properties		WindowGridProperties;
extern GraphNormal_Properties		GraphNormalProperties;
extern XY_Plot_Properties				xyPlotProperties;	
extern FFT_Plot_Properties			fftPlotProperties;
extern Input_Parameters					InputProperties;
extern Trigger_Properties				TriggerProperties;
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
