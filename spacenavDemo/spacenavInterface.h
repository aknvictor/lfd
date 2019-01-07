
/*******************************************************************************************************************
*   Demo application for spacenav 3d mouse
*
*   Interface file
*
********************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include <time.h>

#include <signal.h>
#include <X11/Xlib.h>
#include <spnav.h>


void sig(int s);

int timediff(struct timespec end, struct timespec start);