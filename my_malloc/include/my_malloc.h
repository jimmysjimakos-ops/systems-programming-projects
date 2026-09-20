#ifndef MY_MALLOC_H     //this for including the file
#define MY_MALLOC_H     //this for marking to prevent duplicates

#include <stddef.h>     //this for giving us size_t

void *my_malloc(size_t size);   //this is my version of malloc which takes in a size_t size value
void my_free(void *ptr);           //this is my version of free- takes a malloc ptr

#endif
