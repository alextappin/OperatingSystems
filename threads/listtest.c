//#include <sched.h>
//#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <limits.h>
#include "sequence.h"
#include "list.h"

// maximum number of threads we'll support
#define MAX_THREADS     100

// Type of items inserted into the list
typedef struct
{
    int sequence;       // unique sequence for process
    int value;          // unique sequence for thread
    pthread_t thread;   // thread that created the item
} item_t;

// The list
list_t My_List;
//pthread_mutex_t mutexList = PTHREAD_MUTEX_INITIALIZER;
//********************************
// Function that adds the items into the list
// Intended to be run in a thread
// The arg indicates how many items to be inserted
static void *get_numbers(void *arg)
{
    // cast (void *) to (long)
    long num_sequences = (long)arg;
    int ii;

    // Insert at the end of the list
    for (ii=0; ii<num_sequences; ii++)
    {
        // Create the item
        item_t *item = (item_t *)malloc(sizeof(item_t));
        assert(item != NULL);

        item->sequence = Get_Sequence();
        item->value = ii;
        item->thread = pthread_self();

        // find the last item
//        pthread_mutex_lock(&mutexList);
        list_item_t prev = List_First(My_List);
        list_item_t ptr = List_Next(prev);
        while (ptr != NULL)
        {
            prev = ptr;
            ptr = List_Next(prev);
        }

        // insert at the end of the list
        List_Insert_At(prev, item);
        List_Done_Iterating(My_List);
//        pthread_mutex_unlock(&mutexList);
    }

    // Insert at beginning of the list
    /*
    for (ii=0; ii<num_sequences; ii++)
    {
        item_t *item = (item_t *)malloc(sizeof(item_t));
        item->sequence = Get_Sequence();
        item->value = ii;
        item->thread = pthread_self();
        list_item_t ptr = List_First(My_List);
        List_Insert_At(ptr, item);
    }
    */

    return NULL;
}
//********************************
int main(int argc, char **argv)
{
    pthread_t thread_id[MAX_THREADS];

    int num_threads = 4;
    long num_sequences = 10;
    int ii;

    List_Init(&My_List);

    if (argc > 1) num_threads = atoi( argv[1] );
    if (argc > 2) num_sequences = atoi( argv[2] );

    if (num_threads > MAX_THREADS)
    {
        printf("Running with maximum supported threads (%d)\n", MAX_THREADS);
        num_threads = MAX_THREADS;
    }
  // insert an item of sequence INT_MAX
    /*list_item_t prev = List_First(My_List);
    item_t *maxSequenceItem = (item_t *)malloc(sizeof(item_t));
    maxSequenceItem->sequence = INT_MAX;
    maxSequenceItem->value = num_sequences;
    List_Insert_At(prev, maxSequenceItem);
    List_Done_Iterating(My_List);*/
    // create the threads
    for (ii=0; ii<num_threads; ii++)
    {
        pthread_create(&thread_id[ii], NULL, 
                get_numbers, (void *)num_sequences);
    }

    // wait for the threads to finish
    for (ii=0; ii<num_threads; ii++)
    {
        pthread_join(thread_id[ii], NULL); 
    }

    // Print out the list
    list_item_t item = List_First(My_List);
    item = List_Next(item);
    item_t *data;
    int count = 0;
    int max_seq = -1;
    while (item != NULL)
    {
        count++;
        data = (item_t *)List_Get_At(item);
        if (data->sequence > max_seq) max_seq = data->sequence;
        printf("%d %d %ld\n", data->sequence, data->value, data->thread);
        item = List_Next(item);
    }
    
    //List_Remove_At(prev);
    printf("Found %d items\n", count);
    printf("Max sequence: %d\n", max_seq);

    return 0;
}
