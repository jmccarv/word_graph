#!/usr/bin/env perl

use Graph::Undirected;
use File::Spec;
use Getopt::Long;
use strict;
use warnings;

use v5.14;
no warnings 'experimental::smartmatch';

my $me = (File::Spec->splitpath($0))[2];

my %dir = (
    '-'  => [  0, -1 ],
    '|'  => [ -1,  0 ],
    '\\' => [ -1, -1 ],
    '/'  => [ -1,  1 ]
);

exit main();

sub usage {
    print STDERR @_,"\n" if @_;

    print STDERR <<EOT;
usage: $me [OPTIONS] [FILE]

Solve the word graph (see accompying README) in
FILE (or read from stdin if no FILE specified)

OPTIONS:
 -h, -?, --help     this help
 -o, --out <style>  what results to output, one of:
                    'list' or 'count',  default = count
 -w, --word         word to look for in graph 
                    (usually embedded in input file)

The above options may be set in the word graph FILE using the syntax
option = value. One setting per line, like so:

# comment lines start with '#'
# blank lines are ignored
word = BOB
visualize = 1

B-O-B
|\ /|
O O O
|/ \|
B-O-B
EOT

    exit 1;
}

sub main {
    my @paths;
    my $opts = getopts();

    my ($puzzle_nodes, $g) = parse_puzzle($opts);

    usage 'No word (-w) given to solve'
        unless $opts->{word};

    my $first_letter = substr($opts->{word},0,1);

    search($opts, $g, $_) 
        for grep { $_->{char} eq $first_letter } $g->vertices;

    printf "%d distinct path(s) found that spell '%s'\n", ($opts->{path_count} // 0), $opts->{word}
        if $opts->{out} eq 'count';

    0;
}

sub getopts {
    my %opts;

    $opts{out} = 'count';

    GetOptions(
        \%opts,
        'help|h|?',
        'out|o=s',
        'word|w=s',
    ) or usage();

    usage() if $opts{help};

    \%opts;
}

sub parse_puzzle {
    my $opts = shift;
    my @puzzle_nodes;

    my $g = Graph::Undirected->new(refvertexed => 1);

    my $r = 0;
    while (<>) {
        chomp;
        next if /^\s*$/;
        next if /^\s*#/;

        my $c = 0;

        if (/\s*(.*?)\s*=\s*(.*?)\s*$/) {
            $opts->{$1} //= $2;

        } else {
            my @line = split //, $_;

            for my $char (@line) {
                my $node = { char => $char, r => $r, c => $c };
                $puzzle_nodes[$r]->[$c] = $node;

                if ($dir{$char}) {
                    my $ofs = $dir{$char};
                    my $from_row = $r + $ofs->[0];
                    my $from_col = $c + $ofs->[1];
                    next if $from_row < 0 || $from_col < 0;

                    my $from_node = $puzzle_nodes[$from_row]->[$from_col];
                    next unless $from_node;
                    $node->{from} = $from_node->{from} // $from_node;;

                } else {
                    $g->add_vertex($node);
                    add_edges($g, $node, \@puzzle_nodes);
                }

                $c++;
            }
            $r++;
        }
    }

    (\@puzzle_nodes, $g);
}

sub node_id {
    my $n = shift;
    sprintf("%s[%d,%d]", $n->{char}, $n->{r}, $n->{c});
}

sub add_edges {
    my ($g, $node, $puzzle_nodes) = @_;

    for my $ofs (values %dir) {
        my $r = $node->{r} + $ofs->[0];
        my $c = $node->{c} + $ofs->[1];
        next if $r < 0 || $c < 0;

        my $check_node = $puzzle_nodes->[$r]->[$c];
        next unless $check_node && $check_node->{from};

        $g->add_edge($node, $check_node->{from});
    }
}

sub search {
    my ($opts, $g, @path) = @_;

    if ($opts->{word} eq join('', map{ $_->{char} } @path)) {
        display_path($opts, \@path);
        return;
    }

    my $next = substr($opts->{word}, @path, 1) or return ();

    search($opts, $g, @path, $_)
        for ( grep { $_->{char} eq $next } $g->successors($path[-1]) );
}


sub display_path {
    my $opts = $_[0];

    for ($opts->{out}) {
        disp_stdout(@_) when 'list';
        default         { ++$opts->{path_count} }
    }
}

sub disp_stdout {
    my ($opts, $path) = @_;

    print join ('-', map { node_id($_) } @$path), "\n"
}
