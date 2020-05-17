#include "include/heap.h"

#define USER_POINTER_OFFSET     (2 * sizeof(int))

// ========================================================
// this function initializes a new heap structure, provided
// an empty heap struct, and a place to start the heap
//
// NOTE: this function uses HEAP_INIT_SIZE to determine
// how large the heap is so make sure the same constant
// is used when allocating memory for your heap!
// ========================================================
void init_heap(heap_t *heap, long start) {
    // first we create the initial region, this is the "wilderness" chunk
    // the heap starts as just one big chunk of allocatable memory
    node_t *init_region = (node_t *) start;
    init_region->hole = 1;
    init_region->size = (HEAP_INIT_SIZE) - sizeof(node_t) - sizeof(footer_t);

    create_foot(init_region); // create a foot (size must be defined)
    
    // now we add the region to the correct bin and setup the heap struct
    add_node(heap->bins[get_bin_index(init_region->size)], init_region); 

    heap->start = start;
    heap->end = start + HEAP_INIT_SIZE;
}

// ========================================================
// this is the allocation function of the heap, it takes
// the heap struct pointer and the size of the chunk we 
// want. this function will search through the bins until 
// it finds a suitable chunk. it will then split the chunk
// if neccesary and return the start of the chunk
// ========================================================
void *heap_alloc(heap_t *heap, size_t size) {
    uint index      = 0;
    bin_t *temp     = NULL;
    node_t *found   = NULL;

    // do alignment first
    if (ALIGN_BTYES != 1) {
        size = ((size + ALIGN_BTYES - 1) / ALIGN_BTYES) * ALIGN_BTYES;
    }

    // first get the bin index that this chunk size should be in
    index   = get_bin_index(size);
    // now use this bin to try and find a good fitting chunk!
    temp    = (bin_t *) heap->bins[index];
    found   = get_best_fit(temp, size);
    
    // while no chunk if found advance through the bins until we
    // find a chunk or get to the wilderness
    while (found == NULL) {
        if (index + 1 >= BIN_COUNT)
            return NULL;

        temp = heap->bins[++index];
        found = get_best_fit(temp, size);
    }

    // if the differnce between the found chunk and the requested chunk
    // is bigger than the overhead (metadata size) + the min alloc size
    // then we should split this chunk, otherwise just return the chunk
    if ((found->size - size) > (overhead + MIN_ALLOC_SZ)) {
        // do the math to get where to split at, then set its metadata
        node_t *split = ((char *) found + overhead) + size; 
        split->size = found->size - size - (overhead);
        split->hole = 1;
   
        create_foot(split); // create a footer for the split
    
        // now we need to get the new index for this split chunk
        // place it in the correct bin
        int new_idx = get_bin_index(split->size); 
        add_node(heap->bins[new_idx], split); 
    
        found->size = size; // set the found chunks size
        create_foot(found); // since size changed, remake foot
    }

    found->hole = 0; // not a hole anymore
    remove_node(heap->bins[index], found); // remove it from its bin
    
    // these following lines are checks to determine if the heap should
    // be expanded or contracted
    // ==========================================
    node_t *wild = get_wilderness(heap);
    if (wild->size < MIN_WILDERNESS) {
        int success = expand(heap, 0x1000);
        if (success == 0) {
            return NULL;
        }
    }
    else if (wild->size > MAX_WILDERNESS) {
        contract(heap, 0x1000);
    }
    // ==========================================
    
    // since we don't need the prev and next fields when the chunk
    // is in use by the user, we can clear these and return the
    // address of the next field
    found->prev = NULL;
    found->next = NULL;
    return &found->next; 
}

// ========================================================
// this is the free function of the heap, it takes the 
// heap struct pointer and the pointer provided by the
// heap_alloc function. the given chunk will be possibly
// coalesced  and then placed in the correct bin
// ========================================================
void heap_free(heap_t *heap, void *p) {
    bin_t *list         = NULL;
    footer_t *new_foot  = NULL;
    footer_t *old_foot  = NULL;
    footer_t *f         = NULL;
    node_t *prev        = NULL;
    int has_next        = 0;
    int has_prev        = 0;
    
    // the actual head of the node is not p, it is p minus the size
    // of the fields that precede "next" in the node structure
    // if the node being free is the start of the heap, it would has
    // no previous node to coalesce
    node_t *head = (node_t *) ((char *) p - USER_POINTER_OFFSET);
    if (head != (node_t *)(uintptr_t) heap->start) {
        has_prev = 1;
        f = (footer_t *) ((char *) head - sizeof(footer_t));
        prev = f->header;
    }
    
    // if the node being free is the end of the heap, it would has
    // no next node to coalesce
    node_t *next = (node_t *) ((char *) get_foot(head) + sizeof(footer_t));
    if (next != (node_t *) (uintptr_t)heap->end) {
        has_next = 1;
    }
    
    // if it has the previous node and the previous node is a hole we can coalese!
    if (has_prev && prev->hole) {
        // remove the previous node from its bin
        list = heap->bins[get_bin_index(prev->size)];
        remove_node(list, prev);
        
        // re-calculate the size of thie node and recreate a footer
        prev->size += overhead + head->size;
        create_foot(prev); 
        
        // previous is now the node we are working with, we head to prev
        // because the next if statement will coalesce with the next node
        // and we want that statement to work even when we coalesce with prev
        head = prev; 
    }
    
    // if it has the next node and the next node is free coalesce!
    if (has_next && next->hole) {
        // remove it from its bin
        list = heap->bins[get_bin_index(next->size)];
        remove_node(list, next);
    
        // re-calculate the new size of head
        head->size += overhead + next->size;
        
        // clear out the old metadata from next
        old_foot = get_foot(next);
        old_foot->header = 0;
        next->size = 0;
        next->hole = 0;
        
        // make the new footer!
        create_foot(head); 
    }
    
    // this chunk is now a hole, so put it in the right bin!
    head->hole = 1;
    add_node(heap->bins[get_bin_index(head->size)], head);
}

// these are left here to implement contraction / expansion
uint expand(heap_t *heap, size_t sz) {

}

void contract(heap_t *heap, size_t sz) {

}
// ========================================================
// this function is the hashing function that converts
// size => bin index. changing this function will change 
// the binning policy of the heap. right now it just 
// places any allocation < 8 in bin 0 and then for anything
// above 8 it bins using the log base 2 of the size
// ========================================================
uint get_bin_index(size_t sz) {
    int index = 0;
    sz = sz < 4 ? 4 : sz;

    while (sz >>= 1) index++; 
    index -= 2; 
    
    if (index > BIN_MAX_IDX) index = BIN_MAX_IDX; 
    return index;
}


// ========================================================
// this function will create a footer given a node
// the node's size must be set to the correct value!
// ========================================================
void create_foot(node_t *head) {
    footer_t *foot = get_foot(head);
    foot->header = head;
}

// ========================================================
// this function will get the footer pointer given a node
// ========================================================
footer_t *get_foot(node_t *node) {
    return (footer_t *) ((char *) node + sizeof(node_t) + node->size);
}

// ========================================================
// this function will get the wilderness node given a 
// heap struct pointer
//
// NOTE: this function banks on the heap's end field being
// correct, it simply uses the footer at the end of the 
// heap because that is always the wilderness
// ========================================================
node_t *get_wilderness(heap_t *heap) {
    footer_t *wild_foot = (footer_t *) ((char *) heap->end - sizeof(footer_t));
    return wild_foot->header;
}
