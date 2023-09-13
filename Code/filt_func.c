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

// -------------------------------------------------------------------------
// INCLUDES
// -------------------------------------------------------------------------

#include <stdlib.h>
#include <math.h>

#include "filter.h"
#include "window.h"
#include "fft.h"

// -------------------------------------------------------------------------
// PROTOTYPES
// -------------------------------------------------------------------------

static void GenerateImpulse (real_t [], const ConfigStruct *);
static void Window (real_t [], real_t [], const ConfigStruct *);
static void Quantise (const real_t [], complex_t [], const ConfigStruct *);
static void Convolve (const real_t [], const real_t [], real_t [], const int);
static void Add (const real_t [], const real_t [], real_t [], const real_t, const real_t, const real_t, const int);

// -------------------------------------------------------------------------
// filter
//
// Returns filter kernel/frequency response in CmplxResult,
// and the window coefficients used in window. The filter
// design parameters and configuration are passed in with
// the config structure pointer. 
//                                                         
// -------------------------------------------------------------------------

DLLEXPORT int filter(complex_t CmplxResult[], real_t window[], ConfigStruct *config)
{
    ConfigStruct *C1=config, config2, *C2=&config2;
    real_t (*result)[], (*r1)[], (*r2)[];

    /* Generate some space for the 'real_t' results */
    result = (real_t (*)[]) malloc(COEFFTOTAL * sizeof(real_t));

    /* Bandpass/stop requested */
    if(C1->bandpass || C1->bandstop) {
        /* Create some space for intermediate results r1 and r2 */
        r1 = (real_t (*)[]) malloc(COEFFTOTAL * sizeof(real_t));
        r2 = (real_t (*)[]) malloc(COEFFTOTAL * sizeof(real_t));

        /* Generate a new configuration for inverted LPF at
           2nd cutoff frequency (C1->Fc is lower cutoff point, 
           C1->Fc+C1->Fw is upper cutoff point). Values recalculated
           to make this so, accounting for reversal etc.) */
        *C2 = *C1;
        C2->Fc = C1->Fs/2 - (C1->Fc + (C1->bandpass ? 0 : C1->Fw));
        C1->Fc = C1->Fc + (C1->bandpass ? C1->Fw : 0);
        C2->reversal = ~C1->reversal;
 
        /* Generate impulse response values for the two 
           specifications */
        GenerateImpulse(*r1, C1);
        GenerateImpulse(*r2, C2);
 
        /* For bandpass filtering, convolve the two results; for
           bandstop filtering add them */
        if(C1->bandpass)
            Convolve(*r1, *r2, *result, C1->N); 
        else
            Add(*r1, *r2, *result, C1->Fc, C2->Fc, C1->Fs, C1->N);

        /* Free up the temporary space */
        free(r1); free(r2);
    }
    else
        /* Generate the impulse response values and place in result */
        GenerateImpulse(*result, C1);
            
    /* Multiply impulse response by a window */
    Window(*result, window, C1);
 
    /* Quantise the result into integer values (if requested), 
       and cast into a complex array */
    Quantise(*result, CmplxResult, C1);

    /* Free up the results space */
    free(result);

    /* If impulse response wasn't requested, calculate frequency 
       response. (Values in CmplxResult overwritten.) */
    if(!C1->opimpulse)
        if(fft(CmplxResult, COEFFTOTAL, FALSE)) {
            DisplayMessage(1, (char **)fft_error_msg);
            return BADSTATUS;
        }

    /* Exit with good status */
    return(GOODSTATUS);
}

// -------------------------------------------------------------------------
// Quantise
//
// Takes the impulse response, which varies between +/-1.0,
// and scales to be between +/- 2**(Q-1) - 1 casting the
// result as an integer. These would then be the coefficients
// in a hardware implementation which uses integer
// arithmetic.
//
// -------------------------------------------------------------------------

static void Quantise (const real_t result[], complex_t CmplxResult[], const ConfigStruct *C)
{
    int n;
    real_t scale;

    /* This scale value ensures that the peak of the impulse response
       is 2**(N-1) - 1, giving maximum resolution for the quantisation.
       The resultant gain is 2**(N-1) / (2 * Fc/Fs) */
    scale = SCALEFACTOR / result[C->N / 2];

    /* Scale to have maximum value at '2**(Q-1) - 1' and cast as an 
       integer for studying quantisation effects. */
    for(n=0; n < COEFFTOTAL; n++) {
        if(n < C->N)
            CmplxResult[n].r =  (C->Q < 0) ? (real_t)((float)result[n]) :
                                (C->Q ? (real_t)((long64)(result[n] * scale)) : 
                                      result[n]);
        else
            CmplxResult[n].r = 0.0;

        CmplxResult[n].i = 0.0;
    }
}

// -------------------------------------------------------------------------
// GenerateImpulse
//
// Generates filter impulse response (with effectivel
// uniform windowing). If requested, will do spectral
// reversal.
// 
// -------------------------------------------------------------------------

static void GenerateImpulse (real_t result[], const ConfigStruct *C)
{
    int n;
    real_t tmp;

    /* For n = -pi to +pi */
    for(n = (-1 * C->N)/2; n <= C->N/2; n++) {
        /* X(n) is 'sinc(2 Pi n Fc/Fs).' If spectral reversal is
           selected, multiply odd coefficents by -1. */
        tmp = sinc((real_t)n, C->Fc, C->Fs, C->inversion) *
                             ((C->reversal && abs(n)%2) ? -1.0 : 1.0);
        result[n + C->N/2] = tmp;
    }
}

// -------------------------------------------------------------------------
// Window
//
// Multiplies the incoming results array by one of the
// selected window functions. Data in results must run from
// 0 to C->N, containing the impulse response for n between
// (-C->N/2) to (C->N/2 - 1)
//
// -------------------------------------------------------------------------

static void Window (real_t result[], real_t window[], const ConfigStruct *C)
{
    int n; 

    /* For n = -pi to +pi */
    for(n = (-1 * C->N)/2; n <= C->N/2; n++) {
        /* Calculate the window coefficient as determined by 
           function pointed to with *(C->windowfunc).) */
        window[n+(C->N/2)] = (*(C->windowfunc)) (C->a, (real_t) n, (real_t)C->N);

        /* Multiply result by current window coefficient */
        result[n + C->N/2] *= window[n+(C->N/2)]; 
    }
}

// -------------------------------------------------------------------------
// Convolution of two arrays over M points
// -------------------------------------------------------------------------

static void Convolve (const real_t s1[], const real_t s2[], real_t r[], const int M)
{
    int i, j, x;

    /* Convolve data */
    for(x=0; x < M; x++) {
        /* Clear accumulation */
        r[x] = 0.0;

        /* Convolving from M/2 to 3M/2, other indices are 0 */
        i = x + M/2;

        /* Convolve first signal with second */
        for(j=0; j < M; j++)
            if((i-j) >= 0 && (i-j) < M)
                r[x] += s1[j] * s2[i-j];
    }
}

// -------------------------------------------------------------------------
// Addition of two arrays with frequency scaling
// -------------------------------------------------------------------------

static void Add (const real_t s1[], const real_t s2[], real_t r[],
                 const real_t Fc1, const real_t Fc2, const real_t Fs, const int M)
{
    int n;

    /* Simply add each element of the two arrays */
    for(n=0; n < M; n++)
        r[n] = (s1[n] + s2[n]);
}

