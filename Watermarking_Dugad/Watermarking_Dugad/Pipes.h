#ifndef PIPES_H
#define PIPES_H

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

FILE *getVidIStream(char*);
FILE *getVidOStream(int, int);
FILE *getVidOFStream(char*, int, int);
FILE *getTestPNG(char* filename, int width, int height);
FILE *getVidOFMPEG4(char* filename, int width, int height);
int getVideoContext(char*, int&, int&, int&, int&, int&);

#endif