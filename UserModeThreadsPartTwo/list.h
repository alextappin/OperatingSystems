#pragma once
//********************************************
// Thread-safe list implementation
//
// Author: Phil Howard
// Email:  phil.howard@oit.edu
//


// Items in the list
typedef struct item_ls
{
    char *data;                 // location for user's data
    struct item_ls *next;
    struct item_ls *prev;
} item_lt;

// Structure for actual list
typedef struct 
{
    item_lt *head;
    item_lt *iter;
    pthread_mutex_t lock;
} list_t;

//********************************
list_t *list_init();
void list_close(list_t *l);
void list_lock(list_t *l);
void list_unlock(list_t *l);
void list_start_iter(list_t *l);
void list_next(list_t *l);
int  list_has_next(list_t *l);
void *list_get_at(list_t *l);
void list_remove_at(list_t *l);
void list_insert_at(list_t *l, void *data);
void list_insert_at_head(list_t *l, void *data);
