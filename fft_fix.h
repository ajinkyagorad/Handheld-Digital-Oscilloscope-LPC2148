#ifndef FFT_FIX_H_
#define FFT_FIX_H_

#define fixed int			//use int as type
#ifndef fixed
#define fixed short
#endif

/*
        fix_fft() - perform fast Fourier transform.

        if n>0 FFT is done, if n<0 inverse FFT is done
        fr[n],fi[n] are real,imaginary arrays, INPUT AND RESULT.
        size of data = 2**m
        set inverse to 0=dft, 1=idft
*/
int fix_fft(fixed fr[], fixed fi[], int m, int inverse);
#endif