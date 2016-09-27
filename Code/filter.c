//=============================================================
// 
// Copyright (c) 1999-2016 Simon Southwell. All rights reserved.
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
// $Id: filter.c,v 1.3 2016-09-27 08:42:56 simon Exp $
// $Source: /home/simon/CVS/src/dsp/WinFilter/Code/filter.c,v $
//
//=============================================================
//                                                           
// This program allows exploration of FIR filter
// design using various types of windows. A set of
// coefficients is produced (with Re and Im components)
// which may then be Fourier transformed to explore the      
// resulting frequency response.
//    Various parameters are variable: cut-off frequency,
// sampling frequency, number of taps, quantisation bits
// and Window parameter (alpha), as well as others.
//
// For more (excellent) information on DFTs, filtering and
// windows, try:
//
// www.spd.eee.strath.ac.uk/~interact/FFT/fourier.html       
//
// For a fantastic FREE book on digital signal processing try
//
// www.dspguide.com                                          
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
// EXTERNAL GLOBALS
// -------------------------------------------------------------------------

extern int ConfigErrorOccured;

// -------------------------------------------------------------------------
// PROTOTYPES
// -------------------------------------------------------------------------

extern int  glgraph_main       (int, char **);
extern void OutputCoefficients (complex_t [], real_t [], ConfigStruct *);

// -------------------------------------------------------------------------
// AnalyseFilter()
// -------------------------------------------------------------------------

int AnalyseFilter (int argc, char **argv)
{

    /* Local variables */
    ConfigStruct Config, *C=&Config;
    complex_t (*CmplxResult)[];
    real_t (*WindowBuf)[];
    int status;

    ConfigErrorOccured = FALSE;

    SetConfiguration(argc, argv, C);

    /* Set the configuration state from the command line
       and environment variables */
    if(ConfigErrorOccured == TRUE)
        return(0);

    /* Generate some memory space for the results */
    CmplxResult = malloc(COEFFTOTAL * sizeof(complex_t));
    WindowBuf = malloc(C->N * sizeof(real_t));

    /* Perform filter calculation for the given configuration (C),
       placing response and WindowBuf values in arrays (CmplxResult, 
       WindowBuf) */
    status = filter(*CmplxResult, *WindowBuf, C);

    /* Format and print out the coefficients */
    OutputCoefficients(*CmplxResult, *WindowBuf, C);

    return(status);
}


// -------------------------------------------------------------------------
// ErrorAction()
// -------------------------------------------------------------------------

void ErrorAction(const int Status)
{
    /* In Xfilter, no action (other than a dialog box) 
       is taken */

    ConfigErrorOccured = TRUE;
}
