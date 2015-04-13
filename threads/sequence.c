#include "sequence.h"

static int Sequence = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Return a nunique sequence number
int Get_Sequence()
{
    pthread_mutex_lock(&mutex);
    int localSequence=(++Sequence);
    pthread_mutex_unlock(&mutex);
    return localSequence;
}

