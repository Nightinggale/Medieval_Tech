#pragma once

#ifndef YIELDS_H
#define YIELDS_H

// Yields.h

#include "CvDefines.h"

enum DllExport YieldTypes
{
	NO_YIELD = -1,

	YIELD_FOOD,///0
	///TKs ME
	YIELD_GRAIN,///1NEW*
	YIELD_CATTLE,///2/NEW*
	YIELD_SHEEP,///3/NEW*
	YIELD_WOOL,///4NEW*
	//YIELD_SALT,///5NEW*
	YIELD_LUMBER,///6
	YIELD_STONE,///7NEW*
//	YIELD_LEATHER,///20NEW*
	YIELD_SILVER,///8
	//YIELD_GOLD,///9/NEW*
//	YIELD_IVORY,//9/NEW*
    YIELD_SALT,///5NEW*
	YIELD_SPICES,///10NEW*
	YIELD_FUR,///11
	YIELD_COTTON,///12
	YIELD_BARLEY,///13YIELD_SUGAR,
	YIELD_GRAPES,///14YIELD_TOBACCO,
	YIELD_ORE,///15
	YIELD_CLOTH,///16
	YIELD_COATS,///17
	YIELD_ALE,///18YIELD_RUM,
	YIELD_WINE,///19YIELD_CIGARS,
	YIELD_TOOLS,///20
	YIELD_WEAPONS,///21YIELD_MUSKETS,
	YIELD_HORSES,///22
	YIELD_LEATHER_ARMOR,///23NEW*
	YIELD_SCALE_ARMOR,///24NEW*
	YIELD_MAIL_ARMOR,///25NEW*
	YIELD_PLATE_ARMOR,///26NEW*
	YIELD_TRADE_GOODS,///27
//	YIELD_SILK,///29NEW*
//	YIELD_PORCELAIN,///30NEW*
	YIELD_HAMMERS,///28
	YIELD_BELLS,///29
	YIELD_CROSSES,///30
	YIELD_EDUCATION,///31
	///TKs Invention Core Mod v 1.0
	YIELD_IDEAS,///32
	YIELD_CULTURE,///33
    YIELD_GOLD,///34/NEW*

	///TKe

#ifdef _USRDLL
	NUM_YIELD_TYPES
#endif
};

static inline bool YieldIsBonusResource(YieldTypes eYield)
{
	switch (eYield)
	{
		///Bonus Resources
		case YIELD_SALT:
		case YIELD_SILVER:
		case YIELD_COTTON:
		case YIELD_FUR:
		case YIELD_BARLEY:
		case YIELD_GRAPES:
		case YIELD_ORE:
		case YIELD_CLOTH:
		case YIELD_COATS:
		case YIELD_ALE:
		case YIELD_WINE:
			return true;
		default:
			return false;
	}
}

static inline bool YieldIsRawMaterial(YieldTypes eYield)
{
	switch (eYield)
	{
		case YIELD_COTTON:
		case YIELD_BARLEY:
		case YIELD_GRAPES:
		case YIELD_ORE:
        case YIELD_CATTLE:
        case YIELD_SPICES:
        case YIELD_SHEEP:
        case YIELD_WOOL:
        case YIELD_SALT:
        case YIELD_STONE:
        case YIELD_FUR:
			return true;
		default:
			return false;
	}
}

#endif	// YIELDS_H