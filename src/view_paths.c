#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "inc/parse_graph.h"
#include "inc/path_util.h"
#include "inc/util.h"

typedef struct opts {
    char  *word;
    FILE  *graph_fh;
    long  cell_delay;
    long  path_delay;
} opts_t;

opts_t opts = { NULL, NULL, 50000, 100000 };

void get_opts (int argc, char **argv);

void usage (int rc) {
    printf("usage: view_paths [OPTIONS] FILE\n\n"
           "Read paths (as output by word_graph -l) from stdin and display them\n\n"
           "FILE is a file containng the graph description, the same as passed to word_graph\n\n"
           "OPTIONS:\n"
           " -?        this help\n"
           " -d <sec>  fractional seconds to delay between each movement drawing a\n"
           "           path (default=%.2f)\n"
           " -D <sec>  fractional seconds to delay between each path (default=%.2f)\n"
           " -w <word> word searched for with word_graph (usually embedded in graph file)\n",
           (double)opts.cell_delay/1000000, (double)opts.path_delay/1000000
    );

    exit(rc);
}

void init_curses(void) {
    initscr();
    start_color();
    cbreak();
    noecho();
    nonl();
    intrflush(stdscr, 0);
    keypad(stdscr, 1);
    curs_set(0);
}

void init_board(graph_t *graph) {
    int r;

    erase();
    for (r=1; r <= graph->nr_rows; r++) {
        mvaddnstr(r-1, 0, &graph->lines[r][1], graph->nr_cols);
    }
}

void display_path (graph_t *graph, path_t *path) {
    int i;
    cell_t *cell;
    cell_t prev;

    attr_t attr_path    = A_STANDOUT;
    attr_t attr_curcell = A_STANDOUT;

    if (opts.cell_delay > 0) attr_curcell |= A_UNDERLINE;

    for (i=0; i < path->nr_cells; i++) {
        cell = path->cells + i;

        if (i > 0) {
            prev = *(path->cells + i-1);
            prev.r = cell->r + (prev.r - cell->r)/2;
            prev.c = cell->c + (prev.c - cell->c)/2;

            mvchgat(prev.r, prev.c, 1, attr_curcell, 0, NULL);

            if (opts.cell_delay > 0) {
                refresh();
                usleep(opts.cell_delay);
                chgat(1, attr_path, 0, NULL);
            }
        }

        mvchgat(cell->r, cell->c, 1, attr_curcell, 0, NULL);
        refresh();

        if (opts.cell_delay > 0) {
            usleep(opts.cell_delay);
            chgat(1, attr_path, 0, NULL);
        }
    }
}

int main (int argc, char **argv) {
    get_opts(argc, argv);
    graph_t *graph;
    int line_len;
    path_t path;
    char *line;

    graph = parse_graph(opts.graph_fh, opts.word);

    if (EOF == fclose(opts.graph_fh)) {
        perror("Failed to process input file");
        exit(1);
    }

    init_curses();

    /* input lines look like B[xxx,xxx]-O[xxx,xxx]-B[xxx,xxx] (don't forget \n and \0)
     * should only need +1, but +2 would also allow for \r\n
     */
    line_len = sizeof(char) * graph->word_len * 11 + 2;
    line = xzmalloc(line_len, '\0');

    init_board(graph);
    opts.path_delay -= opts.cell_delay;

    while (fgets(line, line_len, stdin)) {
        display_path(graph, parse_path(line, &path));
        free_path(&path);
        if (opts.path_delay > 0) usleep (opts.path_delay);
        init_board(graph);
    }

    endwin();
    return 0;
}

void get_opts (int argc, char **argv) {
    int which;

    while (-1 != (which = getopt(argc, argv, "d:D:w:?"))) {
        switch(which) {
            case '?':
                usage(0);
                break;

            case 'd':
                opts.cell_delay = atof(optarg) * 1000000;
                break;

            case 'D':
                opts.path_delay = atof(optarg) * 1000000;
                break;

            case 'w':
                opts.word = strdup(optarg);
                break;

            default:
                usage(1);
                break;
        }
    }

    if (optind >= argc) {
        printf("You must specify a graph FILE\n");
        usage(1);
    }

    if (NULL == (opts.graph_fh = fopen(argv[optind], "r"))) {
        perror("Failed to open graph description file");
        exit(1);
    }
}
