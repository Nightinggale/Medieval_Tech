#!/usr/bin/perl -w

#
# author: Nightinggale
#
# Replace an XML tag with another one
# The script replaces first argument with second argument
#
# Example:
# rename.pl BAD_TAG GOOD_TAG
# this will replace all cases of >BAD_TAG< with >GOOD_TAG< in all XML files

use strict;
use warnings;


use XMLaccess;


if ($#ARGV == 1)
{
	replace($ARGV[0], $ARGV[1]);
}

sub replace
{
	my $string = $_[0];
	my $replacement = ">" . $_[1];
	$string = ">" . $string . "<";
	foreach (getAllXMLfiles())
	{
		my @lines = openFile($_);
		foreach (@lines)
		{
			if (index($_, $string) != -1)
			{
				# line found. Rewrite file
				my @output = ();
				foreach (@lines)
				{
					my $offset = index($_, $string);
					if ($offset != -1)
					{
						my $prefix = substr($_, 0, $offset);
						my $postfix = substr($_, $offset);
						$postfix = substr($postfix, index($postfix, "<"));
						push(@output, $prefix . $replacement . $postfix);
					}
					else
					{
						push(@output, $_);
					}
				}
				writeFile(@output);
				last;
			}
		}
	}
}