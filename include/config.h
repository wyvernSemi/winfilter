//=============================================================
// 
// Copyright (c) 1999-2016 Simon Southwell. All rights reserved.
//
// Date: 11th March 1999
//
// Configuration function prototype.
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
// $Id: config.h,v 1.2 2016-09-27 08:42:34 simon Exp $
// $Source: /home/simon/CVS/src/dsp/WinFilter/include/config.h,v $
//
//=============================================================

#ifndef _CONFIG_H_
#define _CONFIG_H_

// -------------------------------------------------------------------------
// DEFINES
// -------------------------------------------------------------------------

#define GOODSTATUS 0
#define BADSTATUS  1

// -------------------------------------------------------------------------
// FUNCTION PROTOTYPES
// -------------------------------------------------------------------------

extern int SetConfiguration (int, char **, ConfigStruct *);

#endif

