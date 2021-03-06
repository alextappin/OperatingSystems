#include <stdio.h>
#include <stdlib.h>

#include "sched.h"
#include "mythreadio.h"

// function we are going to run as a thread
void *thread_test(void *arg)
{
    long count = (long)arg;
    char buff[100];

    while (--count)
    {
        sprintf(buff, "Counting down: %ld\n", count);
        //printf("Printing: %s", buff);
        myprints(buff);
    }

    myprints("Thread is exiting!\n");

    return "Goodbye from the thread";
}

int main()
{
    unsigned long thread_1;
    unsigned long thread_2;
    void *result;

    mythread_init();

    thread_1 = mythread_create(thread_test, (void *)5);

    thread_2 = mythread_create(thread_test, (void *)10);

    mythread_join(thread_1, NULL);
    mythread_join(thread_2, &result);

    printf("Received '%s' from the thread\n", (char *)result);

    printf("main is going away\n");

    mythread_cleanup();

    return 0;
}

