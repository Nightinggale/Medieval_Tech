#!/usr/bin/perl -w

#
# author: Nightinggale
#
# Indent XML files
#

use strict;
use warnings;


use XMLaccess;


sub updateFile
{
	my $filename = $_[0];
	my @lines;
	my $indent = 0;
	my $state = 0;
	
	
	foreach (openFile("Civilizations/CIV4CivilizationInfos.xml"))
	{
		
		my $line = $_;
		if ($state == 0)
		{
			if (index($line, "<bNative>1</bNative>") != -1)
			{
				$state = 1;
			}
		}
		else
		{
			if (index($line, "<bNative>0</bNative>") != -1)
			{
				$state = 0;
			}
			else
			{
				if (index($line, ">UNITCLASS_COLONIST<") != -1)
				{
					my $newline = substr($line, 0, index($line, "UNITCLASS_COLONIST"));
					my $endline = substr($line,    index($line, "UNITCLASS_COLONIST"));
					$endline = substr($endline,    index($endline, "<"));
					
					$line = $newline . "UNITCLASS_SERF" . $endline;
				}
			}
		}
	
		push ( @lines, $line);
	}
	writeFile(@lines);
}

updateFile "aa"