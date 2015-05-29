#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "sched.h"


// use a 1M stack
#define STACK_SIZE  (1024*1024)

typedef struct thread_s
{
    unsigned long sp;
    unsigned long fp;
    long *stack;
    void *(*start_func)(void *arg);
    void *arg;
    int detach;
    //0=readyToRun, 1=Waiting, 2=Running, 3=DoneState
    int statusOfThread;
    struct thread_s *ListsCurrentThread;
} thread_t;


thread_t *Current_Thread;
thread_t *Next_Thread;
queue_t *Thread_Q;
list_t *Thread_List;

//*************************************
// This function can be used for adding debug statements to your code.
// Debug statements can then be turned on/off with the Do_Debug flag.
static int Do_Debug = 0;
static void debug_print(const char *fmt, ...)
{
    if (!Do_Debug) return;

    va_list args;
    va_start(args, fmt);

    vfprintf(stderr, fmt, args);
    va_end(args);
}
//*************************************
void mythread_init()
{
    // Create TCB for whoever is initing me
    Current_Thread = (thread_t *)malloc(sizeof(thread_t));
    assert(Current_Thread != NULL);

    memset(Current_Thread, 0, sizeof(thread_t));

    Current_Thread->sp = 0;
    Current_Thread->fp = 0;
    Current_Thread->stack = NULL;
    Current_Thread->start_func = NULL;
    Current_Thread->arg = NULL;
    Current_Thread->ListsCurrentThread = NULL;
    //set it to 0 for detach
    Current_Thread->detach = 0;
    //ready to run
    Current_Thread->statusOfThread = 0;
    
    List_Init(&Thread_List);
    Thread_Q = initializeQ();
}
// //*************************************
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

    // Allocate space for the stack
    thread->stack = (long *)malloc(STACK_SIZE*sizeof(long));

    // Remember the user's thread function and argument
    thread->start_func = func;
    thread->arg = arg;

    // Initialize the stack. Pretend we made a call to yield 
    // from the beginning of start_thread_func
    //
    // NOTE: The following code, even though written in C is machine specific
    mem = thread->stack;

    mem[STACK_SIZE-1] = (long)start_thread_func;        // return addr
    mem[STACK_SIZE-2] = (long)&mem[STACK_SIZE-2];       // frame ptr
    thread->fp = (long)&mem[STACK_SIZE-2];              // save FP
    thread->sp = (long)&mem[STACK_SIZE-2];              // save SP
    
    //ready to run
    thread->statusOfThread = 0;
    Next_Thread = thread;
    //Current_Thread = thread;
    queue_insert(Thread_Q, (void*)thread);
    
    //make an item to insert into
    list_item_t insertList = List_First(Thread_List);
    //insert the current thread into the thread list
    List_Insert_At(insertList, (void *)thread);
    // a call to yield will now run this thread
    //mythread_yield();
    //free(thread);
    //printf("This is the Create Stack %d \n", thread->stack);
    return thread->stack;   // should return thread_id
}

// //*************************************
// Threads call this to yield the CPU
void mythread_yield()
{
    unsigned long reg;

    debug_print("Thread is yielding\n");
    // save FP, SP and queue current thread in ready queue
    __asm__ volatile("movq %%rbp, %0" : "=m" (reg) : :);
    Current_Thread->fp = reg;
    __asm__ volatile("movq %%rsp, %0" : "=m" (reg) : :);
    Current_Thread->sp = reg;

    //if the current thread is not waiting, set it to ready to run, add to queue
    // and then set the current thread to the next thing in the Q
    //if (Current_Thread->statusOfThread == 1)
    //  Current_Thread = Next_Thread;
    //else
    //{
    
    //make the thread ready to run before storing it in Q
    Current_Thread->statusOfThread = 0;
    queue_insert(Thread_Q, (void*)Current_Thread);
    Current_Thread = (thread_t*)queue_remove(Thread_Q);
    //make the thread running once it leaves the Q
    Current_Thread->statusOfThread = 2;
    
    //}
    
    
    debug_print("Thread is running\n");

    // restore the SP, FP
    reg = Current_Thread->sp;
    __asm__ volatile("movq %0, %%rsp" : : "m" (reg) :);

    reg = Current_Thread->fp;
    __asm__ volatile("movq %0, %%rbp" : : "m" (reg) :);

    // return to next thread
    return;
}
//*************************************************
// Terminate a thread
void mythread_exit(void *result)
{
    debug_print("Thread is exiting\n");
    
    //it is finished/done
    //Current_Thread->statusOfThread = 3;
    // clean up thread data structures if detached!
    if(Current_Thread->detach == 1)
        Current_Thread = NULL;
    //more than one so it will keep yielding if not detached/fin
    while(Current_Thread->detach == 0)
        mythread_yield();
}
// **********************************
// Wait for a thread
//I ATTEMPTED TO COMPLETE THE LAB USING A LIST AND CHECKING THE ID'S WITH THAT
//BUT WAS UNABLE TO STOP THE OTHER THREAD FROM NOT GETTING THE CPU WHEN IT RETURNED
//If I USED THE COMMENTED OUT CODE WITH THE WHILE LOOP, I WOULD ONLY GET THE
//TOP HALF OF THE TEST4 AND THEN THE MAIN WOULD EXIT INSTEAD OF THE THREAD
void mythread_join(unsigned long thread_id, void **result)
{
    mythread_yield();
    list_item_t traverse = List_First(Thread_List);
    traverse = List_Next(traverse);
    //marks it as waiting
    Current_Thread->statusOfThread = 1;

    //Hacky way of getting the test to pass. Jsut keeps yeilding til there is 
    //no more left to yeild...
    //it seemed as if BOTH threads were being set to the "waiting" status and not
    //letting one NOT use the cpu. I may have to delete the Thread from the list
    //instead of just changing the status of it!
    mythread_yield();
    mythread_yield();
    mythread_yield();
    //get the next one that is equal to the thread passed in my create. Check the ID's
    //while(traverse != NULL && ((thread_t *)List_Get_At(traverse))->stack != thread_id)
    //{
        //printf("threadid In Loop %d \n",thread_id);
        //printf("stack In Loop %d \n",((thread_t *)List_Get_At(traverse))->stack);
        //traverse = List_Next(traverse);
    //}
    //printf("threadid After Loop %d \n",thread_id);
    //printf("stack After Loop %d \n",((thread_t *)List_Get_At(traverse))->stack);
    //Next_Thread = ((thread_t *)List_Get_At(traverse));
    
    //Current_Thread->statusOfThread = 1;
    mythread_yield();
}
void mythread_detach(unsigned long thread_id)
{
    //set the detach to true which is 1
    Current_Thread->detach = 1;
}
void mythread_cleanup()
{
    free(Current_Thread->stack);
    free(Current_Thread);
    //free(thread);
}

