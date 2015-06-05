#include <stdio.h>
#include <stdlib.h>

#include "sched.h"
#include "mythreadio.h"

// function we are going to run as a thread
void *write_thread(void *arg)
{
    unsigned long delay = (unsigned long)arg;
    long count = 0;
    char buff[100];

    while (++count)
    {
        sprintf(buff, "%ld Counting up: %d\n", delay, count);
        mysleep(delay);
        myprints(buff);
    }

    myprints("Write thread is exiting!\n");

    return NULL;
}

void *read_thread(void *arg)
{
    char buff[100];

    myprints("Enter a line of text: ");
    mygets(buff, sizeof(buff));
    myprints("Read: ");
    myprints(buff);

    myprints("Enter a line of text: ");
    mygets(buff, sizeof(buff));
    myprints("Read: ");
    myprints(buff);
    return NULL;
}

int main()
{
    unsigned long thread_1;
    unsigned long thread_2;
    void *result;

    mythread_init();

    thread_1 = mythread_create(write_thread, (void *)1);
    mythread_detach(thread_1);
    thread_1 = mythread_create(write_thread, (void *)3);
    mythread_detach(thread_1);
    thread_2 = mythread_create(read_thread, (void *)10);

    mythread_join(thread_2, NULL);

    printf("main is going away\n");

    //mythread_cleanup();

    return 0;
}

