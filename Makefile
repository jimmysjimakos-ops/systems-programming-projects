CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude

test: src/my_malloc.c tests/test_basic.c
	$(CC) $(CFLAGS) src/my_malloc.c tests/test_basic.c -o test_basic

run: test
	./test_basic

valgrind: test
	valgrind ./test_basic

clean:
	rm -f test_basic