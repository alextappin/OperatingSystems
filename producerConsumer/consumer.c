//Alexander Tappin
//Operating Systems OIT
//April 20, 2015
//Dr.Howard
#include "queue.h"

void *consume_queue(void*);
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char* argv[])
{
    int index = 0;
    int NUMBER_OF_THREADS = 1;
    //command line argument
    //Comand Line Arg to specify the number of threads
    if (argc > 1) 
        NUMBER_OF_THREADS = atoi( argv[1] );
    //array of pthreads    
    pthread_t thread_id[NUMBER_OF_THREADS];
    //starts producing which initialized the Q and waits for input
    queue_t* Q = startProducer();
    
    //create the threads then join them
    for (index=0; index<NUMBER_OF_THREADS; index++)
        pthread_create(&thread_id[index], NULL, consume_queue, Q);
    for (index=0; index<NUMBER_OF_THREADS; index++)
        pthread_join(thread_id[index], NULL);
        
    //will finish when main returns.
    return 0;
}

void *consume_queue(void* parameter)
{
    pthread_mutex_lock(&mutex);
    queue_t * Q = (queue_t*) parameter;
    char* buffer = '\0';
    pthread_mutex_unlock(&mutex);
    while(queue_is_open(Q))
    {
        //wait until something is IN the Q
        sem_wait(&Q->full);
        //if the buffer is null and something just got placed in the Q, lock it
        if(buffer == '\0')
            pthread_mutex_lock(&Q->mutex);
        //start extracting from the Q
        buffer = queue_remove(Q);
        //if there is a buffer and its not null print it out
        if(buffer && buffer[0] != '\0')
        {
            printf("ThreadId: %ld \nBufferRead:",pthread_self());
            fprintf(stdout, "%s\n", buffer);
            buffer = '\0';
            //release the lock
            pthread_mutex_unlock(&Q->mutex);
            //says that it now read from it.
            sem_post(&Q->empty);
        }
    }
    //ensure memory is deallocated
    pthread_mutex_lock(&mutex);
    sem_destroy(&Q->empty);
    sem_destroy(&Q->full);
    free(Q);
    //so we dont use the pointer after we free it. Could cause errors
    Q = NULL;
    pthread_mutex_unlock(&mutex);
    return NULL;
}