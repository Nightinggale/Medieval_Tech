//
// Yields_Medieval_Tech.cpp
// Written by Nightinggale
//
#include "CvGameCoreDLL.h"
#include "CvGlobals.h"
#include "YieldsTestBase.h"

#ifdef WORLD_HISTORY

// AI sells unconditionally to natives and Europe unless they are raw materials as well
void Check_YieldGroup_AI_Sell::build()
{
	YieldVector.push_back(YIELD_JEWELLERY);
	YieldVector.push_back(YIELD_CLOTH);
	YieldVector.push_back(YIELD_COATS);
	YieldVector.push_back(YIELD_ALE);
	YieldVector.push_back(YIELD_WINE);
	YieldVector.push_back(YIELD_POTTERY);
	YieldVector.push_back(YIELD_BRONZE);
	YieldVector.push_back(YIELD_WOODEN_GOODS);
	YieldVector.push_back(YIELD_CARVED_IVORY);
	YieldVector.push_back(YIELD_SILK);
	YieldVector.push_back(YIELD_FINE_CLOTH);
	YieldVector.push_back(YIELD_JADE_CARVING);
	YieldVector.push_back(YIELD_ELECTRONICS);
	YieldVector.push_back(YIELD_RUM);
	YieldVector.push_back(YIELD_CERAMICS);
	YieldVector.push_back(YIELD_TAILORED_CLOTHING);
	YieldVector.push_back(YIELD_HOUSEHOLD_GOODS);
	YieldVector.push_back(YIELD_MODERN_CARS);
	YieldVector.push_back(YIELD_CARS);
	YieldVector.push_back(YIELD_APPLIANCES);
	YieldVector.push_back(YIELD_COMPUTERS);
}

// AI sells these in Europe
// AI can also sell unneeded raw materials even if they aren't listed here
void Check_YieldGroup_AI_Sell_To_Europe::build()
{
	YieldVector.push_back(YIELD_JEWELLERY);
	YieldVector.push_back(YIELD_CLOTH);
	YieldVector.push_back(YIELD_COATS);
	YieldVector.push_back(YIELD_ALE);
	YieldVector.push_back(YIELD_WINE);
	YieldVector.push_back(YIELD_POTTERY);
	YieldVector.push_back(YIELD_BRONZE);
	YieldVector.push_back(YIELD_WOODEN_GOODS);
	YieldVector.push_back(YIELD_CARVED_IVORY);
	YieldVector.push_back(YIELD_SILK);
	YieldVector.push_back(YIELD_FINE_CLOTH);
	YieldVector.push_back(YIELD_JADE_CARVING);
	YieldVector.push_back(YIELD_ELECTRONICS);
	YieldVector.push_back(YIELD_RUM);
	YieldVector.push_back(YIELD_CERAMICS);
	YieldVector.push_back(YIELD_TAILORED_CLOTHING);
	YieldVector.push_back(YIELD_HOUSEHOLD_GOODS);
	YieldVector.push_back(YIELD_MODERN_CARS);
	YieldVector.push_back(YIELD_CARS);
	YieldVector.push_back(YIELD_APPLIANCES);
	YieldVector.push_back(YIELD_COMPUTERS);
}

// AI attemps to buy from natives as needed (or whenever offered?)
void Check_YieldGroup_AI_Buy_From_Natives::build()
{
	YieldVector.push_back(YIELD_SPICES);
	YieldVector.push_back(YIELD_STONE_TOOLS);
	YieldVector.push_back(YIELD_GRAIN);
	YieldVector.push_back(YIELD_CATTLE);
	YieldVector.push_back(YIELD_HORSES);
	YieldVector.push_back(YIELD_SHEEP);
	YieldVector.push_back(YIELD_PRECIOUS_METALS);
	YieldVector.push_back(YIELD_HERBS);
	YieldVector.push_back(YIELD_IRON_TOOLS);
	YieldVector.push_back(YIELD_GRAIN);
	YieldVector.push_back(YIELD_FOOD);
	YieldVector.push_back(YIELD_RICH_FOOD);
}

// AI attemps to buy from Europe (Europe as in vanilla functionality)
void Check_YieldGroup_AI_Buy_From_Europe::build()
{
	YieldVector.push_back(YIELD_LEATHER_ARMOR);
	YieldVector.push_back(YIELD_BRONZE_ARMOR);
	YieldVector.push_back(YIELD_SCALE_ARMOR);
	YieldVector.push_back(YIELD_CHAIN_ARMOR);
	YieldVector.push_back(YIELD_PLATE_ARMOR);
	YieldVector.push_back(YIELD_ARMOR);
	YieldVector.push_back(YIELD_MODERN_ARMOR);
	YieldVector.push_back(YIELD_STONE_TOOLS);
	YieldVector.push_back(YIELD_IRON_TOOLS);
	YieldVector.push_back(YIELD_POWER_TOOLS);
	YieldVector.push_back(YIELD_STONE_WEAPONS);
	YieldVector.push_back(YIELD_BRONZE_WEAPONS);
	YieldVector.push_back(YIELD_IRON_WEAPONS);
	YieldVector.push_back(YIELD_STEEL_WEAPONS);
	YieldVector.push_back(YIELD_MUSKETS);
	YieldVector.push_back(YIELD_RIFLES);
	YieldVector.push_back(YIELD_HERBS);
	YieldVector.push_back(YIELD_SPICES);
}

// AI sells unless they are needed
// Used for production building input like ore, cotton etc.
void Check_YieldGroup_AI_Raw_Material::build()
{
	YieldVector.push_back(YIELD_COTTON);
	YieldVector.push_back(YIELD_BARLEY);
	YieldVector.push_back(YIELD_GRAPES);
	YieldVector.push_back(YIELD_SUGAR);
	YieldVector.push_back(YIELD_JADE);
	YieldVector.push_back(YIELD_IVORY);
	YieldVector.push_back(YIELD_ORE);
	YieldVector.push_back(YIELD_CATTLE);
	YieldVector.push_back(YIELD_HERBS);
	YieldVector.push_back(YIELD_SPICES);
	YieldVector.push_back(YIELD_SHEEP);
	YieldVector.push_back(YIELD_WOOL);
	YieldVector.push_back(YIELD_CLAY);
	YieldVector.push_back(YIELD_STONE);
	YieldVector.push_back(YIELD_COPPER);
	YieldVector.push_back(YIELD_TIN);
	YieldVector.push_back(YIELD_BRONZE);
	YieldVector.push_back(YIELD_IRON);
	YieldVector.push_back(YIELD_COAL);
	YieldVector.push_back(YIELD_STEEL);
	YieldVector.push_back(YIELD_SILICON);
	YieldVector.push_back(YIELD_ALUMINIUM);
	YieldVector.push_back(YIELD_OIL);
	YieldVector.push_back(YIELD_PLASTIC);
	YieldVector.push_back(YIELD_RUBBER);
	YieldVector.push_back(YIELD_ELECTRONICS);
	YieldVector.push_back(YIELD_FUEL);
	YieldVector.push_back(YIELD_ROCKETS);
}

// Set yields the natives will produce
// Natives will also produce yields, which has bIsNativeTrade set in XML
// Other code might give natives the ability to produce even more yields.
//   They can produce yields, which is accepted by any one of the options.
void Check_YieldGroup_AI_Native_Product::build()
{
	YieldVector.push_back(YIELD_WOOL);
	YieldVector.push_back(YIELD_CATTLE);
	YieldVector.push_back(YIELD_SHEEP);
	YieldVector.push_back(YIELD_HORSES);
	YieldVector.push_back(YIELD_LUMBER);
	YieldVector.push_back(YIELD_CLAY);
	YieldVector.push_back(YIELD_STONE);
	YieldVector.push_back(YIELD_PRECIOUS_METALS);
	YieldVector.push_back(YIELD_ORE);
	YieldVector.push_back(YIELD_HERBS);
	YieldVector.push_back(YIELD_SPICES);
	YieldVector.push_back(YIELD_IVORY);
	YieldVector.push_back(YIELD_JADE);
	YieldVector.push_back(YIELD_SUGAR);
	YieldVector.push_back(YIELD_BARLEY);
	YieldVector.push_back(YIELD_GEMS);
	YieldVector.push_back(YIELD_DYE);
	YieldVector.push_back(YIELD_COTTON);
	YieldVector.push_back(YIELD_SILK_WORM);
}

// Yields to show up on city billboards
void Check_YieldGroup_City_Billboard::build()
{
	YieldVector.push_back(YIELD_SPICES);
	YieldVector.push_back(YIELD_HORSES);
	YieldVector.push_back(YIELD_CATTLE);
	YieldVector.push_back(YIELD_SHEEP);
	YieldVector.push_back(YIELD_CLAY);
	YieldVector.push_back(YIELD_STONE_TOOLS);
}

// yields, which are affected by an off by one offset error when displaying billboard icons
// TODO: find the real culprint of this bug instead of working around it.
void Check_YieldGroup_City_Billboard_Offset_Fix::build()
{
	YieldVector.push_back(YIELD_SPICES);
	YieldVector.push_back(YIELD_HORSES);
	YieldVector.push_back(YIELD_CLAY);
	YieldVector.push_back(YIELD_STONE_TOOLS);
}

// yield is either light or heavy armor (not both)
void Check_YieldGroup_Armor::build()
{
	YieldVector.push_back(YIELD_LEATHER_ARMOR);
	YieldVector.push_back(YIELD_BRONZE_ARMOR);
	YieldVector.push_back(YIELD_SCALE_ARMOR);
	YieldVector.push_back(YIELD_CHAIN_ARMOR);
	YieldVector.push_back(YIELD_PLATE_ARMOR);
	YieldVector.push_back(YIELD_ARMOR);
	YieldVector.push_back(YIELD_MODERN_ARMOR);
}

// yield is light armor
void Check_YieldGroup_Light_Armor::build()
{
	YieldVector.push_back(YIELD_LEATHER_ARMOR);
	YieldVector.push_back(YIELD_BRONZE_ARMOR);
}

// yield is heavy armor
void Check_YieldGroup_Heavy_Armor::build()
{
	YieldVector.push_back(YIELD_SCALE_ARMOR);
	YieldVector.push_back(YIELD_CHAIN_ARMOR);
	YieldVector.push_back(YIELD_PLATE_ARMOR);
	YieldVector.push_back(YIELD_ARMOR);
	YieldVector.push_back(YIELD_MODERN_ARMOR);
}

// check YieldTypes vs XML yield names
void BaseCheckYieldGroup::checkXML()
{
	// first argument is YieldTypes enum value while the second is the name in XML
	checkSingleXMLType(YIELD_FOOD,                 "YIELD_FOOD");
	checkSingleXMLType(YIELD_GRAIN,                "YIELD_GRAIN");
	checkSingleXMLType(YIELD_HORSES,               "YIELD_HORSES");
	checkSingleXMLType(YIELD_CATTLE,               "YIELD_CATTLE");
	checkSingleXMLType(YIELD_SHEEP,                "YIELD_SHEEP");
	checkSingleXMLType(YIELD_LUMBER,               "YIELD_LUMBER");
	checkSingleXMLType(YIELD_STONE,                "YIELD_STONE");
	checkSingleXMLType(YIELD_STONE_TOOLS,          "YIELD_STONE_TOOLS");
	checkSingleXMLType(YIELD_STONE_WEAPONS,        "YIELD_STONE_WEAPONS");
	checkSingleXMLType(YIELD_ORE,                  "YIELD_ORE");
	checkSingleXMLType(YIELD_PRECIOUS_METALS,      "YIELD_PRECIOUS_METALS");
	checkSingleXMLType(YIELD_GRAPES,               "YIELD_GRAPES");
	checkSingleXMLType(YIELD_WINE,                 "YIELD_WINE");
	checkSingleXMLType(YIELD_HERBS,                "YIELD_HERBS");
	checkSingleXMLType(YIELD_CLAY,                 "YIELD_CLAY");
	checkSingleXMLType(YIELD_POTTERY,              "YIELD_POTTERY");
	checkSingleXMLType(YIELD_COTTON,               "YIELD_COTTON");
	checkSingleXMLType(YIELD_CLOTH,                "YIELD_CLOTH");
	checkSingleXMLType(YIELD_WOOL,                 "YIELD_WOOL");
	checkSingleXMLType(YIELD_COATS,                "YIELD_COATS");
	checkSingleXMLType(YIELD_HIDES,                "YIELD_HIDES");
	checkSingleXMLType(YIELD_LEATHER_ARMOR,        "YIELD_LEATHER_ARMOR");
	checkSingleXMLType(YIELD_COPPER,               "YIELD_COPPER");
	checkSingleXMLType(YIELD_TIN,                  "YIELD_TIN");
	checkSingleXMLType(YIELD_BRONZE,               "YIELD_BRONZE");
	checkSingleXMLType(YIELD_BRONZE_WEAPONS,       "YIELD_BRONZE_WEAPONS");
	checkSingleXMLType(YIELD_BRONZE_ARMOR,         "YIELD_BRONZE_ARMOR");
	checkSingleXMLType(YIELD_RICH_FOOD,            "YIELD_RICH_FOOD");
	checkSingleXMLType(YIELD_BARLEY,               "YIELD_BARLEY");
	checkSingleXMLType(YIELD_ALE,                  "YIELD_ALE");
	checkSingleXMLType(YIELD_SUGAR,                "YIELD_SUGAR");
	checkSingleXMLType(YIELD_RUM,                  "YIELD_RUM");
	checkSingleXMLType(YIELD_WOODEN_GOODS,         "YIELD_WOODEN_GOODS");
	checkSingleXMLType(YIELD_CUT_STONE,            "YIELD_CUT_STONE");
	checkSingleXMLType(YIELD_IRON,                 "YIELD_IRON");
	checkSingleXMLType(YIELD_COAL,                 "YIELD_COAL");
	checkSingleXMLType(YIELD_STEEL,                "YIELD_STEEL");
	checkSingleXMLType(YIELD_GEMS,                 "YIELD_GEMS");
	checkSingleXMLType(YIELD_IVORY,                "YIELD_IVORY");
	checkSingleXMLType(YIELD_CARVED_IVORY,         "YIELD_CARVED_IVORY");
	checkSingleXMLType(YIELD_SPICES,               "YIELD_SPICES");
	checkSingleXMLType(YIELD_SILK_WORM,            "YIELD_SILK_WORM");
	checkSingleXMLType(YIELD_SILK,                 "YIELD_SILK");
	checkSingleXMLType(YIELD_DYE,                  "YIELD_DYE");
	checkSingleXMLType(YIELD_FINE_CLOTH,           "YIELD_FINE_CLOTH");
	checkSingleXMLType(YIELD_JADE,                 "YIELD_JADE");
	checkSingleXMLType(YIELD_JADE_CARVING,         "YIELD_JADE_CARVING");
	checkSingleXMLType(YIELD_JEWELLERY,            "YIELD_JEWELLERY");
	checkSingleXMLType(YIELD_IRON_TOOLS,           "YIELD_IRON_TOOLS");
	checkSingleXMLType(YIELD_IRON_WEAPONS,         "YIELD_IRON_WEAPONS");
	checkSingleXMLType(YIELD_STEEL_WEAPONS,        "YIELD_STEEL_WEAPONS");
	checkSingleXMLType(YIELD_SCALE_ARMOR,          "YIELD_SCALE_ARMOR");
	checkSingleXMLType(YIELD_CHAIN_ARMOR,          "YIELD_CHAIN_ARMOR");
	checkSingleXMLType(YIELD_PLATE_ARMOR,          "YIELD_PLATE_ARMOR");
	checkSingleXMLType(YIELD_TINNED_FOOD,          "YIELD_TINNED_FOOD");
	checkSingleXMLType(YIELD_OIL,                  "YIELD_OIL");
	checkSingleXMLType(YIELD_PLASTIC,              "YIELD_PLASTIC");
	checkSingleXMLType(YIELD_RUBBER,               "YIELD_RUBBER");
	checkSingleXMLType(YIELD_SILICON,              "YIELD_SILICON");
	checkSingleXMLType(YIELD_ELECTRONICS,          "YIELD_ELECTRONICS");
	checkSingleXMLType(YIELD_ALUMINIUM,            "YIELD_ALUMINIUM");
	checkSingleXMLType(YIELD_FUEL,                 "YIELD_FUEL");
	checkSingleXMLType(YIELD_CERAMICS,             "YIELD_CERAMICS");
	checkSingleXMLType(YIELD_STEAM_ENGINES,        "YIELD_STEAM_ENGINES");
	checkSingleXMLType(YIELD_TAILORED_CLOTHING,    "YIELD_TAILORED_CLOTHING");
	checkSingleXMLType(YIELD_HOUSEHOLD_GOODS,      "YIELD_HOUSEHOLD_GOODS");
	checkSingleXMLType(YIELD_APPLIANCES,           "YIELD_APPLIANCES");
	checkSingleXMLType(YIELD_CARS,                 "YIELD_CARS");
	checkSingleXMLType(YIELD_MODERN_CARS,          "YIELD_MODERN_CARS");
	checkSingleXMLType(YIELD_COMPUTERS,            "YIELD_COMPUTERS");
	checkSingleXMLType(YIELD_ARTILLERY_PARTS,      "YIELD_ARTILLERY_PARTS");
	checkSingleXMLType(YIELD_POWER_TOOLS,          "YIELD_POWER_TOOLS");
	checkSingleXMLType(YIELD_MUSKETS,              "YIELD_MUSKETS");
	checkSingleXMLType(YIELD_RIFLES,               "YIELD_RIFLES");
	checkSingleXMLType(YIELD_AUTOMATIC_WEAPONS,    "YIELD_AUTOMATIC_WEAPONS");
	checkSingleXMLType(YIELD_ROCKETS,              "YIELD_ROCKETS");
	checkSingleXMLType(YIELD_GUIDED_MISSILES,      "YIELD_GUIDED_MISSILES");
	checkSingleXMLType(YIELD_ENGINE_PARTS,         "YIELD_ENGINE_PARTS");
	checkSingleXMLType(YIELD_ARMOR,                "YIELD_ARMOR");
	checkSingleXMLType(YIELD_MODERN_ARMOR,         "YIELD_MODERN_ARMOR");
	checkSingleXMLType(YIELD_AERIAL_PARTS,         "YIELD_AERIAL_PARTS");
	checkSingleXMLType(YIELD_TOOLS,                "YIELD_TOOLS");
	checkSingleXMLType(YIELD_WEAPONS,              "YIELD_WEAPONS");
	checkSingleXMLType(YIELD_TRADE_GOODS,          "YIELD_TRADE_GOODS");
	checkSingleXMLType(YIELD_HAMMERS,              "YIELD_HAMMERS");
	checkSingleXMLType(YIELD_BELLS,                "YIELD_BELLS");
	checkSingleXMLType(YIELD_CROSSES,              "YIELD_CROSSES");
	checkSingleXMLType(YIELD_EDUCATION,            "YIELD_EDUCATION");
	checkSingleXMLType(YIELD_IDEAS,                "YIELD_IDEAS");
	checkSingleXMLType(YIELD_CULTURE,              "YIELD_CULTURE");
	checkSingleXMLType(YIELD_GOLD,                 "YIELD_GOLD");
}
#endif // WORLD_HISTORY
