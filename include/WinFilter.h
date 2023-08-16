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

#ifndef _WINFILTER_H_
#define _WINFILTER_H_

// -------------------------------------------------------------------------
// INCLUDES
// -------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

// -------------------------------------------------------------------------
// DEFINES
// -------------------------------------------------------------------------

/* Define the fonts to use */
#define DEFAULTLABELFONT       "lubBI14"
#define DEFAULTBUTTONFONT      "timR14"
#define DEFAULTTEXTFIELDFONT   "fixed"

/* Default margin width */
#define DEFAULTMARGINWIDTH     5

#define MAXARGS                4

#define MAXCMDARGS             20
#define MAXDISPLINES           50

#define MAXFILENAMELEN         1024
#define MAXSTRSIZE             80
#define MAXMESSAGELINES        80
#define X_SIZE                 591
#define Y_SIZE                 508

#define FLAGCHK                        \
      if(!flagset) {                   \
     flagset=TRUE;                     \
     strcpy(cmdstr[argcount], "-");    \
      }

// -------------------------------------------------------------------------
// PROTOTYPES
// -------------------------------------------------------------------------
      
extern LRESULT CALLBACK WndProc         (HWND, UINT, WPARAM, LPARAM);
extern BOOL    CALLBACK DlgProc         (HWND, UINT, WPARAM, LPARAM);
extern void             SetDefaults     (ConfigStruct *);
extern void             ExecuteAnalysis (ConfigStruct *);

#endif
