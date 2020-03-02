#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void *xzmalloc(size_t size, char init) {
    void *ret;

    if (NULL == (ret = malloc(size))) {
        fprintf(stderr, "Failed to malloc() memory!\n");
        exit(1);
    }
    memset(ret, init, size);
    return ret;
}

void *xrealloc(void *ptr, size_t size) {
    void *ret;

    if (NULL == (ret = realloc(ptr, size))) {
        fprintf(stderr, "Failed to realloc()!\n");
        exit(1);
    }

    return ret;
}
