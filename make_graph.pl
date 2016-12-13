#!/usr/bin/env perl

use Graph::Undirected;
use strict;
use warnings;

exit main();

sub main {

   my $word = uc( $ARGV[0] );
   my $word_len = length $word;

   print "word = $word\n\n";

   warn "Length = $word_len\n";

   my @letters = split('',$word);
   my $x = 0;
   my $y = 0;

   for( $y = 0; $y < $word_len; ++$y ){

      if( $y % 2 == 0 ){
         printLetters( $y, 'short', @letters );
      } else {
         printLetters( $y, 'long', @letters );
      }
      if( $y+1 != $word_len){
         if( $y % 2 == 0 ){
            printContractPaths( $word_len );
         } else {
            printExpandPaths( $word_len );
         }
      }
   }

}

sub printLetters {
   my ($y, $row_style, @letters) = @_;

   my $x = 0;
   my $row_len = (@letters);
   my $middle = (@letters)/2+1;
   my $letter = '';

   # The first and last rows
   if( $y == 0 or $y == (@letters)-1 ){
      foreach $letter (@letters) {
         if( $x != 0 ){ print "-"; }
         print "$letter";
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
         }
      }

   }
   print "\n";
}

sub printContractPaths {
   my ($length) = @_;
   --$length;
   $length = $length / 2;
   print '|';
   for( my $i = 0; $i < $length; ++$i ){
      if( $i != 0 ){
         print ' ';
      }
      print '\\ /';
   }
   print "|\n";
}

sub printExpandPaths {
   my ($length) = @_;
   --$length;
   $length /= 2;
   print '|';
   for( my $i = 0; $i < $length; ++$i ){
      if( $i != 0 ){
         print ' ';
      }
      print '/ \\';
   }
   print "|\n";
}
