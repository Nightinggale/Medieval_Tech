#pragma once

// utils.h

#ifndef CIV4_GAMECORE_UTILS_H
#define CIV4_GAMECORE_UTILS_H


#ifndef _USRDLL
// use non inline functions when not in the dll
#define getMapINLINE	getMap
#define getGridHeightINLINE	getGridHeight
#define getGridWidthINLINE	getGridWidth
#define isWrapYINLINE	isWrapY
#define isWrapXINLINE	isWrapX
#define plotINLINE	plot
#define getX_INLINE	getX
#define getY_INLINE	getY

#endif

class CvPlot;
class CvCity;
class CvUnit;
class CvString;
class CvRandom;
class FAStarNode;
class FAStar;
class CvInfoBase;


#ifndef SQR
#define SQR(x) ( (x)*(x))
#endif

#undef max
#undef min

//sign function taken from FirePlace - JW
template<class T> __forceinline T getSign( T x ) { return (( x < 0 ) ? T(-1) : x > 0 ? T(1) : T(0)); };

inline int range(int iNum, int iLow, int iHigh)
{
	FAssertMsg(iHigh >= iLow, "High should be higher than low");

	if (iNum < iLow)
	{
		return iLow;
	}
	else if (iNum > iHigh)
	{
		return iHigh;
	}
	else
	{
		return iNum;
	}
}

inline float range(float fNum, float fLow, float fHigh)
{
	FAssertMsg(fHigh >= fLow, "High should be higher than low");

	if (fNum < fLow)
	{
		return fLow;
	}
	else if (fNum > fHigh)
	{
		return fHigh;
	}
	else
	{
		return fNum;
	}
}

DllExport CvPlot* plotCity(int iX, int iY, int iIndex);
DllExport int plotCityXY(int iDX, int iDY);
DllExport int plotCityXY(const CvCity* pCity, const CvPlot* pPlot);

DllExport bool isLeaderCivMatch(LeaderHeadTypes eLeader, CivilizationTypes eCiv, bool bHuman);

DllExport CardinalDirectionTypes getOppositeCardinalDirection(CardinalDirectionTypes eDir);
DllExport DirectionTypes cardinalDirectionToDirection(CardinalDirectionTypes eCard);
DllExport bool isCardinalDirection(DirectionTypes eDirection);
DllExport DirectionTypes estimateDirection(int iDX, int iDY);
DllExport DirectionTypes estimateDirection(const CvPlot* pFromPlot, const CvPlot* pToPlot);
DllExport float directionAngle(DirectionTypes eDirection);
DllExport bool atWar(TeamTypes eTeamA, TeamTypes eTeamB);
DllExport bool isPotentialEnemy(TeamTypes eOurTeam, TeamTypes eTheirTeam);

DllExport CvCity* getCity(const IDInfo& city);
DllExport CvUnit* getUnit(const IDInfo& unit);

DllExport bool isBeforeUnitCycle(const CvUnit* pFirstUnit, const CvUnit* pSecondUnit);

DllExport ImprovementTypes finalImprovementUpgrade(ImprovementTypes eImprovement, int iCount = 0);

DllExport __int64 getBinomialCoefficient(int iN, int iK);
DllExport int getCombatOdds(CvUnit* pAttacker, CvUnit* pDefender);

DllExport void setTradeItem(TradeData* pItem, TradeableItems eItemType, int iData1, const IDInfo* pTransport);

DllExport bool isPlotEventTrigger(EventTriggerTypes eTrigger);
DllExport void clear(wchar* szString);
DllExport void clear(char* szString);
DllExport void clear(std::string& szString);
DllExport void clear(std::wstring& szString);
DllExport void safecpy(char * szDest, const char * szSource, int iMaxLen);
DllExport void safecpy(wchar * szDest, const wchar * szSource, int iMaxLen);
DllExport void safecpy(CvWString& szDest, const CvWString& szSource, int iMaxLen);
DllExport bool isEmpty(const char* szString);
DllExport bool isEmpty(const std::string& szStr);
DllExport bool isEmpty(const wchar* szString);
DllExport bool isEmpty(const std::wstring& szStr);
DllExport void setListHelp(wchar* szBuffer, const wchar* szStart, const wchar* szItem, const wchar* szSeparator, bool bFirst);
DllExport void setListHelp(CvWString& szBuffer, const wchar* szStart, const wchar* szItem, const wchar* szSeparator, bool bFirst);
DllExport void setListHelp(CvWStringBuffer& szBuffer, const wchar* szStart, const wchar* szItem, const wchar* szSeparator, bool bFirst);

// PlotUnitFunc's...
DllExport bool PUF_isGroupHead( const CvUnit* pUnit, int iData1 = -1, int iData2 = -1);
DllExport bool PUF_isPlayer( const CvUnit* pUnit, int iData1, int iData2 = -1);
DllExport bool PUF_isVisualTeam( const CvUnit* pUnit, int iData1, int iData2 = -1);
DllExport bool PUF_isCombatTeam(const CvUnit* pUnit, int iData1, int iData2);
DllExport bool PUF_isOtherTeam( const CvUnit* pUnit, int iData1, int iData2 = -1);
DllExport bool PUF_isEnemy( const CvUnit* pUnit, int iData1, int iData2 = -1);
///TKs Med
DllExport bool PUF_isBarbarianEnemy( const CvUnit* pUnit, int iData1, int iData2 = -1);
///TKe
DllExport bool PUF_isVisible( const CvUnit* pUnit, int iData1, int iData2 = -1);
DllExport bool PUF_isVisibleDebug( const CvUnit* pUnit, int iData1, int iData2 = -1);
DllExport bool PUF_canSiege( const CvUnit* pUnit, int iData1, int iData2 = -1);
DllExport bool PUF_isPotentialEnemy( const CvUnit* pUnit, int iData1, int iData2 = -1);
DllExport bool PUF_canDeclareWar( const CvUnit* pUnit, int iData1 = -1, int iData2 = -1);
DllExport bool PUF_canDefend( const CvUnit* pUnit, int iData1 = -1, int iData2 = -1);
DllExport bool PUF_cannotDefend( const CvUnit* pUnit, int iData1 = -1, int iData2 = -1);
DllExport bool PUF_canDefendGroupHead( const CvUnit* pUnit, int iData1 = -1, int iData2 = -1);
DllExport bool PUF_canDefendEnemy( const CvUnit* pUnit, int iData1, int iData2 = -1);
DllExport bool PUF_canDefendPotentialEnemy( const CvUnit* pUnit, int iData1, int iData2 = -1);
DllExport bool PUF_isFighting( const CvUnit* pUnit, int iData1, int iData2 = -1);
DllExport bool PUF_isUnitType( const CvUnit* pUnit, int iData1, int iData2 = -1);
DllExport bool PUF_isDomainType( const CvUnit* pUnit, int iData1, int iData2 = -1);
DllExport bool PUF_isUnitAIType( const CvUnit* pUnit, int iData1, int iData2 = -1);
DllExport bool PUF_isCityAIType( const CvUnit* pUnit, int iData1, int iData2 = -1);
DllExport bool PUF_isNotCityAIType( const CvUnit* pUnit, int iData1, int iData2 = -1);
DllExport bool PUF_isSelected( const CvUnit* pUnit, int iData1 = -1, int iData2 = -1);
DllExport bool PUF_makeInfoBarDirty(CvUnit* pUnit, int iData1 = -1, int iData2 = -1);
bool PUF_isNoMission(const CvUnit* pUnit, int iData1 = -1, int iData2 = -1);

// Inet Stuff
DllExport void sendGameStats(wchar* pURL);

// FAStarFunc...
DllExport int pathDestValid(int iToX, int iToY, const void* pointer, FAStar* finder);
DllExport int pathHeuristic(int iFromX, int iFromY, int iToX, int iToY);
DllExport int pathCost(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
DllExport int pathValid(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
DllExport int pathAdd(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
DllExport int stepDestValid(int iToX, int iToY, const void* pointer, FAStar* finder);
DllExport int stepHeuristic(int iFromX, int iFromY, int iToX, int iToY);
DllExport int stepValid(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
DllExport int stepCost(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
DllExport int stepAdd(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
DllExport int routeValid(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
DllExport int borderValid(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
DllExport int areaValid(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
DllExport int joinArea(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);

DllExport bool isPickableName(const TCHAR* szName);

DllExport int getTurnMonthForGame(int iGameTurn, int iStartYear, CalendarTypes eCalendar, GameSpeedTypes eSpeed);
DllExport int getTurnYearForGame(int iGameTurn, int iStartYear, CalendarTypes eCalendar, GameSpeedTypes eSpeed);

DllExport void boolsToString(const bool* pBools, int iNumBools, CvString* szOut);
DllExport void stringToBools(const char* szString, int* iNumBools, bool** ppBools);

void getDirectionTypeString(CvWString& szString, DirectionTypes eDirectionType);
void getCardinalDirectionTypeString(CvWString& szString, CardinalDirectionTypes eDirectionType);
void getActivityTypeString(CvWString& szString, ActivityTypes eActivityType);
void getMissionTypeString(CvWString& szString, MissionTypes eMissionType);
void getMissionAIString(CvWString& szString, MissionAITypes eMissionAI);
void getUnitAIString(CvWString& szString, UnitAITypes eUnitAI);
void getPlayerAIStrategyString(CvWString& szString, StrategyTypes eStrategyAI);///TKs Med Test Code
bool shouldMoveBefore(const CvUnit* pUnitA, const CvUnit* pUnitB);
bool shouldUnitMove(const CvUnit* pUnit);

/// post load function - start - Nightinggale
void postLoadGameFixes(int iFixCount);
/// post load function - end - Nightinggale

class CvShouldMoveBefore
{
public:
	CvShouldMoveBefore(PlayerTypes ePlayer) : m_ePlayer(ePlayer) {}

	bool operator()(int iUnitIdA, int iUnitIdB) const
	{
		return shouldMoveBefore(getUnit(IDInfo(m_ePlayer, iUnitIdA)), getUnit(IDInfo(m_ePlayer, iUnitIdB)));
	}
private:
	PlayerTypes m_ePlayer;
};

class CvShouldUnitMove
{
public:
	CvShouldUnitMove(PlayerTypes ePlayer) : m_ePlayer(ePlayer) {}

	bool operator()(int iUnitId) const
	{
		return shouldUnitMove(getUnit(IDInfo(m_ePlayer, iUnitId)));
	}
private:
	PlayerTypes m_ePlayer;
};
#endif
