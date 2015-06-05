#pragma once

//****************************************

//operation types
#define MYPRINTS     10
#define MYGETS       11
#define MYSLEEP      12
// User level cooperative thread library

// Must be called prior to any other mythread routines
void mythread_init();

// Can be called to clean up any memory used by thread data structures.
// This function assumes all threads have finished.
void mythread_cleanup();

// Equivalent of pthread_create
unsigned long mythread_create( void *(*func)(void *arg), void *arg);

// Threads call this to yield the CPU
void mythread_yield();

// Terminate a thread
void mythread_exit(void *result);

// Wait for a thread
void mythread_join(unsigned long thread_id, void **result);
unsigned long mythread_create( void *(*func)(void *arg), void *arg);

// Detach a thread so it will clean up after itself without waiting 
// to be joined
void mythread_detach(unsigned long thread_id);

void mythread_io(char* str, int sizeTime, int operationType);

void* consumer(void* Q);


