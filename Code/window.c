//=============================================================
// 
// Copyright (c) 1999-2023 Simon Southwell. All rights reserved.
//
// Date: 11th March 1999
//
// Set of routines for calculating an impulse response of
// a high/low pass filter, and various window functions
// that may be used to modify that response.
//
// This file is part of the WinFilter FIR filter design utility.
//
// WinFilter is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// WinFilter is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with WinFilter. If not, see <http://www.gnu.org/licenses/>.
//
//=============================================================

// -------------------------------------------------------------------------
// INCLUDES
// -------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "fft.h"
#include "window.h"
#include "filter.h"

// -------------------------------------------------------------------------
// Sinc function                                           
//                                                         
// Sinc function (sin(x)/x) for use in low/high pass filter
// design.                                                 
//                                                         
// -------------------------------------------------------------------------

real_t sinc (const real_t x, const real_t Fc, const real_t Fs, const unsigned int inversion)
{
    real_t sinc_scaling;

    /* Scaling required to get equation in the impulse form of
       sin(2 Pi Fcut n)/(Pi n), where Fcut = Fc/Fs, and the passed
       x parameter equals (2 Pi Fc/Fs n) */
    sinc_scaling = (2.0 * Fc/Fs);  

    /* If argument is 0, then return an absolute, as division 
       by 0 will upset the 'pute. Inversion is simply multiplying 
       by -1 for all coefficients except when x == 0, where it 
       is the value subtracted from 1. */
    if(x == 0.0)
        return(inversion ? 1.0 - sinc_scaling : sinc_scaling);
    else
        return(sinc_scaling * sin(x)/x * (inversion ? -1.0 : 1.0));
}

// -------------------------------------------------------------------------
// Bartlett window                                          
//                                                          
// This function produces a Bartlett (or triangular) window.
// (NB: alpha value (a) unused in this function.)           
//                                                          
//   w(x) = 1 - |x|,       where -1 < x < 1                 
//                                                          
// -------------------------------------------------------------------------

real_t bartlett (const real_t a, const real_t n, const real_t N)
{
    /* 0 at  n = -N/2 and N/2, 1 at n = 0, linear in between */
    return(1.0 - (real_t) abs((int)n) / (N/2) );
}

// -------------------------------------------------------------------------
// Cosine window                                   
//                                                 
// Cosine from -Pi/2 to +Pi/2 raised to the power a
//                                                 
// -------------------------------------------------------------------------

real_t cosine (const real_t a, const real_t n, const real_t N)
{
    return(pow(cos(M_PI * n / N), a));
}

// -------------------------------------------------------------------------
// Hamming window                                         
//                                                        
// This window funcion produces a raised cosine. The para-
// meter a defines how much the cosine is raised above 0. 
// with a = 0 a uniform window is produced. With a = 0.5 a
// von Hann window is produced.                           
//                                                        
//   w(x) = 2a cos(2 Pi x) + b                            
//                                                        
//        where 2a + b = 1, and -0.5 < x < 0.5            
//                                                        
// -------------------------------------------------------------------------

real_t hamming (const real_t a, const real_t n, const real_t N)
{
    real_t b;

    /* 2a + b = 1, so calculate b from a */
    b = 1.0 - (2.0 * a);

    /* Hamming(n) = 2a cos(2 Pi n/N) + b */
    return((cos((2*M_PI) * n / N) * 2.0 * a) + b);
}

// -------------------------------------------------------------------------
// Bohman window                                 
//                                               
// This function produces a Bohman window.       
// (NB: alpha value (a) unused in this function.)
//                                               
//                                               
//   w(x) = (1 - |x|) cos(Pi x) + sin(Pi |x|)    
//                                ----------     
//                                    Pi         
//                                               
//          where -1 < x < 1                     
//                                               
// -------------------------------------------------------------------------

real_t bohman (const real_t a, const real_t n, const real_t N)
{
    real_t wT, abs_wT;

    /* Adjust range to be from -1 to 1, rather than -0.5 to +0.5 */
    wT =  2.0 * n/N;
    abs_wT = (n < 0) ? -1.0 * wT : wT;

    return((1.0 - abs_wT) * cos(M_PI * wT) + sin(M_PI * abs_wT)/M_PI);
}

// -------------------------------------------------------------------------
// Cauchy window                                 
//                                               
// This function produces a Cauchy window.       
//                                               
//               1                               
//   w(x) = ----------- ,        where -1 < x < 1
//                  2                            
//            1 + ax                             
//                                               
// -------------------------------------------------------------------------

real_t cauchy (const real_t a, const real_t n, const real_t N)
{
    real_t wT;

    /* Adjust range to be from -1 to 1, rather than -0.5 to +0.5 */
    wT =  2.0 * n/N;

    return(1/(1 + (a * a * wT * wT)));
}

// -------------------------------------------------------------------------
// Blackman window                                    
//                                                    
// This function produces a Blackman window.          
// (NB: alpha value (a) unused in this function.)     
//                                                    
//   w(x) = 0.42 - 0.5 cos(2 Pi x) + 0.08 cos(4 Pi x),
//                                                    
//          where 0 < x < 1                           
//                                                    
// -------------------------------------------------------------------------

real_t blackman (const real_t a, const real_t n, const real_t N)
{
    real_t wT;

    /* Adjust range to be from 0 to 1, rather than -0.5 to +0.5 */
    wT = (0.5 + n/N);

    /* This function is 'like' a raised cosine in appearance, 
       but obviously it isn't quite */
    return(0.42 - 0.5 * cos((2*M_PI) * wT) + 
           0.08 * cos(2.0 * (2*M_PI) * wT));
}

// -------------------------------------------------------------------------
// Blackman-Harris  window                         
//                                                 
// This function produces a Blackman-Harris window.
// (NB: alpha value (a) unused in this function.)  
//                                                 
//   w(x) = 0.35875 - 0.48829 cos(2 Pi x) +        
//                    0.14128 cos(4 Pi x) -        
//                    0.01168 cos(6 Pi x),         
//                                                 
//          where 0 < x < 1                        
//                                                 
// -------------------------------------------------------------------------

real_t blackman_harris (const real_t a, const real_t n, const real_t N)
{
    real_t wT;

    /* Adjust range to be from 0 to 1, rather than -0.5 to +0.5 */
    wT = (0.5 + n/N);

    /* This function is 'like' a raised cosine in appearance, 
       but obviously it isn't quite */
    return(0.35875 - 0.48829 * cos((2*M_PI) * wT) + 
           0.14128 * cos(2.0 * (2*M_PI) * wT) -
           0.01168 * cos(3.0 * (2*M_PI) * wT));
}

// -------------------------------------------------------------------------
// Nuttall window                                         
//                                                        
// This function produces a Nuttall window. It is a member
// of the Blackman-Harris family, but with different      
// coefficients.                                          
// (NB: alpha value (a) unused in this function.)         
//                                                        
//   w(x) = 0.3635819 - 0.4891775 cos(2 Pi x) +           
//                      0.1365995 cos(4 Pi x) -           
//                      0.0106411 cos(6 Pi x),            
//                                                        
//          where 0 < x < 1                               
//                                                        
// -------------------------------------------------------------------------

real_t nuttall (const real_t a, const real_t n, const real_t N)
{
    real_t wT;

    /* Adjust range to be from 0 to 1, rather than -0.5 to +0.5 */
    wT = (0.5 + n/N);

    return(0.3635819 - 0.4891775 * cos((2*M_PI) * wT) + 
           0.1365995 * cos(2.0 * (2*M_PI) * wT) -
           0.0106411 * cos(3.0 * (2*M_PI) * wT));
}

// -------------------------------------------------------------------------
// Gaussian curve                                            
//                                      2    2               
//                             -(x - mu) /2(a )              
//                            e                              
//     Gaussian curve g(x) = ------------------              
//                                     0.5                   
//                                a (Pi   )                  
//                                                           
// where a represents the sigma value (the mean mu, normally 
// subtracted from x is always 0 here, and the 1/(a Pi**0.5) 
// normalisation is not done so that the window peaks at 1.0)
//                                                           
// -------------------------------------------------------------------------

real_t gauss (const real_t a, const real_t n, const real_t N)
{
    real_t x;

    /* Scale points to run from +/- Pi */
    x = (2*M_PI) * n/N ;
    
    return(exp((-1.0 * x * x)/(2.0 * a * a)));
}

// -------------------------------------------------------------------------
// Poisson curve                                
//                 -a |x|                                    
//         w(x) = e      ,     where  -1 < x < 1          
//                                                         
// -------------------------------------------------------------------------

real_t poisson (const real_t a, const real_t n, const real_t N)
{
    real_t x;

    /* Scale points to run from +/- 1, and make absolute */
    x = ((n < 0) ? -2.0 : 2.0) * n/N ;
    
    return(exp(-1.0 * a * x));
}

// -------------------------------------------------------------------------
// Reisz curve                                 
//                        2                                             
//          w(x) = 1 - |x| ,   where -1 < x < 1
//                                                                      
// -------------------------------------------------------------------------

real_t reisz (const real_t a, const real_t n, const real_t N)
{
    real_t x;

    /* Scale points to run from +/- 1, and make absolute */
    x = ((n < 0) ? -2.0 : 2.0) * n/N ;
    
    return(1.0 - (x * x));
}

// -------------------------------------------------------------------------
// Riemann curve                                 
//                                                                         
//      w(x) = sin(Pi x)/(Pi x), where -1 < x < 1                          
//                                               
// -------------------------------------------------------------------------

real_t riemann (const real_t a, const real_t n, const real_t N)
{
    real_t x;

    /* Scale points to run from +/- 1 */
    x = 2.0 * n/N ;
    
    return((x == 0) ? 1.0 : (sin(M_PI * x) / (M_PI * x)));
}

// -------------------------------------------------------------------------
// Tukey curve
//                                                 
// w(x) = IF (|x| < a) => 1.0                      
//                                                 
//                         1             |x| - a   
//        OTHERWISE    =>  - (1 + cos(Pi --------))
//                         2               1 - a   
//                                                 
//        where -1 < x < 1                         
//                                                 
// -------------------------------------------------------------------------

real_t tukey (const real_t a, const real_t n, const real_t N)
{
    real_t x;

    /* Scale points to run from +/- 1  and make absolute */
    x = ((n < 0) ? -2.0 : 2.0) * n/N ;
    
    return((x < a) ? 1.0 : (0.5 * (1.0 + cos(M_PI * (x-a)/(1-a)))));
}

// -------------------------------------------------------------------------
// VallePoisson curve                    
//                               2       
//      IF (|x| < 0.5) => 1 - 6 x (1 - x)
//                                       
//                                 3     
//      OTHERWISE      => 2 (1 - x)      
//                                       
//      where -1 < x < 1                 
// -------------------------------------------------------------------------

real_t vallepoisson (const real_t a, const real_t n, const real_t N)
{
    real_t x;

    /* Scale points to run from +/- 1  and make absolute */
    x = ((n < 0) ? -2.0 : 2.0) * n/N ;
    
    return((x < 0.5) ? (1.0 - 6 * x * x * (1 - x)) : 
                       (2 * (1-x) * (1-x) * (1-x)));
}

// -------------------------------------------------------------------------
// Kaiser window                                          
//                                                        
// This function produces a Kaiser window. The parameter a
// defines the shape of the window. With a = 0, a uniform 
// window is produced. With a ~= 5.4, a similar response  
// to a Hamming window is produced.                       
//
// 
//                                   n    2   0.5
//                  I ( a {  1 - [ ----- ]   }    )
//                   0             0.5 N
//  Kaiser(n)  =    -------------------------------
//                              I (a)
//                               0
//                                                       
// -------------------------------------------------------------------------

real_t kaiser (const real_t a, const real_t n, const real_t N)
{
    static real_t I0 (const real_t);
    static real_t last_a = -1.0;
    static real_t I0_a;

    // Only need to calculate I0(a) once for a particular
    // alpha (which is constant for a given window calculation)
    if(last_a != a)
        I0_a = I0(a);

    return( I0(a * sqrt(1.0 - ((n*n)*4.0/(N*N)))) / I0_a);
}

// -------------------------------------------------------------------------
// Modified Bessel function I (order 0) of the first type
//         
//          infinity
//            --+
//            \     1     x   k  2 
//   I  = 1 +  >  ( -  [  -  ]  )
//    0       /     k!    2
//            --+
//           k = 1
// Sum from k = 1 to 69 (ought to be accurate enough---can't 
// do infinity, but series terms converges toward 0 with increasing k) 
// 
// -------------------------------------------------------------------------

static real_t I0 (const real_t x)
{
    int k;
    real_t sum = 0.0;

    sum = 1.0;
    for(k = 1; k <= 69; k++)
        sum += pow((pow((x/2.0), (real_t) k) * (1.0/factorial(k))), 2.0);

    return(sum);
}

// -------------------------------------------------------------------------
// Kaiser parameter calculator
// -------------------------------------------------------------------------

KaiserParamStruct design_kaiser_filter (const real_t Fd, const real_t Fs, const real_t ripple)
{
    static char sbuf[1][80], *sbufptr[1];
    int n = 0;

    KaiserParamStruct result = {0, 0.0};

    /* Check that delta frequency is sensible */
    if(Fd < 0.0) {
        sprintf(sbuf[0], "design_kaiser_filter(): Error! Must specify a delta frequency in auto-design mode\n");
        DisplayMessage(1, (char **)&sbufptr);
        ErrorAction(BADWINDOWSTATUS);
        return result;
    }

    /* Taps = (|Ripple| - 7.95) / (14.36 Fd/Fs). A value of 0.5 is added to guarantee
       rounding to nearest whole number. Fs/2 appears to be needed to get correct
       values. Is Fd value counted twice for both mirrored halves of response? */
    result.N = (int)(0.5 + ((ripple - 7.95)/(14.36 * Fd / (0.5 * Fs))));

    /* Alpha = 0,                                                    |Ripple| <= 21; 
       Alpha = 0.5842 (|Ripple| - 21)^0.4 + 0.07886 (|Ripple| - 21), 21 < |Ripple| < 50; 
       Alpha = 0.1102 (|Ripple| - 8.7),                              Otherwise */
    result.a = (ripple <= 21.0) ? 0.0 :
               ((ripple > 21.0 && ripple < 50) ? 
                (0.5842 * pow((ripple - 21.0), 0.4) + 0.07886 * (ripple - 21.0)) :
                (0.1102 * (ripple - 8.7)));

    return(result);
}

// -------------------------------------------------------------------------
// Chebyshev window                                         
//                                                          
// This function produces a Chebyshev window. The frequency 
// response is given by:                                    
//                                                          
//            Cheb(N-1, beta * cos(pi * n/N))               
//     W(k) = -------------------------------               
//                   Cheb(N-1, beta)                        
//                                                          
// beta = cosh(1/(N-1) * acosh(10^a)), and Cheb(N, x) is Nth
// order polynomial at point x. Denominator is constant, and
// not calculated as the inverse transformed result is      
// normalised. An inverse DFT is used to obtain the time    
// domain values.                                           
//                                                          
// -------------------------------------------------------------------------

real_t chebyshev (const real_t a, const real_t n, const real_t N)
{
    static real_t Cheb(real_t, real_t);
    double beta, dftmax = 1, result = 0;
    static complex_t *buf = NULL;
    int i, M, k, fft_status;

    // Integer versions of length and point index
    M = (int) N;
    k = (int) n;

    // If this is the first value, initialise the window table for the new value of 'a'
    if(k == (-1 * M/2)) {

        // In case buffer memory was not cleared on a previous run, 
        // clear it now for a new allocation
        if(buf != NULL) {
            free(buf);
            buf = NULL;
        }
        
        if((buf = calloc(M, sizeof(complex_t))) == NULL)
            return 0;

        // Calculate beta
        beta = cosh(1/(N-1) * acosh(pow(10.0, a)));

        // Frequency response calculation
        for(i = 0; i < M ; i++) {
            buf[(i+M/2)%M].r = Cheb(N-1, beta * cos(M_PI * ((double)i - N/2)/N));
            buf[i].i = 0;
        }

        // Inverse transform to get time response 
        // DFT used if N is not a power of 2 (FFT needs power of 2 points).
        if(M & (M-1))
            fft_status = dft(buf, M, 1);
        else
            fft_status = fft(buf, M, 1);

        // An error occured in the Fourier transform, so clean up and return
        if(fft_status) {
            DisplayMessage(1, (char **)fft_error_msg);
            free(buf);
            buf = NULL;
            return 0;
        }

        // Find max value
        for(i = 0; i < M; i++) 
            if(dftmax < buf[i].r || i == 0)
                dftmax =  buf[i].r;

        // Normalise
        for(i = 0; i < M; i++) 
            buf[i].r = buf[i].r / dftmax;
    }

    // Reference buffer only if memory allocation was successful
    if(buf != NULL) {
        // Extract window value from table - phase shift so it
        // runs from (N/2  to 3N/2) mod N
        result = buf[(k+M) % M].r;

        // If this is the last point, free the memory
        if(k == M/2 - 1) {
            free(buf);
            buf = NULL;
        }
    }

    return result;
}

// -------------------------------------------------------------------------
// Calculates the nth Chebyshev polynomial at point x
// -------------------------------------------------------------------------

static double Cheb(double n, double x)
{
    if(x <= 1.0)
        return cos  (n * acos(x));
    else
        return cosh (n * acosh(x));
}

