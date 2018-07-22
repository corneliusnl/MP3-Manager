#!/usr/bin/perl
#
# Synopsis:
#
# Perl script to parse database, select as many songs as will fit
# on a single CD-ROM, alter their database entries so they
# point to their new locations on the CD-ROM and move them to a
# temporary directory where they can then be burnt.
#
# Author:
# 
# Jeremiah Cornelious McCarthy <jeremiahmccarthy@roadrunner.nf.net>
#

print "Content-type: text/html\n\n";
print "<h1>Writing CD-ROM</h1>\n";

use DBI;
#require cgi.cgi;

my $cdrom_size_bytes = 2048*333000;
my $cache_size_bytes = 0;
my $disk_number = 0;

# Connect to SQL server
$db_handle = DBI->connect("DBI:mysql:mp3manager:neurotoxin.dhs.org", "root", "convert") || die("Connect error: $DBI::errstr");

# Cacluate disk number
$what = $db_handle->prepare("SELECT * FROM Disks;");
$what->execute;
$disk_number = $what->rows;
$what->finish;

# Insert the new disk title into the disk table
$db_handle->do("INSERT INTO Disks(Label) VALUES (\"Disk_$disk_number\");");

# Determine what songs from cache can fit onto the CD-ROM
$what = $db_handle->prepare("SELECT SongId, FileName, FileSize FROM Songs WHERE DiskId = 1;");
$what->execute;

while(($SongId, $FileName, $FileSize) = $what->fetchrow_array)
{
	if(($FileSize + $cache_size_bytes) <= $cdrom_size_bytes)
	{
	print "$SongId - $FileName - $FileSize<br>\n";
		$cache_size_bytes += $FileSize;
		system("mv \"/usr/local/Music/$FileName\" /usr/local/Music/BurnCache/");
		$db_handle->do("UPDATE Songs SET DiskId=($disk_number+1) WHERE SongId=$SongId;");
	}
}

print "<b>$cache_size_bytes<br></b>\n";
$what->finish;

$db_handle->disconnect;

# Burn CD-ROM
system("sync");
system("mkisofs -r -V Disk_$disk_number /usr/local/Music/BurnCache/ | cdrecord -v fs=6m speed=4 dev=0,0 -");
