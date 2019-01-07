#include "spacenavInterface.h"

/*******************************************************************************************************************
*   Demo application for spacenav 3d mouse
*
*   Interface implementation file
*
********************************************************************************************************************/

void sig(int s)
{
    exit(0);
}

int timediff(struct timespec end, struct timespec start)
{
    int a = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;
    return a;
}