//Alexander Tappin
//Operating Systems OIT
//April 20, 2015
//Dr.Howard
#include <stdio.h>
#include "queue.h"

const int MAX_BUFFER_SIZE = 80;

void* readCharacters(void* parameter)
{
    printf("\nIm in the readCharacters:\n");
    queue_t * Q = (queue_t*) parameter;
    char charBuffer[MAX_BUFFER_SIZE];
    int index = 0;
    char charGet = getchar();
    
    while(charGet != EOF)
    {
        //if buffer size limit is reached or newline
        if((charGet == '\n' && (charBuffer[0] != 0 || charBuffer[0] != '\n')) || index == MAX_BUFFER_SIZE-1)
        {
            //if other threads have the Stuff. Not sure if this is needed
            sem_wait(&Q->empty);
            pthread_mutex_lock(&Q->mutex);
            printf("Producing\n");
            //puts the buffer into the Q
            queue_insert(Q, (void*)charBuffer);
            //unlocks and lets the sem know that it is now full and is ready to read
            pthread_mutex_unlock(&Q->mutex);
            sem_post(&Q->full);
            index = 0;
            charGet = getchar();
            charBuffer[index] = 0;
        }
        else
        {
            //add the charGet to the buffer
            charBuffer[index] = charGet;
            //gets the next char in the input
            charGet = getchar();
            index++;
            //always a null terminated string now
            charBuffer[index] = 0;
        }
    }
    //its all done
    Q->closed = true;
    sem_post(&Q->empty);
    printf("Producer is done\n");
    return NULL;
}

queue_t* startProducer()
{
    queue_t* Q = initializeQ();
    pthread_t threadId = (pthread_t)malloc(sizeof(pthread_t));
    //starts the thread at the consumer?
    pthread_create(&threadId, NULL, readCharacters, Q);
    //detaches the thread
    pthread_detach(threadId);
    return Q;
    
}
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

    printf("Removing from queue\n");
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