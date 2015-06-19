#ifndef COMPLEX_H
#define COMPLEX_H
#include <math.h>

#define _2_PI 6.283185307179586476925286766559
#define _PI 3.1415926535897932384626433832795
#define _e  2.7182818284590452353602874713527
struct complex
{
	float Re;
	float Im;
	complex();
	complex(float _re,float _im);
	
	complex operator + (complex c);
	complex operator - (complex c);
	complex operator * (complex c);
	complex operator / (complex c);
	
	complex operator + (float c);
	complex operator - (float c);
	complex operator * (float c);
	complex operator / (float c);
	
	void operator += (complex c);
	void operator -= (complex c);
	void operator *= (complex c);
	void operator /= (complex c);	
	
	void operator += (float c);
	void operator -= (float c);
	void operator *= (float c);
	void operator /= (float c);	
	
	void operator = (float f);
	
	
	float magnitude(void);
	
};


complex pow_e_i(float);

#endif