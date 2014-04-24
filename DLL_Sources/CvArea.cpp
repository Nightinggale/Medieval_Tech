// area.cpp

#include "CvGameCoreDLL.h"
#include "CvMap.h"
#include "CvGameAI.h"
#include "CvPlayerAI.h"
#include "CvInfos.h"

#include "CvDLLInterfaceIFaceBase.h"

// Public Functions...

CvArea::CvArea()
{
	m_aiUnitsPerPlayer = new int[MAX_PLAYERS];
	m_aiCitiesPerPlayer = new int[MAX_PLAYERS];
	m_aiPopulationPerPlayer = new int[MAX_PLAYERS];
	m_aiPower = new int[MAX_PLAYERS];
	m_aiBestFoundValue = new int[MAX_PLAYERS];
	m_aiNumRevealedTiles = new int[MAX_TEAMS];

	m_aeAreaAIType = new AreaAITypes[MAX_TEAMS];

	m_aTargetCities = new IDInfo[MAX_PLAYERS];

	m_aaiYieldRateModifier = new int*[MAX_PLAYERS];
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		m_aaiYieldRateModifier[i] = new int[NUM_YIELD_TYPES];
	}
	m_aaiNumTrainAIUnits = new int*[MAX_PLAYERS];
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		m_aaiNumTrainAIUnits[i] = new int[NUM_UNITAI_TYPES];
	}
	m_aaiNumAIUnits = new int*[MAX_PLAYERS];
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		m_aaiNumAIUnits[i] = new int[NUM_UNITAI_TYPES];
	}

	reset(0, false, true);
}


CvArea::~CvArea()
{
	uninit();

	SAFE_DELETE_ARRAY(m_aiUnitsPerPlayer);
	SAFE_DELETE_ARRAY(m_aiCitiesPerPlayer);
	SAFE_DELETE_ARRAY(m_aiPopulationPerPlayer);
	SAFE_DELETE_ARRAY(m_aiPower);
	SAFE_DELETE_ARRAY(m_aiBestFoundValue);
	SAFE_DELETE_ARRAY(m_aiNumRevealedTiles);
	SAFE_DELETE_ARRAY(m_aeAreaAIType);
	SAFE_DELETE_ARRAY(m_aTargetCities);
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		SAFE_DELETE_ARRAY(m_aaiYieldRateModifier[i]);
	}
	SAFE_DELETE_ARRAY(m_aaiYieldRateModifier);
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		SAFE_DELETE_ARRAY(m_aaiNumTrainAIUnits[i]);
	}
	SAFE_DELETE_ARRAY(m_aaiNumTrainAIUnits);
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		SAFE_DELETE_ARRAY(m_aaiNumAIUnits[i]);
	}
	SAFE_DELETE_ARRAY(m_aaiNumAIUnits);
}


void CvArea::init(int iID, bool bWater)
{
	//--------------------------------
	// Init saved data
	reset(iID, bWater);

	//--------------------------------
	// Init non-saved data

	//--------------------------------
	// Init other game data
}


void CvArea::uninit()
{
	m_ja_iNumBonuses.resetContent();
	m_ja_iNumImprovements.resetContent();
}


// FUNCTION: reset()
// Initializes data members that are serialized.
void CvArea::reset(int iID, bool bWater, bool bConstructorCall)
{
	int iI, iJ;

	//--------------------------------
	// Uninit class
	uninit();

	m_iID = iID;
	m_iNumTiles = 0;
	m_iNumOwnedTiles = 0;
	m_iNumRiverEdges = 0;
	m_iNumUnits = 0;
	m_iNumCities = 0;
	m_iNumStartingPlots = 0;

	m_bWater = bWater;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		m_aiUnitsPerPlayer[iI] = 0;
		m_aiCitiesPerPlayer[iI] = 0;
		m_aiPopulationPerPlayer[iI] = 0;
		m_aiPower[iI] = 0;
		m_aiBestFoundValue[iI] = 0;
	}

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		m_aiNumRevealedTiles[iI] = 0;
	}

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		m_aeAreaAIType[iI] = NO_AREAAI;
	}

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		m_aTargetCities[iI].reset();
	}

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			m_aaiYieldRateModifier[iI][iJ] = 0;
		}

		for (iJ = 0; iJ < NUM_UNITAI_TYPES; iJ++)
		{
			m_aaiNumTrainAIUnits[iI][iJ] = 0;
			m_aaiNumAIUnits[iI][iJ] = 0;
		}
	}

	if (!bConstructorCall)
	{
		m_ja_iNumBonuses.resetContent();
		m_ja_iNumImprovements.resetContent();
	}
}


int CvArea::getID() const
{
	return m_iID;
}


void CvArea::setID(int iID)
{
	m_iID = iID;
}


int CvArea::calculateTotalBestNatureYield() const
{
	int iCount = 0;

	for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

		if (pLoopPlot->getArea() == getID())
		{
			iCount += pLoopPlot->calculateTotalBestNatureYield(NO_TEAM);
		}
	}

	return iCount;
}


int CvArea::countCoastalLand() const
{
	if (isWater())
	{
		return 0;
	}

	int iCount = 0;

	for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

		if (pLoopPlot->getArea() == getID())
		{
			if (pLoopPlot->isCoastalLand())
			{
				iCount++;
			}
		}
	}

	return iCount;
}


int CvArea::countNumUniqueBonusTypes() const
{
	int iCount = 0;

	for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		if (getNumBonuses((BonusTypes)iI) > 0)
		{
			if (GC.getBonusInfo((BonusTypes)iI).isOneArea())
			{
				iCount++;
			}
		}
	}

	return iCount;
}


int CvArea::getNumTiles() const
{
	return m_iNumTiles;
}


bool CvArea::isLake() const
{
	return (isWater() && (getNumTiles() <= GC.getLAKE_MAX_AREA_SIZE()));
}


void CvArea::changeNumTiles(int iChange)
{
	bool bOldLake;

	if (iChange != 0)
	{
		bOldLake = isLake();

		m_iNumTiles += iChange;
		FAssert(getNumTiles() >= 0);

		if (bOldLake != isLake())
		{
			GC.getMapINLINE().updateYield();
		}
	}
}


int CvArea::getNumOwnedTiles() const
{
	return m_iNumOwnedTiles;
}


int CvArea::getNumUnownedTiles() const
{
	return (getNumTiles() - getNumOwnedTiles());
}


void CvArea::changeNumOwnedTiles(int iChange)
{
	m_iNumOwnedTiles = (m_iNumOwnedTiles + iChange);
	FAssert(getNumOwnedTiles() >= 0);
	FAssert(getNumUnownedTiles() >= 0);
}


int CvArea::getNumRiverEdges() const
{
	return m_iNumRiverEdges;
}


void CvArea::changeNumRiverEdges(int iChange)
{
	m_iNumRiverEdges += iChange;
	FAssert(getNumRiverEdges() >= 0);
}


int CvArea::getNumUnits() const
{
	return m_iNumUnits;
}


int CvArea::getNumCities() const
{
	return m_iNumCities;
}


int CvArea::getNumStartingPlots() const
{
	return m_iNumStartingPlots;
}


void CvArea::changeNumStartingPlots(int iChange)
{
	m_iNumStartingPlots += iChange;
	FAssert(getNumStartingPlots() >= 0);
}


bool CvArea::isWater() const
{
	return m_bWater;
}

bool CvArea::hasEurope() const
{
	for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

		if (pLoopPlot->getArea() == getID())
		{
			if (pLoopPlot->isEurope())
			{
				return true;
			}
		}
	}

	return false;
}

int CvArea::getUnitsPerPlayer(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	return m_aiUnitsPerPlayer[eIndex];
}


void CvArea::changeUnitsPerPlayer(PlayerTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	m_iNumUnits += iChange;
	FAssert(getNumUnits() >= 0);
	m_aiUnitsPerPlayer[eIndex] += iChange;
	FAssert(getUnitsPerPlayer(eIndex) >= 0);
}


int CvArea::getCitiesPerPlayer(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	return m_aiCitiesPerPlayer[eIndex];
}


void CvArea::changeCitiesPerPlayer(PlayerTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	m_iNumCities = (m_iNumCities + iChange);
	FAssert(getNumCities() >= 0);
	m_aiCitiesPerPlayer[eIndex] += iChange;
	FAssert(getCitiesPerPlayer(eIndex) >= 0);
}


int CvArea::getPopulationPerPlayer(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	return m_aiPopulationPerPlayer[eIndex];
}


void CvArea::changePopulationPerPlayer(PlayerTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	m_aiPopulationPerPlayer[eIndex] += iChange;
	FAssert(getPopulationPerPlayer(eIndex) >= 0);
}


int CvArea::getPower(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	return m_aiPower[eIndex];
}


void CvArea::changePower(PlayerTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	m_aiPower[eIndex] += iChange;
	FAssert(getPower(eIndex) >= 0);
}


int CvArea::getBestFoundValue(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	return m_aiBestFoundValue[eIndex];
}


void CvArea::setBestFoundValue(PlayerTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	m_aiBestFoundValue[eIndex] = iNewValue;
	FAssert(getBestFoundValue(eIndex) >= 0);
}


int CvArea::getNumRevealedTiles(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	return m_aiNumRevealedTiles[eIndex];
}


int CvArea::getNumUnrevealedTiles(TeamTypes eIndex) const
{
	return (getNumTiles() - getNumRevealedTiles(eIndex));
}


void CvArea::changeNumRevealedTiles(TeamTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	m_aiNumRevealedTiles[eIndex] += iChange;
	FAssert(getNumRevealedTiles(eIndex) >= 0);
}


AreaAITypes CvArea::getAreaAIType(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be < MAX_TEAMS");
	return m_aeAreaAIType[eIndex];
}


void CvArea::setAreaAIType(TeamTypes eIndex, AreaAITypes eNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be < MAX_TEAMS");
	m_aeAreaAIType[eIndex] = eNewValue;
}


CvCity* CvArea::getTargetCity(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	return getCity(m_aTargetCities[eIndex]);
}


void CvArea::setTargetCity(PlayerTypes eIndex, CvCity* pNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");

	if (pNewValue != NULL)
	{
		m_aTargetCities[eIndex] = pNewValue->getIDInfo();
	}
	else
	{
		m_aTargetCities[eIndex].reset();
	}
}


int CvArea::getYieldRateModifier(PlayerTypes eIndex1, YieldTypes eIndex2) const
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be >= 0");
	FAssertMsg(eIndex1 < MAX_PLAYERS, "eIndex1 is expected to be < MAX_PLAYERS");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be >= 0");
	FAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be < NUM_YIELD_TYPES");
	return m_aaiYieldRateModifier[eIndex1][eIndex2];
}


void CvArea::changeYieldRateModifier(PlayerTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be >= 0");
	FAssertMsg(eIndex1 < MAX_PLAYERS, "eIndex1 is expected to be < MAX_PLAYERS");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be >= 0");
	FAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be < NUM_YIELD_TYPES");

	if (iChange != 0)
	{
		m_aaiYieldRateModifier[eIndex1][eIndex2] = (m_aaiYieldRateModifier[eIndex1][eIndex2] + iChange);

		GET_PLAYER(eIndex1).invalidateYieldRankCache(eIndex2);

		GET_PLAYER(eIndex1).AI_makeAssignWorkDirty();

		if (GET_PLAYER(eIndex1).getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}


int CvArea::getNumTrainAIUnits(PlayerTypes eIndex1, UnitAITypes eIndex2) const
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be >= 0");
	FAssertMsg(eIndex1 < MAX_PLAYERS, "eIndex1 is expected to be < MAX_PLAYERS");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be >= 0");
	FAssertMsg(eIndex2 < NUM_UNITAI_TYPES, "eIndex2 is expected to be < NUM_UNITAI_TYPES");
	return m_aaiNumTrainAIUnits[eIndex1][eIndex2];
}


void CvArea::changeNumTrainAIUnits(PlayerTypes eIndex1, UnitAITypes eIndex2, int iChange)
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be >= 0");
	FAssertMsg(eIndex1 < MAX_PLAYERS, "eIndex1 is expected to be < MAX_PLAYERS");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be >= 0");
	FAssertMsg(eIndex2 < NUM_UNITAI_TYPES, "eIndex2 is expected to be < NUM_UNITAI_TYPES");
	m_aaiNumTrainAIUnits[eIndex1][eIndex2] += iChange;
	FAssert(getNumTrainAIUnits(eIndex1, eIndex2) >= 0);
}


int CvArea::getNumAIUnits(PlayerTypes eIndex1, UnitAITypes eIndex2) const
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be >= 0");
	FAssertMsg(eIndex1 < MAX_PLAYERS, "eIndex1 is expected to be < MAX_PLAYERS");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be >= 0");
	FAssertMsg(eIndex2 < NUM_UNITAI_TYPES, "eIndex2 is expected to be < NUM_UNITAI_TYPES");
	return m_aaiNumAIUnits[eIndex1][eIndex2];
}


void CvArea::changeNumAIUnits(PlayerTypes eIndex1, UnitAITypes eIndex2, int iChange)
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be >= 0");
	FAssertMsg(eIndex1 < MAX_PLAYERS, "eIndex1 is expected to be < MAX_PLAYERS");
	if (eIndex2 != NO_UNITAI)
	{
		m_aaiNumAIUnits[eIndex1][eIndex2] += iChange;
		FAssert(getNumAIUnits(eIndex1, eIndex2) >= 0);
	}
}


int CvArea::getNumBonuses(BonusTypes eBonus) const
{
	return m_ja_iNumBonuses.get(eBonus);
}


int CvArea::getNumTotalBonuses() const
{
	int iTotal = 0;

	for (int iI = 0; iI < m_ja_iNumBonuses.length(); iI++)
	{
		iTotal += m_ja_iNumBonuses.get(iI);
	}

	return iTotal;
}


void CvArea::changeNumBonuses(BonusTypes eBonus, int iChange)
{
	m_ja_iNumBonuses.add(iChange, eBonus);
	FAssert(getNumBonuses(eBonus) >= 0);
}


int CvArea::getNumImprovements(ImprovementTypes eImprovement) const
{
	return m_ja_iNumImprovements.get(eImprovement);
}


void CvArea::changeNumImprovements(ImprovementTypes eImprovement, int iChange)
{
	m_ja_iNumImprovements.add(iChange, eImprovement);
	FAssert(getNumImprovements(eImprovement) >= 0);
}


void CvArea::read(FDataStreamBase* pStream)
{
	int iI;

	// Init saved data
	reset();

	uint uiFlag=0;
	pStream->Read(&uiFlag);	// flags for expansion

	pStream->Read(&m_iID);
	pStream->Read(&m_iNumTiles);
	pStream->Read(&m_iNumOwnedTiles);
	pStream->Read(&m_iNumRiverEdges);
	pStream->Read(&m_iNumUnits);
	pStream->Read(&m_iNumCities);
	pStream->Read(&m_iNumStartingPlots);

	pStream->Read(&m_bWater);

	pStream->Read(MAX_PLAYERS, m_aiUnitsPerPlayer);
	pStream->Read(MAX_PLAYERS, m_aiCitiesPerPlayer);
	pStream->Read(MAX_PLAYERS, m_aiPopulationPerPlayer);
	pStream->Read(MAX_PLAYERS, m_aiPower);
	pStream->Read(MAX_PLAYERS, m_aiBestFoundValue);
	pStream->Read(MAX_TEAMS, m_aiNumRevealedTiles);

	pStream->Read(MAX_TEAMS, (int*)m_aeAreaAIType);

	for (iI=0;iI<MAX_PLAYERS;iI++)
	{
		m_aTargetCities[iI].read(pStream);
	}

	/// JIT array save - start - Nightinggale
	int iNumUnitAITypes = 0;
	pStream->Read(&iNumUnitAITypes);
	/// JIT array save - end - Nightinggale

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		pStream->Read(NUM_YIELD_TYPES, m_aaiYieldRateModifier[iI]);
	}
	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		pStream->Read(iNumUnitAITypes, m_aaiNumTrainAIUnits[iI]);
	}
	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		pStream->Read(iNumUnitAITypes, m_aaiNumAIUnits[iI]);
	}

	m_ja_iNumBonuses.read(pStream);
	m_ja_iNumImprovements.read(pStream);
}


void CvArea::write(FDataStreamBase* pStream)
{
	int iI;

	uint uiFlag=0;
	pStream->Write(uiFlag);		// flag for expansion

	pStream->Write(m_iID);
	pStream->Write(m_iNumTiles);
	pStream->Write(m_iNumOwnedTiles);
	pStream->Write(m_iNumRiverEdges);
	pStream->Write(m_iNumUnits);
	pStream->Write(m_iNumCities);
	pStream->Write(m_iNumStartingPlots);

	pStream->Write(m_bWater);

	pStream->Write(MAX_PLAYERS, m_aiUnitsPerPlayer);
	pStream->Write(MAX_PLAYERS, m_aiCitiesPerPlayer);
	pStream->Write(MAX_PLAYERS, m_aiPopulationPerPlayer);
	pStream->Write(MAX_PLAYERS, m_aiPower);
	pStream->Write(MAX_PLAYERS, m_aiBestFoundValue);
	pStream->Write(MAX_TEAMS, m_aiNumRevealedTiles);

	pStream->Write(MAX_TEAMS, (int*)m_aeAreaAIType);

	for (iI=0;iI<MAX_PLAYERS;iI++)
	{
		m_aTargetCities[iI].write(pStream);
	}

	/// JIT array save - start - Nightinggale
	pStream->Write(NUM_UNITAI_TYPES);
	/// JIT array save - end - Nightinggale

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		pStream->Write(NUM_YIELD_TYPES, m_aaiYieldRateModifier[iI]);
	}
	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		pStream->Write(NUM_UNITAI_TYPES, m_aaiNumTrainAIUnits[iI]);
	}
	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		pStream->Write(NUM_UNITAI_TYPES, m_aaiNumAIUnits[iI]);
	}
	m_ja_iNumBonuses.write(pStream);
	m_ja_iNumImprovements.write(pStream);
}

// Protected Functions...
