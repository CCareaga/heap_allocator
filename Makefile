clang-test:
	clang -O3 llist.c heap.c main.c -o heap_test
	./heap_test	

gcc-test:
	gcc -O3 llist.c heap.c main.c -o heap_test
	./heap_test

clean:
	rm heap_test