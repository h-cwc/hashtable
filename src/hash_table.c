#include <stdlib.h>
#include <string.h>

#include "hash_table.h"

// Define intiialisation function for items. This function allocates a chunk of memory the size of a ht_item, and saves a copy of the strings k and v in the new chunk of memory. The function has been marked as static because it will only ever be called by the code internal to the hash table.
static ht_item* ht_new_item(const char* k, const char* v){
    ht_item* i = malloc(sizeof(ht_item));
    i -> key = strdup(k);
    i -> value = strdup(v);
    return i;
}

// Defining initialisation function for a new hash table.
static ht_hash_table* ht_new_sized(const int base_size) {
    ht_hash_table* ht = xmalloc(sizeof(ht_hash_table));
    ht -> base_size = base_size;

    // Defines how many items we can store.
    ht -> size = next_prime(ht -> base_size);
    ht -> count = 0;
    // Initialises array of items with calloc, filling the allocated memory with NULL. A NULL value indicates the entry is empty.
    ht -> items = xcalloc((size_t)ht -> size, sizeof(ht_item*)); 
    return ht;
}

#define HT_INITIAL_BASE_SIZE 10

ht_hash_table* ht_new() {
    return ht_new_sized(HT_INITIAL_BASE_SIZE);
}

static void ht_resize(ht_hash_table* ht, const int base_size) {
    if (base_size < HT_INITIAL_BASE_SIZE) {
        return;
    }
    ht_hash_table* new_ht = ht_new_sized(base_size);
    for (int i = 0; i < ht -> size; i++) {
        ht_item* item = ht -> items[i];
        if (item != NULL && item != &HT_DELETED_ITEM) {
            insert(new_ht, item -> key, item -> value);
        }
    }
    ht -> base_size = new_ht -> base_size;
    ht -> count = new_ht -> count;

    // To delete new_ht, we give it ht's size and items .
    const int tmp_size = ht -> size;
    ht -> size = new_ht -> size;
    new_ht -> size = tmp_size;

    ht_item** tmp_items = ht -> items;
    ht -> items = new_ht -> items;
    new_ht -> items = tmp_items;

    ht_del_hash_table(new_ht);
}

static void ht_resize_up(ht_hash_table* ht) {
    const int new_size = ht -> base_size * 2;
    ht_resize(ht, new_size);
}

static void ht_resize_down(ht_hash_table* ht) {
    const int new_size = ht -> base_size / 2;
    ht_resize(ht, new_size);
}

// Function to delete ht_item.
static void ht_del_item(ht_item* i){
    // Freeing allocatred memory to avoid memory leaks.
    free(i -> key);
    free(i -> value);
    free(i);
}

// Function to delete ht_hash_tables.
void ht_del_hash_table(ht_hash_table* ht){
    for(int i = 0; i < ht -> size; i++){
        ht_item* item = ht -> items[i];
        if(item != NULL){
            ht_del_item(item);
        }
    }
    // Freeing allocatred memory to avoid memory leaks.
    free(ht -> items);
    free(ht);
}

/* The hash function will:
* 1. Takes input string and returns a number between 0 and m, which is the desired bucket length.
* 2. Return an even distribution of bucket indexes for an average set of inputs.
*/
static int ht_hash(const char* s, const int a, const int m) {
    long hash = 0;
    const int len_s = strlen(s);
    for (int i = 0; i < len_s; i++) {
        hash += (long)pow(a, len_s - (i+1))*s[i];
        hash = hash % m;
    }
    return (int)hash;
}

// Handling collisions
// If our hash function is unevenly distributed (i.e. infinitely large number of inputs to finite number of outputs), it will put more items in some buckets than others. This will lead to a higher rate of collisions, reducing the efficiency of the hash table. Here, we will handle collisions using open addressing with double hashing. We will make use of two hash functions to calculate the index an item should be stored at after 'i' collisions.
#define HT_PRIME_1 151
#define HT_PRIME_2 163

static int ht_get_hash(
    const char* s, const int num_buckets, const int attempt
) {
    const int hash_a = ht_hash(s, HT_PRIME_1, num_buckets);
    const int hash_b = ht_hash(s, HT_PRIME_2, num_buckets);
    return (hash_a + (attempt * (hash_b + 1))) % num_buckets;
}

// Insert function
void insert(ht_hash_table* ht, const char* key, const char* value) {
    const int load = ht -> count * 100 / ht -> size;
    if (load > 70) {
        ht_resize_up(ht);
    }
    ht_item* item = ht_new_item(key, value);
    int index = ht_get_hash(item -> key, ht -> size, 0);
    ht_item* cur_item = ht -> items[index];
    int i = 1;
    while (cur_item != NULL && cur_item != &HT_DELETED_ITEM) {
        if (cur_item != &HT_DELETED_ITEM) {
            if (strcmp(cur_item -> key, key) == 0) {
                ht_del_item(cur_item);
                ht -> items[index] = item;
                return;
            }
        }
        index = ht_get_hash(item -> key, ht -> size, i);
        cur_item = ht -> items[index];
        i++;
    } 
    ht -> items[index] = item;
    ht -> count++;
}

// Search function
char* search(ht_hash_table* ht, const char* key) {
    int index = ht_get_has(key, ht -> size, 0);
    ht_item* item = ht -> items[index];
    int i = 1;
    
    while (item != NULL) {
        if (item != &HT_DELETED_ITEM) {
            if (strcmp(item -> key, key) == 0) {
            return item -> value;
            }
        }
        index = ht_get_hash(key, ht -> size, i);
        item = ht -> items[index];
        i++; 
    }
    return NULL;
}

// Delete function
static ht_item HT_DELETED_ITEM = {NULL, NULL};

void delete(ht_hash_table* ht, const char* key) {
    const int load = ht -> count * 100 / ht -> size;
    if (load < 10) {
        ht_resize_down(ht);
    }
    int index = ht_get_hash(key, ht -> size, 0);
    ht_item* item = ht -> items[index];
    int i = 1;

    while (item != NULL) {
        if (item != &HT_DELETED_ITEM) {
            if (strcmp(item -> key, key) == 0) {
                ht_del_item(item); 
                ht -> items[index] = &HT_DELETED_ITEM;
            } 
        }
        index = ht_get_hash(key, ht -> size, i);
        item = ht -> items[index];
        i++;
    }
    ht -> count--;
}