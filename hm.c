#include "ast.h"

#define INIT_CAPACITY 64
#define LOAD_FACTOR 0.75


size_t fnv1a_hash(const void *key, size_t len) 
{
    const unsigned char *p = key;
    size_t h = 2166136261u;
    for (size_t i = 0; i < len; i++) {
        h ^= p[i];
        h *= 16777619;
    }
    return h;
}

bool key_equal(const void *a, const void *b, size_t len)
{
    return memcmp(a, b, len) == 0;
}

HashMap* hm_new() 
{
    HashMap *hm = malloc(sizeof(HashMap));
    hm->capacity = INIT_CAPACITY;
    hm->size = 0;
    hm->nodes = calloc(hm->capacity, sizeof(HashMapNode*));
    return hm;
}

void hm_resize(HashMap *hm) 
{
    size_t new_capacity = hm->capacity * 2;
    HashMapNode **new_nodes = calloc(new_capacity, sizeof(HashMapNode*));
    for (size_t i = 0; i < hm->capacity; i++) {
        HashMapNode *node = hm->nodes[i];
        while (node) {
            size_t index = fnv1a_hash(node->key, node->key_size) % new_capacity;
            while (new_nodes[index]) {
                index = (index + 1) % new_capacity;
            }
            new_nodes[index] = node;
        }
    }
    free(hm->nodes);
    hm->nodes = new_nodes;
    hm->capacity = new_capacity;
}

/* For debug */
void dump(HashMap *hm) 
{
    for (size_t i = 0; i < hm->capacity; i++) {
        if (hm->nodes[i]) {
            Variable *v = (Variable *)hm->nodes[i]->value;
            printf("key: %s, value: %s\n", hm->nodes[i]->key, v->name);
        }
    }
}

void* hm_get(HashMap *hm, const void *key, size_t key_size) 
{
    size_t index = fnv1a_hash(key, key_size) % hm->capacity;
    while (hm->nodes[index]) {
        HashMapNode *node = hm->nodes[index];
        if (key_equal(node->key, key, key_size)) {
            return node->value;
        }
        index = (index + 1) % hm->capacity;
    }

    return NULL;
}

void hm_put(HashMap *hm, void *key, size_t key_size, void *value, size_t value_size) 
{
    if (hm->size >= hm->capacity * LOAD_FACTOR) {
        hm_resize(hm);
    }
    size_t index = fnv1a_hash(key, key_size) % hm->capacity;
    while (hm->nodes[index]) {
        HashMapNode *node = hm->nodes[index];
        if (key_equal(node->key, key, key_size)) {
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

void hm_free(HashMap *hm)
{
    for (size_t i = 0; i < hm->capacity; i++) {
        if (hm->nodes[i]) {
            free(hm->nodes[i]->key);
            free(hm->nodes[i]->value);
            free(hm->nodes[i]);
        }
    }
    free(hm->nodes);
    free(hm);
}

