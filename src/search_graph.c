#include <inttypes.h>
#include "parse_graph.h"
#include "util.h"

typedef struct position {
    int  r;
    int  c;
    char edge;
} position_t;

position_t ofs[8] = { {-1,-1,'\\'}, {-1,0,'|'}, {-1,1,'/'}, {0,-1,'-'}, {0,1,'-'}, {1,-1,'/'}, {1,0,'|'}, {1,1,'\\'} };

uint64_t descend (graph_t *graph, position_t *path, int word_ofs, int print_paths);

void find_paths (graph_t *graph, int print_paths) {
    int r,c;
    long nr_paths = 0; 
    position_t *path;

    path = xzmalloc(sizeof(position_t) * graph->word_len, '\0');

    for (r=1; r <= graph->max_r; r++) {
        for (c=1; c <= graph->max_c; c++) {
            path[0].r = r;
            path[0].c = c;
            nr_paths += descend(graph, path, 0, print_paths);
        }
    }

    if (!print_paths)
        printf ("Found %"PRIu64" paths\n", nr_paths);
}

void print_path (graph_t *graph, position_t *path) {
    int i;

    for (i=0; i < graph->word_len; i++) {
        if (i > 0) printf("-");
        printf("%c[%d,%d]", graph->word[i], path[i].r-1, path[i].c-1);
    }
    printf("\n");
}

uint64_t descend (graph_t *graph, position_t *path, int word_ofs, int print_paths) {
    int i;
    uint64_t nr_paths = 0;
    position_t *pos = path + word_ofs;
    position_t next;

    if (graph->word[word_ofs] != graph->lines[pos->r][pos->c]) return 0;

    if (word_ofs+1 == graph->word_len) {
        /* We've spelled the word .. yay */
        if (print_paths) print_path(graph, path);
        return 1;
    }
    
    /* This character matches, we now need to follow edges
     * to the next character to check
     */
    for (i=0; i<8; i++) {
        next.r = pos->r+ofs[i].r;
        next.c = pos->c+ofs[i].c;

        if ( graph->lines[next.r][next.c] != ofs[i].edge )
            continue;

        next.r += ofs[i].r;
        next.c += ofs[i].c;

        path[word_ofs+1] = next;
        nr_paths += descend(graph, path, word_ofs+1, print_paths);
    }

    return nr_paths;
}
