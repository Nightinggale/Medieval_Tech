#pragma once

#ifndef YIELDS_H
#define YIELDS_H

//
// Yields.h
// Written by Nightinggale
//

#include "CvDefines.h"


// only include the header needed for the mod in question
// including all mod yield headers will work, but it will force recompilation of all mods when one mod changes something
#if defined(MEDIEVAL_TECH)
#include "Yields_Medieval_Tech.h"
#elif defined(COLONIZATION_2071)
#include "Yields_Colonization_2071.h"
#elif defined(WORLD_HISTORY)
#include "Yields_World_History.h"
#endif

// virtual means it can't be loaded into a ship, meaning it's yields like crosses and bells.
static inline bool YieldGroup_Virtual(YieldTypes eYield)
{
	// Vanilla assumes hammers to be the first virtual yield. This is deeply hardcoded in C++ as well as python.
	return eYield >= YIELD_HAMMERS;
}

static inline bool YieldGroup_Cargo(YieldTypes eYield)
{
	return !YieldGroup_Virtual(eYield);
}

#endif	// YIELDS_H
