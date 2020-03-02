#ifndef _PARSE_GRAPH_H_
#define _PARSE_GRAPH_H_

#include <stdio.h>

typedef struct graph {
    char *word;
    int  word_len;
    int  line_len;
    int  nr_rows;
    int  nr_cols;
    int  nr_lines;
    char **lines;
} graph_t;


/* Parse a graph from fh like the following
 *
 * B-O-B
 * |\ /|
 * O O O
 * |/ \|
 * B-O-B
 *
 * into an array of strings with a border of space characters
 * for easier handling later. Each string represents one row of
 * data for the graph. So the above will be parsed into something
 * like this, where the first character would be found at a[1][1]
 *
 * a[0] = "       "
 * a[1] = " B-O-B "
 * a[2] = " |\ /| "
 * a[3] = " O O O "
 * a[4] = " |/ \| "
 * a[5] = " B-O-B "
 * a[6] = "       "
 * 
 *
 * if word is not passed in (is NULL) we expect to find it in the graph file
 * as an assignment of the form:
 *
 * word = BOB
 *
 * Returns a graph "object" with the following "attributes"
 *  word      the word we're looking for
 *  word_len  self explanitory
 *  line_len  length of each line from the parsed data, for the example above
 *            this would be strlen('B-O-B') = 5
 *  nr_rows   the number of rows of data, 5 for the example above
 *  nr_cols   the number of columns of data, 5 for the example above
 *  lines     array of strings where each string represnets one line of the
 *            parsed graph (see above).
 */
graph_t *parse_graph (FILE *fh, const char *word);

#endif
