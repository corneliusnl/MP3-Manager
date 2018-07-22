<?

require("mp3manager.inc");
mp3manager_header();

$conn = mysql_connect("neurotoxin.dhs.org", "root", "convert") or die("Unable to connect to SQL server!");
mysql_select_db("mp3manager", $conn) or die("Unable to access database!");
$result = mysql_query("SELECT Label, DiskId FROM Disks ORDER BY DiskId;", $conn);
	
if(!$result)
{
	die("No Entries in Database!");
}

if(empty($DiskId))
{
	mp3manager_box_header("Disk Title", "60%");

	print "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n";
	while($data = mysql_fetch_row($result))
	{
		if($alternate == "white")
		{
			$alternate = "lightgrey";
		}
		else
		{
			$alternate = "white";
		}
	
		print "<tr bgcolor=\"$alternate\">\n";
		print "\t<td align=\"center\">\n";
		$data[0] = htmlentities($data[0]);
		print "\t\t<a href=\"$PHP_SELF?DiskId=$data[1]\">$data[0]</a>\n";
		print "\t</td>\n";
		print "</tr>\n";
	}
	mysql_close();
	print "</table>\n";
	mp3manager_box_footer();
}
else
{
	if(empty($offset))
	{
		$offset = 0;
	}
	
	mp3manager_search("Songs.DiskId=$DiskId", "Artist", "$offset", "$PHP_SELF?DiskId=$DiskId");
}

mp3manager_footer();

?>
