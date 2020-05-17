#include "include/heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
    int i;

    heap_t *heap = malloc(sizeof(heap_t));
    memset(heap, 0, sizeof(heap_t));

    void *region = malloc(HEAP_INIT_SIZE);
    memset(region, 0, HEAP_INIT_SIZE);
    
    for (i = 0; i < BIN_COUNT; i++) {
        heap->bins[i] = malloc(sizeof(bin_t));
        memset(heap->bins[i], 0, sizeof(bin_t));
    }

    init_heap(heap, (long) region);
    
    printf("overhead = %d \n", overhead);

    void *a = heap_alloc(heap, 8);
    printf("a = %p size: 8 \n", a);
    void *b = heap_alloc(heap, 128);
    printf("b = %p size: 128 \n", b);
    void *c = heap_alloc(heap, 8);
    printf("c = %p size: 8 \n", c);

    printf("\nfreeing b \n");
    heap_free(heap, b);

    void* d = heap_alloc(heap, 8);
    printf("d = %p size: 8 \n", d);

    void* e = heap_alloc(heap, 16);
    printf("e = %p size: 16 \n", e);
    
    void* f = heap_alloc(heap, 8);
    printf("f = %p size: 8 \n", f);

    void* g = heap_alloc(heap, 8);
    printf("g = %p size: 8 \n", g);

    printf("\nfreeing d & f \n");
    heap_free(heap, d);
    heap_free(heap, f);
    
    printf("\nfreeing e\n");
    heap_free(heap, e);

    void* h = heap_alloc(heap, 128);
    printf("h = %p size: 128 \n", h);
    printf("\n");

    for (i = 1; i <= 2048; i += i) printf("size: %d -> bin: %d \n", i, get_bin_index(i));

    for (i = 0; i < BIN_COUNT; i++) {
        free(heap->bins[i]);
    }

    free(heap);
    free(region);
}
