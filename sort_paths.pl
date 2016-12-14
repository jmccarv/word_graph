#!/usr/bin/env perl

use strict;
use warnings;

# Could probably be faster...

my @paths;
while (<>) {
    next unless /.\[\d+,\d+\]/;

    chomp;

    push @paths, [ split '-', $_ ];
}

print join ('-', @$_), "\n"
    for sort path_sort @paths;

sub path_sort {
    my $cmp = 0;

    for my $i ( 0..$#$a ) {
        next unless $cmp = compare_locations($a->[$i], $b->[$i]);
        return $cmp;
    }

    0;
}

sub compare_locations {
    my ($a, $b) = @_;

    $a =~ /(\d+),(\d+)/;
    my @a = ($1, $2);

    $b =~ /(\d+),(\d+)/;
    my @b = ($1, $2);

    return $a[0] <=> $b[0]
        unless $a[0] == $b[0];

    $a[1] <=> $b[1];
}
