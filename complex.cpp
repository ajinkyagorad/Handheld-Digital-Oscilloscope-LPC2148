#include "complex.h"

float complex::magnitude()
{
	return sqrt(Re*Re+Im*Im);
}
complex pow_e_i(float f)
{
	complex x;
	x.Re= cos(f);
	x.Im= sin(f);
	return x;
}
complex::complex()
{
}
complex::complex(float _re,float _im)
{
	Re=_re;
	Im=_im;
}

complex complex::operator + (complex c)
{
	complex x;
	x.Re=this->Re+c.Re;
	x.Im=this->Im+c.Im;
	return x;
}

complex complex::operator - (complex c)
{
	complex x;
	x.Re=this->Re-c.Re;
	x.Im=this->Im-c.Im;
	return x;	
}
complex complex::operator * (complex c)
{
	complex x;
	x.Re= (this->Re*c.Re-this->Im*c.Im);
	x.Im= (this->Re*c.Im+this->Im*c.Re);
	return x;
}
complex complex::operator / (complex c)
{
	complex x;
	x.Re= (this->Re*c.Re + this->Im*c.Im);
	x.Im= (this->Im*c.Re - this->Re*c.Im);
	x=x/c.magnitude();
	return x;
}

void complex::operator += (complex c)
{
	complex x;
	x.Re=this->Re+c.Re;
	x.Im=this->Im+c.Im;
	this->Re=x.Re;
	this->Im=x.Im;
}

void complex::operator -= (complex c)
{
	complex x;
	x.Re=this->Re-c.Re;
	x.Im=this->Im-c.Im;
	this->Re=x.Re;
	this->Im=x.Im;
}
void complex::operator *= (complex c)
{
	complex x;
	x.Re= (this->Re*c.Re-this->Im*c.Im);
	x.Im= (this->Re*c.Im+this->Im*c.Re);
	this->Re=x.Re;
	this->Im=x.Im;
}
void complex::operator /= (complex c)
{
	complex x;
	x.Re= (this->Re*c.Re + this->Im*c.Im);
	x.Im= (this->Im*c.Re - this->Re*c.Im);
	x=x/c.magnitude();
	this->Re=x.Re;
	this->Im=x.Im;
}
// with floating numbers
complex complex::operator + (float _c)
{
	complex x,c(_c,0);
	x.Re=this->Re+c.Re;
	x.Im=this->Im+c.Im;
	return x;
}

complex complex::operator - (float _c)
{
	complex x,c(_c,0);
	x.Re=this->Re-c.Re;
	x.Im=this->Im-c.Im;
	return x;	
}
complex complex::operator * (float _c)
{
	complex x,c(_c,0);
	x.Re= (this->Re*c.Re-this->Im*c.Im);
	x.Im= (this->Re*c.Im+this->Im*c.Re);
	return x;
}
complex complex::operator / (float _c)
{
	complex x,c(_c,0);
	x.Re= (this->Re*c.Re + this->Im*c.Im);
	x.Im= (this->Im*c.Re - this->Re*c.Im);
	x=x/c.magnitude();
	return x;
}

void complex::operator += (float _c)
{
	complex x,c(_c,0);
	x.Re=this->Re+c.Re;
	x.Im=this->Im+c.Im;
	this->Re=x.Re;
	this->Im=x.Im;
}

void complex::operator -= (float _c)
{
	complex x,c(_c,0);
	x.Re=this->Re-c.Re;
	x.Im=this->Im-c.Im;
	this->Re=x.Re;
	this->Im=x.Im;
}
void complex::operator *= (float _c)
{
	complex x,c(_c,0);
	x.Re= (this->Re*c.Re-this->Im*c.Im);
	x.Im= (this->Re*c.Im+this->Im*c.Re);
	this->Re=x.Re;
	this->Im=x.Im;
}
void complex::operator /= (float _c)
{
	complex x,c(_c,0);
	x.Re= (this->Re*c.Re + this->Im*c.Im);
	x.Im= (this->Im*c.Re - this->Re*c.Im);
	x=x/c.magnitude();
	this->Re=x.Re;
	this->Im=x.Im;
}

void complex::operator = (float f)
{
	this->Re=f;
	this->Im=0;
}
