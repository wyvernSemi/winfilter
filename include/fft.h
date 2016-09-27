//=============================================================
// 
// Copyright (c) 1999-2016 Simon Southwell. All rights reserved.
//
// Date: 11th March 1999
//
// Complex Fourier Transform functions.
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
// $Id: fft.h,v 1.2 2016-09-27 08:42:34 simon Exp $
// $Source: /home/simon/CVS/src/dsp/WinFilter/include/fft.h,v $
//
//=============================================================
//
// Complex Fourier Transform functions, using decimation in
// time. Array array, of size N and type complex_t, is passed
// in and results returned in the same array.
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
// array[] - 'complex' array pointer (type specified in fft.h) 
//           containing data points. If data real, set
//           imaginary points to 0. Resultant data returned in 
//           this array.
//
// N       - int containing length of data array. Must be power
//           of 2 if calling fft().
//
// inverse - int flag. If non-zero, inverse (synthesis)
//           transform perfomed.
//
// RETURN:
//
// Both fft() and dft() return either FFT_OKSTATUS or 
// FFT_ERRORSTATUS. For the latter, fft_error_msg points
// to an error message string. Subsequent calls to fft or
// dft will clear any previous message. Transformed data
// placed in array pointed to by array[].
//
//=============================================================

#ifndef _FFT_H_
#define _FFT_H_

// -------------------------------------------------------------------------
// DEFINES
// -------------------------------------------------------------------------

#ifdef WIN32
#define M_PI 3.1415926535897932384643
#define M_PI_2 (M_PI/2.0)
#endif

#define FFT_ERRORSTATUS 1
#define FFT_OKSTATUS    0

// -------------------------------------------------------------------------
// TYPEDEFS
// -------------------------------------------------------------------------

// Define the basic numerical type (makes it easier to change)
typedef double real_t;

// Complex type
typedef struct {
    real_t r; 
    real_t i;
} complex_t;

// -------------------------------------------------------------------------
// PROTOTYPES
// -------------------------------------------------------------------------

// Exported function prototypes
extern int fft (complex_t array[], const int N, const int inverse);
extern int dft (complex_t array[], const int N, const int inverse);

// Error message pointer
extern char *fft_error_msg;

#endif
