#!/usr/bin/perl -w

#
# author: Nightinggale
#
# Indent XML files
#

use strict;
use warnings;


use XMLaccess;


foreach (getAllXMLfiles())
{
	updateFile($_);
}


sub updateFile
{
	my $filename = $_[0];
	my @lines;
	my $indent = 0;
	my $comments = 0;
	
	
	foreach (openFile($filename))
	{
		my $line = $_;
		if (index($_, '<?xml') != -1 or index($_, '<!--') != -1)
		{
			$comments = 1;
		}

		if ($comments == 0)
		{
	
			if (index($_, '</') != -1 and index($_, '</') == index($_, '<'))
			{
				$indent += -1;
			}
			
			while (substr($line, 0, 1) eq " " or substr($line, 0, 1) eq "	")
			{
				$line = substr($line,1);
			}
			
			my $prefix = "";
			
			for (my $count = 0; $count < $indent; $count++)
			{
				$prefix .= "	";
			}
			
			if (substr($line, 0, 1) eq "<")
			{
				$line = $prefix . $line;
			}
			else
			{
				# this doesn't appear to be proper XML code
				# leave it alone as indenting could have sideeffects
				$line = $_;
			}
			
			if (index($_, '</') == -1 and index($_, '/>') == -1 and index($_, '<') != -1)
			{
				$indent += 1;
			}
		}
		else
		{
			if (index($_, '?>') != -1 or index($_, '-->') != -1)
			{
				$comments = 0;
			}
		}
	
		push ( @lines, $line);
	}
	writeFile(@lines);
}