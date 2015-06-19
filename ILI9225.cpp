//ILI9225 controller 2.2" TFT Library

#include "ILI9225.h"

//set Cursor Limits for text display
void ILI9225::setCursorLimits( unsigned int xmin,unsigned int	xmax 
		,unsigned int  ymin ,unsigned int  ymax,unsigned int _lineGap)
{
		curXMin=(xmin/cfont.width)*cfont.width;
		curXMax=(xmax/cfont.width)*cfont.width;
		curYMin=(ymin/cfont.height)*cfont.height;
		curYMax=(ymax/cfont.height)*cfont.height;
		lineGap=_lineGap;
		curX=curXMin;
		curY=curYMin;
}

ILI9225::ILI9225(pin rst, pin rs, pin cs, pin led) {
	
	_rst  = rst;
	_rs   = rs;
	_cs   = cs;
	_sdi  = _clk = 0;
	_led  = led;
	hwSPI = true;
}

void ILI9225::_orientCoordinates(unsigned int &x1, unsigned int &y1) {

	switch (_orientation) {
	case 0:  // ok
		break;
	case 1: // ok
		y1 = _maxY - y1 - 1;
		_swap(x1, y1);
		break;
	case 2: // o
		x1 = _maxX - x1 - 1;
		y1 = _maxY - y1 - 1;
		break;
	case 3: // ok
		x1 = _maxX - x1 - 1;
		_swap(x1, y1);
		break;
	}
}


void ILI9225::_setWindow(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1) {
	_orientCoordinates(x0, y0);
	_orientCoordinates(x1, y1);

	if (x1<x0) _swap(x0, x1);
	if (y1<y0) _swap(y0, y1);

	_writeRegister(ILI9225_HORIZONTAL_WINDOW_ADDR1,x1);
	_writeRegister(ILI9225_HORIZONTAL_WINDOW_ADDR2,x0);

	_writeRegister(ILI9225_VERTICAL_WINDOW_ADDR1,y1);
	_writeRegister(ILI9225_VERTICAL_WINDOW_ADDR2,y0);

	_writeRegister(ILI9225_RAM_ADDR_SET1,x0);
	_writeRegister(ILI9225_RAM_ADDR_SET2,y0);

	_writeCommand(0x0022);
	//_rs.high();	// make rs high ,for continuous data writes through macro
}


void ILI9225::begin( ) {

	// Set up pins
	_rs.output();
	_cs.output();
	_rst.output();
	_led.output();
	_cs.low();
	//THE  ABOVE IS SPI INITIALISATON SEQUENCE
	// Turn on backlight
	_led.high();

	// Initialization Code
	_rst.high(); // Pull the reset pin high to release the ILI9225C from the reset status
	wait.ms(1); 
	_rst.low(); // Pull the reset pin low to reset ILI9225
	wait.ms(10);
	_rst.high(); // Pull the reset pin high to release the ILI9225C from the reset status
	wait.ms(50);

	/* Start Initial Sequence */
	/* Set SS bit and direction output from S528 to S1 */
	_writeRegister(ILI9225_POWER_CTRL1, 0x0000); // Set SAP,DSTB,STB
	_writeRegister(ILI9225_POWER_CTRL2, 0x0000); // Set APON,PON,AON,VCI1EN,VC
	_writeRegister(ILI9225_POWER_CTRL3, 0x0000); // Set BT,DC1,DC2,DC3
	_writeRegister(ILI9225_POWER_CTRL4, 0x0000); // Set GVDD
	_writeRegister(ILI9225_POWER_CTRL5, 0x0000); // Set VCOMH/VCOML voltage
	wait.ms(40); 

	// Power-on sequence
	_writeRegister(ILI9225_POWER_CTRL2, 0x0018); // Set APON,PON,AON,VCI1EN,VC
	_writeRegister(ILI9225_POWER_CTRL3, 0x6121); // Set BT,DC1,DC2,DC3
	_writeRegister(ILI9225_POWER_CTRL4, 0x006F); // Set GVDD   /*007F 0088 */
	_writeRegister(ILI9225_POWER_CTRL5, 0x495F); // Set VCOMH/VCOML voltage
	_writeRegister(ILI9225_POWER_CTRL1, 0x0800); // Set SAP,DSTB,STB
	wait.ms(10);
	_writeRegister(ILI9225_POWER_CTRL2, 0x103B); // Set APON,PON,AON,VCI1EN,VC
	wait.ms(50);

	_writeRegister(ILI9225_DRIVER_OUTPUT_CTRL, 0x011C); // set the display line number and display direction
	_writeRegister(ILI9225_LCD_AC_DRIVING_CTRL, 0x0100); // set 1 line inversion
	_writeRegister(ILI9225_ENTRY_MODE, 0x1030); // set GRAM write direction and BGR=1.
	_writeRegister(ILI9225_DISP_CTRL1, 0x0000); // Display off
	_writeRegister(ILI9225_BLANK_PERIOD_CTRL1, 0x0808); // set the back porch and front porch
	_writeRegister(ILI9225_FRAME_CYCLE_CTRL, 0x1100); // set the clocks number per line
	_writeRegister(ILI9225_INTERFACE_CTRL, 0x0000); // CPU interface
	_writeRegister(ILI9225_OSC_CTRL, 0x0D01); // Set Osc  /*0e01*/
	_writeRegister(ILI9225_VCI_RECYCLING, 0x0020); // Set VCI recycling
	_writeRegister(ILI9225_RAM_ADDR_SET1, 0x0000); // RAM Address
	_writeRegister(ILI9225_RAM_ADDR_SET2, 0x0000); // RAM Address

	/* Set GRAM area */
	_writeRegister(ILI9225_GATE_SCAN_CTRL, 0x0000); 
	_writeRegister(ILI9225_VERTICAL_SCROLL_CTRL1, 0x00DB); 
	_writeRegister(ILI9225_VERTICAL_SCROLL_CTRL2, 0x0000); 
	_writeRegister(ILI9225_VERTICAL_SCROLL_CTRL3, 0x0000); 
	_writeRegister(ILI9225_PARTIAL_DRIVING_POS1, 0x00DB); 
	_writeRegister(ILI9225_PARTIAL_DRIVING_POS2, 0x0000); 
	_writeRegister(ILI9225_HORIZONTAL_WINDOW_ADDR1, 0x00AF); 
	_writeRegister(ILI9225_HORIZONTAL_WINDOW_ADDR2, 0x0000); 
	_writeRegister(ILI9225_VERTICAL_WINDOW_ADDR1, 0x00DB); 
	_writeRegister(ILI9225_VERTICAL_WINDOW_ADDR2, 0x0000); 

	/* Set GAMMA curve */
	_writeRegister(ILI9225_GAMMA_CTRL1, 0x0000); 
	_writeRegister(ILI9225_GAMMA_CTRL2, 0x0808); 
	_writeRegister(ILI9225_GAMMA_CTRL3, 0x080A); 
	_writeRegister(ILI9225_GAMMA_CTRL4, 0x000A); 
	_writeRegister(ILI9225_GAMMA_CTRL5, 0x0A08); 
	_writeRegister(ILI9225_GAMMA_CTRL6, 0x0808); 
	_writeRegister(ILI9225_GAMMA_CTRL7, 0x0000); 
	_writeRegister(ILI9225_GAMMA_CTRL8, 0x0A00); 
	_writeRegister(ILI9225_GAMMA_CTRL9, 0x0710); 
	_writeRegister(ILI9225_GAMMA_CTRL10, 0x0710); 

	_writeRegister(ILI9225_DISP_CTRL1, 0x0012); 
	wait.ms(50); 
	_writeRegister(ILI9225_DISP_CTRL1, 0x1017);

	setBacklight(true);
	setOrientation(0);

	// Initialize variables
	setBackgroundColor( COLOR_BLACK );

	clear();
	setCursorLimits();
	//_cs.low();		//deselect  lcd
}


void ILI9225::clear(int color) {
	char old = _orientation;
	setOrientation(0);
	fillRectangle(0, 0, _maxX - 1, _maxY - 1, color);
	setOrientation(old);
	 
	wait.ms(10);
}


void ILI9225::invert(bool flag) {
	_writeCommand( flag ? ILI9225C_INVON : ILI9225C_INVOFF);
}


void ILI9225::setBacklight(bool flag) {
	if(flag)_led.high();
	else _led.low();
}


void ILI9225::setDisplay(bool flag) {
	
	if (flag) {
		_writeRegister(0x00ff, 0x0000);
		_writeRegister(ILI9225_POWER_CTRL1, 0x0000);
		wait.ms(50);
		_writeRegister(ILI9225_DISP_CTRL1, 0x1017);
		wait.ms(200);
	} else {
		_writeRegister(0x00ff, 0x0000);
		_writeRegister(ILI9225_DISP_CTRL1, 0x0000);
		wait.ms(50);
		_writeRegister(ILI9225_POWER_CTRL1, 0x0003);
		wait.ms(200);
	}
}


void ILI9225::setOrientation(char orientation) {

	_orientation = orientation % 4;

	switch (_orientation) {
	case 0:
		_maxX = ILI9225_LCD_WIDTH;
		_maxY = ILI9225_LCD_HEIGHT;
		break;
	case 1:
		_maxX = ILI9225_LCD_HEIGHT;
		_maxY = ILI9225_LCD_WIDTH;
		break;
	case 2:
		_maxX = ILI9225_LCD_WIDTH;
		_maxY = ILI9225_LCD_HEIGHT;
		break;
	case 3:
		_maxX = ILI9225_LCD_HEIGHT;
		_maxY = ILI9225_LCD_WIDTH;
		break;
	}
}


char ILI9225::getOrientation() {
	return _orientation;
}


void ILI9225::drawRectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color) {
	drawLine(x1, y1, x1, y2, color);
	drawLine(x1, y1, x2, y1, color);
	drawLine(x1, y2, x2, y2, color);
	drawLine(x2, y1, x2, y2, color);
}


void ILI9225::fillRectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color) {

	_setWindow(x1, y1, x2, y2);

	for(unsigned int t=(y2 - y1 + 1) * (x2 - x1 + 1); t > 0; t--)
		_writeData( color);
}


void ILI9225::drawCircle(unsigned int x0, unsigned int y0, unsigned int r, unsigned int color) {

	int f = 1 - r;
	int ddF_x = 1;
	int ddF_y = -2 * r;
	int x = 0;
	int y = r;

	drawPixel(x0, y0 + r, color);
	drawPixel(x0, y0-  r, color);
	drawPixel(x0 + r, y0, color);
	drawPixel(x0 - r, y0, color);

	while (x<y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		drawPixel(x0 + x, y0 + y, color);
		drawPixel(x0 - x, y0 + y, color);
		drawPixel(x0 + x, y0 - y, color);
		drawPixel(x0 - x, y0 - y, color);
		drawPixel(x0 + y, y0 + x, color);
		drawPixel(x0 - y, y0 + x, color);
		drawPixel(x0 + y, y0 - x, color);
		drawPixel(x0 - y, y0 - x, color);
	}
}


void ILI9225::fillCircle(char x0, char y0, char radius, unsigned int color) {

	int f = 1 - radius;
	int ddF_x = 1;
	int ddF_y = -2 * radius;
	int x = 0;
	int y = radius;

	while (x<y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		drawLine(x0 + x, y0 + y, x0 - x, y0 + y, color); // bottom
		drawLine(x0 + x, y0 - y, x0 - x, y0 - y, color); // top
		drawLine(x0 + y, y0 - x, x0 + y, y0 + x, color); // right
		drawLine(x0 - y, y0 - x, x0 - y, y0 + x, color); // left
	}
	fillRectangle(x0-x, y0-y, x0+x, y0+y, color);
}

int ILI9225::abs(int x)
{
	return (x<0)?(-x):x;
}
void ILI9225::drawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color) {

	// Classic Bresenham algorithm
	int steep = abs(y2 - y1) > abs(x2 - x1);
	int dx, dy;

	if (steep) {
		_swap(x1, y1);
		_swap(x2, y2);
	}

	if (x1 > x2) {
		_swap(x1, x2);
		_swap(y1, y2);
	}

	dx = x2 - x1;
	dy = abs(y2 - y1);

	int err = dx / 2;
	int ystep;

	if (y1 < y2) ystep = 1;
	else ystep = -1;


	for (; x1<=x2; x1++) {
		if (steep) drawPixel(y1, x1, color);
		else       drawPixel(x1, y1, color);

		err -= dy;
		if (err < 0) {
			y1 += ystep;
			err += dx;
		}
	}
}

void ILI9225::drawPixel(unsigned int x1, unsigned int y1, unsigned int color)
{
	//_orientCoordinates(x1, y1);
	_writeRegister(ILI9225_RAM_ADDR_SET1,x1);
	_writeRegister(ILI9225_RAM_ADDR_SET2,y1);
	_writeCommand(0x0022);
	_writeData(color);
}
void ILI9225::_drawPixel(unsigned int x1, unsigned int y1, unsigned int color) {

	if(/*(x1 < 0) ||*/ (x1 >= _maxX) || /*(y1 < 0) ||*/ (y1 >= _maxY)) return;

	_setWindow(x1, y1, x1+1, y1+1);
	_orientCoordinates(x1, y1);
	_writeData(color);
}


unsigned int ILI9225::maxX() {
	return _maxX;
}


unsigned int ILI9225::maxY() {
	return _maxY;
}


unsigned int ILI9225::setColor(char red8, char green8, char blue8) {
	// rgb16 = red5 green6 blue5
	return (red8 >> 3) << 11 | (green8 >> 2) << 5 | (blue8 >> 3);
}


void ILI9225::splitColor(unsigned int rgb, char &red, char &green, char &blue) {
	// rgb16 = red5 green6 blue5
	red   = (rgb & 0xF800) >> 11 << 3;
	green = (rgb & 0x07E0) >>  5 << 2;
	blue  = (rgb & 0x001F)       << 3;
}


void ILI9225::_swap(unsigned int &a, unsigned int &b) {
	unsigned int w = a;
	a = b;
	b = w;
}



void ILI9225::_writeCommand(int data) {

	_rs.low();	
	SSPDR=data;		//spi in 16 bit mode
	//write to SSPDR data register 
	// do not wait because operating at maximum speed
}


void ILI9225::_writeData(int data) {
	_rs.high();
	SSPDR=data;
}


void ILI9225::_writeRegister(unsigned int reg, unsigned int data) {
	_writeCommand(reg);
	_writeData(data);
}


void ILI9225::drawTriangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int x3, unsigned int y3, unsigned int color) {
	drawLine(x1, y1, x2, y2, color);
	drawLine(x2, y2, x3, y3, color);
	drawLine(x3, y3, x1, y1, color);
}


void ILI9225::fillTriangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int x3, unsigned int y3, unsigned int color) {

	unsigned int a, b, y, last;

	// Sort coordinates by Y order (y3 >= y2 >= y1)
	if (y1 > y2) {
		_swap(y1, y2); _swap(x1, x2);
	}
	if (y2 > y3) {
		_swap(y3, y2); _swap(x3, x2);
	}
	if (y1 > y2) {
		_swap(y1, y2); _swap(x1, x2);
	}

	if (y1 == y3) { // Handle awkward all-on-same-line case as its own thing
		a = b = x1;
		if (x2 < a)      a = x2;
		else if (x2 > b) b = x2;
		if (x3 < a)      a = x3;
		else if (x3 > b) b = x3;
			drawLine(a, y1, b, y1, color);
		return;
	}

	unsigned int	dx11 = x2 - x1,
				dy11 = y2 - y1,
				dx12 = x3 - x1,
				dy12 = y3 - y1,
				dx22 = x3 - x2,
				dy22 = y3 - y2,
				sa   = 0,
				sb   = 0;

	// For upper part of triangle, find scanline crossings for segments
	// 0-1 and 0-2.  If y2=y3 (flat-bottomed triangle), the scanline y2
	// is included here (and second loop will be skipped, avoiding a /0
	// error there), otherwise scanline y2 is skipped here and handled
	// in the second loop...which also avoids a /0 error here if y1=y2
	// (flat-topped triangle).
	if (y2 == y3) last = y2;   // Include y2 scanline
	else          last = y2 - 1; // Skip it

	for (y = y1; y <= last; y++) {
	a   = x1 + sa / dy11;
	b   = x1 + sb / dy12;
	sa += dx11;
	sb += dx12;
	/* longhand:
	a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
	b = x1 + (x3 - x1) * (y - y1) / (y3 - y1);
	*/
	if (a > b) _swap(a,b);
		drawLine(a, y, b, y, color);
	}

	// For lower part of triangle, find scanline crossings for segments
	// 0-2 and 1-2.  This loop is skipped if y2=y3.
	sa = dx22 * (y - y2);
	sb = dx12 * (y - y1);
	for (; y<=y3; y++) {
		a   = x2 + sa / dy22;
		b   = x1 + sb / dy12;
		sa += dx22;
		sb += dx12;
		/* longhand:
		a = x2 + (x3 - x2) * (y - y2) / (y3 - y2);
		b = x1 + (x3 - x1) * (y - y1) / (y3 - y1);
		*/
		if (a > b) _swap(a,b);
			drawLine(a, y, b, y, color);
	}
}


void ILI9225::setBackgroundColor(unsigned int color) {
	_bgColor = color;
}


void ILI9225::setFont(const char* font) {

	cfont.font 	   = font;
	cfont.width    = readFontByte(0);
	cfont.height   = readFontByte(1);
	cfont.offset   = readFontByte(2);
	cfont.numchars = readFontByte(3);
	cfont.nbrows   = cfont.height / 8;

	if (cfont.height % 8) cfont.nbrows++;  // Set number of bytes used by height of font in multiples of 8
}

void ILI9225::_updateCurX(int factor)
{
	curX+=factor*cfont.width;				
	if(curX >curXMax )	// if ought to overflow
	{
		curX=curXMin;	//if next  character ought  goes out of screen 
		_updateCurY();		//also increment y cursor
	}
	else if(curX<curXMin)	//if underflow
	{
			curX=curXMax-cfont.width;
			_updateCurY(-1);
	}
}
void ILI9225::_updateCurY(int factor)
{
		curY+=factor*(cfont.height+lineGap);
	curX=(factor>0)?curXMin:curXMax;//-cfont.width;
		if(curY>curYMax)curY=curYMin;
		else if (curY<curYMin)curY=curYMax;
}
void ILI9225::drawText(char*s/*,unsigned int x,unsigned int y*/, unsigned int color)
{
		for(char k=0; s[k]; k++)
		{
				if(s[k]==0x0d)
				{
						_updateCurY();
						continue;
				}
				else if (s[k]=='~'+1)	//if backspace
				{
						_updateCurX(-1);
						drawChar(curX,curY,' ',_bgColor);
						continue;
				}
				else 
				{
					if(drawChar(curX,curY,s[k],color))_updateCurX();		//also effects  curY;
																																	//update only when suitable character is defined
										
				}
			}
					
}
	

void ILI9225::drawText(unsigned int x, unsigned int y, char * s, unsigned int color) {

	unsigned int currx = x;

	// Print every character in char *
	for (char k = 0; s[k]; k++) {
		if(s[k]==0x0d)	//newline character recieved 
		{
				curY+=cfont.height+lineGap;
		}	
		
		currx += drawChar(currx, y, s[k], color) + 1;
	}
}


unsigned int ILI9225::drawChar(unsigned int x, unsigned int y, unsigned int ch, unsigned int color) {

	char charData, charWidth;
	char h, i, j, k;
	unsigned int charOffset;
	if( (ch<' ') || (ch> ('~'+1)) )return 0;
	charOffset = (cfont.width * cfont.nbrows) + 1;  // bytes used by each character
	charOffset = (charOffset * (ch - cfont.offset)) + FONT_HEADER_SIZE;  // char offset (add 4 for font header)
	charWidth  = readFontByte(charOffset);  // get font width from 1st byte
	charOffset++;  // increment pointer to first character data byte

	for (i = 0; i <= charWidth; i++) {  // each font "column" (+1 blank column for spacing)
		h = 0;  // keep track of char height
		for (j = 0; j < cfont.nbrows; j++) 	{  // each column byte
			if (i == charWidth) charData = (char)0x0; // Insert blank column
			else                charData = readFontByte(charOffset);
			charOffset++;
			
			// Process every row in font character
			for (char k = 0; k < 8; k++) {
				if (h >= cfont.height ) break;  // No need to process excess bits
				if (charData&(1<<k)) _drawPixel(x + i, y + (j * 8) + k, color);
				else                      _drawPixel(x + i, y + (j * 8) + k, _bgColor);
				h++;
			};
		};
	};
	return charWidth;
}

