#!/usr/bin/env perl

use Graph::Undirected;
use strict;
use warnings;

my %dir = (
    '-'  => [  0, -1 ],
    '|'  => [ -1,  0 ],
    '\\' => [ -1, -1 ],
    '/'  => [ -1,  1 ]
);

exit main();

sub main {
    my @paths;
    my ($opts, $puzzle_nodes, $g) = parse_puzzle();

    return unless $opts->{word};
    my $first_letter = substr($opts->{word},0,1);

    push @paths, search($opts, $g, $_) 
        for grep { $_->{char} eq $first_letter } $g->vertices;

    print join ('-', map { node_id($_) } @$_), "\n"
        for sort path_sort @paths;

    print @paths." distinct paths found that spell '".$opts->{word}."'\n";
}

sub path_sort {
    my @nb = @$b;
    for my $na (@$a) {
        my $nb = shift @nb;
        next if $na->{idx} == $nb->{idx};
        return $na->{idx} <=> $nb->{idx};
    }
    0;
}

sub parse_puzzle {
    my %opts;
    my @puzzle_nodes;

    my $g = Graph::Undirected->new(refvertexed => 1);

    my $r = 0;
    my $idx = 0;
    while (<>) {
        chomp;
        next if /^\s*$/;

        my $c = 0;

        if (/\s*(.*?)\s*=\s*(.*?)\s*$/) {
            $opts{$1} = $2;

        } else {
            my @line = split //, $_;

            for my $char (@line) {
                my $node = { char => $char, r => $r, c => $c, idx => $idx++ };
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

    #print_graph($g);
    #print Dumper(\@puzzle_nodes);
    (\%opts, \@puzzle_nodes, $g);
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
    my @found = ();

    return \@path if $opts->{word} eq join('', map{ $_->{char} } @path);

    my $next = substr($opts->{word}, @path, 1) or return ();

    push @found, search($opts, $g, @path, $_)
        for ( grep { $_->{char} eq $next } $g->successors($path[-1]) );

    @found;
}


sub virtex_sort {
    $a->{idx} <=> $b->{idx};
}

sub edge_sort {
    my $va = $a->[0]->{idx} <= $a->[1]->{idx} ? $a->[0] : $a->[1];
    my $vb = $b->[0]->{idx} <= $b->[1]->{idx} ? $b->[0] : $b->[1];

    $va->{idx} <=> $vb->{idx};
}

sub print_graph {
    my $g = shift;
    for my $e (sort edge_sort $g->edges) {
        printf("%s-%s\n", node_id($e->[0]), node_id($e->[1]));
    }
}

