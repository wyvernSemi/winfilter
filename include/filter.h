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
// $Id: filter.h,v 1.3 2016-09-27 08:42:34 simon Exp $
// $Source: /home/simon/CVS/src/dsp/WinFilter/include/filter.h,v $
//
//=============================================================

#ifndef _FILTER_H_
#define _FILTER_H_

// -------------------------------------------------------------------------
// INCLUDES
// -------------------------------------------------------------------------

#include <stdio.h>
#include "fft.h"
#include "window.h"

#ifdef	__cplusplus
#define DLLEXPORT extern "C" 
#else 
#define DLLEXPORT
#endif

// -------------------------------------------------------------------------
// TYPEDEFS
// -------------------------------------------------------------------------

typedef double real_t;
typedef unsigned int uint_t;
typedef unsigned char uchar_t;

/* Configuration parameter structure */
typedef struct { 
    uint_t 	opimpulse : 1;
    uint_t 	opwindow  : 1;
    uint_t 	inversion : 1;
    uint_t 	reversal  : 1;
    uint_t 	bandpass  : 1;
    uint_t 	bandstop  : 1;
    uint_t 	decibels  : 1;
    uint_t 	magnitude : 1;
    uint_t 	phase 	  : 1;
    uint_t 	removeplot: 1;
    uint_t 	Xgraph    : 1;
    uint_t 	automode  : 1;
    uint_t      normalise : 1;
    real_t      (*windowfunc)();
    FILE 	*fp;
    FILE 	*wfp;
    uchar_t     window;
    char 	*filename;
    char        *wfilename;
    char 	*plotprog;
    real_t 	a;
    real_t 	ripple;
    long 	Q;
    long 	N; 
    real_t 	Fc;
    real_t 	Fd;
    real_t 	Fw;
    real_t 	Fs;
    real_t 	Ft;
    real_t 	attenuation;} ConfigStruct;


#ifdef WIN32
typedef _int64 long64;
#else
typedef long long long64;
#endif

// -------------------------------------------------------------------------
// DEFINES
// -------------------------------------------------------------------------

/* Basic definitions */
#ifndef TRUE
#define TRUE (1==1)
#define FALSE (1!=1)
#endif

#define GOODSTATUS 0
#define BADSTATUS 1

#define DEFAULT_STR_SIZE 80

/* Value for unset parameters */
#define UNSET -1.0

/* To do with graph plotting */
#if defined(_MSDOS)

/* Nasty hack for DOS graph plotting --- needs trailing ^M */
#define TRAILCHAR 0x0d
#define XFILENAME "filter.dat"
#define WINDOWFILENAME "window.dat"
#define WINPLOTARGS ""
#define XPLOTPROG "curveview"

#else

#define TRAILCHAR 0x20
//#define XFILENAME "filter.xg"
//#define WINDOWFILENAME "window.xg"
#define WINPLOTARGS "-m -ly 0,1"
#define XPLOTPROG "xgraph -bb -tk -fg black -bg \"#e0e0e0\""

#endif

#define PLOTMINIMUM -400

/* Configuraton default values */
#define DEFAULT_winchar		'h'
#define DEFAULT_wstr		"Hamming"
#define DEFAULT_windowfunc	hamming
#define DEFAULT_opimpulse	FALSE
#define DEFAULT_opwindow 	FALSE
#define DEFAULT_inversion  	FALSE
#define DEFAULT_reversal  	FALSE
#define DEFAULT_bandpass 	FALSE
#define DEFAULT_bandstop 	FALSE
#define DEFAULT_decibels 	TRUE
#define DEFAULT_magnitude 	FALSE
#define DEFAULT_phase	 	FALSE
#define DEFAULT_Xgraph	 	TRUE
#define DEFAULT_automode 	FALSE
#define DEFAULT_normalise	TRUE
#define DEFAULT_filename	"filter.dat"
#define DEFAULT_winfilename	"window.dat"
#define DEFAULT_plotprog        XPLOTPROG
#define DEFAULT_removeplot	FALSE
#define DEFAULT_ripple	 	0.0
#define DEFAULT_Fd       	-1.0
#define DEFAULT_a 	 	DEFAULT_HAMMING_ALPHA
#define DEFAULT_N 	 	120 
#define DEFAULT_Q 	 	0
#define DEFAULT_Fc 	 	20000.0 
#define DEFAULT_Fw 	 	10000.0 
#define DEFAULT_Fs 	 	192000.0 
#define DEFAULT_Ft		4000.0
#define DEFAULT_attenuation	-60.0

/* So useful, make it a definition */
//#define TWOPI (real_t)(2.00 * PI)

/* Macro to turn the specified bit width into a scaling factor */
#define SCALEFACTOR (real_t)(C->Q ? (((long64)1<<((long64)(C->Q-1))) - (long64)1) : 1)

/* Actual number of coefficients to be output (i.e. padded with 0s) */
#define COEFFTOTAL (4 * 1024)

#define SMALLNUMBER -1e-35

// -------------------------------------------------------------------------
// PROTOTYPES
// -------------------------------------------------------------------------

/* Function prototypes */
DLLEXPORT int  filter(complex_t [], real_t[], ConfigStruct *);

extern void ErrorAction(const int);
extern void DisplayMessage(const int, char **);

#endif
