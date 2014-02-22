#!/usr/bin/perl -w

#
# author: Nightinggale
#
# Generate a spreadsheet txt of military professions and print it to STDOUT
# Outputs a table for the forum if first argument is "forum"
#

use strict;
use warnings;


use XMLaccess;


my %tech;

my $seperator = "	";
if ($#ARGV >= 0)
{
	if ($ARGV[0] eq "forum")
	{
		$seperator ="|";
	}
}

foreach (openFile('Units/CIV4ProfessionInfos.xml'))
{
	if (index($_, "<Type>") != -1)
	{
		my $type = substr($_, index($_, ">") +1);
		$type = substr($type, 0, index($type, "<"));
		$tech{$type} = " ";
	}
}

my $type = " ";
my $tag = " ";

foreach (openFile('GameInfo/CIV4CivicInfos.xml'))
{
	if ($tag ne " ")
	{
		if (index($_, "-") == -1)
		{
			$tech{$tag} = $type;
		}
		$tag = " ";
	}
	elsif (index($_, "<Type>") != -1)
	{
		$type = substr($_, index($_, ">") +1);
		$type = substr($type, 0, index($type, "<"));
	}
	elsif (index($_, "<ProfessionType>") != -1)
	{
		$tag = substr($_, index($_, ">") +1);
		$tag = substr($tag, 0, index($tag, "<"));
	}
	
}

my $description;
my $parent;
my $iPower;
my $combat;
my @lines = (openFile('Units/CIV4ProfessionInfos.xml'));

if ($seperator eq "|")
{
	print "[TABLE]";
}

print "Description";
print $seperator;
print "Type";
print $seperator;
print "Parent";
print $seperator;
print "iPower";
print $seperator;
print "Tech";
print "\n";

foreach (@lines)
{
	if (index($_, "<ProfessionInfo>") != -1)
	{
		$parent = " ";
	}
	elsif (index($_, "<Type>") != -1)
	{
		$type = substr($_, index($_, ">") +1);
		$type = substr($type, 0, index($type, "<"));
		if ($parent eq " ")
		{
			$parent = $type;
		}
	}
	elsif (index($_, "<Description>") != -1)
	{
		$description = substr($_, index($_, ">") +1);
		$description = substr($description, 0, index($description, "<"));		
	}
	elsif (index($_, "<iPower>") != -1)
	{
		$iPower = substr($_, index($_, ">") +1);
		$iPower = substr($iPower, 0, index($iPower, "<"));		
	}
	elsif (index($_, "<Combat>") != -1)
	{
		$combat = substr($_, index($_, ">") +1);
		$combat = substr($combat, 0, index($combat, "<"));		
	}
	elsif (index($_, "</ProfessionInfo>") != -1 or index($_, "<ProfessionSubType>") != -1)
	{
		if ($combat ne "NONE")
		{
			print $description;
			print $seperator;
			print $type;
			print $seperator;
			if ($parent ne $type)
			{
				print $parent;
			}
			print $seperator;
			print $iPower;
			print $seperator;
			if ($tech{$type} ne " ")
			{
				print $tech{$type};
			}
			print "\n";
		}
	}
}

if ($seperator eq "|")
{
	print "[/TABLE]\n";
}
