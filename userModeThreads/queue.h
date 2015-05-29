//Alexander Tappin
//Operating Systems OIT
//April 20, 2015
//Dr.Howard
#pragma once
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <semaphore.h>
#define MAX_LINE_LENGTH 80

//static int testing = 0;
//int NUMBER_OF_THREADS = 10;
// Items that get queued
typedef struct item_s
{
    char *data;                 // location for user's data
    struct item_s *next;
} item_t;

// Structure for actual queue
typedef struct 
{
    item_t *head;
    item_t *tail;
    bool closed;
    sem_t full;
    sem_t empty;
    pthread_mutex_t mutex;
} queue_t;

queue_t* startProducer();
queue_t* initializeQ();
void queue_insert(queue_t *q, void *data);
void *queue_remove(queue_t *q);
bool queue_is_open(queue_t *q);