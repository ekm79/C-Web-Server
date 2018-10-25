#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "cache.h"

/**
 * Allocate a cache entry
 */
struct cache_entry *alloc_entry(char *path, char *content_type, void *content, int content_length)
{
    struct cache_entry *new_entry = malloc(sizeof(struct cache_entry));
    new_entry->content = content;
    new_entry->next = NULL;
    new_entry->prev = NULL;
    new_entry->path = path;
    new_entry->content_type = content_type;
    new_entry->content_length = content_length;

}

/**
 * Deallocate a cache entry
 */
void free_entry(struct cache_entry *entry)
{
    free(entry->content);
    free(entry);

}

/**
 * Insert a cache entry at the head of the linked list
 */
void dllist_insert_head(struct cache *cache, struct cache_entry *ce)
{
    // Insert at the head of the list
    if (cache->head == NULL) {
        cache->head = cache->tail = ce;
        ce->prev = ce->next = NULL;
    } else {
        cache->head->prev = ce;
        ce->next = cache->head;
        ce->prev = NULL;
        cache->head = ce;
    }
}

/**
 * Move a cache entry to the head of the list
 */
void dllist_move_to_head(struct cache *cache, struct cache_entry *ce)
{
    if (ce != cache->head) {
        if (ce == cache->tail) {
            // We're the tail
            cache->tail = ce->prev;
            cache->tail->next = NULL;

        } else {
            // We're neither the head nor the tail
            ce->prev->next = ce->next;
            ce->next->prev = ce->prev;
        }

        ce->next = cache->head;
        cache->head->prev = ce;
        ce->prev = NULL;
        cache->head = ce;
    }
}


/**
 * Removes the tail from the list and returns it
 * 
 * NOTE: does not deallocate the tail
 */
struct cache_entry *dllist_remove_tail(struct cache *cache)
{
    struct cache_entry *oldtail = cache->tail;

    cache->tail = oldtail->prev;
    cache->tail->next = NULL;

    cache->cur_size--;

    return oldtail;
}

/**
 * Create a new cache
 * 
 * max_size: maximum number of entries in the cache
 * hashsize: hashtable size (0 for default)
 */
struct cache *cache_create(int max_size, int hashsize)
{
    struct cache *new_cache = malloc(sizeof(struct cache));
    new_cache->index = 0;
    new_cache->cur_size = hashsize;
    new_cache->max_size = max_size;
    new_cache->head = NULL;
    new_cache->tail = NULL;
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
}

/**
 * Store an entry in the cache
 *
 * This will also remove the least-recently-used items as necessary.
 * 
 * NOTE: doesn't check for duplicate cache entries
 */
void cache_put(struct cache *cache, char *path, char *content_type, void *content, int content_length)
{
    struct cache_entry *entry = alloc_entry(path, content_type, content, content_length);
    dllist_insert_head(cache, entry);
    hashtable_put(cache->index, entry->path, entry);
    cache->cur_size++;
    if(cache->cur_size > cache->max_size) {
        hashtable_delete(cache->index, entry->path);
        dllist_remove_tail(cache);
        free_entry(entry);

    }


}

/**
 * Retrieve an entry from the cache
 */
struct cache_entry *cache_get(struct cache *cache, char *path)
{
    struct cache_entry *found = hashtable_get(cache->index, path);
    if(!found) {
        return NULL;
    }
    dllist_move_to_head(cache, found);
    return found;

}
