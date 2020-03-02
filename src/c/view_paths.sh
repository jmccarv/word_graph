#!/bin/bash

# Simple wrapper script to run word_graph for you

dir=`dirname $0`

function get_file() {
    while [ -n "$1" ]; do
        file="$1"
        shift;
    done
}

get_file "$@"
$dir/word_graph -l "$file" | $dir/view_paths "$@"
