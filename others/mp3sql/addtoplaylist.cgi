#!/usr/bin/perl

$PLIST = "/tmp/playlist";

$buffer=$ENV{'QUERY_STRING'};

$buffer =~ s/%20/ /g;

print "Content-type: text/html\n\n";


if (! -r $buffer) {
        print "<HEAD><TITLE>Sorry the file is not online</TITLE></HEAD>";
        print "<BODY BGCOLOR=\"#FFFFFF\"><CENTER><H1>Error</H1>The file $buffer is not online</BODY></HTML>";
        die "";
        }

print "<HEAD><TITLE>Added to Playlist</TITLE></HEAD>";
print "<BODY BGCOLOR=\"#FFFFFF\"><CENTER><H1>OK</H1>The file $buffer is added to playlist</BODY></HTML>";


open (OUT,">>$PLIST");
print OUT "$buffer\n";
close (OUT);



