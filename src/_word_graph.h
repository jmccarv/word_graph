#ifndef __WORD_GRAPH_H_
#define __WORD_GRAPH_H_

typedef struct opts {
    int  list;
    char *word;
    FILE *fh;
} opts_t;

extern opts_t  opts;

#endif
