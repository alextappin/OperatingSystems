#include "mythreadio.h"

int myprints(char* str)
{
    mythread_io(str, 0, MYPRINTS);
    return 0;
}

int mygets(char* buffer, int size)
{
    mythread_io(buffer, size, MYGETS);
    //queue these, process everything else immediately
    return 0;
}

int mysleep(int time)
{
    mythread_io(NULL, time, MYSLEEP);
    return 0;
}