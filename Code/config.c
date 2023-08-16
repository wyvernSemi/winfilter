//=============================================================
// 
// Copyright (c) 1999-2023 Simon Southwell. All rights reserved.
//
// Date: 11th March 1999
//
// Configuration function for filter program.
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
//=============================================================

// -------------------------------------------------------------------------
// INCLUDES
// -------------------------------------------------------------------------

#include <stdlib.h>
#include "filter.h"

// -------------------------------------------------------------------------
// PROTOTYPES
// -------------------------------------------------------------------------

void DisplayUsage(char **);

// -------------------------------------------------------------------------
// GLOBALS
// -------------------------------------------------------------------------

int ConfigErrorOccured;

// -------------------------------------------------------------------------
// SetConfiguration
// -------------------------------------------------------------------------

int SetConfiguration (int argc, char **argv, ConfigStruct *config)
{
    static char sbuf[1][80], *sbufptr[1];
    int n = 0;
    
    int getopt(int, char **, char *);

    /* Prototypes */
    static void DisplayBanner(const ConfigStruct *, const char *);
    static void GetEnvironment(ConfigStruct *, char **, char **argv);
    static void SetWindow(ConfigStruct *, char **, char **, char);
   
    /* Option character returned by getopt (see man getopt(3S)) */
    int option, window_specified=FALSE, winchar;
    char *wstr;
    extern char *optarg;
    extern int optind;
    KaiserParamStruct params;

    optind = 1;

    sbufptr[0] = (char *)&sbuf[0];

    /* Set the default values */
    config->opimpulse   = DEFAULT_opimpulse;
    config->opwindow    = DEFAULT_opwindow;
    config->inversion   = DEFAULT_inversion;
    config->reversal    = DEFAULT_reversal;
    config->bandpass    = DEFAULT_bandpass;
    config->bandstop    = DEFAULT_bandstop;
    config->decibels    = DEFAULT_decibels;
    config->magnitude   = DEFAULT_magnitude;
    config->phase       = DEFAULT_phase;
    config->ripple      = DEFAULT_ripple;
    config->Fd          = DEFAULT_Fd;
    config->a           = DEFAULT_a;
    config->N           = DEFAULT_N; 
    config->Q           = DEFAULT_Q; 
    config->Fc          = DEFAULT_Fc; 
    config->Fs          = DEFAULT_Fs; 
    config->Fw          = DEFAULT_Fw; 
    config->filename    = DEFAULT_filename;
    config->wfilename   = DEFAULT_winfilename;
    config->plotprog    = DEFAULT_plotprog;
    config->removeplot  = DEFAULT_removeplot;
    config->windowfunc  = DEFAULT_windowfunc;
    config->Xgraph      = DEFAULT_Xgraph;
    config->normalise   = DEFAULT_normalise;
    config->symimpulse  = DEFAULT_symimpulse;
    wstr                = DEFAULT_wstr;
    winchar             = DEFAULT_winchar;

    /* Override defaults with any environment variable settings */
    GetEnvironment(config, &wstr, argv);

    /* Send data to stdout by default (gets overridden if Xgraph specified) */ 
    config->fp = stdout;
    /* Send window coefficients to stderr by default (gets overridden if 
       Xgraph display specified) */ 
    config->wfp = stderr;

    /* Loop through all options specified */
    while((option = getopt(argc, argv, "R:D:f:dnmpSx:b:riIWuw:c:s:a:Q:N:XP:")) != EOF) {
       /* Set globals based on returned option and arguments where applicable */
       switch(option) {
           case 'P':
               config->Xgraph = TRUE;
               config->plotprog = optarg;
               break;
           case 'X':
               config->Xgraph = TRUE;
               break;
           case 'D':
               sscanf(optarg, "%lf", &config->Fd);
               break;
           case 'R':
               sscanf(optarg, "%lf", &config->ripple);
               if(config->ripple >= 0.0) {
                   sprintf(sbuf[0], "%s: Error! ripple specification must be less than 0\n", argv[0]);
                   DisplayMessage(1, (char **)&sbufptr);
                   ErrorAction(BADSTATUS);
               }
               config->ripple = -1.0 * config->ripple;
               break;
           case 'f':
               config->filename = optarg;
               if(config->fp != stdout) {
                   sprintf(sbuf[0], "%s: Error! Not allowed to specify more than 1 output filename\n", argv[0]);
                   DisplayMessage(1, (char **)&sbufptr);
                   ErrorAction(BADSTATUS);
               }
               if((config->fp = fopen(config->filename, "wb")) == NULL) {
                   sprintf(sbuf[0], "%s: Error! unable to open file %s for writing\n", argv[0], config->filename);
                   DisplayMessage(1, (char **)&sbufptr);
                   ErrorAction(BADSTATUS);
               }
               break;
           case 'd':
               config->decibels = TRUE;
               config->magnitude = FALSE;
               config->phase = FALSE;
               break;
           case 'm':
               config->magnitude = TRUE;
               config->decibels  = FALSE;
               config->phase = FALSE;
               break;
           case 'p':
               config->phase = TRUE;
               config->decibels  = FALSE;
               config->magnitude = FALSE;
               break;
           case 'r':
               config->reversal = TRUE;
               break;
           case 'i':
               config->inversion = TRUE;
               break;
           case 'I':
               config->opimpulse = TRUE;
               break;
           case 'n':
               config->normalise = FALSE;
               break;
           case 'S':
               config->symimpulse = TRUE;
               break;
           case 'W':
               config->opwindow = TRUE;
               break;
           case 'w':
               window_specified = TRUE;
               winchar = optarg[0];
               SetWindow(config, &wstr, argv, (char)winchar);
               break;
           case 's':
               sscanf(optarg, "%lf", &config->Fs);
               if(config->Fs < 0.0) {
                   sprintf(sbuf[0], "%s: Error! Sample frequency must be a positive value\n", argv[0]);
                   DisplayMessage(1, (char **)&sbufptr);
                   ErrorAction(BADSTATUS);
               }
               break;
           case 'b':
               sscanf(optarg, "%lf", &config->Fw);
               config->bandpass = TRUE;
               if(config->Fw < 0.0) {
                   sprintf(sbuf[0], "%s: Error! Band pass/stop width must be a positive value\n", argv[0]);
                   DisplayMessage(1, (char **)&sbufptr);
                   ErrorAction(BADSTATUS);
               }
               break;
           case 'x':
               sscanf(optarg, "%lf", &config->Fw);
               config->bandstop = TRUE;
               if(config->Fw < 0.0) {
                   sprintf(sbuf[0], "%s: Error! Band pass/stop width must be a positive value\n", argv[0]);
                   DisplayMessage(1, (char **)&sbufptr);
                   ErrorAction(BADSTATUS);
               }
               break;
           case 'c':
               sscanf(optarg, "%lf", &config->Fc);
               if(config->Fc < 0.0) {
                   sprintf(sbuf[0], "%s: Error! Cut off frequency must be a positive value\n", argv[0]);
                   DisplayMessage(1, (char **)&sbufptr);
                   ErrorAction(BADSTATUS);
               }
               break;
           case 'a':
               sscanf(optarg, "%lf", &config->a);
               if(config->a < 0.0) {
                   sprintf(sbuf[0], "%s: Error! a must be positive value\n", argv[0]);
                   DisplayMessage(1, (char **)&sbufptr);
                   ErrorAction(BADSTATUS);
               }
               break;
           case 'Q':
               config->Q = strtol(optarg, NULL, 0);
               if(config->Q < -1) {
                   sprintf(sbuf[0], "%s: Error! Q must be a positive value, or -1 for single precision\n", argv[0]);
                   DisplayMessage(1, (char **)&sbufptr);
                   ErrorAction(BADSTATUS);
               }
               if(config->Q > 64) {
                   sprintf(sbuf[0], "%s: Error! Q must be 64 bits or less\n", argv[0]);
                   DisplayMessage(1, (char **)&sbufptr);
                   ErrorAction(BADSTATUS);
               }
               break;
           case 'N':
               config->N = strtol(optarg, NULL, 0);
               if(config->N < 0) {
                   sprintf(sbuf[0], "%s: Error! N must be a positive value\n", argv[0]);
                   DisplayMessage(1, (char **)&sbufptr);
                   ErrorAction(BADSTATUS);
               }
               if(config->N > COEFFTOTAL) {
                   sprintf(sbuf[0], "%s: Error! N mustn't be greater than number of coefficients \n", argv[0]);
                   DisplayMessage(1, (char **)&sbufptr);
                   ErrorAction(BADSTATUS);
               }
               break;
           case 'u':
               DisplayUsage(argv);
               ErrorAction(GOODSTATUS);
               break;
           case '?':
           default:
               DisplayUsage(argv);
               ErrorAction(BADSTATUS);
               break;
         }
    }

    /* Lets do some checking of the requested configuration */
    if(config->Fc >= config->Fs/2) {
        sprintf(sbuf[0], "%s: Error! Cut off frequency must be less than half the sampling frequency\n", argv[0]);
        DisplayMessage(1, (char **)&sbufptr);
        ErrorAction(BADSTATUS);
    }
    if((config->bandpass || config->bandstop) && 
       (config->Fc + config->Fw) >= config->Fs/2) {
        sprintf(sbuf[0], "%s: Error! Band pass/stop width puts upper cut off > half sampling rate\n", argv[0]);
        DisplayMessage(1, (char **)&sbufptr);
        ErrorAction(BADSTATUS);
    }
    if((config->bandpass || config->bandstop) && config->inversion) {
        sprintf(sbuf[0], "%s: Error! Can't use spectral inversion with band pass/stop\n", argv[0]);
        DisplayMessage(1, (char **)&sbufptr);
        ErrorAction(BADSTATUS);
    }
    if(config->bandpass && config->bandstop) {
        sprintf(sbuf[0], "%s: Error! Can't specify both bandpass and band stop simultaneously\n", argv[0]);
        DisplayMessage(1, (char **)&sbufptr);
        ErrorAction(BADSTATUS);
    }
    if((config->decibels && (config->magnitude || config->phase)) ||
       (config->magnitude && config->phase)) {
        sprintf(sbuf[0], "%s: Error! Must specify only one of -d, -m or -p options\n", argv[0]);
        DisplayMessage(1, (char **)&sbufptr);
        ErrorAction(BADSTATUS);
    }

    /* Auto-design mode initiated with ripple specified */
    if(config->ripple != 0.0) {
        if(window_specified) {
            sprintf(sbuf[0], "%s: Error! Cannot specify window type with auto-design active\n", argv[0]);
            DisplayMessage(1, (char **)&sbufptr);
            ErrorAction(BADSTATUS);
        }

        /* Calculate tap length (N) and alpha parameter (a) for given spec. */
        params = design_kaiser_filter(config->Fd, config->Fs, config->ripple);
        if(params.N > COEFFTOTAL) {
            sprintf(sbuf[0], "%s: Error! Taps (%d) calculated  > number of coefficients (%d) \n", 
                            argv[0], params.N, COEFFTOTAL);
            DisplayMessage(1, (char **)&sbufptr);
            ErrorAction(BADSTATUS);
        }

        /* Copy calculated paramters to configuration */
        config->a = params.a;
        config->N = params.N;

        /* Auto-design mode is a Kaiser window */
        config->windowfunc = kaiser;
        wstr = "Kaiser";
    }

    /* Choose a default setting for 'a' parameter, if none already specified,
       dependant on the window to be used */
    if(config->a == UNSET) {
        switch(winchar) {
            case 'c':
            case 'g':
            case 'p':
            case 'y':
                config->a = DEFAULT_COMMON_ALPHA;
                break;
            case 'k':
                config->a = DEFAULT_KAISER_ALPHA;
                break;
            case 'T':
                config->a = DEFAULT_TUKEY_ALPHA;
                break;
            case 'C':
                config->a = DEFAULT_COSINE_ALPHA;
                break;
            case 'h':
                config->a = DEFAULT_HAMMING_ALPHA;
                break;
            default:
                config->a = NULL_ALPHA;
                break;
        }
    }

    /* If requesting an Xgraph plot, then open up a file for writing the data 
       if none has been previously specified */
    if(config->Xgraph && config->fp == stdout) {
        if((config->fp = fopen(config->filename, "wb")) == NULL) {
            sprintf(sbuf[0], "%s: Error! unable to open file %s for writing\n", 
                             argv[0], config->filename);
            DisplayMessage(1, (char **)&sbufptr);
            ErrorAction(BADSTATUS);
        }
        config->removeplot = TRUE;
    } 

    /* If requesting the window coefficients, then open a file to put them in */
    if(config->opwindow) {
        if((config->wfp = fopen(config->wfilename, "wb")) == NULL) {
            sprintf(sbuf[0], "%s: Error! unable to open file %s for writing\n",
                             argv[0], config->wfilename);
            DisplayMessage(1, (char **)&sbufptr);
            ErrorAction(BADSTATUS);
        }
    }

    //if (ConfigErrorOccured != TRUE)
    //    DisplayBanner(config, wstr);

    return(GOODSTATUS);

}

// -------------------------------------------------------------------------
// DisplayUsage
// -------------------------------------------------------------------------

void DisplayUsage(char **argv)
{
    static char sbuf[200][80], *sbufptr[1];
    int n = 0, i;

    sprintf(sbuf[n++], "\nUsage: %s [-unWirIXS] [-w <window>] [-a <num>]\n", argv[0]);
    sprintf(sbuf[n++], "              [-Q <num>] [-N <num>] [-d | -m | -p] [-c <num>]\n");
    sprintf(sbuf[n++], "              [-b <num> | -x <num>] [-s <num>] [-f <filename>]\n");
    sprintf(sbuf[n++], "              [-R <num> -D <num>]\n");
    sprintf(sbuf[n++], "\n        -a Window parameter\n");
    sprintf(sbuf[n++], "        -i Perform spectral inversion (default off)\n");
    sprintf(sbuf[n++], "        -r Perform spectral reversal (default off)\n");
    sprintf(sbuf[n++], "        -c Cut off frequency in Hertz (default 20000)\n");
    sprintf(sbuf[n++], "        -b Band pass width in Hertz (default low pass filter)\n");
    sprintf(sbuf[n++], "        -x Band stop width in Hertz (default low pass filter)\n");
    sprintf(sbuf[n++], "        -s Sampling frequency in Hertz (default 192000)\n");
    sprintf(sbuf[n++], "        -w Select window type:\n");
    sprintf(sbuf[n++], "                b        (Blackman*)\n");
    sprintf(sbuf[n++], "                B        (Blackman-Harris*)\n");
    sprintf(sbuf[n++], "                c        (Cauchy)\n");
    sprintf(sbuf[n++], "                C        (Cosine)\n");
    sprintf(sbuf[n++], "                g        (Gaussian)\n");
    sprintf(sbuf[n++], "                h        (Hamming -- default)\n");
    sprintf(sbuf[n++], "                k        (Kaiser)\n");
    sprintf(sbuf[n++], "                n        (Nuttall*)\n");
    sprintf(sbuf[n++], "                o        (Bohman*)\n");
    sprintf(sbuf[n++], "                p        (Poisson)\n");
    sprintf(sbuf[n++], "                r        (Reisz*)\n");
    sprintf(sbuf[n++], "                R        (Riemann*)\n");
    sprintf(sbuf[n++], "                t        (triangular or Bartlett*)\n");
    sprintf(sbuf[n++], "                T        (Tukey)\n");
    sprintf(sbuf[n++], "                u        (uniform or rectangular*)\n");
    sprintf(sbuf[n++], "                v        (von Hann or Hanning*)\n");
    sprintf(sbuf[n++], "                V        (Valle-Poisson*)\n");
    sprintf(sbuf[n++], "                y        (Chebyschev)\n");
    sprintf(sbuf[n++], "           Windows marked * are fixed and do not use the 'a' parameter\n");
    sprintf(sbuf[n++], "        -d Output values in dBs (not impulse---default off)\n");
    sprintf(sbuf[n++], "        -m Output values as magnitude (not impulse---default off)\n");
    sprintf(sbuf[n++], "        -p Output phase values (not impulse---default off)\n");
    sprintf(sbuf[n++], "        -n Output non-normalised frequency values (not impulse---default on)\n");
    sprintf(sbuf[n++], "        -S Generate symmetrical impulse response (default off)\n");
    sprintf(sbuf[n++], "        -f Specify an output filename\n");
    sprintf(sbuf[n++], "        -I Output impulse response values (default frequency)\n");
    sprintf(sbuf[n++], "        -W Output window values to %s (default off)\n", DEFAULT_winfilename);
    sprintf(sbuf[n++], "        -Q Quantisation (default \'Double Precision\')\n");
    sprintf(sbuf[n++], "        -N Number of taps (default 120)\n");
    sprintf(sbuf[n++], "        -R Auto-design mode maximum passband/stopband ripple in dBs\n");
    sprintf(sbuf[n++], "           (default non-automode)\n");
    sprintf(sbuf[n++], "        -D Auto-design mode maximum transition (delta) frequency step in Hz\n");
    sprintf(sbuf[n++], "           (default non-automode)\n");
    sprintf(sbuf[n++], "        -X Output to graphical display (default off) \n");
    sprintf(sbuf[n++], "        -u Print this message\n");
    sprintf(sbuf[n++], "\n");

    for(i=0; i < n; i++)
       sbufptr[i] = (char *)sbuf[i];
    
    DisplayMessage(n, (char **)&sbufptr);
}

// -------------------------------------------------------------------------
// DisplayBanner
// -------------------------------------------------------------------------

static void DisplayBanner(const ConfigStruct *C, const char *wstr)
{
    static char sbuf[50][80], *sbufptr[50];
    int n = 0, i;

    char *strptr;
    
    sprintf(sbuf[n++], "\n            Filter V1.0.1\n");
    sprintf(sbuf[n++], "             23rd July 2002\n");
    sprintf(sbuf[n++], "     An FIR filter design program\n");
    sprintf(sbuf[n++], "  Copyright (c) 2002 Simon Southwell\n");
    sprintf(sbuf[n++], "         All rights reserved\n");
    sprintf(sbuf[n++], "              ----o----\n\n");
    sprintf(sbuf[n++], "      %s Window\n", wstr);
    if(C->Q == 0)
        sprintf(sbuf[n++], "      Quantisation = Double Precision\n");
    else if(C->Q == -1)
        sprintf(sbuf[n++], "      Quantisation = Single Precision\n");
    else
        sprintf(sbuf[n++], "      Quantisation = %2d Bits\n", C->Q);
    sprintf(sbuf[n++], "      Taps = %4d Alpha = %.3lf\n", C->N, C->a);
    sprintf(sbuf[n++], "      Fc = %.0lf Hz Fs = %.0lf Hz\n", C->Fc, C->Fs);
    
    if(C->reversal && C->inversion)
        strptr = "Spectrally reversed and inverted";
    else if(C->reversal)
        strptr = "Spectrally reversed";
    else if(C->inversion)
        strptr = "Spectrally inverted";
    else
        strptr = "";

    if(C->bandpass)
        sprintf(sbuf[n++], "      Bandpass width = %.0lf Hz\n", C->Fw);
    else if(C->bandstop)
        sprintf(sbuf[n++], "      Bandstop width = %.0lf Hz\n", C->Fw);

    sprintf(sbuf[n++], "      %s\n\n", strptr);
        
    for(i=0; i < n; i++)
       sbufptr[i] = (char *)sbuf[i];
    
    DisplayMessage(n, (char **)&sbufptr);

}

// -------------------------------------------------------------------------
// GetEnvironment
// -------------------------------------------------------------------------

static void GetEnvironment(ConfigStruct *C, char **wstr, char **argv)
{
    static void SetWindow(ConfigStruct *, char **, char **, char);
    char *str;

    if((str = getenv("FLT_XPLOT")) != NULL)
        C->plotprog = str;

    if((str = getenv("FLT_FILENAME")) != NULL)
        C->filename = str;

    if((str = getenv("FLT_TAPS")) != NULL) 
        C->N = strtol(str, NULL, 0);

    if((str = getenv("FLT_QUANTISATION")) != NULL)
        C->Q = strtol(str, NULL, 0);

    if((str = getenv("FLT_FSAMPLE")) != NULL)
        sscanf(str, "%lf", &(C->Fs));

    if((str = getenv("FLT_FCUTOFF")) != NULL)
        sscanf(str, "%lf", &(C->Fc));

    if((str = getenv("FLT_ALPHA")) != NULL)
        sscanf(str, "%lf", &(C->a));

    if((str = getenv("FLT_WINDOW")) != NULL) {
        SetWindow(C, wstr, argv, str[0]);
    }
}

// -------------------------------------------------------------------------
//  SetWindow
// -------------------------------------------------------------------------

static void SetWindow(ConfigStruct *C, char **wstr, char **argv, char wchar)
{
    static char sbuf[1][80], *sbufptr[1];
    int n = 0;

    switch(wchar) {
        case 'o': 
            C->windowfunc = bohman;
            *wstr = "Bohman";
            break;
        case 'r':
            C->windowfunc = reisz;
            *wstr = "Reisz";
            break;
        case 'R':
            C->windowfunc = riemann;
            *wstr = "Riemann";
            break;
        case 'V':
            C->windowfunc = vallepoisson;
            *wstr = "Valle-Poisson";
            break;
        case 'T':
            C->windowfunc = tukey;
            *wstr = "Tukey";
            break;
        case 'p':
            C->windowfunc = poisson;
            *wstr = "Poisson";
            break;
        case 'c':
            C->windowfunc = cauchy;
            *wstr = "Cauchy";
            break;
        case 'C':
            C->windowfunc = cosine;
            *wstr = "Cosine";
            break;
        case 't':
            C->windowfunc = bartlett;
            *wstr = "Bartlett";
            break;
        case 'b':
            C->windowfunc = blackman;
            *wstr = "Blackman";
            break;
        case 'n':
            C->windowfunc = nuttall;
            *wstr = "Nuttall";
            break;
        case 'B':
            C->windowfunc = blackman_harris;
            *wstr = "Blackman-Harris";
            break;
        case 'k':
            C->windowfunc = kaiser;
            *wstr = "Kaiser";
            break;
        case 'g':
            C->windowfunc = gauss;
            *wstr = "Gaussian";
            break;
        case 'v':
            C->windowfunc = hamming;
            C->a = VONHANN_ALPHA;
            *wstr = "von Hann";
            break;
        case 'u':
            C->windowfunc = hamming;
            C->a = UNIFORM_ALPHA;
            *wstr = "Uniform";
            break;
        case 'h':
            C->windowfunc = hamming;
            *wstr = "Hamming";
            break;
        case 'y':
            C->windowfunc = chebyshev;
            *wstr = "Chebyshev";
            break;
        default: 
            sprintf(sbuf[0], "%s: Error! Unrecognised window type\n", argv[0]);
            sbufptr[0] = (char *)&sbuf[0];
            ErrorAction(BADSTATUS);
            break;
    }
}
