#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "word_graph.h"
#include "parse_graph.h"
#include "util.h"
#include "_word_graph.h"

void parse_assignment(graph_t *graph, char *start, char *delim) {
    *delim='\0';

    if (!graph->word && !strncmp(start, "word", 4)) {
        // skip any leading spaces
        for(start = delim+1; *start && isspace(*start); start++);

        // find end of word (first space or EOL)
        for(delim = start; *delim && !isspace(*delim); delim++);

        if (delim > start) {
            graph->word     = strdup(start);
            graph->word_len = strlen(graph->word);
            graph->line_len = graph->max_c = graph->word_len * 2 - 1;
        }
    }
}

void graph_add_line (graph_t *graph, char *line) {
    graph->lines = xrealloc(graph->lines, sizeof(char *) * graph->nr_lines+1);
    graph->lines[graph->nr_lines++] = line;
}

#define BUF_SIZE 1024
graph_t *parse_graph (FILE *fh, const char *word) {
    char buf[BUF_SIZE];
    char *p;
    char *delim;
    int  len = 0;
    int  line = 0;
    char *graph_line;
    char *g;

    int  line_size = 0;
    char *empty_line = NULL;

    graph_t *graph = xzmalloc(sizeof(graph_t), '\0');

    while (fgets(buf, BUF_SIZE, fh)) {
        ++line;

        if (!strlen(buf)) continue;

        // trim trailing spaces
        for (p=buf+strlen(buf)-1; *p && isspace(*p); *(p--) = '\0');

        // skip leading spaces
        for (p=buf; *p && isspace(*p); p++);
        
        // ignore empty lines and comments
        if (!*p || *p == '#') 
            continue;

        delim = strchr(p, '=');
        if (delim) {
            parse_assignment(graph, p, delim);
            continue;
        }

        if (!graph->word) {
            fprintf (stderr, "No word defined, either specify it via -w or have word=<word> in your input before the graph itself\n");
            exit(1);
        }

        if (!empty_line) {
            /* +2 for extra leading and trailing spaces, +1 for trailing NULL */
            line_size = graph->line_len + 3;
            empty_line = xzmalloc(sizeof(char) * line_size, ' ');
            empty_line[line_size-1] = '\0';
            graph_add_line(graph, empty_line);
        }

        g = graph_line = xzmalloc(sizeof(char) * line_size, ' ');
        graph_line[line_size-1] = '\0';

        for(g++, len = 0; *p; p++, g++) {
            if (++len > graph->line_len) {
                fprintf(stderr, "Line %d too long for word '%s'\n", line, graph->word);
                exit(1);
            }

            *g = *p;            
        }

        graph_add_line(graph, graph_line);
        ++graph->max_r;
    }

    if (empty_line) graph_add_line(graph, empty_line);

    /* +2 for extra blank lines above and below graph */
    if (graph->nr_lines != graph->line_len + 2) {
        fprintf(stderr, "Graph is wong dimensions: %dx%d, was expecting %dx%d characters\n", graph->line_len, graph->nr_lines, graph->line_len, graph->line_len);
    }

    return graph;
}
