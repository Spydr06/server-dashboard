#include "hashmap.h"

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#define INIT_SIZE 128
#define HASHMAP_SIZE(size) (1 << ((sizeof(uint64_t) << 3) - __builtin_clzl(((uint64_t)(size)) - 1)))

struct hashmap_pair {
    const char* key;
    void* value;
};

void hashmap_init(hashmap_t* hashmap) {
    size_t size = HASHMAP_SIZE(INIT_SIZE); 
    
    hashmap->size = 0;
    hashmap->alloc = size;
    hashmap->pairs = calloc(hashmap->alloc, sizeof(hashmap_pair_t));
}

void hashmap_free(hashmap_t* hashmap) {
    free(hashmap->pairs);
}

static inline size_t hashmap_calc_size(hashmap_t* hashmap) {
    size_t map_size = hashmap->size + (hashmap->size / 3);
    return map_size < hashmap->alloc ? hashmap->alloc : HASHMAP_SIZE(map_size);
}

static inline void hashmap_rehash(hashmap_t* hashmap) {

}

int hashmap_put(hashmap_t* hashmap, const char* key, void* value) {
    if(!key || !hashmap)
        return errno = EINVAL;

    size_t size = hashmap_calc_size(hashmap);
    if(size > hashmap->size)
        hashmap_rehash(hashmap);

}

void* hashmap_get(hashmap_t* hashmap, const char* key) {

}


