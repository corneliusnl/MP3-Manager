<?
	require("mp3manager.inc");
	mp3manager_header();
?>

<form>
<? mp3manager_box_header("Preferences", "80%"); ?>
<table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr>
	<td align="right" valign="top">
		<b>Search Result Information:</b>
	</td>
	<td>
		<input type="checkbox" name=rating value="1">Rating<br>
		<input type="checkbox" name=title value="1">Title<br>
		<input type="checkbox" name=artist value="1">Artist<br>
		<input type="checkbox" name=disktitle value="1">Disk Title<br>
		<input type="checkbox" name=genre value="1">Genre<br>
		<input type="checkbox" name=filesize value="1">File Size</br>
		<input type="checkbox" name=songlength value="1">Song Length
	</td>
</tr>
<tr>
	<td align="right" valign="top">
		<b>MP3 Song Cache:</b>
	</td>
	<td>
		<input type="text" value="/usr/local/Music">
	</td>
</tr>
<tr>
	<td align="right" valign="top">
		<b>Portable MP3 Player:</b>
	</td>
	<td>
		<input type="checkbox">Present<br>
		<select name="mp3player">
		<option value="Diamond Rio" selected>Diamond Rio</option>
		<option value="Creative Nomad">Creative Nomad</option>
		</select>
		<select name="mp3playersize">
		<option value="32MB" selected>32MB</option>
		<option value="64MB">64MB</option>
		</select>
	</td>
</tr>
<tr>
	<td align="right" valign="top">
		<b>CD-Writer:</b>
	</td>
	<td>
		<input type="checkbox">Present
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

<?
	mp3manager_footer();
?>
