//
// Yields_Medieval_Tech.cpp
// Written by Nightinggale
//
#include "CvGameCoreDLL.h"
#include "CvGlobals.h"
#include "YieldsTestBase.h"

// AI sells unconditionally to natives and Europe unless they are raw materials as well
void Check_YieldGroup_AI_Sell::build()
{
YieldVector.push_back(YIELD_SILVER);
YieldVector.push_back(YIELD_CLOTH);
YieldVector.push_back(YIELD_COATS);
YieldVector.push_back(YIELD_ALE);
YieldVector.push_back(YIELD_WINE);
YieldVector.push_back(YIELD_TRADE_GOODS);
}

// AI sells these in Europe
// AI can also sell unneeded raw materials even if they aren't listed here
void Check_YieldGroup_AI_Sell_To_Europe::build()
{
YieldVector.push_back(YIELD_SALT);
YieldVector.push_back(YIELD_SILVER);
YieldVector.push_back(YIELD_COTTON);
YieldVector.push_back(YIELD_FUR);
YieldVector.push_back(YIELD_BARLEY);
YieldVector.push_back(YIELD_GRAPES);
YieldVector.push_back(YIELD_ORE);
YieldVector.push_back(YIELD_CLOTH);
YieldVector.push_back(YIELD_COATS);
YieldVector.push_back(YIELD_ALE);
YieldVector.push_back(YIELD_WINE);
YieldVector.push_back(YIELD_CATTLE);
YieldVector.push_back(YIELD_SHEEP);
YieldVector.push_back(YIELD_WOOL);
YieldVector.push_back(YIELD_SPICES);
}

// AI attemps to buy from natives as needed (or whenever offered?)
void Check_YieldGroup_AI_Buy_From_Natives::build()
{
YieldVector.push_back(YIELD_SPICES);
YieldVector.push_back(YIELD_TOOLS);
YieldVector.push_back(YIELD_GRAIN);
YieldVector.push_back(YIELD_CATTLE);
}

// AI attemps to buy from Europe (Europe as in vanilla functionality)
void Check_YieldGroup_AI_Buy_From_Europe::build()
{
YieldVector.push_back(YIELD_LEATHER_ARMOR);
YieldVector.push_back(YIELD_SCALE_ARMOR);
YieldVector.push_back(YIELD_MAIL_ARMOR);
YieldVector.push_back(YIELD_PLATE_ARMOR);
YieldVector.push_back(YIELD_TOOLS);
YieldVector.push_back(YIELD_WEAPONS);
YieldVector.push_back(YIELD_HORSES);
YieldVector.push_back(YIELD_TRADE_GOODS);
YieldVector.push_back(YIELD_SPICES);
}

// AI sells unless they are needed
// Used for production building input like ore, cotton etc.
void Check_YieldGroup_AI_Raw_Material::build()
{
YieldVector.push_back(YIELD_COTTON);
YieldVector.push_back(YIELD_BARLEY);
YieldVector.push_back(YIELD_GRAPES);
YieldVector.push_back(YIELD_ORE);
YieldVector.push_back(YIELD_CATTLE);
YieldVector.push_back(YIELD_SPICES);
YieldVector.push_back(YIELD_SHEEP);
YieldVector.push_back(YIELD_WOOL);
YieldVector.push_back(YIELD_SALT);
YieldVector.push_back(YIELD_STONE);
YieldVector.push_back(YIELD_FUR);
}

// Yields to show up on city billboards
void Check_YieldGroup_City_Billboard::build()
{
YieldVector.push_back(YIELD_SPICES);
YieldVector.push_back(YIELD_HORSES);
YieldVector.push_back(YIELD_CATTLE);
YieldVector.push_back(YIELD_SHEEP);
YieldVector.push_back(YIELD_SALT);
YieldVector.push_back(YIELD_TOOLS);
}

// yields, which are affected by an off by one offset error when displaying billboard icons
// TODO: find the real culprint of this bug instead of working around it.
void Check_YieldGroup_City_Billboard_Offset_Fix::build()
{
YieldVector.push_back(YIELD_SPICES);
YieldVector.push_back(YIELD_HORSES);
YieldVector.push_back(YIELD_SALT);
YieldVector.push_back(YIELD_TOOLS);
}

// yield is either light or heavy armor (not both)
void Check_YieldGroup_Armor::build()
{
YieldVector.push_back(YIELD_LEATHER_ARMOR);
YieldVector.push_back(YIELD_SCALE_ARMOR);
YieldVector.push_back(YIELD_MAIL_ARMOR);
YieldVector.push_back(YIELD_PLATE_ARMOR);
}

// yield is light armor
void Check_YieldGroup_Light_Armor::build()
{
YieldVector.push_back(YIELD_LEATHER_ARMOR);
}

// yield is heavy armor
void Check_YieldGroup_Heavy_Armor::build()
{
YieldVector.push_back(YIELD_SCALE_ARMOR);
YieldVector.push_back(YIELD_MAIL_ARMOR);
YieldVector.push_back(YIELD_PLATE_ARMOR);
}

// check YieldTypes vs XML yield names
void BaseCheckYieldGroup::checkXML()
{
// first argument is YieldTypes enum value while the second is the name in XML
checkSingleXMLType(YIELD_FOOD, "YIELD_NUTRIENTS");
checkSingleXMLType(YIELD_GRAIN, "YIELD_BIONICS");
checkSingleXMLType(YIELD_CATTLE, "YIELD_MICROBES");
checkSingleXMLType(YIELD_SHEEP, "YIELD_HARD_CURRENCY");
checkSingleXMLType(YIELD_WOOL, "YIELD_CRYSTALLOIDS");
checkSingleXMLType(YIELD_LUMBER, "YIELD_BIOPOLYMERS");
checkSingleXMLType(YIELD_STONE, "YIELD_SILICATES");
checkSingleXMLType(YIELD_SILVER, "YIELD_PRECIOUS_METALS");
checkSingleXMLType(YIELD_SALT, "YIELD_ELECTROLYTES");
checkSingleXMLType(YIELD_SPICES, "YIELD_ICE");
checkSingleXMLType(YIELD_FUR, "YIELD_ISOTOPES");
checkSingleXMLType(YIELD_COTTON, "YIELD_URANIUM");
checkSingleXMLType(YIELD_BARLEY, "YIELD_RARE_EARTHS");
checkSingleXMLType(YIELD_GRAPES, "YIELD_DATA");
checkSingleXMLType(YIELD_ORE, "YIELD_BASE_METALS");
checkSingleXMLType(YIELD_CLOTH, "YIELD_TISSUE_SAMPLES");
checkSingleXMLType(YIELD_COATS, "YIELD_NUCLEIC_ACIDS");
checkSingleXMLType(YIELD_ALE, "YIELD_AMINO_ACIDS");
checkSingleXMLType(YIELD_WINE, "YIELD_OPIATES");
checkSingleXMLType(YIELD_TOOLS, "YIELD_TOOLS");
checkSingleXMLType(YIELD_WEAPONS, "YIELD_WEAPONS");
checkSingleXMLType(YIELD_HORSES, "YIELD_ROBOTICS");
checkSingleXMLType(YIELD_LEATHER_ARMOR, "YIELD_XENOTOXINS");
checkSingleXMLType(YIELD_SCALE_ARMOR, "YIELD_BOTANICALS");
checkSingleXMLType(YIELD_MAIL_ARMOR, "YIELD_HYDROCARBONS");
checkSingleXMLType(YIELD_PLATE_ARMOR, "YIELD_HALOGENS");
checkSingleXMLType(YIELD_TRADE_GOODS, "YIELD_EARTH_GOODS");
checkSingleXMLType(YIELD_HAMMERS, "YIELD_HAMMERS");
checkSingleXMLType(YIELD_BELLS, "YIELD_BELLS");
checkSingleXMLType(YIELD_CROSSES, "YIELD_CROSSES");
checkSingleXMLType(YIELD_EDUCATION, "YIELD_EDUCATION");
checkSingleXMLType(YIELD_IDEAS, "YIELD_IDEAS");
checkSingleXMLType(YIELD_CULTURE, "YIELD_CULTURE");
checkSingleXMLType(YIELD_GOLD, "YIELD_GOLD");

}