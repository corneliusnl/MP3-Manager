<html>
<title>Directory Test</title>


<?php
	mysql_pconnect("localhost", "nobody", "") or die("Unable to connect to SQL server!");
	mysql_select_db("WebPage") or die("Unable to select database");

<!--	$dir = opendir("/cdrom");
	while($file = readdir($dir))
	{
		$artist = strtok($file, "-");
		$title = strtok(".");
		echo "<B>$artist</B><I>$title</I><BR>";
	}
	closedir($dir); !-->
?>

</html>
