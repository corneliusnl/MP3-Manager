<?
	require("mp3manager.inc");

	mp3manager_header();
?>

<? mp3manager_box_header("Attention!", "90%"); ?>
In order to gain access to modify database and use the fully
functionality of the mp3 management system, a valid username
and password must be provided below.  Otherwise, if you'd just
like to browse the database, just leave the login name and password blank and
click on login.
<? mp3manager_box_footer(); ?>

<form>
<? mp3manager_box_header("Login", "50%"); ?>
<table border="0" width="100%" cellspacing="0" cellpadding="0">
<tr><td align="right"><b>Username:</b></td><td><input type="text"></td></tr>
<tr><td align="right"><b>Password:</b></td><td><input type="password"></td></tr>
<tr><td bgcolor="lightgrey" colspan="2"><input type="submit" value="login"><input type="reset"></td></tr>
</table>
<? mp3manager_box_footer(); ?>
</form>

<?
	mp3manager_footer();
?>
