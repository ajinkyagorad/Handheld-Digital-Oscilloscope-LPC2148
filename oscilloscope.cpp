#include "oscilloscope.h"
#include <stdio.h>
#include "FFT.h"
/*all extern variables in header file*/
/**
 * ADC related variables
 */
const int samples = 256;
int AD0_raw[samples];
int AD1_raw[samples];
//float AD0_val[samples];
//float AD1_val[samples];
int indexADC;
bool isADC_Buffered;
bool isTriggerSet;
bool isTriggered;
/**
 * DAC related variables
 */
const int DAC_Period = 500;
int indexDAC;
unsigned int DAC_raw[DAC_Period];
 
//assigned lower priority to it
__irq void Timer0_Interrupt(void)
{
	//check if DAC interrupt
	
	//readLastConversion
	AD0_raw[indexADC] = (AD0DR2>>6)& 0x3FF;
	AD1_raw[indexADC] = (AD1DR2>>6)& 0x3FF;
	
	//start ADC conversion from ADCGSR , start both simultaneous
	//ADGSR = (1<<24); //start convertion for ADC
	
	AD0CR=(1<<2)/*channel 2*/| (5<<8)/*clock divide by 11+1*/|(1<<24)/*start conversion*/|(1<<21)/*enable ADC*/;
	AD1CR=(1<<2)/*channel 2*/| (5<<8)/*clock divide by 11+1*/|(1<<24)/*start conversion*/|(1<<21)/*enable ADC*/;
	//while((AD0GDR&(1<<31))==0);		//wait while ADC conversion finishes
	
	if(!isTriggered)if(indexADC>2)if(AD0_raw[indexADC]>=100)if(AD0_raw[indexADC-1]>AD0_raw[indexADC]){
			indexADC=0;
			isTriggered=true;
	}
	indexADC++;
	
		if(indexADC>=samples)			// if incremented index reaches given number of samples
		{
			isADC_Buffered=true;		
			isTriggered = false;
			indexADC = 0;
			//disable timer0 interrupt
			VICIntEnClr=(1<<4);
		}
	T0IR=0x01;				//mark the completion of interrupt by writing to 	the flag
	VICVectAddr=0x00; // signify completed ISR
}
__irq void Timer1_Interrupt(void)
{
		
		DACR = DAC_raw[indexDAC];
		indexDAC++;
		if(indexDAC>=DAC_Period)indexDAC =0;
		T1IR = 0x01;				// mark that corresponnding interrupt is executed
		VICVectAddr= 0x00;	//signify completed ISR
}
void initADC(void)
{
	//enable pins for ADC operation
	PINSEL0 |= (3<<20); // for AD1.2
	PINSEL1|=(1<<26);		//for AD0.2
	isTriggered = false;
	
}
void EnableInterruptSampling(void)
{
	VICIntEnable = (1<<4)/*enable timer0 interrupt*/;
	//T0MCR &= ~(1<<0);		/*disable Match 0 interrupt (for adc sampling)*/
}
void EnableInterruptWaveGen(void)
{
	//T1MCR &=~(1<<0) ;  /* disable Match 1 interrupt (for DAC out)*/
	VICIntEnable = (1<<5);/*enable timer1 interrupt*/
}
void initTimer0(void)
{
	T0TCR = (1<<0)/*Enable for counting*/|(1<<1)/*reset counter for now*/;
	T0CTCR = 0x00; // every rising PCLK edge for PC
	T0TC =0;	T0PC =0;		//doing anyways , as it is being already reset//may be removed
	T0PR = 59;				//set it has ( PCLK_freq_MHz-1) for getting TC inc at 1 us
	T0MR0 = 50;				// interrupt every 1 us
	T0MCR = (1<<0)/*Interrupt on match*/|(1<<1)/*Reset on MR0*/;
	//using VIC vector 2 for timer
	VICVectCntl2 = (1<<5)/*enable this interrupt*/|4/*timer0 interrupt*/;
	VICVectAddr2 = unsigned(Timer0_Interrupt);
	T0TCR = (1<<0)/*Enable for counting*/;
}
int U(int x)
{
		if(x>=0)return 1;
		else return 0;
}
void initDACout(void)
{

	indexDAC = 0;			// initialise DAC index to 0
	//Enable D AC out pin 
	PINSEL1 |= 0x00080000; // select pin for DAC
	//Enable buffer as funtion with period
	/**
	 *Period : Number of raw samples to repeatedly output
	 *Buffer : LUT ( look up table),stores raw samples to output
	 */
	
	 for(int i=0;i<DAC_Period;i++)
	 {
			//DAC_raw[i] =500*(U(i-25));
			DAC_raw[i] = 300*sin(i*0.0628)+300;
			DAC_raw[i] = DAC_raw[i]<<6;		// make it ready to write directly ot DACR register;
	 }
	 
	 
	//Enable Timer for interrupt operation
	T1TCR = (1<<0)|(1<<1);
	T1CTCR = 0x00;
	T1TC = 0; T1PC = 0;
	T1PR = 59;
	T1MR0 = 5	 	;//interrupt every 2 us
	T1MCR = (1<<0)/*interrupt on match*/|(1<<1)/*reset on MR0)*/;
	VICVectCntl0 = (1<<5)|5;
	VICVectAddr0 = unsigned(Timer1_Interrupt);
	T1TCR = (1<<0)/*Enable for counting*/;
	//EnableInterruptWaveGen(); /*do later after initialising other routines*/
}
void disableTimer0(void)
{
	T0TCR = 0x2;
}
void disableTimer1(void)
{
	T1TCR = 0x2;
}
void oscilloscope(void)
{
	//init  settings
	indexADC = 0;
	isADC_Buffered = false;
	initADC();
	initDACout();
	initTimer0();
	initFFT_LUT();
	EnableInterruptSampling();	
	EnableInterruptWaveGen();
	while(1)	
	{
		//lcd.drawText("1",COLOR_BLUE);
		if(isADC_Buffered)
		{
			isADC_Buffered = false;
			// disable Timer0
			disableTimer0();
			//display both ADC graph
			lcd._cs.low();
			displayFFT_Plot(AD0_raw,samples,COLOR_RED,true);
			//displayFFT_Plot(AD1_raw,samples,COLOR_BLUE);
			//displayFFT_PlotMix(AD0_raw,AD1_raw,samples,COLOR_RED,COLOR_BLUE);
			//displayFFT_fix(AD0_raw,AD1_raw,samples,COLOR_RED,COLOR_BLUE);
			displayDrawBackgroundGraph();
			displayGraph(AD0_raw,samples,COLOR_GREEN);
			displayGraph(AD1_raw,samples,COLOR_INDIGO);
			displayXY_Plot(AD0_raw,AD1_raw,samples,COLOR_YELLOW	);
			
			
			long avg = 0;
			
			for(int i=0;i <100 ;i++)avg+=AD0_raw[i];
			avg/=100;
			char buf[20];
			lcd.setOrientation(1);
			sprintf(buf,"Vmean = %d",avg);
			lcd.drawText(0,0,buf,COLOR_WHITE);
			lcd.drawText(20,0,"CH0", COLOR_GREEN);
			//enable timer0 again
			initTimer0();
			EnableInterruptSampling();
			lcd._cs.high();
		}
		else
		{
			//do nothing
		}
			
	}
	
}
void displayDrawBackgroundFFT(void)
{
		lcd.setOrientation(0);
		lcd._setWindow(ILI9225_LCD_WIDTH/2,0,ILI9225_LCD_WIDTH	,samples/2);
	
		lcd._rs.high();
		for(int y=0;y<ILI9225_LCD_HEIGHT;y++)
		for(int x=0;x<ILI9225_LCD_WIDTH/2;x++)
		{	
				if(x%20	==0||y%10==0)lcdWrite(COLOR_DARKRED);//lcdWrite(COLOR_DARKGREEN);
				else lcdWrite(COLOR_BLACK);
				//wait till transmit fifo is full	
				while(!SSPSR&(1<<1));
		}
}
void displayDrawBackgroundXY_Plot(void)
{
		lcd.setOrientation(0);
		lcd._setWindow(ILI9225_LCD_WIDTH/2,samples/2+1,ILI9225_LCD_WIDTH,ILI9225_LCD_HEIGHT);
		lcd._rs.high();
		for(int y=0;y<ILI9225_LCD_HEIGHT-samples/2;y++)
		for(int x=0;x<ILI9225_LCD_WIDTH/2;x++)
		{	
				if(x%10	==0||y%10==0)lcdWrite(COLOR_DARKBLUE);//lcdWrite(COLOR_DARKGREEN);
				else lcdWrite(COLOR_BLACK);
				//wait till transmit fifo is full	
				while(!SSPSR&(1<<1));
		}
}


void displayDrawBackgroundGraph(void)
{
		
		lcd.setOrientation(0);
		lcd._setWindow(0,0,ILI9225_LCD_WIDTH/2-1,ILI9225_LCD_HEIGHT);
	
		lcd._rs.high();
		for(int y=0;y<ILI9225_LCD_HEIGHT;y++)
		for(int x=0;x<=ILI9225_LCD_WIDTH/2-1;x++)
		{	
				if(x%25	==0||y%50==0)lcdWrite(COLOR_DARKGREEN);//lcdWrite(COLOR_DARKGREEN);
				else lcdWrite(COLOR_BLACK);
				//wait till transmit fifo is full	
				while(!SSPSR&(1<<1));
		}
		
}
void displayGraph(int * graphData,int len,int color)
{
	//scale graphdata accordingly
	//display only first LCD_HEIGHT data samples
	float factor = (ILI9225_LCD_WIDTH-2)/1024.0;
	for ( int i=0; i<len; i++)
	{
			//using  temporary itself to scale and store
			graphData[i] = factor * graphData[i]/2;
	}
	
	// set window
	//draw lines for data plot
	
	for(int i = 1;i<ILI9225_LCD_HEIGHT;i++)
	{
		lcd.drawLine(graphData[i-1],i-1,graphData[i],i,color);
		
	}
	
}

void displayXY_Plot(int * graphData1,int* graphData2,int len,int color)
{
	//scale graphdata accordingly
	//display only first LCD_HEIGHT data samples
	
	int samplesHalf = samples/2;
	for ( int i=0; i<len; i++)
	{
			//using  temporary itself to scale and store
			graphData1[i] =graphData1[i]+ILI9225_LCD_WIDTH/2;		//along X
			graphData2[i] =graphData2[i]+samplesHalf;			// along Y
	}
	
	// set window
	displayDrawBackgroundXY_Plot();
	for(int i = 1;i<len;i++)
	{
		lcd.drawLine(graphData1[i-1],graphData2[i-1],graphData1[i],graphData2[i],color+(i<<8));
		
	}
	
}
void displayFFT_fix(int*graphData1,int * graphData2, int len, int color1,int color2)
{
			int fftBinsRe[samples];
			int fftBinsIm[samples];
	for(int i=0;i<len;i++){
		fftBinsRe[i] = graphData1[i];			//copy all data
		fftBinsIm[i] = graphData2[i];
	}
	fix_fft(fftBinsRe,fftBinsIm,8,0);
	
	// get fft for each respective
	int lenHalf=len/2;
	int xRe,xIm,yRe,yIm;
	for(int k=0;k<lenHalf;k++)
	{
			xRe = (fftBinsRe[k]+fftBinsRe[len-k])/2;
			xIm = (fftBinsIm[k]-fftBinsIm[len-k])/2;
			yRe = (fftBinsIm[k]+fftBinsIm[len-k])/2;
			yIm = (fftBinsRe[len-k]-fftBinsRe[k])/2;
			fftBinsRe[k] = sqrt(float(xRe*xRe+xIm*xIm));
			fftBinsIm[k] = sqrt(float(yRe*yRe+yIm*yIm));
			//fftBinsRe[k] = (xRe*xRe+xIm*xIm);
			//fftBinsIm[k] = (yRe*yRe+yIm*yIm);
	}
	
	
	int max=0;
	int lcdHalfWidth = ILI9225_LCD_WIDTH/2;
	for(int i=1;i<lenHalf;i++)	if(fftBinsRe[i]>max)max=fftBinsRe[i];
	for(int i=0;i<samples;i++)
	{
			fftBinsRe[i]=(lcdHalfWidth*fftBinsRe[i])/max+lcdHalfWidth;
	}
	 int max1=0;
	for(int i=1;i<lenHalf;i++)	if(fftBinsIm[i]>max1)max1=fftBinsIm[i];
	for(int i=0;i<samples;i++)
	{
			fftBinsIm[i]=(lcdHalfWidth*fftBinsIm[i])/max1+lcdHalfWidth;
	}

	displayDrawBackgroundFFT();
	
	for(int i=1; i<lenHalf;i++){
						if(fftBinsRe[i-1]<=ILI9225_LCD_WIDTH)
						lcd.drawLine(fftBinsRe[i-1],(i-1),fftBinsRe[i],i,color1);
						if(fftBinsIm[i-1]<=ILI9225_LCD_WIDTH)
						lcd.drawLine(fftBinsIm[i-1],(i-1),fftBinsIm[i],i,color2);
				}
	lcd.setOrientation(1);
	char buf[30];
	sprintf(buf,"m1:2 %d,%d",max,max1);
	lcd.drawText(0,10,buf,COLOR_TURQUOISE);
}
void displayFFT_Plot(int* graphData, int len, int color,bool isBackground)
{
	complex fftBins[samples];
	int lenHalf=len/2;
	for(int i=0;i<len;i++){
	fftBins[i].Re = graphData[i];
	fftBins[i].Im = 0;
	}
	fft(fftBins,samples,1);
	for(int i=0;i<len;i++)fftBins[i].Re = fftBins[i].magnitude();
	float max=0;
	float lcdHalfWidth = ILI9225_LCD_WIDTH/2;
	for(int i=5;i<lenHalf;i++)	if(fftBins[i].Re>max)max=fftBins[i].Re;
	for(int i=0;i<samples;i++)
	{
			fftBins[i].Re=lcdHalfWidth*fftBins[i].Re/max+lcdHalfWidth;
	}
	if(isBackground)displayDrawBackgroundFFT();
	for(int i=1; i<lenHalf;i++){
						if(fftBins[i-1].Re<=ILI9225_LCD_WIDTH)
						lcd.drawLine(fftBins[i-1].Re,(i-1),
											fftBins[i].Re,i,color);
				}
}

void displayFFT_PlotMix(int * graphData1,int * graphData2,int len, int color1,int color2 )
{
	complex fftBins[samples];
	
	for(int i=0;i<len;i++){
		fftBins[i].Re = graphData1[i];
		fftBins[i].Im = graphData2[i];
	}
	fft(fftBins,samples,1);
	
	// get fft for each respective
	int lenHalf=len/2;
	complex x,y;
	for(int k=0;k<lenHalf;k++)
	{
			x.Re = (fftBins[k].Re+fftBins[len-k].Re)/2;
			x.Im = (fftBins[k].Im-fftBins[len-k].Im)/2;
			y.Re = (fftBins[k].Im+fftBins[len-k].Im)/2;
			y.Im = (fftBins[len-k].Re-fftBins[k].Re)/2;
			fftBins[k].Re = x.magnitude();
			fftBins[k].Im = y.magnitude();
			
	}
	
	
	float max=0;
	float lcdHalfWidth = ILI9225_LCD_WIDTH/2;
	for(int i=5;i<lenHalf;i++)	if(fftBins[i].Re>max)max=fftBins[i].Re;
	if(max<10)max=10;
	for(int i=0;i<samples;i++)
	{
			fftBins[i].Re=lcdHalfWidth*fftBins[i].Re/max+lcdHalfWidth;
	}
	float max1=0;
	for(int i=5;i<lenHalf;i++)	if(fftBins[i].Im>max1)max1=fftBins[i].Im;
	if(max1<10)max1=10;
	for(int i=0;i<samples;i++)
	{
			fftBins[i].Im=lcdHalfWidth*fftBins[i].Im/max1+lcdHalfWidth;
	}
	
	displayDrawBackgroundFFT();
	for(int i=1; i<lenHalf;i++){
						if(fftBins[i-1].Re<=ILI9225_LCD_WIDTH)
						lcd.drawLine(fftBins[i-1].Re,(i-1),fftBins[i].Re,i,color1);
						if(fftBins[i-1].Im<=ILI9225_LCD_WIDTH)
						lcd.drawLine(fftBins[i-1].Im,(i-1),fftBins[i].Im,i,color2);
				}
}