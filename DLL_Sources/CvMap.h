#pragma once

#ifndef CIV4_MAP_H
#define CIV4_MAP_H

//
//	FILE:	 CvMap.h
//	AUTHOR:  Soren Johnson
//	PURPOSE: Game map class
//-----------------------------------------------------------------------------
//	Copyright (c) 2004 Firaxis Games, Inc. All rights reserved.
//-----------------------------------------------------------------------------
//


#include "CvArea.h"
#include "CvPlot.h"


class FAStar;


inline int coordRange(int iCoord, int iRange, bool bWrap)
{
	if (bWrap)
	{
		if (iRange != 0)
		{
			if (iCoord < 0 )
			{
				return (iRange + (iCoord % iRange));
			}
			else if (iCoord >= iRange)
			{
				return (iCoord % iRange);
			}
		}
	}

	return iCoord;
}


//
// holds initialization info
//
struct CvMapInitData
{
	int m_iGridW;						// in game plots
	int m_iGridH;						// in game plots
	int m_iTopLatitude;
	int m_iBottomLatitude;

	bool m_bWrapX;
	bool m_bWrapY;

	CvMapInitData(int iGridW=0, int iGridH=0, int iTopLatitude=90, int iBottomLatitude=-90, bool bWrapX=false, bool bWrapY=false) :
		m_iGridH(iGridH),m_iGridW(iGridW),m_iTopLatitude(iTopLatitude),m_iBottomLatitude(iBottomLatitude),m_bWrapY(bWrapY),m_bWrapX(bWrapX)
	{ }
};


//
// CvMap
//
class CvSelectionGroup;
class CvMap
{

	friend class CyMap;

public:

	DllExport CvMap();
	DllExport virtual ~CvMap();

	DllExport void init(CvMapInitData* pInitData=NULL);
	DllExport void setupGraphical();
	DllExport void reset(CvMapInitData* pInitData);

protected:

	void uninit();
	void setup();

public:
	DllExport void erasePlots();
	void setRevealedPlots(TeamTypes eTeam, bool bNewValue, bool bTerrainOnly = false);
	void setAllPlotTypes(PlotTypes ePlotType);

	void doTurn();

	DllExport void updateFlagSymbols();

	DllExport void updateFog();
	DllExport void updateVisibility();
	DllExport void updateSymbolVisibility();
	void updateSymbols();
	DllExport void updateMinimapColor();
	void updateSight(bool bIncrement);
	DllExport void updateCenterUnit();
	void updateWorkingCity();
	void updateMinOriginalStartDist(CvArea* pArea);
	void updateYield();
	void updateCulture();

	void verifyUnitValidPlot();
    ///TKs Med
	CvPlot* syncRandPlot(int iFlags = 0, int iArea = -1, int iMinUnitDistance = -1, int iTimeout = 100, bool bIgnoreNativeTeams = false);
	DllExport CvCity* findCity(int iX, int iY, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, bool bSameArea = true, bool bCoastalOnly = false, TeamTypes eTeamAtWarWith = NO_TEAM, DirectionTypes eDirection = NO_DIRECTION, CvCity* pSkipCity = NULL, bool bRandom = false);
	DllExport CvCity* findTraderCity(int iX, int iY, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam=NO_TEAM, bool bSameArea=true, bool bCoastalOnly=false, bool bNative=false, YieldTypes eNativeYield=NO_YIELD, int iMinAttitude = -1, CvUnit* pUnit=NULL, bool bRandom=false);
    ///TKe
	DllExport CvSelectionGroup* findSelectionGroup(int iX, int iY, PlayerTypes eOwner = NO_PLAYER, bool bReadyToSelect = false);

	CvArea* findBiggestArea(bool bWater);

	int getMapFractalFlags();
	bool findWater(CvPlot* pPlot, int iRange, bool bFreshWater);

	DllExport bool isPlot(int iX, int iY) const;
#ifdef _USRDLL
	inline int isPlotINLINE(int iX, int iY) const
	{
		return ((iX >= 0) && (iX < getGridWidthINLINE()) && (iY >= 0) && (iY < getGridHeightINLINE()));
	}
#endif
	DllExport int numPlots() const;
#ifdef _USRDLL
	inline int numPlotsINLINE() const
	{
		return getGridWidthINLINE() * getGridHeightINLINE();
	}
#endif
	DllExport int plotNum(int iX, int iY) const;
#ifdef _USRDLL
	inline int plotNumINLINE(int iX, int iY) const
	{
		return ((iY * getGridWidthINLINE()) + iX);
	}
#endif
	int plotX(int iIndex) const;
	int plotY(int iIndex) const;

	DllExport int pointXToPlotX(float fX);
	DllExport float plotXToPointX(int iX);

	DllExport int pointYToPlotY(float fY);
	DllExport float plotYToPointY(int iY);

	float getWidthCoords();
	float getHeightCoords();
	int maxPlotDistance();
	int maxStepDistance();

	DllExport int getGridWidth() const;
#ifdef _USRDLL
	inline int getGridWidthINLINE() const
	{
		return m_iGridWidth;
	}
#endif
	DllExport int getGridHeight() const;
#ifdef _USRDLL
	inline int getGridHeightINLINE() const
	{
		return m_iGridHeight;
	}
#endif
	DllExport int getLandPlots();
	void changeLandPlots(int iChange);
	DllExport int getOwnedPlots();
	void changeOwnedPlots(int iChange);
	int getTopLatitude();
	int getBottomLatitude();

	int getNextRiverID();
	void incrementNextRiverID();

	DllExport bool isWrapX();
#ifdef _USRDLL
	inline bool isWrapXINLINE() const
	{
		return m_bWrapX;
	}
#endif
	DllExport bool isWrapY();
#ifdef _USRDLL
	inline bool isWrapYINLINE() const
	{
		return m_bWrapY;
	}
#endif
	DllExport bool isWrap();
#ifdef _USRDLL
	inline bool isWrapINLINE() const
	{
		return m_bWrapX || m_bWrapY;
	}
#endif
	DllExport WorldSizeTypes getWorldSize();
	DllExport ClimateTypes getClimate();
	DllExport SeaLevelTypes getSeaLevel();

	DllExport int getNumCustomMapOptions();
	DllExport CustomMapOptionTypes getCustomMapOption(int iOption);

	int getNumBonuses(BonusTypes eIndex);
	void changeNumBonuses(BonusTypes eIndex, int iChange);
	int getNumBonusesOnLand(BonusTypes eIndex);
	void changeNumBonusesOnLand(BonusTypes eIndex, int iChange);
	DllExport CvPlot* plotByIndex(int iIndex) const;
#ifdef _USRDLL
	inline CvPlot* plotByIndexINLINE(int iIndex) const
	{
		return (((iIndex >= 0) && (iIndex < (getGridWidthINLINE() * getGridHeightINLINE()))) ? &(m_pMapPlots[iIndex]) : NULL);
	}
#endif
	DllExport CvPlot* plot(int iX, int iY) const;
#ifdef _USRDLL
	__forceinline CvPlot* plotINLINE(int iX, int iY) const
	{
		if ((iX == INVALID_PLOT_COORD) || (iY == INVALID_PLOT_COORD))
		{
			return NULL;
		}
		int iMapX = coordRange(iX, getGridWidthINLINE(), isWrapXINLINE());
		int iMapY = coordRange(iY, getGridHeightINLINE(), isWrapYINLINE());
		return ((isPlotINLINE(iMapX, iMapY)) ? &(m_pMapPlots[plotNumINLINE(iMapX, iMapY)]) : NULL);
	}
	__forceinline CvPlot* plotSorenINLINE(int iX, int iY) const
	{
		if ((iX == INVALID_PLOT_COORD) || (iY == INVALID_PLOT_COORD))
		{
			return NULL;
		}
		return &(m_pMapPlots[plotNumINLINE(iX, iY)]);
	}
#endif
	DllExport CvPlot* pointToPlot(float fX, float fY);
	int getIndexAfterLastArea();
	DllExport int getNumAreas();
	DllExport int getNumLandAreas();
	CvArea* getArea(int iID);
	CvArea* addArea();
	void deleteArea(int iID);
	// iteration
	CvArea* firstArea(int *pIterIdx, bool bRev=false);
	CvArea* nextArea(int *pIterIdx, bool bRev=false);

	void recalculateAreas();

	void resetPathDistance();
	int calculatePathDistance(CvPlot *pSource, CvPlot *pDest);

	// Serialization:
	DllExport virtual void read(FDataStreamBase* pStream);
	DllExport virtual void write(FDataStreamBase* pStream);
	void rebuild(int iGridW, int iGridH, int iTopLatitude, int iBottomLatitude, bool bWrapX, bool bWrapY, WorldSizeTypes eWorldSize, ClimateTypes eClimate, SeaLevelTypes eSeaLevel, int iNumCustomMapOptions, CustomMapOptionTypes * eCustomMapOptions);

protected:

	int m_iGridWidth;
	int m_iGridHeight;
	int m_iLandPlots;
	int m_iOwnedPlots;
	int m_iTopLatitude;
	int m_iBottomLatitude;
	int m_iNextRiverID;

	bool m_bWrapX;
	bool m_bWrapY;

	BonusArray<int> m_ja_iNumBonus;
	BonusArray<int> m_ja_iNumBonusOnLand;

	CvPlot* m_pMapPlots;

	FFreeListTrashArray<CvArea> m_areas;

	void calculateAreas();

	/// PlotGroup - start - Nightinggale
public:
	void combinePlotGroups(PlayerTypes ePlayer, CvPlotGroup* pPlotGroup1, CvPlotGroup* pPlotGroup2);
	/// PlotGroup - end - Nightinggale
};


// Vanilla code from CvGameCoreUtils.h

inline int coordDistance(int iFrom, int iTo, int iRange, bool bWrap)
{
	if (bWrap && (abs(iFrom - iTo) > (iRange / 2)))
	{
		return (iRange - abs(iFrom - iTo));
	}

	return abs(iFrom - iTo);
}

inline int wrapCoordDifference(int iDiff, int iRange, bool bWrap)
{
	if (bWrap)
	{
		if (iDiff > (iRange / 2))
		{
			return (iDiff - iRange);
		}
		else if (iDiff < -(iRange / 2))
		{
			return (iDiff + iRange);
		}
	}

	return iDiff;
}

inline int xDistance(int iFromX, int iToX)
{
	return coordDistance(iFromX, iToX, GC.getMapINLINE().getGridWidthINLINE(), GC.getMapINLINE().isWrapXINLINE());
}

inline int yDistance(int iFromY, int iToY)
{
	return coordDistance(iFromY, iToY, GC.getMapINLINE().getGridHeightINLINE(), GC.getMapINLINE().isWrapYINLINE());
}

inline int dxWrap(int iDX)
{
	return wrapCoordDifference(iDX, GC.getMapINLINE().getGridWidthINLINE(), GC.getMapINLINE().isWrapXINLINE());
}

inline int dyWrap(int iDY)
{
	return wrapCoordDifference(iDY, GC.getMapINLINE().getGridHeightINLINE(), GC.getMapINLINE().isWrapYINLINE());
}

// 4 | 4 | 3 | 3 | 3 | 4 | 4
// -------------------------
// 4 | 3 | 2 | 2 | 2 | 3 | 4
// -------------------------
// 3 | 2 | 1 | 1 | 1 | 2 | 3
// -------------------------
// 3 | 2 | 1 | 0 | 1 | 2 | 3
// -------------------------
// 3 | 2 | 1 | 1 | 1 | 2 | 3
// -------------------------
// 4 | 3 | 2 | 2 | 2 | 3 | 4
// -------------------------
// 4 | 4 | 3 | 3 | 3 | 4 | 4
//
// Returns the distance between plots according to the pattern above...
inline int plotDistance(int iX1, int iY1, int iX2, int iY2)
{
	int iDX;
	int iDY;

	iDX = xDistance(iX1, iX2);
	iDY = yDistance(iY1, iY2);

	return (std::max(iDX, iDY) + (std::min(iDX, iDY) / 2));
}

// 3 | 3 | 3 | 3 | 3 | 3 | 3
// -------------------------
// 3 | 2 | 2 | 2 | 2 | 2 | 3
// -------------------------
// 3 | 2 | 1 | 1 | 1 | 2 | 3
// -------------------------
// 3 | 2 | 1 | 0 | 1 | 2 | 3
// -------------------------
// 3 | 2 | 1 | 1 | 1 | 2 | 3
// -------------------------
// 3 | 2 | 2 | 2 | 2 | 2 | 3
// -------------------------
// 3 | 3 | 3 | 3 | 3 | 3 | 3
//
// Returns the distance between plots according to the pattern above...
inline int stepDistance(int iX1, int iY1, int iX2, int iY2)
{
	return std::max(xDistance(iX1, iX2), yDistance(iY1, iY2));
}

inline CvPlot* plotDirection(int iX, int iY, DirectionTypes eDirection)
{
	if(eDirection == NO_DIRECTION)
	{
		return GC.getMapINLINE().plotINLINE(iX, iY);
	}
	else
	{
		return GC.getMapINLINE().plotINLINE((iX + GC.getPlotDirectionX()[eDirection]), (iY + GC.getPlotDirectionY()[eDirection]));
	}
}

inline CvPlot* plotCardinalDirection(int iX, int iY, CardinalDirectionTypes eCardinalDirection)
{
	return GC.getMapINLINE().plotINLINE((iX + GC.getPlotCardinalDirectionX()[eCardinalDirection]), (iY + GC.getPlotCardinalDirectionY()[eCardinalDirection]));
}

inline CvPlot* plotXY(int iX, int iY, int iDX, int iDY)
{
	return GC.getMapINLINE().plotINLINE((iX + iDX), (iY + iDY));
}

inline DirectionTypes directionXY(int iDX, int iDY)
{
	if ((abs(iDX) > DIRECTION_RADIUS) || (abs(iDY) > DIRECTION_RADIUS))
	{
		return NO_DIRECTION;
	}
	else
	{
		return GC.getXYDirection((iDX + DIRECTION_RADIUS), (iDY + DIRECTION_RADIUS));
	}
}

inline DirectionTypes directionXY(const CvPlot* pFromPlot, const CvPlot* pToPlot)
{
	return directionXY(dxWrap(pToPlot->getX_INLINE() - pFromPlot->getX_INLINE()), dyWrap(pToPlot->getY_INLINE() - pFromPlot->getY_INLINE()));
}


#endif
