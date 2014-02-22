#!/usr/bin/perl -w

#
# author: Nightinggale
#
# Update XML files to add new tags to existing files
#

use strict;
use warnings;

# adding to files work as follows:
#
# A file is opened with openFile(path) where path is the path to the file from the XML directory
#
# updateFile(A,B) adds a new line.
# A is the tag it should be added after
# B is the line to add. This line will only be added if the following line
#    doesn't have the new tag (auto read from the new line).
#
# writeFile writes the changes to the file. updateFile() works in memory only for performance reasons.
#
#
# After writeFile has been called, the whole process can start over with openFile for a new file
#

use XMLaccess;

my @lines;


# UnitInfo

@lines = (openFile('Units/CIV4UnitInfos.xml'));

updateFile ("DefaultProfession",       "<bNativesInvalid>0</bNativesInvalid>");
updateFile ("bNativesInvalid",         "<bEuropeInvalid>0</bEuropeInvalid>");
updateFile ("bEuropeInvalid",          "<bColonialInvalid>0</bColonialInvalid>");
updateFile ("bTreasure",               "<bAnimal>0</bAnimal>");
updateFile ("FeatureImpassables",      "<TerrainNatives/>");
updateFile ("TerrainNatives",          "<FeatureNatives/>");
updateFile ("FeatureNatives",          "<BonusNatives/>");
updateFile ("bWaterYieldChanges",      "<YieldDemands/>");
updateFile ("iTeacherWeight",          "<iTeachLevel>0</iTeachLevel>");
updateFile ("iTeachLevel",             "<iAnimalPatrolWeight>-1</iAnimalPatrolWeight>");
updateFile ("iAnimalPatrolWeight",     "<iAnimalAttackWeight>-1</iAnimalAttackWeight>");
updateFile ("iLeaderExperience",       "<ProfessionsNotAllowed/>");
updateFile ("ProfessionsNotAllowed",   "<KnightDubbingWeight>-1</KnightDubbingWeight>");
updateFile ("KnightDubbingWeight",     "<iCasteAttribute>0</iCasteAttribute>");
updateFile ("iCasteAttribute",         "<bPreventFounding>0</bPreventFounding>");
updateFile ("bPreventFounding",        "<bPreventTraveling>0</bPreventTraveling>");
updateFile ("bPreventTraveling",       "<EducationUnitClass>NONE</EducationUnitClass>");
updateFile ("EducationUnitClass",      "<RehibilitateUnitClass>NONE</RehibilitateUnitClass>");
updateFile ("RehibilitateUnitClass",   "<LaborForceUnitClass>NONE</LaborForceUnitClass>");
updateFile ("LaborForceUnitClass",     "<iTradeBonus>0</iTradeBonus>");
updateFile ("iTradeBonus",             "<ConvertsToYield>NONE</ConvertsToYield>");
updateFile ("ConvertsToYield",         "<ConvertsToBuildingClass>NONE</ConvertsToBuildingClass>");
updateFile ("ConvertsToBuildingClass", "<ConvertsToGold>0</ConvertsToGold>");

writeFile(@lines);

@lines = (openFile('Units/CIV4ProfessionInfos.xml'));
updateFile ("Button",       "<ProfessionSubTypes/>");
writeFile(@lines);

# end if setup.

sub updateFile
{
	my $KEYWORD = $_[0];
	my $LINE = $_[1];

	my $keyword_to_check_for = $LINE;

	my $end_index = index ($keyword_to_check_for, '>');

	if ($end_index != -1) {
		$keyword_to_check_for = substr($keyword_to_check_for, 0, $end_index);
	}

	$end_index = index ($keyword_to_check_for, '/');

	if ($end_index != -1) {
		$keyword_to_check_for = substr($keyword_to_check_for, 0, $end_index);
	}

	my $keyword_found = 0;
	my $prefix = '';
	my @temp_array;
	
	foreach (@lines)
	{
		if ($keyword_found == 0)
		{
			if (index($_, '</' . $KEYWORD . '>') != -1 or index($_, '<' . $KEYWORD . '/>') != -1)
			{
				$keyword_found = 1;
				$prefix = substr($_, 0, index($_, '<'));
			}
		} else {
			$keyword_found = 0;
			if (index($_, $keyword_to_check_for) == -1)
			{
				push ( @temp_array, $prefix . $LINE . "\n");
			}
		}
		push ( @temp_array, $_);
	}
	@lines = (@temp_array);
}

