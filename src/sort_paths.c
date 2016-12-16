#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "inc/path_util.h"
#include "inc/util.h"

/*  we're imposing a limit of 256x256 graph, so deal with it
 * input looks like this: C[255,255]-... 
*/

#define MAX_LEN 256
#define LINE_SIZE (MAX_LEN * 11 + 2)  /* +1 for \n, +1 for null */
#define NR_ALLOC 1024

long   nr_paths    = 0;
long   alloc_paths = 0;
path_t *paths = NULL;


void add_path     (char *line);
int  path_compare (const void *a, const void *b);
void list_paths   (void);

int main (int argc, char **argv) {
    char line[LINE_SIZE];
    
    alloc_paths = NR_ALLOC;
    paths = xzmalloc(sizeof(path_t) * alloc_paths, '\0');

    while (fgets(line, LINE_SIZE, stdin)) {
        add_path(line);
    }

    qsort(paths, nr_paths, sizeof(path_t), path_compare);

    list_paths();
}

void list_paths (void) {
    int i;
    int j;
    path_t *p;
    cell_t *c;

    for (i = 0; i < nr_paths; i++) {
        p = paths + i;
        for (j = 0; j < p->nr_cells; j++) {
            c = p->cells + j;

            if (j>0) printf("-");

            printf("%c[%d,%d]", c->letter, c->r, c->c);
        }
        printf("\n");
    }
}

int path_compare (const void *a, const void *b) {
    int i;
    path_t *pa = (path_t *)a;
    path_t *pb = (path_t *)b;
    cell_t *ca, *cb;

    int limit = pa->nr_cells < pb->nr_cells ? pa->nr_cells : pb->nr_cells;

    for (i = 0; i < limit; i++) {
        ca = pa->cells + i; 
        cb = pb->cells + i;

        if (ca->r < cb->r) return -1;
        if (ca->r > cb->r) return 1;
        if (ca->c < cb->c) return -1;
        if (ca->c > cb->c) return 1;
    }

    return 0;
}

void add_path (char *line) {
    path_t *path;

    if ( nr_paths == alloc_paths ) {
        alloc_paths += NR_ALLOC;
        paths = xrealloc(paths, sizeof(path_t) * alloc_paths);
    }
    path = paths + nr_paths;
    nr_paths++;

    parse_path(line, path);
}
