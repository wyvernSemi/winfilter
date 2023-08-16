//=============================================================
// 
// Copyright (c) 1999-2023 Simon Southwell. All rights reserved.
//
// Date: 11th March 1999
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
//
// Fourier Transform functions, using decimation in time.
// Array x, of size length and type complex_t, is passed in,
// and results returned in the same array.
//
// fft() uses fast fourier transform, but is restricted to
// transforms with lengths a power of 2. Data may be padded
// with zeros. 
//
// dft() is also supplied with the same interface as fft()
// but will transform arbitrary sized data (albeit slowly).
//
// PARAMETERS:
//
// x[] - 'complex_t' array pointer (type specified in fft.h) 
//       containing data points. If data real, set imaginary 
//       points to 0. Resultant data returned in this array.
//
// length - int containg length of data array. Must be power
//          of 2 if calling fft().
//
// inverse - int flag. If non-zero, inverse (synthesis)
//           transform perfomed.
//
// COMPILATION:
//
//   If compiled with COS_TABLE defined, the cosine/sine 
//   calculations are done with a lookup table, which should
//   speed up the calculations but limits the fft to 4096
//   points.
//
// RETURN:
//
//   Both fft() and dft() return either FFT_OKSTATUS or 
//   FFT_ERRORSTATUS. For the latter, fft_error_msg points
//   to an error message string. Subsequent calls to fft or
//   dft will clear any previous message. Transformed data
//   placed in array pointed to by x[].
//
//=============================================================

// -------------------------------------------------------------------------
// INCLUDES
// -------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "fft.h"

#ifdef COS_TABLE
#include "cos_table.h"
#endif

// -------------------------------------------------------------------------
// DEFINES
// -------------------------------------------------------------------------

// Macros for basic complex_t arithmetic 
#define MULTC(_R, _X, _Y){              \
    _R.r = _X.r * _Y.r - _X.i * _Y.i;   \
    _R.i = _X.i * _Y.r + _X.r * _Y.i;   \
}

#define ADDC(_R, _X, _Y){               \
    _R.r = _X.r + _Y.r;                 \
    _R.i = _X.i + _Y.i;                 \
}

#define SUBC(_R, _X, _Y){               \
    _R.r = _X.r - _Y.r;                 \
    _R.i = _X.i - _Y.i;                 \
}

// -------------------------------------------------------------------------
// PROTOTYPES
// -------------------------------------------------------------------------
static void bitrev (complex_t array[], const int N);

// -------------------------------------------------------------------------
// GLOBALS
// -------------------------------------------------------------------------

// Error messages
static char msgbuf[256] = {0};
char * fft_error_msg = msgbuf;

// -------------------------------------------------------------------------
// fft()                                                     
// -------------------------------------------------------------------------

int fft (complex_t x[], const int length, const int inverse)
{
#ifdef COS_TABLE
    // Size of the Cosine function table, and simple relations 
    int tablelen, tablelen34, tablelen_1;
#endif

    // Indexes for the two points of the array in the butterfly
    int idx, idx2;    
    // The current size of DFT being processed (and divided by 2)
    int n, ndiv2;     
    // The current power Wn is raised to, for butterfly calculation
    int k;            
    // Value of W for current n and k, and value of W for n with k = 1
    complex_t WkN, W1N; 
    // Temporary complex_t number holder
    complex_t tmp;

    // Clear error message
    msgbuf[0] = '\0';

    // If length not a power of 2, return without performing FFT
    idx2 = length-1;
    if((length < 2) || (length & idx2)) {
        sprintf(msgbuf, "fft(): Error! requested FFT length (%d) is not a power of 2", length);
        return FFT_ERRORSTATUS;
    }

#ifdef COS_TABLE

    // Calculate size of cosine table directly from the array
    tablelen   = sizeof(cos_table)/sizeof(cos_table[0]);
    tablelen_1 = tablelen - 1;
    tablelen34 = (3 * tablelen)/4;

    // If length exceeds the size of the cosine lookup table, return without performing FFT
    if(length > tablelen) {
        sprintf(msgbuf, "fft(): Error! requested FFT length (%d) exceeds maximum of %d", length, tablelen);
        return FFT_ERRORSTATUS;
    }
#endif

    // If inverse (synthesis) transform, pre-adjust values (conjugate)
    if(inverse)
        for(n = 0; n < length ; n++) 
            x[n].i *= -1.0;

    // Calculate N/2
    ndiv2 = length >> 1;

    // Bit reverse array x
    bitrev(x, length);

    // Loop for each 'n' point DFT stage
    for(n = 2; n <= length; n <<= 1) {
        ndiv2 = n>>1;    // n / 2

        // Initialise Wkn for k = 0
        WkN.r = 1.0; WkN.i = 0.0;                      

#ifdef COS_TABLE
        // Calculate Wkn for k = 1. cos_table maps 2*PI of cosine 
        // from 0 to tablelen-1, and '+ 3tablelen/4' is 3/4 PI shift to 
        // map -sine from cosine
        W1N.r = cos_table[tablelen/n];                     
        W1N.i = cos_table[(tablelen/n + tablelen34) & tablelen_1];  
#else
        W1N.r = cos((2 * M_PI)/n);
        W1N.i = cos((2 * M_PI)/n - M_PI_2); // -sin(2pi/n)
#endif

        // Loop for each power k between 0 and (n/2 - 1)
        for(k = 0; k < ndiv2; k++) {
            //                           k
            // For each butterfly using W  
            //                           n
            for(idx = k; idx < length; idx += n) {

                // Index across the DFT
                idx2 = idx + ndiv2;
                
                // Butterfly calculation

                // x[idx2] = x[idx] - x[idx2] * Wkn
                MULTC(tmp, x[idx2], WkN);
                SUBC(x[idx2], x[idx], tmp);

                // x[idx]  = x[idx] + x[idx2] * Wkn
                ADDC(x[idx], x[idx], tmp);
            }

            //  k+1    k    1
            // W    = W  * W
            //  n      n    n     
            tmp = WkN;
            MULTC(WkN, tmp, W1N);
        }
    }

    // If inverse (synthesis) transform, post-adjust values (conjugate)
    if(inverse) 
        for(n = 0; n < length ; n++) 
            x[n].i *= -1.0;
    else 
        for(n = 0; n < length ; n++) {
            x[n].i /= length;
            x[n].r /= length;
        }

    // Return with good status
    return FFT_OKSTATUS;
}  

// -------------------------------------------------------------------------
// Digital fourier transform
//
// Calculates the DFT of the data in array[], of 'length'
// points. An inverse transform is done if 'inverse' is true.
// Transformed data is returned in array[].
// -------------------------------------------------------------------------

int dft(complex_t array[], const int length, const int inverse)
{
    int n, i, j;
    double wk, real_s, imag_s ;
    complex_t *x;

    // Clear error message
    msgbuf[0] = '\0';

    // Must have at least 2 points to do a DFT
    if(length < 2) {
        sprintf(msgbuf, "dft(): Error! requested DFT length (%d) is less than minimum of 2", length);
        return FFT_ERRORSTATUS;
    }

    // Obtain some memory for the transform.
    if((x = malloc(length * sizeof(complex_t))) == NULL) {
        sprintf(msgbuf, "dft(): Error! unable to allocate memory");
        return FFT_ERRORSTATUS;
    }

    // Copy input data locally and clear input for accumulation of results
    for(i=0; i < length; i++) {
        x[i] = array[i];
        array[i].r = array[i].i = 0;
    }

    // If inverse (synthesis) transform, pre-adjust values (conjugate)
    if(inverse)
        for(n = 0; n < length ; n++) 
            x[n].i /= -1.0;

    // DFT loops
    for(i=0; i < length; i++) {
        for(j=0; j < length; j++) {
            wk = (2 * M_PI) * ((double)(i*j) / (double)length);
            real_s = cos(wk);
            imag_s = -1 * sin(wk);

            array[i].r += x[j].r * real_s - x[j].i * imag_s;
            array[i].i += x[j].r * imag_s + x[j].i * real_s;
        }
    }

    // If inverse (synthesis) transform, post-adjust values (conjugate)
    if(inverse)  {
        for(n = 0; n < length ; n++) {
            array[n].i /= -1.0 * length;
            array[n].r /= length;
        }
    }

    return FFT_OKSTATUS;
}

// -------------------------------------------------------------------------
// Bit reversal adapted from "The Scientist & Engineer's Guide
// to Digital Signal Processing", 2nd Ed., Steven W. Smith, 
// 1999
// -------------------------------------------------------------------------

static void bitrev(complex_t x[], const int n)
{
    int idx, a, b, ndiv2, nsub1;
    complex_t tmp;

    a = 0;
    ndiv2 = n/2;
    nsub1 = n-1;
    // For the entire array ...
    for (idx = 0; idx < nsub1; idx++) {
        // Swap elements if index is < bit reversed index (could use
        // 'greater than'---either would do; just don't swap twice)
        if(idx < a) {
            tmp = x[a];
            x[a] = x[idx];
            x[idx] = tmp;
        }
        // Calculate next n, which is bit reversed value of idx,
        // as limited by bit width for given length
        b = ndiv2;
        while(b <= a) {
            a -= b;
            b >>= 1;
        }
        a += b;
    }
}

