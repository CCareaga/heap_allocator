# SHMALL - Simple Heap Memory ALLocator
------------
This is a simple heap allocator I wrote for a hobby OS I have been working on. I wrote this allocator with very little research, and it is made to be as easy to understand as possible. I think this repository can be useful to those who are new to OS development (like myself), or are interested in seeing a simplified version or functions like malloc and free.

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
this will run a demo of the allocator and print out some information

### Possible Improvments
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

NOTE: you may notice that all of the variables are very size specific (this may cuase a alot warnings from your compiler)
this is because this code is originally from a hobby OS. it is important that all structures are a predicatable size in order for certain calculations to work in the code. There are various areas where if the math is off a couple bytes, everything may stop working!
