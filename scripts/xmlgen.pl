#!/usr/bin/perl
# generate XML for yields and professions

use Lingua::EN::Inflect qw ( PL PL_N PL_V PL_ADJ NO NUM A AN def_noun def_verb def_adj def_a def_an );
def_noun "Facility"  => "Facilities";
def_noun "Foundry"  => "Foundries";
def_noun "Refinery"  => "Refineries";
def_noun "Celebrity"  => "Celebrities";
def_noun "Mine"  => "Mines";

# ** CONTENT ARRAYS **

# arrays of yields
@cargoyields = ("Nutrients" , "Biopolymers" , "Silicates" , "Base Metals" , "Actinides" , "Isotopes" , "Rare Earths" , "Crystalloids" , "Nucleic Acids" , "Amino Acids" , "Tissue Samples" , "Microbes" , "Datacores" , "Progenitor Artifacts" , "Alien Specimens" , "Precious Metals" , "Opiates" , "Xenotoxins" , "Botanicals" , "Hydrocarbons" , "Clathrates" , "Core Samples" , "Machine Tools" , "Robotics" , "Munitions" , "Photonics" , "Plasteel" , "Duralloy" , "Crystalloy" , "Nucleonics" , "Fusion Cores" , "Semiconductors" , "Plasmids" , "Enzymes" , "Stem Cells" , "State Secrets" , "Progenitor Tech" , "Alien Relics" , "Narcotics" , "Bioweapons" , "Pharmaceuticals" , "Petrochemicals" , "Colloids" , "Catalysts", "Hard Currency", "Earth Goods", "Contraband", "Earthling Specimens");
# array of abstract yields
@abstractyields = ("Industry", "Media", "Liberty", "Research", "Education", "Influence", "Energy", "Pollutants", "Credits");
@allyields = (@cargoyields,@abstractyields);

# improvements, goodyhuts and improvement builds
@builds = ('Farm','Mill','Quarry','Mine','Facility','Lab','Plantation','Rig');
@goody = ('Alien Menhir','Abduction Site','Land Worked','Water Worked','City Ruins');
@improvements = (@builds,@goody);

# arrays of professions: 1st item = description, 2nd item = yield produced, rest of items = yield inputs
# professions producing yields on map plots
@plotprofs = (
	['Agriculture','NUTRIENTS'],
	['Biopolymer Harvesting','BIOPOLYMERS'],
	['Base Metals Mining','BASE_METALS'],
	['Silicate Mining','SILICATES'],
	['Precious Metals Excavation','PRECIOUS_METALS'],
	['Actinide Extraction','ACTINIDES'],
	['Isotope Extraction','ISOTOPES'],
	['Rare Earths Extraction','RARE_EARTHS'],
	['Crystalloid Extraction','CRYSTALLOIDS'],
	['Nucleic Acid Culture','NUCLEIC_ACIDS'],
	['Amino Acid Culture','AMINO_ACIDS'],
	['Tissue Sample Culture','TISSUE_SAMPLES'],
	['Microbial Culture','MICROBES'],
	['Datacore Excavation','DATACORES'],
	['Artifact Excavation','PROGENITOR_ARTIFACTS'],
	['Alien Specimen Excavation','ALIEN_SPECIMENS'],
	['Opiates Cultivation','OPIATES'],
	['Xenotoxin Cultivation','XENOTOXINS'],
	['Botanical Cultivation','BOTANICALS'],
	['Hydrocarbon Drilling','HYDROCARBONS'],
	['Clathrate Drilling','CLATHRATES'],
	['Core Sample Drilling','CORE_SAMPLES']
	);
# professions producing yields in buildings
@cityprofs = (
	['Machine Tool Casting','MACHINE_TOOLS','BASE_METALS'],
	['Robotics Manufacturing','ROBOTICS','MACHINE_TOOLS'],
	['Munitions Manufacturing','MUNITIONS','MACHINE_TOOLS'],
	['Photonics Manufacturing','PHOTONICS','CRYSTALLOIDS'],
	['Plasteel Smelting','PLASTEEL','BASE_METALS','BIOPOLYMERS'],
	['Duralloy Smelting','DURALLOY','BASE_METALS','SILICATES'],
	['Crystalloy Smelting','CRYSTALLOY','PRECIOUS_METALS','CRYSTALLOIDS'],
	['Nucleonics Enrichment','NUCLEONICS','ISOTOPES'],
	['Fusion Core Enrichment','FUSION_CORES','ACTINIDES'],
	['Semiconductor Fabrication','SEMICONDUCTORS','RARE_EARTHS'],
	['Plasmid Biosynthesis','PLASMIDS','NUCLEIC_ACIDS'],
	['Enzyme Biosynthesis','ENZYMES','AMINO_ACIDS'],
	['Stem Cell Dissection','STEM_CELLS','TISSUE_SAMPLES'],
	['Datacore Analysis','STATE_SECRETS','DATACORES'],
	['Artifact Analysis','PROGENITOR_TECH','PROGENITOR_ARTIFACTS'],
	['Alien Specimen Dissection','ALIEN_RELICS','ALIEN_SPECIMENS'],
	['Narcotics Distillation','NARCOTICS','OPIATES'],
	['Bioweapons Distillation','BIOWEAPONS','XENOTOXINS'],
	['Pharmaceuticals Distillation','PHARMACEUTICALS','BOTANICALS'],
	['Petrochemical Refining','PETROCHEMICALS','HYDROCARBONS'],
	['Colloids Refining','COLLOIDS','CLATHRATES'],
	['Catalyst Refining','CATALYSTS','CORE_SAMPLES'],
	['Investment Banking','HARD_CURRENCY','PRECIOUS_METALS'],
	['Construction','INDUSTRY'],
	['Mass Media','MEDIA'],
	['Administration','LIBERTY'],
	['Education','EDUCATION'],
	['Applied Research','RESEARCH']
	);
@prodprofs = (@plotprofs,@cityprofs);

#city production yields needing specialbuildings
@cityyields = ("Machine Tools" , "Robotics" , "Munitions" , "Photonics" , "Hard Currency" , "Plasteel" , "Duralloy" , "Crystalloy" , "Nucleonics" , "Fusion Cores" , "Semiconductors" , "Plasmids" , "Enzymes" , "Stem Cells" , "State Secrets" , "Progenitor Tech" , "Alien Relics" , "Narcotics" , "Bioweapons" , "Pharmaceuticals" , "Petrochemicals" , "Colloids" , "Catalysts", "Industry" , "Media", "Liberty", "Research", "Education");
#other specialbuildings
@miscbuildings = ("Fort","Dock","Warehouse","Market","Prison");
@onetierbuildings = ("Shrine","Trading Post");
@allbuildings = (@cityyields,@miscbuildings,@onetierbuildings);

#From units
# array of arrays for unit content: first element = specialist name, rest = specialist yields
@allspecialists = (
	['Expert Agronomist','NUTRIENTS'],
	['Expert Forester','BIOPOLYMERS'],
	['Expert Miner','BASE_METALS','PRECIOUS_METALS','SILICATES'],
	['Expert Geologist','ACTINIDES','ISOTOPES','RARE_EARTHS','CRYSTALLOIDS'],
	['Expert Lab Technician','NUCLEIC_ACIDS','AMINO_ACIDS','TISSUE_SAMPLES','MICROBES'],
	['Intrepid Archaeologist','DATACORES','PROGENITOR_ARTIFACTS','ALIEN_SPECIMENS','PRECIOUS_METALS'],
	['Expert Xenobotanist','OPIATES','XENOTOXINS','BOTANICALS','MICROBES'],
	['Expert Driller','HYDROCARBONS','CLATHRATES','CORE_SAMPLES'],
	['Master Machinist','MACHINE_TOOLS'],
	['Mechanical Engineer','ROBOTICS'],
	['Infamous Arms Dealer','MUNITIONS','PHOTONICS'],
	['Master Metallurgist','PLASTEEL','DURALLOY','CRYSTALLOY'],
	['Brilliant Physicist','NUCLEONICS','FUSION_CORES','SEMICONDUCTORS'],
	['Brilliant Geneticist','PLASMIDS','ENZYMES','STEM_CELLS'],
	['Brilliant Theorist','STATE_SECRETS','PROGENITOR_TECH','ALIEN_RELICS'],
	['Infamous Druglord','NARCOTICS','BIOWEAPONS','PHARMACEUTICALS'],
	['Chemical Engineer','PETROCHEMICALS','COLLOIDS','CATALYSTS'],
	['Electrical Engineer','SEMICONDUCTORS','CATALYSTS','PHOTONICS'],
	['Brilliant Physician','STEM_CELLS','ALIEN_RELICS','PHARMACEUTICALS'],
	['Infamous Smuggler','EARTH_GOODS','CONTRABAND','HARD_CURRENCY'],
	['Civil Engineer','INDUSTRY'],
	['Fascinating Celebrity','MEDIA'],
	['Artful Politician','LIBERTY'],
	['Visionary Researcher','RESEARCH'],
	['Convict','EDUCATION'],
	['Proletarian','EDUCATION'],
	['UFO Cultist','EDUCATION'],
	['Addict','EDUCATION'],
	['Android','EDUCATION'],
	['Colonist',''],
	['Eminent Anthropologist',''],
	['Hardy Laborer','ENERGY'],
	['Intrepid Explorer',''],
	['Veteran Soldier','']
	);
# professions that "walk" on the map
@walkprofs = ("Colonist", "Explorer", "Laborer", "Mechanized Laborer", "Hunter", "Trader", "Anthropologist", "Emissary", "Militia", "Infantry", "Scout Mecha", "Tactical Mecha", "Heavy Mecha", "Terrorist", "Pirate");
# military and other non-colonist units which don't take professions
@transportships = ("Corvette","Dropship","Freighter","Heavy Freighter");
@warships = ("Frigate","Gunboat","Destroyer","Cruiser","Battleship","Dreadnought");
@miscships = ("Probe","Mining Vessel","Science Vessel","Colony Ship");
@miscland = ("Convoy","Treasure","Relic","Colonial Garrison","Artillery","War Droid");
@miscunits = (@transportships,@warships,@miscships,@miscland);
@beastunits = ("Killbots","Progenitor AI","Arachnid","Scorpion","Amoeba","Enigma Virion");

# TXT_KEY subroutine
sub maketext
{
	my $tag = $_[0];
	my $text = $_[1];
	print TEXT "<TEXT>\n";
	print TEXT "\t<Tag>".$tag."</Tag>\n";
	print TEXT "\t<English>".$text."</English>\n";
	print TEXT "\t<French>".$text."</French>\n";	
	print TEXT "\t<German>".$text."</German>\n";
	print TEXT "\t<Italian>".$text."</Italian>\n";
	print TEXT "\t<Spanish>".$text."</Spanish>\n";
	print TEXT "</TEXT>\n";
}

# buildingclass subroutine
sub makebclass
{
	my $tag = $_[0];
	my $bdesc = $_[1];
	print BCI "<BuildingClassInfo>\n";
	print BCI "\t<Type>BUILDINGCLASS_".$tag."</Type>\n";
	print BCI "\t<Description>TXT_KEY_BUILDING_".$tag."</Description>\n";	
	print BCI "\t<DefaultBuilding>BUILDING_".$tag."</DefaultBuilding>\n";
	print BCI "\t<VictoryThresholds/>\n";
	print BCI "</BuildingClassInfo>\n";
}

# unitclass subroutine
sub makeuclass
{
	my $tag = $_[0];
	my $desc = $_[1];
	print UCI "<UnitClassInfo>\n";
	print UCI "\t<Type>UNITCLASS_".$tag."</Type>\n";
	print UCI "\t<Description>".$desc."</Description>\n";	
	print UCI "\t<DefaultUnit>UNIT_".$tag."</DefaultUnit>\n";
	print UCI "</UnitClassInfo>\n";
}

## ** YIELD / PROFESSION XML **

# open XML for writing
open (YI, '> ../Assets/XML/Terrain/CIV4YieldInfos.xml') or die "Can't write yields: $!";
open (TEXT, '> ../Assets/XML/Text/CIV4GameText_2071.xml') or die "Can't write text: $!";
open (EI, '> ../Assets/XML/GameInfo/CIV4EmphasizeInfo.xml') or die "Can't write yields: $!";
print TEXT '<?xml version="1.0" encoding="ISO-8859-1"?>'."\n";
print TEXT '<Civ4GameText xmlns="http://www.firaxis.com">'."\n";

# generate yieldinfos and emphasizeinfo XML
print YI '<?xml version="1.0"?>'."\n";
print YI '<!-- edited with XMLSPY v2004 rel. 2 U (http://www.xmlspy.com) by EXTREME (Firaxis Games) -->'."\n";
print YI '<!-- Sid Meier\'s Civilization 4 -->'."\n".'<!-- Copyright Firaxis Games 2005 -->'."\n".'<!-- -->'."\n";
print YI '<Civ4YieldInfos xmlns="x-schema:CIV4TerrainSchema.xml">'."\n<YieldInfos>\n";
print EI '<?xml version="1.0"?>'."\n";
print EI '<!-- edited with XMLSPY v2004 rel. 2 U (http://www.xmlspy.com) Alex Mantzaris (Firaxis Games) -->'."\n";
print EI '<!-- Sid Meier\'s Civilization 4 -->'."\n".'<!-- Copyright Firaxis Games 2005 -->'."\n".'<!-- -->'."\n".'<!-- Emphasize Infos -->'."\n";
print EI '<Civ4EmphasizeInfo xmlns="x-schema:CIV4GameInfoSchema.xml">'."\n<EmphasizeInfos>\n";
foreach $item (@allyields)
	{
	my $tag = $item;
	$tag =~ tr/ /_/;
	$tag =~ tr/[a-z]/[A-Z]/;
	my $desc = $item;
	if (grep {$_ eq $item} @cargoyields) {$iscargo=1;} else {$iscargo=0;}
	print YI "<YieldInfo>\n";
	print YI "\t<Type>YIELD_".$tag."</Type>\n";
	print YI "\t<Description>TXT_KEY_YIELD_".$tag."</Description>\n";
	&maketext('TXT_KEY_YIELD_'.$tag,$desc);
	print YI "\t<Civilopedia>TXT_KEY_YIELD_".$tag."_PEDIA</Civilopedia>\n";
	my $pedia = 'The many unique properties of alien materials, together with Earth\'s rapidly depleting resource base, made [COLOR_HIGHLIGHT_TEXT]'.$desc."[COLOR_REVERT] from the New Worlds a scarce and valuable commodity which often became the source of conflict between Earthling colonists and the Alien Empires.";
	&maketext('TXT_KEY_YIELD_'.$tag.'_PEDIA',$pedia);
	print YI "\t<bCargo>".$iscargo."</bCargo>\n";
	print YI "\t<iBuyPriceLow>10</iBuyPriceLow>\n";
	print YI "\t<iBuyPriceHigh>15</iBuyPriceHigh>\n";
	print YI "\t<iSellPriceDifference>2</iSellPriceDifference>\n";
	print YI "\t<iPriceChangeThreshold>800</iPriceChangeThreshold>\n";
	print YI "\t<iPriceCorrectionPercent>2</iPriceCorrectionPercent>\n";
	print YI "\t<iNativeBuyPrice>10</iNativeBuyPrice>\n";
	print YI "\t<iNativeSellPrice>15</iNativeSellPrice>\n";
	print YI "\t<iNativeConsumptionPercent>0</iNativeConsumptionPercent>\n";
	print YI "\t<iNativeHappy>0</iNativeHappy>\n";
	print YI "\t<iHillsChange>0</iHillsChange>\n";
	print YI "\t<iPeakChange>0</iPeakChange>\n";
	print YI "\t<iLakeChange>0</iLakeChange>\n";
	print YI "\t<iCityChange>0</iCityChange>\n";
	print YI "\t<iMinCity>0</iMinCity>\n";
	print YI "\t<iAIWeightPercent>100</iAIWeightPercent>\n";
	print YI "\t<iAIBaseValue>5</iAIBaseValue>\n";
	print YI "\t<iNativeBaseValue>4</iNativeBaseValue>\n";
	print YI "\t<iPower>0</iPower>\n";
	print YI "\t<iAsset>1</iAsset>\n";
	print YI "\t<ColorType>COLOR_YIELD_FOOD</ColorType>\n";
	if (grep {$_ eq $desc} @abstractyields) {
		print YI "\t<UnitClass></UnitClass>\n";
		} else {
		print YI "\t<UnitClass>UNITCLASS_".$tag."</UnitClass>\n";
		}
	print YI "\t<iTextureIndex>".$index."</iTextureIndex>\n";
	print YI "\t<iWaterTextureIndex>20</iWaterTextureIndex>\n";
	print YI "\t<Icon>Art/Buttons/Yields/".$tag.".dds</Icon>\n";
	print YI "\t<HighlightIcon>Art/Buttons/Yields/".$tag.".dds</HighlightIcon>\n";
	print YI "\t".'<Button>Art/Buttons/Yields/'.$tag.'.dds</Button>'."\n";
	print YI "\t<TradeScreenTypes>\n";
	print YI "\t\t<TradeScreenType>\n";
	print YI "\t\t\t<TradeScreen>TRADE_SCREEN_SPICE_ROUTE</TradeScreen>\n";
	print YI "\t\t\t<iPricePercent>100</iPricePercent>\n";
	print YI "\t\t</TradeScreenType>\n";
	print YI "\t\t<TradeScreenType>\n";
	print YI "\t\t\t<TradeScreen>TRADE_SCREEN_SILK_ROAD</TradeScreen>\n";
	print YI "\t\t\t<iPricePercent>100</iPricePercent>\n";
	print YI "\t\t</TradeScreenType>\n";
	print YI "\t</TradeScreenTypes>\n";
	print YI "</YieldInfo>\n";
	
#	emphasize infos
	print EI "<EmphasizeInfo>\n";
	print EI "\t<Type>EMPHASIZE_".$tag."</Type>\n";
	print EI "\t<Description>Emphasize ".$desc."</Description>\n";
	print EI "\t<Button>Art/Interface/Buttons/Yields/".$tag.'.dds</Button>'."\n";
	print EI "\t<bAvoidGrowth>0</bAvoidGrowth>\n";
	print EI "\t<YieldModifiers>\n";
	print EI "\t\t<YieldModifier>\n";
	print EI "\t\t\t<YieldType>YIELD_".$tag."</YieldType>\n";
	print EI "\t\t\t<iYield>1</iYield>\n";
	print EI "\t\t</YieldModifier>\n";
	print EI "\t</YieldModifiers>\n";
	print EI "</EmphasizeInfo>\n";
	print EI "<EmphasizeInfo>\n";
	print EI "\t<Type>EMPHASIZE_NO_".$tag."</Type>\n";
	print EI "\t<Description>Deemphasize ".$desc."</Description>\n";
	print EI "\t<Button>Art/Interface/Buttons/Yields/".$tag.'.dds</Button>'."\n";
	print EI "\t<bAvoidGrowth>0</bAvoidGrowth>\n";
	print EI "\t<YieldModifiers>\n";
	print EI "\t\t<YieldModifier>\n";
	print EI "\t\t\t<YieldType>YIELD_".$tag."</YieldType>\n";
	print EI "\t\t\t<iYield>-1</iYield>\n";
	print EI "\t\t</YieldModifier>\n";
	print EI "\t</YieldModifiers>\n";
	print EI "</EmphasizeInfo>\n";
	$index=1;
	}
# close files
print EI "<EmphasizeInfo>\n";
print EI "\t<Type>EMPHASIZE_AVOID_GROWTH</Type>\n";
print EI "\t<Description>TXT_KEY_EMPHASIZE_AVOID_GROWTH</Description>\n";
&maketext("TXT_KEY_EMPHASIZE_AVOID_GROWTH","Avoid Growth");
print EI "\t<Button>Art/Interface/Buttons/Governor/Food.dds</Button>"."\n";
print EI "\t<bAvoidGrowth>1</bAvoidGrowth>\n";
print EI "\t<YieldModifiers/>\n";
print EI "</EmphasizeInfo>\n";	
print EI "</EmphasizeInfos>\n</Civ4EmphasizeInfo>\n";
close EI;
print YI "</YieldInfos>\n</Civ4YieldInfos>\n";
close YI;

# generate professioninfos XML
open (PI, '> ../Assets/XML/Units/CIV4ProfessionInfos.xml') or die "Can't write profs: $!";
print PI '<?xml version="1.0" encoding="UTF-8"?>'."\n";
print PI '<!-- edited with XMLSPY v2004 rel. 2 U (http://www.xmlspy.com) by EXTREME (Firaxis Games) -->'."\n";
print PI '<!-- Sid Meier\'s Civilization 4 -->'."\n".'<!-- Copyright Firaxis Games 2005 -->'."\n".'<!-- -->'."\n";
print PI '<Civ4ProfessionInfos xmlns="x-schema:CIV4UnitSchema.xml">'."\n<ProfessionInfos>\n";

# production professions
foreach $item (@prodprofs)
	{
	my @yields = @$item;
#	print $yields[0]."\t".$yields[1]."\n";
	my $desc = shift(@yields);
	my $plural = PL_N($desc);	
	my $tag = shift(@yields);
	my $ydesc = ucfirst($tag);
	$ydesc =~ tr/_/ /;
	$ydesc =~ s/(\w+)/\u\L$1/g;
	if (grep {$_ eq $item} @plotprofs) {$isoutdoor=1;} else {$isoutdoor=0;}
	if (not $isoutdoor) {
		$special = 'SPECIALBUILDING_'.$tag;
		$inputyielddesc = $yields[0];
		if ($inputyielddesc =~ /\w+/) {
			$inputyield = 'YIELD_'.$yields[0];
			$inputyielddesc =~ tr/_/ /;
			$inputyielddesc =~ s/(\w+)/\u\L$1/g;
			} else {
			$inputyielddesc = '';
			$inputyield = 'NONE';
			}
		}
		else {
		$inputyield = 'NONE';
		$special = 'NONE';
		}
	print PI "<ProfessionInfo>\n";
	print PI "\t<Type>PROFESSION_".$tag."</Type>\n";
	print PI "\t<Description>TXT_KEY_PROFESSION_".$tag."</Description>\n";
	&maketext("TXT_KEY_PROFESSION_".$tag, $desc);
	print PI "\t<Civilopedia>TXT_KEY_PROFESSION_".$tag."_PEDIA</Civilopedia>\n";
	if ($isoutdoor)
		{$pedia = '[LINK=YIELD_'.$tag.']'.$ydesc.'[\LINK] are harvested from map tiles by citizens assigned to the [COLOR_HIGHLIGHT_TEXT]'.$desc."[COLOR_REVERT] profession.";}
		else {$pedia = '[LINK=YIELD_'.$tag.']'.$ydesc.'[\LINK] are produced from [LINK='.$inputyield.']'.$inputyielddesc.'[\LINK] by citizens assigned to the [COLOR_HIGHLIGHT_TEXT]'.$desc."[COLOR_REVERT] profession.";}
	&maketext('TXT_KEY_PROFESSION_'.$tag.'_PEDIA', $pedia);
	print PI "\t<Strategy></Strategy>\n";
	print PI "\t<Help/>\n";
	print PI "\t<Combat>NONE</Combat>\n";
	print PI "\t<DefaultUnitAI>NONE</DefaultUnitAI>\n";
	print PI "\t<SpecialBuilding>".$special."</SpecialBuilding>\n";
	print PI "\t<bWorkPlot>".$isoutdoor."</bWorkPlot>\n";
	print PI "\t<bCitizen>1</bCitizen>\n";
	print PI "\t<bWater>0</bWater>\n";
	print PI "\t<bScout>0</bScout>\n";
	print PI "\t<bCityDefender>0</bCityDefender>\n";
	print PI "\t<bCanFound>0</bCanFound>\n";
	print PI "\t<bUnarmed>0</bUnarmed>\n";
	print PI "\t<bNoDefensiveBonus>0</bNoDefensiveBonus>\n";
	print PI "\t<iCombatChange>0</iCombatChange>\n";
	print PI "\t<iMovesChange>0</iMovesChange>\n";	
	print PI "\t<iWorkRate>0</iWorkRate>\n";
	print PI "\t<iMissionaryRate>0</iMissionaryRate>\n";
	print PI "\t<iPower>0</iPower>\n";
	print PI "\t<iAsset>0</iAsset>\n";	
	print PI "\t<YieldEquipedNums></YieldEquipedNums>\n";	
	print PI "\t<FreePromotions></FreePromotions>\n";	
	print PI "\t<YieldsProduced>\n";
	print PI "\t\t<YieldType>YIELD_".$tag."</YieldType>\n";
	print PI "\t</YieldsProduced>\n";
	print PI "\t<YieldsConsumed>\n";
	print PI "\t\t<YieldType>".$inputyield."</YieldType>\n";
	print PI "\t</YieldsConsumed>\n";
	print PI "\t".'<Button>Art/Buttons/Yields/'.$tag.'.dds</Button>'."\n";
	print PI "</ProfessionInfo>\n";
	}
	
# professions moving as units on the map
foreach $desc (@walkprofs)
	{
	my $tag = $desc;
	$tag =~ tr/ /_/;
	$tag =~ tr/[a-z]/[A-Z]/;;
	my $plural = PL_N($desc);	
	my $fulldesc = $desc.':'.$plural;
	print PI "<ProfessionInfo>\n";
	print PI "\t<Type>PROFESSION_".$tag."</Type>\n";
	print PI "\t<Description>TXT_KEY_PROFESSION_".$tag."</Description>\n";
	&maketext("TXT_KEY_PROFESSION_".$tag, $desc);
	print PI "\t<Civilopedia>TXT_KEY_PEDIA_PROFESSION_".$tag."</Civilopedia>\n";
	$pedia = 'Colonists assigned to the [COLOR_HIGHLIGHT_TEXT]'.$desc."[COLOR_REVERT] profession are essential for expanding your influence across the hostile terrains of the New Worlds.";
	&maketext("TXT_KEY_PEDIA_PROFESSION_".$tag, $pedia);
	print PI "\t<Strategy></Strategy>\n";
	print PI "\t<Help/>\n";
	print PI "\t<Combat>UNITCOMBAT_MELEE</Combat>\n";
	print PI "\t<DefaultUnitAI>UNITAI_COLONIST</DefaultUnitAI>\n";
	print PI "\t<SpecialBuilding>NONE</SpecialBuilding>\n";
	print PI "\t<bWorkPlot>0</bWorkPlot>\n";
	print PI "\t<bCitizen>0</bCitizen>\n";
	print PI "\t<bWater>0</bWater>\n";
	print PI "\t<bScout>0</bScout>\n";
	print PI "\t<bCityDefender>1</bCityDefender>\n";
	print PI "\t<bCanFound>1</bCanFound>\n";
	print PI "\t<bUnarmed>1</bUnarmed>\n";
	print PI "\t<bNoDefensiveBonus>0</bNoDefensiveBonus>\n";
	print PI "\t<iCombatChange>1</iCombatChange>\n";
	print PI "\t<iMovesChange>0</iMovesChange>\n";	
	print PI "\t<iWorkRate>0</iWorkRate>\n";
	print PI "\t<iMissionaryRate>0</iMissionaryRate>\n";
	print PI "\t<iPower>0</iPower>\n";
	print PI "\t<iAsset>0</iAsset>\n";	
	print PI "\t<YieldEquipedNums>\n";
	print PI "\t\t<YieldEquipedNum>\n";
	print PI "\t\t\t<YieldType>YIELD_MUNITIONS</YieldType>\n";
	print PI "\t\t\t<iYieldAmount>0</iYieldAmount>\n";
	print PI "\t\t</YieldEquipedNum>\n";
	print PI "\t</YieldEquipedNums>\n";	
	print PI "\t<FreePromotions></FreePromotions>\n";	
	print PI "\t<YieldsProduced>\n";
	print PI "\t\t<YieldType></YieldType>\n";
	print PI "\t</YieldsProduced>\n";
	print PI "\t<YieldsConsumed>\n";
	print PI "\t\t<YieldType></YieldType>\n";
	print PI "\t</YieldsConsumed>\n";
	print PI "\t".'<Button>Art/Buttons/Yields/'.$tag.'.dds</Button>'."\n";
	print PI "</ProfessionInfo>\n";
	}
	
print PI "</ProfessionInfos>\n</Civ4ProfessionInfos>\n";
close PI;

# **TERRAINS**
open (TI, '> ../Assets/XML/Terrain/CIV4TerrainInfos.xml') or die "Can't write terrains: $!";
print TI '<?xml version="1.0" encoding="UTF-8"?>'."\n";
print TI '<!-- edited with XMLSPY v2004 rel. 2 U (http://www.xmlspy.com) by Ed Piper (Firaxis Games) -->'."\n";
print TI '<!-- Sid Meier\'s Civilization 4 -->'."\n".'<!-- Copyright Firaxis Games 2005 -->'."\n".'<!-- -->'."\n".'<!-- Terrain Infos -->'."\n";
print TI '<Civ4TerrainInfos xmlns="x-schema:CIV4TerrainSchema.xml">'."\n<TerrainInfos>\n";

open (ADT, '> ../Assets/XML/Art/CIV4ArtDefines_Terrain.xml') or die "Can't write terrains artdef: $!";
print ADT '<?xml version="1.0" encoding="UTF-8" standalone="no"?>'."\n";
print ADT '<!-- edited with XMLSPY v2004 rel. 2 U (http://www.xmlspy.com) by EXTREME (Firaxis Games) -->'."\n";
print ADT '<!-- Sid Meier\'s Civilization 4 -->'."\n".'<!-- Copyright Firaxis Games 2005 -->'."\n".'<!-- -->'."\n".'<!-- Terrain art path information -->'."\n";
print ADT '<Civ4ArtDefines xmlns="x-schema:CIV4ArtDefinesSchema.xml">'."\n<TerrainArtInfos>\n";
$landorder=1;
$waterorder=50;


# 1st arg = terrain tagname, 2nd = description, 3rd = hash (yield=>production)
sub makeland
{
my $tag = shift;
my $desc = shift;
$href = shift;
$desc =~ tr/_/ /;
$desc =~ s/(\w+)/\u\L$1/g;
print TI "<TerrainInfo>\n";
print TI "\t<Type>TERRAIN_$tag</Type>\n";
print TI "\t<Description>TXT_KEY_TERRAIN_$tag</Description>\n";
&maketext("TXT_KEY_TERRAIN_$tag",$desc);
$pedia = 'The expanses of [COLOR_HIGHLIGHT_TEXT]'.$desc.'[COLOR_REVERT] found across certain planets of the New Worlds are often rich in ';
for my $yield ( keys (%$href) ) {
	my $yielddesc = $yield;
	$yielddesc =~ tr/_/ /;
	$yielddesc =~ s/(\w+)/\u\L$1/g;
	$pedia = $pedia.'[LINK=YIELD_'.$yield.']'.$yielddesc.'[\LINK], '; 
	}
print TI "\t<Civilopedia>TXT_KEY_TERRAIN_$tag_PEDIA</Civilopedia>\n";
&maketext("TXT_KEY_TERRAIN_".$tag."_PEDIA",$pedia);
print TI "\t<ArtDefineTag>ART_DEF_TERRAIN_$tag</ArtDefineTag>\n";
print TI "\t<Yields>\n";
for my $yield ( keys (%$href) ) {
	$prod = $href->{$yield};
	if ($prod != 0) {
		print TI "\t\t<YieldIntegerPair>\n";
		print TI "\t\t\t<YieldType>YIELD_".$yield."</YieldType>\n";
		print TI "\t\t\t<iValue>".$prod."</iValue>\n";
		print TI "\t\t</YieldIntegerPair>\n";
		}
	}
print TI "\t</Yields>\n";
print TI "\t<RiverYieldIncreases>\n";
print TI "\t\t<YieldIntegerPair>\n";
print TI "\t\t\t<YieldType>YIELD_NUTRIENTS</YieldType>\n";
print TI "\t\t\t<iValue>1</iValue>\n";
print TI "\t\t</YieldIntegerPair>\n";
print TI "\t</RiverYieldIncreases>\n";
print TI "\t<bWater>0</bWater>\n";
print TI "\t<bImpassable>0</bImpassable>\n";
print TI "\t<bFound>1</bFound>\n";
print TI "\t<bFoundCoast>0</bFoundCoast>\n";
print TI "\t<iMovement>1</iMovement>\n";
print TI "\t<iSeeFrom>1</iSeeFrom>\n";
print TI "\t<iSeeThrough>1</iSeeThrough>\n";
print TI "\t<iBuildModifier>0</iBuildModifier>\n";
print TI "\t<iDefense>0</iDefense>\n";
print TI "\t<Button>Art/Interface\Buttons\WorldBuilder\Terrain_Grass.dds</Button>\n";
print TI "\t<FootstepSounds>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>FOOTSTEP_AUDIO_HUMAN</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_FOOT_UNIT</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>FOOTSTEP_AUDIO_HUMAN_LOW</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_FOOT_UNIT_LOW</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>FOOTSTEP_AUDIO_HORSE</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_HORSE_RUN</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>LOOPSTEP_WHEELS</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_CHARIOT_LOOP</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>ENDSTEP_WHEELS</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_CHARIOT_END</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>LOOPSTEP_WHEELS_2</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_WAR_CHARIOT_LOOP</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>ENDSTEP_WHEELS_2</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_WAR_CHARIOT_END</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>LOOPSTEP_OCEAN1</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_OCEAN_LOOP1</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>ENDSTEP_OCEAN1</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_OCEAN_END1</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>LOOPSTEP_OCEAN2</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_OCEAN_LOOP1</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>ENDSTEP_OCEAN2</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_OCEAN_END2</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>LOOPSTEP_IRONCLAD</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_IRONCLAD_RUN_LOOP</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>ENDSTEP_IRONCLAD</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_IRONCLAD_RUN_END</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>LOOPSTEP_TRANSPORT</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_TRANSPORT_RUN_LOOP</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>ENDSTEP_TRANSPORT</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_TRANSPORT_RUN_END</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>LOOPSTEP_ARTILLERY</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_ARTILLERY_RUN_LOOP</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>ENDSTEP_ARTILLERY</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_ARTILLERY_RUN_END</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>LOOPSTEP_WHEELS_3</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_TREBUCHET_RUN</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>ENDSTEP_WHEELS_3</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_TREBUCHET_STOP</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t</FootstepSounds>\n";
print TI "\t<WorldSoundscapeAudioScript>ASSS_GRASSLAND_SELECT_AMB</WorldSoundscapeAudioScript>\n";
print TI "\t<bGraphicalOnly>0</bGraphicalOnly>\n";
print TI "</TerrainInfo>\n";

print ADT "<TerrainArtInfo>\n";
print ADT "\t<Type>ART_DEF_TERRAIN_".$tag."</Type>\n";
print ADT "\t<Path>Art/Terrain/Textures/LandBlend.dds</Path>\n";
print ADT "\t<Grid>Art/Terrain/Textures/LandGrids.dds</Grid>\n";
print ADT "\t<Detail>Art/Terrain/Textures/".$tag.".dds</Detail>\n";
print ADT "\t<Button>Art/Buttons/Terrains/".$tag.".dds</Button>\n";
print ADT "\t<LayerOrder>".$landorder."</LayerOrder>\n";
print ADT "\t<TerrainGroup>TERRAIN_GROUP_LAND</TerrainGroup>\n";
print ADT "\t<TextureBlend01>8,0</TextureBlend01>\n";
print ADT "\t<TextureBlend02>1,0</TextureBlend02>\n";
print ADT "\t<TextureBlend04>6,0</TextureBlend04>\n";
print ADT "\t<TextureBlend08>5,0</TextureBlend08>\n";
print ADT "\t<TextureBlend03>2,0</TextureBlend03>\n";
print ADT "\t<TextureBlend06>10,0</TextureBlend06>\n";
print ADT "\t<TextureBlend12>12,0</TextureBlend12>\n";
print ADT "\t<TextureBlend09>9,0</TextureBlend09>\n";
print ADT "\t<TextureBlend07>3,0</TextureBlend07>\n";
print ADT "\t<TextureBlend14>14,0</TextureBlend14>\n";
print ADT "\t<TextureBlend13>11,0</TextureBlend13>\n";
print ADT "\t<TextureBlend11>4,0</TextureBlend11>\n";
print ADT "\t<TextureBlend10>7,0</TextureBlend10>\n";
print ADT "\t<TextureBlend05>13,0</TextureBlend05>\n";
print ADT "\t<TextureBlend15>15,0 16,0 18,0 19,0 20,0 21,0 22,0 23,0 24,0 25,0 26,0 27,0 28,0 29,0 30,0 31,0 32,0</TextureBlend15>\n";
print ADT "</TerrainArtInfo>\n";
$landorder++;
}

# 1st arg = terrain name, 2nd = hash (yield=>production)
sub makewater
{
my $tag = shift;
$href = shift;
my $desc = $tag;
$desc =~ tr/_/ /;
$desc =~ s/(\w+)/\u\L$1/g;
print TI "<TerrainInfo>\n";
print TI "\t<Type>TERRAIN_$tag</Type>\n";
print TI "\t<Description>TXT_KEY_TERRAIN_$tag</Description>\n";
&maketext("TXT_KEY_TERRAIN_$tag",$desc);
$pedia = 'The expanses of [COLOR_HIGHLIGHT_TEXT]'.$desc.'[COLOR_REVERT] found across certain planets of the New Worlds are often rich in ';
for my $yield ( keys (%$href) ) {
	my $yielddesc = $yield;
	$yielddesc =~ tr/_/ /;
	$yielddesc =~ s/(\w+)/\u\L$1/g;
	$pedia = $pedia.'[LINK=YIELD_'.$yield.']'.$yielddesc.'[\LINK], '; 
	}
print TI "\t<Civilopedia>TXT_KEY_TERRAIN_$tag_PEDIA</Civilopedia>\n";
&maketext("TXT_KEY_TERRAIN_".$tag."_PEDIA",$pedia);
print TI "\t<ArtDefineTag>ART_DEF_TERRAIN_$tag</ArtDefineTag>\n";
print TI "\t<Yields>\n";
for my $yield ( keys (%$href) ) {
	$prod = $href->{$yield};
	if ($prod != 0) {
		print TI "\t\t<YieldIntegerPair>\n";
		print TI "\t\t\t<YieldType>YIELD_".$yield."</YieldType>\n";
		print TI "\t\t\t<iValue>".$prod."</iValue>\n";
		print TI "\t\t</YieldIntegerPair>\n";
		}
	}
print TI "\t</Yields>\n";
print TI "\t<RiverYieldIncreases/>\n";
print TI "\t<bWater>1</bWater>\n";
print TI "\t<bImpassable>0</bImpassable>\n";
print TI "\t<bFound>0</bFound>\n";
print TI "\t<bFoundCoast>0</bFoundCoast>\n";
print TI "\t<iMovement>1</iMovement>\n";
print TI "\t<iSeeFrom>1</iSeeFrom>\n";
print TI "\t<iSeeThrough>1</iSeeThrough>\n";
print TI "\t<iBuildModifier>0</iBuildModifier>\n";
print TI "\t<iDefense>0</iDefense>\n";
print TI "\t<Button>Art/Interface\Buttons\WorldBuilder\Terrain_Grass.dds</Button>\n";
print TI "\t<FootstepSounds>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>FOOTSTEP_AUDIO_HUMAN</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_FOOT_UNIT</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>FOOTSTEP_AUDIO_HUMAN_LOW</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_FOOT_UNIT_LOW</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>FOOTSTEP_AUDIO_HORSE</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_HORSE_RUN</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>LOOPSTEP_WHEELS</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_CHARIOT_LOOP</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>ENDSTEP_WHEELS</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_CHARIOT_END</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>LOOPSTEP_WHEELS_2</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_WAR_CHARIOT_LOOP</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>ENDSTEP_WHEELS_2</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_WAR_CHARIOT_END</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>LOOPSTEP_OCEAN1</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_OCEAN_LOOP1</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>ENDSTEP_OCEAN1</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_OCEAN_END1</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>LOOPSTEP_OCEAN2</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_OCEAN_LOOP1</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>ENDSTEP_OCEAN2</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_OCEAN_END2</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>LOOPSTEP_IRONCLAD</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_IRONCLAD_RUN_LOOP</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>ENDSTEP_IRONCLAD</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_IRONCLAD_RUN_END</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>LOOPSTEP_TRANSPORT</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_TRANSPORT_RUN_LOOP</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>ENDSTEP_TRANSPORT</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_TRANSPORT_RUN_END</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>LOOPSTEP_ARTILLERY</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_ARTILLERY_RUN_LOOP</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>ENDSTEP_ARTILLERY</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_ARTILLERY_RUN_END</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>LOOPSTEP_WHEELS_3</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_TREBUCHET_RUN</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>ENDSTEP_WHEELS_3</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_TREBUCHET_STOP</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t</FootstepSounds>\n";
print TI "\t<WorldSoundscapeAudioScript>ASSS_GRASSLAND_SELECT_AMB</WorldSoundscapeAudioScript>\n";
print TI "\t<bGraphicalOnly>0</bGraphicalOnly>\n";
print TI "</TerrainInfo>\n";
}

sub makelandvanilla
{
my $tag = shift;
$href = shift;
my $desc = $tag;
$desc =~ tr/_/ /;
$desc =~ s/(\w+)/\u\L$1/g;
print TI "<TerrainInfo>\n";
print TI "\t<Type>TERRAIN_$tag</Type>\n";
print TI "\t<Description>TXT_KEY_TERRAIN_$tag</Description>\n";
&maketext("TXT_KEY_TERRAIN_$tag",$desc);
$pedia = 'The expanses of [COLOR_HIGHLIGHT_TEXT]'.$desc.'[COLOR_REVERT] found across certain planets of the New Worlds are often rich in ';
for my $yield ( keys (%$href) ) {
	my $yielddesc = $yield;
	$yielddesc =~ tr/_/ /;
	$yielddesc =~ s/(\w+)/\u\L$1/g;
	$pedia = $pedia.'[LINK=YIELD_'.$yield.']'.$yielddesc.'[\LINK], '; 
	}
print TI "\t<Civilopedia>TXT_KEY_TERRAIN_$tag_PEDIA</Civilopedia>\n";
&maketext("TXT_KEY_TERRAIN_".$tag."_PEDIA",$pedia);
print TI "\t<ArtDefineTag>ART_DEF_TERRAIN_$tag</ArtDefineTag>\n";
print TI "\t<Yields>\n";
for my $yield ( keys (%$href) ) {
	$prod = $href->{$yield};
	if ($prod != 0) {
		print TI "\t\t<YieldIntegerPair>\n";
		print TI "\t\t\t<YieldType>YIELD_".$yield."</YieldType>\n";
		print TI "\t\t\t<iValue>".$prod."</iValue>\n";
		print TI "\t\t</YieldIntegerPair>\n";
		}
	}
print TI "\t</Yields>\n";
print TI "\t<RiverYieldIncreases>\n";
print TI "\t\t<YieldIntegerPair>\n";
print TI "\t\t\t<YieldType>YIELD_NUTRIENTS</YieldType>\n";
print TI "\t\t\t<iValue>1</iValue>\n";
print TI "\t\t</YieldIntegerPair>\n";
print TI "\t</RiverYieldIncreases>\n";
print TI "\t<bWater>0</bWater>\n";
print TI "\t<bImpassable>0</bImpassable>\n";
print TI "\t<bFound>1</bFound>\n";
print TI "\t<bFoundCoast>0</bFoundCoast>\n";
print TI "\t<iMovement>1</iMovement>\n";
print TI "\t<iSeeFrom>1</iSeeFrom>\n";
print TI "\t<iSeeThrough>1</iSeeThrough>\n";
print TI "\t<iBuildModifier>0</iBuildModifier>\n";
print TI "\t<iDefense>0</iDefense>\n";
print TI "\t<Button>Art/Interface\Buttons\WorldBuilder\Terrain_Grass.dds</Button>\n";
print TI "\t<FootstepSounds>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>FOOTSTEP_AUDIO_HUMAN</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_FOOT_UNIT</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>FOOTSTEP_AUDIO_HUMAN_LOW</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_FOOT_UNIT_LOW</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>FOOTSTEP_AUDIO_HORSE</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_HORSE_RUN</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>LOOPSTEP_WHEELS</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_CHARIOT_LOOP</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>ENDSTEP_WHEELS</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_CHARIOT_END</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>LOOPSTEP_WHEELS_2</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_WAR_CHARIOT_LOOP</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>ENDSTEP_WHEELS_2</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_WAR_CHARIOT_END</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>LOOPSTEP_OCEAN1</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_OCEAN_LOOP1</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>ENDSTEP_OCEAN1</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_OCEAN_END1</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>LOOPSTEP_OCEAN2</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_OCEAN_LOOP1</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>ENDSTEP_OCEAN2</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_OCEAN_END2</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>LOOPSTEP_IRONCLAD</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_IRONCLAD_RUN_LOOP</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>ENDSTEP_IRONCLAD</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_IRONCLAD_RUN_END</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>LOOPSTEP_TRANSPORT</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_TRANSPORT_RUN_LOOP</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>ENDSTEP_TRANSPORT</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_TRANSPORT_RUN_END</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>LOOPSTEP_ARTILLERY</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_ARTILLERY_RUN_LOOP</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>ENDSTEP_ARTILLERY</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_ARTILLERY_RUN_END</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>LOOPSTEP_WHEELS_3</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_TREBUCHET_RUN</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t\t<FootstepSound>\n";
print TI "\t\t\t<FootstepAudioType>ENDSTEP_WHEELS_3</FootstepAudioType>\n";
print TI "\t\t\t<FootstepAudioScript>AS3D_UN_TREBUCHET_STOP</FootstepAudioScript>\n";
print TI "\t\t</FootstepSound>\n";
print TI "\t</FootstepSounds>\n";
print TI "\t<WorldSoundscapeAudioScript>ASSS_GRASSLAND_SELECT_AMB</WorldSoundscapeAudioScript>\n";
print TI "\t<bGraphicalOnly>0</bGraphicalOnly>\n";
print TI "</TerrainInfo>\n";
$landorder++;
}

# make terrains
&makeland('GRASS','Grassland',{'NUTRIENTS'=>3,'BIOPOLYMERS'=>1});
&makelandvanilla('PLAINS','Plains',{'NUTRIENTS'=>2,'HYDROCARBONS'=>2});
&makelandvanilla('DESERT','Desert',{'SILICATES'=>3,'ACTINIDES'=>2});
&makelandvanilla('MARSH','Marsh',{'NUTRIENTS'=>2,'OPIATES'=>2});
&makelandvanilla('TUNDRA','Tundra',{'NUTRIENTS'=>1,'DATACORES'=>2});
&makewater('COAST','Low Orbit',{'NUTRIENTS'=>2,'AMINO_ACIDS'=>2});
&makewater('OCEAN',,'Deep Space',{'NUTRIENTS'=>1});

#Aquatic Planet
&makeland('AQUATIC_FERTILE','Loam',{'NUTRIENTS'=>3,'MICROBES'=>1});
&makeland('AQUATIC_DRY','Silt Beds',{'NUTRIENTS'=>2,'HYDROCARBONS'=>2});
&makeland('AQUATIC_HOT','Diatomaceous',{'SILICATES'=>3,'ACTINIDES'=>2});
&makeland('AQUATIC_WET','Wetland',{'NUTRIENTS'=>2,'OPIATES'=>2});
&makeland('AQUATIC_COLD','Glacial',{'NUTRIENTS'=>1,'DATACORES'=>2});
# &makewater('PELAGIC_COAST',{'NUTRIENTS'=>2,'AMINO_ACIDS'=>2});
# &makewater('ABYSSAL_OCEAN',{'NUTRIENTS'=>1});

#Arid Planet
# &makeland('STEPPE',{'NUTRIENTS'=>3,'SILICATES'=>1});
# &makeland('BADLANDS',{'NUTRIENTS'=>2,'HYDROCARBONS'=>2});
# &makeland('DUNES',{'SILICATES'=>3,'ACTINIDES'=>2});
# &makeland('SALT_FLATS',{'NUTRIENTS'=>2,'OPIATES'=>2});
# &makeland('SCRUBLAND',{'NUTRIENTS'=>1,'DATACORES'=>2});
# &makewater('ALKALI_COAST',{'NUTRIENTS'=>2,'AMINO_ACIDS'=>2});
# &makewater('ALKALI_OCEAN',{'NUTRIENTS'=>1});

#Volcanic Planet
# &makeland('VOLCANIC_SOIL',{'NUTRIENTS'=>3,'PRECIOUS_METALS'=>1});
# &makeland('BATHOLITH',{'NUTRIENTS'=>2,'HYDROCARBONS'=>2});
# &makeland('REGOLITH',{'SILICATES'=>3,'ACTINIDES'=>2});
# &makeland('ASH',{'NUTRIENTS'=>2,'OPIATES'=>2});
# &makeland('FELSIC_ROCK',{'NUTRIENTS'=>1,'DATACORES'=>2});
# &makeland('PYROCLASTIC',{'NUTRIENTS'=>2,'AMINO_ACIDS'=>2});
# &makeland('MAGMA',{'NUTRIENTS'=>1});

#Arctic Planet
# &makeland('ALPINE',{'NUTRIENTS'=>3,'CRYSTALLOIDS'=>1});
# &makeland('HEATH',{'NUTRIENTS'=>2,'HYDROCARBONS'=>2});
# &makeland('LIMESTONE',{'SILICATES'=>3,'ACTINIDES'=>2});
# &makeland('BOG',{'NUTRIENTS'=>2,'OPIATES'=>2});
# &makeland('ARCTIC',{'NUTRIENTS'=>1,'DATACORES'=>2});
# &makewater('BRACKISH_COAST',{'NUTRIENTS'=>2,'AMINO_ACIDS'=>2});
# &makewater('BRACKISH_OCEAN',{'NUTRIENTS'=>1});

&makelandvanilla('SNOW',{''});
&makelandvanilla('PEAK',{''});
&makelandvanilla('HILL',{''});

# add vanilla terrain textures
open (HARD, '< ../Assets/XML/Art/CIV4ArtDefines_Terrain_vanilla.xml') or die "Can't read vanilla terrains: $!";	
foreach (<HARD>) {print ADT  $_;}
close HARD;

print TI "</TerrainInfos>\n</Civ4TerrainInfos>\n";
close TI;

print ADT "</TerrainArtInfos>\n</Civ4ArtDefines>\n";
close ADT;

# ** FEATURES **
open (FI, '> ../Assets/XML/Terrain/CIV4FeatureInfos.xml') or die "Can't write features: $!";
print FI '<?xml version="1.0" encoding="UTF-8"?>'."\n";
print FI '<!-- edited with XMLSPY v2004 rel. 2 U (http://www.xmlspy.com) by Alex Mantzaris (Firaxis Games) -->'."\n";
print FI '<!-- Sid Meier\'s Civilization 4 -->'."\n".'<!-- Copyright Firaxis Games 2005 -->'."\n".'<!-- -->'."\n".'<!-- Feature -->'."\n";
print FI '<Civ4FeatureInfos xmlns="x-schema:CIV4TerrainSchema.xml">'."\n<FeatureInfos>\n";

# 1st arg = terrain name, 2nd = hash (yield=>production)
sub makefeature
{
my $tag = shift;
$href = shift;
my $desc = $tag;
$desc =~ tr/_/ /;
$desc =~ s/(\w+)/\u\L$1/g;
print FI "<FeatureInfo>\n";
print FI "\t\t<Type>FEATURE_".$tag."</Type>\n";
print FI "\t\t<Description>TXT_KEY_FEATURE_".$tag."</Description>\n";
&maketext("TXT_KEY_FEATURE_".$tag,$desc);
print FI "\t\t<Civilopedia>TXT_KEY_FEATURE_JUNGLE_PEDIA</Civilopedia>\n";
print FI "\t\t<ArtDefineTag>ART_DEF_FEATURE_".$tag."</ArtDefineTag>\n";
print FI "\t\t<YieldChanges>\n";
for my $yield ( keys (%$href) ) {
	my $prod = $href->{$yield};
	if ($prod != 0) {
		print FI "\t\t\t<YieldIntegerPair>\n";
		print FI "\t\t\t\t<YieldType>YIELD_".$yield."</YieldType>\n";
		print FI "\t\t\t\t<iValue>".$prod."</iValue>\n";
		print FI "\t\t\t</YieldIntegerPair>\n";
		}
	}
print FI "\t\t</YieldChanges>\n";
print FI "\t\t<RiverYieldIncreases/>\n";
print FI "\t\t<iMovement>2</iMovement>\n";
print FI "\t\t<iSeeThrough>1</iSeeThrough>\n";
print FI "\t\t<iDefense>25</iDefense>\n";
print FI "\t\t<iAppearance>0</iAppearance>\n";
print FI "\t\t<iDisappearance>0</iDisappearance>\n";
print FI "\t\t<iGrowth>16</iGrowth>\n";
print FI "\t\t<bNoCoast>0</bNoCoast>\n";
print FI "\t\t<bNoRiver>0</bNoRiver>\n";
print FI "\t\t<bNoAdjacent>0</bNoAdjacent>\n";
print FI "\t\t<bRequiresFlatlands>0</bRequiresFlatlands>\n";
print FI "\t\t<bRequiresRiver>0</bRequiresRiver>\n";
print FI "\t\t<bImpassable>0</bImpassable>\n";
print FI "\t\t<bNoCity>0</bNoCity>\n";
print FI "\t\t<bNoImprovement>0</bNoImprovement>\n";
print FI "\t\t<bVisibleAlways>0</bVisibleAlways>\n";
print FI "\t\t<OnUnitChangeTo/>\n";
print FI "\t\t<TerrainBooleans>\n";
print FI "\t\t<TerrainBoolean>\n";
print FI "\t\t\t<TerrainType>TERRAIN_MARSH</TerrainType>\n";
print FI "\t\t\t<bTerrain>1</bTerrain>\n";
print FI "\t\t</TerrainBoolean>\n";
print FI "\t\t<TerrainBoolean>\n";
print FI "\t\t\t<TerrainType>TERRAIN_GRASS</TerrainType>\n";
print FI "\t\t\t<bTerrain>1</bTerrain>\n";
print FI "\t\t</TerrainBoolean>\n";
print FI "\t\t<TerrainBoolean>\n";
print FI "\t\t\t<TerrainType>TERRAIN_PLAINS</TerrainType>\n";
print FI "\t\t\t<bTerrain>1</bTerrain>\n";
print FI "\t\t</TerrainBoolean>\n";
print FI "\t\t</TerrainBooleans>\n";
print FI "\t\t<FootstepSounds>\n";
print FI "\t\t<FootstepSound>\n";
print FI "\t\t\t<FootstepAudioType>FOOTSTEP_AUDIO_HUMAN</FootstepAudioType>\n";
print FI "\t\t\t<FootstepAudioScript>AS3D_UN_FOOT_UNIT_FOREST</FootstepAudioScript>\n";
print FI "\t\t</FootstepSound>\n";
print FI "\t\t<FootstepSound>\n";
print FI "\t\t\t<FootstepAudioType>FOOTSTEP_AUDIO_HUMAN_LOW</FootstepAudioType>\n";
print FI "\t\t\t<FootstepAudioScript>AS3D_UN_FOOT_UNIT_LOW_FOREST</FootstepAudioScript>\n";
print FI "\t\t</FootstepSound>\n";
print FI "\t\t<FootstepSound>\n";
print FI "\t\t\t<FootstepAudioType>FOOTSTEP_AUDIO_HORSE</FootstepAudioType>\n";
print FI "\t\t\t<FootstepAudioScript>AS3D_UN_HORSE_RUN_FOREST</FootstepAudioScript>\n";
print FI "\t\t</FootstepSound>\n";
print FI "\t\t<FootstepSound>\n";
print FI "\t\t\t<FootstepAudioType>LOOPSTEP_WHEELS</FootstepAudioType>\n";
print FI "\t\t\t<FootstepAudioScript/>\n";
print FI "\t\t</FootstepSound>\n";
print FI "\t\t<FootstepSound>\n";
print FI "\t\t\t<FootstepAudioType>ENDSTEP_WHEELS</FootstepAudioType>\n";
print FI "\t\t\t<FootstepAudioScript/>\n";
print FI "\t\t</FootstepSound>\n";
print FI "\t\t<FootstepSound>\n";
print FI "\t\t\t<FootstepAudioType>LOOPSTEP_WHEELS_2</FootstepAudioType>\n";
print FI "\t\t\t<FootstepAudioScript/>\n";
print FI "\t\t</FootstepSound>\n";
print FI "\t\t<FootstepSound>\n";
print FI "\t\t\t<FootstepAudioType>ENDSTEP_WHEELS_2</FootstepAudioType>\n";
print FI "\t\t\t<FootstepAudioScript/>\n";
print FI "\t\t</FootstepSound>\n";
print FI "\t\t<FootstepSound>\n";
print FI "\t\t\t<FootstepAudioType>LOOPSTEP_ARTILLERY</FootstepAudioType>\n";
print FI "\t\t\t<FootstepAudioScript>AS3D_UN_ARTILL_RUN_LOOP_FOREST</FootstepAudioScript>\n";
print FI "\t\t</FootstepSound>\n";
print FI "\t\t<FootstepSound>\n";
print FI "\t\t\t<FootstepAudioType>ENDSTEP_ARTILLERY</FootstepAudioType>\n";
print FI "\t\t\t<FootstepAudioScript>AS3D_UN_ARTILL_RUN_END_FOREST</FootstepAudioScript>\n";
print FI "\t\t</FootstepSound>\n";
print FI "\t\t<FootstepSound>\n";
print FI "\t\t\t<FootstepAudioType>LOOPSTEP_WHEELS_3</FootstepAudioType>\n";
print FI "\t\t\t<FootstepAudioScript>AS3D_UN_TREBUCHET_RUN_LEAVES</FootstepAudioScript>\n";
print FI "\t\t</FootstepSound>\n";
print FI "\t\t<FootstepSound>\n";
print FI "\t\t\t<FootstepAudioType>ENDSTEP_WHEELS_3</FootstepAudioType>\n";
print FI "\t\t\t<FootstepAudioScript>AS3D_UN_TREBUCHET_STOP_LEAVES</FootstepAudioScript>\n";
print FI "\t\t</FootstepSound>\n";
print FI "\t\t</FootstepSounds>\n";
print FI "\t\t<WorldSoundscapeAudioScript>ASSS_JUNGLE_SELECT_AMB</WorldSoundscapeAudioScript>\n";
print FI "\t\t<EffectType>EFFECT_BIRDSCATTER</EffectType>\n";
print FI "\t\t<iEffectProbability>15</iEffectProbability>\n";
print FI "\t\t<iAdvancedStartRemoveCost>40</iAdvancedStartRemoveCost>\n";
print FI "</FeatureInfo>\n";
}

# make features
&makefeature('FOREST',{'BIOPOLYMERS'=>4,'NUTRIENTS'=>-1});
&makefeature('LIGHT_FOREST',{'BIOPOLYMERS'=>3,'AMINO_ACIDS'=>1});
&makefeature('JUNGLE',{'BIOPOLYMERS'=>3,'XENOTOXINS'=>1});
&makefeature('ICE',{''});
print FI "</FeatureInfos>\n</Civ4FeatureInfos>\n";
close FI;

# **IMPROVEMENTS**
# generate XML for improvements and builds
open (BUILDS, '> ../Assets/XML/Units/CIV4BuildInfos.xml') or die "Can't write output: $!";
open (IM, '> ../Assets/XML/Terrain/CIV4ImprovementInfos.xml') or die "Can't write output: $!";
open (ADI, '> ../Assets/XML/Art/CIV4ArtDefines_Improvement.xml') or die "Can't write output: $!";

print BUILDS '<?xml version="1.0"?>'."\n";
print BUILDS '<!-- edited with XMLSPY v2004 rel. 2 U (http://www.xmlspy.com) by Alex Mantzaris (Firaxis Games) -->'."\n";
print BUILDS '<!-- Sid Meier\'s Civilization 4 -->'."\n".'<!-- Copyright Firaxis Games 2005 -->'."\n".'<!-- -->'."\n".'<!-- Build Infos -->'."\n";
print BUILDS '<Civ4BuildInfos xmlns="x-schema:CIV4UnitSchema.xml">'."\n<BuildInfos>\n";

print IM '<?xml version="1.0" encoding="UTF-8"?>'."\n";
print IM '<!-- edited with XMLSPY v2005 rel. 3 U (http://www.altova.com) by Soren Johnson (Firaxis Games) -->'."\n";
print IM '<!-- edited with XMLSPY v2004 rel. 2 U (http://www.xmlspy.com) by Alex Mantzaris (Firaxis Games) -->'."\n";
print IM '<!-- Sid Meier\'s Civilization 4 -->'."\n".'<!-- Copyright Firaxis Games 2005 -->'."\n".'<!-- -->'."\n".'<!-- Improvement Infos -->'."\n";
print IM '<Civ4ImprovementInfos xmlns="x-schema:CIV4TerrainSchema.xml">'."\n<ImprovementInfos>\n";

print ADI '<?xml version="1.0" encoding="UTF-8" standalone="no"?>'."\n";
print ADI '<!-- edited with XMLSPY v2004 rel. 2 U (http://www.xmlspy.com) by Alex Mantzaris (Firaxis Games) -->'."\n";
print ADI '<!-- Sid Meier\'s Civilization 4 -->'."\n".'<!-- Copyright Firaxis Games 2005 -->'."\n".'<!-- -->'."\n".'<!-- Improvement art path information -->'."\n";
print ADI '<Civ4ArtDefines xmlns="x-schema:CIV4ArtDefinesSchema.xml">'."\n<ImprovementArtInfos>\n";

foreach $item (@improvements)
	{
	my $desc = $item;
	my $tag = $item;
	$tag =~ tr/[a-z]/[A-Z]/;
	$tag =~ tr/ /_/;
	my $plural = $desc.':'.PL_N($desc);
	if (grep {$_ eq $item} @goody) {$isgoody=1;} else {$isgoody=0;}
    print IM "<ImprovementInfo>\n";
	print IM "\t<Type>IMPROVEMENT_".$tag."</Type>\n";
	print IM "\t<Description>TXT_KEY_IMPROVEMENT_".$tag."</Description>\n";
	&maketext("TXT_KEY_IMPROVEMENT_".$tag, $plural);
	print IM "\t<Civilopedia>TXT_KEY_IMPROVEMENT_".$tag."_PEDIA</Civilopedia>\n";
	my $pedia = 'Development of [COLOR_HIGHLIGHT_TEXT]'.$plural."[COLOR_REVERT] across the hostile wilderness of the New Worlds can provide an important strategic advantage.";
	&maketext("TXT_KEY_IMPROVEMENT_".$tag, $pedia);
	print IM "\t<ArtDefineTag>ART_DEF_IMPROVEMENT_".$tag."</ArtDefineTag>\n";
	print IM "\t<PrereqNatureYields/>\n";
	print IM "\t<YieldIncreases>\n";
	print IM "\t\t<YieldIntegerPair>\n";
	print IM "\t\t\t<YieldType></YieldType>\n";
	print IM "\t\t\t<iValue>0</iValue>\n";
	print IM "\t\t</YieldIntegerPair>\n";
	print IM "\t</YieldIncreases>\n";
	print IM "\t<RiverSideYieldChanges>\n";
	print IM "\t\t<YieldIntegerPair>\n";
	print IM "\t\t\t<YieldType></YieldType>\n";
	print IM "\t\t\t<iValue>0</iValue>\n";
	print IM "\t\t</YieldIntegerPair>\n";
	print IM "\t</RiverSideYieldChanges>\n";
	print IM "\t<HillsYieldChanges/>\n";
	print IM "\t<bActsAsCity>0</bActsAsCity>\n";
	print IM "\t<bHillsMakesValid>0</bHillsMakesValid>\n";
	print IM "\t<bRiverSideMakesValid>0</bRiverSideMakesValid>\n";
	print IM "\t<bRequiresFlatlands>0</bRequiresFlatlands>\n";
	print IM "\t<bRequiresRiverSide>0</bRequiresRiverSide>\n";
	print IM "\t<bRequiresFeature>0</bRequiresFeature>\n";
	print IM "\t<bWater>0</bWater>\n";
	print IM "\t<bGoody>0</bGoody>\n";
	print IM "\t<bPermanent>0</bPermanent>\n";
	print IM "\t<bUseLSystem>0</bUseLSystem>\n";
	print IM "\t<iAdvancedStartCost>10</iAdvancedStartCost>\n";
	print IM "\t<iAdvancedStartCostIncrease>5</iAdvancedStartCostIncrease>\n";
	print IM "\t<iTilesPerGoody>0</iTilesPerGoody>\n";
	print IM "\t<iGoodyRange>0</iGoodyRange>\n";
	print IM "\t<iFeatureGrowth>0</iFeatureGrowth>\n";
	print IM "\t<iUpgradeTime>0</iUpgradeTime>\n";
	print IM "\t<iDefenseModifier>0</iDefenseModifier>\n";
	print IM "\t<iPillageGold>5</iPillageGold>\n";
	print IM "\t<bOutsideBorders>0</bOutsideBorders>\n";
	print IM "\t<TerrainMakesValids>\n";
	print IM "\t\t<TerrainMakesValid>\n";
	print IM "\t\t\t<TerrainType>TERRAIN_PLAINS</TerrainType>\n";
	print IM "\t\t\t<bMakesValid>1</bMakesValid>\n";
	print IM "\t\t</TerrainMakesValid>\n";
	print IM "\t</TerrainMakesValids>\n";
	print IM "\t<FeatureMakesValids/>\n";
	print IM "\t<BonusTypeStructs/>\n";
	print IM "\t<ImprovementPillage/>\n";
	print IM "\t<ImprovementUpgrade></ImprovementUpgrade>\n";
	print IM "\t<RouteYieldChanges/>\n";
	print IM "\t<bRequiresCityYields>0</bRequiresCityYields>\n";
	print IM "\t<RequiredCityYields>\n";
	print IM "\t\t<RequiredCityYield>\n";
	print IM "\t\t\t<YieldType></YieldType>\n";
	print IM "\t\t\t<iValue>0</iValue>\n";
	print IM "\t\t</RequiredCityYield>\n";
	print IM "\t</RequiredCityYields>\n";
	print IM "\t<CreatesBonus></CreatesBonus>\n";
	print IM "\t<bGraphicalOnly>0</bGraphicalOnly>\n";
	print IM "</ImprovementInfo>\n";
	
	print ADI "\t<ImprovementArtInfo>\n";
	print ADI "\t\t<Type>ART_DEF_IMPROVEMENT_".$tag."</Type>\n";
	print ADI "\t\t<bExtraAnimations>0</bExtraAnimations>\n";
	print ADI "\t\t<fScale>0.5</fScale>\n";
	print ADI "\t\t<fInterfaceScale>0.8</fInterfaceScale>\n";
	print ADI "\t\t<NIF>Art/Improvements/".$tag.'/'.$tag.".nif</NIF>\n";
	print ADI "\t\t<KFM/>\n";
	print ADI "\t\t<Button>Art/Buttons/Improvements/".$tag.".dds</Button>\n";
	print ADI "\t</ImprovementArtInfo>\n";

	if ($isgoody == 0) {
	print BUILDS "\t<BuildInfo>\n";
	print BUILDS "\t\t<Type>BUILD_".$tag."</Type>\n";
	print BUILDS "\t\t<Description>Build ".A($desc)."</Description>\n";
	print BUILDS "\t\t<Help/>\n";
	print BUILDS "\t\t<iTime>1000</iTime>\n";
	print BUILDS "\t\t<iCost>100</iCost>\n";
	print BUILDS "\t\t<bKill>0</bKill>\n";
	print BUILDS "\t\t<ImprovementType>IMPROVEMENT_".$tag."</ImprovementType>\n";
	print BUILDS "\t\t<RouteType>NONE</RouteType>\n";
	print BUILDS "\t\t<EntityEvent>ENTITY_EVENT_BUILD</EntityEvent>\n";
	print BUILDS "\t\t<FeatureStructs/>\n";
	print BUILDS "\t\t<iCityType>-1</iCityType>\n";
	print BUILDS "\t\t<HotKey>0</HotKey>\n";
	print BUILDS "\t\t<bAltDown>0</bAltDown>\n";
	print BUILDS "\t\t<bShiftDown>0</bShiftDown>\n";
	print BUILDS "\t\t<bCtrlDown>0</bCtrlDown>\n";
	print BUILDS "\t\t<iHotKeyPriority>0</iHotKeyPriority>\n";
	print BUILDS "\t\t<Button>Art\Interface\Game Hud\Actions\BuildTower.dds</Button>\n";
	print BUILDS "\t</BuildInfo>\n";}
	
	}
print BUILDS "</BuildInfos>\n</Civ4BuildInfos>\n";
close BUILDS;
print IM "</ImprovementInfos>\n</Civ4ImprovementInfos>\n";
close IM;
print ADI "</ImprovementArtInfos>\n</Civ4ArtDefines>\n";
close ADI;
	
# **BUILDINGS**
# generate building XML

# open XML for writing
open (BI, '> ../Assets/XML/Buildings/CIV4BuildingInfos.xml') or die "Can't write output: $!";
open (BCI, '> ../Assets/XML/Buildings/CIV4BuildingClassInfos.xml') or die "Can't write output: $!";
open (SBI, '> ../Assets/XML/Buildings/CIV4SpecialBuildingInfos.xml') or die "Can't write output: $!";
open (ADB, '> ../Assets/XML/Art/CIV4ArtDefines_Building.xml') or die "Can't write output: $!";

# generate XML headers
print BI '<?xml version="1.0"?>'."\n";
print BI '<!-- edited with XMLSPY v2004 rel. 2 U (http://www.xmlspy.com) by Josh Scanlan (Firaxis Games) -->'."\n";
print BI '<!-- Sid Meier\'s Civilization 4 -->'."\n".'<!-- Copyright Firaxis Games 2005 -->'."\n".'<!-- -->'."\n".'<!-- Building Infos -->'."\n";
print BI '<Civ4BuildingInfos xmlns="x-schema:CIV4BuildingsSchema.xml">'."\n<BuildingInfos>\n";

print BCI '<?xml version="1.0"?>'."\n";
print BCI '<!-- edited with XMLSPY v2004 rel. 2 U (http://www.xmlspy.com) by Jason Winokur (Firaxis Games) -->'."\n";
print BCI '<!-- Sid Meier\'s Civilization 4 -->'."\n".'<!-- Copyright Firaxis Games 2005 -->'."\n".'<!-- -->'."\n".'<!-- Building Class Infos -->'."\n";
print BCI '<Civ4BuildingClassInfos xmlns="x-schema:CIV4BuildingsSchema.xml">'."\n<BuildingClassInfos>\n";

print SBI '<?xml version="1.0"?>'."\n";
print SBI '<!-- edited with XMLSPY v2004 rel. 2 U (http://www.xmlspy.com) by EXTREME (Firaxis Games) -->'."\n";
print SBI '<!-- Sid Meier\'s Civilization 4 -->'."\n".'<!-- Copyright Firaxis Games 2005 -->'."\n".'<!-- -->'."\n".'<!-- Special Building -->'."\n";
print SBI '<Civ4SpecialBuildingInfos xmlns="x-schema:CIV4BuildingsSchema.xml">'."\n<SpecialBuildingInfos>\n";

print ADB '<?xml version="1.0" encoding="UTF-8" standalone="no"?>'."\n";
print ADB '<!-- edited with XMLSPY v2004 rel. 2 U (http://www.xmlspy.com) by Jason Winokur (Firaxis Games) -->'."\n";
print ADB '<!-- Sid Meier\'s Civilization 4 -->'."\n".'<!-- Copyright Firaxis Games 2005 -->'."\n".'<!-- -->'."\n".'<!-- Building art path information -->'."\n";
print ADB '<Civ4ArtDefines xmlns="x-schema:CIV4ArtDefinesSchema.xml">'."\n<BuildingArtInfos>\n";

# generate Buildings XML
$index = 0;
foreach $item (@allbuildings)
	{
	if (grep {$_ eq $item} @onetierbuildings) {$isonetier = 1;} else {$isonetier = 0;}
	if (grep {$_ eq $item} @miscbuildings) {$isnoyield = 1;} else {$isnoyield = 0;}
	my $desc = $item;
	$desc =~ tr/_/ /;
	$desc =~ s/(\w+)/\u\L$1/g;
	$item =~ tr/ /_/;
	$item =~ tr/[a-z]/[A-Z]/;

	# make specialbuilding for item
	print SBI "<SpecialBuildingInfo>\n";	
	print SBI "\t<Type>SPECIALBUILDING_".$item."</Type>\n";
	print SBI "\t<Description>TXT_KEY_YIELD_".$item."</Description>\n";
	print SBI "\t<bValid>1</bValid>\n";
	print SBI "\t<FontButtonIndex>".$index."</FontButtonIndex>\n";
	print SBI "\t<ProductionTraits/>\n";
	print SBI "\t".'<Button>Art/Buttons/Yields/'.$item.'.dds</Button>'."\n";
	print SBI "</SpecialBuildingInfo>\n";
	$index++;

	# make level 1 building
	if (not $isonetier) {
		if ($item=~/TOOLS/ or $item=~/MUNITIONS/ or $item=~/ROBOTICS/) {
			$suffix = 'Workshop';
			} else {
			$suffix = 'Facility';
			}
		$bdesc = $desc.' '.$suffix;
		if (A($bdesc) =~ /^(\w+?) / ) {$article = $1;}
		$plural = $desc.' '.PL_N($suffix);
		$tag = $item.'1';
		}
	else
		{$tag = $item;}
	print BI "<BuildingInfo>\n";	
	print BI "\t<Type>BUILDING_".$tag."</Type>\n";
	print BI "\t<BuildingClass>BUILDINGCLASS_".$tag."</BuildingClass>\n";
	print BI "\t<SpecialBuildingType>SPECIALBUILDING_".$item."</SpecialBuildingType>\n";
	print BI "\t<iSpecialBuildingPriority>0</iSpecialBuildingPriority>\n";
	print BI "\t<Description>TXT_KEY_BUILDING_".$tag."</Description>\n";
	&maketext("TXT_KEY_BUILDING_".$tag, $bdesc.':'.$plural);
	print BI "\t<Civilopedia>TXT_KEY_PEDIA_BUILDING_".$tag."</Civilopedia>\n";
	my $pedia = ucfirst($article).' [COLOR_BUILDING_TEXT]'.$bdesc.'[COLOR_REVERT] allows basic production of [LINK=YIELD_'.$item.']'.$desc.'[\LINK] by citizens working in the [LINK=PROFESSION_'.$item.']'.$desc.'[\LINK] profession.[NEWLINE][PARAGRAPH:1]While the Earth superpowers initially maintained tight control over the complex and highly profitable production of '.$desc.' from raw materials imported from the New Worlds, development of the first small-scale '.$plural." by Human colonists and Alien empires eventually began to challenge Earth's previously unassailable monopoly.";
	&maketext("TXT_KEY_PEDIA_BUILDING_".$tag, $pedia);
	print BI "\t<Strategy>TXT_KEY_STRATEGY_BUILDING_".$tag."</Strategy>\n";
	my $strategy = "Build ".$article." [COLOR_BUILDING_TEXT]".$bdesc."[COLOR_REVERT] to allow basic production of [COLOR_HIGHLIGHT_TEXT]".$desc.'[COLOR_REVERT].';
	&maketext("TXT_KEY_STRATEGY_BUILDING_".$tag, $strategy);
	print BI "\t<ArtDefineTag>ART_DEF_BUILDING_".$tag."</ArtDefineTag>\n";
	print BI "\t<MovieDefineTag>NONE</MovieDefineTag>\n";
	print BI "\t<VictoryPrereq>NONE</VictoryPrereq>\n";
	print BI "\t<FreeStartEra>NONE</FreeStartEra>\n";
	print BI "\t<MaxStartEra>NONE</MaxStartEra>\n";
	print BI "\t<iCityType/>\n";
	print BI "\t<ProductionTraits/>\n";
	print BI "\t<FreePromotion>NONE</FreePromotion>\n";
	print BI "\t<bGraphicalOnly>0</bGraphicalOnly>\n";
	print BI "\t<bWorksWater>0</bWorksWater>\n";
	print BI "\t<bWater>0</bWater>\n";
	print BI "\t<bRiver>0</bRiver>\n";
	print BI "\t<bCapital>0</bCapital>\n";
	print BI "\t<bNeverCapture>0</bNeverCapture>\n";
	print BI "\t<bCenterInCity>0</bCenterInCity>\n";
	print BI "\t<iAIWeight>10</iAIWeight>\n";
	print BI "\t<YieldCosts>\n";
	print BI "\t\t<YieldCost>\n";
	print BI "\t\t\t<YieldType>YIELD_INDUSTRY</YieldType>\n";	
	print BI "\t\t\t<iCost>50</iCost>\n";	
	print BI "\t\t</YieldCost>\n";
	print BI "\t</YieldCosts>\n";
	print BI "\t<iHurryCostModifier>0</iHurryCostModifier>\n";
	print BI "\t<iAdvancedStartCost>0</iAdvancedStartCost>\n";
	print BI "\t<iAdvancedStartCostIncrease>0</iAdvancedStartCostIncrease>\n";
	print BI "\t<iProfessionOutput>3</iProfessionOutput>\n";
	print BI "\t<iMaxWorkers>2</iMaxWorkers>\n";
	print BI "\t<iMinAreaSize>-1</iMinAreaSize>\n";
	print BI "\t<iConquestProb>0</iConquestProb>\n";
	print BI "\t<iCitiesPrereq>0</iCitiesPrereq>\n";
	print BI "\t<iTeamsPrereq>0</iTeamsPrereq>\n";
	print BI "\t<iLevelPrereq>0</iLevelPrereq>\n";
	print BI "\t<iMinLatitude>0</iMinLatitude>\n";
	print BI "\t<iMaxLatitude>90</iMaxLatitude>\n";
	print BI "\t<iExperience>0</iExperience>\n";
	print BI "\t<iFoodKept>0</iFoodKept>\n";
	print BI "\t<iHealRateChange>0</iHealRateChange>\n";
	print BI "\t<iMilitaryProductionModifier>0</iMilitaryProductionModifier>\n";
	print BI "\t<iDefense>0</iDefense>\n";
	print BI "\t<iBombardDefense>0</iBombardDefense>\n";
	print BI "\t<iAsset>50</iAsset>\n";
	print BI "\t<iPower>0</iPower>\n";
	print BI "\t<iYieldStorage>0</iYieldStorage>\n";
	print BI "\t<iOverflowSellPercent>0</iOverflowSellPercent>\n";
	print BI "\t<fVisibilityPriority>1.0</fVisibilityPriority>\n";
	print BI "\t<SeaPlotYieldChanges/>\n";	
	print BI "\t<RiverPlotYieldChanges/>\n";
	print BI "\t<YieldChanges/>\n";
	print BI "\t<YieldModifiers>\n";
	print BI "\t\t<YieldModifier>\n";
	if ($isnoyield or $isonetier) {print BI "\t\t\t<YieldType></YieldType>\n";}
		else {print BI "\t\t\t<YieldType>YIELD_".$item."</YieldType>\n";}	
	print BI "\t\t\t<iModifier>0</iModifier>\n";	
	print BI "\t\t</YieldModifier>\n";
	print BI "\t</YieldModifiers>\n";
	print BI "\t<ConstructSound/>\n";
	print BI "\t<UnitCombatFreeExperiences/>\n";
	print BI "\t<DomainFreeExperiences/>\n";
	print BI "\t<DomainProductionModifiers/>\n";
	print BI "\t<PrereqBuildingClasses/>\n";
	print BI "\t<BuildingClassNeededs/>\n";
	print BI "\t<AutoSellsYields/>\n";
	print BI "\t<HotKey/>\n";
	print BI "\t<bAltDown>0</bAltDown>\n";	
	print BI "\t<bShiftDown>0</bShiftDown>\n";
	print BI "\t<bCtrlDown>0</bCtrlDown>\n";
	print BI "\t<iHotKeyPriority>0</iHotKeyPriority>\n";
	print BI "</BuildingInfo>\n";

	&makebclass($tag, $bdesc);

	print ADB "<BuildingArtInfo>\n";
	print ADB "\t<Type>ART_DEF_BUILDING_".$tag."</Type>\n";
	print ADB "\t<LSystem>LSYSTEM_1x1</LSystem>\n";
	print ADB "\t<bAnimated>0</bAnimated>\n";
	print ADB "\t<CityTexture>Art/Buttons/Buildings/".$tag.'.dds</CityTexture>'."\n";
	print ADB "\t<CitySelectedTexture>".',IS,FILLER,TEXT</CitySelectedTexture>'."\n";
	print ADB "\t<fScale>1.0</fScale>\n";
	print ADB "\t<fInterfaceScale>0.5</fInterfaceScale>\n";
	print ADB "\t<NIF>Art/Buildings/".$item.'/'.$tag.'.nif</NIF>'."\n";
	print ADB "\t<KFM/>\n";
	print ADB "\t<Button>Art/Buttons/Buildings/".$tag.'.dds</Button>'."\n";
	print ADB "</BuildingArtInfo>\n";

	next if $isonetier;
	
	# level 2
	my $tag = $item.'2';
	if ($tag=~/TOOLS/ or $tag=~/MUNITIONS/ or $tag=~/ROBOTICS/) {$suffix = 'Plant';}
		elsif ($tag=~/NARCOTICS/ or $tag=~/BIOWEAPONS/ or $tag=~/PHARMACEUTICALS/ or $tag=~/PETROCHEMICALS/ or $tag=~/COLLOIDS/ or $tag=~/CATALYSTS/) {$suffix = 'Refinery';}
		else {$suffix = 'Lab';}
	my $bdesc = $desc.' '.$suffix;
	if (A($bdesc) =~ /^(\w+?) / ) {$article = $1;}
	my $plural = $desc.' '.PL_N($suffix);
	print BI "<BuildingInfo>\n";	
	print BI "\t<Type>BUILDING_".$tag."</Type>\n";
	print BI "\t<BuildingClass>BUILDINGCLASS_".$tag."</BuildingClass>\n";
	print BI "\t<SpecialBuildingType>SPECIALBUILDING_".$item."</SpecialBuildingType>\n";
	print BI "\t<iSpecialBuildingPriority>1</iSpecialBuildingPriority>\n";
	print BI "\t<Description>TXT_KEY_BUILDING_".$tag."</Description>\n";
	&maketext("TXT_KEY_BUILDING_".$tag, $bdesc.':'.$plural);
	print BI "\t<Civilopedia>TXT_KEY_PEDIA_BUILDING_".$tag."</Civilopedia>\n";
	my $pedia = ucfirst($article).' [COLOR_BUILDING_TEXT]'.$bdesc.'[COLOR_REVERT] enhances production of [LINK=YIELD_'.$item.']'.$desc.'[\LINK] by citizens working in the [LINK=PROFESSION_'.$item.']'.$desc.'[\LINK] profession.[NEWLINE][PARAGRAPH:1]By leveraging their growing industrial base to enable the construction of '.$plural.', Human colonies and Alien empires became increasingly able to produce '.$desc." more efficiently and on a larger scale, furthering their economic independence from the mercantilist industries of Earth.";
	&maketext("TXT_KEY_PEDIA_BUILDING_".$tag, $pedia);
	print BI "\t<Strategy>TXT_KEY_STRATEGY_BUILDING_".$tag."</Strategy>\n";
	my $strategy = "Build ".$article." [COLOR_BUILDING_TEXT]".$bdesc."[COLOR_REVERT] to allow more efficient production of [COLOR_HIGHLIGHT_TEXT]".$desc.'[COLOR_REVERT].';
	&maketext("TXT_KEY_STRATEGY_BUILDING_".$tag, $strategy);
	print BI "\t<ArtDefineTag>ART_DEF_BUILDING_".$tag."</ArtDefineTag>\n";
	print BI "\t<MovieDefineTag>NONE</MovieDefineTag>\n";
	print BI "\t<VictoryPrereq>NONE</VictoryPrereq>\n";
	print BI "\t<FreeStartEra>NONE</FreeStartEra>\n";
	print BI "\t<MaxStartEra>NONE</MaxStartEra>\n";
	print BI "\t<iCityType/>\n";
	print BI "\t<ProductionTraits/>\n";
	print BI "\t<FreePromotion>NONE</FreePromotion>\n";
	print BI "\t<bGraphicalOnly>0</bGraphicalOnly>\n";
	print BI "\t<bWorksWater>0</bWorksWater>\n";
	print BI "\t<bWater>0</bWater>\n";
	print BI "\t<bRiver>0</bRiver>\n";
	print BI "\t<bCapital>0</bCapital>\n";
	print BI "\t<bNeverCapture>0</bNeverCapture>\n";
	print BI "\t<bCenterInCity>0</bCenterInCity>\n";
	print BI "\t<iAIWeight>10</iAIWeight>\n";
	print BI "\t<YieldCosts>\n";
	print BI "\t\t<YieldCost>\n";
	print BI "\t\t\t<YieldType>YIELD_INDUSTRY</YieldType>\n";	
	print BI "\t\t\t<iCost>100</iCost>\n";	
	print BI "\t\t</YieldCost>\n";
	print BI "\t\t<YieldCost>\n";
	print BI "\t\t\t<YieldType>YIELD_MACHINE_TOOLS</YieldType>\n";	
	print BI "\t\t\t<iCost>50</iCost>\n";	
	print BI "\t\t</YieldCost>\n";
	print BI "\t</YieldCosts>\n";
	print BI "\t<iHurryCostModifier>0</iHurryCostModifier>\n";
	print BI "\t<iAdvancedStartCost>0</iAdvancedStartCost>\n";
	print BI "\t<iAdvancedStartCostIncrease>0</iAdvancedStartCostIncrease>\n";
	print BI "\t<iProfessionOutput>6</iProfessionOutput>\n";
	print BI "\t<iMaxWorkers>3</iMaxWorkers>\n";
	print BI "\t<iMinAreaSize>-1</iMinAreaSize>\n";
	print BI "\t<iConquestProb>0</iConquestProb>\n";
	print BI "\t<iCitiesPrereq>0</iCitiesPrereq>\n";
	print BI "\t<iTeamsPrereq>0</iTeamsPrereq>\n";
	print BI "\t<iLevelPrereq>0</iLevelPrereq>\n";
	print BI "\t<iMinLatitude>0</iMinLatitude>\n";
	print BI "\t<iMaxLatitude>90</iMaxLatitude>\n";
	print BI "\t<iExperience>0</iExperience>\n";
	print BI "\t<iFoodKept>0</iFoodKept>\n";
	print BI "\t<iHealRateChange>0</iHealRateChange>\n";
	print BI "\t<iMilitaryProductionModifier>0</iMilitaryProductionModifier>\n";
	print BI "\t<iDefense>0</iDefense>\n";
	print BI "\t<iBombardDefense>0</iBombardDefense>\n";
	print BI "\t<iAsset>100</iAsset>\n";
	print BI "\t<iPower>0</iPower>\n";
	print BI "\t<iYieldStorage>0</iYieldStorage>\n";
	print BI "\t<iOverflowSellPercent>0</iOverflowSellPercent>\n";
	print BI "\t<fVisibilityPriority>1.0</fVisibilityPriority>\n";
	print BI "\t<SeaPlotYieldChanges/>\n";	
	print BI "\t<RiverPlotYieldChanges/>\n";
	print BI "\t<YieldChanges/>\n";
	print BI "\t<YieldModifiers>\n";
	print BI "\t\t<YieldModifier>\n";
	if ($isnoyield) {print BI "\t\t\t<YieldType></YieldType>\n\t\t\t<iModifier>0</iModifier>\n";}
		else {print BI "\t\t\t<YieldType>YIELD_".$item."</YieldType>\n\t\t\t<iModifier>25</iModifier>\n";}
	print BI "\t\t</YieldModifier>\n";
	print BI "\t</YieldModifiers>\n";
	print BI "\t<ConstructSound/>\n";
	print BI "\t<UnitCombatFreeExperiences/>\n";
	print BI "\t<DomainFreeExperiences/>\n";
	print BI "\t<DomainProductionModifiers/>\n";
	print BI "\t<PrereqBuildingClasses/>\n";
	print BI "\t<BuildingClassNeededs>\n";
	print BI "\t\t<BuildingClassNeeded>\n";
	print BI "\t\t\t<BuildingClassType>BUILDINGCLASS_".$item."1</BuildingClassType>\n";
	print BI "\t\t\t<bNeededInCity>1</bNeededInCity>\n";
	print BI "\t\t</BuildingClassNeeded>\n";
	print BI "\t</BuildingClassNeededs>\n";
	print BI "\t<AutoSellsYields/>\n";
	print BI "\t<HotKey/>\n";
	print BI "\t<bAltDown>0</bAltDown>\n";	
	print BI "\t<bShiftDown>0</bShiftDown>\n";
	print BI "\t<bCtrlDown>0</bCtrlDown>\n";
	print BI "\t<iHotKeyPriority>0</iHotKeyPriority>\n";
	print BI "</BuildingInfo>\n";

	&makebclass($tag, $bdesc);

	print ADB "<BuildingArtInfo>\n";
	print ADB "\t<Type>ART_DEF_BUILDING_".$tag."</Type>\n";
	print ADB "\t<LSystem>LSYSTEM_2x1</LSystem>\n";
	print ADB "\t<bAnimated>0</bAnimated>\n";
	print ADB "\t<CityTexture>Art/Buttons/Buildings/".$tag.'.dds</CityTexture>'."\n";
	print ADB "\t<CitySelectedTexture>".',IS,FILLER,TEXT</CitySelectedTexture>'."\n";
	print ADB "\t<fScale>1.0</fScale>\n";
	print ADB "\t<fInterfaceScale>0.5</fInterfaceScale>\n";
	print ADB "\t<NIF>Art/Buildings/".$item.'/'.$tag.'.nif</NIF>'."\n";
	print ADB "\t<KFM/>\n";
	print ADB "\t<Button>Art/Buttons/Buildings/".$tag.'.dds</Button>'."\n";
	print ADB "</BuildingArtInfo>\n";

	# level 3
	my $tag = $item.'3';
	if ($tag=~/TOOLS/ or $tag=~/MUNITIONS/ or $tag=~/ROBOTICS/ or $tag=~/SEMICONDUCTORS/) {$suffix = 'Foundry';}
		elsif ($tag=~/PLASMIDS/ or $tag=~/ENZYMES/ or $tag=~/PROGENITOR_TECH/ or $tag=~/ALIEN_RELICS/ or $tag=~/STATE_SECRETS/ or $tag=~/MICROBES/) {$suffix = 'Institute';}
		else {$suffix = 'Complex';}
	my $bdesc = $desc.' '.$suffix;
	if (A($bdesc) =~ /^(\w+?) / ) {$article = $1;}
	my $plural = $desc.' '.PL_N($suffix);
	print BI "<BuildingInfo>\n";	
	print BI "\t<Type>BUILDING_".$tag."</Type>\n";
	print BI "\t<BuildingClass>BUILDINGCLASS_".$tag."</BuildingClass>\n";
	print BI "\t<SpecialBuildingType>SPECIALBUILDING_".$item."</SpecialBuildingType>\n";
	print BI "\t<iSpecialBuildingPriority>2</iSpecialBuildingPriority>\n";	
	print BI "\t<Description>TXT_KEY_BUILDING_".$tag."</Description>\n";
	&maketext("TXT_KEY_BUILDING_".$tag, $bdesc.':'.$plural);
	print BI "\t<Civilopedia>TXT_KEY_PEDIA_BUILDING_".$tag."</Civilopedia>\n";
	my $pedia = ucfirst($article).' [COLOR_BUILDING_TEXT]'.$bdesc.'[COLOR_REVERT] enables highly efficient large-scale production of [LINK=YIELD_'.$item.']'.$desc.'[\LINK] by citizens working in the [LINK=PROFESSION_'.$item.']'.$desc.'[\LINK] profession.[NEWLINE][PARAGRAPH:1]The growing technological sophistication and infrastructure of several Human colonies and Alien empires eventually enabled the construction of large '.$plural.' which could produce '.$desc.' far more efficiently than the aging industrial base of Earth.';
	&maketext("TXT_KEY_PEDIA_BUILDING_".$tag, $pedia);
	print BI "\t<Strategy>TXT_KEY_STRATEGY_BUILDING_".$tag."</Strategy>\n";
	my $strategy = "Build ".$article." [COLOR_BUILDING_TEXT]".$bdesc."[COLOR_REVERT] to maximize production of [COLOR_HIGHLIGHT_TEXT]".$desc.'[COLOR_REVERT].';
	&maketext("TXT_KEY_STRATEGY_BUILDING_".$tag, $strategy);
	print BI "\t<ArtDefineTag>ART_DEF_BUILDING_".$tag."</ArtDefineTag>\n";
	print BI "\t<MovieDefineTag>NONE</MovieDefineTag>\n";
	print BI "\t<VictoryPrereq>NONE</VictoryPrereq>\n";
	print BI "\t<FreeStartEra>NONE</FreeStartEra>\n";
	print BI "\t<MaxStartEra>NONE</MaxStartEra>\n";
	print BI "\t<iCityType/>\n";
	print BI "\t<ProductionTraits/>\n";
	print BI "\t<FreePromotion>NONE</FreePromotion>\n";
	print BI "\t<bGraphicalOnly>0</bGraphicalOnly>\n";
	print BI "\t<bWorksWater>0</bWorksWater>\n";
	print BI "\t<bWater>0</bWater>\n";
	print BI "\t<bRiver>0</bRiver>\n";
	print BI "\t<bCapital>0</bCapital>\n";
	print BI "\t<bNeverCapture>0</bNeverCapture>\n";
	print BI "\t<bCenterInCity>0</bCenterInCity>\n";
	print BI "\t<iAIWeight>10</iAIWeight>\n";
	print BI "\t<YieldCosts>\n";
	print BI "\t\t<YieldCost>\n";
	print BI "\t\t\t<YieldType>YIELD_INDUSTRY</YieldType>\n";	
	print BI "\t\t\t<iCost>200</iCost>\n";	
	print BI "\t\t</YieldCost>\n";
	print BI "\t\t<YieldCost>\n";
	print BI "\t\t\t<YieldType>YIELD_MACHINE_TOOLS</YieldType>\n";	
	print BI "\t\t\t<iCost>100</iCost>\n";	
	print BI "\t\t</YieldCost>\n";
	print BI "\t\t<YieldCost>\n";
	print BI "\t\t\t<YieldType>YIELD_EARTH_GOODS</YieldType>\n";	
	print BI "\t\t\t<iCost>50</iCost>\n";	
	print BI "\t\t</YieldCost>\n";
	print BI "\t</YieldCosts>\n";
	print BI "\t<iHurryCostModifier>0</iHurryCostModifier>\n";
	print BI "\t<iAdvancedStartCost>0</iAdvancedStartCost>\n";
	print BI "\t<iAdvancedStartCostIncrease>0</iAdvancedStartCostIncrease>\n";
	print BI "\t<iProfessionOutput>6</iProfessionOutput>\n";
	print BI "\t<iMaxWorkers>5</iMaxWorkers>\n";
	print BI "\t<iMinAreaSize>-1</iMinAreaSize>\n";
	print BI "\t<iConquestProb>0</iConquestProb>\n";
	print BI "\t<iCitiesPrereq>0</iCitiesPrereq>\n";
	print BI "\t<iTeamsPrereq>0</iTeamsPrereq>\n";
	print BI "\t<iLevelPrereq>0</iLevelPrereq>\n";
	print BI "\t<iMinLatitude>0</iMinLatitude>\n";
	print BI "\t<iMaxLatitude>90</iMaxLatitude>\n";
	print BI "\t<iExperience>0</iExperience>\n";
	print BI "\t<iFoodKept>0</iFoodKept>\n";
	print BI "\t<iHealRateChange>0</iHealRateChange>\n";
	print BI "\t<iMilitaryProductionModifier>0</iMilitaryProductionModifier>\n";
	print BI "\t<iDefense>0</iDefense>\n";
	print BI "\t<iBombardDefense>0</iBombardDefense>\n";
	print BI "\t<iAsset>200</iAsset>\n";
	print BI "\t<iPower>0</iPower>\n";
	print BI "\t<iYieldStorage>0</iYieldStorage>\n";
	print BI "\t<iOverflowSellPercent>0</iOverflowSellPercent>\n";
	print BI "\t<fVisibilityPriority>1.0</fVisibilityPriority>\n";
	print BI "\t<SeaPlotYieldChanges/>\n";	
	print BI "\t<RiverPlotYieldChanges/>\n";
	print BI "\t<YieldChanges/>\n";
	print BI "\t<YieldModifiers>\n";
	print BI "\t\t<YieldModifier>\n";
	if ($isnoyield) {print BI "\t\t\t<YieldType></YieldType>\n\t\t\t<iModifier>0</iModifier>\n";}
		else {print BI "\t\t\t<YieldType>YIELD_".$item."</YieldType>\n\t\t\t<iModifier>50</iModifier>\n";}
	print BI "\t\t</YieldModifier>\n";
	print BI "\t</YieldModifiers>\n";
	print BI "\t<ConstructSound/>\n";
	print BI "\t<UnitCombatFreeExperiences/>\n";
	print BI "\t<DomainFreeExperiences/>\n";
	print BI "\t<DomainProductionModifiers/>\n";
	print BI "\t<PrereqBuildingClasses/>\n";
	print BI "\t<BuildingClassNeededs>\n";
	print BI "\t\t<BuildingClassNeeded>\n";
	print BI "\t\t\t<BuildingClassType>BUILDINGCLASS_".$item."2</BuildingClassType>\n";
	print BI "\t\t\t<bNeededInCity>1</bNeededInCity>\n";
	print BI "\t\t</BuildingClassNeeded>\n";
	print BI "\t</BuildingClassNeededs>\n";
	print BI "\t<AutoSellsYields/>\n";
	print BI "\t<HotKey/>\n";
	print BI "\t<bAltDown>0</bAltDown>\n";	
	print BI "\t<bShiftDown>0</bShiftDown>\n";
	print BI "\t<bCtrlDown>0</bCtrlDown>\n";
	print BI "\t<iHotKeyPriority>0</iHotKeyPriority>\n";
	print BI "</BuildingInfo>\n";

	&makebclass($tag, $bdesc);

	print ADB "<BuildingArtInfo>\n";
	print ADB "\t<Type>ART_DEF_BUILDING_".$tag."</Type>\n";
	print ADB "\t<LSystem>LSYSTEM_2x2</LSystem>\n";
	print ADB "\t<bAnimated>0</bAnimated>\n";
	print ADB "\t<CityTexture>Art/Buttons/Buildings/".$tag.'.dds</CityTexture>'."\n";
	print ADB "\t<CitySelectedTexture>".',IS,FILLER,TEXT</CitySelectedTexture>'."\n";
	print ADB "\t<fScale>1.0</fScale>\n";
	print ADB "\t<fInterfaceScale>0.5</fInterfaceScale>\n";
	print ADB "\t<NIF>Art/Buildings/".$item.'/'.$tag.'.nif</NIF>'."\n";
	print ADB "\t<KFM/>\n";
	print ADB "\t<Button>Art/Buttons/Buildings/".$tag.'.dds</Button>'."\n";
	print ADB "</BuildingArtInfo>\n";
	}	

# closing tags
print BI '</BuildingInfos>'."\n</Civ4BuildingInfos>\n";
close BI;
print BCI '</BuildingClassInfos>'."\n</Civ4BuildingClassInfos>\n";
close BCI;
print SBI '</SpecialBuildingInfos>'."\n</Civ4SpecialBuildingInfos>\n";
close SBI;
print ADB '</BuildingArtInfos>'."\n</Civ4ArtDefines>\n";
close ADB;

# **UNITS**
#!/usr/bin/perl
# generate unit, unitclass, unitartdef XML for each specialist and cargo
	
# open XML for writing
open (UI, '> ../Assets/XML/Units/CIV4UnitInfos.xml') or die "Can't write output: $!";
open (UCI, '> ../Assets/XML/Units/CIV4UnitClassInfos.xml') or die "Can't write output: $!";
open (ADU, '> ../Assets/XML/Art/CIV4ArtDefines_Unit.xml') or die "Can't write output: $!";

# generate XML headers
print UI '<?xml version="1.0" encoding="UTF-8"?>'."\n";
print UI '<!-- edited with XMLSPY v2004 rel. 2 U (http://www.xmlspy.com) by EXTREME (Firaxis Games) -->'."\n";
print UI '<!-- Sid Meier\'s Civilization 4 -->'."\n".'<!-- Copyright Firaxis Games 2005 -->'."\n".'<!-- -->'."\n".'<!-- Unit Infos -->'."\n";
print UI '<Civ4UnitInfos xmlns="x-schema:CIV4UnitSchema.xml">'."\n<UnitInfos>\n";

print UCI '<?xml version="1.0"?>'."\n";
print UCI '<!-- edited with XMLSPY v2004 rel. 2 U (http://www.xmlspy.com) by EXTREME (Firaxis Games) -->'."\n";
print UCI '<!-- Sid Meier\'s Civilization 4 -->'."\n".'<!-- Copyright Firaxis Games 2005 -->'."\n".'<!-- -->'."\n".'<!-- UnitClass Schema -->'."\n";
print UCI '<Civ4UnitClassInfos xmlns="x-schema:CIV4UnitSchema.xml">'."\n<UnitClassInfos>\n";

print ADU '<?xml version="1.0" encoding="UTF-8" standalone="no"?>'."\n";
print ADU '<!-- edited with XMLSPY v2004 rel. 2 U (http://www.xmlspy.com) by XMLSPY 2004 Professional Ed. Release 2, Installed Multi + SMP for 3 users (Firaxis Games) -->'."\n";
print ADU '<!-- Sid Meier\'s Civilization 4 -->'."\n".'<!-- Copyright Firaxis Games 2005 -->'."\n".'<!-- -->'."\n".'<!-- Unit art path information -->'."\n";
print ADU '<Civ4ArtDefines xmlns="x-schema:CIV4ArtDefinesSchema.xml">'."\n<UnitArtInfos>\n";

# generate units xml

sub placeship
{
	print ADU "\t<NIF>Art/Units/Caravel/Caravel_FX.nif</NIF>\n";
	print ADU "\t<KFM>Art/Units/Caravel/Caravel.kfm</KFM>\n";
}

sub placeland
{
	print ADU "\t<NIF>Art/Units/Free_Colonist/Free_Colonist.nif</NIF>\n";
	print ADU "\t<KFM>Art/Units/Free_Colonist/Free_Colonist.kfm</KFM>\n";
}

foreach $item (@allspecialists)
	{
	my @skills = @$item;
	my $desc = shift(@skills);
	my $tag = $desc;
	$tag =~ tr/ /_/;
	$tag =~ tr/a-z/A-Z/;
	if (A($desc) =~ /^(\w+?) / ) {$article = $1;}
	my $plural = PL_N($desc);
	my $fulldesc = $desc.':'.$plural;
	print UI "<UnitInfo>\n";	
	print UI "\t<Type>UNIT_".$tag."</Type>\n";
	print UI "\t<Class>UNITCLASS_".$tag."</Class>\n";
	print UI "\t<UniqueNames/>\n";
	print UI "\t<Special>NONE</Special>\n";
	print UI "\t<Capture>NONE</Capture>\n";
	print UI "\t<Combat>NONE</Combat>\n";
	print UI "\t<Domain>DOMAIN_LAND</Domain>\n";
	print UI "\t<DefaultUnitAI>UNITAI_COLONIST</DefaultUnitAI>\n";
	print UI "\t<DefaultProfession>PROFESSION_COLONIST</DefaultProfession>\n";
	print UI "\t<Invisible>NONE</Invisible>\n";
	print UI "\t<SeeInvisible>NONE</SeeInvisible>\n";
	print UI "\t<Description>TXT_KEY_UNIT_".$tag."</Description>\n";
	&maketext("TXT_KEY_UNIT_".$tag, $fulldesc);
	$pedia = 'Construction of the first [COLOR_HIGHLIGHT_TEXT]'.$plural."[COLOR_REVERT] was a vital step in allowing Aliens and Human colonists to progress toward independence from their distant overlords.";
	$pedia = ucfirst($article).' [BOLD]'.$desc.'[\BOLD] is significantly more efficient than other colonists in the production of ';
	foreach $skill (@skills)
		{
		my $skilldesc = $skill;
		$skilldesc =~ tr/_/ /;
		$skilldesc =~ s/(\w+)/\u\L$1/g;
		$pedia = $pedia.'[LINK=YIELD_'.$skill.']'.$skilldesc.'[\LINK], ';
		}
	$pedia = $pedia.'[NEWLINE][PARAGRAPH:1]The oppressive governments of Earth exerted tight control over offworld emigration, and saw it as a means to expel undesirables such as Convicts and Proletarians while retaining and exploiting the dwindling number of citizens with marketable technical and scientific skills. As economic and social conditions in the offworld colonies (and even some Alien civilizations) became more attractive than those on Earth, increasing numbers of remaining skilled and educated workers including '.$plural." strove to circumvent these restrictions to seek their fortune among the stars.";
	print UI "\t<Civilopedia>TXT_KEY_UNIT_".$tag."_PEDIA</Civilopedia>\n";
	&maketext("TXT_KEY_UNIT_".$tag."_PEDIA", $pedia);
	$strategy = 'Build '.A($desc).' to increase our industrial power.';
	print UI "\t<Strategy>TXT_KEY_UNIT_".$tag."_STRATEGY</Strategy>\n";
	&maketext("TXT_KEY_UNIT_".$tag."_STRATEGY", $strategy);
	print UI "\t<bGraphicalOnly>0</bGraphicalOnly>\n";
	print UI "\t<bNoBadGoodies>0</bNoBadGoodies>\n";
	print UI "\t<bOnlyDefensive>0</bOnlyDefensive>\n";
	print UI "\t<bNoCapture>0</bNoCapture>\n";
	print UI "\t<bQuickCombat>0</bQuickCombat>\n";
	print UI "\t<bRivalTerritory>0</bRivalTerritory>\n";
	print UI "\t<bMilitaryProduction>0</bMilitaryProduction>\n";
	print UI "\t<bFound>1</bFound>\n";
	print UI "\t<bInvisible>0</bInvisible>\n";
	print UI "\t<bNoDefensiveBonus>0</bNoDefensiveBonus>\n";
	print UI "\t<bCanMoveImpassable>0</bCanMoveImpassable>\n";
	print UI "\t<bCanMoveAllTerrain>0</bCanMoveAllTerrain>\n";
	print UI "\t<bFlatMovementCost>0</bFlatMovementCost>\n";
	print UI "\t<bIgnoreTerrainCost>0</bIgnoreTerrainCost>\n";
	print UI "\t<bMechanized>0</bMechanized>\n";
	print UI "\t<bLineOfSight>0</bLineOfSight>\n";
	print UI "\t<bHiddenNationality>0</bHiddenNationality>\n";
	print UI "\t<bAlwaysHostile>0</bAlwaysHostile>\n";
	print UI "\t<bTreasure>0</bTreasure>\n";
	print UI "\t<UnitClassUpgrades/>\n";
	print UI "\t<UnitAIs>\n";
	print UI "\t\t<UnitAI>\n";
	print UI "\t\t\t<UnitAIType>UNITAI_COLONIST</UnitAIType>\n";
	print UI "\t\t\t<bUnitAI>1</bUnitAI>\n";
	print UI "\t\t</UnitAI>\n";
	print UI "\t</UnitAIs>\n";
	print UI "\t<NotUnitAIs/>\n";
	print UI "\t<Builds>\n";
	foreach my $build (@builds) {
		$build =~ tr/ /_/;
		$build =~ tr/a-z/A-Z/;
		print UI "\t\t<Build>\n";
		print UI "\t\t\t<BuildType>BUILD_".$build."</BuildType>\n";
		print UI "\t\t\t<bBuild>1</bBuild>\n";
		print UI "\t\t</Build>\n";
		}
	print UI "\t</Builds>\n";
	print UI "\t<PrereqBuilding>NONE</PrereqBuilding>\n";
	print UI "\t<PrereqOrBuildings/>\n";
	print UI "\t<ProductionTraits/>\n";
	print UI "\t<iAIWeight>0</iAIWeight>\n";
	print UI "\t<YieldCosts/>\n";
	print UI "\t<iHurryCostModifier>0</iHurryCostModifier>\n";
	print UI "\t<iAdvancedStartCost>-1</iAdvancedStartCost>\n";
	print UI "\t<iAdvancedStartCostIncrease>0</iAdvancedStartCostIncrease>\n";
	$cost = 1000;
	if ($skills[0] =~ /\w+/) {
		foreach my $skill (@skills) {
			$cost = $cost + 500; }
		}
	print UI "\t<iEuropeCost>".($cost * 2)."</iEuropeCost>\n";
	print UI "\t<iEuropeCostIncrease>".$cost."</iEuropeCostIncrease>\n";
	print UI "\t<iImmigrationWeight>100</iImmigrationWeight>\n";
	print UI "\t<iImmigrationWeightDecay>10</iImmigrationWeightDecay>\n";
	print UI "\t<iMinAreaSize>-1</iMinAreaSize>\n";
	print UI "\t<iMoves>1</iMoves>\n";
	print UI "\t<bCapturesCargo>0</bCapturesCargo>\n";
	print UI "\t<iWorkRate>0</iWorkRate>\n";
	print UI "\t<iWorkRateModifier>0</iWorkRateModifier>\n";
	print UI "\t<iMissionaryRateModifier>0</iMissionaryRateModifier>\n";
	print UI "\t<TerrainImpassables/>\n";
	print UI "\t<FeatureImpassables/>\n";
	print UI "\t<EvasionBuildings/>\n";
	print UI "\t<iCombat>0</iCombat>\n";
	print UI "\t<iXPValueAttack>4</iXPValueAttack>\n";
	print UI "\t<iXPValueDefense>2</iXPValueDefense>\n";
	print UI "\t<iWithdrawalProb>0</iWithdrawalProb>\n";
	print UI "\t<iCityAttack>0</iCityAttack>\n";
	print UI "\t<iCityDefense>0</iCityDefense>\n";
	print UI "\t<iHillsAttack>0</iHillsAttack>\n";
	print UI "\t<iHillsDefense>0</iHillsDefense>\n";
	print UI "\t<TerrainAttacks/>\n";
	print UI "\t<TerrainDefenses/>\n";
	print UI "\t<FeatureAttacks/>\n";
	print UI "\t<FeatureDefenses/>\n";
	print UI "\t<UnitClassAttackMods/>\n";
	print UI "\t<UnitClassDefenseMods/>\n";
	print UI "\t<UnitCombatMods/>\n";
	print UI "\t<DomainMods/>\n";
	if ($skills[0] =~ /\w+/) {
		print UI "\t<YieldModifiers>\n";
		foreach my $skill (@skills)
			{
			print UI "\t\t<YieldModifier>\n";
			print UI "\t\t\t<YieldType>YIELD_".$skill."</YieldType>\n";
			print UI "\t\t\t<iYieldMod>100</iYieldMod>\n";		
			print UI "\t\t</YieldModifier>\n";
			}
		print UI "\t</YieldModifiers>\n";
		}
		else { print UI "\t<YieldModifiers/>\n";}
	print UI "\t<YieldChanges/>\n";
	print UI "\t<BonusYieldChanges/>\n";
	print UI "\t<bLandYieldChanges>1</bLandYieldChanges>\n";
	print UI "\t<bWaterYieldChanges>1</bWaterYieldChanges>\n";
	print UI "\t<iBombardRate>0</iBombardRate>\n";
	print UI "\t<SpecialCargo>NONE</SpecialCargo>\n";
	print UI "\t<DomainCargo>NONE</DomainCargo>\n";
	print UI "\t<iCargo>0</iCargo>\n";
	print UI "\t<iRequiredTransportSize>1</iRequiredTransportSize>\n";
	print UI "\t<iAsset>100</iAsset>\n";
	print UI "\t<iPower>0</iPower>\n";
	print UI "\t<iNativeLearnTime>3</iNativeLearnTime>\n";
	print UI "\t<iStudentWeight>1000</iStudentWeight>\n";
	print UI "\t<iTeacherWeight>0</iTeacherWeight>\n";
	print UI"\t<ProfessionMeshGroups>\n";
	# for profession colonist, use artdef of this unittype
	print UI"\t\t<UnitMeshGroups>\n";
	print UI"\t\t\t<ProfessionType>PROFESSION_COLONIST</ProfessionType>\n";
	print UI"\t\t\t<fMaxSpeed>1.75</fMaxSpeed>\n";
	print UI"\t\t\t<fPadTime>1</fPadTime>\n";
	print UI"\t\t\t<iMeleeWaveSize>4</iMeleeWaveSize>\n";
	print UI"\t\t\t<iRangedWaveSize>0</iRangedWaveSize>\n";
	print UI"\t\t\t<UnitMeshGroup>\n";
	print UI"\t\t\t\t<iRequired>1</iRequired>\n";
	print UI"\t\t\t\t<ArtDefineTag>ART_DEF_UNIT_".$tag."</ArtDefineTag>\n";
	print UI"\t\t\t</UnitMeshGroup>\n";
	print UI"\t\t</UnitMeshGroups>\n";
	# add artdefs for other map professions
	foreach $prof (@walkprofs) {
		my$ptag = $prof;
		$ptag =~ tr/ /_/;
		$ptag =~ tr/a-z/A-Z/;		
		print UI"\t\t<UnitMeshGroups>\n";
		print UI"\t\t\t<ProfessionType>PROFESSION_".$ptag."</ProfessionType>\n";
		print UI"\t\t\t<fMaxSpeed>1.75</fMaxSpeed>\n";
		print UI"\t\t\t<fPadTime>1</fPadTime>\n";
		print UI"\t\t\t<iMeleeWaveSize>4</iMeleeWaveSize>\n";
		print UI"\t\t\t<iRangedWaveSize>0</iRangedWaveSize>\n";
		print UI"\t\t\t<UnitMeshGroup>\n";
		print UI"\t\t\t\t<iRequired>1</iRequired>\n";
		print UI"\t\t\t\t<ArtDefineTag>ART_DEF_UNIT_".$ptag."</ArtDefineTag>\n";
		print UI"\t\t\t</UnitMeshGroup>\n";
		print UI"\t\t</UnitMeshGroups>\n";
		}
	print UI"\t</ProfessionMeshGroups>\n";
	print UI"\t<FormationType>FORMATION_TYPE_DEFAULT</FormationType>\n";
	print UI"\t<HotKey/>\n";
	print UI"\t<bAltDown>0</bAltDown>\n";
	print UI"\t<bShiftDown>0</bShiftDown>\n";
	print UI"\t<bCtrlDown>0</bCtrlDown>\n";
	print UI"\t<iHotKeyPriority>0</iHotKeyPriority>\n";
	print UI"\t<FreePromotions/>\n";
	print UI"\t<LeaderPromotion>NONE</LeaderPromotion>\n";
	print UI"\t<iLeaderExperience>0</iLeaderExperience>\n";
	print UI"</UnitInfo>\n";

	&makeuclass($tag, $fulldesc);

	print ADU "<UnitArtInfo>\n";
	print ADU "\t<Type>ART_DEF_UNIT_".$tag."</Type>\n";
	print ADU "\t<Button>Art/Buttons/Units/".$tag.'.dds</Button>'."\n";
	print ADU "\t<FullLengthIcon>Art/Buttons/Units/Full/".$tag.'.dds</FullLengthIcon>'."\n";
	print ADU "\t<fScale>1.0</fScale>\n";
	print ADU "\t<fInterfaceScale>0.5</fInterfaceScale>\n";
#	print ADU "\t<NIF>Art/Units/".$tag.'/'.$tag.'.nif</NIF>'."\n";
#	print ADU "\t<KFM>Art/Units/".$tag.'/'.$tag.'.kfm</KFM>'."\n";
	&placeland;
	print ADU "\t<TrailDefinition>\n";
	print ADU "\t\t<Texture>Art/Shared/wheeltread.dds</Texture>\n";
	print ADU "\t\t<fWidth>1.2</fWidth>\n";
	print ADU "\t\t<fLength>180.0</fLength>\n";
	print ADU "\t\t<fTaper>0.0</fTaper>\n";
	print ADU "\t\t<fFadeStartTime>0.2</fFadeStartTime>\n";
	print ADU "\t\t<fFadeFalloff>0.35</fFadeFalloff>\n";
	print ADU "\t</TrailDefinition>\n";
	print ADU "\t<fBattleDistance>0</fBattleDistance>\n";
	print ADU "\t<fRangedDeathTime>0</fRangedDeathTime>\n";
	print ADU "\t<bActAsRanged>0</bActAsRanged>\n";
	print ADU "\t<TrainSound>AS2D_UNIT_BUILD_UNIT</TrainSound>\n";
	print ADU "\t<AudioRunSounds>\n";
	print ADU "\t\t<AudioRunTypeLoop></AudioRunTypeLoop>\n";
	print ADU "\t\t<AudioRunTypeEnd></AudioRunTypeEnd>\n";
	print ADU "\t</AudioRunSounds>\n";
	print ADU "</UnitArtInfo>\n";
	}	

# generate XML for cargo units
foreach $item (@cargoyields)
	{
	my $tag = $item;
	$tag =~ tr/ /_/;
	$tag =~ tr/[a-z]/[A-Z]/;
	my $desc = $item;
	print UI "<UnitInfo>\n";	
	print UI "\t<Type>UNIT_".$tag."</Type>\n";
	print UI "\t<Class>UNITCLASS_".$tag."</Class>\n";
	print UI "\t<UniqueNames/>\n";
	print UI "\t<Special>SPECIALUNIT_YIELD_CARGO</Special>\n";
	print UI "\t<Capture>UNITCLASS_".$tag."</Capture>\n";
	print UI "\t<Combat>NONE</Combat>\n";
	print UI "\t<Domain>DOMAIN_LAND</Domain>\n";
	print UI "\t<DefaultUnitAI>UNITAI_YIELD</DefaultUnitAI>\n";
	print UI "\t<DefaultProfession>NONE</DefaultProfession>\n";
	print UI "\t<Invisible>NONE</Invisible>\n";
	print UI "\t<SeeInvisible>NONE</SeeInvisible>\n";
	print UI "\t<Description>TXT_KEY_YIELD_".$tag."</Description>\n";
	print UI "\t<Civilopedia></Civilopedia>\n";
	print UI "\t<Strategy></Strategy>\n";
	print UI "\t<bGraphicalOnly>1</bGraphicalOnly>\n";
	print UI "\t<bNoBadGoodies>0</bNoBadGoodies>\n";
	print UI "\t<bOnlyDefensive>0</bOnlyDefensive>\n";
	print UI "\t<bNoCapture>0</bNoCapture>\n";
	print UI "\t<bQuickCombat>0</bQuickCombat>\n";
	print UI "\t<bRivalTerritory>0</bRivalTerritory>\n";
	print UI "\t<bMilitaryProduction>0</bMilitaryProduction>\n";
	print UI "\t<bFound>0</bFound>\n";
	print UI "\t<bInvisible>0</bInvisible>\n";
	print UI "\t<bNoDefensiveBonus>0</bNoDefensiveBonus>\n";
	print UI "\t<bCanMoveImpassable>0</bCanMoveImpassable>\n";
	print UI "\t<bCanMoveAllTerrain>0</bCanMoveAllTerrain>\n";
	print UI "\t<bFlatMovementCost>0</bFlatMovementCost>\n";
	print UI "\t<bIgnoreTerrainCost>0</bIgnoreTerrainCost>\n";
	print UI "\t<bMechanized>0</bMechanized>\n";
	print UI "\t<bLineOfSight>0</bLineOfSight>\n";
	print UI "\t<bHiddenNationality>0</bHiddenNationality>\n";
	print UI "\t<bAlwaysHostile>0</bAlwaysHostile>\n";
	print UI "\t<bTreasure>0</bTreasure>\n";
	print UI "\t<UnitClassUpgrades/>\n";
	print UI "\t<UnitAIs>\n";
	print UI "\t\t<UnitAI>\n";
	print UI "\t\t\t<UnitAIType>UNITAI_YIELD</UnitAIType>\n";
	print UI "\t\t\t<bUnitAI>1</bUnitAI>\n";
	print UI "\t\t</UnitAI>\n";
	print UI "\t</UnitAIs>\n";
	print UI "\t<NotUnitAIs/>\n";
	print UI "\t<Builds/>\n";
	print UI "\t<PrereqBuilding>NONE</PrereqBuilding>\n";
	print UI "\t<PrereqOrBuildings/>\n";
	print UI "\t<ProductionTraits/>\n";
	print UI "\t<iAIWeight>10</iAIWeight>\n";
	print UI "\t<YieldCosts/>\n";
	print UI "\t<iHurryCostModifier>0</iHurryCostModifier>\n";
	print UI "\t<iAdvancedStartCost>-1</iAdvancedStartCost>\n";
	print UI "\t<iAdvancedStartCostIncrease>0</iAdvancedStartCostIncrease>\n";
	print UI "\t<iEuropeCost>-1</iEuropeCost>\n";
	print UI "\t<iEuropeCostIncrease>0</iEuropeCostIncrease>\n";
	print UI "\t<iImmigrationWeight>0</iImmigrationWeight>\n";
	print UI "\t<iImmigrationWeightDecay>0</iImmigrationWeightDecay>\n";
	print UI "\t<iMinAreaSize>-1</iMinAreaSize>\n";
	print UI "\t<iMoves>0</iMoves>\n";
	print UI "\t<bCapturesCargo>0</bCapturesCargo>\n";
	print UI "\t<iWorkRate>0</iWorkRate>\n";
	print UI "\t<iWorkRateModifier>0</iWorkRateModifier>\n";
	print UI "\t<iMissionaryRateModifier>0</iMissionaryRateModifier>\n";
	print UI "\t<TerrainImpassables/>\n";
	print UI "\t<FeatureImpassables/>\n";
	print UI "\t<EvasionBuildings/>\n";
	print UI "\t<iCombat>0</iCombat>\n";
	print UI "\t<iXPValueAttack>0</iXPValueAttack>\n";
	print UI "\t<iXPValueDefense>0</iXPValueDefense>\n";
	print UI "\t<iWithdrawalProb>0</iWithdrawalProb>\n";
	print UI "\t<iCityAttack>0</iCityAttack>\n";
	print UI "\t<iCityDefense>0</iCityDefense>\n";
	print UI "\t<iHillsAttack>0</iHillsAttack>\n";
	print UI "\t<iHillsDefense>0</iHillsDefense>\n";
	print UI "\t<TerrainAttacks/>\n";
	print UI "\t<TerrainDefenses/>\n";
	print UI "\t<FeatureAttacks/>\n";
	print UI "\t<FeatureDefenses/>\n";
	print UI "\t<UnitClassAttackMods/>\n";
	print UI "\t<UnitClassDefenseMods/>\n";
	print UI "\t<UnitCombatMods/>\n";
	print UI "\t<DomainMods/>\n";
	print UI "\t<YieldModifiers/>\n";
	print UI "\t<YieldChanges/>\n";
	print UI "\t<BonusYieldChanges/>\n";
	print UI "\t<bLandYieldChanges>0</bLandYieldChanges>\n";
	print UI "\t<bWaterYieldChanges>0</bWaterYieldChanges>\n";
	print UI "\t<iBombardRate>0</iBombardRate>\n";
	print UI "\t<SpecialCargo>NONE</SpecialCargo>\n";
	print UI "\t<DomainCargo>NONE</DomainCargo>\n";
	print UI "\t<iCargo>0</iCargo>\n";
	print UI "\t<iRequiredTransportSize>1</iRequiredTransportSize>\n";
	print UI "\t<iAsset>0</iAsset>\n";
	print UI "\t<iPower>0</iPower>\n";
	print UI "\t<iNativeLearnTime>-1</iNativeLearnTime>\n";
	print UI "\t<iStudentWeight>0</iStudentWeight>\n";
	print UI "\t<iTeacherWeight>0</iTeacherWeight>\n";	
	print UI"\t<ProfessionMeshGroups>\n";
	print UI"\t\t<UnitMeshGroups>\n";
	print UI"\t\t\t<ProfessionType>NONE</ProfessionType>\n";
	print UI"\t\t\t<fMaxSpeed>1.25</fMaxSpeed>\n";
	print UI"\t\t\t<fPadTime>1</fPadTime>\n";
	print UI"\t\t\t<iMeleeWaveSize>4</iMeleeWaveSize>\n";
	print UI"\t\t\t<iRangedWaveSize>0</iRangedWaveSize>\n";
	print UI"\t\t\t<UnitMeshGroup>\n";
	print UI"\t\t\t\t<iRequired>1</iRequired>\n";
	print UI"\t\t\t\t<ArtDefineTag>ART_DEF_UNIT_".$tag."</ArtDefineTag>\n";
	print UI"\t\t\t</UnitMeshGroup>\n";
	print UI"\t\t</UnitMeshGroups>\n";
	print UI"\t</ProfessionMeshGroups>\n";
	print UI"\t<FormationType>FORMATION_TYPE_DEFAULT</FormationType>\n";
	print UI"\t<HotKey/>\n";
	print UI"\t<bAltDown>0</bAltDown>\n";
	print UI"\t<bShiftDown>0</bShiftDown>\n";
	print UI"\t<bCtrlDown>0</bCtrlDown>\n";
	print UI"\t<iHotKeyPriority>0</iHotKeyPriority>\n";
	print UI"\t<FreePromotions/>\n";
	print UI"\t<LeaderPromotion>NONE</LeaderPromotion>\n";
	print UI"\t<iLeaderExperience>0</iLeaderExperience>\n";
	print UI"</UnitInfo>\n";

	&makeuclass($tag, $desc);

	print ADU "<UnitArtInfo>\n";
	print ADU "\t<Type>ART_DEF_UNIT_".$tag."</Type>\n";
	print ADU "\t<Button>Art/Buttons/Units/".$tag.'.dds</Button>'."\n";
	print ADU "\t<FullLengthIcon>Art/Buttons/Units/Full/".$tag.'.dds</FullLengthIcon>'."\n";
	print ADU "\t<fScale>1.0</fScale>\n";
	print ADU "\t<fInterfaceScale>0.5</fInterfaceScale>\n";
#	print ADU "\t<NIF>Art/Units/".$tag.'/'.$tag.'.nif</NIF>'."\n";
#	print ADU "\t<KFM>Art/Units/".$tag.'/'.$tag.'.kfm</KFM>'."\n";
	&placeland;
	print ADU "\t<TrailDefinition>\n";
	print ADU "\t\t<Texture>Art/Shared/wheeltread.dds</Texture>\n";
	print ADU "\t\t<fWidth>1.2</fWidth>\n";
	print ADU "\t\t<fLength>180.0</fLength>\n";
	print ADU "\t\t<fTaper>0.0</fTaper>\n";
	print ADU "\t\t<fFadeStartTime>0.2</fFadeStartTime>\n";
	print ADU "\t\t<fFadeFalloff>0.35</fFadeFalloff>\n";
	print ADU "\t</TrailDefinition>\n";
	print ADU "\t<fBattleDistance>0</fBattleDistance>\n";
	print ADU "\t<fRangedDeathTime>0</fRangedDeathTime>\n";
	print ADU "\t<bActAsRanged>0</bActAsRanged>\n";
	print ADU "\t<TrainSound>AS2D_UNIT_BUILD_UNIT</TrainSound>\n";
	print ADU "\t<AudioRunSounds>\n";
	print ADU "\t\t<AudioRunTypeLoop></AudioRunTypeLoop>\n";
	print ADU "\t\t<AudioRunTypeEnd></AudioRunTypeEnd>\n";
	print ADU "\t</AudioRunSounds>\n";
	print ADU "</UnitArtInfo>\n";
	}

# generate generic XML for misc non-colonist units (uses defaults for ships, must edit content manually)
foreach $item (@miscunits)
	{
	my $tag = $item;
	$tag =~ tr/ /_/;
	$tag =~ tr/[a-z]/[A-Z]/;
	my $desc = $item;
	my $plural = PL_N($desc);
	my $fulldesc = $desc.':'.$plural;
	print UI "<UnitInfo>\n";	
	print UI "\t<Type>UNIT_".$tag."</Type>\n";
	print UI "\t<Class>UNITCLASS_".$tag."</Class>\n";
	print UI "\t<UniqueNames/>\n";
	print UI "\t<Special>NONE</Special>\n";
	print UI "\t<Capture>NONE</Capture>\n";
	print UI "\t<Combat>NONE</Combat>\n";
	print UI "\t<Domain>DOMAIN_SEA</Domain>\n";
	print UI "\t<DefaultUnitAI>UNITAI_TRANSPORT_SEA</DefaultUnitAI>\n";
	print UI "\t<DefaultProfession>NONE</DefaultProfession>\n";
	print UI "\t<Invisible>NONE</Invisible>\n";
	print UI "\t<SeeInvisible>NONE</SeeInvisible>\n";
	print UI "\t<Description>TXT_KEY_UNIT_".$tag."</Description>\n";
	&maketext("TXT_KEY_UNIT_".$tag, $fulldesc);
	print UI "\t<Civilopedia>TXT_KEY_UNIT_".$tag."_PEDIA</Civilopedia>\n";
	$pedia = 'Construction of the first [COLOR_HIGHLIGHT_TEXT]'.$plural."[COLOR_REVERT] was a vital step in allowing Aliens and Human colonists to progress toward independence from their distant overlords.";
	&maketext("TXT_KEY_UNIT_".$tag."_PEDIA", $pedia);
	print UI "\t<Strategy>TXT_KEY_UNIT_".$tag."_STRATEGY</Strategy>\n";
	$strategy = 'Build '.A($desc).' to bolster our military might.';
	&maketext("TXT_KEY_UNIT_".$tag."_STRATEGY", $strategy);
	print UI "\t<bGraphicalOnly>0</bGraphicalOnly>\n";
	print UI "\t<bNoBadGoodies>0</bNoBadGoodies>\n";
	print UI "\t<bOnlyDefensive>0</bOnlyDefensive>\n";
	print UI "\t<bNoCapture>0</bNoCapture>\n";
	print UI "\t<bQuickCombat>0</bQuickCombat>\n";
	print UI "\t<bRivalTerritory>0</bRivalTerritory>\n";
	print UI "\t<bMilitaryProduction>1</bMilitaryProduction>\n";
	print UI "\t<bFound>0</bFound>\n";
	print UI "\t<bInvisible>0</bInvisible>\n";
	print UI "\t<bNoDefensiveBonus>0</bNoDefensiveBonus>\n";
	print UI "\t<bCanMoveImpassable>0</bCanMoveImpassable>\n";
	print UI "\t<bCanMoveAllTerrain>0</bCanMoveAllTerrain>\n";
	print UI "\t<bFlatMovementCost>0</bFlatMovementCost>\n";
	print UI "\t<bIgnoreTerrainCost>0</bIgnoreTerrainCost>\n";
	print UI "\t<bMechanized>1</bMechanized>\n";
	print UI "\t<bLineOfSight>0</bLineOfSight>\n";
	print UI "\t<bHiddenNationality>0</bHiddenNationality>\n";
	print UI "\t<bAlwaysHostile>0</bAlwaysHostile>\n";
	print UI "\t<bTreasure>0</bTreasure>\n";
	print UI "\t<UnitClassUpgrades/>\n";
	print UI "\t<UnitAIs>\n";
	print UI "\t\t<UnitAI>\n";
	print UI "\t\t\t<UnitAIType>UNITAI_TRANSPORT_SEA</UnitAIType>\n";
	print UI "\t\t\t<bUnitAI>1</bUnitAI>\n";
	print UI "\t\t</UnitAI>\n";
	print UI "\t</UnitAIs>\n";
	print UI "\t<NotUnitAIs/>\n";
	print UI "\t<Builds/>\n";
	print UI "\t<PrereqBuilding>NONE</PrereqBuilding>\n";
	print UI "\t<PrereqOrBuildings>\n";
	print UI "\t\t<PrereqOrBuilding>\n";
	print UI "\t\t\t<BuildingClass>BUILDINGCLASS_DOCK2</BuildingClass>\n";
	print UI "\t\t\t<bPrereq>1</bPrereq>\n";
	print UI "\t\t</PrereqOrBuilding>\n";
	print UI "\t\t<PrereqOrBuilding>\n";
	print UI "\t\t\t<BuildingClass>BUILDINGCLASS_DOCK3</BuildingClass>\n";
	print UI "\t\t\t<bPrereq>1</bPrereq>\n";
	print UI "\t\t</PrereqOrBuilding>\n";
	print UI "\t</PrereqOrBuildings>\n";
	print UI "\t<ProductionTraits/>\n";
	print UI "\t<iAIWeight>0</iAIWeight>\n";
	print UI "\t<YieldCosts>\n";
	print UI "\t\t<YieldCost>\n";
	print UI "\t\t\t<YieldType>YIELD_INDUSTRY</YieldType>\n";
	print UI "\t\t\t<iCost>100</iCost>\n";
	print UI "\t\t</YieldCost>\n";
	print UI "\t</YieldCosts>\n";
	print UI "\t<iHurryCostModifier>0</iHurryCostModifier>\n";
	print UI "\t<iAdvancedStartCost>2000</iAdvancedStartCost>\n";
	print UI "\t<iAdvancedStartCostIncrease>500</iAdvancedStartCostIncrease>\n";
	print UI "\t<iEuropeCost>2000</iEuropeCost>\n";
	print UI "\t<iEuropeCostIncrease>500</iEuropeCostIncrease>\n";
	print UI "\t<iImmigrationWeight>0</iImmigrationWeight>\n";
	print UI "\t<iImmigrationWeightDecay>0</iImmigrationWeightDecay>\n";
	print UI "\t<iMinAreaSize>20</iMinAreaSize>\n";
	print UI "\t<iMoves>3</iMoves>\n";
	print UI "\t<bCapturesCargo>0</bCapturesCargo>\n";
	print UI "\t<iWorkRate>0</iWorkRate>\n";
	print UI "\t<iWorkRateModifier>0</iWorkRateModifier>\n";
	print UI "\t<iMissionaryRateModifier>0</iMissionaryRateModifier>\n";
	print UI "\t<TerrainImpassables/>\n";
	print UI "\t<FeatureImpassables/>\n";
	print UI "\t<EvasionBuildings>\n";
	print UI "\t\t<EvasionBuilding>\n";
	print UI "\t\t\t<BuildingClass>BUILDINGCLASS_DOCK1</BuildingClass>\n";
	print UI "\t\t\t<bEvasion>1</bEvasion>\n";
	print UI "\t\t</EvasionBuilding>\n";
	print UI "\t</EvasionBuildings>\n";
	print UI "\t<iCombat>3</iCombat>\n";
	print UI "\t<iXPValueAttack>4</iXPValueAttack>\n";
	print UI "\t<iXPValueDefense>2</iXPValueDefense>\n";
	print UI "\t<iWithdrawalProb>10</iWithdrawalProb>\n";
	print UI "\t<iCityAttack>0</iCityAttack>\n";
	print UI "\t<iCityDefense>0</iCityDefense>\n";
	print UI "\t<iHillsAttack>0</iHillsAttack>\n";
	print UI "\t<iHillsDefense>0</iHillsDefense>\n";
	print UI "\t<TerrainAttacks/>\n";
	print UI "\t<TerrainDefenses/>\n";
	print UI "\t<FeatureAttacks/>\n";
	print UI "\t<FeatureDefenses/>\n";
	print UI "\t<UnitClassAttackMods/>\n";
	print UI "\t<UnitClassDefenseMods/>\n";
	print UI "\t<UnitCombatMods/>\n";
	print UI "\t<DomainMods/>\n";
	print UI "\t<YieldModifiers/>\n";
	print UI "\t<YieldChanges/>\n";
	print UI "\t<BonusYieldChanges/>\n";
	print UI "\t<bLandYieldChanges>1</bLandYieldChanges>\n";
	print UI "\t<bWaterYieldChanges>1</bWaterYieldChanges>\n";
	print UI "\t<iBombardRate>0</iBombardRate>\n";
	print UI "\t<SpecialCargo>NONE</SpecialCargo>\n";
	print UI "\t<DomainCargo>DOMAIN_LAND</DomainCargo>\n";
	print UI "\t<iCargo>3</iCargo>\n";
	print UI "\t<iRequiredTransportSize>1</iRequiredTransportSize>\n";
	print UI "\t<iAsset>200</iAsset>\n";
	print UI "\t<iPower>50</iPower>\n";
	print UI "\t<iNativeLearnTime>-1</iNativeLearnTime>\n";
	print UI "\t<iStudentWeight>0</iStudentWeight>\n";
	print UI "\t<iTeacherWeight>0</iTeacherWeight>\n";	
	print UI"\t<ProfessionMeshGroups>\n";
	print UI"\t\t<UnitMeshGroups>\n";
	print UI"\t\t\t<ProfessionType>NONE</ProfessionType>\n";
	print UI"\t\t\t<fMaxSpeed>1.25</fMaxSpeed>\n";
	print UI"\t\t\t<fPadTime>1</fPadTime>\n";
	print UI"\t\t\t<iMeleeWaveSize>4</iMeleeWaveSize>\n";
	print UI"\t\t\t<iRangedWaveSize>0</iRangedWaveSize>\n";
	print UI"\t\t\t<UnitMeshGroup>\n";
	print UI"\t\t\t\t<iRequired>1</iRequired>\n";
	print UI"\t\t\t\t<ArtDefineTag>ART_DEF_UNIT_".$tag."</ArtDefineTag>\n";
	print UI"\t\t\t</UnitMeshGroup>\n";
	print UI"\t\t</UnitMeshGroups>\n";
	print UI"\t</ProfessionMeshGroups>\n";
	print UI"\t<FormationType>FORMATION_TYPE_MACHINE</FormationType>\n";
	print UI"\t<HotKey/>\n";
	print UI"\t<bAltDown>0</bAltDown>\n";
	print UI"\t<bShiftDown>0</bShiftDown>\n";
	print UI"\t<bCtrlDown>0</bCtrlDown>\n";
	print UI"\t<iHotKeyPriority>0</iHotKeyPriority>\n";
	print UI"\t<FreePromotions/>\n";
	print UI"\t<LeaderPromotion>NONE</LeaderPromotion>\n";
	print UI"\t<iLeaderExperience>0</iLeaderExperience>\n";
	print UI"</UnitInfo>\n";

	&makeuclass($tag, $fulldesc);

	print ADU "<UnitArtInfo>\n";
	print ADU "\t<Type>ART_DEF_UNIT_".$tag."</Type>\n";
	print ADU "\t<Button>Art/Buttons/Units/".$tag.'.dds</Button>'."\n";
	print ADU "\t<FullLengthIcon>Art/Buttons/Units/Full/".$tag.'.dds</FullLengthIcon>'."\n";
	print ADU "\t<fScale>0.24</fScale>\n";
	print ADU "\t<fInterfaceScale>0.7</fInterfaceScale>\n";
#	print ADU "\t<NIF>Art/Units/".$tag.'/'.$tag.'.nif</NIF>'."\n";
#	print ADU "\t<KFM>Art/Units/".$tag.'/'.$tag.'.kfm</KFM>'."\n";
	&placeship;
	print ADU "\t<TrailDefinition>\n";
	print ADU "\t\t<Texture>Art/Shared/wheeltread.dds</Texture>\n";
	print ADU "\t\t<fWidth>1.2</fWidth>\n";
	print ADU "\t\t<fLength>180.0</fLength>\n";
	print ADU "\t\t<fTaper>0.0</fTaper>\n";
	print ADU "\t\t<fFadeStartTime>0.2</fFadeStartTime>\n";
	print ADU "\t\t<fFadeFalloff>0.35</fFadeFalloff>\n";
	print ADU "\t</TrailDefinition>\n";
	print ADU "\t<fBattleDistance>0</fBattleDistance>\n";
	print ADU "\t<fRangedDeathTime>0</fRangedDeathTime>\n";
	print ADU "\t<bActAsRanged>0</bActAsRanged>\n";
	print ADU "\t<TrainSound>AS2D_UNIT_BUILD_UNIT</TrainSound>\n";
	print ADU "\t<AudioRunSounds>\n";
	print ADU "\t\t<AudioRunTypeLoop></AudioRunTypeLoop>\n";
	print ADU "\t\t<AudioRunTypeEnd></AudioRunTypeEnd>\n";
	print ADU "\t</AudioRunSounds>\n";
	print ADU "</UnitArtInfo>\n";	
	}
# generate XML for beast units
foreach $item (@beastunits)
	{
	my $tag = $item;
	$tag =~ tr/ /_/;
	$tag =~ tr/[a-z]/[A-Z]/;
	my $desc = $item;
	my $plural = PL_N($desc);
	my $fulldesc = $desc.':'.$plural;
	print UI "<UnitInfo>\n";	
	print UI "\t<Type>UNIT_".$tag."</Type>\n";
	print UI "\t<Class>UNITCLASS_".$tag."</Class>\n";
	print UI "\t<UniqueNames/>\n";
	print UI "\t<Special>NONE</Special>\n";
	print UI "\t<Capture>NONE</Capture>\n";
	print UI "\t<Combat>NONE</Combat>\n";
	print UI "\t<Domain>DOMAIN_LAND</Domain>\n";
	print UI "\t<DefaultUnitAI>UNITAI_OFFENSIVE</DefaultUnitAI>\n";
	print UI "\t<DefaultProfession>NONE</DefaultProfession>\n";
	print UI "\t<Invisible>NONE</Invisible>\n";
	print UI "\t<SeeInvisible>NONE</SeeInvisible>\n";
	print UI "\t<Description>TXT_KEY_UNIT_".$tag."</Description>\n";
	&maketext("TXT_KEY_UNIT_".$tag, $fulldesc);
	print UI "\t<Civilopedia>TXT_KEY_UNIT_".$tag."_PEDIA</Civilopedia>\n";
	$pedia = 'Survival in the already harsh environment of the New Worlds was made even more difficult by frequenty incursions from endemic creatures such as [COLOR_HIGHLIGHT_TEXT]'.$plural."[COLOR_REVERT].";
	&maketext("TXT_KEY_UNIT_".$tag."_PEDIA", $pedia);
	print UI "\t<Strategy>TXT_KEY_UNIT_".$tag."_STRATEGY</Strategy>\n";
	$strategy = 'Build '.A($desc).' to bolster our military might.';
	&maketext("TXT_KEY_UNIT_".$tag."_STRATEGY", $strategy);
	print UI "\t<bGraphicalOnly>0</bGraphicalOnly>\n";
	print UI "\t<bNoBadGoodies>0</bNoBadGoodies>\n";
	print UI "\t<bOnlyDefensive>0</bOnlyDefensive>\n";
	print UI "\t<bNoCapture>0</bNoCapture>\n";
	print UI "\t<bQuickCombat>0</bQuickCombat>\n";
	print UI "\t<bRivalTerritory>0</bRivalTerritory>\n";
	print UI "\t<bMilitaryProduction>1</bMilitaryProduction>\n";
	print UI "\t<bFound>0</bFound>\n";
	print UI "\t<bInvisible>0</bInvisible>\n";
	print UI "\t<bNoDefensiveBonus>0</bNoDefensiveBonus>\n";
	print UI "\t<bCanMoveImpassable>0</bCanMoveImpassable>\n";
	print UI "\t<bCanMoveAllTerrain>0</bCanMoveAllTerrain>\n";
	print UI "\t<bFlatMovementCost>0</bFlatMovementCost>\n";
	print UI "\t<bIgnoreTerrainCost>0</bIgnoreTerrainCost>\n";
	print UI "\t<bMechanized>0</bMechanized>\n";
	print UI "\t<bLineOfSight>0</bLineOfSight>\n";
	print UI "\t<bHiddenNationality>0</bHiddenNationality>\n";
	print UI "\t<bAlwaysHostile>1</bAlwaysHostile>\n";
	print UI "\t<bTreasure>0</bTreasure>\n";
	print UI "\t<UnitClassUpgrades/>\n";
	print UI "\t<UnitAIs>\n";
	print UI "\t\t<UnitAI>\n";
	print UI "\t\t\t<UnitAIType>UNITAI_OFFENSIVE</UnitAIType>\n";
	print UI "\t\t\t<bUnitAI>1</bUnitAI>\n";
	print UI "\t\t</UnitAI>\n";
	print UI "\t</UnitAIs>\n";
	print UI "\t<NotUnitAIs/>\n";
	print UI "\t<Builds/>\n";
	print UI "\t<PrereqBuilding>NONE</PrereqBuilding>\n";
	print UI "\t<PrereqOrBuildings/>\n";
	print UI "\t<ProductionTraits/>\n";
	print UI "\t<iAIWeight>0</iAIWeight>\n";
	print UI "\t<YieldCosts/>\n";
	print UI "\t<iHurryCostModifier>0</iHurryCostModifier>\n";
	print UI "\t<iAdvancedStartCost>-1</iAdvancedStartCost>\n";
	print UI "\t<iAdvancedStartCostIncrease>0</iAdvancedStartCostIncrease>\n";
	print UI "\t<iEuropeCost>-1</iEuropeCost>\n";
	print UI "\t<iEuropeCostIncrease>-1</iEuropeCostIncrease>\n";
	print UI "\t<iImmigrationWeight>0</iImmigrationWeight>\n";
	print UI "\t<iImmigrationWeightDecay>0</iImmigrationWeightDecay>\n";
	print UI "\t<iMinAreaSize>-1</iMinAreaSize>\n";
	print UI "\t<iMoves>2</iMoves>\n";
	print UI "\t<bCapturesCargo>0</bCapturesCargo>\n";
	print UI "\t<iWorkRate>0</iWorkRate>\n";
	print UI "\t<iWorkRateModifier>0</iWorkRateModifier>\n";
	print UI "\t<iMissionaryRateModifier>0</iMissionaryRateModifier>\n";
	print UI "\t<TerrainImpassables/>\n";
	print UI "\t<FeatureImpassables/>\n";
	print UI "\t<EvasionBuildings/>\n";
	print UI "\t<iCombat>3</iCombat>\n";
	print UI "\t<iXPValueAttack>5</iXPValueAttack>\n";
	print UI "\t<iXPValueDefense>5</iXPValueDefense>\n";
	print UI "\t<iWithdrawalProb>10</iWithdrawalProb>\n";
	print UI "\t<iCityAttack>0</iCityAttack>\n";
	print UI "\t<iCityDefense>0</iCityDefense>\n";
	print UI "\t<iHillsAttack>0</iHillsAttack>\n";
	print UI "\t<iHillsDefense>0</iHillsDefense>\n";
	print UI "\t<TerrainAttacks/>\n";
	print UI "\t<TerrainDefenses/>\n";
	print UI "\t<FeatureAttacks/>\n";
	print UI "\t<FeatureDefenses/>\n";
	print UI "\t<UnitClassAttackMods/>\n";
	print UI "\t<UnitClassDefenseMods/>\n";
	print UI "\t<UnitCombatMods/>\n";
	print UI "\t<DomainMods/>\n";
	print UI "\t<YieldModifiers/>\n";
	print UI "\t<YieldChanges/>\n";
	print UI "\t<BonusYieldChanges/>\n";
	print UI "\t<bLandYieldChanges>0</bLandYieldChanges>\n";
	print UI "\t<bWaterYieldChanges>0</bWaterYieldChanges>\n";
	print UI "\t<iBombardRate>0</iBombardRate>\n";
	print UI "\t<SpecialCargo>NONE</SpecialCargo>\n";
	print UI "\t<DomainCargo>NONE</DomainCargo>\n";
	print UI "\t<iCargo>0</iCargo>\n";
	print UI "\t<iRequiredTransportSize>1</iRequiredTransportSize>\n";
	print UI "\t<iAsset>0</iAsset>\n";
	print UI "\t<iPower>0</iPower>\n";
	print UI "\t<iNativeLearnTime>-1</iNativeLearnTime>\n";
	print UI "\t<iStudentWeight>0</iStudentWeight>\n";
	print UI "\t<iTeacherWeight>0</iTeacherWeight>\n";	
	print UI"\t<ProfessionMeshGroups>\n";
	print UI"\t\t<UnitMeshGroups>\n";
	print UI"\t\t\t<ProfessionType>NONE</ProfessionType>\n";
	print UI"\t\t\t<fMaxSpeed>1.25</fMaxSpeed>\n";
	print UI"\t\t\t<fPadTime>1</fPadTime>\n";
	print UI"\t\t\t<iMeleeWaveSize>4</iMeleeWaveSize>\n";
	print UI"\t\t\t<iRangedWaveSize>0</iRangedWaveSize>\n";
	print UI"\t\t\t<UnitMeshGroup>\n";
	print UI"\t\t\t\t<iRequired>1</iRequired>\n";
	print UI"\t\t\t\t<ArtDefineTag>ART_DEF_UNIT_".$tag."</ArtDefineTag>\n";
	print UI"\t\t\t</UnitMeshGroup>\n";
	print UI"\t\t</UnitMeshGroups>\n";
	print UI"\t</ProfessionMeshGroups>\n";
	print UI"\t<FormationType>FORMATION_TYPE_DEFAULT</FormationType>\n";
	print UI"\t<HotKey/>\n";
	print UI"\t<bAltDown>0</bAltDown>\n";
	print UI"\t<bShiftDown>0</bShiftDown>\n";
	print UI"\t<bCtrlDown>0</bCtrlDown>\n";
	print UI"\t<iHotKeyPriority>0</iHotKeyPriority>\n";
	print UI"\t<FreePromotions/>\n";
	print UI"\t<LeaderPromotion>NONE</LeaderPromotion>\n";
	print UI"\t<iLeaderExperience>0</iLeaderExperience>\n";
	print UI"</UnitInfo>\n";

	&makeuclass($tag, $fulldesc);

	print ADU "<UnitArtInfo>\n";
	print ADU "\t<Type>ART_DEF_UNIT_".$tag."</Type>\n";
	print ADU "\t<Button>Art/Buttons/Units/".$tag.'.dds</Button>'."\n";
	print ADU "\t<FullLengthIcon>Art/Buttons/Units/Full/".$tag.'.dds</FullLengthIcon>'."\n";
	print ADU "\t<fScale>1.0</fScale>\n";
	print ADU "\t<fInterfaceScale>0.5</fInterfaceScale>\n";
#	print ADU "\t<NIF>Art/Units/".$tag.'/'.$tag.'.nif</NIF>'."\n";
#	print ADU "\t<KFM>Art/Units/".$tag.'/'.$tag.'.kfm</KFM>'."\n";
	&placeland;
	print ADU "\t<TrailDefinition>\n";
	print ADU "\t\t<Texture>Art/Shared/wheeltread.dds</Texture>\n";
	print ADU "\t\t<fWidth>1.2</fWidth>\n";
	print ADU "\t\t<fLength>180.0</fLength>\n";
	print ADU "\t\t<fTaper>0.0</fTaper>\n";
	print ADU "\t\t<fFadeStartTime>0.2</fFadeStartTime>\n";
	print ADU "\t\t<fFadeFalloff>0.35</fFadeFalloff>\n";
	print ADU "\t</TrailDefinition>\n";
	print ADU "\t<fBattleDistance>0</fBattleDistance>\n";
	print ADU "\t<fRangedDeathTime>0</fRangedDeathTime>\n";
	print ADU "\t<bActAsRanged>0</bActAsRanged>\n";
	print ADU "\t<TrainSound>AS2D_UNIT_BUILD_UNIT</TrainSound>\n";
	print ADU "\t<AudioRunSounds>\n";
	print ADU "\t\t<AudioRunTypeLoop></AudioRunTypeLoop>\n";
	print ADU "\t\t<AudioRunTypeEnd></AudioRunTypeEnd>\n";
	print ADU "\t</AudioRunSounds>\n";
	print ADU "</UnitArtInfo>\n";	
	}

# make unit artdefs for professions that walk on the map
foreach $item (@walkprofs)
	{
	$tag = $item;
	$tag =~ tr/ /_/;
	$tag =~ tr/[a-z]/[A-Z]/;
	next if $tag =~ /COLONIST/;
	print ADU "<UnitArtInfo>\n";
	print ADU "\t<Type>ART_DEF_UNIT_".$tag."</Type>\n";
	print ADU "\t<Button>Art/Buttons/Units/".$tag.'.dds</Button>'."\n";
	print ADU "\t<FullLengthIcon>Art/Buttons/Units/Full/".$tag.'.dds</FullLengthIcon>'."\n";
	print ADU "\t<fScale>1.0</fScale>\n";
	print ADU "\t<fInterfaceScale>0.5</fInterfaceScale>\n";
#	print ADU "\t<NIF>Art/Units/".$tag.'/'.$tag.'.nif</NIF>'."\n";
#	print ADU "\t<KFM>Art/Units/".$tag.'/'.$tag.'.kfm</KFM>'."\n";
	&placeland;
	print ADU "\t<TrailDefinition>\n";
	print ADU "\t\t<Texture>Art/Shared/wheeltread.dds</Texture>\n";
	print ADU "\t\t<fWidth>1.2</fWidth>\n";
	print ADU "\t\t<fLength>180.0</fLength>\n";
	print ADU "\t\t<fTaper>0.0</fTaper>\n";
	print ADU "\t\t<fFadeStartTime>0.2</fFadeStartTime>\n";
	print ADU "\t\t<fFadeFalloff>0.35</fFadeFalloff>\n";
	print ADU "\t</TrailDefinition>\n";
	print ADU "\t<fBattleDistance>0</fBattleDistance>\n";
	print ADU "\t<fRangedDeathTime>0</fRangedDeathTime>\n";
	print ADU "\t<bActAsRanged>0</bActAsRanged>\n";
	print ADU "\t<TrainSound>AS2D_UNIT_BUILD_UNIT</TrainSound>\n";
	print ADU "\t<AudioRunSounds>\n";
	print ADU "\t\t<AudioRunTypeLoop></AudioRunTypeLoop>\n";
	print ADU "\t\t<AudioRunTypeEnd></AudioRunTypeEnd>\n";
	print ADU "\t</AudioRunSounds>\n";
	print ADU "</UnitArtInfo>\n";
	}
	
# closing tags
print UI '</UnitInfos>'."\n</Civ4UnitInfos>\n";
close UI;
print UCI '</UnitClassInfos>'."\n</Civ4UnitClassInfos>\n";
close UCI;
print ADU '</UnitArtInfos>'."\n</Civ4ArtDefines>\n";
close ADU;

# ** CIVILIZATIONS / LEADERHEADS **

# open XML for writing
open (CI, '> ../Assets/XML/Civilizations/CIV4CivilizationInfos.xml') or die "Can't write output: $!";
#open (LI, '> ../Assets/XML/Civilizations/CIV4LeaderheadInfos.xml') or die "Can't write output: $!";
open (ADC, '> ../Assets/XML/Art/CIV4ArtDefines_Civilization.xml') or die "Can't write output: $!";

# generate XML headers
print CI '<?xml version="1.0"?>'."\n";
print CI '<!-- edited with XMLSPY v2004 rel. 2 U (http://www.xmlspy.com) by EXTREME (Firaxis Games) -->'."\n";
print CI '<!-- Sid Meier\'s Civilization 4 -->'."\n".'<!-- Copyright Firaxis Games 2005 -->'."\n".'<!-- -->'."\n".'<!-- Civilization Infos -->'."\n";
print CI '<Civ4CivilizationInfos xmlns="x-schema:CIV4CivilizationsSchema.xml">'."\n<CivilizationInfos>\n";

#print LI '<?xml version="1.0"?>'."\n";
#print LI '<!-- edited with XMLSPY v2004 rel. 2 U (http://www.xmlspy.com) by EXTREME (Firaxis Games) -->'."\n";
#print LI '<!-- Sid Meier\'s Civilization 4 -->'."\n".'<!-- Copyright Firaxis Games 2005 -->'."\n".'<!-- -->'."\n".'<!-- Leader Infos and AI Settings -->'."\n";
#print LI '<Civ4LeaderHeadInfos xmlns="x-schema:CIV4CivilizationsSchema.xml">'."\n<LeaderHeadInfos>\n";

print ADC '<?xml version="1.0" encoding="UTF-8" standalone="no"?>'."\n";
print ADC '<!-- edited with XMLSPY v2004 rel. 2 U (http://www.xmlspy.com) by Alex Mantzaris (Firaxis Games) -->'."\n";
print ADC '<!-- Sid Meier\'s Civilization 4 -->'."\n".'<!-- Copyright Firaxis Games 2005 -->'."\n".'<!-- -->'."\n".'<!-- Unit art path information -->'."\n";
print ADC '<Civ4ArtDefines xmlns="x-schema:CIV4ArtDefinesSchema.xml">'."\n<CivilizationArtInfos>\n";

# generate civ and LH XML

sub makekingciv
{
	my $desc = shift;
	my $tag1 = $desc;
	$tag1 =~ tr/ /_/;
	$tag1 =~ tr/[a-z]/[A-Z]/;
	$tag = $tag1."_KING";
	print CI "<CivilizationInfo>\n";
	print CI "\t<Type>CIVILIZATION_".$tag."</Type>\n";
	print CI "\t<Description>TXT_KEY_CIVILIZATION_".$tag."</Description>\n";
	&maketext("TXT_KEY_CIVILIZATION_".$tag, $desc." Government");
	print CI "\t<ShortDescription>TXT_KEY_CIVILIZATION_".$tag."_SHORT</ShortDescription>\n";
	&maketext("TXT_KEY_CIVILIZATION_".$tag."_SHORT", $desc);
	print CI "\t<Adjective>TXT_KEY_CIVILIZATION_".$tag."_ADJ</Adjective>\n";
	&maketext("TXT_KEY_CIVILIZATION_".$tag."_ADJ", $desc);
	my $pedia = '[COLOR_HIGHLIGHT_TEXT]'.$desc." Government[COLOR_REVERT].";
	print CI "\t<Civilopedia>TXT_KEY_CIVILIZATION_".$tag."_PEDIA</Civilopedia>\n";
	&maketext("TXT_KEY_CIVILIZATION_".$tag."_PEDIA", $desc);
	print CI "\t<DefaultPlayerColor>PLAYERCOLOR_WHITE</DefaultPlayerColor>\n";
	print CI "\t<ArtDefineTag>ART_DEF_CIVILIZATION_".$tag."</ArtDefineTag>\n";
	print CI "\t<ArtStyleType>ARTSTYLE_EUROPEAN</ArtStyleType>\n";
	print CI "\t<UnitArtStyleType>UNIT_ARTSTYLE_ANGLO</UnitArtStyleType>\n";
	print CI "\t<bPlayable>0</bPlayable>\n";
	print CI "\t<bAIPlayable>0</bAIPlayable>\n";
	print CI "\t<bWaterStart>0</bWaterStart>\n";
	print CI "\t<bOpenBorders>0</bOpenBorders>\n";
	print CI "\t<bWaterWorks>1</bWaterWorks>\n";
	print CI "\t<bEurope>1</bEurope>\n";
	print CI "\t<bNative>0</bNative>\n";
	print CI "\t<iAdvancedStartPoints>0</iAdvancedStartPoints>\n";
	print CI "\t<iAreaMultiplier>100</iAreaMultiplier>\n";
	print CI "\t<iDensityMultiplier>100</iDensityMultiplier>\n";
	print CI "\t<iTreasure>0</iTreasure>\n";
	print CI "\t<FavoredTerrain>NONE</FavoredTerrain>\n";
	print CI "\t<DefaultProfession>PROFESSION_COLONIST</DefaultProfession>\n";
	print CI "\t<Cities>\n";
	print CI "\t\t<City>City</City>\n";
	print CI "\t</Cities>\n";
	print CI "\t<Buildings/>\n";
	print CI "\t<Units>\n";
	print CI "\t\t<Unit>\n";
	print CI "\t\t\t<UnitClassType>UNITCLASS_KILLBOTS</UnitClassType>\n";
	print CI "\t\t\t<UnitType>NONE</UnitType>\n";
	print CI "\t\t</Unit>\n";
	print CI "\t</Units>\n";
	print CI "\t<Professions>\n";
	print CI "\t\t<Profession>\n";
	print CI "\t\t\t<ProfessionType>PROFESSION_TERRORIST</ProfessionType>\n";
	print CI "\t\t\t<bValid>0</bValid>\n";
	print CI "\t\t</Profession>\n";
	print CI "\t\t<Profession>\n";
	print CI "\t\t\t<ProfessionType>PROFESSION_PIRATE</ProfessionType>\n";
	print CI "\t\t\t<bValid>0</bValid>\n";
	print CI "\t\t</Profession>\n";
	print CI "\t</Professions>\n";
	print CI "\t<Traits>\n";
	print CI "\t\t<Trait>\n";
	print CI "\t\t\t<TraitType>TRAIT_RESEARCH_ERA_1_MOD</TraitType>\n";
	print CI "\t\t\t<bTrait>0</bTrait>\n";
	print CI "\t\t</Trait>\n";
	print CI "\t</Traits>\n";
	print CI "\t<FreeUnitClasses>\n";
	print CI "\t\t<FreeUnitClass>\n";
	print CI "\t\t\t<UnitClassType>UNITCLASS_COLONIST</UnitClassType>\n";
	print CI "\t\t\t<FreeUnitProfession>PROFESSION_COLONIST</FreeUnitProfession>\n";
	print CI "\t\t</FreeUnitClass>\n";
	print CI "\t\t<FreeUnitClass>\n";
	print CI "\t\t\t<UnitClassType>UNITCLASS_COLONIST</UnitClassType>\n";
	print CI "\t\t\t<FreeUnitProfession>PROFESSION_COLONIST</FreeUnitProfession>\n";
	print CI "\t\t</FreeUnitClass>\n";
	print CI "\t</FreeUnitClasses>\n";
	print CI "\t<FreeBuildingClasses>\n";
	print CI "\t\t<FreeBuildingClass>\n";
	print CI "\t\t\t<BuildingClassType>BUILDINGCLASS_INDUSTRY1</BuildingClassType>\n";
	print CI "\t\t\t<bFreeBuildingClass>1</bFreeBuildingClass>\n";
	print CI "\t\t</FreeBuildingClass>\n";
	print CI "\t</FreeBuildingClasses>\n";
	print CI "\t<TeachUnitClasses/>\n";
	print CI "\t<FreeYields>\n";
	print CI "\t\t<FreeYield>\n";
	print CI "\t\t\t<YieldType>YIELD_NUTRIENTS</YieldType>\n";
	print CI "\t\t\t<iYield>1</iYield>\n";
	print CI "\t\t</FreeYield>\n";
	print CI "\t\t<FreeYield>\n";
	print CI "\t\t\t<YieldType>YIELD_INDUSTRY</YieldType>\n";
	print CI "\t\t\t<iYield>1</iYield>\n";
	print CI "\t\t</FreeYield>\n";
	print CI "\t\t<FreeYield>\n";
	print CI "\t\t\t<YieldType>YIELD_MUNITIONS</YieldType>\n";
	print CI "\t\t\t<iYield>1</iYield>\n";
	print CI "\t\t</FreeYield>\n";
	print CI "\t</FreeYields>\n";
	print CI "\t<CapturedCityUnitClass>NONE</CapturedCityUnitClass>\n";
	print CI "\t<InitialCivics/>\n";
	print CI "\t<Leaders>\n";
	print CI "\t\t<Leader>\n";
	print CI "\t\t\t<LeaderName>LEADER_OFFA</LeaderName>\n";
	print CI "\t\t\t<bLeaderAvailability>1</bLeaderAvailability>\n";
	print CI "\t\t</Leader>\n";
	print CI "\t</Leaders>\n";
	print CI "\t<DerivativeCiv>CIVILIZATION_".$tag1."</DerivativeCiv>\n";
	print CI "\t<CivilizationSelectionSound>AS3D_ENGLAND_SELECT</CivilizationSelectionSound>\n";
	print CI "\t<CivilizationActionSound>AS3D_ENGLAND_ORDER</CivilizationActionSound>\n";
	print CI "\t<FreeTechs/>\n";
	print CI "</CivilizationInfo>\n";

	print ADC "<CivilizationArtInfo>\n";
	print ADC "\t<Type>ART_DEF_CIVILIZATION_".$tag."</Type>\n";
	print ADC "\t<Button>Art/Interface/TeamColor/".$tag.".dds</Button>\n";
	print ADC "\t<Path>Art/Interface/TeamColor/".$tag.".dds</Path>\n";
	print ADC "\t<bWhiteFlag>1</bWhiteFlag>\n";
	print ADC "\t<bInvertFlag>0</bInvertFlag>\n";
	print ADC "\t<iFontButtonIndex>21</iFontButtonIndex>\n";
	print ADC "</CivilizationArtInfo>\n";
	}

# 1st arg = civ description, rest = LHs
sub makehumanciv
{
	my $desc = shift;
	my $tag = $desc;
	$tag =~ tr/ /_/;
	$tag =~ tr/[a-z]/[A-Z]/;
	print CI "<CivilizationInfo>\n";
	print CI "\t<Type>CIVILIZATION_".$tag."</Type>\n";
	print CI "\t<Description>TXT_KEY_CIVILIZATION_".$tag."</Description>\n";
	&maketext("TXT_KEY_CIVILIZATION_".$tag, $desc." Colonies");
	print CI "\t<ShortDescription>TXT_KEY_CIVILIZATION_".$tag."_SHORT</ShortDescription>\n";
	&maketext("TXT_KEY_CIVILIZATION_".$tag."_SHORT", $desc);
	print CI "\t<Adjective>TXT_KEY_CIVILIZATION_".$tag."_ADJ</Adjective>\n";
	&maketext("TXT_KEY_CIVILIZATION_".$tag."_ADJ", $desc);
	my $pedia = '[COLOR_HIGHLIGHT_TEXT]'.$desc."[COLOR_REVERT].";
	print CI "\t<Civilopedia>TXT_KEY_CIVILIZATION_".$tag."_PEDIA</Civilopedia>\n";
	&maketext("TXT_KEY_CIVILIZATION_".$tag."_PEDIA", $desc);
	print CI "\t<DefaultPlayerColor>PLAYERCOLOR_WHITE</DefaultPlayerColor>\n";
	print CI "\t<ArtDefineTag>ART_DEF_CIVILIZATION_".$tag."</ArtDefineTag>\n";
	print CI "\t<ArtStyleType>ARTSTYLE_EUROPEAN</ArtStyleType>\n";
	print CI "\t<UnitArtStyleType>UNIT_ARTSTYLE_ANGLO</UnitArtStyleType>\n";
	print CI "\t<bPlayable>1</bPlayable>\n";
	print CI "\t<bAIPlayable>1</bAIPlayable>\n";
	print CI "\t<bWaterStart>1</bWaterStart>\n";
	print CI "\t<bOpenBorders>0</bOpenBorders>\n";
	print CI "\t<bWaterWorks>1</bWaterWorks>\n";
	print CI "\t<bEurope>0</bEurope>\n";
	print CI "\t<bNative>0</bNative>\n";
	print CI "\t<iAdvancedStartPoints>0</iAdvancedStartPoints>\n";
	print CI "\t<iAreaMultiplier>100</iAreaMultiplier>\n";
	print CI "\t<iDensityMultiplier>100</iDensityMultiplier>\n";
	print CI "\t<iTreasure>0</iTreasure>\n";
	print CI "\t<FavoredTerrain>NONE</FavoredTerrain>\n";
	print CI "\t<DefaultProfession>PROFESSION_COLONIST</DefaultProfession>\n";
	print CI "\t<Cities>\n";
	print CI "\t\t<City>City</City>\n";
	print CI "\t</Cities>\n";
	print CI "\t<Buildings/>\n";
	print CI "\t<Units>\n";
	print CI "\t\t<Unit>\n";
	print CI "\t\t\t<UnitClassType>UNITCLASS_KILLBOTS</UnitClassType>\n";
	print CI "\t\t\t<UnitType>NONE</UnitType>\n";
	print CI "\t\t</Unit>\n";
	print CI "\t</Units>\n";
	print CI "\t<Professions>\n";
	print CI "\t\t<Profession>\n";
	print CI "\t\t\t<ProfessionType>PROFESSION_TERRORIST</ProfessionType>\n";
	print CI "\t\t\t<bValid>0</bValid>\n";
	print CI "\t\t</Profession>\n";
	print CI "\t\t<Profession>\n";
	print CI "\t\t\t<ProfessionType>PROFESSION_PIRATE</ProfessionType>\n";
	print CI "\t\t\t<bValid>0</bValid>\n";
	print CI "\t\t</Profession>\n";
	print CI "\t</Professions>\n";
	print CI "\t<Traits>\n";
	print CI "\t\t<Trait>\n";
	print CI "\t\t\t<TraitType>TRAIT_RESEARCH_ERA_1_MOD</TraitType>\n";
	print CI "\t\t\t<bTrait>0</bTrait>\n";
	print CI "\t\t</Trait>\n";
	print CI "\t</Traits>\n";
	print CI "\t<FreeUnitClasses>\n";
	print CI "\t\t<FreeUnitClass>\n";
	print CI "\t\t\t<UnitClassType>UNITCLASS_CORVETTE</UnitClassType>\n";
	print CI "\t\t\t<FreeUnitProfession>NONE</FreeUnitProfession>\n";
	print CI "\t\t</FreeUnitClass>\n";
	print CI "\t\t<FreeUnitClass>\n";
	print CI "\t\t\t<UnitClassType>UNITCLASS_COLONIST</UnitClassType>\n";
	print CI "\t\t\t<FreeUnitProfession>PROFESSION_COLONIST</FreeUnitProfession>\n";
	print CI "\t\t</FreeUnitClass>\n";
	print CI "\t\t<FreeUnitClass>\n";
	print CI "\t\t\t<UnitClassType>UNITCLASS_COLONIST</UnitClassType>\n";
	print CI "\t\t\t<FreeUnitProfession>PROFESSION_COLONIST</FreeUnitProfession>\n";
	print CI "\t\t</FreeUnitClass>\n";
	print CI "\t</FreeUnitClasses>\n";
	print CI "\t<FreeBuildingClasses>\n";
	print CI "\t\t<FreeBuildingClass>\n";
	print CI "\t\t\t<BuildingClassType>BUILDINGCLASS_INDUSTRY1</BuildingClassType>\n";
	print CI "\t\t\t<bFreeBuildingClass>1</bFreeBuildingClass>\n";
	print CI "\t\t</FreeBuildingClass>\n";
	print CI "\t</FreeBuildingClasses>\n";
	print CI "\t<TeachUnitClasses/>\n";
	print CI "\t<FreeYields>\n";
	print CI "\t\t<FreeYield>\n";
	print CI "\t\t\t<YieldType>YIELD_NUTRIENTS</YieldType>\n";
	print CI "\t\t\t<iYield>1</iYield>\n";
	print CI "\t\t</FreeYield>\n";
	print CI "\t\t<FreeYield>\n";
	print CI "\t\t\t<YieldType>YIELD_INDUSTRY</YieldType>\n";
	print CI "\t\t\t<iYield>1</iYield>\n";
	print CI "\t\t</FreeYield>\n";
	print CI "\t\t<FreeYield>\n";
	print CI "\t\t\t<YieldType>YIELD_MEDIA</YieldType>\n";
	print CI "\t\t\t<iYield>1</iYield>\n";
	print CI "\t\t</FreeYield>\n";
	print CI "\t</FreeYields>\n";
	print CI "\t<CapturedCityUnitClass>NONE</CapturedCityUnitClass>\n";
	print CI "\t<InitialCivics/>\n";
	print CI "\t<Leaders>\n";
	print CI "\t\t<Leader>\n";
	print CI "\t\t\t<LeaderName>LEADER_OFFA</LeaderName>\n";
	print CI "\t\t\t<bLeaderAvailability>1</bLeaderAvailability>\n";
	print CI "\t\t</Leader>\n";
	print CI "\t</Leaders>\n";
	print CI "\t<DerivativeCiv>NONE</DerivativeCiv>\n";
	print CI "\t<CivilizationSelectionSound>AS3D_ENGLAND_SELECT</CivilizationSelectionSound>\n";
	print CI "\t<CivilizationActionSound>AS3D_ENGLAND_ORDER</CivilizationActionSound>\n";
	print CI "\t<FreeTechs/>\n";
	print CI "</CivilizationInfo>\n";

	print ADC "<CivilizationArtInfo>\n";
	print ADC "\t<Type>ART_DEF_CIVILIZATION_".$tag."</Type>\n";
	print ADC "\t<Button>Art/Interface/TeamColor/".$tag.".dds</Button>\n";
	print ADC "\t<Path>Art/Interface/TeamColor/".$tag.".dds</Path>\n";
	print ADC "\t<bWhiteFlag>1</bWhiteFlag>\n";
	print ADC "\t<bInvertFlag>0</bInvertFlag>\n";
	print ADC "\t<iFontButtonIndex>21</iFontButtonIndex>\n";
	print ADC "</CivilizationArtInfo>\n";

	&makekingciv($desc);
	}

# create civs and parents
&makehumanciv('NAFTA');
&makehumanciv('EU');
&makehumanciv('China');
&makehumanciv('Russia');
&makehumanciv('Japan');
&makehumanciv('Caliphate');
&makehumanciv('Consortium');
&makehumanciv('Syndicate');

# add hardcoded civs
open (HARD, '< ../Assets/XML/Civilizations/CIV4CivilizationInfos_hardcoded.xml') or die "Can't read hardcoded civs: $!";	
foreach (<HARD>) {print CI  $_;}
close HARD;

# close files
print CI '</CivilizationInfos>'."\n</Civ4CivilizationInfos>\n";
close CI;
#print LI '</LeaderheadInfos>'."\n</Civ4LeaderheadInfos>\n";
#close LI;
print ADC '</CivilizationArtInfos>'."\n</Civ4ArtDefines>\n";
close ADC;

# close text
print TEXT "</Civ4GameText>\n";
close TEXT;

# ** PLACEHOLDER ARTDEFS **
# placeholder art paths

open (ADB, '< ../Assets/XML/Art/CIV4ArtDefines_Building.xml');
while (<ADB>) {
	push (@lines, $_);
	}
close ADB;
open (ADB, '> ../Assets/XML/Art/CIV4ArtDefines_Building.xml');
foreach $line (@lines) {
		if ($line =~ /CityTexture/) {print ADB "\t<CityTexture>,Art/Interface/Screens/City_Management/city_buildings_altas.dds,1,1</CityTexture>\n";}
		elsif ($line =~ /<NIF>/) {print ADB "\t<NIF>Art/Structures/Buildings/Town_Hall/Town_Hall.nif</NIF>\n";}		
		elsif ($line =~ /<Button>/) {print ADB "\t<Button>,Art/Interface/Buttons/Unit_Resource_Colonization_Atlas.dds,4,6</Button>\n";}
		else {print ADB $line;}
		}
close ADB;
@lines = '';

open (ADU, '< ../Assets/XML/Art/CIV4ArtDefines_Unit.xml');
while (<ADU>) {
	push (@lines, $_);
	}
close ADU;
open (ADU, '> ../Assets/XML/Art/CIV4ArtDefines_Unit.xml');
foreach $line (@lines) {
		if ($line =~ /FullLength/) {print ADU "\t<FullLengthIcon>,Art/Interface/Screens/City_Management/Free_Colonist.dds</FullLengthIcon>\n";}
#		elsif ($line =~ /<NIF>/) {print ADU "\t<NIF>Art/Units/Free_Colonist/Free_Colonist.nif</NIF>\n";}
#		elsif ($line =~ /<KFM>/) {print ADU "\t<KFM>Art/Units/Free_Colonist/Free_Colonist.kfm</KFM>\n";}			
		elsif ($line =~ /<Button>/) {print ADU "\t<Button>,Art/Interface/Buttons/Unit_Resource_Colonization_Atlas.dds,1,3</Button>\n";}
		else {print ADU $line;}
		}
close ADU;
@lines = '';

open (ADI, '< ../Assets/XML/Art/CIV4ArtDefines_Improvement.xml');
while (<ADI>) {
	push (@lines, $_);
	}
close ADI;
open (ADI, '> ../Assets/XML/Art/CIV4ArtDefines_Improvement.xml');
foreach $line (@lines) {
		if ($line =~ /Button/) {print ADI "\t<Button>,Art/Interface/Buttons/Unit_Resource_Colonization_Atlas.dds,5,17</Button>\n";}
		elsif ($line =~ /<NIF>/) {print ADI "\t<NIF>Art/Structures/Improvements/Farm/farm_2x1_building.nif</NIF>\n";}		
		else {print ADI $line;}
		}
close ADI;