#!/usr/bin/perl -w
#Mp32Sql: This is basicaly a script that goes through a directory recursivly
#		 It then takes any .mp3 file names and adds them to an SQL database
#		 This is useful for searching or adding a php front end for a web site
#		 Hopefuly the incorperation of the files id3 name shouldnt be that difficult
#
# Writen by Chris Milbert: Thanks to #perl on efnet.  Special Thanks to dynweb

##################################################################
#External modules to use								         #
##################################################################
use MPEG::MP3Info qw(:all);	   #Returns mp3 tag info
use Gtk::Dialog;			   #Gtk perl mod
use Getopt::Std;			   #Checks the parameters passed during exec
use Getopt::Long;			   #Checks the parameters passed during exec
use Cwd;                       #Current working directory mod
use DBI;				   #Database interaction mod
use File::Find;				   #Directory mod
##################################################################

##################################################################
#Variables section 											     #
#This is basically the main configuration section				 # 
##################################################################
getopt('d:e:db:t:s:u:p');            #reads in data to opt_o and opt_T
GetOptions("help" => \$needhelp);

$searchdir = getcwd;		  #Start directory
$filetype = '.mp3';			  #File extention
$outdb = 'mp3db'; 		      #Database to output to the table defs are in MySQL_Table
$outtable = 'mp3main';        #Table to input data into (useful if your adding multiple directories)
$sqlserver = 'localhost';     #SQL server to use
$sqluser = '';			      #SQL username
$sqlpass = '';		          #SQL password

#Run GUI check
if ($opt_g) {
%guihash = gtk_dialog {
  title => 'mp32SQL GTK Frontend',
  column => {
    1 => {
      type => 'entry',
      title => 'Directory to recurse:',
      tag => 'dir',
      lines => '1',
      default => getcwd,
      tip => 'Enter the Directory to Recurse'
      },
    2 => {
      type => 'entry',
      title => 'File Extention',
      tag => 'extention',
      lines => '1',
      default => '.mp3',
      tip => 'Enter the Extention to Search for'
      },
    3 => {
      type => 'entry',
      title => 'Database Server',
      tag => 'sqlserver',
      lines => '1',
      default => 'localhost',
      tip => 'Enter the SQL Server'
      },
    4 => {
      type => 'entry',
      title => 'SQL Server Username',
      tag => 'sqlusername',
      lines => '1',
      default => 'nobody',
      tip => 'Enter a SQL Username',
      },
    5 => {
      type => 'entry',
      title => 'SQL Server Password',
      tag => 'sqlpassword',
      line => '1',
      default => '',
      tip => 'Enter a SQL Password',
      visible => '0'
      },
    6 => {
      type => 'entry',
      title => 'Database',
      tag => 'db',
      lines => '1',
      default => 'mp3db',
      tip => 'Enter the SQL Database to Use'
      },
    7 => {
      type => 'entry',
      title => 'Table',
      tag => 'table',
      lines => '1',
      default => 'mp3main',
      tip => 'Enter the SQL Table to Use'
      },
    8 => {
      type => 'button',
      title => 'Process',
      default => '1',
      tag => '1'
      },
    }
   };
 $searchdir = $guihash{dir};
 $filetype = $guihash{extention};
 $sqlserver = $guihash{sqlserver};
 $outdb = $guihash{db};
 $outtable = $guihash{table};
 $sqluser = $guihash{sqlusername};
 $sqlpass = $guihash{sqlpassword};
 print $searchdir;
 }
else{

#help check
if ($needhelp || $opt_h) {
print "Syntax: \n";
print "mp32sql.pl [parameters] \n";
print "Parameters: \n";
print "  -d [Directory to recurse Default: $searchdir] \n";
print "  -e [File extention to look for Default: .mp3] \n";
print "\n";
print "  -s [SQL server to use Default: localhost] \n";
print "  -b [Database to use Default: mp3db] \n";
print "  -t [Table to use Default: mp3main] \n";
print "\n";
print "  -u [SQL username] \n";
print "  -p [SQL password] \n";
print "  -g [GTK graphical front end (requires: Gtk::Dialog)] \n";
die "              \n";
$opt_h = "Needed help";
}

#Check params and use default if not specified
if ($opt_d) {$searchdir = $opt_d;}
else {$searchdir = getcwd;}

if ($opt_e) {$filetype = $opt_e;}
else {$filetype = '.mp3';}

if ($opt_s) {$sqlserver = $opt_s;}
else {$sqlserver = 'localhost';}

if ($opt_b) {$outdb = $opt_b;}
else {$outdb = 'mp3db';};

if ($opt_t) {$outtable = $opt_t;}
else {$outtable = 'mp3main';}

if ($opt_u) {$sqluser = $opt_u}
else {
print "\n";
print "Please enter a SQL user name: ";
$sqluser = <STDIN>;
chop($sqluser);
print "\n";
}

if ($opt_p) {$sqlpass = $opt_p}
else {
print "*NOTE: password is not hidden* \n";
print "Please enter a password for user $sqluser: ";
$sqlpass = <STDIN>;
chop($sqlpass);
}

#Echo values back to the user and check for correctness
print "Directory to recurse: $searchdir \n";
print "File extention:       $filetype \n";
print "SQL server:           $sqlserver \n";
print "SQL database:         $outdb \n";
print "SQL table:            $outtable \n";
print "SQL username:         $sqluser \n";
print "\n";
print "Is this information correct?[y/n]  ";
$correct = <STDIN>;
chop($correct);
if ($correct eq "n" || $correct eq "N") {
print "\n";
die "Wrong information so quitting \n";}
}
##################################################################
#Start loop check

print "Adding.. \n";

$dbh = DBI->connect("DBI:mysql:database=$outdb;host=$sqlserver",
							$sqluser, $sqlpass, {RaiseError =>0});
find sub { if ($_ =~ m/$filetype$/si)
{
	$filename = $_;
        #Get the mp3 info from the id3 tag							     #
	#this basicaly outputs all the tag info and the song time and    #
	#bitrate info to a data                                          #
	#array called$tag and $info to get to that data we use           #
	#$tag->{TITLE} for the title                                     #
	#and $info->{MM} for the play time in minitutes				     #
	
	$tag = get_mp3tag($filename);
	$info = get_mp3info($filename);

	#Start Database output section								     #
	
	$tag->{GENRE} = use_winamp_genres($tag->{GENRE});
	
	#check all of the insert variables and find ' and replace it with \'
	$filename =~ tr/'//d;
	$filename =~ tr/"//d;
	$filename =~ tr/?//d;
	$filename =~ tr/%//d;
	
	$File::Find::dir =~ tr/'//d;
	$File::Find::dir =~ tr/"//d;
	$File::Find::dir =~ tr/?//d;
	$File::Find::dir =~ tr/%//d;
	
	if ($tag->{GENRE}){
		$tag->{GENRE} =~ tr/'//d;
		$tag->{GENRE} =~ tr/"//d;
		$tag->{GENRE} =~ tr/?//d;
		$tag->{GENRE} =~ tr/%//d;	
	}
	else {$tag->{GENRE} = "None";}

	if ($tag->{ARTIST}){
		$tag->{ARTIST} =~ tr/'//d;
		$tag->{ARTIST} =~ tr/"//d;
		$tag->{ARTIST} =~ tr/?//d;
		$tag->{ARTIST} =~ tr/%//d;	
	}
	else {$tag->{ARTIST} = "None";}

	if ($tag->{TITLE}){
		$tag->{TITLE} =~ tr/'//d;
		$tag->{TITLE} =~ tr/"//d;
		$tag->{TITLE} =~ tr/?//d;
		$tag->{TITLE} =~ tr/%//d;	
	}
	else {$tag->{TITLE} = "None";}
	
	if ($tag->{ALBUM}){
		$tag->{ALBUM} =~ tr/'//d;
		$tag->{ALBUM} =~ tr/"//d;
		$tag->{ALBUM} =~ tr/?//d;
		$tag->{ALBUM} =~ tr/%//d;
	}
	else {$tag->{ALBUM} = "None";}
	
	$query = "INSERT INTO $outtable (filename,filedir,genre,artist,songname,album,min,sec) VALUES ('$filename','$File::Find::dir','$tag->{GENRE}','$tag->{ARTIST}','$tag->{TITLE}','$tag->{ALBUM}','$info->{MM}','$info->{SS}')";
	
	$dbh->do($query);

	$sum++;
	print "Added: $filename \n";
}
}, $searchdir;
#Loop Ended
$dbh->disconnect();
if ($opt_g){
	$msgstring = 'Finished Processing Files';
	gtk_message_box $msgstring, $sum, 'Files were added', [ ' OK ' ]; 
	die "Finished processing of files.\n";}
else {
	print "Done processing $searchdir \nA total of $sum files were added\n";
     }
