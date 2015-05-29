//Alexander Tappin
//Operating Systems OIT
//April 20, 2015
//Dr.Howard
#include <stdio.h>
#include "queue.h"

//********************************
queue_t* initializeQ()
{
    queue_t* Q = (queue_t*)malloc(sizeof(queue_t));
    Q->head = NULL;
    Q->tail = NULL;
    Q->closed = false;
    //second argument 0 because shared between threads
    sem_init (&Q->empty, 0, 1);
    sem_init (&Q->full, 0, 0);
    return Q;
}
//********************************
void queue_insert(queue_t *q, void *data)
{
    assert(q != NULL);

    item_t *item = (item_t *)malloc(sizeof(item_t));
    assert(item != NULL);

    item->data = data;
    item->next = NULL;
    if (q->tail != NULL) q->tail->next = item;
    q->tail = item;
    if (q->head == NULL) q->head = item;
}
//********************************
void *queue_remove(queue_t *q)
{
    item_t *item;
    char *data;

    assert(q != NULL);

    if (q->head == NULL) return NULL;

    //printf("Removing from queue\n");
    item = q->head;
    q->head = item->next;
    if (q->head == NULL) q->tail = NULL;

    data = item->data;

    free(item);

    return data;
}

bool queue_is_open(queue_t *q)
{
    if(q == NULL || (q->head == NULL && q->closed))
        return false;
    return true;
}
