#!/usr/bin/perl

sub ReadParse
{
	if(@_)
	{
		local (*in) = @_;
	}

	local ($i, $loc, $key, $val);

	if($ENV{'REQUEST_METHOD'} eq "GET")
	{
		$in = $ENV{'QUERY_STRING'};
	}
	elsif ($ENV{'REQUEST_METHOD'} eq "POST")
	{
		for($i = 0; $i < $ENV{'CONTENT_LENGTH'}; $i++)
		{
			$in .= getc;
		}
	}

	@in = split(/&/, $in);

	foreach $i (0 .. $#in)
	{
		$in[$i] =~ s/\+/ /g;

		$in[$i] =~ s/%(..)/pack("c",hex($1))/ge;

		$loc = index($in[$i], "=");
		$key = substr($in[$i], 0, $loc);
		$val = substr($in[$i], $loc+1);
		$in{$key} .= '\0' if (defined($in{$key}));
		$in{$key} .= $val;
	}
	return 1;
}

1;
