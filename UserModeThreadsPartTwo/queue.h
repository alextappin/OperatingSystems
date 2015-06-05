#pragma once
//********************************************
// Thread-safe queue implementation
//
// Author: Phil Howard
// Email:  phil.howard@oit.edu
//

// Items that get queued
typedef struct item_qs
{
    char *data;                 // location for user's data
    struct item_qs *next;
} item_qt;

// Structure for actual queue
typedef struct 
{
    item_qt *head;
    item_qt *tail;
    int is_open;
    pthread_mutex_t lock;
    pthread_cond_t  cond;
} queue_t;

//********************************
queue_t *queue_create();
void queue_insert(queue_t *q, void *data);
void *queue_remove(queue_t *q);
void queue_mark_closed(queue_t *q);
int queue_is_open(queue_t *q);
void queue_close(queue_t *q);
