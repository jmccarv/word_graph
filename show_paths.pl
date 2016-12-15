#!/usr/bin/env perl

use Graph::Undirected;
use File::Spec;
use Getopt::Long;
use Curses;
use Time::HiRes qw( sleep );
use strict;
use warnings;

use v5.14;
no warnings 'experimental::smartmatch';

my $me = (File::Spec->splitpath($0))[2];

exit main();

sub usage {
    print STDERR @_,"\n" if @_;

    print STDERR <<EOT;
usage: $me [OPTIONS] [FILE]

Display all paths found from word_graph.pl, animated with curses

OPTIONS:
 -h, -?, --help   this help
 -w WORD, --word  word being spelled

EOT

    exit 1;
}

sub main {
    my @paths;
    my $opts = getopts();

    0;
}

sub getopts {
    my %opts;

    $opts{out} = 'count';

    GetOptions(
        \%opts,
        'help|h|?',
        'word|w=s',
    ) or usage();

    usage() if $opts{help};

    \%opts;
}

sub visualize {
    my ($opts, $puzzle_nodes, $g, @paths) = @_;
    my @nodes = map { @$_ } @$puzzle_nodes;

    initscr;
    start_color;
    cbreak;
    noecho();
    nonl();
    intrflush(stdscr, 0);
    keypad(stdscr, 1);
    curs_set(0);

    clear();
    for (@paths) {
        init_board(\@nodes);
        draw_path($opts, $puzzle_nodes, $g, $_);
        sleep 1;
    }

    endwin;
}

sub draw_path {
    my ($opts, $nodes, $g, $path) = @_;    

    for (@$path) {
        chgat($_->{r}, $_->{c}, 1, A_STANDOUT, 0, undef);
         refresh();
        sleep .5; 
    }
}

sub init_board {
    my $nodes = shift;

    erase();
    for (@$nodes) {
        addch($_->{r}, $_->{c}, $_->{char});
    }
    refresh();
}
