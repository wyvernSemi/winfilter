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
// $Id: window.h,v 1.2 2016-09-27 08:42:34 simon Exp $
// $Source: /home/simon/CVS/src/dsp/WinFilter/include/window.h,v $
//
//=============================================================

#ifndef _WINDOW_H_
#define _WINDOW_H_

// -------------------------------------------------------------------------
// INCLUDES
// -------------------------------------------------------------------------

#include "fft.h"

// -------------------------------------------------------------------------
// DEFINES
// -------------------------------------------------------------------------

#ifdef WIN32
#define acosh(_X) (log(_X + sqrt(_X-1) * sqrt(_X+1)))
#endif


/* Some alpha values for hamming window to give less
   general window outputs */
#define UNIFORM_ALPHA         0.00
#define VONHANN_ALPHA         0.25
#define DEFAULT_KAISER_ALPHA  5.4
#define DEFAULT_COMMON_ALPHA  2.0
#define DEFAULT_TUKEY_ALPHA   0.5
#define DEFAULT_HAMMING_ALPHA 0.23
#define DEFAULT_COSINE_ALPHA  1.0
#define NULL_ALPHA            0.0

#define BADWINDOWSTATUS 1

// -------------------------------------------------------------------------
// TYPEDEFS
// -------------------------------------------------------------------------

typedef struct {
   int  N;
   real_t a;
} KaiserParamStruct;

// -------------------------------------------------------------------------
// PROTOTYPES
// -------------------------------------------------------------------------

/* Externally defined function (factorial.c) */
extern real_t            factorial            (const int);

/* Function prototype declarations */
extern real_t            sinc                 (const real_t, const real_t, const real_t, const unsigned int);
extern real_t            bartlett             (const real_t, const real_t, const real_t);
extern real_t            hamming              (const real_t, const real_t, const real_t);
extern real_t            bohman               (const real_t, const real_t, const real_t);
extern real_t            vallepoisson         (const real_t, const real_t, const real_t);
extern real_t            tukey                (const real_t, const real_t, const real_t);
extern real_t            riemann              (const real_t, const real_t, const real_t);
extern real_t            reisz                (const real_t, const real_t, const real_t);
extern real_t            poisson              (const real_t, const real_t, const real_t);
extern real_t            cosine               (const real_t, const real_t, const real_t);
extern real_t            cauchy               (const real_t, const real_t, const real_t);
extern real_t            blackman             (const real_t, const real_t, const real_t);
extern real_t            blackman_harris      (const real_t, const real_t, const real_t);
extern real_t            nuttall              (const real_t, const real_t, const real_t);
extern real_t            kaiser               (const real_t, const real_t, const real_t);
extern real_t            gauss                (const real_t, const real_t, const real_t);
extern real_t            chebyshev            (const real_t, const real_t, const real_t);
extern KaiserParamStruct design_kaiser_filter (const real_t, const real_t, const real_t);

#endif

