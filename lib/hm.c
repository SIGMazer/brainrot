#include "../ast.h"
#include "hm.h"

/**
 * @brief Computes FNV-1a hash of the given data
 * @param key Pointer to the data to hash
 * @param len Length of the data in bytes
 * @return size_t Hash value
 *
 * Implements the FNV-1a non-cryptographic hash algorithm.
 * Uses the FNV offset basis of 2166136261 and FNV prime of 16777619.
 */
size_t fnv1a_hash(const void *key, size_t len)
{
    const unsigned char *p = key;
    size_t h = 2166136261u;
    for (size_t i = 0; i < len; i++)
    {
        h ^= p[i];
        h *= 16777619;
    }
    return h;
}

/**
 * @brief Compares two memory regions for equality
 * @param a Pointer to first memory region
 * @param b Pointer to second memory region
 * @param len Length of memory regions to compare in bytes
 * @return true if memory regions are identical, false otherwise
 *
 * Performs a byte-by-byte comparison of two memory regions.
 * Used for key comparison in the hashmap.
 */
bool key_equal(const void *a, const void *b, size_t len)
{
    return memcmp(a, b, len) == 0;
}

/**
 * @brief Creates a new empty hashmap
 * @return HashMap* Pointer to newly allocated hashmap
 *
 * Allocates and initializes a new hashmap with default initial capacity.
 * The nodes array is zero-initialized.
 * Caller is responsible for freeing the returned hashmap using hm_free().
 */
HashMap *hm_new()
{
    HashMap *hm = malloc(sizeof(HashMap));
    hm->capacity = INIT_CAPACITY;
    hm->size = 0;
    hm->nodes = calloc(hm->capacity, sizeof(HashMapNode *));
    return hm;
}

/**
 * @brief Doubles the capacity of the hashmap
 * @param hm Pointer to hashmap to resize
 *
 * Allocates a new nodes array with double capacity and rehashes
 * all existing entries into the new array.
 * Used internally when load factor threshold is exceeded.
 */
void hm_resize(HashMap *hm)
{
    size_t new_capacity = hm->capacity * 2;
    HashMapNode **new_nodes = calloc(new_capacity, sizeof(HashMapNode *));
    for (size_t i = 0; i < hm->capacity; i++)
    {
        HashMapNode *node = hm->nodes[i];
        while (node)
        {
            size_t index = fnv1a_hash(node->key, node->key_size) % new_capacity;
            while (new_nodes[index])
            {
                index = (index + 1) % new_capacity;
            }
            new_nodes[index] = node;
        }
    }
    free(hm->nodes);
    hm->nodes = new_nodes;
    hm->capacity = new_capacity;
}

/**
 * @brief Debug function to print hashmap contents
 * @param hm Pointer to hashmap to dump
 *
 * Prints key, variable name, and array flag for each entry.
 * Assumes values are Variable structs.
 * For debugging purposes only.
 */
void dump(HashMap *hm)
{
    for (size_t i = 0; i < hm->capacity; i++)
    {
        if (hm->nodes[i])
        {
            Variable *v = (Variable *)hm->nodes[i]->value;
            printf("key: %s, value: %s, is_array: %s\n", hm->nodes[i]->key, v->name, v->is_array ? "true" : "false");
        }
    }
}

/**
 * @brief Retrieves a value from the hashmap
 * @param hm Pointer to the hashmap
 * @param key Pointer to the key
 * @param key_size Size of the key in bytes
 * @return void* Pointer to the value if found, NULL if not found
 *
 * Uses linear probing to handle collisions.
 * Returns NULL if key is not found.
 */
void *hm_get(HashMap *hm, const void *key, size_t key_size)
{
    size_t index = fnv1a_hash(key, key_size) % hm->capacity;
    while (hm->nodes[index])
    {
        HashMapNode *node = hm->nodes[index];
        if (key_equal(node->key, key, key_size))
        {
            return node->value;
        }
        index = (index + 1) % hm->capacity;
    }

    return NULL;
}

/**
 * @brief Inserts or updates a key-value pair in the hashmap
 * @param hm Pointer to the hashmap
 * @param key Pointer to the key
 * @param key_size Size of the key in bytes
 * @param value Pointer to the value
 * @param value_size Size of the value in bytes
 *
 * Creates a deep copy of both key and value.
 * Updates existing value if key already exists.
 * Resizes hashmap if load factor threshold would be exceeded.
 * Uses linear probing to handle collisions.
 */
void hm_put(HashMap *hm, void *key, size_t key_size, void *value, size_t value_size)
{
    if (hm->size >= hm->capacity * LOAD_FACTOR)
    {
        hm_resize(hm);
    }
    Variable *v = (Variable *)value;
    size_t index = fnv1a_hash(key, key_size) % hm->capacity;
    while (hm->nodes[index])
    {
        HashMapNode *node = hm->nodes[index];
        if (key_equal(node->key, key, key_size))
        {
            memcpy(node->value, value, value_size);
            return;
        }
        index = (index + 1) % hm->capacity;
    }
    HashMapNode *node = malloc(sizeof(HashMapNode));
    node->key_size = key_size;
    node->value_size = value_size;
    node->key = malloc(key_size);
    node->value = malloc(value_size);
    memcpy(node->value, value, value_size);
    memcpy(node->key, key, key_size);

    hm->nodes[index] = node;
    hm->size++;
}

/**
 * @brief Frees all memory associated with a hashmap
 * @param hm Pointer to hashmap to free
 *
 * Frees all allocated node keys and values,
 * then frees the nodes array and hashmap struct itself.
 * After calling, the hashmap pointer should not be used.
 */
void hm_free(HashMap *hm)
{
    for (size_t i = 0; i < hm->capacity; i++)
    {
        if (hm->nodes[i])
        {
            free(hm->nodes[i]->key);
            free(hm->nodes[i]->value);
            free(hm->nodes[i]);
        }
    }
    free(hm->nodes);
    free(hm);
}
