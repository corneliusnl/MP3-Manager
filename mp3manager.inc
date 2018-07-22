<?

function mp3manager_header($title)
{
	print "<body bgcolor=\"white\" marginwidth=\"0\" marginheight=\"0\" topmargin=\"0\" leftmargin=\"0\" rightmargin=\"0\" bottommargin=\"0\">\n\n";

	print "<table width=\"100%\"  border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n";
	print "<tr align=\"left\" valign=\"top\">\n";
	print "<td bgcolor=\"#3a6f7a\">\n";
	print "<img src=\"mp3manager.png\">\n";
	print "</td>\n";
	print "<td width=\"50%\" bgcolor=\"#3a6f7a\" valign=\"center\">\n";
	
	$conn = mysql_connect("neurotoxin.dhs.org", "root", "convert") or die("Unable to connect to SQL server!");
	mysql_select_db("mp3manager", $conn) or die("Unable to access database!");

	$result = mysql_query("SELECT * FROM Songs;", $conn);
	$numsongs = mysql_num_rows($result);
		
	$result = mysql_query("SELECT SUM(Filesize) FROM Songs;", $conn);
	$row = mysql_fetch_row($result);
	$sizemb = intval((int)$row[0]/1048576);

	$result = mysql_query("SELECT * FROM Disks;", $conn);
	$numcds = mysql_num_rows($result);
	$numcds--;
		
	print "\t\t$numsongs Songs in Database<br>\n";
	print "\t\t$numcds CD's in Collection<br>\n";
	print "\t\t$sizemb MB in Collection<br>\n";

	$result = mysql_query("SELECT SUM(Filesize) FROM Songs WHERE DiskId = 1;");
	$row = mysql_fetch_row($result);
	$size = (int)$row[0];
	$sizemb = intval($size/1048576);

	if($size >= 681984000)
	{
		print "\t\t$sizemb MB in Cache (<a href=\"writecache.php\">Transfer to CD</a>)\n";
	}
	else
	{
		print "\t\t$sizemb MB in Cache\n";
	}

	mysql_close($conn);

	
	print "</td>\n";
	print "</tr>\n";
	print "<tr>\n";
	print "<td bgcolor=\"black\" colspan=\"2\">\n";
	print "<img src=\"blank.gif\" height=\"2\" vspace=\"0\"></td>\n";
	print "</tr>\n";
	print "<tr align=\"center\">\n";
	print "<td bgcolor=\"lightgrey\" colspan=\"2\">\n";
	print "[<a href=\"index.php\">Index</a> |\n";
	print "<a href=\"add.php\">Add</a> |\n";
	print "<a href=\"browse.php\">Browse</a> |\n";
	print "<a href=\"search.php\">Search</a> |\n";
	print "<a href=\"preferences.php\">Edit Preferences |</a>\n";
	print "<a href=\"createcd.php\">Create CD |</a>\n";
	print "<a href=\"upload.php\">Upload Songs</a>]\n";
	print "</td>\n";
	print "</tr>\n";
	print "<tr>\n";
	print "<td bgcolor=\"black\" colspan=\"2\">\n";
	print "<img src=\"blank.gif\" height=\"2\" vspace=\"0\"></td>\n";
	print "</tr>\n";
	print "</table>\n";
	print "<font face=\"tahoma, verdana, arial, helvetica, sans-serif\">\n";
}

function mp3manager_footer()
{
	print "</font>\n";

	print "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n";
	print "<tr>\n";
	print "<td bgcolor=\"black\" colspan=\"2\">\n";
	print "<img src=\"blank.gif\" height=\"2\" vspace=\"0\"></td>\n";
	print "</tr>\n";
	print "<tr align=\"center\">\n";
	print "<td bgcolor=\"lightgrey\" colspan=\"2\">\n";
	print "[<a href=\"index.php\">Index</a> |\n";
	print "<a href=\"add.php\">Add</a> |\n";
	print "<a href=\"browse.php\">Browse</a> |\n";
	print "<a href=\"search.php\">Search</a> |\n";
	print "<a href=\"preferences.php\">Edit Preferences |</a>\n";
	print "<a href=\"createcd.php\">Create CD |</a>\n";
	print "<a href=\"upload.php\">Upload Songs</a>]\n";
	print "</td>\n";
	print "</tr>\n";
	print "<tr>\n";
	print "<td bgcolor=\"black\" colspan=\"2\">\n";
	print "<img src=\"blank.gif\" height=\"2\" vspace=\"0\"></td>\n";
	print "</tr>\n";
	print "<tr align=\"left\" valign=\"top\">\n";
	print "<td bgcolor=\"#3a6f7a\">\n";
	print "<img src=\"mp3manager.png\">\n";
	print "</td>\n";
	print "<td width=\"50%\" bgcolor=\"#3a6f7a\" valign=\"center\">\n";
	print "Play Mode<br>\n";
	print "? Songs Selected<br>\n";
	print "? in Play Length<br>\n";
	print "? Total Size in Megabytes\n";
	print "</td>\n";
	print "</tr>\n";
	print "</table>\n";
}

function mp3manager_box_header($caption, $width)
{
	echo("<p>\n");
	echo("<center>\n");
	echo("<table width=\"$width\" border=\"0\" cellpadding=\"2\" cellspacing=\"2\">\n");
	echo("<tr>\n");
	echo("<td bgcolor=\"black\">\n");
	echo("<table width=\"100%\" bgcolor=\"white\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
	echo("<tr bgcolor=\"#c0c0c0\" align=\"left\">\n");
	echo("<td>\n");
	echo("<b><font size=\"4\">$caption</font></b>\n");
	echo("</td>\n");
	echo("</tr>\n");
	echo("<tr>\n");
	echo("<td bgcolor=\"black\">\n");
	echo("<img src=\"blank.gif\" height=\"2\" vspace=\"0\"></td>\n");
	echo("</tr>\n");
	echo("<tr>\n");
	echo("<td>\n");
}

function mp3manager_box_footer()
{
	echo("</td>\n");
	echo("</tr>\n");
	echo("</table>\n");
	echo("</td>\n");
	echo("</tr>\n");
	echo("</table\n");
	echo("</center>\n");
	echo("<p>\n");
}
	
function mp3manager_search($where, $sortby, $offset, $pagename)
{
	$conn = mysql_connect("neurotoxin.dhs.org", "jmccarth", "convert") or die("Unable to connect to SQL server!");
	mysql_select_db("mp3manager", $conn) or die("Unable to access database!");

/*	if(empty($offset))
	{
		$offset = 0;
	}*/
	
	$result = mysql_query("SELECT * FROM Songs WHERE $where ORDER BY $sortby", $conn);
	$numrows = mysql_num_rows($result) or die("Unable to comply\n");
	
	if($offset)
	{
		$newoffset = $offset - 20;
		$navigator .= "[ <a href=\"$pagename&offset=$newoffset\">Prev</a>\n";
	}
	else
	{
		$navigator .= "[ Prev \n";
	}

	$pages = intval($numrows / 20);

	if($numrows % 20)
	{
		$pages++;
	}

	for($i = 1; $i <= $pages; $i++)
	{
		$newoffset = 20 * ($i-1);
		if($offset != $newoffset)
		{
			$navigator .= "| <a href=\"$pagename&offset=$newoffset\">$i</a>\n";
		}
		else
		{
			$navigator .= "| $i\n";
		}
	}

	if((($offset / 20) != $pages-1) && ($pages != 1))
	{
		$newoffset = $offset + 20;
		$navigator .= "| <a href=\"$pagename&offset=$newoffset\">Next</a>] \n";
	}
	else
	{
		$navigator .= "| Next ]\n";
	}

	print "<p>\n";
	print "<center>\n";
	print "Found $numrows matching entries in database.<br>\n";
	print "$navigator\n";
	print "</center>\n";

	print "<form>\n";
	mp3manager_box_header("Search Results", "98%");
	print "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n";
	print "<tr bgcolor=\"black\" align=\"left\">\n";
	print "<td>\n";
	print "<img src=\"blank.gif\">\n";
	print "</td>\n";
	print "<td width=\"30%\" align=\"left\">\n";
	print "<font color=\"white\">Artist Name</font>\n";
	print "</td>\n";
	print "<td width=\"60%\">\n";
	print "<font color=\"white\">Song Title</font>\n";
	print "</td>\n";
	print "<td width=\"10%\">\n";
	print "<font color=\"white\">Disk</font>\n";
	print "</td>\n";
	print "<td>\n";
	print "<img src=\"blank.gif\">\n";
	print "</td>\n";
	print "</tr>\n";
	print "<tr>\n";
	print "<td bgcolor=\"black\" colspan=\"5\">\n";
	print "<img src=\"blank.gif\" height=\"2\" vspace=\"0\"></td>\n";
	print "</tr>\n";

	$result = mysql_query("SELECT Artist,Title,SongId,FileName,Label FROM Songs,Disks WHERE Songs.DiskId=Disks.DiskId AND $where ORDER BY $sortby LIMIT $offset,20", $conn);

//	if($result)
//	{
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
		print "<td>\n";
		print "<input type=\"checkbox\">\n";
		print "</td>\n";
		print "<td>\n";
		print "$data[0]\n";
		print "</td>\n";
		print "<td>\n";
		print "$data[1]\n";
		print "</td>\n";
		print "<td>\n";
		print "$data[4]\n";
		print "</td>\n";
		print "<td>\n";
		print "<a href=\"javascript:OpenWin('info.php?id=$data[2]', 'Song Information', '600', '400');\"><img src=\"info.gif\" border=\"0\" alt=\"Song Information\"></a>\n";
		print "<a href=\"info.php?id=$data[2]\"><img src=\"down.gif\" border=\"0\" alt=\"Download Song\"></a>\n";
		print "<a href=\"/usr/local/Music/$data[3]\"><img src=\"play.gif\" border=\"0\" alt=\"Play Song\"></a>\n";
		print "</td>\n";
		print "</tr>\n";
	}
//	}
	mysql_close();

	print "<tr>\n";
	print "<td bgcolor=\"#000000\" colspan=\"5\">\n";
	print "<input type=\"button\" value=\"Select All\">\n";
	print "<input type=\"button\" value=\"Unselect All\">\n";
	print "<input type=\"button\" value=\"Review Selection\">\n";
	print "</td>\n";
	print "</tr>\n";
	print "</table>\n";
	mp3manager_box_footer();
	print "</form>\n";
	print "<center>\n";
	print "$navigator<p>\n";
	print "</center>\n";
}
?>

