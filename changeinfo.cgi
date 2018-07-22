#!/usr/bin/perl

require "cgi.cgi";

&ReadParse;

#system("/usr/bin/id3tool -t $in{'Artist'} $in{'Title'}");

print "Content-type: text/html\n\n";

print "$in{'Artist'}\n";
print "$in{'Title'}\n";
