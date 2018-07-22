<?
	require("mp3manager.inc");
	mp3manager_header();
?>

<form action="writecache.cgi" method="post">
<? mp3manager_box_header("Transfer to CD-ROM", "80%"); ?>
Proceeding will remove the songs from the harddrive cache
directory and write them to CD-ROM.  The information will
still remain in the online database.
<p>

<input type="submit" value="Burn CD-ROM">
<? mp3manager_box_footer(); ?>
</form>

<?
	mp3manager_footer();
?>
