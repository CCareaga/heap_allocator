#include "include/heap.h"
#include <stdio.h>

int main(int argc, char** argv) {
    heap_t *heap = malloc(sizeof(heap_t));
    char *region = malloc(HEAP_INIT_SIZE);
    
    for (int i = 0; i < BIN_COUNT; i++) {
        heap->bins[i] = malloc(sizeof(bin_t));
    }

    init_heap(heap, region, region + HEAP_INIT_SIZE, HEAP_MAX_SIZE);
    
    for (int j = 0; j < 10; j++) {
        int *p = alloc(heap, 20);
        printf("alloc'd: %p \n", p);
    }
}
