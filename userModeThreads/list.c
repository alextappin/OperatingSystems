#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread.h>

#include "list.h"

typedef struct item_s1
{
    void *value;
    struct item_s1 *next;
    struct item_s1 *prev;
    pthread_mutex_t mutex;
} list_tt;

//**********************************
void List_Init(list_t *l)
{
    assert(l != NULL);
    list_tt *list = (list_tt *)malloc(sizeof(list_tt));
    pthread_mutex_init(&list->mutex, NULL);
    //pthread_mutex_lock(&list->mutex);
    assert(list != NULL);
    list->next = NULL;
    list->prev = NULL;
    list->value = NULL;
    *l = list;
    //pthread_mutex_unlock(&list->mutex);
}
//**********************************
// NOTE: This function destroys the list nodes, but not the
//       items stored in the list
void List_Destroy(list_t list)
{
    if (list == NULL) return;

    list_tt *ptr = (list_tt *)list;
    list_tt *prev;

    prev = ptr;
    ptr = ptr->next;

    while (ptr != NULL)
    {
        free(prev);
        prev = ptr;
        ptr = ptr->next;
    }
    free(prev);
}
//**********************************
list_item_t List_First(list_t list)
{
    list_tt *ptr = (list_tt *)list;
    
    if (list == NULL) 
    {
        return NULL;
    }
    //pthread_mutex_lock(&ptr->mutex);
    return (list_item_t)list;
}
//**********************************
list_item_t List_Next(list_item_t item)
{
    list_tt *ptr = (list_tt *)item;
    if (ptr == NULL) 
        return NULL;
    //if (ptr->next == NULL)
        //pthread_mutex_unlock(&ptr->mutex);
    return ptr->next;
}
//**********************************
list_item_t List_Insert_At(list_item_t item, void *value)
{
    assert(item != NULL);

    list_tt *ptr = (list_tt *)item;
    list_tt *next = ptr->next;

    ptr->next = (list_tt *)malloc(sizeof(list_tt));
    assert(ptr->next != NULL);

    ptr->next->value = value;
    ptr->next->next = next;
    ptr->next->prev = ptr;
    if (next != NULL) next->prev = ptr->next;
    return ptr->next;
}
//**********************************
list_item_t List_Remove_At(list_item_t item)
{
    assert(item != NULL);
    list_tt *ptr = (list_tt *)item;
    list_tt *next;

    // make sure this isn't the dummy at the beginning
    assert (ptr->prev != NULL);

    next = ptr->next;

    ptr->prev->next = ptr->next;
    if (ptr->next != NULL) ptr->next->prev = ptr->prev;

    free(ptr);

    return next;
}
//**********************************
void *List_Get_At(list_item_t item)
{
    assert(item != NULL);

    list_tt *ptr = (list_tt *)item;

    return ptr->value;
}
//**********************************
void List_Done_Iterating(list_item_t item)
{
    list_tt *ptr = (list_tt *)item;
    //pthread_mutex_unlock(&ptr->mutex);
}