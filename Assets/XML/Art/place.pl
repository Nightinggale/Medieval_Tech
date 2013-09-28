#!/usr/bin/perl
# placeholder art paths

open (ADB, '< CIV4ArtDefines_Building.xml');
while (<ADB>) {
	push (@lines, $_);
	}
close ADB;
open (ADB, '> CIV4ArtDefines_Building.xml');
foreach $line (@lines) {
		if ($line =~ /CityTexture/) {print ADB "\t<CityTexture>,Art/Interface/Screens/City_Management/city_buildings_altas.dds,1,1</CityTexture>\n";}
		elsif ($line =~ /<NIF>/) {print ADB "\t<NIF>Art/Structures/Buildings/Town_Hall/Town_Hall.nif</NIF>\n";}		
		elsif ($line =~ /<Button>/) {print ADB "\t<Button>,Art/Interface/Buttons/Unit_Resource_Colonization_Atlas.dds,4,6</Button>\n";}
		else {print ADB $line;}
		}
close ADB;
@lines = '';

open (ADU, '< CIV4ArtDefines_Unit.xml');
while (<ADU>) {
	push (@lines, $_);
	}
close ADU;
open (ADU, '> CIV4ArtDefines_Unit.xml');
foreach $line (@lines) {
		if ($line =~ /FullLength/) {print ADU "\t<FullLengthIcon>,Art/Interface/Screens/City_Management/Free_Colonist.dds</FullLengthIcon>\n";}
		elsif ($line =~ /<NIF>/) {print ADU "\t<NIF>Art/Units/Free_Colonist/Free_Colonist.nif</NIF>\n";}
		elsif ($line =~ /<KFM>/) {print ADU "\t<KFM>Art/Units/Free_Colonist/Free_Colonist.kfm</KFM>\n";}			
		elsif ($line =~ /<Button>/) {print ADU "\t<Button>,Art/Interface/Buttons/Unit_Resource_Colonization_Atlas.dds,1,3</Button>\n";}
		else {print ADU $line;}
		}
close ADU;
@lines = '';

open (ADI, '< CIV4ArtDefines_Improvement.xml');
while (<ADI>) {
	push (@lines, $_);
	}
close ADI;
open (ADI, '> CIV4ArtDefines_Improvement.xml');
foreach $line (@lines) {
		if ($line =~ /Button/) {print ADI "\t<Button>,Art/Interface/Buttons/Unit_Resource_Colonization_Atlas.dds,5,17</Button>\n";}
		elsif ($line =~ /<NIF>/) {print ADI "\t<NIF>Art/Structures/Improvements/Farm/farm_2x1_building.nif</NIF>\n";}		
		else {print ADI $line;}
		}
close ADI;