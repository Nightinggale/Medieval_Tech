#!/usr/bin/perl

# generate source code for wiki pages

use strict;
use warnings;

sub MakeFiles {
	my $path = $_[0];
	my $schema = $_[1];

}

sub checkDir {
	my $directory = $_[0];  # '../Assets/XML';

	print "Trying dir: " . $directory . "\n";

	opendir (DIR, $directory) or die $!;

	my @directories = ();

	while (my $file = readdir(DIR)) {
		next if ($file =~ m/^\./);

		if (index($file, 'Schema.xml') != -1) {
			print "schema $file\n";
		}

		next unless (-d "$directory/$file");
		push(@directories, $file);
    }
	closedir(DIR);
	foreach(@directories) {
		checkDir($directory . "/" . $_);
	}
}

checkDir('../Assets/XML/Terrain');

exit();


foreach (@ARGV)
{
	print  $_ . "\n";
}