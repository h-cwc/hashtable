// Defining struct to store the key-value pairs.
typedef struct{
    char* key;
    char* value;
} ht_item;

// Defining hash table to store an array of pointers to the items and details on its size and how full it is.
typedef struct {
    int base_size;
    int size;
    int count;
    ht_item** items;
} ht_hash_table;

// The hash function implements the following API.
void insert(ht_hash_table* ht, const char* key, const char* value);
char* search(ht_hash_table* ht, const char* key);
void delete(ht_hash_table* h, const char* key);