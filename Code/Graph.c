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

#include <windows.h>
#include <math.h>
#include <stdio.h>
#include "Graph.h"

// -------------------------------------------------------------------------
// PROTOTYPES
// -------------------------------------------------------------------------

LRESULT CALLBACK GraphCallback (HWND, UINT, WPARAM, LPARAM) ;
static int ReadData(CoordType *, char *, double *, 
                    double *, double *, double *);


// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

#ifndef WINFILTER

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow)

{
    static TCHAR szAppName[] = TEXT ("Graph") ;
    HWND         hwnd ;
    MSG          msg ;
    WNDCLASS     wndclass ;
    GraphConf    gphc;
    
    wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
    wndclass.lpfnWndProc   = GraphCallback ;
    wndclass.cbClsExtra    = 0 ;
    wndclass.cbWndExtra    = 0 ;
    wndclass.hInstance     = hInstance ;
    wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
    wndclass.hbrBackground = (HBRUSH) GetStockObject (LTGRAY_BRUSH) ;
    wndclass.lpszMenuName  = NULL ;
    wndclass.lpszClassName = szAppName ; 

    RegisterClass (&wndclass);

    gphc.filename = "filter.dat";
            
    strncpy(gphc.x_units, "", MAXSTRLEN);
    strncpy(gphc.Title, "Graph", MAXSTRLEN);
    strncpy(gphc.y_units, "", MAXSTRLEN);
    strncpy(gphc.x_units, "", MAXSTRLEN);
    
    hwnd = CreateWindow (szAppName, TEXT ("Graph"),
                         WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT,
                         CW_USEDEFAULT, CW_USEDEFAULT,
                         NULL, NULL, hInstance, &gphc) ;
    
    ShowWindow (hwnd, iCmdShow) ;
    UpdateWindow (hwnd) ;
    
    while (GetMessage (&msg, NULL, 0, 0))
    {
         TranslateMessage (&msg) ;
         DispatchMessage (&msg) ;
    }
    return msg.wParam ;
}

#endif

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

void PlotValueY(HDC hdc, int cy_b, int cy_c, int cx_b, double miny, double maxy, double value)
{
    int    tmp;
    double frac;
    char   str[MAXSTRLEN] ;

    frac = (1.0-fabs(value/miny)) * (double)(cy_c - 2 * cy_b);

    tmp = cy_b + (int)fabs(floor(frac));
    MoveToEx(hdc, cx_b - BORDERSTANDOFF, tmp, NULL);
    LineTo(hdc, cx_b - BORDERSTANDOFF/2, tmp);
    sprintf(str, "%3.1lf", value);
    TextOut(hdc, cx_b - BORDERSTANDOFF - 5, tmp , str, strlen(str));
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------/

LRESULT CALLBACK GraphCallback (HWND hwnd, UINT message, 
                                WPARAM wParam, LPARAM lParam)
{
    // These static variables are unique to each open window, and
    // are identified by cross referencing the window handle against
    // the index into the variables.
    static int cxClient[MAXCTX], cyClient[MAXCTX], n[MAXCTX] ;
    static int cxBorder[MAXCTX], cyBorder[MAXCTX];
    static CoordType pts[MAXCTX][MAXNUMPOINTS];
    static HPEN hpen[MAXCTX], hpenB[MAXCTX], hpenT[MAXCTX], hpenR[MAXCTX];
    static double min_x[MAXCTX], min_y[MAXCTX];
    static double max_x[MAXCTX], max_y[MAXCTX];
    static HWND hwnd_ctx[MAXCTX]; 
    static GraphConf gphc[MAXCTX];
    static int ctx_active[MAXCTX], ctx_init = FALSE;
    static int ctx_id = 0;
    static boolean button_down[MAXCTX];

    static int smouse_x, smouse_y, emouse_x, emouse_y;
    static int cmouse_x, cmouse_y, old_cmouse_x, old_cmouse_y;

    HDC         hdc ;
    int         i, this_ctx = MAXCTX-1; 
    PAINTSTRUCT ps ;
    POINT       apt [MAXNUMPOINTS] ;
    char        str[MAXSTRLEN] ;
    int DrawingMode;
    double scale_y;
    //double tmp;
    RECT rect;
    HRGN hrgn;

    switch (message) {
    case WM_CREATE:
        // If this is the first call, initialise the context active status
        if(ctx_init == FALSE) {
            ctx_init = TRUE;
            for(i = 0; i < MAXCTX; i++)
                ctx_active[i] = FALSE;
        }
        // Find a free context
        for(ctx_id = 0; ctx_id < MAXCTX; ctx_id++)
            if(ctx_active[ctx_id] == FALSE)
                break;
        // If there's at least one free context, use it...
        if(ctx_id < MAXCTX) {
            // Filname ppointer passed in via the lpCreateParams field of the
            // CREATESTRUCT pointed to by lParam
            gphc[ctx_id] = 
                 *((GraphConf *)(((CREATESTRUCT *)lParam)->lpCreateParams));
 
            // Read in the data from the file (n = number of points read)
            n[ctx_id] = ReadData(pts[ctx_id], gphc[ctx_id].filename, 
                                 &max_x[ctx_id], &max_y[ctx_id], 
                                 &min_x[ctx_id], &min_y[ctx_id]);
 
            // Create a few pens for drawing
            hpen[ctx_id]  = CreatePen(PS_SOLID, 0, PLOTCOLOUR);
            hpenT[ctx_id] = CreatePen(PS_SOLID, 0, TEXTCOLOUR);
            hpenB[ctx_id] = CreatePen(PS_DOT,   1, BORDERCOLOUR); 
            hpenR[ctx_id] = CreatePen(PS_DOT,   0, RECTCOLOUR);
            hwnd_ctx[ctx_id] = hwnd;
 
            ctx_active[ctx_id] = TRUE;
            button_down[ctx_id] = FALSE;
 
        } else {
            MessageBox (NULL, 
                        TEXT("Graph: reached maximum number of open windows.\nClose an open window, and try again."), 
                        TEXT("Error"), MB_ICONEXCLAMATION);
            DestroyWindow(hwnd);
        }

        return 0;

    case WM_SIZE:
        // Identify the context from the list of window handles
        for(i = 0; i < MAXCTX; i++) {
            if(hwnd_ctx[i] == hwnd) {
                this_ctx = i;
                break;
            }
        }

        // Get the new window client area size
        cxClient[this_ctx] = LOWORD (lParam) ;
        cyClient[this_ctx] = HIWORD (lParam) ;

        // Calculate scaled borders
        cxBorder[this_ctx] = 60; 
        cyBorder[this_ctx] = 60;
        return 0 ;
         
    case WM_PAINT:
        hdc = BeginPaint (hwnd, &ps) ;

        // Identify the context from the list of window handles
        for(i = 0; i < MAXCTX; i++) {
            if(hwnd_ctx[i] == hwnd) {
                this_ctx = i;
                break;
            }
        }

        SetMapMode(hdc, MM_ANISOTROPIC);
        SetWindowExtEx(hdc, cxClient[this_ctx], cyClient[this_ctx], NULL);
        SetViewportExtEx(hdc, cxClient[this_ctx], -1 * cyClient[this_ctx], NULL);
        SetViewportOrgEx(hdc, 0, cyClient[this_ctx], NULL);

        // Gaps between dotted lines etc. are transparent (not white)
        SetBkMode(hdc, TRANSPARENT);

        // Select the border pen
        SelectObject(hdc, hpenB[this_ctx]);

        // Draw a box around the main area
        MoveToEx(hdc, cxBorder[this_ctx] - BORDERSTANDOFF, 
                      cyBorder[this_ctx] - BORDERSTANDOFF, NULL);

        LineTo (hdc, cxBorder[this_ctx] - BORDERSTANDOFF, 
                     cyClient[this_ctx] - cyBorder[this_ctx] + BORDERSTANDOFF);

        LineTo (hdc, cxClient[this_ctx] - cxBorder[this_ctx] + BORDERSTANDOFF, 
                     cyClient[this_ctx] - cyBorder[this_ctx] + BORDERSTANDOFF);

        LineTo (hdc, cxClient[this_ctx] - cxBorder[this_ctx] + BORDERSTANDOFF, 
                     cyBorder[this_ctx] - BORDERSTANDOFF);

        LineTo (hdc, cxBorder[this_ctx] - BORDERSTANDOFF, 
                     cyBorder[this_ctx] - BORDERSTANDOFF);

        SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
        SelectObject(hdc, hpenT[this_ctx]);

        SetTextAlign(hdc, TA_CENTER | TA_TOP);
        TextOut(hdc, cxClient[this_ctx]/2, cyClient[this_ctx] - 10, gphc[this_ctx].Title, 
                     strlen(gphc[this_ctx].Title));

        SetTextAlign(hdc, TA_RIGHT | TA_BASELINE);

        MoveToEx(hdc, cxBorder[this_ctx] - BORDERSTANDOFF, 
                      cyClient[this_ctx] - cyBorder[this_ctx], NULL);
        LineTo(hdc, cxBorder[this_ctx] - BORDERSTANDOFF/2, 
                    cyClient[this_ctx] - cyBorder[this_ctx]);

        sprintf(str, "%3.1lf", max_y[this_ctx]);
        TextOut(hdc, cxBorder[this_ctx] - BORDERSTANDOFF - 5, 
                     cyClient[this_ctx] - cyBorder[this_ctx],
                     str, strlen(str));

        MoveToEx(hdc, cxBorder[this_ctx] - BORDERSTANDOFF, cyClient[this_ctx]/2, NULL);
        LineTo(hdc, cxBorder[this_ctx] - BORDERSTANDOFF/2, cyClient[this_ctx]/2);
        sprintf(str, "%3.1lf", (max_y[this_ctx] + min_y[this_ctx])/2.0);
        TextOut(hdc, cxBorder[this_ctx] - BORDERSTANDOFF - 5, cyClient[this_ctx]/2 , str, strlen(str));


        MoveToEx(hdc, cxBorder[this_ctx] - BORDERSTANDOFF, 
                      cyBorder[this_ctx], NULL);
        LineTo(hdc, cxBorder[this_ctx] - BORDERSTANDOFF/2, 
                    cyBorder[this_ctx]);

        sprintf(str, "%3.1lf", min_y[this_ctx]);
        TextOut(hdc, cxBorder[this_ctx] - BORDERSTANDOFF - 5, 
                     cyBorder[this_ctx] ,
                     str, strlen(str));

        MoveToEx(hdc, cxBorder[this_ctx], 
                      cyBorder[this_ctx] - BORDERSTANDOFF, 
                      NULL);
        LineTo(hdc, cxBorder[this_ctx], 
                    cyBorder[this_ctx] - BORDERSTANDOFF/2);

        SetTextAlign(hdc, TA_LEFT | TA_TOP);
        sprintf(str, "%3.1lf", pts[this_ctx][0]);
        TextOut(hdc, cxBorder[this_ctx], 
                     cyBorder[this_ctx] - 20,
                     str, strlen(str));

        MoveToEx(hdc, cxClient[this_ctx]/2, 
                      cyBorder[this_ctx] - BORDERSTANDOFF, 
                      NULL);
        LineTo(hdc, cxClient[this_ctx]/2, 
                    cyBorder[this_ctx] - BORDERSTANDOFF/2);

        SetTextAlign(hdc, TA_CENTER);
        sprintf(str, "%3.1lf", pts[this_ctx][n[this_ctx]/2]);
        TextOut(hdc, cxClient[this_ctx]/2, 
                     cyBorder[this_ctx] - 20,
                     str, strlen(str));

        MoveToEx(hdc, cxClient[this_ctx] - cxBorder[this_ctx], 
                      cyBorder[this_ctx] - BORDERSTANDOFF, 
                      NULL);
        LineTo(hdc, cxClient[this_ctx] - cxBorder[this_ctx], 
                    cyBorder[this_ctx] - 
                                         BORDERSTANDOFF/2);

        SetTextAlign(hdc, TA_LEFT | TA_TOP);
        sprintf(str, "%3.1lf", pts[this_ctx][n[this_ctx]-1].x);
        TextOut(hdc, cxClient[this_ctx] - cxBorder[this_ctx],
                     cyBorder[this_ctx] - 20,
                     str, strlen(str));

        SetTextAlign(hdc, TA_RIGHT);
        TextOut(hdc, cxBorder[this_ctx] - BORDERSTANDOFF - 5, 
                cyClient[this_ctx] - cyBorder[this_ctx]/2, 
                gphc[this_ctx].y_units, 
                strlen(gphc[this_ctx].y_units));

        SetTextAlign(hdc, TA_LEFT);
        TextOut(hdc, cxClient[this_ctx] - cxBorder[this_ctx] + 
                                          BORDERSTANDOFF + 5, 
                cyBorder[this_ctx]/2 - 5 , 
                gphc[this_ctx].x_units, 
                strlen(gphc[this_ctx].x_units));

        // Set the view port to be within the border, and map the
        // plot values to this area. (Scale the y coordinates to fit
        // the y drawing area to avoid quantisation errors)
        scale_y = (cyClient[this_ctx] - cyBorder[this_ctx]*2) / (max_y[this_ctx] - min_y[this_ctx]);
        SetWindowExtEx(hdc, n[this_ctx], 
                            (int)(scale_y * (max_y[this_ctx] - min_y[this_ctx])), NULL);
        SetWindowOrgEx(hdc, (int)0, (int)(scale_y * min_y[this_ctx]), NULL);

        SetViewportExtEx(hdc, cxClient[this_ctx] - cxBorder[this_ctx]*2, 
                              -1 * (cyClient[this_ctx] - cyBorder[this_ctx]*2), NULL);
        SetViewportOrgEx(hdc, cyBorder[this_ctx], cyClient[this_ctx]-cyBorder[this_ctx], NULL);

        hrgn = CreateRectRgn(cxBorder[this_ctx]-1, 
                             cyBorder[this_ctx]-1, 
                             cxClient[this_ctx] - cxBorder[this_ctx]+1,
                             cyClient[this_ctx] - cyBorder[this_ctx]+1);
        SelectClipRgn(hdc, hrgn);

        // Calculate the plot the data
        for (i = 0 ; i < n[this_ctx] ; i++) {
            apt[i].x = i;
                       
            apt[i].y = (long)(scale_y * pts[this_ctx][i].y);

        }

        // Select the plot pen
        SelectObject(hdc, hpen[this_ctx]);
         
        // Plot the data
        Polyline (hdc, apt, n[this_ctx]) ;

        // If the mouse button is down, draw a selection box
        if(button_down[this_ctx]) {
            SetWindowExtEx(hdc, cxClient[this_ctx], cyClient[this_ctx], NULL);
            SetWindowOrgEx(hdc, 0, 0, NULL);
            SetViewportExtEx(hdc, cxClient[this_ctx], cyClient[this_ctx], NULL);
            SetViewportOrgEx(hdc, 0, 0, NULL);

            hrgn = CreateRectRgn(cxBorder[this_ctx] - BORDERSTANDOFF, 
                                 cyBorder[this_ctx] - BORDERSTANDOFF, 
                                 cxClient[this_ctx] - cxBorder[this_ctx] + BORDERSTANDOFF,
                                 cyClient[this_ctx] - cyBorder[this_ctx] + BORDERSTANDOFF);
            SelectClipRgn(hdc, hrgn);

            // Remember current drawing mode
            DrawingMode = GetROP2 (hdc);
            // Set drawing mode to be exclusive or, so we can erase
            // the old selection rectangle
            SetROP2(hdc, R2_XORPEN);
            // Select the appropriate pen and brush 
            SelectObject(hdc, hpenR[this_ctx]); 
            SelectObject(hdc, GetStockObject(NULL_BRUSH));
            // Erase old rectangle
            Rectangle(hdc, smouse_x, old_cmouse_y, 
                      old_cmouse_x, smouse_y);
            // Draw new selection rectangle
            Rectangle(hdc, smouse_x, cmouse_y, 
                      cmouse_x, smouse_y);
            // Restore white brush
            SelectObject(hdc, GetStockObject(WHITE_BRUSH));
            // Restore original drawing mode
            SetROP2(hdc, DrawingMode);
        }
        return 0 ;

    case WM_LBUTTONDOWN:
        for(i = 0; i < MAXCTX; i++) {
            if(hwnd_ctx[i] == hwnd) {
                this_ctx = i;
                break;
            }
        }
        smouse_x = LOWORD(lParam);
        smouse_y = HIWORD(lParam);
        cmouse_x = old_cmouse_x = smouse_x;
        cmouse_y = old_cmouse_y = smouse_y;
        button_down[this_ctx] = TRUE;
        
        return 0;

    case WM_LBUTTONUP:
    case WM_NCMOUSEMOVE:
        for(i = 0; i < MAXCTX; i++) {
            if(hwnd_ctx[i] == hwnd) {
                this_ctx = i;
                break;
            }
        }
        if(message == WM_LBUTTONUP) {
            emouse_x = LOWORD(lParam);
            emouse_y = HIWORD(lParam);

        }
        button_down[this_ctx] = FALSE;
        InvalidateRect(hwnd, NULL, TRUE);
        return 0;

    case WM_MOUSEMOVE:
        for(i = 0; i < MAXCTX; i++) {
            if(hwnd_ctx[i] == hwnd) {
                this_ctx = i;
                break;
            }
        }
        if(button_down[this_ctx]) {
            old_cmouse_x = cmouse_x;
            old_cmouse_y = cmouse_y;
            cmouse_x = LOWORD(lParam);
            cmouse_y = HIWORD(lParam);
            rect.bottom = cyClient[this_ctx] - cyBorder[this_ctx] + BORDERSTANDOFF;
            rect.top = cyBorder[this_ctx] - BORDERSTANDOFF;
            rect.left = cxBorder[this_ctx] - BORDERSTANDOFF;
            rect.right = cxClient[this_ctx] - cxBorder[this_ctx] + BORDERSTANDOFF;
            InvalidateRect(hwnd, &rect, FALSE);
        }
        return 0;

    case WM_CHAR:
        // If the user type ^D when in the window, close it
        // down.
        if(wParam == CTL_D)
            DestroyWindow(hwnd);
        return 0;
         
    case WM_DESTROY:
#ifndef WINFILTER
        PostQuitMessage (0) ;
#endif
        // Identify the context from the list of window handles
        for(i = 0; i < MAXCTX; i++) {
            if(hwnd_ctx[i] == hwnd) {
                this_ctx = i;
                break;
            }
        }
        ctx_active[this_ctx] = FALSE;
        return 0 ;
    }
    return DefWindowProc (hwnd, message, wParam, lParam) ;
}

// -------------------------------------------------------------------------
// ReadData                                                
//                                                         
// Read from 'filename' XY pairs of numbers, and place in  
// 'p'. Min/max values placed in argument pointers, and the
// number of points read is returned.                      
//                                                         
// -------------------------------------------------------------------------

int ReadData(CoordType *p, char *filename, 
            double *max_x, double *max_y, 
            double *min_x, double *min_y)
{
    FILE *fp;
    int idx = 0;

    if((fp = fopen(filename, "rb")) == NULL) 
        return 0;

    // Read in XY pairs until no more, or we've reach the limit. Calculate
    // the X and Y minimum and maximum values, as we go.
    while(idx < MAXNUMPOINTS && 
          fscanf(fp, "%lf %lf", &p[idx].x, &p[idx].y) == 2) {
        if(idx == 0) {
            *min_x = *max_x = p[idx].x;
            *min_y = *max_y = p[idx].y;
        } else {
            if(*min_x > p[idx].x)
                *min_x = p[idx].x;
            if(*min_y > p[idx].y)
                *min_y = p[idx].y;
            if(*max_x < p[idx].x)
                *max_x = p[idx].x;
            if(*max_y < p[idx].y)
                *max_y = p[idx].y;
        }
        idx++;
    }

    fclose(fp);

    return idx;
}
