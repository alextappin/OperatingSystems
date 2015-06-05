//********************************************
// Thread-safe list implementation
//
// Author: Phil Howard
// Email:  phil.howard@oit.edu
//

#include <assert.h>
#include <stdlib.h>
#include <pthread.h>

#include "list.h"

//********************************
list_t *list_init()
{
    list_t *list = (list_t *)malloc(sizeof(list_t));
    assert(list != NULL);

    list->head = NULL;
    list->iter = NULL;
    pthread_mutex_init(&list->lock, NULL);

    return list;
}
//********************************
void list_close(list_t *list)
{
    item_lt *curr, *prev;

    prev = list->head;
    while (prev != NULL)
    {
        curr = prev->next;
        free(prev);
        prev = curr;
    }

    free(list);
}
//********************************
void list_lock(list_t *list)
{
    pthread_mutex_lock(&list->lock);
}
//********************************
void list_unlock(list_t *list)
{
    pthread_mutex_unlock(&list->lock);
}
//********************************
void list_start_iter(list_t *list)
{
    list->iter = list->head;
}
//********************************
void list_next(list_t *list)
{
    if (list->iter != NULL) list->iter = list->iter->next;
}
//********************************
int  list_has_next(list_t *list)
{
    return (list->iter != NULL) && (list->iter->next != NULL);
}
//********************************
void *list_get_at(list_t *list)
{
    if (list->iter != NULL) return list->iter->data;
    return NULL;
}
//********************************
void list_remove_at(list_t *list)
{
    item_lt *item;

    if (list->iter != NULL)
    {
        if (list->head == list->iter) list->head = list->head->next;

        item = list->iter;
        if (list->iter->prev != NULL) list->iter->prev->next = list->iter->next;
        if (list->iter->next != NULL) list->iter->next->prev = list->iter->prev;
        list->iter = list->iter->next;
        free(item);
    }
}
//********************************
void list_insert_at_head(list_t *list, void *data)
{
    list->iter = NULL;
    list_insert_at(list, data);
}
//********************************
void list_insert_at(list_t *list, void *data)
{
    item_lt *item;

    item = (item_lt *)malloc(sizeof(item_lt));
    assert(item != NULL);

    item->data = data;

    if (list->iter == NULL)
    {
        // insert at head
        item->prev = NULL;
        item->next = list->head;
        if (list->head != NULL) list->head->prev = item;
        list->head = item;
    } else {
        // insert at iter
        item->prev = list->iter->prev;
        item->next = list->iter->next;
        if (list->iter->next != NULL) list->iter->next->prev = item;
        if (list->iter->prev != NULL) list->iter->prev->next = item;
        list->iter = item;
    }
}
