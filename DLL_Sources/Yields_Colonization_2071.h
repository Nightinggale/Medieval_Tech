#pragma once

#ifndef YIELDS_COLONIZATION_2071_H
#define YIELDS_COLONIZATION_2071_H

//
// Yields_Colonization_2071.h
// Written by Nightinggale
//

#ifdef COLONIZATION_2071

// keep this enum in sync with python::enum_<YieldTypes>("YieldTypes") in CyEnumsInterface.cpp

enum DllExport YieldTypes
{
	NO_YIELD = -1,

	YIELD_NUTRIENTS,
	YIELD_BIOPOLYMERS,
	YIELD_SILICATES,
	YIELD_BASE_METALS,
	YIELD_ACTINIDES,
	YIELD_ISOTOPES,
	YIELD_RARE_EARTHS,
	YIELD_CRYSTALLOIDS,
	YIELD_NUCLEIC_ACIDS,
	YIELD_AMINO_ACIDS,
	YIELD_TISSUE_SAMPLES,
	YIELD_MICROBES,
	YIELD_DATACORES,
	YIELD_PROGENITOR_ARTIFACTS,
	YIELD_ALIEN_SPECIMENS,
	YIELD_PRECIOUS_METALS,
	YIELD_OPIATES,
	YIELD_XENOTOXINS,
	YIELD_BOTANICALS,
	YIELD_HYDROCARBONS,
	YIELD_CLATHRATES,
	YIELD_CORE_SAMPLES,
	YIELD_MACHINE_TOOLS,
	YIELD_ROBOTICS,
	YIELD_MUNITIONS,
	YIELD_PHOTONICS,
	YIELD_PLASTEEL,
	YIELD_DURALLOY,
	YIELD_CRYSTALLOY,
	YIELD_NUCLEONICS,
	YIELD_FUSION_CORES,
	YIELD_SEMICONDUCTORS,
	YIELD_PLASMIDS,
	YIELD_ENZYMES,
	YIELD_STEM_CELLS,
	YIELD_STATE_SECRETS,
	YIELD_PROGENITOR_TECH,
	YIELD_ALIEN_RELICS,
	YIELD_NARCOTICS,
	YIELD_BIOWEAPONS,
	YIELD_PHARMACEUTICALS,
	YIELD_PETROCHEMICALS,
	YIELD_COLLOIDS,
	YIELD_CATALYSTS,
	YIELD_HARD_CURRENCY,
	YIELD_EARTH_GOODS,
	YIELD_CONTRABAND,
	YIELD_EARTHLING_SPECIMENS,

	// virtual yields
	YIELD_INDUSTRY,
	YIELD_MEDIA,
	YIELD_LIBERTY,
	YIELD_RESEARCH,
	YIELD_EDUCATION,
	YIELD_INFLUENCE,
	YIELD_ENERGY,
	YIELD_POLLUTANTS,
	YIELD_CREDITS,

#ifdef _USRDLL
	NUM_YIELD_TYPES,
#endif

	YIELD_FROM_ANIMALS = NO_YIELD,

	// name connection with hardcoded names
	YIELD_FOOD        = YIELD_NUTRIENTS,
	YIELD_LUMBER      = YIELD_BIOPOLYMERS,
	YIELD_STONE       = YIELD_SILICATES,
	YIELD_ORE         = YIELD_BASE_METALS,
	YIELD_TOOLS       = YIELD_MACHINE_TOOLS,
	YIELD_WEAPONS     = YIELD_MUNITIONS,
	YIELD_HORSES      = YIELD_ROBOTICS,
	YIELD_TRADE_GOODS = YIELD_EARTH_GOODS,

	// virtual
	YIELD_HAMMERS = YIELD_INDUSTRY,
	YIELD_BELLS = YIELD_LIBERTY,
	YIELD_CROSSES = YIELD_MEDIA,
	//YIELD_EDUCATION,
	YIELD_IDEAS = YIELD_RESEARCH,
	YIELD_CULTURE = YIELD_INFLUENCE,
    YIELD_GOLD = YIELD_CREDITS,

	YIELD_DEFAULT_ARMOR_TYPE = YIELD_PLASTEEL,

	NUM_CARGO_YIELD_TYPES = YIELD_HAMMERS,
};

static inline bool YieldGroup_AI_Sell(YieldTypes eYield)
{
	return (eYield >= YIELD_EARTH_GOODS && eYield <= YIELD_CONTRABAND);
}

static inline bool YieldGroup_AI_Sell_To_Europe(YieldTypes eYield)
{
	return (eYield <= YIELD_CORE_SAMPLES && eYield >= YIELD_BASE_METALS) || (eYield >= YIELD_PHOTONICS && eYield <= YIELD_HARD_CURRENCY) || (eYield == YIELD_EARTHLING_SPECIMENS);
}

static inline bool YieldGroup_AI_Buy_From_Natives(YieldTypes eYield)
{
	return (eYield <= YIELD_CORE_SAMPLES && eYield >= YIELD_ACTINIDES);
}

static inline bool YieldGroup_AI_Buy_From_Europe(YieldTypes eYield)
{
	return (eYield <= YIELD_MUNITIONS && eYield >= YIELD_MACHINE_TOOLS) || (eYield >= YIELD_EARTH_GOODS && eYield <= YIELD_CONTRABAND);
}

static inline bool YieldGroup_AI_Raw_Material(YieldTypes eYield)
{
	return (eYield <= YIELD_BASE_METALS && eYield >= YIELD_NUTRIENTS) || (eYield <= YIELD_CRYSTALLOY && eYield >= YIELD_PHOTONICS) || (eYield == YIELD_MACHINE_TOOLS);
}

static inline bool YieldGroup_AI_Native_Product(YieldTypes eYield)
{
	return false;
}

static inline bool YieldGroup_City_Billboard(YieldTypes eYield)
{
	return false;
}

static inline bool YieldGroup_City_Billboard_Offset_Fix(YieldTypes eYield)
{
	return false;
}

static inline bool YieldGroup_Armor(YieldTypes eYield)
{
	return false;
}

static inline bool YieldGroup_Light_Armor(YieldTypes eYield)
{
	return false;
}

static inline bool YieldGroup_Heavy_Armor(YieldTypes eYield)
{
	return false;
}

static inline bool YieldGroup_Luxury_Food(YieldTypes eYield)
{
	return false;
}

// MOD specific global defines

// number of education levels
// vanilla has 3 (schoolhouse, college, university)
#define NUM_TEACH_LEVELS 3

#endif // COLONIZATION_2071

#endif	// YIELDS_MEDIEVAL_TECH_H
