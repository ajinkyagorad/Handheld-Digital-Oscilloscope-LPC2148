#include "FFT.h"
const int sizeLUT = 256;
float sineLUT[sizeLUT];
//following is from http://staff.science.nus.edu.sg/~phywjs/CZ5101/fft.c
/*----------------------------------------------------------------------*/
/* Truncated Stockham algorithm for multi-column vector,
       X(n1,n2) <- F_n1 X(n1,n2)
   x[] input data of size n, viewed as n1 = n/n2 by n2 dimensional 
   array, flag =+1 forward transform, -1 for backward transform, y[] is 
   working space, which must be n in size.  This function is supposed 
   to be internal (static), not used by application.  Note that the 
   terminology of column or row respect to algorithms in the Loan's 
   book is reversed, because we use row major convention of C.
*/
void initFFT_LUT(void)
{
	for(int i=0; i<sizeLUT; i++)
	{
		sineLUT[i] = sin(M_PI*i/sizeLUT);
	}
}
static void stockham(complex x[], int n, int flag, int n2, complex y[])
{
   complex  *y_orig, *tmp;
   int  i, j, k, k2, Ls, r, jrs;
   int  half, m, m2;
   real  wr, wi, tr, ti;
   
   y_orig = y;
   r = half = n >> 1;
   Ls = 1;                                         /* Ls=L/2 is the L star */
  
   while(r >= n2) {                              /* loops log2(n/n2) times */
      tmp = x;                           /* swap pointers, y is always old */
      x = y;                                   /* x is always for new data */
      y = tmp;
      m = 0;                        /* m runs over first half of the array */
      m2 = half;                             /* m2 for second half, n2=n/2 */
      for(j = 0; j < Ls; ++j) {
         wr = cos(M_PI*j/Ls);                   /* real and imaginary part */
         wi = -flag * sin(M_PI*j/Ls);                      /* of the omega */
				 //using LUT
				/* wr = (j>=Ls/2)?
									( -sineLUT[(sizeLUT*j/Ls)-128]):
												sineLUT[(sizeLUT*j/Ls)+128];
				 wi = -flag*sineLUT[(sizeLUT/Ls)*j];
				*/
         jrs = j*(r+r);
         for(k = jrs; k < jrs+r; ++k) {           /* "butterfly" operation */
            k2 = k + r;
            tr =  wr*y[k2].Re - wi*y[k2].Im;      /* complex multiply, w*y */
            ti =  wr*y[k2].Im + wi*y[k2].Re;
            x[m].Re = y[k].Re + tr;
            x[m].Im = y[k].Im + ti;
            x[m2].Re = y[k].Re - tr;
            x[m2].Im = y[k].Im - ti;
            ++m;
            ++m2;
         }
      } 
      r  >>= 1;
      Ls <<= 1;
   };

   if (y != y_orig) {                     /* copy back to permanent memory */
      for(i = 0; i < n; ++i) {               /* if it is not already there */
         y[i] = x[i];               /* performed only if log2(n/n2) is odd */
      }
   }
   
   assert(Ls == n/n2);                        /* ensure n is a power of 2  */
   assert(1 == n || m2 == n);           /* check array index within bound  */
}  


/* The Cooley-Tukey multiple column algorithm, see page 124 of Loan.
   x[] is input data, overwritten by output, viewed as n/n2 by n2
   array. flag = 1 for forward and -1 for backward transform.
*/
void cooley_tukey(complex x[], int n, int flag, int n2)
{
   complex c;
   int i, j, k, m, p, n1;
   int Ls, ks, ms, jm, dk;
   real wr, wi, tr, ti;

   n1 = n/n2;                               /* do bit reversal permutation */
   for(k = 0; k < n1; ++k) {        /* This is algorithms 1.5.1 and 1.5.2. */
      j = 0; 
      m = k;
      p = 1;                               /* p = 2^q,  q used in the book */
      while(p < n1) {
         j = 2*j + (m&1);
         m >>= 1;
         p <<= 1;
      }
      assert(p == n1);                   /* make sure n1 is a power of two */
      if(j > k) {
         for(i = 0; i < n2; ++i) {                     /* swap k <-> j row */
            c = x[k*n2+i];                              /* for all columns */
            x[k*n2+i] = x[j*n2+i];
            x[j*n2+i] = c;
         }
      }
   }
                                              /* This is (3.1.7), page 124 */
   p = 1;
   while(p < n1) {
      Ls = p;
      p <<= 1;
      jm = 0;                                                /* jm is j*n2 */
      dk = p*n2;
      for(j = 0; j < Ls; ++j) {
         wr = cos(M_PI*j/Ls);                   /* real and imaginary part */
         wi = -flag * sin(M_PI*j/Ls);                      /* of the omega */
         for(k = jm; k < n; k += dk) {                      /* "butterfly" */
            ks = k + Ls*n2;
            for(i = 0; i < n2; ++i) {                      /* for each row */
               m = k + i;
               ms = ks + i;
               tr =  wr*x[ms].Re - wi*x[ms].Im;
               ti =  wr*x[ms].Im + wi*x[ms].Re;
               x[ms].Re = x[m].Re - tr;
               x[ms].Im = x[m].Im - ti;
               x[m].Re += tr;
               x[m].Im += ti;
            }
         }
         jm += n2;			
      } 
   }
}



/* 1D Fourier transform: 
   Simply call stockham with proper arguments.  
   Allocated working space of size n dynamically.
*/
void fft(complex x[], int n, int flag)
{
   complex *y;
   
   assert(1 == flag || -1 == flag);
   y = (complex *) malloc( n*sizeof(complex) );
   assert(NULL != y);
   stockham(x, n, flag, 1, y);
   free(y);
}

 
/* 3D Fourier transform:  
   The index for x[m] is mapped to (i,j,k) by
   m = k + n3*j + n3*n2*i, i.e. the row major convention of C.
   All indices start from 0.
   This algorithm requires working space of n2*n3.
   Stockham is efficient, good stride feature, but takes extra
   memory same size as input data; Cooley-Tukey is in place,
   so we take a compromise of the two.
*/

void fft2D(complex x[], int n1, int n2, int flag)
{
   complex *y;
   int i, n;

   assert(1 == flag || -1 == flag);
   n = n1*n2;
   y = (complex *) malloc( n2*sizeof(complex) );
   assert(NULL != y);

   for(i=0; i < n; i += n2) {                                  /* FFT in y */
      stockham(x+i, n2, flag, 1, y); 
   }
   free(y);
   cooley_tukey(x, n, flag, n2);                               /* FFT in x */
}

void fft3D(complex x[], int n1, int n2, int n3, int flag)
{
   complex *y;
   int i, n, n23;

   assert(1 == flag || -1 == flag);
   n23 = n2*n3;
   n = n1*n23;
   y = (complex *) malloc( n23*sizeof(complex) );
   assert(NULL != y);

   for(i=0; i < n; i += n3) {                                  /* FFT in z */
      stockham(x+i, n3, flag, 1, y);
   }
   for(i=0; i < n; i += n23) {                                 /* FFT in y */
      stockham(x+i, n23, flag, n3, y); 
   }
   free(y);
   cooley_tukey(x, n, flag, n23);                              /* FFT in x */
}




//assumes only real input

//void FFT(complex * in, complex* out,int size, int initial/*=0*/ ,int period/*=1*/)
/*{
	int  dynSize = size/period;
	
	if(dynSize == 2)
	{
		out[initial]=in[initial]+in[initial+period];
		out[initial]=in[initial]-in[initial+period];
		return;
	}
	
	FFT(in,out,size,initial,period*2);
	FFT(in,out,size,initial+period,period*2);
	
	for(int i=0;i<dynSize/2;i++)
	{
		out[initial+period*i] = out[initial+ 2*i*period]+
							pow_e_i((-1.0* _2_PI* i)/dynSize)*out[initial+ (2*i+1)*period];
		out[initial+period*(i+dynSize/2)] = out[initial+ 2*i*period]-
							pow_e_i((-1.0* _2_PI* i)/dynSize)*out[initial+ (2*i+1)*period];	
	}
	return;
}
//following gets magnitude bin array in its real part
void FFT_real(complex* bin,int size)
{
	for(int i=0;i <size ;i ++)
	{
		bin[i] = bin[i].magnitude();
	}
}

void FFT_normalise(complex* bin, int size, float value)
{
	float max=0;
	for(int i=0;i<size;i++)if(bin[i].re>max)max=bin[i].re;
	for(int i=0;i<size;i++) bin[i].re*=value/max;
}
*/