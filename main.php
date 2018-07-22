<?
	require("../../common.inc");
	common_header("MP3Manager");
	common_navbar();
?>

<h1>Table Of Contents</h1>
<ul>
<li><a href="#intro">Introduction</a></li>
<li><a href="#demo">Online Demo</a></li>
<li><a href="#downloads">Downloads</a></li>
</ul>

<h1>Introduction</h1>
Welcome to mp3manager, using this program you could (ideally, and
theoretically) maintain an online searchable database of your MP3
music collection... of course you can't because this is my music collection :)
<p>
It will take your music extract it's ID3 tag and write the information
into the MySQL server database, if tag is not present it will prompt for the
artist and song title.  It will also record the song length, bit rate,
filesize, and check for errors in the MP3 file.  If no errors are present, and
the song does not already exist in the database, the song will be named in
the "Artist - Title.mp3" format and moved into a temporary cache.  When the
size of the files in the temporary cache has reached critical mass (ie.
>= 650MB) a cd-rom will be generated.
<p>
All the songs in the database can be retrieved along with their information
which can be modified until they are burnt to cd-rom.  Songs can be downloaded,
played, or selected.  The selection feature allows you to select with songs
you want to burn to MP3 CD, audio CD, or portable MP3 player.

<h1><a name="demo">Online Demo</a></h1>
A sample collection of music managed by MP3Manager is available online
<a href="main.php">here</a>.

<h1><a name="downloads">Downloads</a></h1>
Not available at this time.

<?
	common_footer();
?>
