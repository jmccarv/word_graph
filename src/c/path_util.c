#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "inc/path_util.h"
#include "inc/util.h"

/* we're imposing a limit of 256x256 graph, so deal with it
 * input looks like this: C[r,c]-...
 * where r and c are integer values ranging from 0 - 255
*/

#define MAX_LEN 256
#define LINE_SIZE (MAX_LEN * 11 + 2)  /* +1 for \n, +1 for null */
#define NR_ALLOC 1024

void free_path (path_t *path) {
    if (path && path->cells)
        free(path->cells);

    path->nr_cells = 0;
}

path_t *parse_path (char *line, path_t *path) {
    char *p = line;
    char *q;
    char *z;
    cell_t *cell;
    char cellflag[MAX_LEN][MAX_LEN];

    path->cells = NULL;
    path->nr_cells = 0;
    path->has_cycle = 0;

    if (!line) return path;

    memset(cellflag, '\0', sizeof(char) * MAX_LEN * MAX_LEN);

    path->nr_cells++;
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

        //if ( cellflag[cell->r][cell->c]++ )
        if ( cellflag[(int)cell->r][(int)cell->c]++ )
            path->has_cycle = 1;

        p = q ? q+1 : NULL;
        cell++;
    }

    return path;
}
