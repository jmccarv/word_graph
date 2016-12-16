#ifndef _PATH_UTIL_H_
#define _PATH_UTIL_H_

typedef struct cell {
    char r;
    char c;
    char letter;
} cell_t;

typedef struct path {
    cell_t *cells;
    char nr_cells;
} path_t;


/* Given a line of output from word_graph, like B[0,0]-O[0,2]-B[0-4]
 * populated the path structured pointed to by path
 *
 * Returns path (same as what is passed in)
 */
path_t *parse_path (char *line, path_t *path);

/* Free up resources used within path, does not free path itself */
void free_path (path_t *path);

#endif
