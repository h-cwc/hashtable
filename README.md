A hash table (also called a hash map, map, hash, or dictionary) is an array of 'buckets', each of which stores a key-value pair. In order to find the location of the bucket where a key-value pair should be stored, the key is passed through a hashing function. The function returns an integer, which is used as the pair's index in the array of buckets. To retrieve a key-value pair, we supply the key to the same hashing function, receiving its index and used it find it in the array.

Our hash table here maps string kyes to string values, but the principals used here are applicable to hash tables that map arbitrary key types to arbitrary value types. Only ASCII strings are supported here.

## API
We make use of associative arrays, which are a collection of unordered key-value pairs. Duplicate keys
are not permitted. The following operations are supported:

- `insert(a, k, v)`: store pair `k:v` in associative array `a`.
- `search(a, k)`: return value `v` associated with key `k` from associative array `a`, or `NULL` if the key does not exist.
- `delete(a, k)`: delete the `k:v` pair associated with `k`, or do nothing if
  `k` does not exist.

Here, our associate array is an abstract data structure that implements our API described above.

## Code Structure
Our code is laid out in the following directory structure.
```
.
├── build
└── src
    ├── hash_table.c
    ├── hash_table.h
    ├── prime.c
    └── prime.h
```
The `src` folder will contain our code, while `build` will contain our compiled binaries.