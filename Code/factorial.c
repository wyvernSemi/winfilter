//=============================================================
// 
// Copyright (c) 1999-2023 Simon Southwell. All rights reserved.
//
// Date: 11th March 1999
//
// Returns factorial for inputs of 0 to 69 to 21 signifcant
// figures. Returns 0.0 if input is out of this range.
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

double factorial(const int x)
{
    const double fact[70] = {
        1.00000000000000000000e+00,
        1.00000000000000000000e+00,
        2.00000000000000000000e+00,
        6.00000000000000000000e+00,
        2.40000000000000000000e+01,
        1.20000000000000000000e+02,
        7.20000000000000000000e+02,
        5.04000000000000000000e+03,
        4.03200000000000000000e+04,
        3.62880000000000000000e+05,
        3.62880000000000000000e+06,
        3.99168000000000000000e+07,
        4.79001600000000000000e+08,
        6.22702080000000000000e+09,
        8.71782912000000000000e+10,
        1.30767436800000000000e+12,
        2.09227898880000000000e+13,
        3.55687428096000000000e+14,
        6.40237370572800000000e+15,
        1.21645100408832000000e+17,
        2.43290200817664000000e+18,
        5.10909421717094400000e+19,
        1.12400072777760768000e+21,
        2.58520167388849782129e+22,
        6.20448401733239410000e+23,
        1.55112100433309860553e+25,
        4.03291461126605650323e+26,
        1.08888694504183519402e+28,
        3.04888344611713836735e+29,
        8.84176199373970077272e+30,
        2.65252859812191032189e+32,
        8.22283865417792243020e+33,
        2.63130836933693517766e+35,
        8.68331761881188593872e+36,
        2.95232799039604119555e+38,
        1.03331479663861442222e+40,
        3.71993326789901177492e+41,
        1.37637530912263431030e+43,
        5.23022617466601037914e+44,
        2.03978820811974415878e+46,
        8.15915283247897683796e+47,
        3.34525266131638027640e+49,
        1.40500611775287978878e+51,
        6.04152630633738340744e+52,
        2.65827157478844852913e+54,
        1.19622220865480188575e+56,
        5.50262215981208845667e+57,
        2.58623241511168177673e+59,
        1.24139155925360725283e+61,
        6.08281864034267522489e+62,
        3.04140932017133755764e+64,
        1.55111875328738218947e+66,
        8.06581751709438768456e+67,
        4.27488328406002548479e+69,
        2.30843697339241379244e+71,
        1.26964033536582764469e+73,
        7.10998587804863481025e+74,
        4.05269195048772205276e+76,
        2.35056133128287890630e+78,
        1.38683118545689864933e+80,
        8.32098711274139158006e+81,
        5.07580213877224835834e+83,
        3.14699732603879393903e+85,
        1.98260831540444008497e+87,
        1.26886932185884165438e+89,
        8.24765059208247151674e+90,
        5.44344939077443069445e+92,
        3.64711109181886832212e+94,
        2.48003554243683054797e+96,
        1.71122452428141297376e+98};

    /* If input in range, return factorial from table. Otherwise
       return 0.0 */
    if(x >= 0 && x <= 69)
        return(fact[x]);
    else
        return(0.0);
}
