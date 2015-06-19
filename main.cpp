#include <lpc214x.h>
extern "C"{
#include "spi1.h"
}
#include "ILI9225.h"
#include "delay.h"
#include "pll.h"
#include "pin.h"
// main application file
#include "oscilloscope.h"
pll _clock(12000000,5,1);		// maximum
delayClass wait(60000000);
pin lcd_rst(124), lcd_rs(8), lcd_cs(7), lcd_led(9);
ILI9225 lcd(lcd_rst,lcd_rs,lcd_cs,lcd_led);
int main(void)
{
			//all initialisation sequence here
			//first init SPI ( SSP ) interface
			initSPI1();
			lcd.begin();
			lcd.setOrientation(1/*LANDSCAPE*/);
			lcd.clear();
			lcd.setFont(Terminal6x8);
			lcd.drawText(0,0,"INITIALISED..",COLOR_BLUE);
			wait.ms(500);
			//call application
			oscilloscope();
}