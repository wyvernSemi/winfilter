
[Setup]
AppName                 = WinFilter
AppVerName              = WINFILTER_1_0_0
DefaultDirName          = {pf}\WinFilter
DisableProgramGroupPage = yes
OutputBaseFilename      = setup_winfilter_1_0_0

[Files]
; This inno file
Source: "winfilter.iss";                               DestDir: "{app}"

; Documentation
Source: "doc\WinFilter.pdf";                           DestDir: "{app}\doc"; DestName: "README.pdf"; Flags: isreadme

; Visual studio files for 2010 express
Source: "WinFilter.sln";                               DestDir: "{app}"
Source: "WinFilter\WinFilter.vcxproj";                 DestDir: "{app}\WinFilter"
Source: "WinFilter\WinFilter.vcxproj.filters";         DestDir: "{app}\WinFilter"


; Pre-compiled executable for windows
Source: "Release\WinFilter.exe";                       DestDir: "{app}"

; Source code
Source: "Code\config.c";                               DestDir: "{app}\Code"
Source: "Code\factorial.c";                            DestDir: "{app}\Code"
Source: "Code\fft.c";                                  DestDir: "{app}\Code"
Source: "Code\filt_func.c";                            DestDir: "{app}\Code"
Source: "Code\filter.c";                               DestDir: "{app}\Code"
Source: "Code\Getopt.c";                               DestDir: "{app}\Code"
Source: "Code\Graph.c";                                DestDir: "{app}\Code"
Source: "Code\Graph.h";                                DestDir: "{app}\Code"
Source: "Code\op_coeff.c";                             DestDir: "{app}\Code"
Source: "Code\window.c";                               DestDir: "{app}\Code"
Source: "Code\WinFilter.c";                            DestDir: "{app}\Code"

Source: "include\config.h";                            DestDir: "{app}\include"
Source: "include\cos_table.h";                         DestDir: "{app}\include"
Source: "include\fft.h";                               DestDir: "{app}\include"
Source: "include\filter.h";                            DestDir: "{app}\include"
Source: "include\window.h";                            DestDir: "{app}\include"
Source: "include\WinFilter.h";                         DestDir: "{app}\include"

Source: "Resources\WinFilter.rc";                      DestDir: "{app}\Resources"
Source: "Resources\winfilter.ico";                     DestDir: "{app}\Resources"
Source: "Resources\xfilter.bmp";                       DestDir: "{app}\Resources"
Source: "Resources\resource.h";                        DestDir: "{app}\Resources"

; Third party libraries
Source: "lib\glgraph.lib";                             DestDir: "{app}\lib"
Source: "lib\glut32.lib";                              DestDir: "{app}\lib"
Source: "lib\glut32.dll";                              DestDir: "{app}\lib"

