# SHMALL - Simple Heap Memory ALLocator
------------
This is a simple heap allocator I wrote for a hobby OS I have been working on. I wrote this allocator with very little research, and it is made to be as easy to understand as possible. I think this repository can be useful to those who are new to OS development (like myself), or are interested in seeing a simplified version of functions like malloc and free.

---
![SHAMLL](SHMALL.png "SHMALL")
---

### Features
------------
  - Binning uses doubly-linked lists based on size.
  - Coalescing freed chunks.
  - Quick Best-fit due to sorting of free lists.
  - Easy expansion and contraction.
  - Very small (about 230 lines, heap and linked-list)

### Compiling
------------
There are two header files that define the heap and the linked list.
to compile this repository:
``` 
$ gcc main.c llist.c heap.c -o heap_test 
$ ./heap_test
```

this will run a demo of the allocator and print out some information.


### Explanation
------------

##### Initialization:
In order to initialize this heap, a section of memory must be provided. In this repository, that memory is supplied by ```malloc``` (yes, allocating a heap via heap). In an OS setting some pages would need to be mapped and supplied to the heap (one scenario). Note that the bins in the ```heap_t``` struct also need memory allocated for them.

When the function init_heap is called the address of the empty heap struct (with allocated bin pointers) must be provided. The init_heap function will then create one large chunk with header (```node_t``` struct) and a footer (```footer_t``` struct). To determine the size of this chunk the function uses the constant ```HEAP_INIT_SIZE```. It will add this to the ```start``` argument in order to determine where the heap ends.

##### Metadata and Design:
Each chunk of memory has a node struct at the begining and a footer struct at the end. The node holds size, whether the chunk is free or not, and two pointers used in the doubly-linked list (next and prev). The footer struct simply holds a pointer to the header (used while freeing adjacent chunks). The chunk at the end of the heap is called the "wilderness" chunk. It is the largest chunk and its min and max sizes are defined in heap.h. contracting and expanding the heap is as easy as resizing this wilderness chunk. Free chunks of memory are stored in "bins" each bin is actually just a doubly-linked lists of nodes with similar sizes. The heap structure holds a defined number of bins (```BIN_COUNT``` in heap.h). To determine which bin to place a chunk, the size of the chunk is mapped to a bin index by the function ```get_bin_index```. This consistent binning function will ensure that chunks can be accesed and stored in defined fashion. Chunks are sorted as they are inserted into the bins so chunk insertion is not O(1) but this makes it much easier to find chunks that have the best fit. Note, the binning function can be defined however the user of this heap feels fit. it  may be beneficial to determine a more sophisticated binning function in order to aid the quick retrieval of chunks.

##### Allocation:
The function ```heap_alloc``` takes the address of the heap struct to allocate from and a size. The function simply uses ```get_bin_index``` to determine where a chunk of this size SHOULD be, of course there may not be a chunk of that size. If no chunks are found in the corresponding bin then the next bin will be checked. This will continue until a chunk is found, or the last bin is reached in which case a peice of memory will just be taken from the wilderness chunk. If the chunk that is found is large enough then it will be split. In order to determine if a chunk should be split the amount of metadata (overhead) is subtracted from what our current allocation doesn't use. If what is left is bigger than or equal to ```MIN_ALLOC_SZ``` then it means we should split this chunk and place the leftovers in the correct bin. Once we are ready to return the chunk we found then we take the address of the ```next``` field and return that. This is done because the ```next``` and ```prev``` fields are unused while a chunk is allocated therefore the user of the chunk can write data to these fields without any affecting the inner-workings of the heap.

##### Freeing: 
The function ```heap_free``` takes a pointer returned by ```heap_alloc```. It subtracts the correct offset in order to get the address of the node struct. Instead of simply placing the chunk into the correct bin, the chunks surrounding the provided chunk are checked. If either of these chunks are free then we can coalesce the chunks in order to create a larger chunk. To colaesce the chunks the footer is used to get the node struct of the previous chunk and the node struct of the next chunk. For example, say we have a chunk called ```to_free```. To get the the chunk before this chunk we subtract ```sizeof(footer_t)``` to get the footer of the previous chunk. The footer holds a pointer to the head of the previous chunk. To get the next chunk we simply get the footer of ```to_free``` and then add ```sizeof(footer_t)``` in order to get the next chunk. Once all of this is done and sizes are re-calculated the chunk is placed back into a bin.


### Possible Improvements
------------
  - Error-Checking - check for heap corruption, double-free, etc.
  - Improve binning policy.
    - currently the heap uses a simple hashing function to map chunk sizes to a bin index.
  - Rigorous testing to determine if crashes or fragmentation occur.
  - Minimize overhead (metadata) 
    - possibly change footer to hold a size rather than a pointer to a header.

### Sources 
------------
* [Doug Lea's Memory Allocator](http://g.oswego.edu/dl/html/malloc.html)

NOTE: This code was originally compiled for 32-bit machine, this means that when this code was used it was acceptable to cast an int to a pointer due to the fact they are both the same size. On a 64-bit system the compiler will warn you of the size difference. To silence these warnings I have used the ```uintptr_t``` when casting from an unsigned int to a pointer. This does muddy the readability a bit and some of the casts and pointer arithmetic are verbose so bear that in mind when reading the code.  
