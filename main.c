#include "include/heap.h"
#include <stdio.h>

int main(int argc, char** argv) {
    heap_t *heap = malloc(sizeof(heap_t));
    void *region = malloc(HEAP_INIT_SIZE);
    
    for (int i = 0; i < BIN_COUNT; i++) {
        heap->bins[i] = malloc(sizeof(bin_t));
    }

    init_heap(heap, region, region + HEAP_INIT_SIZE, HEAP_MAX_SIZE);
    
    printf("overhead = %d \n", overhead);

    void *a = alloc(heap, 8);
    printf("a = %d size: 8 \n", a);
    void *b = alloc(heap, 128);
    printf("b = %d size: 128 \n", b);
    void *c = alloc(heap, 8);
    printf("c = %d size: 8 \n", c);

    printf("\nfreeing b \n");
    free(heap, b);

    void* d = alloc(heap, 16);
    printf("d = %d size: 4 \n", d);

    void* e = alloc(heap, 8);
    printf("e = %d size: 4 \n", e);
    
    void* f = alloc(heap, 16);
    printf("f = %d size: 4 \n", f);

    void* g = alloc(heap, 8);
    printf("g = %d size: 8 \n", g);

    printf("\nfreeing d & f \n");
    free(heap, d);
    free(heap, f);
    
    printf("\nfreeing e\n");
    free(heap, e);

    void* h = alloc(heap, 128);
    printf("h = %d size: 128 \n", h);

    int i;
    for (i = 1; i <= 2048; i += i) printf("size: %d -> bin: %d \n", i, get_bin_index(i));
}
