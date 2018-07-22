<?
	require("mp3manager.inc");
	mp3manager_header();
?>

<script language="JavaScript1.2">
function OpenWin(url, name, w, h) {
	window.open(url, name, 'width='+w+',height='+h+',status=no,resizeable=no,scrollbars=0');
}
</script>

<form action="search.php" method="get">
<? mp3manager_box_header("Search", "70%"); ?>
<table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr>
	<td align="right">
		<b>Search:</b>
	</td>
	<td>
<?php
		print "<input type=\"text\" name=\"search\" value=\"$search\">\n";
?>
	</td>
</tr>
<tr>
	<td align="right">
		<b>Sort by:</b>
	</td>
	<td>
		<select name="sortby">
		<option value="Artist" selected>Artist</option>
		<option value="Title">Song Title</option>
		<option value="Year">Year</option>
		<option value="Genre">Genre</option>
		<option value="Album">Album</option>
		<option value="Rating">Rating</option>
		<option value="DiskLabel">Disk Label</option>
		</select>
	</td>
</tr>
<tr bgcolor="lightgrey">
	<td colspan="2" align="center">
		<input type="submit" value="Ok">
		<input type="reset" value="Reset">
	</td>
</tr>
</table>
<? mp3manager_box_footer(); ?>
</form>

<?php

if(!empty($search))
{
	if(empty($offset))
	{
		$offset = 0;
	}
	
	mp3manager_search("Artist LIKE '%$search%' OR Title LIKE '%$search%'", "$sortby", "$offset", "$PHP_SELF?search=$search&sortby=$sortby");
}

	mp3manager_footer();
?>
