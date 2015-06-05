//********************************************
// User mode cooperative thread implementation
//
// Author: Alexander Tappin
//         Phil Howard
// Email:  alextappin@gmail.com
//         phil.howard@oit.edu
//

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>
#include <unistd.h>
#include "sched.h"
#include "list.h"
#include "queue.h"
#include <time.h>

static const int STACK_SIZE = 1024*1024;
static unsigned long Next_Thread_Id = 1;

#define RUNNING      0
#define READY_TO_RUN 1
#define WAIT_JOIN    2
#define STOPPED      3
#define IO_WAIT      4
#define SLEEPING     5

typedef struct thread_s
{
    unsigned long sp;
    unsigned long fp;
    unsigned long thread_id;
    void *return_val;
    long *stack;
    void *(*start_func)(void *arg);
    void *arg;
    int  is_detached;
    int  state;
    struct thread_s *waiting_for_me;
    time_t endTime;
    time_t startTime;
} thread_t;

typedef struct io_stuff 
{
    thread_t *threadControlBock;
    //types are defined
    int operationType;
    char* str;
    int number;
}io_blk;

static thread_t *Current_Thread;
static list_t *Thread_List;
static queue_t *Ready_Queue;
static queue_t *Io_Blk_Queue;
static queue_t *Print_Blk_Queue;
pthread_t getThread;
pthread_t printThread;

time_t CurrentTime;

//*************************************
static int Do_Debug = 0;
static void debug_print(const char *fmt, ...)
{
    if (!Do_Debug) return;

    va_list args;
    va_start(args, fmt);

    vfprintf(stderr, fmt, args);
    va_end(args);
}
//*************************************************
// Search for a thread in the Thread List
// Leave the iterator at this thread in case the thread needs to be
// deleted
static thread_t *find_thread(int thread_id)
{
    thread_t *thread;

    list_start_iter(Thread_List);
    while (list_has_next(Thread_List))
    {
        thread = (thread_t *)list_get_at(Thread_List);
        if (thread->thread_id == thread_id) return thread;
        list_next(Thread_List);
    }

    return NULL;
}
//*************************************
void mythread_init()
{
    Ready_Queue = queue_create();
    Io_Blk_Queue = queue_create();
    Print_Blk_Queue = queue_create();
    Thread_List = list_init();
    Current_Thread = NULL;
    // Create TCB for whoever is initing me
    Current_Thread = (thread_t *)malloc(sizeof(thread_t));
    assert(Current_Thread != NULL);

    memset(Current_Thread, 0, sizeof(thread_t));

    Current_Thread->sp = 0;
    Current_Thread->fp = 0;
    Current_Thread->thread_id = Next_Thread_Id++;
    Current_Thread->return_val = NULL;
    Current_Thread->stack = NULL;
    Current_Thread->start_func = NULL;
    Current_Thread->arg = NULL;
    Current_Thread->is_detached = 1;
    Current_Thread->state = RUNNING;
    Current_Thread->endTime = time(NULL);
    Current_Thread->startTime = time(NULL);
    
    
    list_insert_at_head(Thread_List, Current_Thread);
    
    pthread_create(&getThread, NULL, consumer, Io_Blk_Queue);
    pthread_create(&printThread, NULL, consumer, Print_Blk_Queue);
}
//*************************************
// Can be called to clean up any memory used by thread data structures.
// This function assumes all threads have finished.
void mythread_cleanup()
{
    queue_mark_closed(Ready_Queue);
    queue_mark_closed(Io_Blk_Queue);
    queue_mark_closed(Print_Blk_Queue);
    pthread_join(getThread,NULL);
    pthread_join(printThread,NULL);
    queue_close(Ready_Queue);
    queue_close(Io_Blk_Queue);
    queue_close(Print_Blk_Queue);
    list_close(Thread_List);
    free(Current_Thread);
}
//*************************************
// Function that gets called on thread startup. It calls the main thread
// function and then stops the thread.
static void start_thread_func()
{
    void *result;
    result = Current_Thread->start_func(Current_Thread->arg);

    mythread_exit(result);
}

// //*************************************
// Equivalent of pthread_create
unsigned long mythread_create( void *(*func)(void *arg), void *arg)
{
    long *mem;
    thread_t *thread;

    // allocate the thread control block
    thread = (thread_t *)malloc(sizeof(thread_t));
    assert(thread != NULL);

    memset(thread, 0, sizeof(thread_t));

    // Initialize thread info
    thread->thread_id = Next_Thread_Id++;
    thread->is_detached = 0;
    thread->state = READY_TO_RUN;

    // Allocate space for the stack
    thread->stack = (long *)malloc(STACK_SIZE*sizeof(long));
    debug_print("Create stack: %p\n", thread->stack);

    // Remember the user's thread function and argument
    thread->start_func = func;
    thread->arg = arg;

    // Initialize the stack. Pretend we made a call to yield 
    // from the beginning of start_thread_func
    // NOTE: The following code, even though written in C is machine specific
    mem = thread->stack;

    mem[STACK_SIZE-1] = (long)start_thread_func;        // return addr
    mem[STACK_SIZE-2] = (long)&mem[STACK_SIZE-2];       // frame ptr
    thread->fp = (long)&mem[STACK_SIZE-2];              // save FP
    thread->sp = (long)&mem[STACK_SIZE-2];              // save SP

    // add the TCB to the thread list and to the ready queue
    list_insert_at_head(Thread_List, thread);
    queue_insert(Ready_Queue, thread);

    // a call to yield will now run this thread
    return thread->thread_id;
}

// //*************************************
// Threads call this to yield the CPU
void mythread_yield()
{
    unsigned long reg;
    static thread_t *prev_thread;

    prev_thread = Current_Thread;

    if (Current_Thread->state != STOPPED)
    {
        debug_print("Thread %ld is yielding\n", Current_Thread->thread_id);
        // save FP, SP and queue current thread in ready queue
        __asm__ volatile("movq %%rbp, %0" : "=m" (reg) : :);
        Current_Thread->fp = reg;
        __asm__ volatile("movq %%rsp, %0" : "=m" (reg) : :);
        Current_Thread->sp = reg;

        // If I'm currently running, add myself to the ready queue
        if (Current_Thread->state == RUNNING)
        {
            Current_Thread->state = READY_TO_RUN;
            queue_insert(Ready_Queue, Current_Thread);
        } 
    }

    // Get the next thread
    Current_Thread = (thread_t *)queue_remove(Ready_Queue);
    while (Current_Thread == NULL)
    {
        Current_Thread = (thread_t *)queue_remove(Ready_Queue);
    }
    //does a busy wait... but its okay
    while (Current_Thread->state == SLEEPING)
    {
        //keep it sleeping if the time to end is not expired yet
        time(&CurrentTime); //gets the current Time
        //if the time has not expired yet...
        if(difftime(Current_Thread->endTime, CurrentTime) > 0)
            queue_insert(Ready_Queue, Current_Thread);
        else //it is finished sleeeping
        {
            Current_Thread->state = READY_TO_RUN;
            queue_insert(Ready_Queue, Current_Thread);
        }
        //get the next thread in the Q
        Current_Thread = (thread_t *)queue_remove(Ready_Queue);
    }

    debug_print("Thread %ld is running\n", Current_Thread->thread_id);
    Current_Thread->state = RUNNING;

    // restore the SP, FP
    reg = Current_Thread->sp;
    __asm__ volatile("movq %0, %%rsp" : : "m" (reg) :);

    reg = Current_Thread->fp;
    __asm__ volatile("movq %0, %%rbp" : : "m" (reg) :);

    // Clean up stopped threads
    if (prev_thread->state == STOPPED && prev_thread->is_detached)
    {
        thread_t *thread = find_thread(prev_thread->thread_id);
        if (thread == NULL) 
        {
            fprintf(stderr, "Threading error. Invalid thread ID\n");
            exit(-1);
        }
        list_remove_at(Thread_List);
        debug_print("Free stack: %p\n", prev_thread->stack);
        if (prev_thread->stack != NULL) free(prev_thread->stack);
        free(prev_thread);
    }

    // return to next thread
    return;
}
//*************************************************
// Terminate a thread
void mythread_exit(void *result)
{
    debug_print("Thread %ld is exiting\n", Current_Thread->thread_id);
    Current_Thread->return_val = result;
    Current_Thread->state = STOPPED;

    if (Current_Thread->waiting_for_me != NULL) 
    {
        Current_Thread->waiting_for_me->state = READY_TO_RUN;
        queue_insert(Ready_Queue, Current_Thread->waiting_for_me);
    }

    // this call should never return. Thread is DONE.
    mythread_yield();
}
// **********************************
// Wait for a thread
void mythread_join(unsigned long thread_id, void **result)
{
    thread_t *thread;

    // need to yield so that if the thread I'm waiting for hasn't started,
    // it has a chance to start before we try to join it.
    debug_print("Thread %ld is in join %ld\n", 
            Current_Thread->thread_id, thread_id);
    mythread_yield();

    debug_print("Thread %ld is joining %ld\n", 
            Current_Thread->thread_id, thread_id);
    thread = find_thread(thread_id);
    if (thread == NULL) return;

    debug_print("Thread %ld is checking state of %ld\n", 
            Current_Thread->thread_id, thread_id);
    if (thread->state != STOPPED)
    { 
        debug_print("Thread %ld is waiting for %ld\n", 
            Current_Thread->thread_id, thread_id);
        thread->waiting_for_me = Current_Thread;
        Current_Thread->state = WAIT_JOIN;
        mythread_yield();
    }

    // at this point, the thread we are waiting on has finished, so we
    // can clean up memory.
    debug_print("Thread %ld is joined %ld\n", 
            Current_Thread->thread_id, thread_id);

    // handle return value of thread
    if (result != NULL) *result = thread->return_val;

    // need to repeat find because another thread may have move iter
    thread = find_thread(thread_id);
    if (thread == NULL) return;

    list_remove_at(Thread_List);
    if (thread->stack != NULL) free(thread->stack);
    free(thread);
}
//********************************************
// Detach a thread so it will clean up after itself without waiting 
// to be joined
void mythread_detach(unsigned long thread_id)
{
    thread_t *thread;

    thread = find_thread(thread_id);
    if (thread != NULL) thread->is_detached = 1;
}

//initiallizes a new ioblock and puts it into the correct q
void mythread_io(char* str, int sizeTime, int operationType)
{
    io_blk *Io_Blk;
    
    Io_Blk = (io_blk *)malloc(sizeof(io_blk));
    Io_Blk->threadControlBock = Current_Thread;
    Io_Blk->operationType = operationType;
    Io_Blk->number = sizeTime;
    Io_Blk->str = str;
    
    //specific Q for the Mygets
    if(operationType == MYGETS)
        queue_insert(Io_Blk_Queue, Io_Blk);
    else
        queue_insert(Print_Blk_Queue, Io_Blk);
    //need to set the state to iowait so that it has a chance to run
    Current_Thread->state = IO_WAIT;
    mythread_yield();
}

void* consumer(void* Q)
{
    queue_t* queue = (queue_t*)Q;
    while(queue_is_open(queue))
    {
        io_blk *block = queue_remove(queue);
        if(block != NULL)
        {
            if (block->operationType == MYGETS)
            {
                fgets(block->str, block->number, stdin);
                block->threadControlBock->return_val = block->str;
                
                block->threadControlBock->state = READY_TO_RUN;
            }
            else if(block->operationType == MYPRINTS)
            {
                printf("%s", block->str);
                usleep(100000); //this helps it not go crazy on test 7
                    
                block->threadControlBock->state = READY_TO_RUN;
            }
            else if(block->operationType == MYSLEEP)
            {
                //we set the state to sleeping then set the time it will wakeup
                block->threadControlBock->state = SLEEPING;
                block->threadControlBock->endTime = time(&CurrentTime) + (block->number); 
                //gets the current time + when it needs to end and stores it
            }
            
            queue_insert(Ready_Queue, block->threadControlBock);
            
            free(block);
        }
    }
    return Q;
}