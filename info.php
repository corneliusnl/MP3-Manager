<html>
<body bgcolor="#ffffff">

<?
	require("mp3manager.inc");
//	mp3manager_header();

	if($phase == "two")
	{
		$conn = mysql_connect("neurotoxin.dhs.org", "jmccarth", "convert") or die("Unable to connect to SQL server!");
		mysql_select_db("mp3manager", $conn) or die("Unable to access database!");
		print "Thank You.\n";
	}
	else
	{
		$conn = mysql_connect("neurotoxin.dhs.org", "jmccarth", "convert") or die("Unable to connect to SQL server!");
		mysql_select_db("mp3manager", $conn) or die("Unable to access database!");

		$result = mysql_query("SELECT Artist, Title, Album, Genre, Year, Rating, Length, BitRate, Label, FileSize FROM Songs, Disks WHERE SongId = $id AND Songs.DiskId = Disks.DiskId;");
		$data = mysql_fetch_row($result);
		mysql_close();

?>

<form action="changeinfo.cgi" method="post">
<? mp3manager_box_header("Song Information", "95%"); ?>
<table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr>
	<td align="right">
		<b>Artist:</b>
	</td>
	<td>
<?php		
		if($data[8] == "CD_Cache")
		{
			print "<input type=\"text\" name=\"artist\" value=\"$data[0]\" size=\"30\" maxlength=\"60\">\n";
		}
		else
		{
			print "$data[0]\n";
		}
?>
	</td>
</tr>
<tr>
	<td align="right">
		<b>Song Title:</b>
	</td>
	<td>
<?php	
		if($data[8] == "CD_Cache")
		{
			print "<input type=\"text\" name=\"artist\" value=\"$data[1]\" size=\"30\" maxlength=\"60\">\n";
		}
		else
		{
			print "$data[1]\n";
		}
		
?>
	</td>
</tr>
<tr>
	<td align="right">
		<b>Album:</b>
	</td>
	<td>
<?php	
		if($data[8] == "CD_Cache")
		{
			print "<input type=\"text\" name=\"artist\" value=\"$data[2]\" size=\"30\" maxlength=\"60\">\n";
		}
		else
		{
			print "$data[2]\n";
		}
?>
	</td>
</tr>
<tr>
	<td align="right">
		<b>Genre:</b>
	</td>
	<td>
<?php
		if($data[8] == "CD_Cache")
		{
			print "<input type=\"text\" name=\"artist\" value=\"$data[3]\" size=\"20\" maxlength=\"20\">\n";
		}
		else
		{
			print "$data[3]\n";
		}
?>
	</td>
</tr>
<tr>
	<td align="right">
		<b>Year:</b>
	</td>
	<td>
<?php
		if($data[8] == "CD_Cache")
		{
			print "<input type=\"text\" name=\"artist\" value=\"$data[4]\" size=\"4\" maxlength=\"4\">\n";
		}
		else
		{
			print "$data[4]\n";
		}
?>
	</td>
</tr>
<tr>
	<td align="right">
		<b>Rating:</b>
	</td>
	<td>
		<input type="checkbox" name="star1">
		<input type="checkbox" name="star2">
		<input type="checkbox" name="star3">
		<input type="checkbox" name="star4">
		<input type="checkbox" name="star5">
	</td>
</tr>
<tr>
	<td align="right">
		<b>Song Length:</b>
	</td>
	<td>
<?php
		$min = intval((int)$data[6] / 60);
		$sec = (int)$data[6] % 60;
		$format = sprintf("%d:%02d", $min, $sec);
		print "$format\n";
?>
	</td>
</tr>
<tr>
	<td align="right">
		<b>Bitrate:</b>
	</td>
	<td>
<?php		print "$data[7]\n";	?>
	</td>
</tr>
<tr>
	<td align="right">
		<b>Disk Label:</b>
	</td>
	<td>
<?php		print "$data[8]\n";	?>
	</td>
</tr>
<tr>
	<td align="right">
		<b>Filesize:</b>
	</td>
	<td>
<?php		print number_format($data[9], 0) . "\n";	?>
	</td>
</tr>
<input type="hidden" name="phase" value="two"> 
<tr bgcolor="lightgrey">
	<td colspan="2" align="center">
		<input type="submit" value="Ok" onclick="javascript:window.close()">
		<input type="button" value="Cancel" onclick="javascript:window.close()">
	</td>
</tr>
</table>
<? mp3manager_box_footer(); ?>
</form>

<?
	}
//	mp3manager_footer();
?>

</body>
</html>
