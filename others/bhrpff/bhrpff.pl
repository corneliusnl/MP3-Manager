#!/usr/bin/perl -w

# BHRPFF - Ben Hates Reading Poorly Formatted Filenames!
# (C) Benjamin James Cooper - Winter 1999.
# See LICENSE for information about the license.
# See README for usage information.

# Disclaimer: This code could hurt you to read if you hate syntax

##################### code...

############## Modules

use strict;
use integer;

use File::Basename qw(fileparse);
use Text::Wrap qw(wrap $columns);

## Text::Wrap options 
$columns = 120;

############## filename parsing

## Global Configuration variables for &groups
## The defaults should be fine for most things.

# syntactical groupings
my $esc = '\[\(\{\]\)\}\-';
# bad things which shouldn't exist in filenames
my $bad = '\%\^\"\,\.\*\r\n\t';
# things which should be spaces
my $spc = '\_';		

# split beastly filenames into their canonical groupings
sub groups {
   $_ = $_[0] if @_;
   my $esc = $esc;		# shadow $esc, it may change later

   study;
   s/[$bad]+//g;		# cruft
   s/\s{2}/ /g;			# bad spacing
   s/([$esc])[$esc]/$1/g;	# so we don't split as much

   # if we can't find any other notation for groupings _ must be one..
   $esc .= '\_' unless /[$esc]/;

   # split, clean, change things up. This is the most important thing.
   return map { s/[$spc]+/ /g; s/^\s+|\s+$//g; $_ }
       grep { length and !/^\s+$/ }
       split /[$esc]/;
}

## Global Configureation variables for &canonicalname
# the canonical token divisor, this is used in canonicalname to
# seperate the groupings which are not comments (see below)
my $cdiv = ' - ';
# the limit for the number of name groupings before a comment group at
# the end.
my $glim = 2;

# Return the canonical name for group.
# This assumes by rule of thumb that the last entry in a grouping of
# over $glim non-integer groups is some kind of comment, like: (Remix)
sub canonicalname {
  ((scalar(grep { !/\d+/ } @_) > $glim) ?
     (join($cdiv, @_[0..$#_-1]) . " ($_[$#_])") :
      join($cdiv, @_))
}

############## end filename parsing

############## supporting functions

# Ask the user if $_[0] should be done. If anything other than a word
# which begins with y is entered then integer false (0) is returned.
sub shouldi {
   my $in;

   print shift, " ? ";
   chomp($in = <STDIN>); 
   $in = lc $in;

   if($in =~ /^y/) {
      return 1;
   } else {
      return 0;
   }
}

# print $_[0] to STDOUT and assume yes
sub always {
   print shift, "\n"; 
   return 1;
}

############## main... this is where the magic happens

# ask about name changes by default
my $ask = \&shouldi;

# Just rename all the files without asking.
# change $ask ref and remove the argument.
$ask = \&always and shift(@ARGV) 
   if(@ARGV and $ARGV[0] =~ /^-N/);

# variables used in main loop..
my (
     $nfn, 		# new filename (minus extension)
     $ofn, 		# old filename ( ''         '' )
     $ext,     		# file extension.. doesnt change
     $path		# the path to a given file
  );

# main loop..
for my $arg (@ARGV) {
   ($ofn, $path, $ext) = fileparse($arg, '\.[^\.]*$');

   $nfn = &canonicalname(&groups($ofn));
   rename($arg, ($path . $nfn . $ext)) 
      if(($ofn ne $nfn) and &$ask(wrap('', '', "\"$ofn\" -> \"$nfn\"")));
}

############## end main.

##################### end code...

##################### various things..
# Things I know this program wont currently do.. 

# 1. Jumbled Garbage
# BenHelloHereLittleBear

# 2. Debastardize netscape filenames
# Ben_%20Hello_%20There_%20Little_%20Bear
# s/\_(\d\d)/chr($1)/eg; 

# 3. Files that have tokens which use seperators as tokens
# Ben - Hello There (Little-Bear Remix)
# To fix this would require counting types of seperators
