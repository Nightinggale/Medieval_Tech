#pragma once

#ifndef YIELDS_WORLD_HISTORY_H
#define YIELDS_WORLD_HISTORY_H

//
// Yields_Medieval_Tech.h
// Written by Nightinggale
//

#ifdef WORLD_HISTORY

// keep this enum in sync with python::enum_<YieldTypes>("YieldTypes") in CyEnumsInterface.cpp

enum DllExport YieldTypes
{
	NO_YIELD = -1,

	YIELD_FOOD,
	YIELD_GRAIN,
	YIELD_HORSES,
	YIELD_CATTLE,
	YIELD_SHEEP,
	YIELD_LUMBER,
	YIELD_STONE,
	YIELD_STONE_TOOLS,
	YIELD_STONE_WEAPONS,
	YIELD_ORE,
	YIELD_PRECIOUS_METALS,
	YIELD_GRAPES,
	YIELD_WINE,
	YIELD_HERBS,
	YIELD_CLAY,
	YIELD_POTTERY,
	YIELD_COTTON,
	YIELD_CLOTH,
	YIELD_WOOL,
	YIELD_COATS,
	YIELD_HIDES,
	YIELD_LEATHER_ARMOR,
	YIELD_COPPER,
	YIELD_TIN,
	YIELD_BRONZE,
	YIELD_BRONZE_WEAPONS,
	YIELD_BRONZE_ARMOR,
	YIELD_RICH_FOOD,
	YIELD_BARLEY,
	YIELD_ALE,
	YIELD_SUGAR,
	YIELD_RUM,
	YIELD_WOODEN_GOODS,
	YIELD_CUT_STONE,
	YIELD_IRON,
	YIELD_COAL,
	YIELD_STEEL,
	YIELD_GEMS,
	YIELD_IVORY,
	YIELD_CARVED_IVORY,
	YIELD_SPICES,
	YIELD_SILK_WORM,
	YIELD_SILK,
	YIELD_DYE,
	YIELD_FINE_CLOTH,
	YIELD_JADE,
	YIELD_JADE_CARVING,
	YIELD_JEWELLERY,
	YIELD_IRON_TOOLS,
	YIELD_IRON_WEAPONS,
	YIELD_STEEL_WEAPONS,
	YIELD_SCALE_ARMOR, 
	YIELD_CHAIN_ARMOR,
	YIELD_PLATE_ARMOR,
	YIELD_TINNED_FOOD,
	YIELD_OIL,
	YIELD_PLASTIC,
	YIELD_RUBBER,
	YIELD_SILICON,
	YIELD_ELECTRONICS,
	YIELD_ALUMINIUM,
	YIELD_FUEL,
	YIELD_CERAMICS,
	YIELD_STEAM_ENGINES,
	YIELD_TAILORED_CLOTHING,
	YIELD_HOUSEHOLD_GOODS,
	YIELD_APPLIANCES,
	YIELD_CARS,
	YIELD_MODERN_CARS,
	YIELD_COMPUTERS,
	YIELD_ARTILLERY_PARTS,
	YIELD_POWER_TOOLS,
	YIELD_MUSKETS,
	YIELD_RIFLES,
	YIELD_AUTOMATIC_WEAPONS,
	YIELD_ROCKETS,
	YIELD_GUIDED_MISSILES,
	YIELD_ENGINE_PARTS,
	YIELD_ARMOR,
	YIELD_MODERN_ARMOR,
	YIELD_AERIAL_PARTS,
	YIELD_TOOLS,
	YIELD_WEAPONS,
	YIELD_TRADE_GOODS,
	YIELD_HAMMERS,
	YIELD_BELLS,
	YIELD_CROSSES,
	YIELD_EDUCATION,
	///TKs Invention Core Mod v 1.0
	YIELD_IDEAS,
	YIELD_CULTURE,
    YIELD_GOLD,

	///TKe

#ifdef _USRDLL
	NUM_YIELD_TYPES,
#endif

	// Setup for which yields to have certain hardcoded functions
	YIELD_FROM_ANIMALS = YIELD_FOOD,
	YIELD_DEFAULT_ARMOR_TYPE = YIELD_BRONZE_ARMOR,

	NUM_CARGO_YIELD_TYPES = YIELD_HAMMERS,
};

static inline bool YieldGroup_AI_Sell(YieldTypes eYield)
{
	return (eYield == YIELD_PRECIOUS_METALS) || (eYield == YIELD_WINE) || (eYield == YIELD_POTTERY) || (eYield == YIELD_CLOTH) || (eYield == YIELD_COATS) || (eYield == YIELD_ALE);
}

static inline bool YieldGroup_AI_Sell_To_Europe(YieldTypes eYield)
{
	return (eYield <= YIELD_SHEEP && eYield >= YIELD_CATTLE) || (eYield <= YIELD_WINE && eYield >= YIELD_ORE) || (eYield <= YIELD_COATS && eYield >= YIELD_COTTON) || (eYield <= YIELD_ALE && eYield >= YIELD_BARLEY) || (eYield == YIELD_CLAY) || (eYield == YIELD_SPICES);
}

static inline bool YieldGroup_AI_Buy_From_Natives(YieldTypes eYield)
{
	return (eYield == YIELD_GRAIN) || (eYield == YIELD_CATTLE) || (eYield == YIELD_STONE_TOOLS) || (eYield == YIELD_SPICES);
}

static inline bool YieldGroup_AI_Buy_From_Europe(YieldTypes eYield)
{
	return (eYield >= YIELD_TOOLS && eYield <= YIELD_WEAPONS) || (eYield == YIELD_HORSES) || (eYield == YIELD_POTTERY) || (eYield == YIELD_LEATHER_ARMOR) || (eYield == YIELD_SPICES) || (eYield == YIELD_SCALE_ARMOR) || (eYield == YIELD_PLATE_ARMOR);
}

static inline bool YieldGroup_AI_Raw_Material(YieldTypes eYield)
{
	return (eYield <= YIELD_SHEEP && eYield >= YIELD_CATTLE) || (eYield == YIELD_STONE) || (eYield == YIELD_ORE) || (eYield == YIELD_GRAPES) || (eYield == YIELD_CLAY) || (eYield == YIELD_COTTON) || (eYield == YIELD_WOOL) || (eYield == YIELD_BRONZE) || (eYield == YIELD_BARLEY) || (eYield == YIELD_SPICES);
}

static inline bool YieldGroup_AI_Native_Product(YieldTypes eYield)
{
	return (eYield <= YIELD_SHEEP && eYield >= YIELD_CATTLE) || (eYield == YIELD_WOOL);
}

static inline bool YieldGroup_City_Billboard(YieldTypes eYield)
{
	return (eYield <= YIELD_SHEEP && eYield >= YIELD_HORSES) || (eYield == YIELD_STONE_TOOLS) || (eYield == YIELD_CLAY) || (eYield == YIELD_SPICES);
}

static inline bool YieldGroup_City_Billboard_Offset_Fix(YieldTypes eYield)
{
	return (eYield == YIELD_HORSES) || (eYield == YIELD_STONE_TOOLS) || (eYield == YIELD_CLAY) || (eYield == YIELD_SPICES);
}

static inline bool YieldGroup_Armor(YieldTypes eYield)
{
	return (eYield == YIELD_LEATHER_ARMOR) || (eYield == YIELD_SCALE_ARMOR) || (eYield == YIELD_PLATE_ARMOR);
}

static inline bool YieldGroup_Light_Armor(YieldTypes eYield)
{
	return (eYield == YIELD_LEATHER_ARMOR);
}

static inline bool YieldGroup_Heavy_Armor(YieldTypes eYield)
{
	return (eYield == YIELD_SCALE_ARMOR) || (eYield == YIELD_PLATE_ARMOR);
}

static inline bool YieldGroup_Luxury_Food(YieldTypes eYield)
{
	return eYield == YIELD_GRAIN;
}

// MOD specific global defines

// number of education levels
// vanilla has 3 (schoolhouse, college, university)
#define NUM_TEACH_LEVELS 3

#endif // MEDIEVAL_TECH

#endif	// YIELDS_MEDIEVAL_TECH_H
