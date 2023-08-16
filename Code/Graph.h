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

#ifndef _GRAPH_H_
#define _GRAPH_H_ 

// -------------------------------------------------------------------------
// INCLUDES
// -------------------------------------------------------------------------

#define MAXNUMPOINTS    4096
#define BORDERFRACT     15
#define BORDERSTANDOFF  15

#define BLACK           RGB(0x00, 0x00, 0x00)
#define GREY            RGB(0x80, 0x80, 0x80)
#define WHITE           RGB(0xff, 0xff, 0xff)
#define RED             RGB(0xff, 0x00, 0x00)
#define GREEN           RGB(0x00, 0xff, 0x00)
#define BLUE            RGB(0x00, 0x00, 0xff)
#define YELLOW          RGB(0xff, 0xff, 0x00)
#define CYAN            RGB(0x00, 0xff, 0xff)
#define MAGENTA         RGB(0xff, 0x00, 0xff)
#define LTGREY          RGB(0x40, 0x40, 0x40)

#define BORDERCOLOUR    BLACK
#define PLOTCOLOUR      RED
#define TEXTCOLOUR      BLACK
#define RECTCOLOUR      LTGREY

#define MAXCTX          10
#define MAXSTRLEN       80

#define CTL_D           0x04

// -------------------------------------------------------------------------
// TYPEDEFS
// -------------------------------------------------------------------------

typedef struct {
    double x;
    double y;
} CoordType;

typedef struct {
    char *filename;
    char x_units[MAXSTRLEN];
    char y_units[MAXSTRLEN];
    char Title[MAXSTRLEN];
} GraphConf;

#endif
