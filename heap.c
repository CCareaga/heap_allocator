#include "include/heap.h"

uint32_t offset = sizeof(node_t) - 8;

void init_heap(heap_t *heap, uint32_t start, uint32_t end, uint32_t max) {
    node_t *init_region = (node_t *) start;
    init_region->hole = 1;
    init_region->size = (end - start) - sizeof(node_t) - sizeof(footer_t);

    create_foot(init_region);

    add_node(heap->bins[get_bin_index(init_region->size)], init_region);

    heap->start = start;
    heap->end = end;
    heap->max = max;
}

void *alloc(heap_t *heap, size_t size) {

    uint32_t index = get_bin_index(size);
    bin_t *temp = (bin_t *) heap->bins[index];
    node_t *found = get_best_fit(temp, size);

    while (found == NULL) {
        temp = heap->bins[++index];
        found = get_best_fit(temp, size);
    }

    if ((found->size - size) > 4) {
        node_t *split = ((char *) found + sizeof(node_t) + sizeof(footer_t)) + size; 
        split->size = found->size - size - sizeof(node_t) - sizeof(footer_t);
        split->hole = 1;
   
        create_foot(split);

        uint32_t new_idx = get_bin_index(split->size);

        add_node(heap->bins[new_idx], split); 

        found->size = size; 
        create_foot(found); 
    }

    found->hole = 0; 
    remove_node(heap->bins[index], found); 
    
    node_t *wild = get_wilderness(heap);
    if (wild->size < MIN_WILDERNESS) {
        uint8_t success = expand(heap, 0x1000);
        if (success == 0) {
            return NULL;
        }
    }
    else if (wild->size > MAX_WILDERNESS) {
        contract(heap, 0x1000);
    }

    found->prev = NULL;
    found->next = NULL;
    return &found->next; 
}

void free(heap_t *heap, void *p) {
    bin_t *list;
    footer_t *new_foot, *old_foot;

    node_t *head = (node_t *) ((char *) p - offset);
    if (head == heap->start) {
        head->hole = 1; 
        add_node(heap->bins[get_bin_index(head->size)], head);
        return;
    }

    node_t *next = (node_t *) ((char *) get_foot(head) + sizeof(footer_t));
    node_t *prev = (node_t *) * ((uint32_t *) ((char *) head - sizeof(footer_t)));
    
    if (prev->hole) {
        list = heap->bins[get_bin_index(prev->size)];
        remove_node(list, prev);

        prev->size += sizeof(footer_t) + sizeof(node_t) + head->size;
        new_foot = get_foot(head);
        new_foot->header = prev;

        head = prev;
    }

    if (next->hole) {
        list = heap->bins[get_bin_index(next->size)];
        remove_node(list, next);

        head->size += sizeof(node_t) + next->size;

        old_foot = get_foot(next);
        old_foot->header = 0;
        next->size = 0;
        next->hole = 0;
        
        new_foot = get_foot(head);
        new_foot->header = head;
    }

    head->hole = 1;
    add_node(heap->bins[get_bin_index(head->size)], head);
}

uint8_t expand(heap_t *heap, size_t sz) {

}

void contract(heap_t *heap, size_t sz) {

}

uint32_t get_bin_index(size_t sz) {
    uint32_t index = 0;
    sz = sz < 4 ? 4 : sz;

    while (sz >>= 1) index++; 
    index -= 2; 
    
    if (index > 8) index = 8; 
    return index;
}

void create_foot(node_t *head) {
    footer_t *foot = get_foot(head);
    foot->header = head;
}

footer_t *get_foot(node_t *node) {
    return (footer_t *) ((char *) node + sizeof(node_t) + node->size);
}

node_t *get_wilderness(heap_t *heap) {
    footer_t *wild_foot = (footer_t *) ((char *) heap->end - sizeof(footer_t));
    return wild_foot->header;
}
