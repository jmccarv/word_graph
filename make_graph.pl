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
      printLetters( $y, @letters );
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
   my ($index, @letters) = @_;

   my $x = 0;

   if( $index == 0 or $index == (@letters)-1 ){
      foreach my $letter (@letters) {
         if( $x != 0 ){ print "-"; }
         print "$letter";
         ++$x;
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
