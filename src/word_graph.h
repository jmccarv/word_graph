#ifndef _WORD_GRAPH_H_
#define _WORD_GRAPH_H_

typedef struct cell {
    char r;
    char c;
    char letter;
} cell_t;

typedef struct path {
    cell_t *cells;
    char nr_cells;
} path_t;

#endif
