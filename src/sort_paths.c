#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "word_graph.h"
#include "util.h"

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

    char *p = line;
    char *q;
    char *z;
    cell_t *cell;


    if ( nr_paths == alloc_paths ) {
        alloc_paths += NR_ALLOC;
        paths = xrealloc(paths, sizeof(path_t) * alloc_paths);
    }
    path = paths + nr_paths;
    nr_paths++;


    path->nr_cells = 1;
    for (; (p = strchr(p, '-')); path->nr_cells++, p++);

    cell = path->cells = xzmalloc(sizeof(cell_t) * path->nr_cells, '\0');


    /* Line will be a '-' separated list of cells, where each cell will
     * look like C[#,#] where C is a character and # is a number 
     * between 0 and 255
     */

    p = q = line;
    //printf("Enter line: %s\n", line);
    while (p) {
        //printf("Top of loop\n");
        q = strchr(p, '-');
        if (q) *q = '\0';

        cell->letter = *p;

        /* Find row number */
        p+=2;
        z = strchr(p, ',');
        if (!z) {
            fprintf(stderr, "Failed to parse input :(\n");
            exit(1);
        }
        *z = '\0';

        cell->r = atoi(p);
        p = z+1;

        /* Now find the column number */
        z = strchr(p, ']');
        if (!z) {
            fprintf(stderr, "Failed to parse input :(\n");
            exit(1);
        }
        *z = '\0';

        cell->c = atoi(p);
        //printf("parsed: %c[%d,%d]\n", cell->letter, cell->r, cell->c);

        p = q ? q+1 : NULL;
        cell++;
    }
}
