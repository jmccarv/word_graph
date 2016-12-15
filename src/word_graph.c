#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "word_graph.h"
#include "parse_graph.h"
#include "search_graph.h"
#include "util.h"
#include "_word_graph.h"

opts_t  opts  = { 0, NULL, NULL };

void get_opts   (int argc, char **argv);

void usage (int rc) {
    printf("usage: word_graph [OPTIONS] [FILE]\n\n"
           "Solve the word graph (see accompying README) in\n"
           "FILE (or read from stdin if no FILE or FILE is '-')\n\n"
           "OPTIONS:\n"
           " -?        this help\n"
           " -l        list paths instead of count\n"
           " -w <word> word to search for (usually embedded in input file)\n"
    );

    exit(rc);
}

int main (int argc, char **argv) {
    get_opts(argc, argv);
    graph_t *graph;

    graph = parse_graph(opts.fh, opts.word);

    if (EOF == fclose(opts.fh)) {
        perror("Failed to process input file");
        exit(1);
    }

    find_paths(graph, opts.list);

    return 0;
}

void get_opts (int argc, char **argv) {
    int which;

    while (-1 != (which = getopt(argc, argv, "lw:?"))) {
        switch(which) {
            case 'l':
                opts.list = 1;
                break;

            case 'w':
                opts.word = strdup(optarg);
                break;

            case '?':
                usage(0);
                break;

            default:
                usage(1);
                break;
        }
    }

    if (optind >= argc || !strcmp(argv[optind], "-")) {
        opts.fh = stdin;

    } else {
        if (NULL == (opts.fh = fopen(argv[optind], "r"))) {
            perror("Failed to open input file");
            exit(1);
        }
    }
}
