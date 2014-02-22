#!/usr/bin/perl -w

#
# author: Nightinggale
#
# This script generates a graph of all techs (civics) with dependancy arrows
# Standard output is a dot file for graphviz
# www.graphviz.org
#
# It will use YOURMOD for XML reading if it is set in Makefile.settings
#

my $xml_location = '../Assets/xml';
my $xml_source_location = $xml_location;

init();

print "digraph G {\n";

my $source_file = $xml_location . "/GameInfo/CIV4CivicInfos.xml";

my $type = "";
my $required = "";
my $used = 1;
my @unused = ();
my %used;

open (READFILE, "< " . $source_file) or die "Can't open file " . $source_file . "\n" . $!;
foreach (<READFILE>)
{
	if (index($_, "<Type>") != -1)
	{
		if ($used == 0)
		{
			push(@unused, $type);
		}
	
		$used = 0;
		$type = substr($_, index($_, ">") + 1);
		$type = substr($type, 0, index($type, "<"));
		$used{$type} = 2;
	}
	
	if (index($_, "</RequiredInvention>") != -1)
	{
		$used = 1;
		$required = substr($_, index($_, ">") + 1);
		$required = substr($required, 0, index($required, "<"));
		print $required . "->" . $type . "\n";
		$used{$required} = 1;
		#print $required . " -> " . $used{$required} . "\n";
	}
}
close READFILE;

print "subgraph cluster_0 {\n";

my $previous = "a";

foreach (@unused)
{
	if ($used{$_} == 2)
	{
		if ($previous ne "a")
		{
			print $_ . " -> " . $previous . ";\n";
		}
		$previous = $_;
	}
}

print "}\n";

print "}\n";

sub init
{
	my $source_dir = $xml_location . '/../../DLL_Sources';
	
	my $settings_file = $source_dir . '/Makefile.settings';
	
	if ( -e $settings_file )
	{
		open (READFILE, "< " . $settings_file) or die "Can't open file " . $settings_file . "\n" . $!;
		my @settings_lines = <READFILE>;
		close READFILE;
	
		foreach (@settings_lines)
		{
			if (substr($_, 0, 7) eq 'YOURMOD')
			{
				my $path = substr($_, 7);
				while (substr($path, 0, 1) eq " " or substr($path, 0, 1) eq "=")
				{
					$path = substr($path, 1);
				}
				
				if (substr($path, 1, 2) eq ":\\")
				{
					if ($^O eq 'MSWin32')
					{
						$xml_location = $path;
					} else {
						$xml_location = substr($path, 2);
					}
				}
				else
				{
					$xml_location = $source_dir . "/" . $path;
				}
				
				if ($^O ne 'MSWin32')
				{
					$xml_location =~ s/\\/\//g;
				}
				$xml_location =~ s/[\x0A\x0D]//g;
				chomp($xml_location);
				
				$xml_location = $xml_location . "/Assets/xml";
				return;
			}
		}
	}
}