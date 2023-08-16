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
#include <stdlib.h>
#include <stdio.h>
#include <direct.h>

#include "resource.h"
#include "filter.h"
#include "WinFilter.h"
#include "Graph.h"
#include "window.h"
#include "fft.h"

// -------------------------------------------------------------------------
// PROTOTYPES
// -------------------------------------------------------------------------

extern int AnalyseFilter (int, char **);
extern LRESULT CALLBACK GraphCallback (HWND, UINT, WPARAM, LPARAM);
extern glgraph_main(int, char**);

// -------------------------------------------------------------------------
// STATICS
// -------------------------------------------------------------------------

// Only used in this file
static HWND  hdlg ;
static HINSTANCE ProgramInstance;
static TCHAR szAppName[] = TEXT ("WINFILTER") ;
static TCHAR szGraphName[] = TEXT("GRAPH") ;
static TCHAR szFileName[MAX_PATH], szTitleName[MAX_PATH] ;

static char wdir[MAXFILENAMELEN];

// -------------------------------------------------------------------------
// WinMain()
//
// WinMain Top Level Routine
//
// -------------------------------------------------------------------------

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
    HWND     hwnd ;
    MSG      msg ;
    WNDCLASS wndclass ;


    ProgramInstance = hInstance;

    // Remember the working directory, as we don't won't 
    // to move, but the Open and Save dialogs will change
    // the directory
    _getcwd(wdir, MAXFILENAMELEN);
     
    // Define the class of window we want to use
    wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
    wndclass.lpfnWndProc   = WndProc ;
    wndclass.cbClsExtra    = 0 ;
    wndclass.cbWndExtra    = 0 ;
    wndclass.hInstance     = hInstance ;
    wndclass.hIcon         = LoadIcon (hInstance, (LPCTSTR)IDI_WINFILTER) ;
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
    wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
    wndclass.lpszMenuName  = szAppName ; 
    wndclass.lpszClassName = szAppName ;
     
    // Register the class with windows, and set the whole
    // thing off
    RegisterClass (&wndclass);

    wndclass.lpfnWndProc   = GraphCallback ;
    wndclass.lpszMenuName  = NULL ;
    wndclass.lpszClassName = szGraphName ;

    // Register the graph windw class
    RegisterClass (&wndclass);

    // Create the main window
    hwnd = CreateWindow (szAppName, TEXT("WinFilter"),
                         WS_OVERLAPPED  | WS_CAPTION | WS_SYSMENU | 
                         WS_MINIMIZEBOX |WS_MAXIMIZEBOX | WS_CLIPCHILDREN,
                         CW_USEDEFAULT, CW_USEDEFAULT, X_SIZE, Y_SIZE, 
                         NULL, NULL, hInstance, NULL) ;
     
    ShowWindow (hwnd, iCmdShow) ;
    UpdateWindow (hwnd) ;
     
    while (GetMessage (&msg, NULL, 0, 0)) {
        if (hdlg == 0 || !IsDialogMessage (hdlg, &msg)) {
               TranslateMessage (&msg) ;
               DispatchMessage (&msg) ;
        }
    }
    return msg.wParam ;
}
        
// -------------------------------------------------------------------------
// WndProc()
//
// Top level window callback function
//
// -------------------------------------------------------------------------

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HINSTANCE hInstance;
    char str[MAXSTRSIZE*MAXMESSAGELINES];

    HDC              hdc ;
    PAINTSTRUCT      ps ;
    DWORD Error;

    switch (message) {
    // First massage the window will receive
    case WM_CREATE:
        // Create an instance, and construct a dialog (a form)
        // managed by DlgProc callback
        hInstance = ((LPCREATESTRUCT) lParam)->hInstance;
        hdlg = CreateDialog (hInstance, TEXT("WINFILTER_DLG"), hwnd, DlgProc);
        Error = GetLastError();
        SetCursor(LoadCursor(NULL, IDC_ARROW));
        return 0 ;

    // Handle focus event messages
    case WM_SETFOCUS:
        SetFocus (hdlg) ;
        return 0 ;

    // Place processing of menu commands here
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_FILE_EXIT:
            SendMessage(hwnd, WM_CLOSE, 0, 0);
            return TRUE;
        case ID_FILE_OUTPUTFILE:
            return TRUE;
        case ID_HELP_ABOUT:
            sprintf(str, "  WinFilter V1.0.2 - 16th August 2023\n"
                         "Copyright (c) 2002-2023 Simon Southwell\n\n"
                         "e-mail: simon@anita-simulators.org.uk\n");
            MessageBox(NULL, str, TEXT("About"), MB_ICONASTERISK);
            return TRUE;
        }
        return 0 ;

   // Window needs repainting. No special manipulation needed.
   case WM_PAINT:
        hdc = BeginPaint (hwnd, &ps) ;
        EndPaint (hwnd, &ps) ;
        return 0 ;
          
   // Window needs killing
   case WM_DESTROY:
        PostQuitMessage (0) ;
        return 0 ;
   }

   // Return unprocessed messages back to windows
   return DefWindowProc (hwnd, message, wParam, lParam) ;
}

// -------------------------------------------------------------------------
// DlgProc()
//
// Call back for Main window dialog box
//
// -------------------------------------------------------------------------

BOOL CALLBACK DlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{

    static ConfigStruct Config, *C=&Config;
    static HWND hwndStatus, hwtmp;
    static BOOL Connected = FALSE;
    static unsigned char fname[MAXSTRSIZE], wfname[MAXSTRSIZE], valstr[MAXSTRSIZE];
    int i;
    real_t tmp;
//    GraphConf gphc_r, gphc_w;
    KaiserParamStruct k;
    char cmd[10][80], *args[10];
    char buf[DEFAULT_STR_SIZE], buf2[DEFAULT_STR_SIZE], *str=buf, *str2=buf2;

    switch(message) {
    // Set some default displayed text and button checks
    case WM_INITDIALOG :
        // Set the default configuration
        C=&Config;
        SetDefaults(C);

        CheckRadioButton(hDlg, IDC_BHARRIS, IDC_VALLEPOISSON, IDC_HAMMING);
        CheckRadioButton(hDlg, IDC_LOWHIPASS, IDC_BANDSTOP, IDC_LOWHIPASS);
        CheckRadioButton(hDlg, IDC_IMPULSE, IDC_PHASE, IDC_FREQDB);
        CheckDlgButton(hDlg, IDC_INVERSION, C->inversion);
        CheckDlgButton(hDlg, IDC_REVERSAL, C->reversal);
        CheckDlgButton(hDlg, IDC_AUTOMODE, C->automode);
        EnableWindow(GetDlgItem(hDlg, IDC_ALPHA), !C->automode);
        EnableWindow(GetDlgItem(hDlg, IDC_TAPS), !C->automode);
        EnableWindow(GetDlgItem(hDlg, IDC_FDELTA), C->automode);
        EnableWindow(GetDlgItem(hDlg, IDC_ATTEN), C->automode);
        SetDlgItemText(hDlg, IDC_OPFILENAME, C->filename);
        SetDlgItemText(hDlg, IDC_WINFILENAME, C->wfilename);
        EnableWindow(GetDlgItem(hDlg, IDC_WINFILENAME), FALSE);
        CheckDlgButton(hDlg, IDC_GRAPHOP, C->Xgraph);
        CheckDlgButton(hDlg, IDC_OPWINDOW, C->opwindow);
        CheckDlgButton(hDlg, IDC_SYMIMP, C->symimpulse);
        sprintf(valstr, "%.2lf", C->Fc + C->Fw);
        SetDlgItemText(hDlg, IDC_FC2, valstr);
        sprintf(valstr, "%.2lf", C->Fc);
        SetDlgItemText(hDlg, IDC_FC1, valstr);
        EnableWindow(GetDlgItem(hDlg, IDC_FC2), C->bandstop || C->bandpass);
        sprintf(valstr, "%.2lf", C->Fs);
        SetDlgItemText(hDlg, IDC_FS, valstr);
        SetDlgItemInt(hDlg, IDC_Q, C->Q, FALSE);
        SetDlgItemInt(hDlg, IDC_TAPS, C->N, FALSE);
        sprintf(valstr, "%.4lf", C->a);
        SetDlgItemText(hDlg, IDC_ALPHA, valstr);
        sprintf(valstr, "%.1lf", C->attenuation);
        SetDlgItemText(hDlg, IDC_ATTEN, valstr);
        sprintf(valstr, "%.2lf", C->Ft);
        SetDlgItemText(hDlg, IDC_FDELTA, valstr);

        return TRUE ;

    // Process my messages sent from parent (none at the moment)
    case WM_USER :
        return TRUE;

    // Process commands to this dialog
    case WM_COMMAND :
        switch(LOWORD(wParam)) {

        // The 'Execute' button has been pushed
        case IDC_EXECUTE:
            // Create the data files
            ExecuteAnalysis(C);
            if(C->automode) {
                k = design_kaiser_filter(C->Ft, C->Fs, -1.0 * C->attenuation);
                C->a = k.a;
                C->N = k.N;
                sprintf(valstr, "%.4lf", k.a);
                SetDlgItemText(hDlg, IDC_ALPHA, valstr);
                SetDlgItemInt(hDlg, IDC_TAPS, k.N, FALSE);
            }

            if(C->Xgraph || C->opwindow) {
#ifdef WIN32
                strcpy(cmd[0], "WinFilter");
                /* If the WindowBuf values were stored, then plot them */
                if(C->opwindow)
                    strcpy(cmd[1], C->wfilename);
                else 
                    strcpy(cmd[1], C->filename);
                
                args[0] = cmd[0];
                args[1] = cmd[1];
                glgraph_main(2, args);
#else

#if defined(_MSDOS)
               sprintf(str, "sh -c \"%s &;%s %s&\"", str2, C->plotprog, C->filename);
               system(str);
#else
               sprintf(str, "%s %s&", C->plotprog, C->filename);
               system(str);
               system(str2);
#endif
#endif
            }
            return TRUE;

        // Design Parameters
        case IDC_Q:
            C->Q = GetDlgItemInt(hDlg, IDC_Q, NULL, FALSE);
            return TRUE;
        case IDC_TAPS:
            C->N = GetDlgItemInt(hDlg, IDC_TAPS, NULL, FALSE);
            return TRUE;
        case IDC_ALPHA:
            GetDlgItemText(hDlg, IDC_ALPHA, valstr, MAXSTRSIZE);
            sscanf(valstr, "%lf", &(C->a));
            return TRUE;
        case IDC_ATTEN:
            GetDlgItemText(hDlg, IDC_ATTEN, valstr, MAXSTRSIZE);
            sscanf(valstr, "%lf", &(C->attenuation));
            return TRUE;
        case IDC_FDELTA:
            GetDlgItemText(hDlg, IDC_FDELTA, valstr, MAXSTRSIZE);
            sscanf(valstr, "%lf", &(C->Ft));
            return TRUE;

        // Frequencies
        case IDC_FC1:
            GetDlgItemText(hDlg, IDC_FC1, valstr, MAXSTRSIZE);
            sscanf(valstr, "%lf", &(C->Fc));

            GetDlgItemText(hDlg, IDC_FC2, valstr, MAXSTRSIZE);
            sscanf(valstr, "%lf", &tmp);
            C->Fw = tmp - C->Fc;
            return TRUE;

        case IDC_FC2:
            GetDlgItemText(hDlg, IDC_FC2, valstr, MAXSTRSIZE);
            sscanf(valstr, "%lf", &tmp);
            C->Fw = tmp - C->Fc;
            return TRUE;

        case IDC_FS:
            GetDlgItemText(hDlg, IDC_FS, valstr, MAXSTRSIZE);
            sscanf(valstr, "%lf", &(C->Fs));
            return TRUE;

        // Output graphics
        case IDC_OPWINDOW:
            C->opwindow   = !C->opwindow;
            C->Xgraph     = C->opwindow ? FALSE : C->Xgraph;
            C->symimpulse = C->opwindow ? FALSE : C->symimpulse;
            CheckDlgButton(hDlg, IDC_OPWINDOW, C->opwindow);
            CheckDlgButton(hDlg, IDC_GRAPHOP, C->Xgraph);
            CheckDlgButton(hDlg, IDC_SYMIMP, C->symimpulse);
            return TRUE;

        case IDC_SYMIMP:
            C->symimpulse = !C->symimpulse;
            C->Xgraph     = C->symimpulse ? FALSE : C->Xgraph;
            C->opwindow   = C->symimpulse ? FALSE : C->opwindow;
            C->opimpulse  = TRUE;
            C->decibels   = FALSE;
            C->magnitude  = FALSE;
            C->phase      = FALSE;
            CheckDlgButton(hDlg, IDC_SYMIMP, C->symimpulse);
            CheckRadioButton(hDlg, IDC_IMPULSE, IDC_PHASE, IDC_IMPULSE);
            CheckDlgButton(hDlg, IDC_OPWINDOW, C->opwindow);
            CheckDlgButton(hDlg, IDC_GRAPHOP, C->Xgraph);
            return TRUE;

        case IDC_GRAPHOP:
            C->Xgraph     = !C->Xgraph;
            C->opwindow   = C->Xgraph   ? FALSE : C->opwindow;
            C->symimpulse = C->Xgraph ? FALSE : C->symimpulse;
            CheckDlgButton(hDlg, IDC_GRAPHOP, C->Xgraph);
            CheckDlgButton(hDlg, IDC_OPWINDOW, C->opwindow);
            CheckDlgButton(hDlg, IDC_SYMIMP, C->symimpulse);
            return TRUE;

        // Automode check button
        case IDC_AUTOMODE:
            C->automode = !C->automode;
            CheckDlgButton(hDlg, IDC_AUTOMODE, C->automode);
            // Automode uses Kaiser window
            CheckRadioButton(hDlg, IDC_BHARRIS, IDC_VALLEPOISSON, IDC_KAISER);
            C->window = 'k';

            // In automode the F delta and attenuation values are
            // used. Otherwise the Alpha value and number of taps
            // are used.
            EnableWindow(GetDlgItem(hDlg, IDC_ALPHA), !C->automode);
            EnableWindow(GetDlgItem(hDlg, IDC_TAPS), !C->automode);
            EnableWindow(GetDlgItem(hDlg, IDC_FDELTA), C->automode);
            EnableWindow(GetDlgItem(hDlg, IDC_ATTEN), C->automode);
            for(i = IDC_BHARRIS; i <= IDC_VALLEPOISSON; i++)
                EnableWindow(GetDlgItem(hDlg, i), !C->automode);

            return TRUE;

        // Output filename
        case IDC_WINFILENAME:
            GetDlgItemText(hDlg, IDC_WINFILENAME, wfname, MAXSTRSIZE);
            C->wfilename = wfname;
            return TRUE;

        // Output filename
        case IDC_OPFILENAME:
            GetDlgItemText(hDlg, IDC_OPFILENAME, fname, MAXSTRSIZE);
            C->filename = fname;
            return TRUE;

        // Spectral check boxes
        case IDC_INVERSION:
            C->inversion = !C->inversion;
            CheckDlgButton(hDlg, IDC_INVERSION, C->inversion);
            return TRUE;
        case IDC_REVERSAL:
            C->reversal = !C->reversal;
            CheckDlgButton(hDlg, IDC_REVERSAL, C->reversal);
            return TRUE;

        // Output type radio buttons
        case IDC_IMPULSE:
            C->decibels  = FALSE;
            C->magnitude = FALSE;
            C->phase     = FALSE;
            C->opimpulse = TRUE;
            return TRUE;
        case IDC_FREQDB:
            C->decibels  = TRUE;
            C->magnitude = FALSE;
            C->phase     = FALSE;
            C->opimpulse = FALSE;
            return TRUE;
        case IDC_FREQMAG:
            C->decibels  = FALSE;
            C->magnitude = TRUE;
            C->phase     = FALSE;
            C->opimpulse = FALSE;
            return TRUE;
        case IDC_PHASE:
            C->decibels  = FALSE;
            C->magnitude = FALSE;
            C->phase     = TRUE;
            C->opimpulse = FALSE;
            return TRUE;

        // Filter type
        case IDC_LOWHIPASS:
            C->bandstop = FALSE;
            C->bandpass = FALSE;
            CheckRadioButton(hDlg, IDC_LOWHIPASS, IDC_BANDSTOP, wParam);
            EnableWindow(GetDlgItem(hDlg, IDC_INVERSION), TRUE);
            EnableWindow(GetDlgItem(hDlg, IDC_FC2), FALSE);
            return TRUE;
        case IDC_BANDPASS:
            C->bandstop = FALSE;
            C->bandpass = TRUE;
            C->inversion = FALSE;
            CheckRadioButton(hDlg, IDC_LOWHIPASS, IDC_BANDSTOP, wParam);
            CheckDlgButton(hDlg, IDC_INVERSION, C->inversion);
            EnableWindow(GetDlgItem(hDlg, IDC_INVERSION), FALSE);
            EnableWindow(GetDlgItem(hDlg, IDC_FC2), TRUE);
            return TRUE;
        case IDC_BANDSTOP:
            C->bandstop = TRUE;
            C->bandpass = FALSE;
            C->inversion = FALSE;
            CheckRadioButton(hDlg, IDC_LOWHIPASS, IDC_BANDSTOP, wParam);
            CheckDlgButton(hDlg, IDC_INVERSION, C->inversion);
            EnableWindow(GetDlgItem(hDlg, IDC_INVERSION), FALSE);
            EnableWindow(GetDlgItem(hDlg, IDC_FC2), TRUE);
            return TRUE;

        // Windows
        case IDC_BHARRIS:
            C->window = 'B';
            C->a = NULL_ALPHA;
            CheckRadioButton(hDlg, IDC_BHARRIS, IDC_VALLEPOISSON, wParam);
            EnableWindow(GetDlgItem(hDlg, IDC_ALPHA), FALSE);
            return TRUE;
        case IDC_BLACKMAN:
            C->window = 'b';
            C->a = NULL_ALPHA;
            CheckRadioButton(hDlg, IDC_BHARRIS, IDC_VALLEPOISSON, wParam);
            EnableWindow(GetDlgItem(hDlg, IDC_ALPHA), FALSE);
            return TRUE;
        case IDC_NUTTALL:
            C->window = 'n';
            C->a = NULL_ALPHA;
            CheckRadioButton(hDlg, IDC_BHARRIS, IDC_VALLEPOISSON, wParam);
            EnableWindow(GetDlgItem(hDlg, IDC_ALPHA), FALSE);
            return TRUE;
        case IDC_CHEBYSHEV:
            C->window = 'y';
            C->a = DEFAULT_COMMON_ALPHA;
            CheckRadioButton(hDlg, IDC_BHARRIS, IDC_VALLEPOISSON, wParam);
            sprintf(valstr, "%.4lf", C->a);
            SetDlgItemText(hDlg, IDC_ALPHA, valstr);
            EnableWindow(GetDlgItem(hDlg, IDC_ALPHA), TRUE);
            return TRUE;
        case IDC_CAUCHY:
            C->window = 'c';
            C->a = DEFAULT_COMMON_ALPHA;
            CheckRadioButton(hDlg, IDC_BHARRIS, IDC_VALLEPOISSON, wParam);
            sprintf(valstr, "%.4lf", C->a);
            SetDlgItemText(hDlg, IDC_ALPHA, valstr);
            EnableWindow(GetDlgItem(hDlg, IDC_ALPHA), TRUE);
            return TRUE;
        case IDC_COSINE:
            C->window = 'C';
            C->a = DEFAULT_COSINE_ALPHA;
            CheckRadioButton(hDlg, IDC_BHARRIS, IDC_VALLEPOISSON, wParam);
            sprintf(valstr, "%.4lf", C->a);
            SetDlgItemText(hDlg, IDC_ALPHA, valstr);
            EnableWindow(GetDlgItem(hDlg, IDC_ALPHA), TRUE);
            return TRUE;
        case IDC_GAUSSIAN:
            C->window = 'g';
            C->a = DEFAULT_COMMON_ALPHA;
            CheckRadioButton(hDlg, IDC_BHARRIS, IDC_VALLEPOISSON, wParam);
            sprintf(valstr, "%.4lf", C->a);
            SetDlgItemText(hDlg, IDC_ALPHA, valstr);
            EnableWindow(GetDlgItem(hDlg, IDC_ALPHA), TRUE);
            return TRUE;
        case IDC_HAMMING:
            C->window = 'h';
            C->a = DEFAULT_HAMMING_ALPHA;
            CheckRadioButton(hDlg, IDC_BHARRIS, IDC_VALLEPOISSON, wParam);
            sprintf(valstr, "%.4lf", C->a);
            SetDlgItemText(hDlg, IDC_ALPHA, valstr);
            EnableWindow(GetDlgItem(hDlg, IDC_ALPHA), TRUE);
            return TRUE;
        case IDC_KAISER:
            C->window = 'k';
            C->a = DEFAULT_KAISER_ALPHA;
            CheckRadioButton(hDlg, IDC_BHARRIS, IDC_VALLEPOISSON, wParam);
            sprintf(valstr, "%.4lf", C->a);
            SetDlgItemText(hDlg, IDC_ALPHA, valstr);
            EnableWindow(GetDlgItem(hDlg, IDC_ALPHA), TRUE);
            return TRUE;
        case IDC_BOHMAN:
            C->window = 'o';
            C->a = NULL_ALPHA;
            CheckRadioButton(hDlg, IDC_BHARRIS, IDC_VALLEPOISSON, wParam);
            EnableWindow(GetDlgItem(hDlg, IDC_ALPHA), FALSE);
            return TRUE;
        case IDC_POSSION:
            C->window = 'p';
            C->a = DEFAULT_COMMON_ALPHA;
            CheckRadioButton(hDlg, IDC_BHARRIS, IDC_VALLEPOISSON, wParam);
            sprintf(valstr, "%.4lf", C->a);
            SetDlgItemText(hDlg, IDC_ALPHA, valstr);
            EnableWindow(GetDlgItem(hDlg, IDC_ALPHA), TRUE);
            return TRUE;
        case IDC_REISZ:
            C->window = 'r';
            C->a = NULL_ALPHA;
            CheckRadioButton(hDlg, IDC_BHARRIS, IDC_VALLEPOISSON, wParam);
            EnableWindow(GetDlgItem(hDlg, IDC_ALPHA), FALSE);
            return TRUE;
        case IDC_RIEMANN:
            C->window = 'R';
            C->a = NULL_ALPHA;
            CheckRadioButton(hDlg, IDC_BHARRIS, IDC_VALLEPOISSON, wParam);
            EnableWindow(GetDlgItem(hDlg, IDC_ALPHA), FALSE);
            return TRUE;
        case IDC_BARTLETT:
            C->window = 't';
            C->a = NULL_ALPHA;
            CheckRadioButton(hDlg, IDC_BHARRIS, IDC_VALLEPOISSON, wParam);
            EnableWindow(GetDlgItem(hDlg, IDC_ALPHA), FALSE);
            return TRUE;
        case IDC_TUKEY:
            C->window = 'T';
            C->a = DEFAULT_TUKEY_ALPHA;
            CheckRadioButton(hDlg, IDC_BHARRIS, IDC_VALLEPOISSON, wParam);
            sprintf(valstr, "%.4lf", C->a);
            SetDlgItemText(hDlg, IDC_ALPHA, valstr);
            EnableWindow(GetDlgItem(hDlg, IDC_ALPHA), TRUE);
            return TRUE;
        case IDC_UNIFORM:
            C->window = 'u';
            C->a = NULL_ALPHA;
            CheckRadioButton(hDlg, IDC_BHARRIS, IDC_VALLEPOISSON, wParam);
            EnableWindow(GetDlgItem(hDlg, IDC_ALPHA), FALSE);
            return TRUE;
        case IDC_VONHANN:
            C->window = 'v';
            C->a = NULL_ALPHA;
            CheckRadioButton(hDlg, IDC_BHARRIS, IDC_VALLEPOISSON, wParam);
            EnableWindow(GetDlgItem(hDlg, IDC_ALPHA), FALSE);
            return TRUE;
        case IDC_VALLEPOISSON:
            C->window = 'V';
            C->a = NULL_ALPHA;
            CheckRadioButton(hDlg, IDC_BHARRIS, IDC_VALLEPOISSON, wParam);
            EnableWindow(GetDlgItem(hDlg, IDC_ALPHA), FALSE);
            return TRUE;

        }
        return 0;
        break;
    
    case WM_DESTROY:

        return TRUE;
    }
    return FALSE ;
}


// -------------------------------------------------------------------------
// Set the main configuration structure default values
// -------------------------------------------------------------------------

void SetDefaults(ConfigStruct *config)
{
    // Set the default values 
    config->opimpulse   = DEFAULT_opimpulse;
    config->opwindow    = DEFAULT_opwindow;
    config->inversion   = DEFAULT_inversion;
    config->reversal    = DEFAULT_reversal;
    config->bandpass    = DEFAULT_bandpass;
    config->bandstop    = DEFAULT_bandstop;
    config->decibels    = DEFAULT_decibels;
    config->magnitude   = DEFAULT_magnitude;
    config->phase       = DEFAULT_phase;
    config->automode    = DEFAULT_automode;
    config->normalise   = DEFAULT_normalise;
    config->symimpulse  = DEFAULT_symimpulse;
    config->Xgraph      = DEFAULT_Xgraph;
    config->window      = DEFAULT_winchar;
    config->ripple      = DEFAULT_ripple;
    config->filename    = DEFAULT_filename;
    config->wfilename   = DEFAULT_winfilename;
    config->Fd          = DEFAULT_Fd;
    config->a           = DEFAULT_a;
    config->N           = DEFAULT_N; 
    config->Q           = DEFAULT_Q; 
    config->Fc          = DEFAULT_Fc; 
    config->Fs          = DEFAULT_Fs; 
    config->Fw          = DEFAULT_Fw; 
    config->Ft          = DEFAULT_Ft;
    config->attenuation = DEFAULT_attenuation;
}


void ExecuteAnalysis(ConfigStruct *C)
{
    char cmdstr[MAXCMDARGS][MAXSTRSIZE], *argvec[MAXSTRSIZE];
    char flagset = FALSE;
    int argcount=0, i;

    /* Point to all the strings in the command argument buffers */
    for(i=0; i<MAXSTRSIZE; i++)
       argvec[i] = cmdstr[i];

    strcpy(cmdstr[argcount++], "filter");

    /* Do all the flags first */
    if(C->inversion) {
        FLAGCHK;
        strcat(cmdstr[argcount], "i");
    }
    if(C->reversal) {
        FLAGCHK;
        strcat(cmdstr[argcount], "r");
    }
    if(C->decibels) {
        FLAGCHK;
        strcat(cmdstr[argcount], "d");
    }
    if(C->magnitude) {
        FLAGCHK;
        strcat(cmdstr[argcount], "m");
    }
    if(C->phase) {
        FLAGCHK;
        strcat(cmdstr[argcount], "p");
    }
    if(C->opimpulse) {
        FLAGCHK;
        strcat(cmdstr[argcount], "I");
    }
    if(C->opwindow) {
        FLAGCHK;
        strcat(cmdstr[argcount], "W");
    }
    if(C->Xgraph) {
        FLAGCHK;
        strcat(cmdstr[argcount], "X");
    }
    if(C->symimpulse) {
        FLAGCHK;
        strcat(cmdstr[argcount], "S");
    }

    if(flagset)
        argcount++;

    if(!C->automode) {
        strcpy(cmdstr[argcount++], "-w");
        sprintf(cmdstr[argcount++], "%c", C->window);
    }

    /* The decimal values */
    strcpy(cmdstr[argcount++], "-N");
    sprintf(cmdstr[argcount++], "%d", C->N);

    strcpy(cmdstr[argcount++], "-Q");
    sprintf(cmdstr[argcount++], "%d", C->Q);

    /* The floating values */
    if((C->window != 'v') && (C->window != 'u')) {
        strcpy(cmdstr[argcount++], "-a");
        sprintf(cmdstr[argcount++], "%.3lf", C->a);
    }

    strcpy(cmdstr[argcount++], "-c");
    sprintf(cmdstr[argcount++], "%.3lf", C->Fc);

    strcpy(cmdstr[argcount++], "-s");
    sprintf(cmdstr[argcount++], "%.3lf", C->Fs);

    if(C->bandpass) {
        strcpy(cmdstr[argcount++], "-b");
        sprintf(cmdstr[argcount++], "%.3lf", C->Fw);
    }

    if(C->bandstop) {
        strcpy(cmdstr[argcount++], "-x");
        sprintf(cmdstr[argcount++], "%.3lf", C->Fw);
    }

    if(C->automode) {
        strcpy(cmdstr[argcount++], "-R");
        sprintf(cmdstr[argcount++], "%.3lf", C->attenuation);
        strcpy(cmdstr[argcount++], "-D");
        sprintf(cmdstr[argcount++], "%.3lf", C->Ft);
    }

    strcpy(cmdstr[argcount++], "-f");
    sprintf(cmdstr[argcount++], "%s", C->filename);

    argvec[argcount] = NULL;

#if defined(_DEBUG)
    fprintf(stdout, "argcount = %d\n", argcount);
    for(i=0; i<argcount; i++) {
        fprintf(stdout, "%s\n", argvec[i]);
    }
#endif
    AnalyseFilter(argcount, (char **)&argvec);

}

// -------------------------------------------------------------------------
// DisplayMessage()
// -------------------------------------------------------------------------

void DisplayMessage(const int n, char **str)
{
    char msgbuf[200*80];
    int i;

    // return;

    msgbuf[0] = '\0';
    for(i = 0; i < n; i++)
       strcat(msgbuf, str[i]);

    MessageBox (NULL, msgbuf, TEXT ("Message"), MB_ICONEXCLAMATION) ;
}
