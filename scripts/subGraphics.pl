#!/usr/bin/perl -w

#
# author: Nightinggale
#
# Update XML files to add graphics to subtypes
#

use strict;
use warnings;


# setup (global definitions, which has to be first)
my $xml_location = '../Assets/xml';
my $xml_source_location = $xml_location;
my $to_file = 1;
my $FILE = "INIT";
my @lines;
init();



openFile('Units/CIV4ProfessionInfos.xml');

my $type = "";
my %subTypes = ();

foreach (@lines)
{
	if (index($_, '</ProfessionInfo>') != -1)
	{
		$type = "";
	} elsif (index($_, '<Type>') != -1)
	{
		my $new_type = substr($_, index($_, '>')+1);
		$new_type = substr($new_type, 0, index($new_type, '<'));
		$subTypes{$new_type} = "1";
		if ( $type ne "" )
		{
			$subTypes{$type} = $new_type;
		}
		$type = $new_type;
	}
}
$subTypes{'NONE'} = "1";

openFile('Civilizations/CIV4UnitArtStyleTypeInfos.xml');
updateFile('StyleUnit');
writeFile();

openFile('Units/CIV4UnitInfos.xml');
updateFile('UnitMeshGroups');
writeFile();



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

				print "Using XML location: " . $xml_location . "\n";
				return;
			}
		}
	}
	print "Using XML location: " . $xml_location . "\n";
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
	print "Updating file $FILE\n";
	
	my $DESTINATION_FILE = $xml_location . '/' . $FILE;
	open (READFILE, "< " . $DESTINATION_FILE) or die "Can't open file " . $DESTINATION_FILE . "\n" . $!;
	@lines = <READFILE>;
	close READFILE;
}

sub writeFile
{
	my $DESTINATION_FILE = $xml_location . '/' . $FILE;
	if ($to_file != 0) {
		open (WRITE_FILE, "> " . $DESTINATION_FILE) or die "Can't open file " . $DESTINATION_FILE . "\n" . $!;
	}
	foreach (@lines)
	{
		writeoutput $_;
	}
	close WRITE_FILE;
}

sub updateFile
{
	my $KEYWORD = $_[0];

	my @temp_array;
	my $state = 0;
	
	my $old_type = "";
	
	my @token;
	my @old_token;
	
	
	foreach (@lines)
	{
		if (index($_, '<ProfessionType>') != -1)
		{
			my $new_type = substr($_, index($_, '>')+1);
			$new_type = substr($new_type, 0, index($new_type, '<'));

			while ($old_type ne "" and $subTypes{$old_type} ne "1" and $subTypes{$old_type} ne $new_type)
			{
				foreach (@old_token)
				{
					if (index($_, '<ProfessionType>') != -1)
					{
						$old_type = $subTypes{$old_type};
						push ( @temp_array, substr($_, 0, index($_, '>')+1) . $old_type . "</ProfessionType>\n");
					} else {
						push ( @temp_array, $_);
					}
				}
			}
			$old_type = $new_type;
			
		}
		elsif (index($_, '<' . $KEYWORD . '>') != -1)
		{
			$state = 1;
		}
		elsif (index($_, '</' . $KEYWORD . '>') != -1)
		{
			$state = 0;
			@old_token = (@token);
			push ( @old_token, $_);
			
			foreach (@token)
			{
				push ( @temp_array, $_);
			}
			
			@token = ();
			
		}
		elsif ($state == 2)
		{
			$state = 0;
		}
		
		if ($state == 1)
		{
			push ( @token, $_);
		} else {
			push ( @temp_array, $_);
		}
	}
	@lines = (@temp_array);
}

