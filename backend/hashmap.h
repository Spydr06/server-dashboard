#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>

typedef struct hashmap_pair hashmap_pair_t;

typedef struct {
    size_t size;
    size_t alloc;
    hashmap_pair_t* pairs;
} hashmap_t;

void hashmap_init(hashmap_t* hashmap);
void hashmap_free(hashmap_t* hashmap);

int hashmap_put(hashmap_t* hashmap, const char* key, void* value);
void* hashmap_get(hashmap_t* hashmap, const char* key);

#endif /* HASHMAP_H */

