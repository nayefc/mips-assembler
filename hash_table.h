#ifndef __HASH_TABLE__
#define __HASH_TABLE__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <stdint.h>
#include "hash_function.h"

#define TRUE 1
#define FALSE 0

typedef struct hash_entry_type
{
  void *key;
  void *data;
  uint32_t key_len;
  struct hash_entry_type *next; 
  struct hash_entry_type *prev;
} hash_entry_t;

typedef struct 
{
  hash_entry_t **row;
  hash_entry_t **tail;
#ifdef __USE_HASH_LOCKS__
  sem_t *row_lock;
#endif
  uint32_t size;
} hash_table_t;

/* 
   hash table functions 
   copyright (c) 2005 dr. srinidhi varadarajan
*/

/*

   creates a hash table and returns a pointer to it

   parameters:
   hash_table_size : size of the hash table to create

   returns: pointer to created hash table or NULL on failure

*/

static inline hash_table_t *create_hash_table(uint32_t hash_table_size)
{
  uint32_t t;
  hash_table_t *hash_table;
  
  hash_table = ( hash_table_t *) malloc(sizeof( hash_table_t));
  if (hash_table == NULL) return(NULL);

  hash_table->row = ( hash_entry_t **) malloc(sizeof( hash_entry_t *) * (hash_table_size));
  if (hash_table->row == NULL) return(NULL);
  
  hash_table->tail = ( hash_entry_t **) malloc(sizeof( hash_entry_t *) * (hash_table_size));
  if (hash_table->tail == NULL) return(NULL);

#ifdef __USE_HASH_LOCKS__
  hash_table->row_lock = (sem_t *) malloc(sizeof(sem_t) * (hash_table_size + 2));
  if (hash_table->row_lock == NULL) return(NULL);
#endif

  for (t=0; t<hash_table_size; t++)
    {
      hash_table->row[t] = NULL;
      hash_table->tail[t] = NULL;
#ifdef __USE_HASH_LOCKS__
      sem_init(&hash_table->row_lock[t], 0, 1);
#endif
    }
  
  hash_table->size = hash_table_size;
  return(hash_table);
}

/* 
   inserts a structure into the hash table. 
   
   parameters : 
   hash_table : Hash table to use
   key : key to index the hash table
   key_len: length of the hash key in bytes
   data : pointer to the data to insert. you should allocate and free the 
          data pointer within your application
      
   returns: 
   TRUE if key was inserted into the table
   FALSE if key could not be inserted into the table

   note: a data element can be inserted more than once in this
         hash structure. be careful when you use hash_insert to make sure
         that if you insert multiple times, you also delete multiple times.
*/
static inline int32_t hash_insert( hash_table_t *hash_table, void *key, uint32_t key_len, void *data)
{
  uint32_t hash_key, hash_table_size;
  hash_entry_t *new_entry, *prev_ptr;
  
  hash_table_size = hash_table->size;

  hash_key  = hash(key, key_len, 7) % hash_table_size;

#ifdef __USE_HASH_LOCKS__
  sem_wait(&hash_table->row_lock[hash_key]);
#endif
  
  new_entry = ( hash_entry_t *) malloc(sizeof( hash_entry_t));
  if (new_entry == NULL) 
    {
#ifdef __USE_HASH_LOCKS__
      sem_post(&hash_table->row_lock[hash_key]);
#endif
      return(FALSE);
    }
  
  new_entry->key = (char *) malloc(key_len);
  if (new_entry->key == NULL)
    {
      printf("Warning: Unable to allocate memory for hash key. \n");
      free(new_entry);
#ifdef __USE_HASH_LOCKS__
      sem_post(&hash_table->row_lock[hash_key]);
#endif
      return(FALSE);
    }
  
  prev_ptr = hash_table->tail[hash_key];
  new_entry->next = NULL;
  new_entry->prev = hash_table->tail[hash_key];
  if (prev_ptr == NULL)
    hash_table->row[hash_key] = new_entry;
  else
    prev_ptr->next = new_entry;
  
  hash_table->tail[hash_key] = new_entry;
  memcpy(new_entry->key, key, key_len);
  new_entry->data = data;
  new_entry->key_len = key_len;
#ifdef __USE_HASH_LOCKS__
  sem_post(&hash_table->row_lock[hash_key]);
#endif
  return(TRUE);
}

/* 
   deletes a hash table entry. 
   
   parameters : 
   hash_table : hash table to use
   key : key to index the hash table
   key_len: length of the key in bytes
   
   returns:
   TRUE: if key was successfully deleted  
   FALSE: if key could not be deleted (key was not found)
				  
*/
static inline int32_t hash_delete( hash_table_t *hash_table, void *key, uint32_t key_len)
{
  uint32_t hash_key, hash_table_size;
   hash_entry_t *ptr, *prev_ptr;
  
  hash_table_size = hash_table->size;
  
  hash_key  = hash(key, key_len, 7) % hash_table_size;
 
#ifdef __USE_HASH_LOCKS__
  sem_wait(&(hash_table->row_lock[hash_key]));
#endif
  
  ptr = hash_table->row[hash_key];
  prev_ptr = NULL;

  while (ptr != NULL)
    {
      if (memcmp(ptr->key, key, key_len) == 0)
	{
	  if (prev_ptr == NULL) // First entry
	    hash_table->row[hash_key] = ptr->next;
	  else
	    prev_ptr->next = ptr->next;
	  
	  if (ptr->next == NULL) hash_table->tail[hash_key] = prev_ptr;
	  
	  free(ptr->key);
	  free(ptr);
#ifdef __USE_HASH_LOCKS__
	  sem_post(&hash_table->row_lock[hash_key]);
#endif
	  return(TRUE);
	}
      prev_ptr = ptr;
      ptr = ptr->next;
    }

#ifdef __USE_HASH_LOCKS__
  sem_post(&hash_table->row_lock[hash_key]);
#endif

  return(FALSE);
}


/*
  finds the entry corresponding to key in the hash table

  parameters:
  hash_table : pointer to the hash table to use
  key : key to index the hash table.
  key_len: length of the key in bytes
  
  returns: 
  pointer to the data field in the hash table on success
  NULL on failure
*/
static inline void *hash_find( hash_table_t *hash_table, void *key, uint32_t key_len)
{
  uint32_t hash_key, hash_table_size;
  hash_entry_t *ptr;
  

  hash_table_size = hash_table->size;

  hash_key  = hash(key, key_len, 7) % hash_table_size;

#ifdef __USE_HASH_LOCKS__
  sem_wait(&hash_table->row_lock[hash_key]);
#endif
 
  ptr = hash_table->row[hash_key];
  while (ptr != NULL)
    {
      if ((key_len == ptr->key_len) && (memcmp(ptr->key, key, key_len) == 0))
	{
#ifdef __USE_HASH_LOCKS__
	  sem_post(&hash_table->row_lock[hash_key]);
#endif
	  return(ptr->data);
	}
      ptr = ptr->next;
    }
#ifdef __USE_HASH_LOCKS__  
  sem_post(&hash_table->row_lock[hash_key]);
#endif
  return(NULL);
}

/*
  destroys the hash table and frees all allocated memory

  parameters:
  hash_table : pointer to the hash table to use
  
  returns : nothing
*/

static inline void destroy_hash_table( hash_table_t *hash_table)
{
  uint32_t t, count, max_count=0, tot_count=0, hash_table_size;
  hash_entry_t *cur_ptr, *tmp_ptr;

  hash_table_size = hash_table->size;
  
  for (t=0; t<hash_table_size; t++) 
    {
#ifdef __USE_HASH_LOCKS__
      sem_wait(&hash_table->row_lock[t]);
#endif
      if (hash_table->row[t] != NULL)
	{
	  cur_ptr = hash_table->row[t];
	  count = 1;
	  while (cur_ptr != NULL)
	    {
	      free(cur_ptr->key);
	      tmp_ptr = cur_ptr->next;
	      free(cur_ptr);
	      cur_ptr = tmp_ptr;
	      count++;
	    }
	  hash_table->row[t] = NULL;
	  tot_count += count;
	  if (count > max_count) max_count = count;
	}
#ifdef __USE_HASH_LOCKS__
      sem_post(&hash_table->row_lock[t]);
#endif
    }

  printf("Max collision list entries: %u. Total: %u\n", max_count, tot_count);
  free(hash_table->row);
  free(hash_table->tail);

#ifdef __USE_HASH_LOCKS__
  free(hash_table->row_lock);
#endif

  free(hash_table);
}

#endif
