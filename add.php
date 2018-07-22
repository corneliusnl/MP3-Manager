<?
	require("mp3manager.inc");
	mp3manager_header("Add to database");
?>

<form action="add.php" enctype="multipart/form-data" method="get">
<? mp3manager_box_header("Add Songs", "98%"); ?>
<table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr>
	<td align="right">
		<b>Path:</b>
	</td>
	<td>
		<input type="file" name="files">
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
