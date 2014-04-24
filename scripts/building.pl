#!/usr/bin/perl -w

#
# author: Nightinggale
#
# 
#

use strict;
use warnings;
use XMLaccess;

my @lines;


# UnitInfo

@lines = (openFile('../Python/Screens/CvMainInterface.py'));

my @buildings = ();

my $string = "BUILDING_DATA_MC[";

foreach (@lines)
{
	if (substr($_, 0, length $string) eq $string)
	{
		my $x = substr($_, index($_, "="));
		$x = substr($x, index($x, "[")+1);
		
		my $y = substr($x, index($x, ",")+2);
		
		$x = substr($x, 0, index($x, ","));
		$y = substr($y, 0, index($y, ","));

		push(@buildings, [$x, $y]);
	}
}


@lines = (openFile('Buildings/CIV4SpecialBuildingInfos.xml'));

my @output = ();
my $counter = 0;

foreach (@lines)
{
	push(@output, $_);
	if (index ($_, "</FontButtonIndex>") != -1)
	{
		#push(@output, "<posX>" . $buildings[counter][0] . "/<posX>");
		my $x = "<iPosX>" . $buildings[$counter][0] . "</iPosX>" . "\n";
		my $y = "<iPosY>" . $buildings[$counter][1] . "</iPosY>" . "\n";
	
		my $size_x = "<iSizeX>18</iSizeX>\n";
		my $size_y = "<iSizeY>11</iSizeY>\n";
	
		push(@output, $x);
		push(@output, $y);
		push(@output, $size_x);
		push(@output, $size_y);
	
		$counter = $counter + 1;
	}
}

writeFile @output;
