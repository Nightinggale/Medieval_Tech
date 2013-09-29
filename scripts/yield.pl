#!/usr/bin/perl

# Script to autogenerate needed lists for yields
# Written by Nightinggale

# arguments (order doesn't matter)
# 2071: input/output is for Colonization 2071 instead of Medieval Conquest
#
# YieldGroup* or Check_YieldGroup*: The return line is generated based on the group in the cpp file
#
# python: outputs the code for CyEnumsInterface.cpp
#
# checkXML: outputs the code for BaseCheckYieldGroup::checkXML in Yields_(mod).cpp

use strict;
use warnings;

my $target = 'Medieval_Tech';
my $group = "";

# read all arguments and look for known keywords
foreach (@ARGV)
{
	if ($_ eq '2071') {
		$target = 'Colonization_2071';
	} elsif (substr($_,0, 10) eq 'YieldGroup' or substr($_,0, 16) eq 'Check_YieldGroup') {
		if (substr($_,0, 1) eq 'C') {
			$group = $_;
		} else {
			$group = 'Check_' . $_;
		}
	} elsif (substr($_,0, 6) eq 'python') {
		$group = 'python';
	} elsif (substr($_,0, 8) eq 'checkXML') {
		$group = 'checkXML';
	} 
}

##
## generate a hash and an array with all yields
##
open (FILE, "< ../DLL_Sources/Yields_" . $target . ".h") or die "Can't open file DLL_Sources/Yields_" . $target . ".h";

my $status = 0;

my @yield_array = ();
my %yield_hash = ();
my %yield_convertion = ();

while (<FILE>) {
	if ($status == 0) {
		if (index($_, 'NO_YIELD = -1') != -1) {
			$status = 1;
		}
	} elsif ($status == 1) {
		if (index($_, 'NUM_YIELD_TYPES') != -1) {
			$status = 2;
		} elsif (index($_, 'YIELD_') != -1) {
			my $line = substr($_,index($_, 'YIELD_'));
			$line = substr($line, 0, index($line, ','));
			$yield_hash{$line} = scalar(@yield_array);
			$yield_convertion{$line} = $line;
			push(@yield_array, $line);
		}
	} elsif ($status == 2) {
		if (substr($_,0,1) eq '}') {
			last;
		} elsif (index($_, '=') != -1) {
			my $left = substr($_, index($_, 'Y'));
			$left = substr($left, 0, index($left, ' ') );
			
			my $right = substr($_, index($_, '='));
			$right = substr($right, index($right, 'Y'));
			$right = substr($right,0,index($right, ',') );
			
			$yield_convertion{$right} = $left;
		}
	}
}

close FILE;

# at this point we can look up yield_names by index in @yield_array and yield_index by name in %yield_hash



sub GetOptimizedgroup{
	my $group = $_[0];
	my @active_yields = ();
	my $status = 0;

	# init all active yields to not being included
	foreach (@yield_array)
	{
		push(@active_yields, -1);
	}

	open (FILE, "< ../DLL_Sources/Yields_" . $target . ".cpp") or die "Can't open file DLL_Sources/Yields_" . $target . ".cpp";
	while (<FILE>) {
		if ($status == 0) {
			if (index($_, $group) != -1) {
				$status = 1;
			}
		} elsif ($status == 1) {
			my $index = index($_, 'YieldVector.push_back');
			if (index($_, '}') != -1) {
				last
			} elsif ($index != -1) {
				my $comment_index = index($_, '//');
				if ($comment_index == -1 or $comment_index > $index)
				{
					my $yield = substr($_,index($_, '(') + 1);
					$yield = substr($yield, 0, index($yield, ')'));
					$active_yields[$yield_hash{$yield}] = $yield_hash{$yield};
				}
			}
		}
	}
	close FILE;
	
	
	print "\treturn ";
	
	my $num_yields = scalar(@yield_array);
	
	my $first = -1;
	my $last = -1;
	my $empty = 1;
	for (@active_yields)
	{
		if ($_ != -1) {
			if ($first == -1)
			{
				$first = $_;
			} else {
				$last = $_;
			}
		} elsif ($first != -1 and $_ == -1) {
			if ($last != -1)
			{
				if ($empty == 0) {
					print " || "
				}
				$empty = 0;
				if (($num_yields - $first) > ($num_yields + ($last - $num_yields)))
				{
					print "(eYield <= " . $yield_array[$last] . " && eYield >= " . $yield_array[$first] . ")";
				} else {
					print "(eYield >= " . $yield_array[$first] . " && eYield <= " . $yield_array[$last] . ")";
				}
			}
			$first = -1;
			$last = -1;
		}
	}
	
	$first = -1;
	$last = -1;
	
	for (@active_yields)
	{
		if ($_ != -1) {
			if ($first == -1)
			{
				$first = $_;
			} else {
				$last = $_;
			}
		} elsif ($first != -1 and $_ == -1) {
			if ($last == -1) {
				if ($empty == 0) {
					print " || "
				}
				$empty = 0;
				print "(eYield == " . $yield_array[$first] . ")";
			}
			$first = -1;
			$last = -1;
		}
	}
	
	if ($empty == 1) {
		print "false";
	}
	print ";\n";
}

sub checkXML {
	print "Fill in Yields_" . $target . ".cpp\n";
	print "in function BaseCheckYieldGroup::checkXML\n";
	for (@yield_array) {
		print "\tcheckSingleXMLType(" . $_ . ",";
		for (my $count = 26; $count >= length $_; $count--) {
			print " ";
		}
		print "\"" . $_ . "\");\n";
	}
}

sub createPythonAccess {
	print "Fill in CyEnumsInterface.cpp\n";
	print "python::enum_<YieldTypes>(\"YieldTypes\")\n";
	print "under " . $target . "\n";
	for (@yield_array) {
		print "\t\t.value(\"" . $yield_convertion{$_} . "\", " . $_ . ")\n";
	}
}


# call the subroutines ordered by command line arguments

if (substr($group,0, 16) eq 'Check_YieldGroup') {
	GetOptimizedgroup($group);
} elsif ($group eq 'python') {
	createPythonAccess();
} elsif ($group eq 'checkXML') {
	checkXML();
}
