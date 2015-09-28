#include <lpc214x.h>
extern "C"{
#include "spi1.h"		// SPI interface using SSP module
}
#include "ILI9225.h"  // header for LCD 
#include "delay.h"		// approximate  delay
#include "pll.h"			// clock to uC control
#include "pin.h"			// individual Pin control
// main application file	
#include "oscilloscope.h"		
pll _clock(12000000,5,1);		// set at maximum recommended operating  frequency @12MHz (x5) = @60MHz
delayClass wait(60000000);	//delay class object with operating frequency input
pin lcd_rst(124), lcd_rs(8), lcd_cs(7), lcd_led(9); // for individual control pins for lcd's
ILI9225 lcd(lcd_rst,lcd_rs,lcd_cs,lcd_led);		// LCD object  with  LCD pins
int main(void)
{
			//all initialisation sequence here
			//first init SPI ( SSP ) interface
			initSPI1();
			lcd.begin();				//initialise the LCD
			lcd.setOrientation(1/*LANDSCAPE*/);	//using landscape orientation  for LCD
			lcd.clear();					// removeany gabage data on LCD
			lcd.setFont(Terminal6x8);		// set font for some text
			lcd.drawText(0,0,"INITIALISED..",COLOR_BLUE); // initial display text
			//wait.ms(500);				//delay to view the initial text // neglected for quick operation
			//call application
			oscilloscope();				// main activity to call { contains endless loop}
}