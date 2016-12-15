#ifndef _PARSE_GRAPH_H_
#define _PARSE_GRAPH_H_

#include <stdio.h>

typedef struct graph {
    char *word;
    int  word_len;
    int  line_len;
    int  max_r;
    int  max_c;
    int  nr_lines;
    char **lines;
} graph_t;


/* Parse a graph from fh like the following where '*'s are the border (i.e.
 * there's nothing there). So the first character would be found at
 * offset (0,0)
 *
 * *******
 * *B-O-B*
 * *|\ /|*
 * *O O O*
 * *|/ \|*
 * *B-O-B*
 * *******
 *
 * into an array of strings with a border of space characters
 * for easier handling later. So the above will be parsed into something
 * like this, where the first character would be found at offset (1,1)
 *
 * *********
 * *       *
 * * B-O-B *
 * * |\ /| *
 * * O O O *
 * * |/ \| *
 * * B-O-B *
 * *       *
 * *********
 * 
 * if word is not passed in (is NULL) we expect to find it in the graph file
 * as an assignment of the form:
 # word = WORDINGRAPH
 */
graph_t *parse_graph (FILE *fh, const char *word);

#endif
