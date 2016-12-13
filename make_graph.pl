#!/usr/bin/env perl

use strict;
use warnings;

exit main();

sub main {

   my $word = uc( $ARGV[0] );
   my $word_len = length $word;

   if( $word_len % 2 == 0 ){
      print "
      make_graph.pl PALINDROME_STRING

         where PALINDROME_STRING is an odd character length palindrome such as:
            BOB
            POP
            LEVEL
            MADAM
            RACECAR
            MADAMIMADAM
            SOMEMENINTERPRETNINEMEMOS \n\n";
      return 1;
   }

   print "word = $word\n\n";

   my @letters = split('',$word);
   my $y = 0;

   my $graph_stats = {
      verts => 0,
      edges => 0,
   };

   for( $y = 0; $y < $word_len; ++$y ){

      if( $y % 2 == 0 ){
         printLetters( $y, 'short', $graph_stats, @letters );
      } else {
         printLetters( $y, 'long', $graph_stats, @letters );
      }
      if( $y+1 != $word_len){
         if( $y % 2 == 0 ){
            printContractPaths( $word_len, $graph_stats );
         } else {
            printExpandPaths( $word_len, $graph_stats);
         }
      }
   }

   my $pockets      = 2-($graph_stats->{verts} - $graph_stats->{edges});

   print "\n# Edges:       $graph_stats->{edges}\n";
   print "# Vertices:    $graph_stats->{verts}\n";
   print "# Pockets:     $pockets\n";
   print "# Word Length: $word_len\n";

   # TODO: Solve this seemingly NP-Hard (maybe NP-Coplete) stuff for said planar graph
   #my $worst_case   = ($graph_stats->{verts} * $graph_stats->{edges} ) / 2;
   #my $upper_bound  = $graph_stats->{verts} * $word_len;
   #my $max_capacity = 2 * $graph_stats->{edges} * (log($word_len)/log(2));
   #print "# Worst Case:  $worst_case\n";
   #print "# Upper Bound: $upper_bound\n";
   #print "# Capacity:    $max_capacity\n";
   print "\n";
}

sub printLetters {
   my ($y, $row_style, $graph_stats, @letters) = @_;

   my $x = 0;
   my $row_len = (@letters);
   my $middle = (@letters)/2+1;
   my $letter = '';

   # The first and last rows
   if( $y == 0 or $y == (@letters)-1 ){
      foreach $letter (@letters) {
         if( $x != 0 ){
            print "-";
            $graph_stats->{edges} += 1;
         }
         print "$letter";
         $graph_stats->{verts} += 1;
         ++$x;
      }
   } else {
      # It's a middle row, now long or short

      if( $row_style eq 'long' ){

         $row_len = int((@letters)/2)+2;
         $letter = $letters[$y];
         for( $x = 0; $x < $row_len; ++$x ){
            if( $x == 0 or $x+2 == $row_len  ){
               print "$letter ";
            } elsif( $x+1 != $row_len ){
               print "$letter   ";
            } else {
               print "$letter";
            }
            $graph_stats->{verts} += 1;
         }

      } else {

         $row_len = int((@letters)/2)+1;
         $letter = $letters[$y];
         for( $x = 0; $x < $row_len; ++$x ){
            if( $x+1 != $row_len ){
               print "$letter   ";
            } else {
               print "$letter";
            }
            $graph_stats->{verts} += 1;
         }
      }

   }
   print "\n";
}

sub printContractPaths {
   my ($length, $graph_stats) = @_;
   --$length;
   $length = $length / 2;
   print '|';
   $graph_stats->{edges} += 1;
   for( my $i = 0; $i < $length; ++$i ){
      if( $i != 0 ){
         print ' ';
      }
      print '\\ /';
      $graph_stats->{edges} += 2;
   }
   print "|\n";
   $graph_stats->{edges} += 1;
}

sub printExpandPaths {
   my ($length, $graph_stats) = @_;
   --$length;
   $length /= 2;
   print '|';
   $graph_stats->{edges} += 1;
   for( my $i = 0; $i < $length; ++$i ){
      if( $i != 0 ){
         print ' ';
      }
      print '/ \\';
      $graph_stats->{edges} += 2;
   }
   print "|\n";
   $graph_stats->{edges} += 1;
}
