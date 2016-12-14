#!/usr/bin/env perl

use strict;
use warnings;

# Could probably be faster...

my @paths;

# Assume the output is being piped from word_graph.pl into
# this script, it makes sense to word during this loop that
# will save us time during the sort later
while (<>) {
    next unless /.\[\d+,\d+\]/;

    chomp;
    my @vertices = split '-', $_;

    push @paths, [ map { m/(\d+),(\d+)/; [ $1*@vertices + $2, $_ ] } @vertices ];
}

print join ('-', map { $_->[1] } @$_), "\n"
    for sort path_sort @paths;

sub path_sort {
    for my $i ( 0..$#$a ) {
        next if $a->[$i]->[0] ==  $b->[$i]->[0];
        return  $a->[$i]->[0] <=> $b->[$i]->[0];
    }

    0;
}
