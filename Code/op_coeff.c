//=============================================================
// 
// Copyright (c) 1999-2016 Simon Southwell. All rights reserved.
//
// Date: 11th March 1999
//
// Outputs the result (whether impulse or frequency response)
// to the file stream of C->fp. If graph plotting
// is required, then the system call is made here, using the
// filed data just output. Similarly WindowBuf data is output
// and plotted if requested.
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
// $Id: op_coeff.c,v 1.3 2016-09-27 08:42:56 simon Exp $
// $Source: /home/simon/CVS/src/dsp/WinFilter/Code/op_coeff.c,v $
//
//=============================================================

// -------------------------------------------------------------------------
// INCLUDES
// -------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef WIN32
#include <strings.h>
#else
#include <string.h>
#endif

#include "filter.h"
#include "config.h"

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

void OutputCoefficients (complex_t result[], real_t WindowBuf[], ConfigStruct *C)
{
    char buf[DEFAULT_STR_SIZE], *str=buf;
    int n, idx=0;
    real_t mag[COEFFTOTAL], max=SMALLNUMBER, mag_dB, phase[COEFFTOTAL];
    real_t freq_step;

    /* If requested, output the WindowBuf coefficients to stderr */
    if(C->opwindow) {

        if(!strncmp(C->plotprog, "xgraph", 6)) {
            fprintf(C->wfp, "TitleText: WindowBuf function (a = %.2lf)\n", C->a);
            fprintf(C->wfp, "XUnitText: Tap\n");
            fprintf(C->wfp, "YUnitText: Value\n");
        }

        for(n=(-1 * C->N/2); n < C->N/2; n++)
            fprintf(C->wfp, "%d %.20lf%c\n", n, WindowBuf[n+(C->N/2)], TRAILCHAR);
 
        fflush(C->wfp);
        fclose(C->wfp);
    }

    /* The actual step in frequency between adjacent points is the
       sampling frequency divided by the total number of points */
    freq_step = C->Fs/(real_t)COEFFTOTAL;

    /* Output impulse response coefficients as train of integers (if Q > 0),
       or as real_t numbers */
    if(C->opimpulse)
        for(n=0; n < C->N; n++)
            if(C->Q > 0)
                fprintf(C->fp, "%d %5d%c\n", n, (int)result[n].r, TRAILCHAR);
            else
                fprintf(C->fp, "%d %.20e%c\n", n, result[n].r, TRAILCHAR);

    /* Output frequency response (if not in dBs) scaled by max quantised
       impulse response value to make independant of Q */
    else if(!C->decibels && !C->magnitude && !C->phase)
        for(n=0; n < COEFFTOTAL; n++)
            fprintf(C->fp, "%.20e %.20e%c\n", result[n].r, result[n].i, TRAILCHAR);

    /* Frequency response output to be in dBs */
    else {
        /* Calculate magnitude  and phase values from complex results, 
           and find maximum magnitude value for normalisation later on */
        for(n=0; n < COEFFTOTAL; n++) {
            mag[n] = sqrt(result[n].r*result[n].r + result[n].i*result[n].i);
            phase[n] = (real_t)180.0 * atan(result[n].i/result[n].r)/M_PI;

            /* Correct the phase to be in the right quadrant, based on the
               sign of the real_t and imaginary parts */
            if((result[n].r < 0.0) && (result[n].i < 0.0))
                phase[n] -= (real_t)180.0;
            else if((result[n].r < 0.0) && (result[n].i >= (real_t)0.0))
                phase[n] += (real_t)180.0;
            
            if(mag[n] > max)
                max = mag[n];
        }

        /* Add some labels for Xgraph plotting */
        if(C->Xgraph && (!strncmp(C->plotprog, "xgraph", 6) || !strncmp(C->plotprog, "glgraph", 7))) {
            fprintf(C->fp, "TitleText: Frequency Response\n");
            fprintf(C->fp, "XUnitText: Freq. (Hz)\n");
            if(C->decibels)
               fprintf(C->fp, "YUnitText: Mag. (dB)\n");
            else if (C->phase)
               fprintf(C->fp, "YUnitText: Phase (deg)\n");
            else
               fprintf(C->fp, "YUnitText: Mag.\n");
        }

        /* Print out normalised response */
        for(n=0; n < COEFFTOTAL/2; n++) {
            /* Magnitude in decibels is 20log(mag(n)). Magnitude
               normalised by dividing with maximum value */
            if(C->decibels)
                mag_dB = 20 * log10(mag[n]/max);

            /* Bug fix for when log_mag is -infinity */
            if(mag_dB < PLOTMINIMUM)
               mag_dB = PLOTMINIMUM; 

            /* Print out the selected response values */
            fprintf(C->fp, "%d %.20e%c\n", (int)((real_t)n*freq_step), 
                                       C->phase ? phase[n] :
                                      (C->magnitude ? mag[n]/max : mag_dB), TRAILCHAR);
        }
    }
    fflush(C->fp);
    fclose(C->fp);


    /* If requested, plot the data using xgraph */
    if(C->Xgraph) {
        fflush(NULL);

        /* If the file was only for plotting, delete it after we're done */
        system("sleep 1");
        if(C->removeplot) {
            fclose(C->fp);
            sprintf(str, "rm -f %s", C->filename);
            system(str);
        }
    }
}
