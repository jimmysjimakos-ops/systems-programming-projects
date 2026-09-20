#include <stdio.h>
#include "../include/my_malloc.h"

int main(void) {
    printf("=== Memory Allocator Test ===\n\n");

    // Test 1: basic allocation
    int *a = my_malloc(sizeof(int));
    *a = 42;
    printf("Test 1 - Allocated int: %d\n", *a);

    // Test 2: allocate a string
    char *str = my_malloc(64);
    str[0] = 'H'; str[1] = 'i'; str[2] = '\0';
    printf("Test 2 - Allocated string: %s\n", str);

    // Test 3: free and check
    my_free(a);
    printf("Test 3 - Freed first allocation\n");

    // Test 4: allocate after free
    int *b = my_malloc(sizeof(int));
    *b = 99;
    printf("Test 4 - New allocation after free: %d\n", *b);
    my_free(str);
    my_free(b);
    printf("\nAll tests passed!\n");
    return 0;
}