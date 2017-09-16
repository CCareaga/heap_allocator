#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>
#include <stddef.h>

#define HEAP_INIT_SIZE 0x10000
#define HEAP_MAX_SIZE 0xF0000
#define HEAP_MIN_SIZE 0x10000

#define MIN_ALLOC_SZ 4

#define MIN_WILDERNESS 0x2000
#define MAX_WILDERNESS 0x1000000

#define BIN_COUNT 9
#define BIN_MAX_IDX (BIN_COUNT - 1)

typedef struct node_t {
    uint8_t hole;
    uint32_t size;
    struct node_t* next;
    struct node_t* prev;
} node_t;

typedef struct { 
    node_t *header;
} footer_t;

typedef struct {
    node_t* head;
} bin_t;

typedef struct {
    uint32_t start;
    uint32_t end;
    bin_t *bins[BIN_COUNT];
} heap_t;

static uint32_t overhead = sizeof(footer_t) + sizeof(node_t);

void init_heap(heap_t *heap, uint32_t start);

void *heap_alloc(heap_t *heap, size_t size);
void heap_free(heap_t *heap, void *p);
uint8_t expand(heap_t *heap, size_t sz);
void contract(heap_t *heap, size_t sz);

uint32_t get_bin_index(size_t sz);
void create_foot(node_t *head);
footer_t *get_foot(node_t *head);

node_t *get_wilderness(heap_t *heap);

#endif
