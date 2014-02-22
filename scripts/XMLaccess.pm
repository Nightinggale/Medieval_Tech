#!/usr/bin/perl -w

#
# author: Nightinggale
#
# Simplified open and close XML files
# YOURMOD path is used to find XML files
# This mean master branch scripts can target files in other branches
#
# openFile(FILEPATH)
#   reads the entire file into an array and returns the array
# 
# writeFile(array)
#   overwrites a file with the array given in the argument
#   the file in question is the last one given to openFile()
#
# getXMLlocation()
#   returns the path to the XML root dir (often not needed)
#

use strict;
use warnings;

package XMLaccess;

use Exporter;
our @ISA= qw( Exporter );
our @EXPORT = qw( openFile writeFile getXMLlocation getAllXMLfiles);


my $xml_location = '../Assets/xml';
my $xml_source_location = $xml_location;
my $to_file = 1;
my $FILE = "INIT";

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

				last;
			}
		}
	}
	print "Using XML location: " . $xml_location . "\n";
}

sub getXMLlocation()
{
	return $xml_location;
}

sub writeoutput
{
	if ($to_file == 0) {
		print $_[0];
	} else {
		print WRITE_FILE $_[0];
	}
}

sub openFile
{
	$FILE = $_[0];
	
	if ($xml_source_location ne $xml_location)
	{
		if (index($FILE, "/") != -1)
		{
			my $subdir = substr($FILE, 0, index($FILE, "/"));
			
			my $schemaFile = "";
			my $dirname = $xml_source_location . "/" . $subdir;
			
			opendir my($dh), $dirname or die "Couldn't open dir '$dirname': $!";
			foreach (readdir $dh)
			{
				if (index($_, "Schema") != -1)
				{
					$schemaFile = $_;
				} 
			}
			closedir $dh;
			
			if ($schemaFile ne "" )
			{
				print "Copying " . $schemaFile . "\n";
				
				my $source_file = $dirname . "/" . $schemaFile;
				my $destination_file = $xml_location . "/" . $subdir . "/" . $schemaFile;
				
				open (READFILE, "< " . $source_file) or die "Can't open file " . $source_file . "\n" . $!;
				open (WRITE_FILE, "> " . $destination_file) or die "Can't open file " . $destination_file . "\n" . $!;
				
				foreach (<READFILE>)
				{
					print WRITE_FILE $_;
				}
				
				close READFILE;
				close WRITE_FILE;
			}
		}
	}
	print "Loading file $FILE\n";
	
	my $DESTINATION_FILE = $xml_location . '/' . $FILE;
	open (READFILE, "< " . $DESTINATION_FILE) or die "Can't open file " . $DESTINATION_FILE . "\n" . $!;
	my @lines = <READFILE>;
	close READFILE;
	return @lines;
}

sub writeFile
{
	my $DESTINATION_FILE = $xml_location . '/' . $FILE;
	if ($to_file != 0) {
		open (WRITE_FILE, "> " . $DESTINATION_FILE) or die "Can't open file " . $DESTINATION_FILE . "\n" . $!;
	}
	foreach (@_)
	{
		writeoutput $_;
	}
	close WRITE_FILE;
}

sub getXMLfilesInDir
{
	my $dirname = $_[0];
	
	my @return_array = ();
	
	my $pathdirname = getXMLlocation() . "/" . $dirname;
	
	opendir my($dh), $pathdirname or die "Couldn't open dir '$pathdirname': $!";
	my @files = grep { !/^\.\.?$/ } readdir $dh;
	closedir $dh;

	foreach (@files)
	{
		my $filename = $dirname . "/" . $_;
		my $pathfilename = getXMLlocation() . "/" . $filename;
		if (-d $pathfilename)
		{
			foreach (getXMLfilesInDir($filename))
			{
				push(@return_array, $_);
			}
		}
		elsif (substr($_, -4) eq '.xml')
		{
			push(@return_array, $filename);
		}
	}
	return @return_array;
}

sub getAllXMLfiles
{
	return getXMLfilesInDir(".");
}

