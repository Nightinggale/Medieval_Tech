// unit.cpp

#include "CvGameCoreDLL.h"
#include "CvUnit.h"
#include "CvArea.h"
#include "CvPlot.h"
#include "CvCity.h"
#include "CvGlobals.h"
#include "CvGameCoreUtils.h"
#include "CvGameAI.h"
#include "CvMap.h"
#include "CvPlayerAI.h"
#include "CvRandom.h"
#include "CvTeamAI.h"
#include "CvGameCoreUtils.h"
#include "CyUnit.h"
#include "CyArgsList.h"
#include "CyPlot.h"
#include "CvDLLEntityIFaceBase.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLEngineIFaceBase.h"
#include "CvDLLEventReporterIFaceBase.h"
#include "CvDLLPythonIFaceBase.h"
#include "CvDLLFAStarIFaceBase.h"
#include "CvInfos.h"
#include "FProfiler.h"
#include "CvPopupInfo.h"
#include "CvArtFileMgr.h"
#include "CvDiploParameters.h"
#include "CvTradeRoute.h"

#include "CvInfoProfessions.h"

// Public Functions...

CvUnitTemporaryStrengthModifier::CvUnitTemporaryStrengthModifier(CvUnit* pUnit, ProfessionTypes eProfession) :
	m_pUnit(pUnit),
	m_eProfession(eProfession)
{
	if (m_pUnit != NULL)
	{
		m_pUnit->processProfessionStats(m_pUnit->getProfession(), -1);
		m_pUnit->processProfessionStats(m_eProfession, 1);
	}
}

CvUnitTemporaryStrengthModifier::~CvUnitTemporaryStrengthModifier()
{
	if (m_pUnit != NULL)
	{
		m_pUnit->processProfessionStats(m_eProfession, -1);
		m_pUnit->processProfessionStats(m_pUnit->getProfession(), 1);
	}
}


CvUnit::CvUnit() :
	m_eUnitType(NO_UNIT),
	m_iID(-1)
{
	m_aiExtraDomainModifier = new int[NUM_DOMAIN_TYPES];


	CvDLLEntity::createUnitEntity(this);		// create and attach entity to unit

	reset(0, NO_UNIT, NO_PLAYER, true);
}


CvUnit::~CvUnit()
{
	if (!gDLL->GetDone() && GC.IsGraphicsInitialized())						// don't need to remove entity when the app is shutting down, or crash can occur
	{
		gDLL->getEntityIFace()->RemoveUnitFromBattle(this);
		CvDLLEntity::removeEntity();		// remove entity from engine
	}

	CvDLLEntity::destroyEntity();			// delete CvUnitEntity and detach from us

	uninit();

	SAFE_DELETE_ARRAY(m_aiExtraDomainModifier);
}

void CvUnit::reloadEntity()
{
	//has not been initialized so don't reload
	if(!gDLL->getEntityIFace()->isInitialized(getEntity()))
	{
		return;
	}

	bool bSelected = IsSelected();

	//destroy old entity
	if (!gDLL->GetDone() && GC.IsGraphicsInitialized())						// don't need to remove entity when the app is shutting down, or crash can occur
	{
		gDLL->getEntityIFace()->RemoveUnitFromBattle(this);
		CvDLLEntity::removeEntity();		// remove entity from engine
	}

	CvDLLEntity::destroyEntity();			// delete CvUnitEntity and detach from us

	//creat new one
	CvDLLEntity::createUnitEntity(this);		// create and attach entity to unit
	setupGraphical();
	if (bSelected)
	{
		gDLL->getInterfaceIFace()->insertIntoSelectionList(this, false, false);
	}
}


void CvUnit::init(int iID, UnitTypes eUnit, ProfessionTypes eProfession, UnitAITypes eUnitAI, PlayerTypes eOwner, int iX, int iY, DirectionTypes eFacingDirection, int iYieldStored)
{
	CvWString szBuffer;
	int iUnitName;
	int iI, iJ;

	FAssert(NO_UNIT != eUnit);

	//--------------------------------
	// Init saved data
	reset(iID, eUnit, eOwner);

	m_iYieldStored = iYieldStored;
	m_eFacingDirection = eFacingDirection;
	if(m_eFacingDirection == NO_DIRECTION)
	{
		CvPlot* pPlot = GC.getMapINLINE().plotINLINE(iX, iY);
		if((pPlot != NULL) && pPlot->isWater() && (getDomainType() == DOMAIN_SEA))
		{
			m_eFacingDirection = (DirectionTypes) GC.getXMLval(XML_WATER_UNIT_FACING_DIRECTION);
		}
		else
		{
			m_eFacingDirection = DIRECTION_SOUTH;
		}
	}

	iUnitName = GC.getGameINLINE().getUnitCreatedCount(getUnitType());
	int iNumNames = m_pUnitInfo->getNumUnitNames();
	if (iUnitName < iNumNames)
	{
		int iOffset = GC.getGameINLINE().getSorenRandNum(iNumNames, "Unit name selection");

		for (iI = 0; iI < iNumNames; iI++)
		{
			int iIndex = (iI + iOffset) % iNumNames;
			CvWString szName = gDLL->getText(m_pUnitInfo->getUnitNames(iIndex));
			if (!GC.getGameINLINE().isGreatGeneralBorn(szName))
			{
				setName(szName);
				GC.getGameINLINE().addGreatGeneralBornName(szName);
				break;
			}
		}
	}

	setGameTurnCreated(GC.getGameINLINE().getGameTurn());

	GC.getGameINLINE().incrementUnitCreatedCount(getUnitType());
	GC.getGameINLINE().incrementUnitClassCreatedCount((UnitClassTypes)(m_pUnitInfo->getUnitClassType()));

	updateOwnerCache(1);

	for (iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		if (m_pUnitInfo->getFreePromotions(iI))
		{
			setHasRealPromotion(((PromotionTypes)iI), true);
			//TKs Civilian Promo
			if (GC.getPromotionInfo((PromotionTypes)iI).isCivilian())
			{
				processPromotion((PromotionTypes)iI, 1);
			}
		}
	}

	///TKs Med
    YieldTypes eYield = getYield();
    if (m_pUnitInfo->getFreeBuildingClass() == NO_BUILDINGCLASS)
    {
        setAddedFreeBuilding(true);
    }
    if (GC.getGameINLINE().isBarbarianPlayer(getOwner()))
    {
        setBarbarian(true);
    }
	///TKe

	if (NO_UNITCLASS != getUnitClassType())
	{
		for (iJ = 0; iJ < GC.getNumPromotionInfos(); iJ++)
		{
			if (GET_PLAYER(getOwnerINLINE()).isFreePromotion(getUnitClassType(), (PromotionTypes)iJ))
			{
				changeFreePromotionCount(((PromotionTypes)iJ), 1);
			}
		}
		///TKs Med Free Promotion
		for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
        {
            if (GET_PLAYER(getOwnerINLINE()).hasTrait((TraitTypes)iI))
            {
                for (int iJ = 0; iJ < GC.getNumPromotionInfos(); iJ++)
                {
                    if (GC.getTraitInfo((TraitTypes) iI).isFreePromotion(iJ))
                    {
                        if (GC.getTraitInfo((TraitTypes) iI).isFreePromotionUnitClass(getUnitClassType()))
                        {
                            changeFreePromotionCount(((PromotionTypes)iJ), 1);
                        }
                    }
                }
            }
        }
        ///TKe
	}

	processUnitCombatType((UnitCombatTypes) m_pUnitInfo->getUnitCombatType(), 1);

	updateBestLandCombat();

	AI_init();

    ///TKs Med
    FAssert(eProfession != INVALID_PROFESSION);
	setProfession(eProfession, true);
    ///Tke
	if (isNative() || GET_PLAYER(getOwnerINLINE()).isEurope())
	{
		std::vector<int> aiPromo(GC.getNumPromotionInfos(), 0);
		for (iI = 0; iI < GC.getNumPromotionInfos(); iI++)
		{
			aiPromo[iI] = iI;
		}
		GC.getGameINLINE().getSorenRand().shuffleArray(aiPromo, NULL);

		for (iI = 0; iI < GC.getNumPromotionInfos(); iI++)
		{
			PromotionTypes eLoopPromotion = (PromotionTypes)aiPromo[iI];
			if (canAcquirePromotion(eLoopPromotion))
			{
				if (GC.getPromotionInfo(eLoopPromotion).getPrereqPromotion() != NO_PROMOTION || GC.getPromotionInfo(eLoopPromotion).getPrereqOrPromotion1() != NO_PROMOTION)
				{
					if (GC.getGameINLINE().getSorenRandNum(100, "AI free native/europe promotion") < 25)
					{
						setHasRealPromotion(eLoopPromotion, true);
						break;
					}
				}
			}
		}
	}

	addToMap(iX, iY);
	AI_setUnitAIType(eUnitAI);

	gDLL->getEventReporterIFace()->unitCreated(this);

	FAssert(GET_PLAYER(getOwnerINLINE()).checkPopulation());
}


void CvUnit::uninit()
{
	m_ja_bHasRealPromotion.resetContent();
	m_ja_iFreePromotionCount.resetContent();
}


// FUNCTION: reset()
// Initializes data members that are serialized.
void CvUnit::reset(int iID, UnitTypes eUnit, PlayerTypes eOwner, bool bConstructorCall)
{
	int iI;

	//--------------------------------
	// Uninit class
	uninit();

	m_iID = iID;
	m_iGroupID = FFreeList::INVALID_INDEX;
	m_iHotKeyNumber = -1;
	m_iX = INVALID_PLOT_COORD;
	m_iY = INVALID_PLOT_COORD;
	m_iLastMoveTurn = 0;
	m_iGameTurnCreated = 0;
	m_iDamage = 0;
	m_iMoves = 0;
	m_iExperience = 0;
	m_iLevel = 1;
	m_iCargo = 0;
	m_iAttackPlotX = INVALID_PLOT_COORD;
	m_iAttackPlotY = INVALID_PLOT_COORD;
	m_iCombatTimer = 0;
	
	m_iCombatDamage = 0;
	m_iFortifyTurns = 0;
	m_iBlitzCount = 0;
	m_iAmphibCount = 0;
	m_iRiverCount = 0;
	m_iEnemyRouteCount = 0;
	m_iAlwaysHealCount = 0;
	m_iHillsDoubleMoveCount = 0;
	m_iExtraVisibilityRange = 0;
	m_iExtraMoves = 0;
	m_iExtraMoveDiscount = 0;
	m_iExtraWithdrawal = 0;
	m_iExtraBombardRate = 0;
	m_iExtraEnemyHeal = 0;
	m_iExtraNeutralHeal = 0;
	m_iExtraFriendlyHeal = 0;
	m_iSameTileHeal = 0;
	m_iAdjacentTileHeal = 0;
	m_iExtraCombatPercent = 0;
	m_iExtraCityAttackPercent = 0;
	m_iExtraCityDefensePercent = 0;
	m_iExtraHillsAttackPercent = 0;
	m_iExtraHillsDefensePercent = 0;
	m_iPillageChange = 0;
	m_iUpgradeDiscount = 0;
	m_iExperiencePercent = 0;
	m_eFacingDirection = DIRECTION_SOUTH;
	m_iImmobileTimer = 0;
	m_iYieldStored = 0;
	m_iExtraWorkRate = 0;
	m_iUnitTravelTimer = 0;
	m_iBadCityDefenderCount = 0;
	m_iUnarmedCount = 0;
	//TKs New Promotion Effects
	m_iPlotWorkedBonus = 0;
	m_iBuildingWorkedBonus = 0;
    ///TK Med
    m_iInvisibleTimer = 0;
    m_iTravelPlotX = INVALID_PLOT_COORD;
	m_iTravelPlotY = INVALID_PLOT_COORD;
	m_iCombatFirstStrikes = 0;
	m_iTrainCounter = 0;
	m_iTraderCode = 0;
	m_iImmuneToFirstStrikesCount = 0;
	m_iExtraFirstStrikes = 0;
	m_iExtraChanceFirstStrikes = 0;
	m_iCombatBlockParrys = 0;
	m_iEscortPromotion = -1;
	m_bCrushingBlows = false;
	m_bGlancingBlows = false;
	m_bFreeBuilding = false;
	//m_iEventTimer = 0;
	//m_iEventTimerPromotion = NO_PROMOTION;

	///TKe
	m_bMadeAttack = false;
	m_bPromotionReady = false;
	m_bDeathDelay = false;
	m_bCombatFocus = false;
	m_bInfoBarDirty = false;
	m_bColonistLocked = false;
	// < JAnimals Mod Start >
	m_bBarbarian = false;
	// < JAnimals Mod End >

	m_eOwner = eOwner;
	m_eCapturingPlayer = NO_PLAYER;
	m_eUnitType = eUnit;
	m_pUnitInfo = (NO_UNIT != m_eUnitType) ? &GC.getUnitInfo(m_eUnitType) : NULL;
	m_iBaseCombat = (NO_UNIT != m_eUnitType) ? m_pUnitInfo->getCombat() : 0;
	m_eLeaderUnitType = NO_UNIT;
	m_iCargoCapacity = (NO_UNIT != m_eUnitType) ? m_pUnitInfo->getCargoSpace() : 0;
	m_eProfession = NO_PROFESSION;
	m_eUnitTravelState = NO_UNIT_TRAVEL_STATE;
    ///TKs Med **TradeRoute**
	m_eUnitTradeMarket = NO_EUROPE;
	m_ConvertToUnit = NO_UNIT;
	///Tke
	m_combatUnit.reset();

	m_transportUnit.reset();
	m_homeCity.reset();
	m_iPostCombatPlotIndex = -1;

	for (iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		m_aiExtraDomainModifier[iI] = 0;
	}

	clear(m_szName);
	m_szScriptData ="";

	if (!bConstructorCall)
	{
		FAssertMsg((0 < GC.getNumPromotionInfos()), "GC.getNumPromotionInfos() is not greater than zero but an array is being allocated in CvUnit::reset");
		m_ja_bHasRealPromotion.resetContent();
		m_ja_iFreePromotionCount.resetContent();

		m_ja_iTerrainDoubleMoveCount.resetContent();
		m_ja_iFeatureDoubleMoveCount.resetContent();
		m_ja_iExtraTerrainAttackPercent.resetContent();
		m_ja_iExtraTerrainDefensePercent.resetContent();
		m_ja_iExtraFeatureAttackPercent.resetContent();
		m_ja_iExtraFeatureDefensePercent.resetContent();
		m_ja_iExtraUnitClassAttackModifier.resetContent();
		m_ja_iExtraUnitClassDefenseModifier.resetContent();
		m_ja_iExtraUnitCombatModifier.resetContent();

		AI_reset();
	}
}


//////////////////////////////////////
// graphical only setup
//////////////////////////////////////
void CvUnit::setupGraphical()
{
	if (!GC.IsGraphicsInitialized())
	{
		return;
	}

	CvDLLEntity::setup();
}


void CvUnit::convert(CvUnit* pUnit, bool bKill)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pTransportUnit;
	CvUnit* pLoopUnit;

	setGameTurnCreated(pUnit->getGameTurnCreated());
	setDamage(pUnit->getDamage());
	setMoves(pUnit->getMoves());
	setYieldStored(pUnit->getYieldStored());
	setFacingDirection(pUnit->getFacingDirection(false));

	setLevel(pUnit->getLevel());
	int iOldModifier = std::max(1, 100 + GET_PLAYER(pUnit->getOwnerINLINE()).getLevelExperienceModifier());
	int iOurModifier = std::max(1, 100 + GET_PLAYER(getOwnerINLINE()).getLevelExperienceModifier());
	setExperience(std::max(0, (pUnit->getExperience() * iOurModifier) / iOldModifier));

	setName(pUnit->getNameNoDesc());
	setLeaderUnitType(pUnit->getLeaderUnitType());
	if (bKill)
	{
		ProfessionTypes eProfession = pUnit->getProfession();
		CvCity* pCity = pUnit->getCity();
		if (pCity != NULL)
		{
			pCity->AI_setWorkforceHack(true);
		}
		pUnit->setProfession(NO_PROFESSION);  // leave equipment behind
		setProfession(eProfession, true);
		if (pCity != NULL)
		{
			pCity->AI_setWorkforceHack(false);
		}
	}
	setUnitTravelState(pUnit->getUnitTravelState(), false);
	setUnitTravelTimer(pUnit->getUnitTravelTimer());

	for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		PromotionTypes ePromotion = (PromotionTypes) iI;
		if (pUnit->isHasRealPromotion(ePromotion))
		{
			setHasRealPromotion(ePromotion, true);
		}
	}

	pTransportUnit = pUnit->getTransportUnit();

	bool bAlive = true;
	if (pTransportUnit != NULL)
	{
		pUnit->setTransportUnit(NULL, false);
		bAlive = setTransportUnit(pTransportUnit);
	}

	if (bAlive)
	{
		if (pUnit->IsSelected() && isOnMap() && getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->insertIntoSelectionList(this, true, false);
		}
	}

	CvPlot* pPlot = pUnit->plot();
	if (pPlot != NULL)
	{
		if (bAlive)
		{
			pUnitNode = pPlot->headUnitNode();

			while (pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = pPlot->nextUnitNode(pUnitNode);

				if (pLoopUnit->getTransportUnit() == pUnit)
				{
					pLoopUnit->setTransportUnit(this);
				}
			}
		}

		if (bKill)
		{
			pUnit->kill(true);
		}
	}
	else //off map
	{
		if (bKill)
		{
			pUnit->updateOwnerCache(-1);
			SAFE_DELETE(pUnit);
		}
	}
}


void CvUnit::kill(bool bDelay, CvUnit* pAttacker)
{
	PROFILE_FUNC();

	CvWString szBuffer;

	CvPlot* pPlot = plot();
	FAssertMsg(pPlot != NULL, "Plot is not assigned a valid value");
	FAssert(GET_PLAYER(getOwnerINLINE()).checkPopulation());

	static std::vector<IDInfo> oldUnits;
	oldUnits.erase(oldUnits.begin(), oldUnits.end());
	CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();

	while (pUnitNode != NULL)
	{
		oldUnits.push_back(pUnitNode->m_data);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);
	}

	for(int i=0;i<(int)oldUnits.size();i++)
	{
		CvUnit* pLoopUnit = ::getUnit(oldUnits[i]);

		if (pLoopUnit != NULL)
		{
			if (pLoopUnit->getTransportUnit() == this)
			{
				//save old units because kill will clear the static list
				std::vector<IDInfo> tempUnits = oldUnits;

				if (pPlot->isValidDomainForLocation(*pLoopUnit))
				{
					pLoopUnit->setCapturingPlayer(getCapturingPlayer());
				}

				if (pLoopUnit->getCapturingPlayer() == NO_PLAYER)
				{
					if (pAttacker != NULL && pAttacker->getUnitInfo().isCapturesCargo())
					{
						pLoopUnit->setCapturingPlayer(pAttacker->getOwnerINLINE());
					}
				}

				pLoopUnit->kill(false, pAttacker);

				oldUnits = tempUnits;
			}
		}
	}

	if (pAttacker != NULL)
	{
		gDLL->getEventReporterIFace()->unitKilled(this, pAttacker->getOwnerINLINE());
        ///TKs Med
//		if (NO_UNIT != getLeaderUnitType())
//		{
//			for (int iI = 0; iI < MAX_PLAYERS; iI++)
//			{
//				if (GET_PLAYER((PlayerTypes)iI).isAlive())
//				{
//					szBuffer = gDLL->getText("TXT_KEY_MISC_GENERAL_KILLED", getNameKey());
//					gDLL->getInterfaceIFace()->addMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, GC.getEraInfo(GC.getGameINLINE().getCurrentEra()).getAudioUnitDefeatScript(), MESSAGE_TYPE_MAJOR_EVENT);
//				}
//			}
//		}
		///TKe
	}

	if (bDelay)
	{
		startDelayedDeath();
		return;
	}

	finishMoves();

	int iYieldStored = getYieldStored();
	setYieldStored(0);

	removeFromMap();
	updateOwnerCache(-1);

	PlayerTypes eOwner = getOwnerINLINE();
	PlayerTypes eCapturingPlayer = getCapturingPlayer();
	UnitTypes eCaptureUnitType = NO_UNIT;
	ProfessionTypes eCaptureProfession = getProfession();
	FAssert(eCaptureProfession == NO_PROFESSION || !GC.getProfessionInfo(eCaptureProfession).isCitizen());
	if (eCapturingPlayer != NO_PLAYER)
	{
		eCaptureUnitType = getCaptureUnitType(GET_PLAYER(eCapturingPlayer).getCivilizationType());
	}
	YieldTypes eYield = getYield();

	gDLL->getEventReporterIFace()->unitLost(this);

    GET_PLAYER(getOwnerINLINE()).AI_removeUnitFromMoveQueue(this);
	GET_PLAYER(getOwnerINLINE()).deleteUnit(getID());

	FAssert(GET_PLAYER(eOwner).checkPopulation());

	if ((eCapturingPlayer != NO_PLAYER) && (eCaptureUnitType != NO_UNIT))
	{
		if (GET_PLAYER(eCapturingPlayer).isHuman() || GET_PLAYER(eCapturingPlayer).AI_captureUnit(eCaptureUnitType, pPlot) || 0 == GC.getXMLval(XML_AI_CAN_DISBAND_UNITS))
		{
			if (!GET_PLAYER(eCapturingPlayer).isProfessionValid(eCaptureProfession, eCaptureUnitType))
			{
				eCaptureProfession = (ProfessionTypes) GC.getUnitInfo(eCaptureUnitType).getDefaultProfession();
			}
			CvUnit* pkCapturedUnit = GET_PLAYER(eCapturingPlayer).initUnit(eCaptureUnitType, eCaptureProfession, pPlot->getX_INLINE(), pPlot->getY_INLINE(), NO_UNITAI, NO_DIRECTION, iYieldStored);
			if (pkCapturedUnit != NULL)
			{
				bool bAlive = true;
				if (pAttacker != NULL && pAttacker->getUnitInfo().isCapturesCargo())
				{
				    if (pkCapturedUnit->getDomainType() == DOMAIN_LAND && pAttacker->getDomainType() == DOMAIN_SEA)
                    {
                        pkCapturedUnit->setXY(pAttacker->getX_INLINE(), pAttacker->getY_INLINE());
                        pkCapturedUnit->setTransportUnit(pAttacker);
                        if(pkCapturedUnit->getTransportUnit() == NULL) //failed to load
                        {
                            bAlive = false;
                            pkCapturedUnit->kill(false);
                        }
                    }
                    else
                    {
                        pkCapturedUnit->setXY(pAttacker->getX_INLINE(), pAttacker->getY_INLINE());

                        if(pkCapturedUnit->getTransportUnit() == NULL) //failed to load
                        {
                            bAlive = false;
                            pkCapturedUnit->kill(false);
                        }
                    }
				}
				else if (pAttacker != NULL)
				{

                    YieldTypes eCapturedYield = (YieldTypes)pkCapturedUnit->getYield();
					bool bCaptured = false;
                    if (eCapturedYield != NO_YIELD)
                    {
                        int YieldStored = pkCapturedUnit->getYieldStored();
                        if (pAttacker->cargoSpace() > 0)
                        {
                            if (YieldStored <= 0 && pAttacker->getProfession() != NO_PROFESSION)
                            {
								for (int iI=0;iI < GC.getProfessionInfo(pAttacker->getProfession()).getNumCaptureCargoTypes(); iI++)
								{
									if ((UnitClassTypes)GC.getProfessionInfo(pAttacker->getProfession()).getCaptureCargoTypes(iI) == GC.getUnitInfo(eCaptureUnitType).getUnitCaptureClassType())
									{
									   YieldStored = GC.getProfessionInfo(pAttacker->getProfession()).getCaptureCargoTypeAmount(iI);
									   bCaptured = true;
									   break;
									}
								}
                            }

							if (bCaptured)
							{
                                YieldStored = (GC.getGameINLINE().getSorenRandNum(YieldStored, "Random Yield Stored") + 1);
								YieldStored *= std::max(1, (GET_PLAYER(pAttacker->getOwnerINLINE()).getHuntingYieldPercent() + 100));
								YieldStored /= 100;
                                pkCapturedUnit->setYieldStored(YieldStored);
								pkCapturedUnit->setXY(pAttacker->getX_INLINE(), pAttacker->getY_INLINE());
								pkCapturedUnit->setTransportUnit(pAttacker);
								if(pkCapturedUnit->getTransportUnit() != NULL || pkCapturedUnit->isDelayedDeath())
								{

									szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_CAPTURED_ANIMAL_CARGO", YieldStored, GC.getUnitInfo(eCaptureUnitType).getTextKeyWide(), GC.getUnitInfo(eCaptureUnitType).getTextKeyWide());
									gDLL->getInterfaceIFace()->addMessage(eCapturingPlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_UNITCAPTURE", MESSAGE_TYPE_INFO, pkCapturedUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pPlot->getX_INLINE(), pPlot->getY_INLINE());
								}
							}

                        }

                        if(pkCapturedUnit->getTransportUnit() == NULL) //failed to load
                        {
                            //if (pAttacker->getProfession() == (ProfessionTypes)GC.getXMLval(XML_DEFAULT_HUNTSMAN_PROFESSION))
                           // {
                                if (bCaptured && pAttacker->getLoadYieldAmount(eCapturedYield) == 0)
                                {
                                    szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_CAPTURED_CARGO_FULL", YieldStored, GC.getUnitInfo(eCaptureUnitType).getTextKeyWide(), GC.getUnitInfo(eCaptureUnitType).getTextKeyWide());
                                    gDLL->getInterfaceIFace()->addMessage(eCapturingPlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_UNITCAPTURE", MESSAGE_TYPE_INFO, pkCapturedUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pPlot->getX_INLINE(), pPlot->getY_INLINE());

                                }
                           // }

                            bAlive = false;
                            pkCapturedUnit->kill(false);
                        }

                    }
                    else if (pkCapturedUnit->getDomainType() == DOMAIN_LAND && pAttacker->getDomainType() == DOMAIN_SEA)
                    {
                        pkCapturedUnit->setXY(pAttacker->getX_INLINE(), pAttacker->getY_INLINE());
                        pkCapturedUnit->setTransportUnit(pAttacker);
                        if(pkCapturedUnit->getTransportUnit() == NULL) //failed to load
                        {
                            bAlive = false;
                            pkCapturedUnit->kill(false);
                        }
                    }
				}
				else if (pkCapturedUnit->getYield() != NO_YIELD)
				{
				    bAlive = false;
                    pkCapturedUnit->kill(false);
				}
				if (bAlive)
				{
				    YieldTypes eCapturedYield = (YieldTypes)pkCapturedUnit->getYield();
				    if (eCapturedYield == NO_YIELD)
				    {
				        szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_CAPTURED_UNIT", GC.getUnitInfo(eCaptureUnitType).getTextKeyWide());
                        gDLL->getInterfaceIFace()->addMessage(eCapturingPlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_UNITCAPTURE", MESSAGE_TYPE_INFO, pkCapturedUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pPlot->getX_INLINE(), pPlot->getY_INLINE());

				    }
				    else
					{
					    int YieldStored = pkCapturedUnit->getYieldStored();
//					    if (pAttacker != NULL && pAttacker->getProfession() == (ProfessionTypes)GC.getXMLval(XML_DEFAULT_HUNTSMAN_PROFESSION))
//                        {
//                            szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_CAPTURED_ANIMAL_CARGO", YieldStored, GC.getUnitInfo(eCaptureUnitType).getTextKeyWide());
//                            gDLL->getInterfaceIFace()->addMessage(eCapturingPlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_UNITCAPTURE", MESSAGE_TYPE_INFO, pkCapturedUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pPlot->getX_INLINE(), pPlot->getY_INLINE());
//                        }
//                        else
                        if (pAttacker->getDomainType() == DOMAIN_SEA)
                        {
                           szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_CAPTURED_CARGO", YieldStored, GC.getUnitInfo(eCaptureUnitType).getTextKeyWide());
                            gDLL->getInterfaceIFace()->addMessage(eCapturingPlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_UNITCAPTURE", MESSAGE_TYPE_INFO, pkCapturedUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pPlot->getX_INLINE(), pPlot->getY_INLINE());
                        }

					}
					if (!pkCapturedUnit->isCargo())
					{
						// Add a captured mission
						CvMissionDefinition kMission;
						kMission.setMissionTime(GC.getMissionInfo(MISSION_CAPTURED).getTime() * gDLL->getSecsPerTurn());
						kMission.setUnit(BATTLE_UNIT_ATTACKER, pkCapturedUnit);
						kMission.setUnit(BATTLE_UNIT_DEFENDER, NULL);
						kMission.setPlot(pPlot);
						kMission.setMissionType(MISSION_CAPTURED);
						gDLL->getEntityIFace()->AddMission(&kMission);
					}
					///TKe

					pkCapturedUnit->finishMoves();

					if (!GET_PLAYER(eCapturingPlayer).isHuman())
					{
						CvPlot* pPlot = pkCapturedUnit->plot();
						if (pPlot && !pPlot->isCity(false))
						{
							if (GET_PLAYER(eCapturingPlayer).AI_getPlotDanger(pPlot) && GC.getXMLval(XML_AI_CAN_DISBAND_UNITS))
							{
								pkCapturedUnit->kill(false);
							}
						}
					}
				}
			}
		}
	}
}

void CvUnit::removeFromMap()
{
	if ((getX_INLINE() != INVALID_PLOT_COORD) && (getY_INLINE() != INVALID_PLOT_COORD))
	{
		if (IsSelected())
		{
			if (gDLL->getInterfaceIFace()->getLengthSelectionList() == 1)
			{
				if (!(gDLL->getInterfaceIFace()->isFocused()) && !(gDLL->getInterfaceIFace()->isCitySelection()) && !(gDLL->getInterfaceIFace()->isDiploOrPopupWaiting()))
				{
					GC.getGameINLINE().updateSelectionList();
				}

				if (IsSelected())
				{
					gDLL->getInterfaceIFace()->setCycleSelectionCounter(1);
				}
				else
				{
					gDLL->getInterfaceIFace()->setDirty(SelectionCamera_DIRTY_BIT, true);
				}
			}
		}

		gDLL->getInterfaceIFace()->removeFromSelectionList(this);

		// XXX this is NOT a hack, without it, the game crashes.
		gDLL->getEntityIFace()->RemoveUnitFromBattle(this);

		plot()->setFlagDirty(true);

		FAssertMsg(!isCombat(), "isCombat did not return false as expected");

		CvUnit* pTransportUnit = getTransportUnit();

		if (pTransportUnit != NULL)
		{
			setTransportUnit(NULL);
		}

		AI_setMovePriority(0);

		FAssertMsg(getAttackPlot() == NULL, "The current unit instance's attack plot is expected to be NULL");
		FAssertMsg(getCombatUnit() == NULL, "The current unit instance's combat unit is expected to be NULL");

		if (!gDLL->GetDone() && GC.IsGraphicsInitialized())	// don't need to remove entity when the app is shutting down, or crash can occur
		{
			CvDLLEntity::removeEntity();		// remove entity from engine
		}

		CvDLLEntity::destroyEntity();
		CvDLLEntity::createUnitEntity(this);		// create and attach entity to unit
	}

	AI_setUnitAIType(NO_UNITAI);

	setXY(INVALID_PLOT_COORD, INVALID_PLOT_COORD, true);

	joinGroup(NULL, false, false);
}

void CvUnit::addToMap(int iPlotX, int iPlotY)
{
	if((iPlotX != INVALID_PLOT_COORD) && (iPlotY != INVALID_PLOT_COORD))
	{
		//--------------------------------
		// Init pre-setup() data
		setXY(iPlotX, iPlotY, false, false);

		//--------------------------------
		// Init non-saved data
		setupGraphical();

		//--------------------------------
		// Init other game data
		plot()->updateCenterUnit();

		plot()->setFlagDirty(true);
	}

	if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
	{
		gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
	}
}

void CvUnit::updateOwnerCache(int iChange)
{
	CvPlayer& kPlayer = GET_PLAYER(getOwnerINLINE());

	GET_TEAM(getTeam()).changeUnitClassCount(((UnitClassTypes)(m_pUnitInfo->getUnitClassType())), iChange);
	kPlayer.changeUnitClassCount(((UnitClassTypes)(m_pUnitInfo->getUnitClassType())), iChange);
	kPlayer.changeAssets(getAsset() * iChange);
	kPlayer.changePower(getPower() * iChange);
	CvArea* pArea = area();
	if (pArea != NULL)
	{
		pArea->changePower(getOwnerINLINE(), getPower() * iChange);
	}
	if (m_pUnitInfo->isFound())
	{
		GET_PLAYER(getOwnerINLINE()).changeTotalPopulation(iChange);
	}
}


void CvUnit::NotifyEntity(MissionTypes eMission)
{
	gDLL->getEntityIFace()->NotifyEntity(getUnitEntity(), eMission);
}


void CvUnit::doTurn()
{
	PROFILE_FUNC();
     ///TKs Invention Core Mod v 1.0
	if (isOnMap())
	{
	    //if (isAlwaysHostile(plot()))
     //   {
     //       //FAssert(m_pUnitInfo->getDefaultUnitAIType() == AI_getUnitAIType());
     //   }
	    if (getEscortPromotion() != NO_PROMOTION)
	    {
	        CvCity* pPlotCity = plot()->getPlotCity();
	        bool bEscortLeaves = false;
	        if (pPlotCity != NULL && pPlotCity->getOwnerINLINE() == getOwner())
	        {
                bEscortLeaves = true;
	        }
	        else if (GET_PLAYER(getOwnerINLINE()).getGold() < GC.getXMLval(XML_HIRE_GUARD_COST))
	        {
	            bEscortLeaves = true;
	        }
	        else
	        {
	            GET_PLAYER(getOwnerINLINE()).changeGold(-GC.getXMLval(XML_HIRE_GUARD_COST));
	        }

	        if (bEscortLeaves)
	        {
                PromotionTypes ePromotion = (PromotionTypes) GC.getXMLval(XML_HIRE_GUARD_PROMOTION);
                setHasRealPromotion(ePromotion, false);
                setEscortPromotion(NO_PROMOTION);
                for (int iPromotion = 0; iPromotion < GC.getNumPromotionInfos(); ++iPromotion)
                {
                    if (isHasPromotion((PromotionTypes) iPromotion))
                    {
                        if (GC.getPromotionInfo((PromotionTypes) iPromotion).getEscortUnitClass() != NO_UNITCLASS)
                        {
                            UnitTypes eEscortUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits((UnitClassTypes)GC.getPromotionInfo(ePromotion).getEscortUnitClass());

                            if (eEscortUnit != NO_UNIT)
                            {
                                m_eLeaderUnitType = eEscortUnit;
                            }
                        }
                    }
                }
                reloadEntity();
	        }

	    }
        ///TKs Med 2.1 This kills all this teams Marauders once they form an alliance
	    if (isAlwaysHostile(plot()))
	    {
	        if (isNative() && GET_PLAYER(getOwner()).getVassalOwner() != NO_PLAYER)
	        {
	            kill(true);
	        }
	    }

        if (getConvertToUnit() != NO_UNIT && !getGroup()->isAutomated())
        {
           int iRandConvert = GC.getGameINLINE().getSorenRandNum(100, "Random COnvert Unit");
           if (iRandConvert > 75)
           {
               CvUnit* pUnit = GET_PLAYER(getOwner()).initUnit(getConvertToUnit(), (ProfessionTypes)GC.getUnitInfo(getConvertToUnit()).getDefaultProfession(), getX_INLINE(), getY_INLINE(), AI_getUnitAIType());
               if (pUnit != NULL)
               {
                   pUnit->joinGroup(getGroup());
                   pUnit->convert(this,false);
                   kill(true);
                   return;
               }
           }
        }

	}
    ///TKe
	FAssertMsg(!isDead(), "isDead did not return false as expected");
	FAssert(getGroup() != NULL || GET_PLAYER(getOwnerINLINE()).getPopulationUnitCity(getID()) != NULL);
	//if (isNative())
   // {
    //    FAssert(getGroup() != NULL);
    //}
   // int iID = getID();
	testPromotionReady();
///Tks Med
    if (isOnMap())
    {
        if (hasMoved())
        {
            if (isAlwaysHeal())
            {
                doHeal();
            }
        }
        else
        {
            if (isHurt())
            {
                doHeal();
            }

            if (!isCargo())
            {
                changeFortifyTurns(1);
            }
        }

        bool bGuarding = (getImmobileTimer() > 0);
        changeImmobileTimer(-1);
        if (isBarbarian() && !isOnlyDefensive())
        {
            if (!bGuarding && getImmobileTimer() == 0 && plot()->getImprovementType() != NO_IMPROVEMENT)
            {
                if (GC.getImprovementInfo(plot()->getImprovementType()).isGoody())
                {
                    int iRand = GC.getGameINLINE().getSorenRandNum(GC.getXMLval(XML_ANIMAL_BANDITS_GUARD_GOODY_TIMER), "ANIMAL_BANDITS_GUARD_GOODY_TIMER");
                    changeImmobileTimer(iRand);
                }
            }
        }
    }

	if (getUnitTravelState() == UNIT_TRAVEL_STATE_LIVE_AMONG_NATIVES)
	{
		if (isHurt())
        {
            doHeal();
        }
	}

	doUnitTravelTimer();

	setMadeAttack(false);

	setMoves(0);
}
///Tke

void CvUnit::resolveCombat(CvUnit* pDefender, CvPlot* pPlot, CvBattleDefinition& kBattle)
{
	CombatDetails cdAttackerDetails;
	CombatDetails cdDefenderDetails;

	int iAttackerStrength = currCombatStr(NULL, NULL, &cdAttackerDetails);
	int iAttackerFirepower = currFirepower(NULL, NULL);
	int iDefenderStrength;
	int iAttackerDamage;
	int iDefenderDamage;
	int iDefenderOdds;

	getDefenderCombatValues(*pDefender, pPlot, iAttackerStrength, iAttackerFirepower, iDefenderOdds, iDefenderStrength, iAttackerDamage, iDefenderDamage, &cdDefenderDetails);

	if (isHuman() || pDefender->isHuman())
	{
		CyArgsList pyArgsCD;
		pyArgsCD.add(gDLL->getPythonIFace()->makePythonObject(&cdAttackerDetails));
		pyArgsCD.add(gDLL->getPythonIFace()->makePythonObject(&cdDefenderDetails));
		pyArgsCD.add(getCombatOdds(this, pDefender));
		gDLL->getEventReporterIFace()->genericEvent("combatLogCalc", pyArgsCD.makeFunctionArgs());
	}
    ///TK Med TODO this sets it up so that Units only gain XP and GG XP if level 1 when fighting Animals, should this only be in M:C?
    bool bGreatGeneralXP = true;
    if (m_pUnitInfo->isAnimal() || GC.getUnitInfo(pDefender->getUnitType()).isAnimal() || m_pUnitInfo->isHiddenNationality() || pDefender->getUnitInfo().isHiddenNationality())
    {
        bGreatGeneralXP = false;
    }
    bool bCombatXP = true;
    if (GC.getUnitInfo(pDefender->getUnitType()).isAnimal())
    {
        if (getLevel() >= GC.getXMLval(XML_MAX_LEVEL_FROM_ANIMAL_XP))
        {
            bCombatXP = false;
        }
    }

    int iFinalDamage = 0;
	int iBattleEffect = 0;
	int iAttackerFirstStrikeChance = 0;
	int iDefenderFirstStrikeChance = 0;
	bool bAttackHasBeenHit = false;
	bool bDefenderHasBeenHit = false;
	//bEvade sets it up so that Defensive Units are not killed by Bandits/Animals/Marauders but are sent to towns wouded instead.
	//TODO should this be turned on or off my XML values and or PlayerOptions/Difficulty? 
    bool bEvade = (pDefender->m_pUnitInfo->isFound() && (!pDefender->canAttack() || GC.getProfessionInfo(pDefender->getProfession()).isScout() || GC.getProfessionInfo(pDefender->getProfession()).getWorkRate() > 0));
	while (true)
	{
		iBattleEffect = 0;
		if (GC.getGameINLINE().getSorenRandNum(GC.getXMLval(XML_COMBAT_DIE_SIDES), "Combat") < iDefenderOdds)
		{
		    if (getCombatFirstStrikes() == 0)\
			{
			    if (getCombatBlockParrys() == 0)
			    {
                    if (getDamage() + iAttackerDamage >= maxHitPoints())
                    {
                        ///TKs Med
                        if (GC.getGameINLINE().getSorenRandNum(100, "Withdrawal") < withdrawalProbability() && bCombatXP)
                        {
                            changeExperience(GC.getXMLval(XML_EXPERIENCE_FROM_WITHDRAWL), pDefender->maxXPValue(), true, pPlot->getOwnerINLINE() == getOwnerINLINE(), bGreatGeneralXP);
                            break;
                        }
						// TODO finish the Ransom Knight Code
                        if (GET_PLAYER(pDefender->getOwnerINLINE()).isOption(PLAYEROPTION_MODDER_4) || GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_MODDER_4))
                        {
                            if (m_pUnitInfo->getKnightDubbingWeight() == -1 || isHasRealPromotion((PromotionTypes)GC.getXMLval(XML_DEFAULT_KNIGHT_PROMOTION)))
                            {
                                if (!pDefender->getUnitInfo().isAnimal() && !GET_PLAYER(pDefender->getOwnerINLINE()).isEurope())
                                {
                                    CvCity* pCity = getEvasionCity();
                                    FAssert(pCity != NULL);
                                    if (pCity != NULL)
                                    {
                                        changeTraderCode(1);
                                        setPostCombatPlot(pCity->getX_INLINE(), pCity->getY_INLINE());
                                        break;
                                    }
                                }
                            }
                        }

                        if (GC.getGameINLINE().getSorenRandNum(100, "Evasion") < getEvasionProbability(*pDefender))
                        {
                            // evasion
                            CvCity* pCity = getEvasionCity();
                            FAssert(pCity != NULL);
                            if (pCity != NULL)
                            {
                                setPostCombatPlot(pCity->getX_INLINE(), pCity->getY_INLINE());
                                break;
                            }
                        }


                    }
					bAttackHasBeenHit = true;
                    iFinalDamage = iAttackerDamage;
                    if (pDefender->isCombatCrushingBlow())
                    {
                        if (GC.getGameINLINE().getSorenRandNum(GC.getXMLval(XML_COMBAT_DIE_SIDES), "CrushingBlow") < iDefenderOdds)
                        {
							iBattleEffect = 1;
							iFinalDamage *= 2;
                        }
                    }
                    else if (pDefender->isCombatGlancingBlow())
                    {
                        if (GC.getGameINLINE().getSorenRandNum(GC.getXMLval(XML_COMBAT_DIE_SIDES), "GlancingBlow") >= iDefenderOdds)
                        {
							iBattleEffect = 2;
							iFinalDamage /= 2;
                        }
                    }

                    changeDamage(iFinalDamage, pDefender);


                    if (pDefender->getCombatFirstStrikes() > 0 && pDefender->isRanged())
                    {

                        //kBattle.addFirstStrikes(BATTLE_UNIT_DEFENDER, 1);
                        kBattle.addDamage(BATTLE_UNIT_ATTACKER, BATTLE_TIME_RANGED, iFinalDamage);
                    }

					if (iDefenderFirstStrikeChance > 0 && !bDefenderHasBeenHit)
					{
						iBattleEffect = 4;
						iDefenderFirstStrikeChance = 0;
					}

                    cdAttackerDetails.iCurrHitPoints = currHitPoints();

                    if (isHuman() || pDefender->isHuman())
                    {
                        CyArgsList pyArgs;
                        pyArgs.add(gDLL->getPythonIFace()->makePythonObject(&cdAttackerDetails));
                        pyArgs.add(gDLL->getPythonIFace()->makePythonObject(&cdDefenderDetails));
                        pyArgs.add(1);
                        pyArgs.add(iFinalDamage);
						pyArgs.add(iBattleEffect);
                        gDLL->getEventReporterIFace()->genericEvent("combatLogHit", pyArgs.makeFunctionArgs());
                    }
			    }
			    else
			    {
					if (isHuman() || pDefender->isHuman())
					{
						CyArgsList pyArgs;
						pyArgs.add(gDLL->getPythonIFace()->makePythonObject(&cdAttackerDetails));
						pyArgs.add(gDLL->getPythonIFace()->makePythonObject(&cdDefenderDetails));
						pyArgs.add(1);
						pyArgs.add(iAttackerDamage);
						pyArgs.add(3);
						gDLL->getEventReporterIFace()->genericEvent("combatLogHit", pyArgs.makeFunctionArgs());
					}
					setCombatBlockParrys(getCombatBlockParrys() - 1);
			    }
			}
			else
			{
				iAttackerFirstStrikeChance++;
			}
		}
		else
		{
		    if (pDefender->getCombatFirstStrikes() == 0)
			{
			    if (pDefender->getCombatBlockParrys() == 0)
			    {
			        //bool bRansomed = false;
                    if (pDefender->getDamage() + iDefenderDamage >= pDefender->maxHitPoints())
			        {
			            if (GET_PLAYER(pDefender->getOwnerINLINE()).isOption(PLAYEROPTION_MODDER_4) || GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_MODDER_4))
                        {
                            if (pDefender->getUnitInfo().getKnightDubbingWeight() == -1 || pDefender->isHasRealPromotion((PromotionTypes)GC.getXMLval(XML_DEFAULT_KNIGHT_PROMOTION)))
                            {
                                if (!m_pUnitInfo->isAnimal() && !GET_PLAYER(getOwnerINLINE()).isEurope())
                                {
                                    CvCity* pCity = pDefender->getEvasionCity();
                                    FAssert(pCity != NULL);
                                    if (pCity != NULL)
                                    {
                                        pDefender->changeTraderCode(1);
                                        pDefender->setPostCombatPlot(pCity->getX_INLINE(), pCity->getY_INLINE());
                                        break;
                                    }
                                }
                            }
                        }

                        if ((isAlwaysHostile(pPlot) || isBarbarian()) && !pPlot->isCity(false, pDefender->getTeam()) && bEvade)
                        {
                            CvCity* pCity = pDefender->getEvasionCity(1);
                            FAssert(pCity != NULL);
                            if (pCity != NULL)
                            {
                                if (!m_pUnitInfo->isAnimal())
                                {
                                    pDefender->changeTraderCode(3);
                                }
                                //pPlot = pDefender->plot();
                                CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
								CvUnit*  pLoopUnit;
                                while (pUnitNode != NULL)
                                {
                                    pLoopUnit = ::getUnit(pUnitNode->m_data);
                                    pUnitNode = pPlot->nextUnitNode(pUnitNode);

                                    if (pLoopUnit->getTransportUnit() == pDefender)
                                    {
                                        CvPlayer& kOwner = GET_PLAYER(pDefender->getOwnerINLINE());
                                        if (kOwner.getParent() != NO_PLAYER && pLoopUnit->getYield() != NO_YIELD )
                                        {
                                            CvPlayer& kParent = GET_PLAYER(kOwner.getParent());
                                            setYieldStored(kParent.getYieldBuyPrice(pLoopUnit->getYield()) * pLoopUnit->getYieldStored() + getYieldStored());
                                        }
                                        pLoopUnit->kill(true);
                                    }
                                }

                                pDefender->setPostCombatPlot(pCity->getX_INLINE(), pCity->getY_INLINE());
                                break;
                            }

                        }

                        if (GC.getGameINLINE().getSorenRandNum(100, "Evasion") < pDefender->getEvasionProbability(*this))
                        {
                            // evasion
                            CvCity* pCity = pDefender->getEvasionCity();
                            FAssert(pCity != NULL);
                            if (pCity != NULL)
                            {
                                pDefender->setPostCombatPlot(pCity->getX_INLINE(), pCity->getY_INLINE());
                                break;
                            }
                        }

			        }
                     ///Tke

                    iFinalDamage = iDefenderDamage;
                    if (isCombatCrushingBlow())
                    {
                        if (GC.getGameINLINE().getSorenRandNum(GC.getXMLval(XML_COMBAT_DIE_SIDES), "DefenderCrushingBlow") >= iDefenderOdds)
                        {
							iBattleEffect = 1;
                            iFinalDamage *= 2;
                        }
                    }
                    else if (isCombatGlancingBlow())
                    {
                        if (GC.getGameINLINE().getSorenRandNum(GC.getXMLval(XML_COMBAT_DIE_SIDES), "DefenderGlancingBlow") < iDefenderOdds)
                        {
							iBattleEffect = 2;
                            iFinalDamage /= 2;
                        }
                    }
					bDefenderHasBeenHit = true;
                    pDefender->changeDamage(iFinalDamage, this);

                    if (getCombatFirstStrikes() > 0 && isRanged())
                    {
                        //kBattle.addFirstStrikes(BATTLE_UNIT_ATTACKER, 1);
                        kBattle.addDamage(BATTLE_UNIT_DEFENDER, BATTLE_TIME_RANGED, iFinalDamage);
                    }

					if (iAttackerFirstStrikeChance > 0 && !bAttackHasBeenHit)
					{
						iBattleEffect = 4;
						iAttackerFirstStrikeChance = 0;
					}

                    cdDefenderDetails.iCurrHitPoints=pDefender->currHitPoints();

                    if (isHuman() || pDefender->isHuman())
                    {
                        CyArgsList pyArgs;
                        pyArgs.add(gDLL->getPythonIFace()->makePythonObject(&cdAttackerDetails));
                        pyArgs.add(gDLL->getPythonIFace()->makePythonObject(&cdDefenderDetails));
                        pyArgs.add(0);
                        pyArgs.add(iFinalDamage);
						pyArgs.add(iBattleEffect);
                        gDLL->getEventReporterIFace()->genericEvent("combatLogHit", pyArgs.makeFunctionArgs());
                    }
			    }
			    else
			    {
					if (isHuman() || pDefender->isHuman())
                    {
                        CyArgsList pyArgs;
                        pyArgs.add(gDLL->getPythonIFace()->makePythonObject(&cdAttackerDetails));
                        pyArgs.add(gDLL->getPythonIFace()->makePythonObject(&cdDefenderDetails));
                        pyArgs.add(0);
                        pyArgs.add(iDefenderDamage);
						pyArgs.add(3);
                        gDLL->getEventReporterIFace()->genericEvent("combatLogHit", pyArgs.makeFunctionArgs());
                    }
			        pDefender->setCombatBlockParrys(pDefender->getCombatBlockParrys() - 1);
			    }
			}
			else
			{
				iDefenderFirstStrikeChance++;
			}
		}


        if (getCombatFirstStrikes() > 0)
		{
			changeCombatFirstStrikes(-1);
		}

		if (pDefender->getCombatFirstStrikes() > 0)
		{
			pDefender->changeCombatFirstStrikes(-1);
		}
        if ((isDead() || pDefender->isDead()) && !bCombatXP)
        {
            break;
        }
		else if ((isDead() || pDefender->isDead()))
		{
			if (isDead())
			{
				int iExperience = defenseXPValue();
				iExperience = ((iExperience * iAttackerStrength) / iDefenderStrength);
				iExperience = range(iExperience, GC.getXMLval(XML_MIN_EXPERIENCE_PER_COMBAT), GC.getXMLval(XML_MAX_EXPERIENCE_PER_COMBAT));
				pDefender->changeExperience(iExperience, maxXPValue(), true, pPlot->getOwnerINLINE() == pDefender->getOwnerINLINE(), bGreatGeneralXP);
			}
			else
			{
				int iExperience = pDefender->attackXPValue();
				iExperience = ((iExperience * iDefenderStrength) / iAttackerStrength);
				iExperience = range(iExperience, GC.getXMLval(XML_MIN_EXPERIENCE_PER_COMBAT), GC.getXMLval(XML_MAX_EXPERIENCE_PER_COMBAT));
				changeExperience(iExperience, pDefender->maxXPValue(), true, pPlot->getOwnerINLINE() == getOwnerINLINE(), bGreatGeneralXP);
			}
            ///TKe
			break;
		}
	}
}


void CvUnit::updateCombat(bool bQuick)
{
	CvWString szBuffer;

	bool bFinish = false;
	bool bVisible = false;

	if (getCombatTimer() > 0)
	{
		changeCombatTimer(-1);

		if (getCombatTimer() > 0)
		{
			return;
		}
		else
		{
			bFinish = true;
		}
	}

	CvPlot* pPlot = getAttackPlot();

	if (pPlot == NULL)
	{
		return;
	}

	CvUnit* pDefender = NULL;
	if (bFinish)
	{
		pDefender = getCombatUnit();
	}
	else
	{
		pDefender = pPlot->getBestDefender(NO_PLAYER, getOwnerINLINE(), this, true);
	}

	if (pDefender == NULL)
	{
		setAttackPlot(NULL);
		setCombatUnit(NULL);

		getGroup()->groupMove(pPlot, true, ((canAdvance(pPlot, 0)) ? this : NULL));

		getGroup()->clearMissionQueue();

		return;
	}

	//check if quick combat
	if (!bQuick)
	{
		bVisible = isCombatVisible(pDefender);
	}

	//FAssertMsg((pPlot == pDefender->plot()), "There is not expected to be a defender or the defender's plot is expected to be pPlot (the attack plot)");

	//if not finished and not fighting yet, set up combat damage and mission
	if (!bFinish)
	{
		if (!isFighting())
		{
			if (plot()->isFighting() || pPlot->isFighting())
			{
				return;
			}

			setMadeAttack(true);

			//rotate to face plot
			DirectionTypes newDirection = estimateDirection(this->plot(), pDefender->plot());
			if (newDirection != NO_DIRECTION)
			{
				setFacingDirection(newDirection);
			}

			//rotate enemy to face us
			newDirection = estimateDirection(pDefender->plot(), this->plot());
			if (newDirection != NO_DIRECTION)
			{
				pDefender->setFacingDirection(newDirection);
			}

			setCombatUnit(pDefender, true);
			pDefender->setCombatUnit(this, false);

			pDefender->getGroup()->clearMissionQueue();

			bool bFocused = (bVisible && isCombatFocus() && gDLL->getInterfaceIFace()->isCombatFocus());

			if (bFocused)
			{
				DirectionTypes directionType = directionXY(plot(), pPlot);
				//								N			NE				E				SE					S				SW					W				NW
				NiPoint2 directions[8] = {NiPoint2(0, 1), NiPoint2(1, 1), NiPoint2(1, 0), NiPoint2(1, -1), NiPoint2(0, -1), NiPoint2(-1, -1), NiPoint2(-1, 0), NiPoint2(-1, 1)};
				NiPoint3 attackDirection = NiPoint3(directions[directionType].x, directions[directionType].y, 0);
				float plotSize = GC.getPLOT_SIZE();
				NiPoint3 lookAtPoint(plot()->getPoint().x + plotSize / 2 * attackDirection.x, plot()->getPoint().y + plotSize / 2 * attackDirection.y, (plot()->getPoint().z + pPlot->getPoint().z) / 2);
				attackDirection.Unitize();
				gDLL->getInterfaceIFace()->lookAt(lookAtPoint, (((getOwnerINLINE() != GC.getGameINLINE().getActivePlayer()) || gDLL->getGraphicOption(GRAPHICOPTION_NO_COMBAT_ZOOM)) ? CAMERALOOKAT_BATTLE : CAMERALOOKAT_BATTLE_ZOOM_IN), attackDirection);
			}
			else
			{
				PlayerTypes eAttacker = getVisualOwner(pDefender->getTeam());
				CvWString szMessage;
				if (UNKNOWN_PLAYER != eAttacker)
				{
					szMessage = gDLL->getText("TXT_KEY_MISC_YOU_UNITS_UNDER_ATTACK", GET_PLAYER(eAttacker).getNameKey());
				}
				else
				{
					szMessage = gDLL->getText("TXT_KEY_MISC_YOU_UNITS_UNDER_ATTACK_UNKNOWN");
				}

				gDLL->getInterfaceIFace()->addMessage(pDefender->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szMessage, "AS2D_COMBAT", MESSAGE_TYPE_DISPLAY_ONLY, getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pPlot->getX_INLINE(), pPlot->getY_INLINE(), true);
			}
		}

		FAssertMsg(pDefender != NULL, "Defender is not assigned a valid value");

		FAssertMsg(plot()->isFighting(), "Current unit instance plot is not fighting as expected");
		FAssertMsg(pPlot->isFighting(), "pPlot is not fighting as expected");

		if (!pDefender->canDefend())
		{
			if (!bVisible)
			{
				bFinish = true;
			}
			else
			{
				CvMissionDefinition kMission;
				kMission.setMissionTime(getCombatTimer() * gDLL->getSecsPerTurn());
				kMission.setMissionType(MISSION_SURRENDER);
				kMission.setUnit(BATTLE_UNIT_ATTACKER, this);
				kMission.setUnit(BATTLE_UNIT_DEFENDER, pDefender);
				kMission.setPlot(pPlot);
				gDLL->getEntityIFace()->AddMission(&kMission);

				// Surrender mission
				setCombatTimer(GC.getMissionInfo(MISSION_SURRENDER).getTime());

				GC.getGameINLINE().incrementTurnTimer(getCombatTimer());
			}

			// Kill them!
			pDefender->setDamage(GC.getMAX_HIT_POINTS());
		}
		else
		{
			CvBattleDefinition kBattle;
			kBattle.setUnit(BATTLE_UNIT_ATTACKER, this);
			kBattle.setUnit(BATTLE_UNIT_DEFENDER, pDefender);
			kBattle.setDamage(BATTLE_UNIT_ATTACKER, BATTLE_TIME_BEGIN, getDamage());
			kBattle.setDamage(BATTLE_UNIT_DEFENDER, BATTLE_TIME_BEGIN, pDefender->getDamage());

			resolveCombat(pDefender, pPlot, kBattle);

			if (!bVisible)
			{
				bFinish = true;
			}
			else
			{
				kBattle.setDamage(BATTLE_UNIT_ATTACKER, BATTLE_TIME_END, getDamage());
				kBattle.setDamage(BATTLE_UNIT_DEFENDER, BATTLE_TIME_END, pDefender->getDamage());
				kBattle.setAdvanceSquare(canAdvance(pPlot, 1));

				if (isRanged() && pDefender->isRanged()) //ranged
				{
					kBattle.setDamage(BATTLE_UNIT_ATTACKER, BATTLE_TIME_RANGED, kBattle.getDamage(BATTLE_UNIT_ATTACKER, BATTLE_TIME_END));
					kBattle.setDamage(BATTLE_UNIT_DEFENDER, BATTLE_TIME_RANGED, kBattle.getDamage(BATTLE_UNIT_DEFENDER, BATTLE_TIME_END));
				}
				else if(kBattle.isOneStrike()) //melee dies right away
				{
					kBattle.setDamage(BATTLE_UNIT_ATTACKER, BATTLE_TIME_RANGED, kBattle.getDamage(BATTLE_UNIT_ATTACKER, BATTLE_TIME_END));
					kBattle.setDamage(BATTLE_UNIT_DEFENDER, BATTLE_TIME_RANGED, kBattle.getDamage(BATTLE_UNIT_DEFENDER, BATTLE_TIME_END));
				}
				else //melee fighting
				{
					kBattle.addDamage(BATTLE_UNIT_ATTACKER, BATTLE_TIME_RANGED, kBattle.getDamage(BATTLE_UNIT_ATTACKER, BATTLE_TIME_BEGIN));
					kBattle.addDamage(BATTLE_UNIT_DEFENDER, BATTLE_TIME_RANGED, kBattle.getDamage(BATTLE_UNIT_DEFENDER, BATTLE_TIME_BEGIN));
				}

				int iTurns = planBattle( kBattle);
				kBattle.setMissionTime(iTurns * gDLL->getSecsPerTurn());
				setCombatTimer(iTurns);

				GC.getGameINLINE().incrementTurnTimer(getCombatTimer());

				if (pPlot->isActiveVisible(false))
				{
					ExecuteMove(0.5f, true);
					gDLL->getEntityIFace()->AddMission(&kBattle);
				}
			}
		}
	}

	if (bFinish)
	{
		if (bVisible)
		{
			if (isCombatFocus() && gDLL->getInterfaceIFace()->isCombatFocus())
			{
				if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
				{
					gDLL->getInterfaceIFace()->releaseLockedCamera();
				}
			}
		}

		//end the combat mission if this code executes first
		gDLL->getEntityIFace()->RemoveUnitFromBattle(this);
		gDLL->getEntityIFace()->RemoveUnitFromBattle(pDefender);
		setAttackPlot(NULL);
		bool bDefenderEscaped = (pDefender->getPostCombatPlot() != pPlot);
		bool bAttackerEscaped = (getPostCombatPlot() != plot());
		setCombatUnit(NULL);
		pDefender->setCombatUnit(NULL);
		NotifyEntity(MISSION_DAMAGE);
		pDefender->NotifyEntity(MISSION_DAMAGE);

		///TK Med RK
		if (getTraderCode() == 1)
		{
		    doRansomKnight(GET_PLAYER(pDefender->getOwnerINLINE()).getID());
		}
		else if (pDefender->getTraderCode() == 1)
		{
		    pDefender->doRansomKnight(GET_PLAYER(getOwnerINLINE()).getID());
		}
		///TKe

		if (isDead())
		{
			if (!m_pUnitInfo->isHiddenNationality() && !pDefender->getUnitInfo().isHiddenNationality())
			{
				GET_TEAM(pDefender->getTeam()).AI_changeWarSuccess(getTeam(), GC.getXMLval(XML_WAR_SUCCESS_DEFENDING));
			}

			CvCity* pCity = pPlot->getPlotCity();
			if (pCity != NULL)
			{
				raidGoods(pCity);
			}

			szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_DIED_ATTACKING", getNameOrProfessionKey(), pDefender->getNameOrProfessionKey());
			gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, GC.getEraInfo(GC.getGameINLINE().getCurrentEra()).getAudioUnitDefeatScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pPlot->getX_INLINE(), pPlot->getY_INLINE());
			szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_KILLED_ENEMY_UNIT", pDefender->getNameOrProfessionKey(), getNameOrProfessionKey(), getVisualCivAdjective(pDefender->getTeam()));
			gDLL->getInterfaceIFace()->addMessage(pDefender->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, GC.getEraInfo(GC.getGameINLINE().getCurrentEra()).getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pPlot->getX_INLINE(), pPlot->getY_INLINE());

			// report event to Python, along with some other key state
			gDLL->getEventReporterIFace()->combatResult(pDefender, this);
		}
		else if (pDefender->isDead())
		{
			if (!m_pUnitInfo->isHiddenNationality() && !pDefender->getUnitInfo().isHiddenNationality())
			{
				GET_TEAM(getTeam()).AI_changeWarSuccess(pDefender->getTeam(), GC.getXMLval(XML_WAR_SUCCESS_ATTACKING));
				if (GET_PLAYER(getOwnerINLINE()).isNative())
				{
					GET_TEAM(getTeam()).AI_changeDamages(pDefender->getTeam(), -2 * pDefender->getUnitInfo().getAssetValue());
				}
			}

			szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_DESTROYED_ENEMY", getNameOrProfessionKey(), pDefender->getNameOrProfessionKey());
			gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, GC.getEraInfo(GC.getGameINLINE().getCurrentEra()).getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pPlot->getX_INLINE(), pPlot->getY_INLINE());
			if (pDefender->isBarbarian() || pDefender->isAlwaysHostile(pPlot))
			{
                for (int iI = 0; iI < MAX_PLAYERS; iI++)
                {
                    CvPlayerAI& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
                    if (kLoopPlayer.isAlive() && pDefender->plot()->isVisible(kLoopPlayer.getTeam(), true) && getOwner() != (PlayerTypes)iI)
                    {
                        if (pDefender->getUnitInfo().isAnimal() && pDefender->plot()->getOwner() == (PlayerTypes)iI)
                        {
                            szBuffer = gDLL->getText("TXT_KEY_MISC_UNIT_POACHED_ANIMAL", GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey(), getNameOrProfessionKey(), pDefender->getNameOrProfessionKey());
                        }
                        else if (pDefender->getUnitInfo().isAnimal())
                        {
                            szBuffer = gDLL->getText("TXT_KEY_MISC_UNIT_POACHED_ANIMAL_NEAR", GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey(), getNameOrProfessionKey(), pDefender->getNameOrProfessionKey());
                        }
                        else
                        {
                            szBuffer = gDLL->getText("TXT_KEY_MISC_UNIT_DESTROYED_BARBARIAN", GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey(), getNameOrProfessionKey(), pDefender->getNameOrProfessionKey());
                        }
                        gDLL->getInterfaceIFace()->addMessage((PlayerTypes)iI, true, GC.getEVENT_MESSAGE_TIME(), szBuffer, GC.getEraInfo(GC.getGameINLINE().getCurrentEra()).getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pPlot->getX_INLINE(), pPlot->getY_INLINE());
                    }
                }

                if (pDefender->getYieldStored() > 0)
                {
                    UnitTypes eTreasureUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getXMLval(XML_TREASURE_UNITCLASS));
                     if (eTreasureUnit != NO_UNIT)
                    {
                        CvUnit* pTreasureUnit = GET_PLAYER(getOwnerINLINE()).initUnit(eTreasureUnit, (ProfessionTypes) GC.getUnitInfo(eTreasureUnit).getDefaultProfession(), INVALID_PLOT_COORD, INVALID_PLOT_COORD);
                        //CvPlot* pTreasePlot = pPlot;
                        //if (getPostCombatPlot() != plot())
                        pTreasureUnit->setYieldStored(pDefender->getYieldStored());
                        pTreasureUnit->addToMap(pPlot->getX_INLINE(), pPlot->getY_INLINE());
                        szBuffer = gDLL->getText("TXT_KEY_BANDIT_ATTACK_UNIT_RECOVERED", pDefender->getYieldStored(), pDefender->getNameOrProfessionKey());
                        gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, GC.getEraInfo(GC.getGameINLINE().getCurrentEra()).getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pPlot->getX_INLINE(), pPlot->getY_INLINE());
                    }
                }
			}
			if (getVisualOwner(pDefender->getTeam()) != getOwnerINLINE())
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED_UNKNOWN", pDefender->getNameOrProfessionKey(), getNameOrProfessionKey());
			}
			else
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED", pDefender->getNameOrProfessionKey(), getNameOrProfessionKey(), getVisualCivAdjective(pDefender->getTeam()));
			}

			gDLL->getInterfaceIFace()->addMessage(pDefender->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer,GC.getEraInfo(GC.getGameINLINE().getCurrentEra()).getAudioUnitDefeatScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pPlot->getX_INLINE(), pPlot->getY_INLINE());

			// report event to Python, along with some other key state
			gDLL->getEventReporterIFace()->combatResult(this, pDefender);

			bool bAdvance = false;
			bool bRaided = raidWeapons(pDefender);

			if (!pDefender->isUnarmed() || GET_PLAYER(getOwnerINLINE()).isNative())
			{
				CvCity* pCity = pPlot->getPlotCity();
				if (NULL != pCity && pCity->getOwnerINLINE() == pDefender->getOwnerINLINE())
				{
					if (pPlot->getNumVisibleEnemyDefenders(this) <= 1)
					{
						pCity->ejectBestDefender(NULL, NULL);
					}
				}
			}
			else
			{
				if (!isNoUnitCapture())
				{
					CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
					while (pUnitNode != NULL)
					{
						CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
						pUnitNode = pPlot->nextUnitNode(pUnitNode);

						if (pLoopUnit != pDefender)
						{
							if (isEnemy(pLoopUnit->getCombatTeam(getTeam(), pPlot), pPlot))
							{
								pLoopUnit->setCapturingPlayer(getOwnerINLINE());
							}
						}
					}
				}
			}

			bAdvance = canAdvance(pPlot, ((pDefender->canDefend()) ? 1 : 0));

			if (bAdvance)
			{
				if (!isNoUnitCapture())
				{
				    ///TKs Med
				    UnitTypes eCaptureUnitType = pDefender->getCaptureUnitType(getCivilizationType());
					if (eCaptureUnitType != NO_UNIT)
					{
                        if (GC.getUnitInfo(eCaptureUnitType).getDefaultUnitAIType() == UNITAI_YIELD)
                        {
                            if (cargoSpace() > 0)
                            {
                                pDefender->setCapturingPlayer(getOwnerINLINE());
                            }
                        }
                        else if (pDefender->canDefend())
                        {
                            if (getDomainType() == DOMAIN_SEA)
                            {
                                if (cargoSpace() > 0 && !isFull())
                                {
                                    if (GC.getGameINLINE().getSorenRandNum(100, "Criminal Capture") <= GC.getXMLval(XML_CHANCE_TO_CAPTURE_CRIMINALS))
                                    {
                                        pDefender->setCapturingPlayer(getOwnerINLINE());
                                    }
                                }
                            }
                            else if (GC.getGameINLINE().getSorenRandNum(100, "Criminal Capture") <= GC.getXMLval(XML_CHANCE_TO_CAPTURE_CRIMINALS))
                            {
                                pDefender->setCapturingPlayer(getOwnerINLINE());
                            }

                        }
					}
					else if (!pDefender->canDefend())
					{
                        pDefender->setCapturingPlayer(getOwnerINLINE());
					}
					///TKe
				}
			}
            ///TKs Med
			pDefender->kill(false, this);
			///TKe
			pDefender = NULL;

			if (!bAdvance)
			{
				changeMoves(pPlot->movementCost(this, plot()));

				if (!canMove() || !isBlitz())
				{
					if (IsSelected())
					{
						if (gDLL->getInterfaceIFace()->getLengthSelectionList() > 1)
						{
							gDLL->getInterfaceIFace()->removeFromSelectionList(this);
						}
					}
				}
			}

			if (!bRaided)
			{
				CvCity* pCity = pPlot->getPlotCity();
				if (pCity != NULL)
				{
					if (!raidWeapons(pCity))
					{
						raidGoods(pCity);
					}
				}
			}

			if (pPlot->getNumVisibleEnemyDefenders(this) == 0)
			{
				getGroup()->groupMove(pPlot, true, ((bAdvance) ? this : NULL));
			}

			// This is is put before the plot advancement, the unit will always try to walk back
			// to the square that they came from, before advancing.
			getGroup()->clearMissionQueue();
		}
		else if (bDefenderEscaped && pDefender->getTraderCode() != 1)
		{
		    if (pDefender->getTraderCode() == 3)
		    {
                int iGoldStolen = pDefender->m_pUnitInfo->getPowerValue();
                iGoldStolen += pDefender->m_pUnitInfo->getAssetValue();
                iGoldStolen = GC.getGameINLINE().getSorenRandNum(iGoldStolen + 1, "Bandit Steals Gold");
                iGoldStolen = (iGoldStolen * 30 / 100) + 10;
                if (GET_PLAYER(pDefender->getOwnerINLINE()).getGold() >= iGoldStolen)
                {
                    GET_PLAYER(pDefender->getOwnerINLINE()).changeGold(-iGoldStolen);
                    setYieldStored(iGoldStolen + getYieldStored());
                    CvWString szBuffer = gDLL->getText("TXT_KEY_BANDIT_ATTACK_UNIT",  getNameOrProfessionKey(), getYieldStored());
    gDLL->getInterfaceIFace()->addMessage(pDefender->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, GC.getEraInfo(GC.getGameINLINE().getCurrentEra()).getAudioUnitDefeatScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pPlot->getX_INLINE(), pPlot->getY_INLINE());
                }
                CvCity* pCity = pDefender->plot()->getPlotCity();
                if (pCity != NULL)
                {
                    szBuffer = gDLL->getText("TXT_KEY_MISC_ENEMY_UNIT_ESCAPED", pDefender->getNameOrProfessionKey(), getNameOrProfessionKey(), pCity->getNameKey());
                    gDLL->getInterfaceIFace()->addMessage(pDefender->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_UNITCAPTURE", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pCity->getX_INLINE(), pCity->getY_INLINE());
                }
		    }

			szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_ESCAPED", getNameOrProfessionKey(), pDefender->getNameOrProfessionKey());
			gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pPlot->getX_INLINE(), pPlot->getY_INLINE());

			if (pDefender->getTraderCode() != 3)
			{
                CvCity* pCity = pDefender->plot()->getPlotCity();
                if (pCity != NULL)
                {
                    szBuffer = gDLL->getText("TXT_KEY_MISC_ENEMY_UNIT_ESCAPED", pDefender->getNameOrProfessionKey(), getNameOrProfessionKey(), pCity->getNameKey());
                    gDLL->getInterfaceIFace()->addMessage(pDefender->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pCity->getX_INLINE(), pCity->getY_INLINE());
                }
			}

			bool bAdvance = canAdvance(pPlot, 0);
			if (!bAdvance)
			{
				changeMoves(std::max(GC.getMOVE_DENOMINATOR(), pPlot->movementCost(this, plot())));

				if (!canMove() || !isBlitz())
				{
					if (IsSelected())
					{
						if (gDLL->getInterfaceIFace()->getLengthSelectionList() > 1)
						{
							gDLL->getInterfaceIFace()->removeFromSelectionList(this);
						}
					}
				}
			}

			CvCity* pRaidCity = pPlot->getPlotCity();
			if (pRaidCity != NULL)
			{
				raidGoods(pRaidCity);
			}

			if (m_pUnitInfo->isCapturesCargo())
			{
				std::vector<CvUnit*> cargoUnits;
				CLLNode<IDInfo>* pUnitNode = pDefender->plot()->headUnitNode();
				while (pUnitNode != NULL)
				{
					CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
					if (pLoopUnit != NULL && pLoopUnit->getTransportUnit() == pDefender)
					{
						cargoUnits.push_back(pLoopUnit);
					}

					pUnitNode = pDefender->plot()->nextUnitNode(pUnitNode);
				}

				for (uint i = 0; i < cargoUnits.size(); ++i)
				{
					CvUnit* pLoopUnit = cargoUnits[i];
					pLoopUnit->setCapturingPlayer(getOwnerINLINE());
					pLoopUnit->kill(false, this);
				}
			}

			if (pPlot->getNumVisibleEnemyDefenders(this) == 0)
			{
				getGroup()->groupMove(pPlot, true, ((bAdvance) ? this : NULL));
			}

			getGroup()->clearMissionQueue();
		}
		else if (bAttackerEscaped && getTraderCode() != 1)
		{
			CvCity* pCity = plot()->getPlotCity();
			if (pCity != NULL)
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_ENEMY_UNIT_ESCAPED", getNameOrProfessionKey(), pDefender->getNameOrProfessionKey(), pCity->getNameKey());
				gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pCity->getX_INLINE(), pCity->getY_INLINE());
			}
			szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_ESCAPED", pDefender->getNameOrProfessionKey(), getNameOrProfessionKey());
			gDLL->getInterfaceIFace()->addMessage(pDefender->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pPlot->getX_INLINE(), pPlot->getY_INLINE());

			if (IsSelected())
			{
				if (gDLL->getInterfaceIFace()->getLengthSelectionList() > 1)
				{
					gDLL->getInterfaceIFace()->removeFromSelectionList(this);
				}
			}


			// This is is put before the plot advancement, the unit will always try to walk back
			// to the square that they came from, before advancing.
			getGroup()->clearMissionQueue();
		}
		else if (getTraderCode() != 1 && pDefender->getTraderCode() != 1)
		{
			szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WITHDRAW", getNameOrProfessionKey(), pDefender->getNameOrProfessionKey());
			gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pPlot->getX_INLINE(), pPlot->getY_INLINE());
			szBuffer = gDLL->getText("TXT_KEY_MISC_ENEMY_UNIT_WITHDRAW", getNameOrProfessionKey(), pDefender->getNameOrProfessionKey());
			gDLL->getInterfaceIFace()->addMessage(pDefender->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pPlot->getX_INLINE(), pPlot->getY_INLINE());

			changeMoves(std::max(GC.getMOVE_DENOMINATOR(), pPlot->movementCost(this, plot())));
			CvCity* pCity = pPlot->getPlotCity();
			if (pCity != NULL)
			{
				raidGoods(pCity);
			}

			getGroup()->clearMissionQueue();
		}
	}
}
///TK end

bool CvUnit::isActionRecommended(int iAction)
{
	CvCity* pWorkingCity;
	ImprovementTypes eImprovement;
	ImprovementTypes eFinalImprovement;
	BuildTypes eBuild;
	RouteTypes eRoute;
	BonusTypes eBonus;
	int iIndex;

	if (getOwnerINLINE() != GC.getGameINLINE().getActivePlayer())
	{
		return false;
	}

	if (GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_NO_UNIT_RECOMMENDATIONS))
	{
		return false;
	}

	CyUnit* pyUnit = new CyUnit(this);
	CyArgsList argsList;
	argsList.add(gDLL->getPythonIFace()->makePythonObject(pyUnit));	// pass in unit class
	argsList.add(iAction);
	long lResult=0;
	gDLL->getPythonIFace()->callFunction(PYGameModule, "isActionRecommended", argsList.makeFunctionArgs(), &lResult);
	delete pyUnit;	// python fxn must not hold on to this pointer
	if (lResult == 1)
	{
		return true;
	}

	CvPlot* pPlot = gDLL->getInterfaceIFace()->getGotoPlot();
	if (pPlot == NULL)
	{
		if (gDLL->shiftKey())
		{
			pPlot = getGroup()->lastMissionPlot();
		}
	}

	if (pPlot == NULL)
	{
		pPlot = plot();
	}

	switch (GC.getActionInfo(iAction).getMissionType())
	{
	case MISSION_FORTIFY:
		if (pPlot->isCity(true, getTeam()))
		{
			if (canDefend(pPlot) && !isUnarmed())
			{
				if (pPlot->getNumDefenders(getOwnerINLINE()) < ((atPlot(pPlot)) ? 2 : 1))
				{
					return true;
				}
			}
		}
		break;
	case MISSION_HEAL:
		if (isHurt())
		{
			if (!hasMoved())
			{
				if ((pPlot->getTeam() == getTeam()) || (healTurns(pPlot) < 4))
				{
					return true;
				}
			}
		}
		break;

	case MISSION_FOUND:
		if (canFound(pPlot))
		{
			if (pPlot->isBestAdjacentFound(getOwnerINLINE()))
			{
				return true;
			}
		}
		break;

	case MISSION_BUILD:
		if (pPlot->getOwner() == getOwnerINLINE())
		{
			eBuild = ((BuildTypes)(GC.getActionInfo(iAction).getMissionData()));
			FAssert(eBuild != NO_BUILD);
			FAssertMsg(eBuild < GC.getNumBuildInfos(), "Invalid Build");

			if (canBuild(pPlot, eBuild))
			{
				eImprovement = ((ImprovementTypes)(GC.getBuildInfo(eBuild).getImprovement()));
				eRoute = ((RouteTypes)(GC.getBuildInfo(eBuild).getRoute()));
				eBonus = pPlot->getBonusType();
				pWorkingCity = pPlot->getWorkingCity();

				if (pPlot->getImprovementType() == NO_IMPROVEMENT)
				{
					if (pWorkingCity != NULL)
					{
						iIndex = pWorkingCity->getCityPlotIndex(pPlot);

						if (iIndex != -1)
						{
							if (pWorkingCity->AI_getBestBuild(iIndex) == eBuild)
							{
								return true;
							}
						}
					}

					if (eImprovement != NO_IMPROVEMENT)
					{
						if (pPlot->getImprovementType() == NO_IMPROVEMENT)
						{
							if (pWorkingCity != NULL)
							{
								if (GC.getImprovementInfo(eImprovement).getYieldIncrease(YIELD_FOOD) > 0)
								{
									return true;
								}
							}
						}
					}
				}

				if (eRoute != NO_ROUTE)
				{
					if (!(pPlot->isRoute()))
					{
						if (eBonus != NO_BONUS)
						{
							return true;
						}

						if (pWorkingCity != NULL)
						{
							if (pPlot->isRiver())
							{
								return true;
							}
						}
					}

					eFinalImprovement = eImprovement;

					if (eFinalImprovement == NO_IMPROVEMENT)
					{
						eFinalImprovement = pPlot->getImprovementType();
					}

					if (eFinalImprovement != NO_IMPROVEMENT)
					{
						for (int i = 0; i < NUM_YIELD_TYPES; ++i)
						{
							if (GC.getImprovementInfo(eFinalImprovement).getRouteYieldChanges(eRoute, (YieldTypes)i) > 0)
							{
								return true;
							}
						}
					}
				}
			}
		}
		break;

	default:
		break;
	}

	if (GC.getActionInfo(iAction).getAutomateType() == AUTOMATE_SAIL || GC.getActionInfo(iAction).getCommandType() == COMMAND_SAIL_TO_EUROPE)
	{
		CLinkList<IDInfo> listCargo;
		getGroup()->buildCargoUnitList(listCargo);
		CLLNode<IDInfo>* pUnitNode = listCargo.head();
		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = listCargo.next(pUnitNode);

			if (pLoopUnit->getYield() != NO_YIELD && GET_PLAYER(getOwnerINLINE()).isYieldEuropeTradable(pLoopUnit->getYield()))
			{
				return true;
			}

			if (pLoopUnit->getUnitInfo().isTreasure())
			{
				return true;
			}
		}

		if (getCargo() == 0)
		{
			if (GET_PLAYER(getOwnerINLINE()).getNumEuropeUnits() > 0)
			{
				return true;
			}
		}
	}

	switch (GC.getActionInfo(iAction).getCommandType())
	{
	case COMMAND_PROMOTION:
	case COMMAND_PROMOTE:
	case COMMAND_KING_TRANSPORT:
	case COMMAND_ESTABLISH_MISSION:
	case COMMAND_SPEAK_WITH_CHIEF:
	case COMMAND_YIELD_TRADE:
	case COMMAND_LEARN:
		return true;
		break;
	default:
		break;
	}

	return false;
}


bool CvUnit::isBetterDefenderThan(const CvUnit* pDefender, const CvUnit* pAttacker, bool bBreakTies) const
{
	int iOurDefense;
	int iTheirDefense;

	if (pDefender == NULL)
	{
		return true;
	}

	TeamTypes eAttackerTeam = NO_TEAM;
	if (NULL != pAttacker)
	{
		eAttackerTeam = pAttacker->getTeam();
	}

	if (canCoexistWithEnemyUnit(eAttackerTeam))
	{
		return false;
	}

	if (!canDefend())
	{
		return false;
	}

	if (canDefend() && !(pDefender->canDefend()))
	{
		return true;
	}

	bool bOtherUnarmed = pDefender->isUnarmed();
	if (isUnarmed() != bOtherUnarmed)
	{
		return bOtherUnarmed;
	}

    ///TK FS
	iOurDefense = currCombatStr(plot(), pAttacker);
    if (NULL != pAttacker)
    {
		if (!(pAttacker->immuneToFirstStrikes()))
		{
			iOurDefense *= ((((firstStrikes() * 2) + chanceFirstStrikes()) * ((GC.getXMLval(XML_COMBAT_DAMAGE) * 2) / 5)) + 100);
			iOurDefense /= 100;
		}

		if (immuneToFirstStrikes())
		{
			iOurDefense *= ((((pAttacker->firstStrikes() * 2) + pAttacker->chanceFirstStrikes()) * ((GC.getXMLval(XML_COMBAT_DAMAGE) * 2) / 5)) + 100);
			iOurDefense /= 100;
		}
	}

	iOurDefense /= (getCargo() + 1);

	iTheirDefense = pDefender->currCombatStr(plot(), pAttacker);
	if (NULL != pAttacker)
    {
		if (!(pAttacker->immuneToFirstStrikes()))
		{
			iTheirDefense *= ((((pDefender->firstStrikes() * 2) + pDefender->chanceFirstStrikes()) * ((GC.getXMLval(XML_COMBAT_DAMAGE) * 2) / 5)) + 100);
			iTheirDefense /= 100;
		}

		if (pDefender->immuneToFirstStrikes())
		{
			iTheirDefense *= ((((pAttacker->firstStrikes() * 2) + pAttacker->chanceFirstStrikes()) * ((GC.getXMLval(XML_COMBAT_DAMAGE) * 2) / 5)) + 100);
			iTheirDefense /= 100;
		}
	}

	///TKe
	iTheirDefense /= (pDefender->getCargo() + 1);

	if (iOurDefense == iTheirDefense)
	{
		if (isOnMap() && !pDefender->isOnMap())
		{
			++iOurDefense;
		}
		else if (!isOnMap() && pDefender->isOnMap())
		{
			++iTheirDefense;
		}
		if (NO_UNIT == getLeaderUnitType() && NO_UNIT != pDefender->getLeaderUnitType())
		{
			++iOurDefense;
		}
		else if (NO_UNIT != getLeaderUnitType() && NO_UNIT == pDefender->getLeaderUnitType())
		{
			++iTheirDefense;
		}
		else if (bBreakTies && isBeforeUnitCycle(this, pDefender))
		{
			++iOurDefense;
		}
	}

	return (iOurDefense > iTheirDefense);
}


bool CvUnit::canDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible, bool bTestBusy)
{
	CvUnit* pUnit;

	if (bTestBusy && getGroup()->isBusy())
	{
		return false;
	}

	switch (eCommand)
	{
	case COMMAND_PROMOTION:
		if (canPromote((PromotionTypes)iData1, iData2))
		{
			return true;
		}
		break;

	case COMMAND_UPGRADE:
		if (canUpgrade(((UnitTypes)iData1), bTestVisible))
		{
			return true;
		}
		break;

	case COMMAND_AUTOMATE:
		if (canAutomate((AutomateTypes)iData1))
		{
			return true;
		}
		break;

	case COMMAND_WAKE:
		if (!isAutomated() && isWaiting())
		{
			return true;
		}
		break;

	case COMMAND_CANCEL:
	case COMMAND_CANCEL_ALL:
		if (!isAutomated() && (getGroup()->getLengthMissionQueue() > 0))
		{
			return true;
		}
		break;

	case COMMAND_STOP_AUTOMATION:
		if (isAutomated())
		{
			return true;
		}
		break;

	case COMMAND_DELETE:
		if (canScrap())
		{
			return true;
		}
		break;

	case COMMAND_GIFT:
		if (canGift(bTestVisible))
		{
			return true;
		}
		break;

	case COMMAND_LOAD:
		if (canLoad(plot(), true))
		{
			return true;
		}
		break;

	case COMMAND_LOAD_YIELD:
		if (canLoadYield(plot(), (YieldTypes) iData1, false))
		{
			return true;
		}
		break;

	case COMMAND_LOAD_CARGO:
		if (canLoadYield(plot(), NO_YIELD, false))
		{
			return true;
		}
		break;

	case COMMAND_LOAD_UNIT:
		pUnit = ::getUnit(IDInfo(((PlayerTypes)iData1), iData2));
		if (pUnit != NULL)
		{
			if (canLoadUnit(pUnit, plot(), true))
			{
				return true;
			}
		}
		break;

	case COMMAND_YIELD_TRADE:
		if (canTradeYield(plot()))
		{
			return true;
		}
		break;

	case COMMAND_SAIL_TO_EUROPE:
         ///Tks Med
		{
			 if (isOnMap() && GC.getLeaderHeadInfo(GET_PLAYER(getOwnerINLINE()).getLeaderType()).getTravelCommandType() == 1)
			 {
				 return false;
			 }
			
			EuropeTypes eTradeRoute = (EuropeTypes)iData2;
			if (canCrossOcean(plot(), (UnitTravelStates)iData1, NO_TRADE_ROUTES, false, eTradeRoute))
			{
				return true;
			}
		}
		break;
		///Tke
	case COMMAND_CHOOSE_TRADE_ROUTES:
	case COMMAND_ASSIGN_TRADE_ROUTE:
		if (iData2 == 0 || canAssignTradeRoute(iData1))
		{
			return true;
		}
		break;

	case COMMAND_PROMOTE:
		{
			CvSelectionGroup* pSelection = gDLL->getInterfaceIFace()->getSelectionList();
			if (pSelection != NULL)
			{
				if (pSelection->isPromotionReady())
				{
					return true;
				}
			}
		}
		break;

	case COMMAND_PROFESSION:
		{
			if (iData1 == -1)
			{
				CvSelectionGroup* pSelection = gDLL->getInterfaceIFace()->getSelectionList();
				if (pSelection != NULL)
				{
					if (pSelection->canChangeProfession())
					{
						return true;
					}
				}
			}
			else
			{
				if (canHaveProfession((ProfessionTypes) iData1, false))
				{
					return true;
				}
			}
		}
		break;

	case COMMAND_CLEAR_SPECIALTY:
		if (canClearSpecialty())
		{
			return true;
		}
		break;

	case COMMAND_UNLOAD:
		if (canUnload())
		{
			return true;
		}
		break;

	case COMMAND_UNLOAD_ALL:
		if (canUnloadAll())
		{
			return true;
		}
		break;

	case COMMAND_LEARN:
		if (canLearn())
		{
			return true;
		}
		break;

	case COMMAND_KING_TRANSPORT:
	///TKs Med
		if (canKingTransport(bTestVisible))
		{
			return true;
		}
		break;
    ///Tke
	case COMMAND_ESTABLISH_MISSION:
		if (canEstablishMission())
		{
			return true;
		}
		break;

	case COMMAND_SPEAK_WITH_CHIEF:
		if (canSpeakWithChief(plot()))
		{
			return true;
		}
		break;

	case COMMAND_HOTKEY:
		if (isGroupHead())
		{
			return true;
		}
		break;
    ///TKs Return Home
     case COMMAND_ASSIGN_HOME_CITY:
        if (GET_PLAYER(getOwnerINLINE()).getNumCities() > 0 && (cargoSpace() > 0 || getDomainType() == DOMAIN_SEA || m_pUnitInfo->isTreasure()))
        {

            return true;
        }
        break;
        case COMMAND_CALL_BANNERS:
            if (canCallBanners(bTestVisible))
            {
                return true;
            }
             break;
        case COMMAND_BUILD_HOME:
            if (canBuildHome(bTestVisible))
            {
                return true;
            }
             break;
        case COMMAND_BUILD_TRADINGPOST:
            if (canBuildTradingPost(bTestVisible))
            {
                return true;
            }
            break;
        case COMMAND_HIRE_GUARD:
            if (canHireGuard(bTestVisible))
            {
                return true;
            }
             break;
    ///TKs Med
        case COMMAND_SAIL_SPICE_ROUTE:
			{
				EuropeTypes eEuropeTradeRoute = (EuropeTypes)GC.getCommandInfo(COMMAND_SAIL_SPICE_ROUTE).getEuropeTradeRoute();
				if (canCrossOcean(plot(), (UnitTravelStates)iData1, TRADE_ROUTE_SPICE_ROUTE, false, eEuropeTradeRoute))
				{
					//return true;
					return false;
				}
			}
            break;
        case COMMAND_TRAVEL_TO_FAIR:
            if (GC.getLeaderHeadInfo(GET_PLAYER(getOwnerINLINE()).getLeaderType()).getTravelCommandType() == 1)
            {
				if (canCrossOcean(plot(), (UnitTravelStates)iData1, TRADE_ROUTE_FAIR))
                {
                   //return true;
					return false;
                }
            }
            break;
        case COMMAND_TRAVEL_SILK_ROAD:
            if (canCrossOcean(plot(), (UnitTravelStates)iData1, TRADE_ROUTE_SILK_ROAD))
            {
                //return true;
				return false;
            }
            break;
        case COMMAND_CONVERT_UNIT:
//			if (plot()->isCity(false, getTeam()))
//			{
//			    if (m_pUnitInfo->getConvertsToBuildingClass() != NO_BUILDINGCLASS)
//			    {
//			        return true;
//			    }
//                else if (m_pUnitInfo->getConvertsToYield() != NO_YIELD)
//			    {
//			        return true;
//			    }
//			    else if (m_pUnitInfo->getConvertsToGold() != 0)
//			    {
//			        return true;
//			    }
//			}
			break;
#ifdef USE_NOBLE_CLASS
        case COMMAND_HOLD_FEAST:
			{
				//Currently Not used, return false here
				return false;
			}
            if (!canMove())
            {
                return false;
            }
            if (m_pUnitInfo->getKnightDubbingWeight() == -1 || isHasRealPromotion((PromotionTypes)GC.getXMLval(XML_DEFAULT_KNIGHT_PROMOTION)))
            {
                CvCity* ePlotCity = plot()->getPlotCity();
                if (ePlotCity != NULL)
                {
                    if (ePlotCity->getYieldStored(YIELD_SHEEP) < GC.getXMLval(XML_BANQUET_YIELD_AMOUNT))
                    {
                        return false;
                    }
                    if (ePlotCity->getYieldStored(YIELD_CATTLE) < GC.getXMLval(XML_BANQUET_YIELD_AMOUNT))
                    {
                        return false;
                    }
                    if (ePlotCity->getYieldStored(YIELD_WINE) < GC.getXMLval(XML_BANQUET_YIELD_AMOUNT))
                    {
                        return false;
                    }
                    if (ePlotCity->getYieldStored(YIELD_ALE) < GC.getXMLval(XML_BANQUET_YIELD_AMOUNT))
                    {
                        return false;
                    }
                    return true;
                }
            }
            break;
#endif

	default:
		FAssert(false);
		break;
	}

	return false;
}


void CvUnit::doCommand(CommandTypes eCommand, int iData1, int iData2)
{
	CvUnit* pUnit;
	bool bCycle;

	bCycle = false;

	FAssert(getOwnerINLINE() != NO_PLAYER);

	if (canDoCommand(eCommand, iData1, iData2))
	{
		switch (eCommand)
		{
		case COMMAND_PROMOTION:
			promote((PromotionTypes)iData1, iData2);
			break;

		case COMMAND_UPGRADE:
			upgrade((UnitTypes)iData1);
			bCycle = true;
			break;

		case COMMAND_AUTOMATE:
			automate((AutomateTypes)iData1);
			bCycle = true;
			break;

		case COMMAND_WAKE:
			getGroup()->setActivityType(ACTIVITY_AWAKE);
			break;

		case COMMAND_CANCEL:
			getGroup()->popMission();
			break;

		case COMMAND_CANCEL_ALL:
			getGroup()->clearMissionQueue();
			break;

		case COMMAND_STOP_AUTOMATION:
			getGroup()->setAutomateType(NO_AUTOMATE);
			break;

		case COMMAND_DELETE:
			scrap();
			bCycle = true;
			break;

		case COMMAND_GIFT:
			gift();
			bCycle = true;
			break;

		case COMMAND_LOAD:
			load(true);
			bCycle = true;
			break;

		case COMMAND_LOAD_YIELD:
			{
				if (iData2 >= 0)
				{
					loadYieldAmount((YieldTypes) iData1, iData2, false);
				}
				else
				{
					loadYield((YieldTypes) iData1, false);
				}
			}
			break;

		case COMMAND_LOAD_CARGO:
			{
				CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_LOAD_CARGO);
				gDLL->getInterfaceIFace()->addPopup(pInfo);
			}
			break;

		case COMMAND_LOAD_UNIT:
			pUnit = ::getUnit(IDInfo(((PlayerTypes)iData1), iData2));
			if (pUnit != NULL)
			{
				loadUnit(pUnit);
				bCycle = true;
			}
			break;

		case COMMAND_YIELD_TRADE:
			tradeYield();
			break;

		case COMMAND_SAIL_TO_EUROPE:
			if (iData2 != NO_EUROPE)
			{
				setSailEurope((EuropeTypes) iData2);
			}
			crossOcean((UnitTravelStates) iData1, false, (EuropeTypes)iData2);
			break;

		case COMMAND_CHOOSE_TRADE_ROUTES:
			if (GET_PLAYER(getOwnerINLINE()).getNumTradeRoutes() > 0)
			{
				if (gDLL->getInterfaceIFace()->getHeadSelectedUnit() == this)
				{
					CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_TRADE_ROUTES, getID());
					gDLL->getInterfaceIFace()->addPopup(pInfo, getOwnerINLINE(), true, true);
				}
			}
			else
			{
			}
			break;

		case COMMAND_ASSIGN_TRADE_ROUTE:
			if (isGroupHead())
			{
				getGroup()->assignTradeRoute(iData1, iData2);
			}
			break;

		case COMMAND_PROMOTE:
			if (gDLL->getInterfaceIFace()->getHeadSelectedUnit() == this)
			{
				CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_PROMOTE);
				pInfo->setData1(getGroupID());
				gDLL->getInterfaceIFace()->addPopup(pInfo, getOwnerINLINE(), true, true);
			}
			break;

		case COMMAND_PROFESSION:
            ///TKs Test
            // gDLL->getInterfaceIFace()->setDirty(NewYieldAvailable_DIRTY_BIT, true);
            ///Tke
			if (iData1 == -1)
			{
				if (gDLL->getInterfaceIFace()->getHeadSelectedUnit() == this)
				{
					CvPlot* pPlot = plot();
					if (pPlot != NULL)
					{
						CvCity* pCity = pPlot->getPlotCity();
						if (pCity != NULL)
						{
							CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CHOOSE_PROFESSION, pCity->getID(), getID());
							gDLL->getInterfaceIFace()->addPopup(pInfo, getOwnerINLINE(), true, true);
						}
					}
				}
			}
			else
			{
				setProfession((ProfessionTypes) iData1);
			}
			break;

		case COMMAND_CLEAR_SPECIALTY:
			clearSpecialty();
			break;

		case COMMAND_UNLOAD:
			if (iData2 >= 0)
			{
				FAssert(iData1 == getYield());
				FAssert(getYield() != NO_YIELD);
				unloadStoredAmount(iData2);
			}
			else
			{
				unload();
			}
			bCycle = true;
			break;

		case COMMAND_UNLOAD_ALL:
			unloadAll();
			bCycle = true;
			break;

		case COMMAND_LEARN:
			learn();
			bCycle = true;
			break;

		case COMMAND_KING_TRANSPORT:
			kingTransport(false);
			bCycle = true;
			break;

		case COMMAND_ESTABLISH_MISSION:
			establishMission();
			bCycle = true;
			break;

		case COMMAND_SPEAK_WITH_CHIEF:
			if(isGroupHead())
			{
				getGroup()->speakWithChief();
			}
			break;
        ///Tks Med & Return Home
        case COMMAND_HIRE_GUARD:
             hireGuard();
             break;
        case COMMAND_BUILD_TRADINGPOST:
             buildTradingPost(false);
             break;
        case COMMAND_CALL_BANNERS:
             callBanners();
             break;
        case COMMAND_BUILD_HOME:
             buildHome();
             break;
        case COMMAND_ASSIGN_HOME_CITY:
			checkHasHomeCity(false);
			bCycle = true;
			break;
        ///TKe
		case COMMAND_HOTKEY:
			setHotKeyNumber(iData1);
			break;
        ///TKs Med
        case COMMAND_TRAVEL_TO_FAIR:
			crossOcean((UnitTravelStates) iData1);
			break;
        case COMMAND_SAIL_SPICE_ROUTE:
			//if (iData2 != NO_EUROPE)
			//{
				//setSailEurope((EuropeTypes) iData2);
			//}
			crossOcean((UnitTravelStates) iData1, false, (EuropeTypes) iData2);
			break;
        case COMMAND_TRAVEL_SILK_ROAD:
			crossOcean((UnitTravelStates) iData1);
			break;
        case COMMAND_CONVERT_UNIT:
			doKingTransport();
			break;
#ifdef USE_NOBLE_CLASS
        case COMMAND_HOLD_FEAST:
			{
				CvCity* ePlotCity = plot()->getPlotCity();
				if (ePlotCity != NULL)
				{
					int iBanquetAmount = GC.getXMLval(XML_BANQUET_YIELD_AMOUNT);
					ePlotCity->changeYieldStored(YIELD_SHEEP, -iBanquetAmount);
					ePlotCity->changeYieldStored(YIELD_CATTLE, -iBanquetAmount);
					ePlotCity->changeYieldStored(YIELD_WINE, -iBanquetAmount);
					ePlotCity->changeYieldStored(YIELD_ALE, -iBanquetAmount);
					CvPlayer& kPlayer = GET_PLAYER(getOwnerINLINE());
					kPlayer.changeBellsStored(kPlayer.getYieldRate(YIELD_BELLS));
					kPlayer.changeCrossesStored(kPlayer.getYieldRate(YIELD_CROSSES));
				}
			}
            break;
#endif
        ///Tke

		default:
			FAssert(false);
			break;
		}
	}

	if (bCycle)
	{
		if (IsSelected())
		{
			gDLL->getInterfaceIFace()->setCycleSelectionCounter(1);
		}
	}

	if (getGroup() != NULL)
	{
		getGroup()->doDelayedDeath();
	}
}


FAStarNode* CvUnit::getPathLastNode() const
{
	return getGroup()->getPathLastNode();
}


CvPlot* CvUnit::getPathEndTurnPlot() const
{
	return getGroup()->getPathEndTurnPlot();
}

int CvUnit::getPathCost() const
{
	return getGroup()->getPathCost();
}

bool CvUnit::generatePath(const CvPlot* pToPlot, int iFlags, bool bReuse, int* piPathTurns) const
{
	return getGroup()->generatePath(plot(), pToPlot, iFlags, bReuse, piPathTurns);
}


bool CvUnit::canEnterTerritory(PlayerTypes ePlayer, bool bIgnoreRightOfPassage) const
{
	if (ePlayer == NO_PLAYER)
	{
		return true;
	}

	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();

	if (GET_TEAM(getTeam()).isFriendlyTerritory(eTeam))
	{
		return true;
	}

	if (isEnemy(eTeam))
	{
		return true;
	}

	if (isRivalTerritory())
	{
		return true;
	}

	if (alwaysInvisible())
	{
		return true;
	}

	if (GET_PLAYER(getOwnerINLINE()).isAlwaysOpenBorders())
	{
		return true;
	}

	if (GET_PLAYER(ePlayer).isAlwaysOpenBorders())
	{
		return true;
	}

	if (!bIgnoreRightOfPassage)
	{
		if (GET_TEAM(getTeam()).isOpenBorders(eTeam))
		{
			return true;
		}
	}

	return false;
}


bool CvUnit::canEnterArea(PlayerTypes ePlayer, const CvArea* pArea, bool bIgnoreRightOfPassage) const
{
	if (!canEnterTerritory(ePlayer, bIgnoreRightOfPassage))
	{
		return false;
	}

	return true;
}

// Returns the ID of the team to declare war against
TeamTypes CvUnit::getDeclareWarUnitMove(const CvPlot* pPlot) const
{
	FAssert(isHuman());

	if (!pPlot->isVisible(getTeam(), false))
	{
		return NO_TEAM;
	}

	bool bCityThreat = canAttack() && !isNoCityCapture() && getDomainType() == DOMAIN_LAND;
	if (getProfession() != NO_PROFESSION && GC.getProfessionInfo(getProfession()).isScout())
	{
		bCityThreat = false;
	}

	//check territory
	TeamTypes eRevealedTeam = pPlot->getRevealedTeam(getTeam(), false);
	PlayerTypes eRevealedPlayer = pPlot->getRevealedOwner(getTeam(), false);
	if (eRevealedTeam != NO_TEAM)
	{
		if (GET_TEAM(getTeam()).canDeclareWar(pPlot->getTeam()))
		{
			if (!canEnterArea(eRevealedPlayer, pPlot->area()))
			{
				return eRevealedTeam;
			}

			if(getDomainType() == DOMAIN_SEA && !canCargoEnterArea(eRevealedPlayer, pPlot->area(), false) && getGroup()->isAmphibPlot(pPlot))
			{
				return eRevealedTeam;
			}

			if (pPlot->isCity() && bCityThreat)
			{
				if (GET_PLAYER(eRevealedPlayer).isAlwaysOpenBorders())
				{
					return eRevealedTeam;
				}
			}
		}
	}

	//check unit
	if (canMoveInto(pPlot, true, true))
	{
		CvUnit* pUnit = pPlot->plotCheck(PUF_canDeclareWar, getOwnerINLINE(), isAlwaysHostile(pPlot), NO_PLAYER, NO_TEAM, PUF_isVisible, getOwnerINLINE());
		if (pUnit != NULL)
		{
			if (!pPlot->isCity() || bCityThreat)
			{
				return pUnit->getTeam();
			}
		}
	}

	return NO_TEAM;
}

bool CvUnit::canMoveInto(const CvPlot* pPlot, bool bAttack, bool bDeclareWar, bool bIgnoreLoad) const
{
	FAssertMsg(pPlot != NULL, "Plot is not assigned a valid value");

	if (atPlot(pPlot))
	{
		return false;
	}

	if (pPlot->isImpassable())
	{
		if (!canMoveImpassable())
		{
			return false;
		}
	}

	///TKs Med
	if (isHuman() && m_pUnitInfo->isPreventTraveling() && !m_pUnitInfo->isMechUnit())
	{
	    if (plot()->getOwner() == getOwner() && pPlot->getOwner() != getOwner())
	    {
			return false;
	    }
	}

    //if (isBarbarian() && pPlot->isCity())
    //{
        //return false;
    //}

	CvArea *pPlotArea = pPlot->area();
	TeamTypes ePlotTeam = pPlot->getTeam();
	bool bCanEnterArea = canEnterArea(pPlot->getOwnerINLINE(), pPlotArea);
	if (bCanEnterArea)
	{
		if (pPlot->getFeatureType() != NO_FEATURE)
		{
			if (m_pUnitInfo->getFeatureImpassable(pPlot->getFeatureType()))
			{
				if (DOMAIN_SEA != getDomainType() || pPlot->getTeam() != getTeam())  // sea units can enter impassable in own cultural borders
				{
					return false;
				}
			}
		}
		else
		{
			if (m_pUnitInfo->getTerrainImpassable(pPlot->getTerrainType()))
			{
				if (DOMAIN_SEA != getDomainType() || pPlot->getTeam() != getTeam())  // sea units can enter impassable in own cultural borders
				{
					if (bIgnoreLoad || !canLoad(pPlot, true))
					{
						return false;
					}
				}
			}
		}
	}

	if (m_pUnitInfo->getMoves() == 0)
	{
		return false;
	}

	switch (getDomainType())
	{
	case DOMAIN_SEA:
		if (!pPlot->isWater() && !m_pUnitInfo->isCanMoveAllTerrain())
		{
		    ///TKs Med
			if (!pPlot->isFriendlyCity(*this, false) || !pPlot->isCoastalLand())
			{
				return false;
			}
			///Tke
		}
		break;

	case DOMAIN_LAND:
		if (pPlot->isWater() && !m_pUnitInfo->isCanMoveAllTerrain())
		{
			if (bIgnoreLoad || plot()->isWater() || !canLoad(pPlot, false))
			{
				return false;
			}
		}
		///TKs Med
		//if (isAlwaysHostile(NULL) && pPlot->isCity())
		//{
		    //if (pPlot->getTeam() == getTeam())
		   // {
		       // return false;
		   // }
		//}
		///TKe
		break;

	case DOMAIN_IMMOBILE:
		return false;
		break;

	default:
		FAssert(false);
		break;
	}

	if (!bAttack)
	{
		if (isNoCityCapture() && pPlot->isEnemyCity(*this))
		{
			return false;
		}
	}

	if (bAttack)
	{
		if (isMadeAttack() && !isBlitz())
		{
			return false;
		}
	}
	///TKs Med
    if (m_pUnitInfo->isAnimal())
    {
        if (pPlot->isEnemyCity(*this))
        {
            CvCity* pCity = pPlot->getPlotCity();
            if (pCity->getBuildingDefense() > 0)
            {
                return false;
            }
        }
    }
    ///TKe
	if (canAttack())
	{
		if (bAttack || !canCoexistWithEnemyUnit(NO_TEAM))
		{
			if (!isHuman() || (pPlot->isVisible(getTeam(), false)))
			{
				if (pPlot->isVisibleEnemyUnit(this) != bAttack)
				{
					//FAssertMsg(isHuman() || (!bDeclareWar || (pPlot->isVisibleOtherUnit(getOwnerINLINE()) != bAttack)), "hopefully not an issue, but tracking how often this is the case when we dont want to really declare war");
					if (!bDeclareWar || (pPlot->isVisibleOtherUnit(getOwnerINLINE()) != bAttack && !(bAttack && pPlot->getPlotCity() && !isNoCityCapture())))
					{
						return false;
					}


				}
            ///TKs Med
                if (isBarbarian())
                {
                    CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
                    while (pUnitNode != NULL)
                    {
                        CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
                        pUnitNode = pPlot->nextUnitNode(pUnitNode);

                        if (pLoopUnit->getEscortPromotion() != NO_PROMOTION)
                        {
                            return false;
                        }
                        if (!pLoopUnit->isHuman() && !isNative())
                        {
                            return false;
                        }
                    }
                }
                ///TKe
			}
		}
	}
	else
	{
		if (bAttack)
		{
			return false;
		}

		if (!canCoexistWithEnemyUnit(NO_TEAM))
		{
			if (!isHuman() || pPlot->isVisible(getTeam(), false))
			{
				if (pPlot->isEnemyCity(*this))
				{
					return false;
				}

				if (pPlot->isVisibleEnemyUnit(this))
				{
				    ///TKs Med
				   // bool bNoUnitsCanAttack = false;
                    CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
                    while (pUnitNode != NULL)
                    {
                        CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
                        pUnitNode = pPlot->nextUnitNode(pUnitNode);


                        if (pLoopUnit->isBarbarian())
                        {
                            if (pLoopUnit->canAttack())
                            {
                                return false;
                            }
                        }
                    }
					///TKe
				}
			}
		}
	}

	if (isHuman())
	{
		ePlotTeam = pPlot->getRevealedTeam(getTeam(), false);
		bCanEnterArea = canEnterArea(pPlot->getRevealedOwner(getTeam(), false), pPlotArea);
	}

	if (!bCanEnterArea)
	{
		FAssert(ePlotTeam != NO_TEAM);

		if (!(GET_TEAM(getTeam()).canDeclareWar(ePlotTeam)))
		{
			return false;
		}

		if (isHuman())
		{
			if (!bDeclareWar)
			{
				return false;
			}
		}
		else
		{
			if (GET_TEAM(getTeam()).AI_isSneakAttackReady(ePlotTeam))
			{
				if (!(getGroup()->AI_isDeclareWar(pPlot)))
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
	}

	if (GC.getUSE_UNIT_CANNOT_MOVE_INTO_CALLBACK())
	{
		// Python Override
		CyArgsList argsList;
		argsList.add(getOwnerINLINE());	// Player ID
		argsList.add(getID());	// Unit ID
		argsList.add(pPlot->getX());	// Plot X
		argsList.add(pPlot->getY());	// Plot Y
		long lResult=0;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "unitCannotMoveInto", argsList.makeFunctionArgs(), &lResult);

		if (lResult != 0)
		{
			return false;
		}
	}

	return true;
}


bool CvUnit::canMoveOrAttackInto(const CvPlot* pPlot, bool bDeclareWar) const
{
	return (canMoveInto(pPlot, false, bDeclareWar) || canMoveInto(pPlot, true, bDeclareWar));
}


bool CvUnit::canMoveThrough(const CvPlot* pPlot) const
{
	return canMoveInto(pPlot, false, false, true);
}


void CvUnit::attack(CvPlot* pPlot, bool bQuick)
{
	FAssert(canMoveInto(pPlot, true));
	FAssert(getCombatTimer() == 0);

	setAttackPlot(pPlot);

	updateCombat(bQuick);
}

void CvUnit::move(CvPlot* pPlot, bool bShow)
{
	FAssert(canMoveOrAttackInto(pPlot) || isMadeAttack());

	CvPlot* pOldPlot = plot();

	changeMoves(pPlot->movementCost(this, plot()));

	setXY(pPlot->getX_INLINE(), pPlot->getY_INLINE(), true, true, bShow, bShow);

	//change feature
	FeatureTypes featureType = pPlot->getFeatureType();
	if(featureType != NO_FEATURE)
	{
		CvString featureString(GC.getFeatureInfo(featureType).getOnUnitChangeTo());
		if(!featureString.IsEmpty())
		{
			FeatureTypes newFeatureType = (FeatureTypes) GC.getInfoTypeForString(featureString);
			pPlot->setFeatureType(newFeatureType);
		}
	}

	if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
	{
		if (!(pPlot->isOwned()))
		{
			//spawn birds if trees present - JW
			if (featureType != NO_FEATURE)
			{
				if (GC.getASyncRand().get(100) < GC.getFeatureInfo(featureType).getEffectProbability())
				{
					EffectTypes eEffect = (EffectTypes)GC.getInfoTypeForString(GC.getFeatureInfo(featureType).getEffectType());
					gDLL->getEngineIFace()->TriggerEffect(eEffect, pPlot->getPoint(), (float)(GC.getASyncRand().get(360)));
					gDLL->getInterfaceIFace()->playGeneralSound("AS3D_UN_BIRDS_SCATTER", pPlot->getPoint());
				}
			}
		}
	}

	gDLL->getEventReporterIFace()->unitMove(pPlot, this, pOldPlot);
	///TKs Med
	if (!GC.getCivilizationInfo(getCivilizationType()).isWaterStart() && m_pUnitInfo->getLostAtSeaPercent() > 0 && getUnitTravelState() != UNIT_TRAVEL_LOST_AT_SEA && !plot()->isAdjacentToLand())
	{
	    if (movesLeft() == 0)
	    {
            if (m_pUnitInfo->getLostAtSeaPercent() > GC.getGameINLINE().getSorenRandNum(100, "Lost at Sea"))
            {
                int ilost = GC.getGameINLINE().getSorenRandNum(GC.getXMLval(XML_RANDOM_TURNS_LOST_AT_SEA), "Lost at Sea Random");
                setUnitTravelState(UNIT_TRAVEL_LOST_AT_SEA, false);
                setUnitTravelTimer(ilost);
            }
	    }

	}
	///Tke

}

// false if unit is killed
bool CvUnit::jumpToNearestValidPlot()
{
	FAssertMsg(!isAttacking(), "isAttacking did not return false as expected");
	FAssertMsg(!isFighting(), "isFighting did not return false as expected");

	CvCity* pNearestCity = GC.getMapINLINE().findCity(getX_INLINE(), getY_INLINE(), getOwnerINLINE());
	int iBestValue = MAX_INT;
	CvPlot* pBestPlot = NULL;
    ///TKs Med Update 1.1g
    bool bDomainSea = getDomainType() == DOMAIN_SEA;

	for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);
		if (bDomainSea)
		{
		    if (!pLoopPlot->isEuropeAccessable())
		    {
		        continue;
		    }
		}
        ///TKe Update
		if (isValidPlot(pLoopPlot))
		{
			if (canMoveInto(pLoopPlot))
			{
				FAssertMsg(!atPlot(pLoopPlot), "atPlot(pLoopPlot) did not return false as expected");

				if (pLoopPlot->isRevealed(getTeam(), false))
				{
					int iValue = (plotDistance(getX_INLINE(), getY_INLINE(), pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE()) * 2);

					if (pNearestCity != NULL)
					{
						iValue += plotDistance(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), pNearestCity->getX_INLINE(), pNearestCity->getY_INLINE());
					}

					if (pLoopPlot->area() != area())
					{
						iValue *= 3;
					}

					if (iValue < iBestValue)
					{
						iBestValue = iValue;
						pBestPlot = pLoopPlot;
					}
				}
			}
		}
	}

	bool bValid = true;
	if (pBestPlot != NULL)
	{
		setXY(pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE());
	}
	else
	{
		kill(false);
		bValid = false;
	}

	return bValid;
}

bool CvUnit::isValidPlot(const CvPlot* pPlot) const
{
	if (!pPlot->isValidDomainForLocation(*this))
	{
		return false;
	}

	if (!canEnterArea(pPlot->getOwnerINLINE(), pPlot->area()))
	{
		return false;
	}

	TeamTypes ePlotTeam = pPlot->getTeam();
	if (ePlotTeam != NO_TEAM)
	{
		if (pPlot->isCity(true, ePlotTeam) && !canCoexistWithEnemyUnit(ePlotTeam) && isEnemy(ePlotTeam))
		{
			return false;
		}
	}

	return true;
}


bool CvUnit::canAutomate(AutomateTypes eAutomate) const
{
	if (eAutomate == NO_AUTOMATE)
	{
		return false;
	}

	switch (eAutomate)
	{
	case AUTOMATE_BUILD:
		if (workRate(true) <= 0)
		{
			return false;
		}
		break;

	case AUTOMATE_CITY:
		if (workRate(true) <= 0)
		{
			return false;
		}
		if (!plot()->isCityRadius())
		{
			return false;
		}
		if ((plot()->getWorkingCity() == NULL) || plot()->getWorkingCity()->getOwnerINLINE() != getOwnerINLINE())
		{
			return false;
		}
		break;

	case AUTOMATE_EXPLORE:
		if ((!canFight() && (getDomainType() != DOMAIN_SEA)) || (getDomainType() == DOMAIN_IMMOBILE))
		{
			return false;
		}
		break;

	case AUTOMATE_SAIL:
		//TKs Trade Screen
		if (!canAutoCrossOcean(plot()))
		{
			return false;
		}
		break;

	case AUTOMATE_TRANSPORT_ROUTES:
		if (cargoSpace() == 0)
		{
			return false;
		}
		break;

	case AUTOMATE_TRANSPORT_FULL:
		if (cargoSpace() == 0)
		{
			return false;
		}
		break;
     ///Tks Med Return Home
     case AUTOMATE_SAIL_SPICE_ROUTE:
		if (!canAutoCrossOcean(plot(), TRADE_ROUTE_SPICE_ROUTE))
		{
			return false;
		}
		break;
    case AUTOMATE_HUNT:
		if (!canFight() || cargoSpace() == 0)
		{
			return false;
		}
		break;
    case AUTOMATE_RETURN_HOME:
        if (GET_PLAYER(getOwnerINLINE()).getNumCities() <= 0)
		{
		    return false;
		}
		if (isOnMap() && getHomeCity() != NULL)
		{
            CvCity* ePlotCity = plot()->getPlotCity();
            if (ePlotCity != NULL)
            {
                if (getHomeCity() == ePlotCity)
                {
                    return false;
                }
                if (getDomainType() == DOMAIN_LAND && ePlotCity->getArea() != getArea())
                {
                    return false;
                }
            }
		}
        if (cargoSpace() == 0 && !m_pUnitInfo->isTreasure())
		{
		   if (getDomainType() != DOMAIN_SEA)
		   {
                return false;
		   }
		}
		break;
    case AUTOMATE_IMMIGRATION:
        if (GET_PLAYER(getOwnerINLINE()).getNumCities() <= 0)
		{
		    return false;
		}
		if (isOnMap() && getHomeCity() != NULL)
		{
            CvCity* ePlotCity = plot()->getPlotCity();
            if (ePlotCity != NULL)
            {
                if (getHomeCity() == ePlotCity)
                {
                    return false;
                }
            }
		}
        if (cargoSpace() == 0 && !m_pUnitInfo->isTreasure())
		{
		   if (getDomainType() != DOMAIN_SEA)
		   {
                return false;
		   }
		}
		break;
    case AUTOMATE_TRAVEL_FAIR:
        {
            if (GET_PLAYER(getOwnerINLINE()).getNumCities() <= 0 || GC.getCivilizationInfo(getCivilizationType()).isWaterStart())
            {
                return false;
            }
            //if (!GET_PLAYER(getOwnerINLINE()).getHasTradeRouteType(TRADE_ROUTE_FAIR))
		    //{
		        /*if (GC.getXMLval(XML_CHEAT_TRAVEL_ALL) == 0)
		        {
                    return false;
		        }*/
		    //}
            if (plot()->isCity())
            {
                return false;
            }
            if (cargoSpace() == 0)
            {
               return false;
            }
            if (getDomainType() == DOMAIN_SEA)
            {
                return false;
            }
            if (m_pUnitInfo->isPreventTraveling())
            {
                return false;
            }

        }
		break;
    case AUTOMATE_TRAVEL_SILK_ROAD:
        if (!canAutoCrossOcean(plot(), TRADE_ROUTE_SILK_ROAD))
		{
			return false;
		}
        /*if (!GET_PLAYER(getOwnerINLINE()).getHasTradeRouteType(TRADE_ROUTE_SILK_ROAD))
        {
            if (GC.getXMLval(XML_CHEAT_TRAVEL_ALL) == 0)
            {
                return false;
            }
        }
        if (cargoSpace() == 0)
        {
           return false;
        }
        if (getDomainType() != DOMAIN_LAND)
        {
            return false;
        }*/
		break;
    ///TKe
	case AUTOMATE_FULL:
		if (!GC.getGameINLINE().isDebugMode())
		{
			return false;
		}
		break;

	default:
		FAssert(false);
		break;
	}

	return true;
}


void CvUnit::automate(AutomateTypes eAutomate)
{
	if (canAutomate(eAutomate))
	{
		getGroup()->setAutomateType(eAutomate);
	}
}


bool CvUnit::canScrap() const
{
	if (plot()->isFighting())
	{
		return false;
	}

	return true;
}


void CvUnit::scrap()
{
	if (!canScrap())
	{
		return;
	}

	kill(true);
}


bool CvUnit::canGift(bool bTestVisible, bool bTestTransport)
{
	CvPlot* pPlot = plot();
	CvUnit* pTransport = getTransportUnit();
	CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());

	if (!(pPlot->isOwned()))
	{
		return false;
	}

	if (pPlot->getOwnerINLINE() == getOwnerINLINE())
	{
		return false;
	}

	if (!GET_PLAYER(pPlot->getOwnerINLINE()).isProfessionValid(getProfession(), getUnitType()))
	{
		return false;
	}

	if (pPlot->isVisibleEnemyUnit(this))
	{
		return false;
	}

	if (pPlot->isVisibleEnemyUnit(pPlot->getOwnerINLINE()))
	{
		return false;
	}

	if (!pPlot->isValidDomainForLocation(*this) && NULL == pTransport)
	{
		return false;
	}

	if (hasCargo())
	{
		CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = pPlot->nextUnitNode(pUnitNode);

			if (pLoopUnit->getTransportUnit() == this)
			{
				if (!pLoopUnit->canGift(false, false))
				{
					return false;
				}
			}
		}
	}

	if (bTestTransport)
	{
		if (pTransport != NULL && pTransport->getTeam() != pPlot->getTeam())
		{
			return false;
		}
	}

	if (!bTestVisible)
	{
		if (!(GET_PLAYER(pPlot->getOwnerINLINE()).AI_acceptUnit(this)))
		{
			return false;
		}
	}

	if (atWar(pPlot->getTeam(), getTeam()))
	{
		return false;
	}

	// to shut down free units from king exploit
	if (kOwner.getNumCities() == 0)
	{
		return false;
	}

	// to shut down free ship from king exploit
	if (kOwner.getParent() != NO_PLAYER)
	{
		CvPlayer& kEurope = GET_PLAYER(kOwner.getParent());
		if (kEurope.isAlive() && kEurope.isEurope() && !::atWar(getTeam(), kEurope.getTeam()) && getDomainType() == DOMAIN_SEA)
		{
			bool bHasOtherShip = false;
			int iLoop;
			for (CvUnit* pLoopUnit = kOwner.firstUnit(&iLoop); pLoopUnit != NULL && !bHasOtherShip; pLoopUnit = kOwner.nextUnit(&iLoop))
			{
				if (pLoopUnit != this && pLoopUnit->getDomainType() == DOMAIN_SEA)
				{
					bHasOtherShip = true;
				}
			}

			if (!bHasOtherShip )
			{
				return false;
			}
		}
	}

	return true;
}


void CvUnit::gift(bool bTestTransport)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pGiftUnit;
	CvUnit* pLoopUnit;
	CvPlot* pPlot;
	CvWString szBuffer;
	PlayerTypes eOwner;

	if (!canGift(false, bTestTransport))
	{
		return;
	}

	pPlot = plot();

	pUnitNode = pPlot->headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if (pLoopUnit->getTransportUnit() == this)
		{
			pLoopUnit->gift(false);
		}
	}

	FAssertMsg(plot()->getOwnerINLINE() != NO_PLAYER, "plot()->getOwnerINLINE() is not expected to be equal with NO_PLAYER");
	pGiftUnit = GET_PLAYER(plot()->getOwnerINLINE()).initUnit(getUnitType(), getProfession(), getX_INLINE(), getY_INLINE(), AI_getUnitAIType(), getFacingDirection(false), getYieldStored());

	FAssertMsg(pGiftUnit != NULL, "GiftUnit is not assigned a valid value");

	eOwner = getOwnerINLINE();

	pGiftUnit->convert(this, true);

	int iUnitValue = 0;
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		iUnitValue += pGiftUnit->getUnitInfo().getYieldCost(iYield);
	}
	GET_PLAYER(pGiftUnit->getOwnerINLINE()).AI_changePeacetimeGrantValue(eOwner, iUnitValue / 5);

	szBuffer = gDLL->getText("TXT_KEY_MISC_GIFTED_UNIT_TO_YOU", GET_PLAYER(eOwner).getNameKey(), pGiftUnit->getNameKey());
	gDLL->getInterfaceIFace()->addMessage(pGiftUnit->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_UNITGIFTED", MESSAGE_TYPE_INFO, pGiftUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pGiftUnit->getX_INLINE(), pGiftUnit->getY_INLINE(), true, true);

	// Python Event
	gDLL->getEventReporterIFace()->unitGifted(pGiftUnit, getOwnerINLINE(), plot());
}


bool CvUnit::canLoadUnit(const CvUnit* pTransport, const CvPlot* pPlot, bool bCheckCity) const
{
	FAssert(pTransport != NULL);
	FAssert(pPlot != NULL);

	if (getUnitTravelState() != pTransport->getUnitTravelState())
	{
		return false;
	}

	if (pTransport == this)
	{
		return false;
	}

	if (getTransportUnit() == pTransport)
	{
		return false;
	}

	if (pTransport->getTeam() != getTeam())
	{
		return false;
	}

	if (getCargo() > 0)
	{
		return false;
	}

	if (pTransport->isCargo())
	{
		return false;
	}

    ///TKs Med
    if (GC.getUnitInfo(getUnitType()).isFound() && GC.getUnitInfo(pTransport->getUnitType()).isFound())
	{
		return false;
	}

	if (GC.getUnitInfo(pTransport->getUnitType()).isFound() && canMove())
	{
		return false;
	}

	if (cargoSpace() > 0 && getDomainType() == pTransport->getDomainType())
	{
	    return false;
	}

	if (getDomainType() == DOMAIN_SEA && pTransport->getDomainType() == DOMAIN_LAND)
	{
	    return false;
	}

    ///Tke
	if (!(pTransport->cargoSpaceAvailable(getSpecialUnitType(), getDomainType())))
	{
		return false;
	}

	if (pTransport->cargoSpace() < getUnitInfo().getRequiredTransportSize())
	{
		return false;
	}

	if (!(pTransport->atPlot(pPlot)))
	{
		return false;
	}

	if (bCheckCity && !pPlot->isCity(true))
	{
		return false;
	}

	return true;
}


void CvUnit::loadUnit(CvUnit* pTransport)
{
	if (!canLoadUnit(pTransport, plot(), true))
	{
		return;
	}

	setTransportUnit(pTransport);
}

bool CvUnit::shouldLoadOnMove(const CvPlot* pPlot) const
{
	if (isCargo())
	{
		return false;
	}

	if (getYield() != NO_YIELD)
	{
		CvCity* pCity = pPlot->getPlotCity();
		if (pCity != NULL && GET_PLAYER(getOwnerINLINE()).canUnloadYield(pCity->getOwnerINLINE()))
		{
			return false;
		}
	}

	if (getUnitTravelState() != NO_UNIT_TRAVEL_STATE)
	{
		return false;
	}

	if (!pPlot->isValidDomainForLocation(*this))
	{
		return true;
	}

	if (m_pUnitInfo->getTerrainImpassable(pPlot->getTerrainType()))
	{
		return true;
	}

	return false;
}

int CvUnit::getLoadedYieldAmount(YieldTypes eYield) const
{
	CvPlot* pPlot = plot();
	if (pPlot == NULL)
	{
		return 0;
	}

	int iTotal = 0;
	//check if room in other cargo
	for (int i=0;i<pPlot->getNumUnits();i++)
	{
		CvUnit* pLoopUnit = pPlot->getUnitByIndex(i);
		if(pLoopUnit != NULL)
		{
			if(pLoopUnit->getTransportUnit() == this)
			{
				if(pLoopUnit->getYield() == eYield)
				{
					iTotal += pLoopUnit->getYieldStored();
				}
			}
		}
	}

	return iTotal;
}

int CvUnit::getLoadYieldAmount(YieldTypes eYield) const
{
	CvPlot* pPlot = plot();
	if (pPlot == NULL)
	{
		return 0;
	}

	bool bFull = isFull();
	if (!bFull)
	{
		UnitClassTypes eUnitClass = (UnitClassTypes) GC.getYieldInfo(eYield).getUnitClass();
		FAssert(eUnitClass != NO_UNITCLASS);
		if (eUnitClass != NO_UNITCLASS)
		{
			UnitTypes eUnit = (UnitTypes) GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(eUnitClass);
			if (eUnit != NO_UNIT)
			{
				CvUnitInfo& kUnit = GC.getUnitInfo(eUnit);
				if (!cargoSpaceAvailable((SpecialUnitTypes) kUnit.getSpecialUnitType(), (DomainTypes) kUnit.getDomainType()))
				{
					bFull = true;
				}

				if (cargoSpace() < kUnit.getRequiredTransportSize())
				{
					bFull = true;
				}
			}
		}
	}

	if (!bFull)
	{
		return GC.getGameINLINE().getCargoYieldCapacity();
	}

	//check if room in other cargo
	for (int i=0;i<pPlot->getNumUnits();i++)
	{
		CvUnit* pLoopUnit = pPlot->getUnitByIndex(i);
		if(pLoopUnit != NULL)
		{
			if(pLoopUnit->getTransportUnit() == this)
			{
				if(pLoopUnit->getYield() == eYield)
				{
					int iSpaceAvailable = GC.getGameINLINE().getCargoYieldCapacity() - pLoopUnit->getYieldStored();
					//check if space available
					if(iSpaceAvailable > 0)
					{
						return iSpaceAvailable;
					}
				}
			}
		}
	}

	return 0;
}

bool CvUnit::canLoadYields(const CvPlot* pPlot, bool bTrade) const
{
	for (int iYield = 0; iYield <  NUM_YIELD_TYPES; ++iYield)
	{
		if(canLoadYield(pPlot, (YieldTypes) iYield, bTrade))
		{
			return true;
		}
	}

	return false;
}

bool CvUnit::canLoadYield(const CvPlot* pPlot, YieldTypes eYield, bool bTrade) const
{
	if (eYield == NO_YIELD)
	{
		FAssert(!bTrade);
		return canLoadYields(pPlot, bTrade);
	}

	CvYieldInfo& kYield = GC.getYieldInfo(eYield);

	if (kYield.isCargo() && !isCargo())
	{
		if (pPlot != NULL)
		{
			CvCity* pCity = pPlot->getPlotCity();
			if (NULL != pCity)
			{
				if(GET_PLAYER(getOwnerINLINE()).canLoadYield(pCity->getOwnerINLINE()) || bTrade)
				{
					if (kYield.isCargo())
					{
						if (pCity->getYieldStored(eYield) > 0)
						{
							if (getLoadYieldAmount(eYield) > 0)
							{
								return true;
							}
						}
					}
				}
			}
		}
	}

	return false;
}

void CvUnit::loadYield(YieldTypes eYield, bool bTrade)
{
	if (!canLoadYield(plot(), eYield, bTrade))
	{
		return;
	}

	loadYieldAmount(eYield, getMaxLoadYieldAmount(eYield), bTrade);
}

void CvUnit::loadYieldAmount(YieldTypes eYield, int iAmount, bool bTrade)
{
	if (!canLoadYield(plot(), eYield, bTrade))
	{
		return;
	}

	if (iAmount <= 0 || iAmount > getMaxLoadYieldAmount(eYield))
	{
		return;
	}

	CvUnit* pUnit = plot()->getPlotCity()->createYieldUnit(eYield, getOwnerINLINE(), iAmount);
	FAssert(pUnit != NULL);
	if(pUnit != NULL)
	{
		pUnit->setTransportUnit(this);
	}
}

int CvUnit::getMaxLoadYieldAmount(YieldTypes eYield) const
{
	int iMaxAmount = GC.getGameINLINE().getCargoYieldCapacity();
	iMaxAmount = std::min(iMaxAmount, getLoadYieldAmount(eYield));
	CvCity* pCity = plot()->getPlotCity();
	///TKs Med
	if (isOnMap() && pCity != NULL)
	///TKe
	{
		int iMaxAvailable = pCity->getYieldStored(eYield);
		if (!isHuman() || isAutomated())
		{
			// transport feeder - start - Nightinggale
			//iMaxAvailable -= pCity->getMaintainLevel(eYield);
			iMaxAvailable -= pCity->getAutoMaintainThreshold(eYield);
			// transport feeder - end - Nightinggale
		}
		iMaxAmount = std::min(iMaxAmount, iMaxAvailable);
	}

	return std::max(iMaxAmount, 0);
}

bool CvUnit::canTradeYield(const CvPlot* pPlot) const
{
	FAssert(pPlot != NULL);
	CvCity* pCity = pPlot->getPlotCity();
	if (pCity == NULL)
	{
		return false;
	}

	if (pCity->getOwnerINLINE() == getOwnerINLINE())
	{
		return false;
	}

	if (cargoSpace() == 0)
	{
		return false;
	}

	if (!canMove())
	{
		return false;
	}

	///Tks Med
	if (isCargo())
	{
	    return false;
	}
	if (isHuman() && m_pUnitInfo->isMechUnit())
	{
	    return false;
	}
	///Tke

	//check if we have any yield cargo
	bool bYieldFound = false;
	if (hasCargo())
	{
		for (int i=0;i<pPlot->getNumUnits();i++)
		{
			CvUnit* pLoopUnit = pPlot->getUnitByIndex(i);
			if (pLoopUnit != NULL)
			{
				if (pLoopUnit->getTransportUnit() == this)
				{
					if (pLoopUnit->getYield() != NO_YIELD)
					{
						bYieldFound = true;
						break;
					}
				}
			}
		}
	}

	//check if the city has any cargo that we can fit
	if(!bYieldFound)
	{
		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			YieldTypes eYield = (YieldTypes) iYield;
			if ((pCity->getYieldStored(eYield) > 0) && (getLoadYieldAmount(eYield) > 0))
			{
				bYieldFound = true;
				break;
			}
		}
	}

	if (!bYieldFound)
	{
		return false;
	}

	return true;
}

void CvUnit::tradeYield()
{
	if(!canTradeYield(plot()))
	{
		return;
	}

	PlayerTypes eOtherPlayer = plot()->getOwnerINLINE();

	//both human
	if (GET_PLAYER(getOwnerINLINE()).isHuman() && GET_PLAYER(eOtherPlayer).isHuman())
	{
		if (GC.getGameINLINE().isPbem() || GC.getGameINLINE().isHotSeat() || (GC.getGameINLINE().isPitboss() && !gDLL->isConnected(GET_PLAYER(eOtherPlayer).getNetID())))
		{
			if (gDLL->isMPDiplomacy())
			{
				gDLL->beginMPDiplomacy(eOtherPlayer, false, false, getIDInfo());
			}
		}
		else if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
		{
			//clicking the flashing goes through CvPlayer::contact where it sends the response message
			gDLL->sendContactCiv(NETCONTACT_INITIAL, eOtherPlayer, getID());
		}
	}
	else if(GET_PLAYER(getOwnerINLINE()).isHuman()) //we're human contacting them
	{
		CvDiploParameters* pDiplo = new CvDiploParameters(eOtherPlayer);
		pDiplo->setDiploComment((DiploCommentTypes) GC.getInfoTypeForString("AI_DIPLOCOMMENT_TRADING"));
		pDiplo->setTransport(getIDInfo());
		pDiplo->setCity(plot()->getPlotCity()->getIDInfo());
		gDLL->beginDiplomacy(pDiplo, getOwnerINLINE());
	}
	else if(!isHuman() && GET_PLAYER(eOtherPlayer).isHuman()) //AI contacting  human Player
	{
		CvDiploParameters* pDiplo = new CvDiploParameters(getOwnerINLINE());
		pDiplo->setDiploComment((DiploCommentTypes) GC.getInfoTypeForString("AI_DIPLOCOMMENT_AI_PEDDLER_TRADING"));
		pDiplo->setTransport(getIDInfo());
		gDLL->beginDiplomacy(pDiplo, eOtherPlayer);
	}
	else if(GET_PLAYER(eOtherPlayer).isHuman()) //they're human contacting us
	{
		CvDiploParameters* pDiplo = new CvDiploParameters(getOwnerINLINE());
		pDiplo->setDiploComment((DiploCommentTypes) GC.getInfoTypeForString("AI_DIPLOCOMMENT_TRADING"));
		pDiplo->setTransport(getIDInfo());
		gDLL->beginDiplomacy(pDiplo, getOwnerINLINE());
	}
	else //both AI
	{
		FAssertMsg(false, "Don't go through here. Implement deals directly.");
	}
}

bool CvUnit::canClearSpecialty() const
{
	if (m_pUnitInfo->getTeacherWeight() <= 0)
	{
		return false;
	}
    ///TKs Invention Core Mod v 1.0
	UnitTypes eUnit = (UnitTypes) GET_PLAYER(getOwnerINLINE()).getDefaultPopUnit();
	///TKe
	if (eUnit == NO_UNIT)
	{
		return false;
	}

	return true;
}

void CvUnit::clearSpecialty()
{
	if (!canClearSpecialty())
	{
		return;
	}

	bool bLocked = isColonistLocked();
	///TKs Invention Core Mod v 1.0
	UnitTypes eUnit = (UnitTypes) GET_PLAYER(getOwnerINLINE()).getDefaultPopUnit();

	CvUnit* pNewUnit = GET_PLAYER(getOwnerINLINE()).initUnit(eUnit, NO_PROFESSION, getX_INLINE(), getY_INLINE(), AI_getUnitAIType());
	FAssert(pNewUnit != NULL);
    PromotionTypes eHomeBoy = (PromotionTypes) GC.getXMLval(XML_PROMOTION_BUILD_HOME);
    if (isHasRealPromotion(eHomeBoy))
    {
        setHasRealPromotion(eHomeBoy, false);
    }
    ///TKe
	CvCity *pCity = GET_PLAYER(getOwnerINLINE()).getPopulationUnitCity(getID());
	if (pCity != NULL)
	{
		pNewUnit->convert(this, false);
		pCity->replaceCitizen(pNewUnit->getID(), getID(), false);
		pNewUnit->setColonistLocked(bLocked);
		pCity->removePopulationUnit(this, true, NO_PROFESSION);
	}
	else
	{
		pNewUnit->convert(this, true);
	}
}

bool CvUnit::canAutoCrossOcean(const CvPlot* pPlot, TradeRouteTypes eTradeRouteType, bool bAIForce) const
{
    ///TKe MEd
	if (cargoSpace() <= 0)
	{
        return false;
	}

	if (getTransportUnit() != NULL)
	{
		return false;
	}

	if (m_pUnitInfo->isPreventTraveling())
	{
	    return false;
	}

	/*if (getUnitTravelState() == NO_UNIT_TRAVEL_STATE && !canMove())
	{
		return false;
	}*/
	//bool bWaterRoute = false;
	if (pPlot != NULL && pPlot->isEurope())
	{
		//bWaterRoute = (GC.getEuropeInfo((EuropeTypes)pPlot->getEurope()).getMinLandDistance() > 0);
		if (!GC.getEuropeInfo((EuropeTypes)pPlot->getEurope()).getDomainsValid(DOMAIN_SEA))
		{
			if (getDomainType() == DOMAIN_SEA)
			{
				return false;
			}
		}
		if (!GC.getEuropeInfo((EuropeTypes)pPlot->getEurope()).getDomainsValid(DOMAIN_LAND))
		{
			if (getDomainType() == DOMAIN_LAND)
			{
				return false;
			}
		}
	}

	if (eTradeRouteType == TRADE_ROUTE_EUROPE || eTradeRouteType == NO_TRADE_ROUTES)
	{
        if (canCrossOcean(pPlot, UNIT_TRAVEL_STATE_TO_EUROPE, eTradeRouteType, bAIForce))
        {
            return false;
        }
        if (!GC.getCivilizationInfo(getCivilizationType()).isWaterStart())
        {
            return false;
        }
	}

///TKe
	if (!GET_PLAYER(getOwnerINLINE()).canTradeWithEurope())
	{
		return false;
	}

	/*if (getDomainType() != DOMAIN_SEA)
	{
		return false;
	}*/

	if (!pPlot->isEuropeAccessable())
	{
		return false;
	}

	return true;
}
///Tks Med
bool CvUnit::canAutoSailTradeScreen(const CvPlot* pPlot, EuropeTypes eTradeScreenType, bool bAIForce) const
{
	FAssert(eTradeScreenType != NO_EUROPE);
	if (eTradeScreenType == NO_EUROPE)
	{
		return false;
	}

	if (cargoSpace() <= 0)
	{
        return false;
	}

	if (getTransportUnit() != NULL)
	{
		return false;
	}

	if (m_pUnitInfo->isPreventTraveling())
	{
	    return false;
	}

	if (!GC.getLeaderHeadInfo(GET_PLAYER(getOwner()).getLeaderType()).isTradeScreenAllowed(eTradeScreenType))
	{
		return false;
	}

	FAssert(pPlot != NULL);
	if (pPlot == NULL)
	{
		return false;
	}

	//bool bWaterRoute = false;
	if (GC.getEuropeInfo(eTradeScreenType).isAIonly())
	{
		if (isHuman())
		{
			return false;
		}
	}

	if (GC.getEuropeInfo(eTradeScreenType).isRequiresTech())
	{
		if (!GET_PLAYER(getOwnerINLINE()).getHasTradeRouteType(eTradeScreenType))
		{
			return false;
		}
	}

	//bWaterRoute = (GC.getEuropeInfo(eTradeScreenType).getMinLandDistance() > 0);
	if (!GC.getEuropeInfo(eTradeScreenType).getDomainsValid(DOMAIN_SEA))
	{
		if (getDomainType() == DOMAIN_SEA)
		{
			return false;
		}
	}
	if (!GC.getEuropeInfo(eTradeScreenType).getDomainsValid(DOMAIN_LAND))
	{
		if (getDomainType() == DOMAIN_LAND)
		{
			return false;
		}
	}

	if (canCrossOcean(pPlot, UNIT_TRAVEL_STATE_TO_EUROPE, NO_TRADE_ROUTES, bAIForce, eTradeScreenType))
    {
        return false;
    }

	if (pPlot->getDistanceToTradeScreen(eTradeScreenType) == MAX_SHORT)
	{
		return false;
	}

	return true;
}

bool CvUnit::canCrossOcean(const CvPlot* pPlot, UnitTravelStates eNewState, TradeRouteTypes eTradeRouteType, bool bAIForce, EuropeTypes eEuropeTradeRoute) const
{
	if (cargoSpace() <= 0)
	{
        return false;
	}

	if (getTransportUnit() != NULL)
	{
		return false;
	}

	if (m_pUnitInfo->isPreventTraveling())
	{
	    return false;
	}
	
	if (!GC.getLeaderHeadInfo(GET_PLAYER(getOwner()).getLeaderType()).isTradeScreenAllowed(eEuropeTradeRoute))
	{
		return false;
	}

	switch (getUnitTravelState())
	{
	case NO_UNIT_TRAVEL_STATE:
		{
			if (eNewState != UNIT_TRAVEL_STATE_TO_EUROPE)
			{
				return false;
			}

			FAssert(pPlot != NULL);

			//bool bWaterRoute = false;
			if (isHuman())
			{
				
				FAssert(eEuropeTradeRoute < GC.getNumEuropeInfos());
				FAssert(eEuropeTradeRoute >= -1);

				if (eEuropeTradeRoute == NO_EUROPE)
				{
					return false;
				}

				if (GC.getEuropeInfo(eEuropeTradeRoute).isAIonly())
				{
					return false;
				}

				if (GC.getEuropeInfo(eEuropeTradeRoute).isRequiresTech())
				{
					if (!GET_PLAYER(getOwnerINLINE()).getHasTradeRouteType(eEuropeTradeRoute))
					{
						return false;
					}
				}

				//bWaterRoute = (GC.getEuropeInfo(eEuropeTradeRoute).getMinLandDistance() > 0);
				if (!GC.getEuropeInfo(eEuropeTradeRoute).getDomainsValid(DOMAIN_SEA))
				{
					if (getDomainType() == DOMAIN_SEA)
					{
						return false;
					}
				}
				if (!GC.getEuropeInfo(eEuropeTradeRoute).getDomainsValid(DOMAIN_LAND))
				{
					if (getDomainType() == DOMAIN_LAND)
					{
						return false;
					}
				}

				if (GC.getEuropeInfo(eEuropeTradeRoute).isNoEuropePlot())
				{
					CvCity* pPlotCity = pPlot->getPlotCity();
					if (pPlotCity != NULL && GC.getEuropeInfo(eEuropeTradeRoute).getCityRequiredBuilding() != NO_BUILDINGCLASS)
					{
						if (!pPlotCity->isHasBuildingClass((BuildingClassTypes)GC.getEuropeInfo(eEuropeTradeRoute).getCityRequiredBuilding()))
						{
							return false;
						}
					}

					if (GC.getEuropeInfo(eEuropeTradeRoute).isLeaveFromBarbarianCity())
					{
						if (pPlotCity != NULL)
						{
							if (!pPlotCity->isNative())
							{
								return false;
							}
						}
						else
						{
							return false;
						}
					}

					if (GC.getEuropeInfo(eEuropeTradeRoute).isLeaveFromForeignCity())
					{
						if (pPlotCity != NULL)
						{
							if (pPlotCity->isNative() || pPlotCity->getOwner() == getOwner())
							{
								return false;
							}
						}
						else
						{
							return false;
						}
					}

					if (GC.getEuropeInfo(eEuropeTradeRoute).isLeaveFromOwnedCity())
					{
						if (pPlotCity != NULL)
						{
							if (pPlotCity->getOwner() != getOwner())
							{
								return false;
							}
						}
						else
						{
							return false;
						}
					}

					if (GC.getEuropeInfo(eEuropeTradeRoute).isLeaveFromAnyCity())
					{
						if (pPlotCity == NULL)
						{
							return false;
						}
					}

					return true;
				}

				if (!pPlot->isEurope())
				{
					return false;
				}

				if (!pPlot->isTradeScreenAccessPlot(eEuropeTradeRoute))
				{
					return false;
				}

			}

			return true;
		}
		break;
	case UNIT_TRAVEL_STATE_IN_EUROPE:
		if (eNewState != UNIT_TRAVEL_STATE_FROM_EUROPE)
		{
			return false;
		}
		break;
    case UNIT_TRAVEL_STATE_IN_SPICE_ROUTE:
		if (eNewState == UNIT_TRAVEL_STATE_FROM_SPICE_ROUTE)
		{
			return true;
		}

		break;
    case UNIT_TRAVEL_STATE_IN_SILK_ROAD:
		if (eNewState == UNIT_TRAVEL_STATE_FROM_SILK_ROAD)
		{
			return true;
		}
		break;
    case UNIT_TRAVEL_STATE_IN_TRADE_FAIR:
		if (eNewState == UNIT_TRAVEL_STATE_FROM_TRADE_FAIR)
		{
			return true;
		}
		break;
	default:
		FAssertMsg(false, "Invalid trip");
		return false;
		break;
	}

	FAssert(pPlot != NULL);
	if (!pPlot->isEurope() && !isHuman())
	{
		return false;
	}

	return true;
}
///Tke
///TKs
void CvUnit::crossOcean(UnitTravelStates eNewState, bool bAIForce, EuropeTypes eTradeMarket)
{
	
 

	if (!bAIForce && !canCrossOcean(plot(), eNewState, NO_TRADE_ROUTES, false, eTradeMarket))
	{
		return;
	}

	int iTravelTime = 0;
    if (plot()->isEurope() != false)
    {
        if (eTradeMarket != NO_EUROPE)
        {
            CvPlot* pStartingTradePlot = GET_PLAYER(getOwnerINLINE()).getStartingTradeRoutePlot(eTradeMarket);
            if (pStartingTradePlot == NULL)
            {
                GET_PLAYER(getOwnerINLINE()).setStartingTradeRoutePlot(plot(), eTradeMarket);
            }
        }

        iTravelTime = GC.getEuropeInfo(plot()->getEurope()).getTripLength();
    }
    else
    {

        iTravelTime = GC.getEuropeInfo((EuropeTypes)GC.getXMLval(XML_EUROPE_EAST)).getTripLength();
    }
	if (eTradeMarket != NO_EUROPE)
	{
		iTravelTime += GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTradeRouteTripLength(eTradeMarket);
	}
	iTravelTime *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
	iTravelTime /= 100;

	for (int iTrait = 0; iTrait < GC.getNumTraitInfos(); ++iTrait)
	{
		TraitTypes eTrait = (TraitTypes) iTrait;
		if (GET_PLAYER(getOwnerINLINE()).hasTrait(eTrait))
		{
			iTravelTime *= 100 + GC.getTraitInfo(eTrait).getEuropeTravelTimeModifier();
			iTravelTime /= 100;
		}
	}

	setUnitTravelState(eNewState, false);
	//TK **TradeRoute**
	setUnitTradeMarket(eTradeMarket);

	if (iTravelTime > 0)
	{
		setUnitTravelTimer(iTravelTime);
	}
	else
	{
		setUnitTravelTimer(1);
		doUnitTravelTimer();
		finishMoves();
	}
}
///TKe

bool CvUnit::canLoad(const CvPlot* pPlot, bool bCheckCity) const
{
	PROFILE_FUNC();

	FAssert(pPlot != NULL);

	CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if (canLoadUnit(pLoopUnit, pPlot, bCheckCity))
		{
			return true;
		}
	}

	return false;
}


bool CvUnit::load(bool bCheckCity)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvPlot* pPlot;
	int iPass;

	if (!canLoad(plot(), bCheckCity))
	{
		return true;
	}

	pPlot = plot();

	for (iPass = 0; iPass < 2; iPass++)
	{
		pUnitNode = pPlot->headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = pPlot->nextUnitNode(pUnitNode);

			if (canLoadUnit(pLoopUnit, pPlot, bCheckCity))
			{
				if ((iPass == 0) ? (pLoopUnit->getOwnerINLINE() == getOwnerINLINE()) : (pLoopUnit->getTeam() == getTeam()))
				{
					if (!setTransportUnit(pLoopUnit))
					{
						return false;
					}
					break;
				}
			}
		}

		if (isCargo())
		{
			break;
		}
	}

	return true;
}


bool CvUnit::canUnload() const
{
	if (getTransportUnit() == NULL)
	{
		return false;
	}

	if (!plot()->isValidDomainForLocation(*this))
	{
		return false;
	}

	YieldTypes eYield = getYield();
	if (eYield != NO_YIELD)
	{
		CvCity* pCity = plot()->getPlotCity();
		FAssert(pCity != NULL);
		if (pCity == NULL || !GET_PLAYER(getOwnerINLINE()).canUnloadYield(pCity->getOwnerINLINE()))
		{
			return false;
		}
	}

	return true;
}


void CvUnit::unload()
{
	if (!canUnload())
	{
		return;
	}

	setTransportUnit(NULL);
}

// returns true if the unit is still alive
void CvUnit::unloadStoredAmount(int iAmount)
{
	if (!canUnload())
	{
		return;
	}

	FAssert(iAmount <= getYieldStored());
	if (iAmount > getYieldStored())
	{
		return;
	}

	FAssert(isGoods());

	doUnloadYield(iAmount);
}

void CvUnit::doUnloadYield(int iAmount)
{
	YieldTypes eYield = getYield();
	FAssert(eYield != NO_YIELD);
	if (eYield == NO_YIELD)
	{
		return;
	}

	if (getYieldStored() == 0)
	{
		return;
	}

	CvUnit* pUnloadingUnit = this;
	if (iAmount < getYieldStored())
	{
		UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getYieldInfo(eYield).getUnitClass());
		if (NO_UNIT != eUnit)
		{
			pUnloadingUnit = GET_PLAYER(getOwnerINLINE()).initUnit(eUnit, getProfession(), getX_INLINE(), getY_INLINE(), NO_UNITAI, NO_DIRECTION, iAmount);
			FAssert(pUnloadingUnit != NULL);
			setYieldStored(getYieldStored() - iAmount);
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		}
	}


	CvCity* pCity = plot()->getPlotCity();
	if (pCity != NULL)
	{
		pCity->changeYieldStored(eYield, pUnloadingUnit->getYieldStored());
		pCity->AI_changeTradeBalance(eYield, iAmount);
		if (pCity->AI_getDesiredYield() == eYield)
		{
		    ///Tks Med
			if (iAmount > GC.getGameINLINE().getSorenRandNum(pCity->getMaxYieldCapacity(eYield), "change desired yield"))
			{
				pCity->AI_assignDesiredYield();
			}
			///Tke
		}
		pUnloadingUnit->setYieldStored(0);
	}

}

bool CvUnit::canUnloadAll() const
{
	if (getCargo() == 0)
	{
		return false;
	}

	CvPlot* pPlot = plot();
	if(pPlot == NULL)
	{
		return false;
	}

	CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if (pLoopUnit->getTransportUnit() == this)
		{
			if(pLoopUnit->canUnload())
			{
				return true;
			}
		}
	}

	return false;
}


void CvUnit::unloadAll()
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvPlot* pPlot;

	if (!canUnloadAll())
	{
		return;
	}

	pPlot = plot();

	pUnitNode = pPlot->headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if (pLoopUnit->getTransportUnit() == this)
		{
			if (pLoopUnit->canUnload())
			{
				pLoopUnit->setTransportUnit(NULL);
			}
			else
			{
				FAssert(isHuman());
				pLoopUnit->getGroup()->setActivityType(ACTIVITY_AWAKE);
			}
		}
	}
}

bool CvUnit::canLearn() const
{
	UnitTypes eUnitType = getLearnUnitType(plot());
	if(eUnitType == NO_UNIT)
	{
		return false;
	}
	///TKs TODO 
	/*CvPlayerAI& kPlayer = GET_PLAYER(getOwner());
	if (!kPlayer.canUseUnit((UnitTypes)GC.getCivilizationInfo(kPlayer.getCivilizationType()).getCivilizationUnits(eUnitType)))
	{
		return false;
	}*/
#if 0
    for (int iCivic = 0; iCivic < GC.getNumCivicInfos(); ++iCivic)
    {

        CvCivicInfo& kCivicInfo = GC.getCivicInfo((CivicTypes) iCivic);
        if (kCivicInfo.getAllowsUnitClasses(GC.getUnitInfo(eUnitType).getUnitClassType()) > 0)
        {
            if (GET_PLAYER(getOwner()).getIdeasResearched((CivicTypes) iCivic) <= 0)
            {
                return false;
            }
        }

    }
#endif
    ///TKe
	if (isCargo() && !canUnload())
	{
		return false;
	}

	if (!canMove())
	{
		return false;
	}

	return true;
}

void CvUnit::learn()
{
	if(!canLearn())
	{
		return;
	}

	CvCity* pCity = plot()->getPlotCity();
	PlayerTypes eNativePlayer = pCity->getOwnerINLINE();

	if (isHuman() && !getGroup()->AI_isControlled() && !GET_PLAYER(eNativePlayer).isHuman())
	{
		UnitTypes eUnitType = getLearnUnitType(plot());
		FAssert(eUnitType != NO_UNIT);

		CvDiploParameters* pDiplo = new CvDiploParameters(eNativePlayer);
		pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_LIVE_AMONG_NATIVES"));
		pDiplo->addDiploCommentVariable(pCity->getNameKey());
		pDiplo->addDiploCommentVariable(GC.getUnitInfo(eUnitType).getTextKeyWide());
		pDiplo->setData(getID());
		pDiplo->setAIContact(true);
		pDiplo->setCity(pCity->getIDInfo());
		gDLL->beginDiplomacy(pDiplo, getOwnerINLINE());
	}
	else
	{
		doLiveAmongNatives();
	}
}

void CvUnit::doLiveAmongNatives()
{
	if(!canLearn())
	{
		return;
	}

	unload();

	CvCity* pCity = plot()->getPlotCity();

	pCity->setTeachUnitMultiplier(pCity->getTeachUnitMultiplier() * (100 + GC.getXMLval(XML_NATIVE_TEACH_THRESHOLD_INCREASE)) / 100);
	int iLearnTime = getLearnTime();
	if (iLearnTime > 0)
	{
		setUnitTravelState(UNIT_TRAVEL_STATE_LIVE_AMONG_NATIVES, false);
		setUnitTravelTimer(iLearnTime);
	}
	else
	{
		doLearn();
	}
}

void CvUnit::doLearn()
{
	if(!canLearn())
	{
		return;
	}

	UnitTypes eUnitType = getLearnUnitType(plot());
	FAssert(eUnitType != NO_UNIT);

	CvUnit* pLearnUnit = GET_PLAYER(getOwnerINLINE()).initUnit(eUnitType, getProfession(), getX_INLINE(), getY_INLINE(), AI_getUnitAIType());
	FAssert(pLearnUnit != NULL);
	pLearnUnit->joinGroup(getGroup());
	pLearnUnit->convert(this, true);
    ///TK Update 1.1
    bool bFoundTech = false;
    for (int iX = 0; iX < NUM_YIELD_TYPES; iX++)
    {
        if (bFoundTech)
        {
            break;
        }
        if (GC.getUnitInfo(eUnitType).getYieldModifier((YieldTypes)iX) > 0)
        {
            for (int iCivic = 0; iCivic < GC.getNumCivicInfos(); ++iCivic)
            {
                CvCivicInfo& kCivicInfo = GC.getCivicInfo((CivicTypes) iCivic);
                if (kCivicInfo.getAllowsYields(iX) > 0)
                {
                    if (GET_PLAYER(getOwner()).getIdeasResearched((CivicTypes) iCivic) <= 0)
                    {
                        GET_PLAYER(getOwner()).processCivics((CivicTypes) iCivic, 1);
                        //GET_PLAYER(getOwner()).changeIdeasResearched((CivicTypes) iCivic, 1);
                        if (GET_PLAYER(getOwner()).getCurrentResearch() == (CivicTypes) iCivic)
                        {
                            if (isHuman())
                            {
                                CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CHOOSE_INVENTION, 0, -99, false);
                                gDLL->getInterfaceIFace()->addPopup(pInfo, GET_PLAYER(getOwner()).getID(), true);
                            }
                            GET_PLAYER(getOwner()).setCurrentResearch(NO_CIVIC);
                            GET_PLAYER(getOwner()).changeIdeasStored(-1);
                            GET_PLAYER(getOwner()).setIdeaProgress((CivicTypes) iCivic, -99);
                        }



                        CvWString szBuffer = gDLL->getText("TXT_KEY_LEARN_DISCOVER_KNOWLEDGE", GC.getCivicInfo((CivicTypes) iCivic).getDescription());
                        gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_LEARN).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE(),
                         true, true);
                         bFoundTech = true;
                         break;
                    }

                }
            }
        }
    }
   



	gDLL->getEventReporterIFace()->unitLearned(pLearnUnit->getOwnerINLINE(), pLearnUnit->getID());
}

UnitTypes CvUnit::getLearnUnitType(const CvPlot* pPlot) const
{
	if (getUnitInfo().getCasteAttribute() == 6)
	{
		return NO_UNIT;
	}

	if (getUnitInfo().getLearnTime() < 0)
	{
		return NO_UNIT;
	}

	if (pPlot == NULL)
	{
		return NO_UNIT;
	}

	CvCity* pCity = pPlot->getPlotCity();
	if (pCity == NULL)
	{
		return NO_UNIT;
	}

	if (pCity->getOwnerINLINE() == getOwnerINLINE())
	{
		return NO_UNIT;
	}

	if (!pCity->isScoutVisited(getTeam()))
	{
		return NO_UNIT;
	}

	UnitClassTypes eTeachUnitClass = pCity->getTeachUnitClass();
	if (eTeachUnitClass == NO_UNITCLASS)
	{
		return NO_UNIT;
	}

	UnitTypes eTeachUnit = (UnitTypes) GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(eTeachUnitClass);
	if (eTeachUnit == getUnitType())
	{
		return NO_UNIT;
	}

	return eTeachUnit;
}
 //TK end Update
int CvUnit::getLearnTime() const
{
	CvCity* pCity = plot()->getPlotCity();
	if (pCity == NULL)
	{
		return MAX_INT;
	}

	int iLearnTime = m_pUnitInfo->getLearnTime() * pCity->getTeachUnitMultiplier() / 100;

	iLearnTime *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
	iLearnTime /= 100;

	for (int iTrait = 0; iTrait < GC.getNumTraitInfos(); ++iTrait)
	{
		TraitTypes eTrait = (TraitTypes) iTrait;
		if (GET_PLAYER(getOwnerINLINE()).hasTrait(eTrait) || GET_PLAYER(pCity->getOwnerINLINE()).hasTrait(eTrait))
		{
			iLearnTime *= 100 + GC.getTraitInfo(eTrait).getLearnTimeModifier();
			iLearnTime /= 100;
		}
	}

	return iLearnTime;
}

///TKs Med
bool CvUnit::canKingTransport(bool bTestVisible) const
{
	if (!m_pUnitInfo->isTreasure())
	{
		return false;
	}
	CvPlot* pPlot = plot();
	if (pPlot == NULL)
	{
		return false;
	}
	CvCity* pCity = plot()->getPlotCity();
	if (GC.getLeaderHeadInfo(GET_PLAYER(getOwner()).getLeaderType()).getVictoryType() <= 0)
	{
		PlayerTypes eParent = GET_PLAYER(getOwnerINLINE()).getParent();
		if (eParent == NO_PLAYER || !GET_PLAYER(eParent).isAlive() || ::atWar(getTeam(), GET_PLAYER(eParent).getTeam()))
		{
			return false;
		}

		if (!pCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
		{
			return false;
		}

	}

if (!bTestVisible)
{
	if (pCity != NULL)
	{
        if (pPlot->getTeam() != getTeam())
        {
            return false;
        }
	}
	else
    {
        return false;
    }

    if (m_pUnitInfo->getConvertsToBuildingClass() != NO_BUILDINGCLASS)
    {
		BuildingTypes eBuilding = ((BuildingTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings((BuildingClassTypes)m_pUnitInfo->getConvertsToBuildingClass())));
        if (!pCity->isCityType((MedCityTypes)GC.getBuildingInfo(eBuilding).getCityType()))
        {
            return false;
        }
    }
}
	if (getYieldStored() == 0)
	{
		return false;
	}

	return true;
}
///Tke
//
//bool CvUnit::canKingTransport() const
//{
//	PlayerTypes eParent = GET_PLAYER(getOwnerINLINE()).getParent();
//	if (eParent == NO_PLAYER || !GET_PLAYER(eParent).isAlive() || ::atWar(getTeam(), GET_PLAYER(eParent).getTeam()))
//	{
//		return false;
//	}
//
//	if (!canMove())
//	{
//		return false;
//	}
//
//	CvPlot* pPlot = plot();
//	if (pPlot == NULL)
//	{
//		return false;
//	}
//
//	if (pPlot->getTeam() != getTeam())
//	{
//		return false;
//	}
//
//	CvCity* pCity = pPlot->getPlotCity();
//	if (pCity == NULL)
//	{
//		return false;
//	}
//
//	if (!pCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
//	{
//		return false;
//	}
//
//	if (getYieldStored() == 0)
//	{
//		return false;
//	}
//
//	if (!m_pUnitInfo->isTreasure())
//	{
//		return false;
//	}
//
//	return true;
//}

void CvUnit::kingTransport(bool bSkipPopup)
{
	if (!canKingTransport())
	{
		return;
	}
    ///TKs SkipDip
    //bSkipPopup = true;
    ///Tke
	if (isHuman() && GC.getLeaderHeadInfo(GET_PLAYER(getOwner()).getLeaderType()).getVictoryType() <= 0)
	{
		CvDiploParameters* pDiplo = new CvDiploParameters(GET_PLAYER(getOwnerINLINE()).getParent());
		pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_TREASURE_TRANSPORT"));
		pDiplo->setData(getID());
		int iCommission = GC.getXMLval(XML_KING_TRANSPORT_TREASURE_COMISSION);
		///TKs Invention Core Mod v 1.0
        for (int iCivic = 0; iCivic < GC.getNumCivicInfos(); ++iCivic)
        {

            CvCivicInfo& kCivicInfo = GC.getCivicInfo((CivicTypes) iCivic);
            if (kCivicInfo.getKingTreasureTransportMod() > 0)
            {
                if (GET_PLAYER(getOwner()).getIdeasResearched((CivicTypes) iCivic) > 0)
                {
                    iCommission = iCommission * kCivicInfo.getKingTreasureTransportMod() / 100;
                }
            }

        }
		///TKe
		pDiplo->addDiploCommentVariable(iCommission);
		int iAmount = getYieldStored();
		iAmount -= (iAmount * iCommission) / 100;
		iAmount -= (iAmount * GET_PLAYER(getOwnerINLINE()).getTaxRate()) / 100;
		pDiplo->addDiploCommentVariable(iAmount);
		pDiplo->setAIContact(true);
		gDLL->beginDiplomacy(pDiplo, getOwnerINLINE());
	}
	else
	{
		doKingTransport();
	}
}
//Tks Med
void CvUnit::doKingTransport()
{
		if (m_pUnitInfo->getConvertsToBuildingClass() == NO_BUILDINGCLASS && GC.getLeaderHeadInfo(GET_PLAYER(getOwnerINLINE()).getLeaderType()).getVictoryType() <= 0)
		{
			GET_PLAYER(getOwnerINLINE()).sellYieldUnitToEurope(this, getYieldStored(), GC.getDefineINT("KING_TRANSPORT_TREASURE_COMISSION"));
		
            plot()->addCrumbs(10);
            kill(true);
			return;
		}

        bool bKill = false;
        bool bRequiresBuilding = false;
        CvCity* ePlotCity = plot()->getPlotCity();
        if (ePlotCity != NULL && m_pUnitInfo->getConvertsToBuildingClass() != NO_BUILDINGCLASS)
        {
            bRequiresBuilding = true;
            BuildingTypes eBuilding = ((BuildingTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings((BuildingClassTypes)m_pUnitInfo->getConvertsToBuildingClass())));
            if (eBuilding != NO_BUILDING && ePlotCity->isCityType((MedCityTypes)GC.getBuildingInfo(eBuilding).getCityType()))
            {
                ePlotCity->setHasRealBuilding(eBuilding, true);
            }
            bKill = true;
			int iRand = GC.getGameINLINE().getSorenRandNum(GC.getXMLval(XML_PILGRAM_OFFER_GOLD), "Random Pilgram 1");
			iRand *= 2;
			//ePlotCity->changeYieldStored((YieldTypes)GC.getDefineINT("DEFAULT_TREASURE_YIELD") ,iRand);
			//GET_PLAYER(getOwner()).changeGold(getYieldStored());
			//iRand = GC.getGameINLINE().getSorenRandNum(GC.getDefineINT("PILGRAM_OFFER_GOLD"), "Random Pilgram 1");
			ePlotCity->changeCulture(ePlotCity->getOwner(), iRand, true);
			iRand = getYieldStored();
			CvWString szBuffer = gDLL->getText("TXT_KEY_UNIT_RELIC_ARRIVES", ePlotCity->getNameKey(), iRand);
			gDLL->getInterfaceIFace()->addMessage(ePlotCity->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getUnitInfo(getUnitType()).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getX_INLINE(), getY_INLINE(), true, true);
        }
        if (ePlotCity != NULL)
        {
            int iGoldBars = 0;
            if (m_pUnitInfo->getConvertsToYield() != NO_YIELD)
            {
                YieldTypes eGoldBars = (YieldTypes)m_pUnitInfo->getConvertsToYield();

                if (eGoldBars != NO_YIELD && getYieldStored() > 0)
                {
                    //iGoldBars = GET_PLAYER(getOwnerINLINE()).getYieldSellPrice(eGoldBars);
                    //iGoldBars = getYieldStored() / iGoldBars;
                    //ePlotCity->changeYieldStored(eGoldBars, iGoldBars);
					iGoldBars = getYieldStored();
                    GET_PLAYER(getOwnerINLINE()).changeGold(getYieldStored());
                    bKill = true;
                    //GET_PLAYER(getOwnerINLINE()).getSellToEuropeProfit(eYield, iLoss)
                }

            }
			else if (m_pUnitInfo->getConvertsToGold() != 0)
            {
				// is this code ever reached?
				// is it working if it's reached?
				// Nightinggale
				//FAssert(false);
				iGoldBars = m_pUnitInfo->getConvertsToGold();
                GET_PLAYER(getOwnerINLINE()).changeGold(m_pUnitInfo->getConvertsToGold());
                bKill = true;
            }
            if (isHuman() && !bRequiresBuilding)
            {
                CvWString szBuffer = gDLL->getText("TXT_KEY_UNIT_TREASURE_ARRIVES", ePlotCity->getNameKey(), iGoldBars);
                gDLL->getInterfaceIFace()->addMessage(ePlotCity->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getUnitInfo(getUnitType()).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getX_INLINE(), getY_INLINE(), true, true);
                gDLL->getInterfaceIFace()->playGeneralSound("AS2D_UNITGIFTED");
            }
        }

        if (bKill)
        {
             plot()->addCrumbs(10);
             kill(true);
        }


}
///Tke

bool CvUnit::canEstablishMission() const
{
	if (getProfession() == NO_PROFESSION)
	{
		return false;
	}

	if (GC.getProfessionInfo(getProfession()).getMissionaryRate() <= 0)
	{
		return false;
	}

	if(!canMove())
	{
		return false;
	}

	CvPlot* pPlot = plot();
	if (pPlot == NULL)
	{
		return false;
	}

	CvCity* pCity = pPlot->getPlotCity();
	if (pCity == NULL)
	{
		return false;
	}

	CvPlayer& kCityOwner = GET_PLAYER(pCity->getOwnerINLINE());
	if (!kCityOwner.canHaveMission(getOwnerINLINE()))
	{
		return false;
	}

	if (pCity->getMissionaryCivilization() == getCivilizationType())
	{
		return false;
	}

	return true;
}

void CvUnit::establishMission()
{
	if (!canEstablishMission())
	{
		return;
	}

	CvCity* pCity = plot()->getPlotCity();

	if (GC.getGameINLINE().getSorenRandNum(100, "Mission failure roll") > getMissionarySuccessPercent())
	{
		CvWString szBuffer = gDLL->getText("TXT_KEY_MISSION_FAILED", plot()->getPlotCity()->getNameKey());
		gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_ESTABLISH_MISSION).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), getX_INLINE(), getY_INLINE(), true, true);
		GET_PLAYER(pCity->getOwnerINLINE()).AI_changeMemoryCount((getOwnerINLINE()), MEMORY_MISSIONARY_FAIL, 1);
	}
	else
	{
		GET_PLAYER(getOwnerINLINE()).setMissionarySuccessPercent(GET_PLAYER(getOwnerINLINE()).getMissionarySuccessPercent() * GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getMissionFailureThresholdPercent() / 100);

		int iMissionaryRate = GC.getProfessionInfo(getProfession()).getMissionaryRate() * (100 + getUnitInfo().getMissionaryRateModifier()) / 100;
		if (!isHuman())
		{
			iMissionaryRate = (iMissionaryRate * 100 + 50) / GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIGrowthPercent();
		}
		pCity->setMissionaryPlayer(getOwnerINLINE());
		pCity->setMissionaryRate(iMissionaryRate);
		///Tks Civics //Civic Reset
		GET_PLAYER(pCity->getOwnerINLINE()).changeMissionaryPoints(getOwnerINLINE(), 1);
		CvPlayer& kPlayer = GET_PLAYER(getOwnerINLINE());
		kPlayer.resetConnectedPlayerYieldBonus();
		//tke

		for (int i = 0; i < GC.getNumFatherPointInfos(); ++i)
		{
			FatherPointTypes ePointType = (FatherPointTypes) i;
			GET_PLAYER(getOwnerINLINE()).changeFatherPoints(ePointType, GC.getFatherPointInfo(ePointType).getMissionaryPoints());
		}
	}
	//Tks Civic Screen
	if (GET_PLAYER(getOwnerINLINE()).getMissionaryHide() < 0)
	{
		setUnitTravelState(UNIT_TRAVEL_STATE_HIDE_UNIT, false);
		setUnitTravelTimer(MAX_SHORT);
		//GET_PLAYER(pCity->getOwnerINLINE()).changeMissionaryPoints(getOwnerINLINE(), 1000);
	}
	else if (GET_PLAYER(getOwnerINLINE()).getMissionaryHide() > 0)
	{
		setUnitTravelState(UNIT_TRAVEL_STATE_HIDE_UNIT, false);
		setUnitTravelTimer(GET_PLAYER(getOwnerINLINE()).getMissionaryHide());
	}
	else
	{
		kill(true);
	}
	//Tke
	
}

int CvUnit::getMissionarySuccessPercent() const
{
	return GET_PLAYER(getOwnerINLINE()).getMissionarySuccessPercent() * (100 + (getUnitInfo().getMissionaryRateModifier() * GC.getXMLval(XML_MISSIONARY_RATE_EFFECT_ON_SUCCESS) / 100)) / 100;
}

bool CvUnit::canSpeakWithChief(CvPlot* pPlot) const
{
	ProfessionTypes eProfession = getProfession();
	if (eProfession == NO_PROFESSION)
	{
		return false;
	}

	if (pPlot != NULL)
	{
		CvCity* pCity = pPlot->getPlotCity();
		if (pCity == NULL)
		{
			return false;
		}

		if (!pCity->isNative())
		{
			return false;
		}

		if (pCity->isScoutVisited(getTeam()))
		{
			return false;
		}
	}

	if (isNative())
	{
		return false;
	}

	// < JAnimals Mod Start >
	if (isBarbarian())
	{
		return false;
	}
	// < JAnimals Mod End >

	if (!canMove())
	{
		return false;
	}

	return true;
}

void CvUnit::speakWithChief()
{
	if(!canSpeakWithChief(plot()))
	{
		return;
	}

	CvCity* pCity = plot()->getPlotCity();
	GoodyTypes eGoody = pCity->getGoodyType(this);
	PlayerTypes eNativePlayer = pCity->getOwnerINLINE();

	if (isHuman() && !GET_PLAYER(eNativePlayer).isHuman())
	{
		CvWString szExpertText;
		int iGoodyValue = pCity->doGoody(this, eGoody);
		UnitClassTypes eTeachUnitClass = pCity->getTeachUnitClass();
		if (eTeachUnitClass != NO_UNITCLASS)
		{
			UnitTypes eTeachUnit = (UnitTypes) GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(eTeachUnitClass);
			if (eTeachUnit != NO_UNIT)
			{
				szExpertText = gDLL->getText("AI_DIPLO_CHIEF_LEARN_UNIT_DESCRIPTION", GC.getUnitInfo(eTeachUnit).getTextKeyWide());
			}
		}

		CvWString szYieldText;
		YieldTypes eDesiredYield = pCity->AI_getDesiredYield();
		if (eDesiredYield != NO_YIELD)
		{
			szYieldText = gDLL->getText("AI_DIPLO_CHIEF_DESIRED_YIELD_DESCRIPTION", GC.getYieldInfo(eDesiredYield).getTextKeyWide());
		}

		CvWString szGoodyText;
		if (eGoody != NO_GOODY)
		{
			szGoodyText = gDLL->getText(GC.getGoodyInfo(eGoody).getChiefTextKey(), iGoodyValue);
		}

		CvDiploParameters* pDiplo = new CvDiploParameters(pCity->getOwnerINLINE());
		pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_CHIEF_GOODY"));
		pDiplo->addDiploCommentVariable(pCity->getNameKey());
		pDiplo->addDiploCommentVariable(szExpertText);
		pDiplo->addDiploCommentVariable(szYieldText);
		pDiplo->addDiploCommentVariable(szGoodyText);
		pDiplo->setAIContact(true);
		pDiplo->setCity(pCity->getIDInfo());
		gDLL->beginDiplomacy(pDiplo, getOwnerINLINE());
	}
	else
	{
		pCity->doGoody(this, eGoody);
	}
}


bool CvUnit::canHold(const CvPlot* pPlot) const
{
	return true;
}


bool CvUnit::canSleep(const CvPlot* pPlot) const
{
	if (isFortifyable())
	{
		return false;
	}

	if (isWaiting())
	{
		return false;
	}

	return true;
}


bool CvUnit::canFortify(const CvPlot* pPlot) const
{
	if (!isFortifyable())
	{
		return false;
	}

	if (isWaiting())
	{
		return false;
	}

	return true;
}


bool CvUnit::canHeal(const CvPlot* pPlot) const
{
	if (!isHurt())
	{
		return false;
	}

	if (isWaiting())
	{
		return false;
	}

	if (healRate(pPlot) <= 0)
	{
		return false;
	}

	return true;
}


bool CvUnit::canSentry(const CvPlot* pPlot) const
{
	if (!canDefend(pPlot))
	{
		return false;
	}

	if (isWaiting())
	{
		return false;
	}
	//Tks Med
	if (isOnlyDefensive())
	{
		return false;
	}

	if (isUnarmed())
	{
		return false;
	}
	//tke

	return true;
}


int CvUnit::healRate(const CvPlot* pPlot) const
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvPlot* pLoopPlot;
	int iHeal;
	int iBestHeal;
	int iI;


	int iTotalHeal = 0;

	if (pPlot->isCity(true, getTeam()))
	{
		iTotalHeal += GC.getXMLval(XML_CITY_HEAL_RATE) + (GET_TEAM(getTeam()).isFriendlyTerritory(pPlot->getTeam()) ? getExtraFriendlyHeal() : getExtraNeutralHeal());
		CvCity* pCity = pPlot->getPlotCity();
		if (pCity && !pCity->isOccupation())
		{
			iTotalHeal += pCity->getHealRate();
		}
	}
	else
	{
		if (!GET_TEAM(getTeam()).isFriendlyTerritory(pPlot->getTeam()))
		{
		    ///TKs Invention Core Mod v 1.0
            for (int iCivic = 0; iCivic < GC.getNumCivicInfos(); ++iCivic)
            {

                CvCivicInfo& kCivicInfo = GC.getCivicInfo((CivicTypes) iCivic);
                if (kCivicInfo.getIncreasedEnemyHealRate() > 0)
                {
                    if (GET_PLAYER(getOwner()).getIdeasResearched((CivicTypes) iCivic) > 0)
                    {
                        iTotalHeal += kCivicInfo.getIncreasedEnemyHealRate();
                    }
                }

            }
                ///TKe
			if (isEnemy(pPlot->getTeam(), pPlot))
			{
				iTotalHeal += (GC.getXMLval(XML_ENEMY_HEAL_RATE) + getExtraEnemyHeal());
			}
			else
			{
				iTotalHeal += (GC.getXMLval(XML_NEUTRAL_HEAL_RATE) + getExtraNeutralHeal());
			}
		}
		else
		{
			iTotalHeal += (GC.getXMLval(XML_FRIENDLY_HEAL_RATE) + getExtraFriendlyHeal());
		}
	}

	// XXX optimize this (save it?)
	iBestHeal = 0;

	pUnitNode = pPlot->headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if (pLoopUnit->getTeam() == getTeam()) // XXX what about alliances?
		{
			iHeal = pLoopUnit->getSameTileHeal();

			if (iHeal > iBestHeal)
			{
				iBestHeal = iHeal;
			}
		}
	}

	for (iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pLoopPlot = plotDirection(pPlot->getX_INLINE(), pPlot->getY_INLINE(), ((DirectionTypes)iI));

		if (pLoopPlot != NULL)
		{
			if (pLoopPlot->area() == pPlot->area())
			{
				pUnitNode = pLoopPlot->headUnitNode();

				while (pUnitNode != NULL)
				{
					pLoopUnit = ::getUnit(pUnitNode->m_data);
					pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

					if (pLoopUnit->getTeam() == getTeam()) // XXX what about alliances?
					{
						iHeal = pLoopUnit->getAdjacentTileHeal();

						if (iHeal > iBestHeal)
						{
							iBestHeal = iHeal;
						}
					}
				}
			}
		}
	}

	iTotalHeal += iBestHeal;
	// XXX

	return iTotalHeal;
}


int CvUnit::healTurns(const CvPlot* pPlot) const
{
	int iHeal;
	int iTurns;

	if (!isHurt())
	{
		return 0;
	}

	iHeal = healRate(pPlot);

	if (iHeal > 0)
	{
		iTurns = (getDamage() / iHeal);

		if ((getDamage() % iHeal) != 0)
		{
			iTurns++;
		}

		return iTurns;
	}
	else
	{
		return MAX_INT;
	}
}


void CvUnit::doHeal()
{
	changeDamage(-(healRate(plot())));
}


CvCity* CvUnit::bombardTarget(const CvPlot* pPlot) const
{
	int iBestValue = MAX_INT;
	CvCity* pBestCity = NULL;

	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pLoopPlot = plotDirection(pPlot->getX_INLINE(), pPlot->getY_INLINE(), ((DirectionTypes)iI));

		if (pLoopPlot != NULL)
		{
			CvCity* pLoopCity = pLoopPlot->getPlotCity();

			if (pLoopCity != NULL)
			{
				if (pLoopCity->isBombardable(this))
				{
					int iValue = pLoopCity->getDefenseDamage();

					// always prefer cities we are at war with
					if (isEnemy(pLoopCity->getTeam(), pPlot))
					{
						iValue *= 128;
					}

					if (iValue < iBestValue)
					{
						iBestValue = iValue;
						pBestCity = pLoopCity;
					}
				}
			}
		}
	}

	return pBestCity;
}


bool CvUnit::canBombard(const CvPlot* pPlot) const
{
	if (bombardRate() <= 0)
	{
		return false;
	}

	if (isMadeAttack())
	{
		return false;
	}

	if (isCargo())
	{
		return false;
	}

	if (bombardTarget(pPlot) == NULL)
	{
		return false;
	}

	return true;
}


bool CvUnit::bombard()
{
	CvPlot* pPlot = plot();
	if (!canBombard(pPlot))
	{
		return false;
	}

	CvCity* pBombardCity = bombardTarget(pPlot);
	FAssertMsg(pBombardCity != NULL, "BombardCity is not assigned a valid value");

	CvPlot* pTargetPlot = pBombardCity->plot();
	if (!isEnemy(pTargetPlot->getTeam()))
	{
		getGroup()->groupDeclareWar(pTargetPlot, true);
	}

	if (!isEnemy(pTargetPlot->getTeam()))
	{
		return false;
	}

	pBombardCity->changeDefenseModifier(-(bombardRate() * std::max(0, 100 - pBombardCity->getBuildingBombardDefense())) / 100);

	setMadeAttack(true);
	changeMoves(GC.getMOVE_DENOMINATOR());

	CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_DEFENSES_IN_CITY_REDUCED_TO", pBombardCity->getNameKey(), pBombardCity->getDefenseModifier(), GET_PLAYER(getOwnerINLINE()).getNameKey());
	gDLL->getInterfaceIFace()->addMessage(pBombardCity->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", MESSAGE_TYPE_INFO, getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pBombardCity->getX_INLINE(), pBombardCity->getY_INLINE(), true, true);

	szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_REDUCE_CITY_DEFENSES", getNameOrProfessionKey(), pBombardCity->getNameKey(), pBombardCity->getDefenseModifier());
	gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARD", MESSAGE_TYPE_INFO, getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pBombardCity->getX_INLINE(), pBombardCity->getY_INLINE());

	if (pPlot->isActiveVisible(false))
	{
		CvUnit *pDefender = pBombardCity->plot()->getBestDefender(NO_PLAYER, getOwnerINLINE(), this, true);

		// Bombard entity mission
		CvMissionDefinition kDefiniton;
		kDefiniton.setMissionTime(GC.getMissionInfo(MISSION_BOMBARD).getTime() * gDLL->getSecsPerTurn());
		kDefiniton.setMissionType(MISSION_BOMBARD);
		kDefiniton.setPlot(pBombardCity->plot());
		kDefiniton.setUnit(BATTLE_UNIT_ATTACKER, this);
		kDefiniton.setUnit(BATTLE_UNIT_DEFENDER, pDefender);
		gDLL->getEntityIFace()->AddMission(&kDefiniton);
	}

	return true;
}


bool CvUnit::canPillage(const CvPlot* pPlot) const
{
	if (!canAttack())
	{
		return false;
	}

	if (pPlot->isCity())
	{
		return false;
	}

	if (pPlot->getImprovementType() == NO_IMPROVEMENT)
	{
		if (!(pPlot->isRoute()))
		{
			return false;
		}
	}
	else
	{
		if (GC.getImprovementInfo(pPlot->getImprovementType()).isPermanent())
		{
			return false;
		}
		///TKs Med Prevent native destroying goodies
		if (GC.getImprovementInfo(pPlot->getImprovementType()).isGoody())
        {
            if (isNative() || isBarbarian())
            {
                return false;
            }
        }
		///Tke
	}

	if (pPlot->isOwned())
	{
		if (!potentialWarAction(pPlot))
		{
			if ((pPlot->getImprovementType() == NO_IMPROVEMENT) || (pPlot->getOwnerINLINE() != getOwnerINLINE()))
			{
				return false;
			}
		}
	}

	if (!(pPlot->isValidDomainForAction(*this)))
	{
		return false;
	}

	return true;
}


bool CvUnit::pillage()
{
	CvWString szBuffer;
	int iPillageGold;
	long lPillageGold;
	ImprovementTypes eTempImprovement = NO_IMPROVEMENT;
	RouteTypes eTempRoute = NO_ROUTE;

	CvPlot* pPlot = plot();

	if (!canPillage(pPlot))
	{
		return false;
	}

	if (pPlot->isOwned())
	{
		// we should not be calling this without declaring war first, so do not declare war here
		if (!isEnemy(pPlot->getTeam(), pPlot))
		{
			if ((pPlot->getImprovementType() == NO_IMPROVEMENT) || (pPlot->getOwnerINLINE() != getOwnerINLINE()))
			{
				return false;
			}
		}
	}

	if (pPlot->getImprovementType() != NO_IMPROVEMENT)
	{
		eTempImprovement = pPlot->getImprovementType();

		if (pPlot->getTeam() != getTeam())
		{
			// Use python to determine pillage amounts...
			lPillageGold = 0;

			CyPlot* pyPlot = new CyPlot(pPlot);
			CyUnit* pyUnit = new CyUnit(this);

			CyArgsList argsList;
			argsList.add(gDLL->getPythonIFace()->makePythonObject(pyPlot));	// pass in plot class
			argsList.add(gDLL->getPythonIFace()->makePythonObject(pyUnit));	// pass in unit class

			gDLL->getPythonIFace()->callFunction(PYGameModule, "doPillageGold", argsList.makeFunctionArgs(),&lPillageGold);

			delete pyPlot;	// python fxn must not hold on to this pointer
			delete pyUnit;	// python fxn must not hold on to this pointer

			iPillageGold = (int)lPillageGold;

			if (iPillageGold > 0)
			{
				GET_PLAYER(getOwnerINLINE()).changeGold(iPillageGold);

				szBuffer = gDLL->getText("TXT_KEY_MISC_PLUNDERED_GOLD_FROM_IMP", iPillageGold, GC.getImprovementInfo(pPlot->getImprovementType()).getTextKeyWide());
				gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGE", MESSAGE_TYPE_INFO, getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pPlot->getX_INLINE(), pPlot->getY_INLINE());

				if (pPlot->isOwned())
				{
					szBuffer = gDLL->getText("TXT_KEY_MISC_IMP_DESTROYED", GC.getImprovementInfo(pPlot->getImprovementType()).getTextKeyWide(), getNameOrProfessionKey(), getVisualCivAdjective(pPlot->getTeam()));
					gDLL->getInterfaceIFace()->addMessage(pPlot->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGED", MESSAGE_TYPE_INFO, getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pPlot->getX_INLINE(), pPlot->getY_INLINE(), true, true);
				}
			}
		}

		pPlot->setImprovementType((ImprovementTypes)(GC.getImprovementInfo(pPlot->getImprovementType()).getImprovementPillage()));
	}
	else if (pPlot->isRoute())
	{
		eTempRoute = pPlot->getRouteType();
		pPlot->setRouteType(NO_ROUTE); // XXX downgrade rail???
	}

	changeMoves(GC.getMOVE_DENOMINATOR());

	if (pPlot->isActiveVisible(false))
	{
		// Pillage entity mission
		CvMissionDefinition kDefiniton;
		kDefiniton.setMissionTime(GC.getMissionInfo(MISSION_PILLAGE).getTime() * gDLL->getSecsPerTurn());
		kDefiniton.setMissionType(MISSION_PILLAGE);
		kDefiniton.setPlot(pPlot);
		kDefiniton.setUnit(BATTLE_UNIT_ATTACKER, this);
		kDefiniton.setUnit(BATTLE_UNIT_DEFENDER, NULL);
		gDLL->getEntityIFace()->AddMission(&kDefiniton);
	}

	if (eTempImprovement != NO_IMPROVEMENT || eTempRoute != NO_ROUTE)
	{
		gDLL->getEventReporterIFace()->unitPillage(this, eTempImprovement, eTempRoute, getOwnerINLINE());
	}

	return true;
}
///Tks Med
bool CvUnit::canFound(const CvPlot* pPlot, bool bTestVisible, int iFoundType) const
{
    MedCityTypes eFoundCityType = (MedCityTypes)iFoundType;
	if (!m_pUnitInfo->isFound())
	{
		return false;
	}

    if (m_pUnitInfo->isPreventFounding())
    {
        return false;
    }
    int iCityType = 0;
	if (getProfession() != NO_PROFESSION)
	{
	    MedCityTypes eCityType = (MedCityTypes)GC.getProfessionInfo(getProfession()).getFoundCityType();

		if (!GC.getProfessionInfo(getProfession()).canFound())
		{
			return false;
		}

		if (eCityType == CITYTYPE_MONASTERY)
		{
		    if (eFoundCityType != eCityType)
            {
                return false;
            }
		}

		if (eCityType == CITYTYPE_MILITARY)
		{
		    if (eFoundCityType != eCityType)
            {
                if (GC.getProfessionInfo(getProfession()).isUnarmed())
                {
                    return false;
                }
                if (GC.getProfessionInfo(getProfession()).getCombatChange() <= 2)
                {
                    return false;
                }
            }
		}
	}

    int iEconomyType = GC.getLeaderHeadInfo(GET_PLAYER(getOwnerINLINE()).getLeaderType()).getEconomyType();
    if (GC.getGameINLINE().isFinalInitialized() && isHuman() && iEconomyType == 1)
    {
        if (!GET_PLAYER(getOwnerINLINE()).isFirstCityRazed())
        {
            return false;
        }
    }
    ///Tke
	if (pPlot != NULL)
	{
		if (pPlot->isCity())
		{
			return false;
		}

		if (!(GET_PLAYER(getOwnerINLINE()).canFound(pPlot->getX_INLINE(), pPlot->getY_INLINE(), bTestVisible, iFoundType)))
		{
			return false;
		}
	}

	return true;
}

///TKs Med
bool CvUnit::found(int iType)
{
	if (!canFound(plot(), false, iType))
	{
		return false;
	}
	CvPlayer& kPlayer = GET_PLAYER(getOwnerINLINE());
    kPlayer.setCurrentFoundCityType(iType);
	PlayerTypes eParent = kPlayer.getParent();
	if (eParent != NO_PLAYER && !GC.getEraInfo(kPlayer.getCurrentEra()).isRevolution() && !isAutomated())
	{
		int iFoodDifference = plot()->calculateNatureYield(YIELD_FOOD, getTeam(), true) - GC.getFOOD_CONSUMPTION_PER_POPULATION();
		bool bInland = !plot()->isCoastalLand(GC.getXMLval(XML_MIN_WATER_SIZE_FOR_OCEAN));

		DiploCommentTypes eDiploComment = NO_DIPLOCOMMENT;
		///TKs Med
		if (iFoodDifference < 0 && GC.getLeaderHeadInfo(kPlayer.getLeaderType()).getVictoryType() <= 0 && kPlayer.shouldDisplayFeatPopup(FEAT_CITY_NO_FOOD))
		{
			eDiploComment = (DiploCommentTypes) GC.getInfoTypeForString("AI_DIPLOCOMMENT_FOUND_CITY_NO_FOOD");
			kPlayer.setFeatAccomplished(FEAT_CITY_NO_FOOD, true);
		}
		else if (bInland && GC.getLeaderHeadInfo(kPlayer.getLeaderType()).getVictoryType() <= 0 && kPlayer.shouldDisplayFeatPopup(FEAT_CITY_INLAND))
		{
			eDiploComment = (DiploCommentTypes) GC.getInfoTypeForString("AI_DIPLOCOMMENT_FOUND_CITY_INLAND");
			kPlayer.setFeatAccomplished(FEAT_CITY_INLAND, true);
		}
        ///TKe
		if (eDiploComment != NO_DIPLOCOMMENT)
		{
			CvDiploParameters* pDiplo = new CvDiploParameters(eParent);
			pDiplo->setDiploComment(eDiploComment);
			pDiplo->setData(getID());
			pDiplo->setAIContact(true);
			gDLL->beginDiplomacy(pDiplo, getOwnerINLINE());
			return true;
		}
	}

	return doFoundCheckNatives(iType);
}

bool CvUnit::doFoundCheckNatives(int iType)
{
	if (!canFound(plot(), false, iType))
	{
		return false;
	}

	if (isHuman() && !isAutomated())
	{
		PlayerTypes eNativeOwner = NO_PLAYER;
		int iCost = 0;
		for (int i = 0; i < NUM_CITY_PLOTS; ++i)
		{
			CvPlot* pLoopPlot = ::plotCity(getX_INLINE(), getY_INLINE(), i);
			if (pLoopPlot != NULL)
			{
				if (pLoopPlot->isOwned() && !pLoopPlot->isCity())
				{
					if (GET_PLAYER(pLoopPlot->getOwnerINLINE()).isNative() && !GET_TEAM(pLoopPlot->getTeam()).isAtWar(getTeam()))
					{
						eNativeOwner = pLoopPlot->getOwnerINLINE();
						iCost += pLoopPlot->getBuyPrice(getOwnerINLINE());
					}
				}
			}
		}
		///Tks med
		iCost -= iCost * GC.getUnitInfo(getUnitType()).getTradeBonus() / 100;
		//iCost = std:: iCost (iCost, 1);
		///Tke

		if (eNativeOwner != NO_PLAYER)
		{
			GET_TEAM(getTeam()).meet(GET_PLAYER(eNativeOwner).getTeam(), false);
		}

		if (eNativeOwner != NO_PLAYER && !GET_PLAYER(eNativeOwner).isHuman())
		{
			CvDiploParameters* pDiplo = new CvDiploParameters(eNativeOwner);
			if (GET_PLAYER(getOwnerINLINE()).getNumCities() == 0)
			{
				pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_FOUND_FIRST_CITY"));
			}
			else if(iCost > GET_PLAYER(getOwnerINLINE()).getGold())
			{
				pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_FOUND_CITY_CANT_AFFORD"));
				pDiplo->addDiploCommentVariable(iCost);
			}
			else
			{
				pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_FOUND_CITY"));
				pDiplo->addDiploCommentVariable(iCost);
			}
			pDiplo->setData(getID());
			pDiplo->setAIContact(true);
			gDLL->beginDiplomacy(pDiplo, getOwnerINLINE());
		}
		else
		{
			doFound(false, iType);
		}
	}
	else
	{
		AI_doFound(iType);
	}

	return true;
}

bool CvUnit::doFound(bool bBuyLand, int iType)
{
	if (!canFound(plot(), false, iType))
	{
		return false;
	}

	if (GC.getGameINLINE().getActivePlayer() == getOwnerINLINE())
	{
		gDLL->getInterfaceIFace()->lookAt(plot()->getPoint(), CAMERALOOKAT_NORMAL);
	}

	//first city takes land for free
	bool bIsFirstCity = (GET_PLAYER(getOwnerINLINE()).getNumCities() == 0);
	if (bBuyLand || bIsFirstCity)
	{
		for (int i = 0; i < NUM_CITY_PLOTS; ++i)
		{
			CvPlot* pLoopPlot = ::plotCity(getX_INLINE(), getY_INLINE(), i);
			if (pLoopPlot != NULL)
			{
				if (pLoopPlot->isOwned() && !pLoopPlot->isCity())
				{
					//don't buy land if at war, it will be taken
					if (GET_PLAYER(pLoopPlot->getOwnerINLINE()).isNative() && !GET_TEAM(pLoopPlot->getTeam()).isAtWar(getTeam()))
					{
						GET_PLAYER(getOwnerINLINE()).buyLand(pLoopPlot, bIsFirstCity);
					}
				}
			}
		}
	}

	GET_PLAYER(getOwnerINLINE()).found(getX_INLINE(), getY_INLINE(), iType);

	CvPlot* pCityPlot = GC.getMapINLINE().plotINLINE(getX_INLINE(), getY_INLINE());
	FAssert(NULL != pCityPlot);
	if (pCityPlot != NULL)
	{
		if (pCityPlot->isActiveVisible(false))
		{
			NotifyEntity(MISSION_FOUND);
			EffectTypes eEffect = (EffectTypes)GC.getInfoTypeForString("EFFECT_SETTLERSMOKE");
			gDLL->getEngineIFace()->TriggerEffect(eEffect, pCityPlot->getPoint(), (float)(GC.getASyncRand().get(360)));
			gDLL->getInterfaceIFace()->playGeneralSound("AS3D_UN_FOUND_CITY", pCityPlot->getPoint());
		}

		CvCity* pCity = pCityPlot->getPlotCity();
		FAssert(NULL != pCity);
		if (NULL != pCity)
		{
		    ///TKs Med
		    unloadAll();
		    ///TKe
			pCity->addPopulationUnit(this, NO_PROFESSION);
		}
	}

	return true;
}
///Tke
bool CvUnit::canJoinCity(const CvPlot* pPlot, bool bTestVisible) const
{
	CvCity* pCity = pPlot->getPlotCity();

	if (pCity == NULL)
	{
		return false;
	}

	///TKs Med
    if (!isOnMap())
    {
        return false;
    }
    ///Tke

	if (pCity->getOwnerINLINE() != getOwnerINLINE())
	{
		return false;
	}

	if (pCity->isDisorder())
	{
		return false;
	}

	if (!m_pUnitInfo->isFound())
	{
		return false;
	}

	if (isDelayedDeath())
	{
		return false;
	}

	if (!bTestVisible)
	{
		///Tks New Food
		//if (pCity->getRawYieldProduced(YIELD_FOOD) < pCity->getPopulation() * GC.getFOOD_CONSUMPTION_PER_POPULATION())
		if (pCity->getRawYieldProduced(YIELD_FOOD) < pCity->getMaxFoodConsumed())
		{
			if (!canJoinStarvingCity(*pCity))
			{
				return false;
			}
		}
		///Tke
		ProfessionTypes eProfession = getProfession();
		if (eProfession == NO_PROFESSION || GC.getProfessionInfo(eProfession).isUnarmed() || GC.getProfessionInfo(eProfession).isCitizen())
		{
			if (movesLeft() == 0)
			{
				return false;
			}
		}
		else
		{
			if (hasMoved())
			{
				return false;
			}
		}
		///TKs Med Cheat for AI
		if (isHuman())
		{
            int iPopulation = pCity->getPopulation();
            int iMaxPop = pCity->getMaxCityPop();
            if (iPopulation == iMaxPop)
            {
                if (iMaxPop < GC.getXMLval(XML_MAX_CITY_POPULATION_COMMUNE))
                {
                    return false;
                }
            }
		}
		///TKe
	}

	return true;
}

bool CvUnit::canJoinStarvingCity(const CvCity& kCity) const
{
	FAssert(kCity.foodDifference() < 0);

	if (kCity.getYieldStored(YIELD_FOOD) >= GC.getGameINLINE().getCargoYieldCapacity() / 4)
	{
		return true;
	}
	///Tks New FOod
	int iNewPop = kCity.getPopulation() + 1;
	int iFoodConsumed = m_pUnitInfo->getFoodConsumed();
	//if (kCity.AI_getFoodGatherable(iNewPop, 0) >= iNewPop * GC.getFOOD_CONSUMPTION_PER_POPULATION())
	if (kCity.AI_getFoodGatherable(iNewPop, 0) >= kCity.getMaxFoodConsumed() + iFoodConsumed)
	{
		return true;
	}
	///tke
	if (!isHuman())
	{
		ProfessionTypes eProfession = AI_getIdealProfession();
		if (eProfession != NO_PROFESSION)
		{
			// MultipleYieldsProduced Start by Aymerick 22/01/2010
			for (int i = 0; i < GC.getProfessionInfo(eProfession).getNumYieldsProduced(); i++)
			{
				if (GC.getProfessionInfo(eProfession).getYieldsProduced(i) == YIELD_FOOD)
			{
				return true;
			}
		}
			// MultipleYieldsProduced End
		}
	}

	return false;
}

bool CvUnit::joinCity()
{
	if (!canJoinCity(plot()))
	{
		return false;
	}

    ///TKs Med
    unloadAll();

    ///Tke

	if (plot()->isActiveVisible(false))
	{
		NotifyEntity(MISSION_JOIN_CITY);
	}

	CvCity* pCity = plot()->getPlotCity();

	if (pCity != NULL)
	{
	    ///Tks Med
	    if (!isHuman() && canBuildHome(plot()))
        {
            BuildingTypes eBuilding = ((BuildingTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings((BuildingClassTypes)m_pUnitInfo->getFreeBuildingClass())));
            if (pCity != NULL && eBuilding != NO_BUILDING)
            {
                pCity->setHasRealBuilding(eBuilding, true);
                setAddedFreeBuilding(true);
                //CvWString szBuffer = gDLL->getText("TXT_KEY_UNIT_BUILT_HOME", pCity->getNameKey(), 10);
               // gDLL->getInterfaceIFace()->addMessage(pCity->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getUnitInfo(getUnitType()).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getX_INLINE(), getY_INLINE(), true, true);
            }
        }
	    ///Tke
		pCity->addPopulationUnit(this, NO_PROFESSION);
	}

	return true;
}
///TKs Med start
bool CvUnit::canBuild(const CvPlot* pPlot, BuildTypes eBuild, bool bTestVisible) const
{

    if (workRate(true) <= 0)
	{
		return false;
	}


    ///TKs Med Cheat
//	if (gDLL->ctrlKey() && gDLL->altKey())
//	{
//	    return true;
//	}
	///Tke
    if (!(GET_PLAYER(getOwnerINLINE()).canBuild(pPlot, eBuild, false, bTestVisible)))
	{
		return false;
	}

	if (!pPlot->isValidDomainForAction(*this))
	{
		return false;
	}

	if (!bTestVisible)
	{
        FAssertMsg(eBuild < GC.getNumBuildInfos(), "Index out of bounds");
        if (GC.getBuildInfo(eBuild).getImprovement() != NO_IMPROVEMENT)
        {

//            BonusTypes eBonus = plot()->getBonusType();
//            if (eBonus != NO_BONUS)
//            {
//                if (!GC.getImprovementInfo((ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement()).getImprovementBonusYield(eBonus, YIELD_HORSES))
//                {
//                    return false;
//                }
//            }

            if (GC.getImprovementInfo((ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement()).isRequiresCityYields() && (plot()->getBuildProgress(eBuild) == 0))
            {
               if (plot()->isPeak())
               {
                   return false;
               }
                bool bPlotHasBonus = false;
                if (GC.getImprovementInfo((ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement()).getBonusCreated() != NO_BONUS)
                {
                    if (plot()->getBonusType() == (BonusTypes)GC.getImprovementInfo((ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement()).getBonusCreated())
                    {
                        bPlotHasBonus = true;
                    }
                }
                bool bOutside = GC.getImprovementInfo((ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement()).isOutsideBorders();
                if ((!bPlotHasBonus && isHuman()) || bOutside)
                {
                    if (!bOutside)
                    {
                        if (!plot()->isCityRadius())
                        {
                           return false;
                        }
                        CvCity* pWorkingCity = plot()->getWorkingCity();
                        if (pWorkingCity != NULL)
                        {
                            int iRequiredYield = 0;
                            bool bRequiresYield = false;
                            for (int i = 0; i < NUM_YIELD_TYPES; ++i)
                            {
                                YieldTypes eYield = (YieldTypes) i;
                                iRequiredYield = GC.getImprovementInfo((ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement()).getRequiredCityYields(eYield);
                                if (iRequiredYield > 0)
                                {
                                    bRequiresYield = true;
                                    if (pWorkingCity->getYieldStored(eYield) < iRequiredYield)
                                    {
                                        return false;
                                    }
                                }
                            }

                        }
                    }
                    else
                    {

                        int iRequiredYield = 0;
                        int iFoundMaterial = 0;
                        int iRequiredMaterial = 0;
                        for (int i = 0; i < NUM_YIELD_TYPES; ++i)
                        {
                            YieldTypes eYield = (YieldTypes) i;
                            iRequiredYield = GC.getImprovementInfo((ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement()).getRequiredCityYields(eYield);
                            if (iRequiredYield > 0)
                            {
                                ++iRequiredMaterial;
                                for (int i = 0; i < pPlot->getNumUnits(); i++)
                                {
                                    CvUnit* pLoopUnit = pPlot->getUnitByIndex(i);
                                    if(pLoopUnit != NULL)
                                    {
                                        if (pLoopUnit->getOwner() == getOwner() && pLoopUnit->getYield() == eYield)
                                        {
                                            if (pLoopUnit->getYieldStored() >= iRequiredYield)
                                            {
                                                ++iFoundMaterial;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        if (iFoundMaterial != iRequiredMaterial)
                        {
                            return false;
                        }
                    }
                }
            }
            if (GC.getBuildInfo(eBuild).getCityType() > -1)
            {
                if (!isHuman())
                {
                    return false;
                }
                if (plot()->isPeak())
                {
                    return false;
                }
                CvPlot* pPlot = plot();
                if (pPlot != NULL)
                {
                    if (pPlot->isCity())
                    {
                        return false;
                    }

                    if (!(GET_PLAYER(getOwnerINLINE()).canFound(pPlot->getX_INLINE(), pPlot->getY_INLINE(), false)))
                    {
                        return false;
                    }
                }
            }
        }
	}
//	if (!(m_pUnitInfo->getBuilds(eBuild)))
//	{
//		return false;
//	}

	return true;
}

int CvUnit::canBuildInt(const CvPlot* pPlot, BuildTypes eBuild, bool bTestVisible) const
{

    if (workRate(true) <= 0)
	{
		return 0;
	}

    if (!bTestVisible)
	{
        FAssertMsg(eBuild < GC.getNumBuildInfos(), "Index out of bounds");
        if (GC.getBuildInfo(eBuild).getImprovement() != NO_IMPROVEMENT)
        {
            if (GC.getBuildInfo(eBuild).getCityType() > -1)
            {
                CvPlot* pPlot = plot();
                if (pPlot != NULL)
                {
                    if (pPlot->isCity())
                    {
                        return 3;
                    }

                    if (!(GET_PLAYER(getOwnerINLINE()).canFound(pPlot->getX_INLINE(), pPlot->getY_INLINE(), false)))
                    {
                        return 3;
                    }
                }
            }
            if (GC.getImprovementInfo((ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement()).isRequiresCityYields() && (plot()->getBuildProgress(eBuild) == 0))
            {
                bool bPlotHasBonus = false;
                if (GC.getImprovementInfo((ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement()).getBonusCreated() != NO_BONUS)
                {
                    if (plot()->getBonusType() == (BonusTypes)GC.getImprovementInfo((ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement()).getBonusCreated())
                    {
                        bPlotHasBonus = true;
                    }
                }
                bool bOutside = GC.getImprovementInfo((ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement()).isOutsideBorders();
                if (!bPlotHasBonus || bOutside)
                {
                    if (!bOutside)
                    {
                        CvCity* pWorkingCity = plot()->getWorkingCity();
                        if (pWorkingCity != NULL)
                        {
                            int iRequiredYield = 0;
                            bool bRequiresYield = false;
                            for (int i = 0; i < NUM_YIELD_TYPES; ++i)
                            {
                                YieldTypes eYield = (YieldTypes) i;
                                iRequiredYield = GC.getImprovementInfo((ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement()).getRequiredCityYields(eYield);
                                if (iRequiredYield > 0)
                                {
                                    bRequiresYield = true;
                                    if (pWorkingCity->getYieldStored(eYield) < iRequiredYield)
                                    {
                                        return 1;
                                    }
                                }
                            }

                        }
                    }
                    else
                    {
                        int iRequiredYield = 0;
                        int iFoundMaterial = 0;
                        int iRequiredMaterial = 0;
                        for (int i = 0; i < NUM_YIELD_TYPES; ++i)
                        {
                            YieldTypes eYield = (YieldTypes) i;
                            iRequiredYield = GC.getImprovementInfo((ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement()).getRequiredCityYields(eYield);
                            if (iRequiredYield > 0)
                            {
                                ++iRequiredMaterial;
                                for (int i = 0; i < pPlot->getNumUnits(); i++)
                                {
                                    CvUnit* pLoopUnit = pPlot->getUnitByIndex(i);
                                    if(pLoopUnit != NULL)
                                    {
                                        if (pLoopUnit->getOwner() == getOwner() && pLoopUnit->getYield() == eYield)
                                        {
                                            if (pLoopUnit->getYieldStored() >= iRequiredYield)
                                            {
                                                ++iFoundMaterial;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        if (iFoundMaterial != iRequiredMaterial)
                        {
                            return 2;
                        }
                    }
                }
            }

        }
	}
//	if (!(m_pUnitInfo->getBuilds(eBuild)))
//	{
//		return false;
//	}


	if (!(GET_PLAYER(getOwnerINLINE()).canBuild(pPlot, eBuild, false, bTestVisible)))
	{
		return 0;
	}

	if (!pPlot->isValidDomainForAction(*this))
	{
		return 0;
	}

	return -1;
}

///TK end
// Returns true if build finished...
bool CvUnit::build(BuildTypes eBuild)
{
	bool bFinished;

	FAssertMsg(eBuild < GC.getNumBuildInfos(), "Invalid Build");

	if (!canBuild(plot(), eBuild))
	{
		return false;
	}

//	/Tks Med
//	if (isHuman() && (eBuild == (BuildTypes)GC.getDefineINT("DEFAULT_BUILD_MOTTE_AND_BAILEY") || eBuild == (BuildTypes)GC.getDefineINT("DEFAULT_BUILD_CASTLE")))
//	{
//	    int iCost = 0;
//	    PlayerTypes eNativeOwner = NO_PLAYER;
//		for (int i = 0; i < NUM_CITY_PLOTS; ++i)
//		{
//			CvPlot* pLoopPlot = ::plotCity(getX_INLINE(), getY_INLINE(), i);
//			if (pLoopPlot != NULL)
//			{
//				if (pLoopPlot->isOwned() && !pLoopPlot->isCity())
//				{
//					if (GET_PLAYER(pLoopPlot->getOwnerINLINE()).isNative() && !GET_TEAM(pLoopPlot->getTeam()).isAtWar(getTeam()))
//					{
//						eNativeOwner = pLoopPlot->getOwnerINLINE();
//						iCost += pLoopPlot->getBuyPrice(getOwnerINLINE());
//					}
//				}
//			}
//		}
//		if (iCost > 0)
//        {
//            doFoundCheckNatives(GC.getBuildInfo(eBuild).getCityType());
//            return false;
//        }
//
//	}

	// Note: notify entity must come before changeBuildProgress - because once the unit is done building,
	// that function will notify the entity to stop building.
	NotifyEntity((MissionTypes)GC.getBuildInfo(eBuild).getMissionType());

	GET_PLAYER(getOwnerINLINE()).changeGold(-(GET_PLAYER(getOwnerINLINE()).getBuildCost(plot(), eBuild)));

    bool bFound = false;
	///Tk Civics 
	bool bCivic = false;
	if (isHuman() && GET_PLAYER(getOwnerINLINE()).getWorkersBuildAfterMove() > 0)
	{
		bCivic = true;
	}
    int iWorkRate = workRate(bCivic);
    if (GC.getBuildInfo(eBuild).getImprovement() != NO_IMPROVEMENT)
    {
        if (GC.getBuildInfo(eBuild).getCityType() > -1)
        {
            bFound = true;
        }
        if (GC.getImprovementInfo((ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement()).isRequiresCityYields()  && (plot()->getBuildProgress(eBuild) == 0))
        {
            bool bPlotHasBonus = false;
            if (GC.getImprovementInfo((ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement()).getBonusCreated() != NO_BONUS)
            {
                if (plot()->getBonusType() == (BonusTypes)GC.getImprovementInfo((ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement()).getBonusCreated())
                {
                    bPlotHasBonus = true;
                }
            }
            bool bOutside = GC.getImprovementInfo((ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement()).isOutsideBorders();
            if (!bPlotHasBonus || bOutside)
            {
                if (!bOutside)
                {
                    CvCity* pWorkingCity = plot()->getWorkingCity();
                    if (pWorkingCity != NULL)
                    {
                        int iRequiredYield = 0;
                        bool bRequiresYield = false;
                        for (int i = 0; i < NUM_YIELD_TYPES; ++i)
                        {
                            YieldTypes eYield = (YieldTypes) i;
                            iRequiredYield = GC.getImprovementInfo((ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement()).getRequiredCityYields(eYield);
                            if (iRequiredYield > 0 && isHuman())
                            {
                                pWorkingCity->setYieldStored(eYield, (pWorkingCity->getYieldStored(eYield) - iRequiredYield));

                            }
                        }
                    }
                }
                else
                {
                    CvPlot* pPlot = plot();
                    for (int i = 0; i < NUM_YIELD_TYPES; ++i)
                    {
                        YieldTypes eYield = (YieldTypes) i;
                        int iRequiredYield = GC.getImprovementInfo((ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement()).getRequiredCityYields(eYield);
                        if (iRequiredYield > 0)
                        {
                            for (int i = 0; i < pPlot->getNumUnits(); i++)
                            {
                                CvUnit* pLoopUnit = pPlot->getUnitByIndex(i);
                                if(pLoopUnit != NULL)
                                {
                                    if (pLoopUnit->getOwner() == getOwner() && pLoopUnit->getYield() == eYield)
                                    {
                                        if (pLoopUnit->getYieldStored() >= iRequiredYield)
                                        {
											if (pLoopUnit->getYieldStored() > iRequiredYield)
											{
												pLoopUnit->setYieldStored(-iRequiredYield);
											}
											else
											{
												pLoopUnit->kill(true);
											}
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

    }

	bFinished = plot()->changeBuildProgress(eBuild, iWorkRate, getTeam());
    ///TKe
	finishMoves(); // needs to be at bottom because movesLeft() can affect workRate()...

	if (bFinished)
	{
		if (GC.getBuildInfo(eBuild).isKill())
		{
			kill(true);
		}
		///TKs Med
		if (bFound)
		{
		   // plot()->setPlotType(PLOT_HILLS, true, true);
		    doFound(true, GC.getBuildInfo(eBuild).getCityType());
		    //return bFinished;
		}
		///Tke
	}

	// Python Event
	gDLL->getEventReporterIFace()->unitBuildImprovement(this, eBuild, bFinished);

	return bFinished;
}


bool CvUnit::canPromote(PromotionTypes ePromotion, int iLeaderUnitId) const
{
	if (ePromotion == NO_PROMOTION)
	{
		return false;
	}

	if (iLeaderUnitId >= 0)
	{
		if (iLeaderUnitId == getID())
		{
			return false;
		}

		if (!GC.getPromotionInfo(ePromotion).isLeader())
		{
			return false;
		}

		CvUnit* pWarlord = GET_PLAYER(getOwnerINLINE()).getUnit(iLeaderUnitId);
		if (pWarlord == NULL)
		{
			return false;
		}

		if (pWarlord->getUnitInfo().getLeaderPromotion() != ePromotion)
		{
			return false;
		}

		if (!canAcquirePromotion(ePromotion))
		{
			return false;
		}

		if (!canAcquirePromotionAny())
		{
			return false;
		}
	}
	else
	{
		if (GC.getPromotionInfo(ePromotion).isLeader())
		{
			return false;
		}

		if (!canAcquirePromotion(ePromotion))
		{
			return false;
		}

		if (!isPromotionReady())
		{
			return false;
		}
	}

	return true;
}

void CvUnit::promote(PromotionTypes ePromotion, int iLeaderUnitId)
{
	if (!canPromote(ePromotion, iLeaderUnitId))
	{
		return;
	}

	if (iLeaderUnitId >= 0)
	{
		CvUnit* pWarlord = GET_PLAYER(getOwnerINLINE()).getUnit(iLeaderUnitId);
		if (pWarlord)
		{
			pWarlord->giveExperience();
			if (!pWarlord->getNameNoDesc().empty())
			{
				setName(pWarlord->getNameKey());
			}

			//update graphics models
			///Tks Med
			if (!isHuman())
			{
			//m_eLeaderUnitType = pWarlord->getUnitType();
			//reloadEntity();
			}
			///tks
		}
	}

	if (!GC.getPromotionInfo(ePromotion).isLeader())
	{
		changeLevel(1);
		changeDamage(-(getDamage() / 2));
	}
    ///Tks
//    if (isHasRealPromotion((PromotionTypes)GC.getDefineINT("DEFAULT_UNTRAINED_PROMOTION")))
//    {
//        setHasRealPromotion(((PromotionTypes)GC.getDefineINT("DEFAULT_UNTRAINED_PROMOTION")), false);
//    }
    ///Tke
	setHasRealPromotion(ePromotion, true);

	testPromotionReady();

	if (IsSelected())
	{
		gDLL->getInterfaceIFace()->playGeneralSound(GC.getPromotionInfo(ePromotion).getSound());

		gDLL->getInterfaceIFace()->setDirty(UnitInfo_DIRTY_BIT, true);
	}
	else
	{
		setInfoBarDirty(true);
	}

	gDLL->getEventReporterIFace()->unitPromoted(this, ePromotion);

}

bool CvUnit::lead(int iUnitId)
{
	if (!canLead(plot(), iUnitId))
	{
		return false;
	}
	///TKs Med
        changeTraderCode(2);
        if(isHuman())
        {
            giveExperience();
            return true;
        }
		///TKe

	PromotionTypes eLeaderPromotion = (PromotionTypes)m_pUnitInfo->getLeaderPromotion();

	if (-1 == iUnitId)
	{
		FAssert(isHuman());
		CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_LEADUNIT, eLeaderPromotion, getID());
		if (pInfo)
		{
			gDLL->getInterfaceIFace()->addPopup(pInfo, getOwnerINLINE(), true);
		}
		return false;
	}
	else
	{
		CvUnit* pUnit = GET_PLAYER(getOwnerINLINE()).getUnit(iUnitId);
		if (!pUnit || !pUnit->canPromote(eLeaderPromotion, getID()))
		{
			return false;
		}

		pUnit->promote(eLeaderPromotion, getID());

		if (plot()->isActiveVisible(false))
		{
			NotifyEntity(MISSION_LEAD);
		}
        ///TKs Med
        changeTraderCode(2);
        if(!isHuman())
        {
            kill(true);
        }
		///TKe

		return true;
	}
}


int CvUnit::canLead(const CvPlot* pPlot, int iUnitId) const
{
	PROFILE_FUNC();

	if (isDelayedDeath())
	{
		return 0;
	}

	if (NO_UNIT == getUnitType())
	{
		return 0;
	}
    ///TKs Med
    if (getTraderCode() == 2)
    {
        return 0;
    }
    ///TKe
	int iNumUnits = 0;
	CvUnitInfo& kUnitInfo = getUnitInfo();

	if (-1 == iUnitId)
	{
		CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
		while(pUnitNode != NULL)
		{
			CvUnit* pUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = pPlot->nextUnitNode(pUnitNode);
            ///TKs Med
			if (pUnit && pUnit != this && pUnit->isOnMap() && pUnit->getOwnerINLINE() == getOwnerINLINE() && pUnit->canPromote((PromotionTypes)kUnitInfo.getLeaderPromotion(), getID()))
			{
				++iNumUnits;
			}
			///Tke
		}
	}
	else
	{
		CvUnit* pUnit = GET_PLAYER(getOwnerINLINE()).getUnit(iUnitId);
		if (pUnit && pUnit != this && pUnit->canPromote((PromotionTypes)kUnitInfo.getLeaderPromotion(), getID()))
		{
			iNumUnits = 1;
		}
	}
	return iNumUnits;
}


int CvUnit::canGiveExperience(const CvPlot* pPlot) const
{
	int iNumUnits = 0;

	if (NO_UNIT != getUnitType() && m_pUnitInfo->getLeaderExperience() > 0)
	{
		CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
		while(pUnitNode != NULL)
		{
			CvUnit* pUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = pPlot->nextUnitNode(pUnitNode);

			if (pUnit && pUnit != this && pUnit->getOwnerINLINE() == getOwnerINLINE() && pUnit->canAcquirePromotionAny())
			{
				++iNumUnits;
			}
		}
	}

	return iNumUnits;
}

bool CvUnit::giveExperience()
{
	CvPlot* pPlot = plot();

	if (pPlot)
	{
		int iNumUnits = canGiveExperience(pPlot);
		if (iNumUnits > 0)
		{
			int iTotalExperience = getStackExperienceToGive(iNumUnits);

			int iMinExperiencePerUnit = iTotalExperience / iNumUnits;
			int iRemainder = iTotalExperience % iNumUnits;

			CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
			int i = 0;
			while(pUnitNode != NULL)
			{
				CvUnit* pUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = pPlot->nextUnitNode(pUnitNode);

				if (pUnit && pUnit != this && pUnit->getOwnerINLINE() == getOwnerINLINE() && pUnit->canAcquirePromotionAny())
				{
					pUnit->changeExperience(i < iRemainder ? iMinExperiencePerUnit+1 : iMinExperiencePerUnit);
					pUnit->testPromotionReady();
				}

				i++;
			}

			return true;
		}
	}

	return false;
}

int CvUnit::getStackExperienceToGive(int iNumUnits) const
{
	return (m_pUnitInfo->getLeaderExperience() * (100 + std::min(50, (iNumUnits - 1) * GC.getXMLval(XML_WARLORD_EXTRA_EXPERIENCE_PER_UNIT_PERCENT)))) / 100;
}

int CvUnit::upgradePrice(UnitTypes eUnit) const
{
	int iPrice;

	CyArgsList argsList;
	argsList.add(getOwner());
	argsList.add(getID());
	argsList.add((int) eUnit);
	long lResult=0;
	gDLL->getPythonIFace()->callFunction(PYGameModule, "getUpgradePriceOverride", argsList.makeFunctionArgs(), &lResult);
	if (lResult >= 0)
	{
		return lResult;
	}

	iPrice = GC.getXMLval(XML_BASE_UNIT_UPGRADE_COST);

	iPrice += (std::max(0, (GET_PLAYER(getOwnerINLINE()).getYieldProductionNeeded(eUnit, YIELD_HAMMERS) - GET_PLAYER(getOwnerINLINE()).getYieldProductionNeeded(getUnitType(), YIELD_HAMMERS))) * GC.getXMLval(XML_UNIT_UPGRADE_COST_PER_PRODUCTION));

	if (!isHuman())
	{
		iPrice *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIUnitUpgradePercent();
		iPrice /= 100;

		iPrice *= std::max(0, ((GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIPerEraModifier() * GET_PLAYER(getOwnerINLINE()).getCurrentEra()) + 100));
		iPrice /= 100;
	}

	iPrice -= (iPrice * getUpgradeDiscount()) / 100;

	return iPrice;
}


bool CvUnit::upgradeAvailable(UnitTypes eFromUnit, UnitClassTypes eToUnitClass, int iCount) const
{
	UnitTypes eLoopUnit;
	int iI;
	int numUnitClassInfos = GC.getNumUnitClassInfos();

	if (iCount > numUnitClassInfos)
	{
		return false;
	}

	CvUnitInfo &fromUnitInfo = GC.getUnitInfo(eFromUnit);

	if (fromUnitInfo.getUpgradeUnitClass(eToUnitClass))
	{
		return true;
	}

	for (iI = 0; iI < numUnitClassInfos; iI++)
	{
		if (fromUnitInfo.getUpgradeUnitClass(iI))
		{
			eLoopUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI)));

			if (eLoopUnit != NO_UNIT)
			{
				if (upgradeAvailable(eLoopUnit, eToUnitClass, (iCount + 1)))
				{
					return true;
				}
			}
		}
	}

	return false;
}


bool CvUnit::canUpgrade(UnitTypes eUnit, bool bTestVisible) const
{
	if (eUnit == NO_UNIT)
	{
		return false;
	}

	if(!isReadyForUpgrade())
	{
		return false;
	}

	if (!bTestVisible)
	{
		if (GET_PLAYER(getOwnerINLINE()).getGold() < upgradePrice(eUnit))
		{
			return false;
		}
	}

	if (hasUpgrade(eUnit))
	{
		return true;
	}

	return false;
}

bool CvUnit::isReadyForUpgrade() const
{
	if (!canMove())
	{
		return false;
	}

	if (plot()->getTeam() != getTeam())
	{
		return false;
	}

	return true;
}

// has upgrade is used to determine if an upgrade is possible,
// it specifically does not check whether the unit can move, whether the current plot is owned, enough gold
// those are checked in canUpgrade()
// does not search all cities, only checks the closest one
bool CvUnit::hasUpgrade(bool bSearch) const
{
	return (getUpgradeCity(bSearch) != NULL);
}

// has upgrade is used to determine if an upgrade is possible,
// it specifically does not check whether the unit can move, whether the current plot is owned, enough gold
// those are checked in canUpgrade()
// does not search all cities, only checks the closest one
bool CvUnit::hasUpgrade(UnitTypes eUnit, bool bSearch) const
{
	return (getUpgradeCity(eUnit, bSearch) != NULL);
}

// finds the 'best' city which has a valid upgrade for the unit,
// it specifically does not check whether the unit can move, or if the player has enough gold to upgrade
// those are checked in canUpgrade()
// if bSearch is true, it will check every city, if not, it will only check the closest valid city
// NULL result means the upgrade is not possible
CvCity* CvUnit::getUpgradeCity(bool bSearch) const
{
	CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());
	UnitAITypes eUnitAI = AI_getUnitAIType();
	CvArea* pArea = area();

	int iCurrentValue = kPlayer.AI_unitValue(getUnitType(), eUnitAI, pArea);

	int iBestSearchValue = MAX_INT;
	CvCity* pBestUpgradeCity = NULL;

	for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		int iNewValue = kPlayer.AI_unitValue(((UnitTypes)iI), eUnitAI, pArea);
		if (iNewValue > iCurrentValue)
		{
			int iSearchValue;
			CvCity* pUpgradeCity = getUpgradeCity((UnitTypes)iI, bSearch, &iSearchValue);
			if (pUpgradeCity != NULL)
			{
				// if not searching or close enough, then this match will do
				if (!bSearch || iSearchValue < 16)
				{
					return pUpgradeCity;
				}

				if (iSearchValue < iBestSearchValue)
				{
					iBestSearchValue = iSearchValue;
					pBestUpgradeCity = pUpgradeCity;
				}
			}
		}
	}

	return pBestUpgradeCity;
}

// finds the 'best' city which has a valid upgrade for the unit, to eUnit type
// it specifically does not check whether the unit can move, or if the player has enough gold to upgrade
// those are checked in canUpgrade()
// if bSearch is true, it will check every city, if not, it will only check the closest valid city
// if iSearchValue non NULL, then on return it will be the city's proximity value, lower is better
// NULL result means the upgrade is not possible
CvCity* CvUnit::getUpgradeCity(UnitTypes eUnit, bool bSearch, int* iSearchValue) const
{
	if (eUnit == NO_UNIT)
	{
		return false;
	}

	CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());
	CvUnitInfo& kUnitInfo = GC.getUnitInfo(eUnit);

	if (GC.getCivilizationInfo(kPlayer.getCivilizationType()).getCivilizationUnits(kUnitInfo.getUnitClassType()) != eUnit)
	{
		return false;
	}

	if (!upgradeAvailable(getUnitType(), ((UnitClassTypes)(kUnitInfo.getUnitClassType()))))
	{
		return false;
	}

	if (kUnitInfo.getCargoSpace() < getCargo())
	{
		return false;
	}

	CLLNode<IDInfo>* pUnitNode = plot()->headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = plot()->nextUnitNode(pUnitNode);

		if (pLoopUnit->getTransportUnit() == this)
		{
			if (kUnitInfo.getSpecialCargo() != NO_SPECIALUNIT)
			{
				if (kUnitInfo.getSpecialCargo() != pLoopUnit->getSpecialUnitType())
				{
					return false;
				}
			}

			if (kUnitInfo.getDomainCargo() != NO_DOMAIN)
			{
				if (kUnitInfo.getDomainCargo() != pLoopUnit->getDomainType())
				{
					return false;
				}
			}
		}
	}

	// sea units must be built on the coast
	bool bCoastalOnly = (getDomainType() == DOMAIN_SEA);

	// results
	int iBestValue = MAX_INT;
	CvCity* pBestCity = NULL;

	// if search is true, check every city for our team
	if (bSearch)
	{
		TeamTypes eTeam = getTeam();
		int iArea = getArea();
		int iX = getX_INLINE(), iY = getY_INLINE();

		// check every player on our team's cities
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			// is this player on our team?
			CvPlayerAI& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
			if (kLoopPlayer.isAlive() && kLoopPlayer.getTeam() == eTeam)
			{
				int iLoop;
				for (CvCity* pLoopCity = kLoopPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kLoopPlayer.nextCity(&iLoop))
				{
					// if coastal only, then make sure we are coast
					CvArea* pWaterArea = NULL;
					if (!bCoastalOnly || ((pWaterArea = pLoopCity->waterArea()) != NULL && !pWaterArea->isLake()))
					{
						// can this city tran this unit?
						if (pLoopCity->canTrain(eUnit, false, false, true))
						{
							int iValue = plotDistance(iX, iY, pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE());

							// if not same area, not as good (lower numbers are better)
							if (iArea != pLoopCity->getArea() && (!bCoastalOnly || iArea != pWaterArea->getID()))
							{
								iValue *= 16;
							}

							// if we cannot path there, not as good (lower numbers are better)
							if (!generatePath(pLoopCity->plot(), 0, true))
							{
								iValue *= 16;
							}

							if (iValue < iBestValue)
							{
								iBestValue = iValue;
								pBestCity = pLoopCity;
							}
						}
					}
				}
			}
		}
	}
	else
	{
		// find the closest city
		CvCity* pClosestCity = GC.getMapINLINE().findCity(getX_INLINE(), getY_INLINE(), NO_PLAYER, getTeam(), true, bCoastalOnly);
		if (pClosestCity != NULL)
		{
			// if we can train, then return this city (otherwise it will return NULL)
			if (pClosestCity->canTrain(eUnit, false, false, true))
			{
				// did not search, always return 1 for search value
				iBestValue = 1;

				pBestCity = pClosestCity;
			}
		}
	}

	// return the best value, if non-NULL
	if (iSearchValue != NULL)
	{
		*iSearchValue = iBestValue;
	}

	return pBestCity;
}

void CvUnit::upgrade(UnitTypes eUnit)
{
	CvUnit* pUpgradeUnit;

	if (!canUpgrade(eUnit))
	{
		return;
	}

	GET_PLAYER(getOwnerINLINE()).changeGold(-(upgradePrice(eUnit)));

	pUpgradeUnit = GET_PLAYER(getOwnerINLINE()).initUnit(eUnit, getProfession(), getX_INLINE(), getY_INLINE(), AI_getUnitAIType());

	FAssertMsg(pUpgradeUnit != NULL, "UpgradeUnit is not assigned a valid value");

	pUpgradeUnit->joinGroup(getGroup());

	pUpgradeUnit->convert(this, true);

	pUpgradeUnit->finishMoves();

	if (pUpgradeUnit->getLeaderUnitType() == NO_UNIT)
	{
		if (pUpgradeUnit->getExperience() > GC.getXMLval(XML_MAX_EXPERIENCE_AFTER_UPGRADE))
		{
			pUpgradeUnit->setExperience(GC.getXMLval(XML_MAX_EXPERIENCE_AFTER_UPGRADE));
		}
	}
}


HandicapTypes CvUnit::getHandicapType() const
{
	return GET_PLAYER(getOwnerINLINE()).getHandicapType();
}


CivilizationTypes CvUnit::getCivilizationType() const
{
	return GET_PLAYER(getOwnerINLINE()).getCivilizationType();
}

const wchar* CvUnit::getVisualCivAdjective(TeamTypes eForTeam) const
{
	if (getVisualOwner(eForTeam) == getOwnerINLINE())
	{
		return GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey();
	}

	return L"";
}

SpecialUnitTypes CvUnit::getSpecialUnitType() const
{
	return ((SpecialUnitTypes)(m_pUnitInfo->getSpecialUnitType()));
}


UnitTypes CvUnit::getCaptureUnitType(CivilizationTypes eCivilization) const
{
	FAssert(eCivilization != NO_CIVILIZATION);
    UnitTypes eCaptureUnit = NO_UNIT;
	if(m_pUnitInfo->getUnitCaptureClassType() != NO_UNITCLASS)
	{
		eCaptureUnit = (UnitTypes)GC.getCivilizationInfo(eCivilization).getCivilizationUnits(m_pUnitInfo->getUnitCaptureClassType());
	}

	if (eCaptureUnit == NO_UNIT && isUnarmed())
	{
		eCaptureUnit = (UnitTypes)GC.getCivilizationInfo(eCivilization).getCivilizationUnits(getUnitClassType());
	}

	if (eCaptureUnit == NO_UNIT)
	{
		return NO_UNIT;
	}

	///TKs Med
	if (!GET_PLAYER(GC.getGameINLINE().getActivePlayer()).canUseYield(YIELD_FROM_ANIMALS))
	{
		return NO_UNIT;
	}
#if 0
	// this section is replaced by a simple check to see if the player can use the yield in question
	 if (YIELD_FROM_ANIMALS != NO_YIELD && eCaptureUnit != NO_UNIT)
	 {
	    //YieldTypes eCapturedYield = (YieldTypes)m_pUnitInfo->getYield();
	    if (GC.getYieldInfo(YIELD_FROM_ANIMALS).getUnitClass() == m_pUnitInfo->getUnitCaptureClassType())
	    {
            for (int iCivic = 0; iCivic < GC.getNumCivicInfos(); ++iCivic)
            {
                if (GC.getCivicInfo((CivicTypes) iCivic).getCivicOptionType() == CIVICOPTION_INVENTIONS)
                {
                    CvCivicInfo& kCivicInfo = GC.getCivicInfo((CivicTypes) iCivic);
                    if (kCivicInfo.getAllowsYields(YIELD_FROM_ANIMALS) > 0)
                    {
                        if (GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getIdeasResearched((CivicTypes) iCivic) == 0)
                        {
                            return NO_UNIT;
                        }
                    }
                }
            }
	    }
	 }
#endif
	///TKe

	CvUnitInfo& kUnitInfo = GC.getUnitInfo(eCaptureUnit);
	if (kUnitInfo.getDefaultProfession() != NO_PROFESSION)
	{
		CvCivilizationInfo& kCivInfo = GC.getCivilizationInfo(eCivilization);
		if (!kCivInfo.isValidProfession(kUnitInfo.getDefaultProfession()))
		{
			return NO_UNIT;
		}
	}

	return eCaptureUnit;
}

UnitCombatTypes CvUnit::getProfessionUnitCombatType(ProfessionTypes eProfession) const
{
	if (eProfession != NO_PROFESSION)
	{
		UnitCombatTypes eUnitCombat = (UnitCombatTypes) GC.getProfessionInfo(eProfession).getUnitCombatType();
		if(eUnitCombat != NO_UNITCOMBAT)
		{
			return eUnitCombat;
		}
	}

	return ((UnitCombatTypes)(m_pUnitInfo->getUnitCombatType()));
}

// CombatGearTypes - start - Nightinggale
bool CvUnit::hasUnitCombatType(UnitCombatTypes eUnitCombat) const
{
	if (getProfession() != NO_PROFESSION)
	{
		if (GC.getProfessionInfo(getProfession()).getCombatGearTypes(eUnitCombat))
		{
			return true;
		}
	}
	return ((UnitCombatTypes)(m_pUnitInfo->getUnitCombatType())) == eUnitCombat;
}
// CombatGearTypes - end - Nightinggale

void CvUnit::processUnitCombatType(UnitCombatTypes eUnitCombat, int iChange)
{
	if (iChange != 0)
	{
		//update unit combat changes
		for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
		{
			if (GET_PLAYER(getOwnerINLINE()).hasTrait((TraitTypes)iI))
			{
				for (int iJ = 0; iJ < GC.getNumPromotionInfos(); iJ++)
				{
					if (GC.getTraitInfo((TraitTypes) iI).isFreePromotion(iJ))
					{
						if ((eUnitCombat != NO_UNITCOMBAT) && GC.getTraitInfo((TraitTypes) iI).isFreePromotionUnitCombat(eUnitCombat))
						{
							changeFreePromotionCount(((PromotionTypes)iJ), iChange);
						}
					}
				}
			}
		}

		if (NO_UNITCOMBAT != eUnitCombat)
		{
			for (int iJ = 0; iJ < GC.getNumPromotionInfos(); iJ++)
			{
				if (GET_PLAYER(getOwnerINLINE()).isFreePromotion(eUnitCombat, (PromotionTypes)iJ))
				{
					changeFreePromotionCount(((PromotionTypes)iJ), iChange);
				}
			}
		}

		if (IsSelected())
		{
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
		}
	}
}

UnitCombatTypes CvUnit::getUnitCombatType() const
{
	return getProfessionUnitCombatType(getProfession());
}


DomainTypes CvUnit::getDomainType() const
{
	return ((DomainTypes)(m_pUnitInfo->getDomainType()));
}


InvisibleTypes CvUnit::getInvisibleType() const
{
	return ((InvisibleTypes)(m_pUnitInfo->getInvisibleType()));
}

int CvUnit::getNumSeeInvisibleTypes() const
{
	return m_pUnitInfo->getNumSeeInvisibleTypes();
}

InvisibleTypes CvUnit::getSeeInvisibleType(int i) const
{
	return (InvisibleTypes)(m_pUnitInfo->getSeeInvisibleType(i));
}

bool CvUnit::isHuman() const
{
	return GET_PLAYER(getOwnerINLINE()).isHuman();
}

bool CvUnit::isNative() const
{
	return GET_PLAYER(getOwnerINLINE()).isNative();
}

int CvUnit::visibilityRange() const
{
	return (GC.getXMLval(XML_UNIT_VISIBILITY_RANGE) + getExtraVisibilityRange());
}

int CvUnit::baseMoves() const
{
	int iBaseMoves = m_pUnitInfo->getMoves();
	iBaseMoves += getExtraMoves();
	iBaseMoves += GET_PLAYER(getOwnerINLINE()).getUnitMoveChange(getUnitClassType());

	if(getProfession() != NO_PROFESSION)
	{
		iBaseMoves += GET_PLAYER(getOwnerINLINE()).getProfessionMoveChange(getProfession());
	}

	return iBaseMoves;
}


int CvUnit::maxMoves() const
{
	return (baseMoves() * GC.getMOVE_DENOMINATOR());
}


int CvUnit::movesLeft() const
{
	return std::max(0, (maxMoves() - getMoves()));
}


bool CvUnit::canMove() const
{
	if (isDead())
	{
		return false;
	}

	if (getMoves() >= maxMoves())
	{
		return false;
	}

	if (getImmobileTimer() > 0)
	{
		return false;
	}

	if (!isOnMap())
	{
		return false;
	}

	return true;
}


bool CvUnit::hasMoved()	const
{
	return (getMoves() > 0);
}


// XXX should this test for coal?
bool CvUnit::canBuildRoute() const
{
	int iI;

	for (iI = 0; iI < GC.getNumBuildInfos(); iI++)
	{
		if (GC.getBuildInfo((BuildTypes)iI).getRoute() != NO_ROUTE)
		{
		    ///TKs Med
		    for (int iCivic = 0; iCivic < GC.getNumCivicInfos(); ++iCivic)
            {
                if (GC.getCivicInfo((CivicTypes) iCivic).getCivicOptionType() == CIVICOPTION_INVENTIONS)
                {
                    CvCivicInfo& kCivicInfo = GC.getCivicInfo((CivicTypes) iCivic);
                    if (kCivicInfo.getAllowsRoute(GC.getBuildInfo((BuildTypes)iI).getRoute()) > 0)
                    {
                        if (GET_PLAYER(getOwner()).getIdeasResearched((CivicTypes) iCivic) == 0)
                        {
                            return false;
                        }
                    }
                }
            }
		    ///Tke
			if (m_pUnitInfo->getBuilds(iI))
			{
				return true;
			}
		}
	}

	return false;
}

BuildTypes CvUnit::getBuildType() const
{
	CvSelectionGroup* pGroup = getGroup();
	if (pGroup == NULL)
	{
		return NO_BUILD;
	}

	if (pGroup->headMissionQueueNode() != NULL)
	{
		switch (pGroup->headMissionQueueNode()->m_data.eMissionType)
		{
		case MISSION_MOVE_TO:
			break;

		case MISSION_ROUTE_TO:
			{
				BuildTypes eBuild;
				if (pGroup->getBestBuildRoute(plot(), &eBuild) != NO_ROUTE)
				{
					return eBuild;
				}
			}
			break;

		case MISSION_MOVE_TO_UNIT:
		case MISSION_SKIP:
		case MISSION_SLEEP:
		case MISSION_FORTIFY:
		case MISSION_HEAL:
		case MISSION_SENTRY:
		case MISSION_BOMBARD:
		case MISSION_PILLAGE:
		case MISSION_FOUND:
		///TKs Med
        case MISSION_FOUND_MONASTERY:
        case MISSION_FOUND_OUTPOST:
        case MISSION_COLLECT_TAXES:
        case MISSION_HUNT:
        //TKe
		case MISSION_JOIN_CITY:
		case MISSION_LEAD:
			break;

		case MISSION_BUILD:
			return (BuildTypes)pGroup->headMissionQueueNode()->m_data.iData1;
			break;

		default:
			FAssert(false);
			break;
		}
	}

	return NO_BUILD;
}


int CvUnit::workRate(bool bMax) const
{
	///Tks Civics
	if (!bMax)
	{
		if (!canMove())
		{
			return 0;
		}
	}

	int iRate = m_pUnitInfo->getWorkRate() + getExtraWorkRate();

	iRate *= std::max(0, (GET_PLAYER(getOwnerINLINE()).getWorkerSpeedModifier() + m_pUnitInfo->getWorkRateModifier() + 100));
	iRate /= 100;

	if (!isHuman())
	{
		iRate *= std::max(0, (GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIWorkRateModifier() + 100));
		iRate /= 100;
	}

	return iRate;
}

void CvUnit::changeExtraWorkRate(int iChange)
{
	m_iExtraWorkRate += iChange;
}

int CvUnit::getExtraWorkRate() const
{
	return m_iExtraWorkRate;

}

bool CvUnit::isNoBadGoodies() const
{
    ///TKs Med
    for (int iPromotion = 0; iPromotion < GC.getNumPromotionInfos(); ++iPromotion)
    {
        if (isHasPromotion((PromotionTypes) iPromotion))
        {
            if (GC.getPromotionInfo((PromotionTypes) iPromotion).isNoBadGoodies())
            {
                return true;
            }
        }
    }
    ///Tke
	return m_pUnitInfo->isNoBadGoodies();
}


bool CvUnit::isOnlyDefensive() const
{
	return m_pUnitInfo->isOnlyDefensive();
}


bool CvUnit::isNoUnitCapture() const
{
	return m_pUnitInfo->isNoCapture();
}


bool CvUnit::isNoCityCapture() const
{
	return m_pUnitInfo->isNoCapture();
}


bool CvUnit::isRivalTerritory() const
{
	return m_pUnitInfo->isRivalTerritory();
}

bool CvUnit::canCoexistWithEnemyUnit(TeamTypes eTeam) const
{
	if (!m_pUnitInfo->isInvisible())
	{
		if (getInvisibleType() == NO_INVISIBLE)
		{
			return false;
		}

		if (NO_TEAM == eTeam || plot()->isInvisibleVisible(eTeam, getInvisibleType()))
		{
			return false;
		}
	}

	///TKs Med
//	if (isOnlyDefensive())
//	{
//	    CLLNode<IDInfo>* pUnitNode;
//        CvUnit* pLoopUnit;
//        CvPlot* pPlot;
//
//        pPlot = plot();
//
//        pUnitNode = pPlot->headUnitNode();
//
//        while (pUnitNode != NULL)
//        {
//            pLoopUnit = ::getUnit(pUnitNode->m_data);
//            pUnitNode = pPlot->nextUnitNode(pUnitNode);
//
//            if (pLoopUnit->

	//}
	///Tke

	return true;
}

bool CvUnit::isFighting() const
{
	return (getCombatUnit() != NULL);
}


bool CvUnit::isAttacking() const
{
	return (getAttackPlot() != NULL && !isDelayedDeath());
}


bool CvUnit::isDefending() const
{
	return (isFighting() && !isAttacking());
}


bool CvUnit::isCombat() const
{
	return (isFighting() || isAttacking());
}


int CvUnit::maxHitPoints() const
{
	return GC.getMAX_HIT_POINTS();
}


int CvUnit::currHitPoints()	const
{
	return (maxHitPoints() - getDamage());
}


bool CvUnit::isHurt() const
{
	return (getDamage() > 0);
}


bool CvUnit::isDead() const
{
	return (getDamage() >= maxHitPoints());
}


void CvUnit::setBaseCombatStr(int iCombat)
{
	m_iBaseCombat = iCombat;
	updateBestLandCombat();
}

int CvUnit::baseCombatStr() const
{
	return m_iBaseCombat;
}

void CvUnit::updateBestLandCombat()
{
	if (getDomainType() == DOMAIN_LAND)
	{
		if (baseCombatStr() > GC.getGameINLINE().getBestLandUnitCombat())
		{
			GC.getGameINLINE().setBestLandUnitCombat(baseCombatStr());
		}
	}
}


// maxCombatStr can be called in four different configurations
//		pPlot == NULL, pAttacker == NULL for combat when this is the attacker
//		pPlot valid, pAttacker valid for combat when this is the defender
//		pPlot valid, pAttacker == NULL (new case), when this is the defender, attacker unknown
//		pPlot valid, pAttacker == this (new case), when the defender is unknown, but we want to calc approx str
//			note, in this last case, it is expected pCombatDetails == NULL, it does not have to be, but some
//			values may be unexpectedly reversed in this case (iModifierTotal will be the negative sum)
int CvUnit::maxCombatStr(const CvPlot* pPlot, const CvUnit* pAttacker, CombatDetails* pCombatDetails) const
{
	int iCombat;

	FAssertMsg((pPlot == NULL) || (pPlot->getTerrainType() != NO_TERRAIN), "(pPlot == NULL) || (pPlot->getTerrainType() is not expected to be equal with NO_TERRAIN)");

	// handle our new special case
	const	CvPlot*	pAttackedPlot = NULL;
	bool	bAttackingUnknownDefender = false;
	if (pAttacker == this)
	{
		bAttackingUnknownDefender = true;
		pAttackedPlot = pPlot;

		// reset these values, we will fiddle with them below
		pPlot = NULL;
		pAttacker = NULL;
	}
	// otherwise, attack plot is the plot of us (the defender)
	else if (pAttacker != NULL)
	{
		pAttackedPlot = plot();
	}

	if (pCombatDetails != NULL)
	{
		pCombatDetails->iExtraCombatPercent = 0;
		pCombatDetails->iNativeCombatModifierTB = 0;
		pCombatDetails->iNativeCombatModifierAB = 0;
		pCombatDetails->iPlotDefenseModifier = 0;
		pCombatDetails->iFortifyModifier = 0;
		pCombatDetails->iCityDefenseModifier = 0;
		pCombatDetails->iHillsAttackModifier = 0;
		pCombatDetails->iHillsDefenseModifier = 0;
		pCombatDetails->iFeatureAttackModifier = 0;
		pCombatDetails->iFeatureDefenseModifier = 0;
		pCombatDetails->iTerrainAttackModifier = 0;
		pCombatDetails->iTerrainDefenseModifier = 0;
		pCombatDetails->iCityAttackModifier = 0;
		pCombatDetails->iDomainDefenseModifier = 0;
		pCombatDetails->iClassDefenseModifier = 0;
		pCombatDetails->iClassAttackModifier = 0;
		pCombatDetails->iCombatModifierA = 0;
		pCombatDetails->iCombatModifierT = 0;
		pCombatDetails->iDomainModifierA = 0;
		pCombatDetails->iDomainModifierT = 0;
		pCombatDetails->iRiverAttackModifier = 0;
		pCombatDetails->iAmphibAttackModifier = 0;
		pCombatDetails->iRebelPercentModifier = 0;
		pCombatDetails->iModifierTotal = 0;
		pCombatDetails->iBaseCombatStr = 0;
		pCombatDetails->iCombat = 0;
		pCombatDetails->iMaxCombatStr = 0;
		pCombatDetails->iCurrHitPoints = 0;
		pCombatDetails->iMaxHitPoints = 0;
		pCombatDetails->iCurrCombatStr = 0;
		pCombatDetails->eOwner = getOwnerINLINE();
		pCombatDetails->eVisualOwner = getVisualOwner();
		if (getProfession() == NO_PROFESSION)
		{
			pCombatDetails->sUnitName = getName().GetCString();
		}
		else
		{
			pCombatDetails->sUnitName = CvWString::format(L"%s (%s)", GC.getProfessionInfo(getProfession()).getDescription(), getName().GetCString());
		}
	}
	
	if (baseCombatStr() == 0)
	{
		return 0;
	}

	int iModifier = 0;
	int iExtraModifier;

	iExtraModifier = getExtraCombatPercent();
	iModifier += iExtraModifier;
	if (pCombatDetails != NULL)
	{
		pCombatDetails->iExtraCombatPercent = iExtraModifier;
	}
	
	if (pAttacker != NULL)
	{
		///Tks Civics
		iModifier += GET_PLAYER(getOwnerINLINE()).calculateCivicCombatBonuses(pAttacker->getOwnerINLINE());
	    ///TK Med
		if (isNative() || m_pUnitInfo->getCasteAttribute() == 7)
		{
			iExtraModifier = -GET_PLAYER(pAttacker->getOwnerINLINE()).getNativeCombatModifier();
			if (!pAttacker->isHuman())
			{
				iExtraModifier -= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAINativeCombatModifier();
			}
			iModifier += iExtraModifier;
			if (pCombatDetails != NULL)
			{
				pCombatDetails->iNativeCombatModifierTB = iExtraModifier;
			}
		}

		if (pAttacker->isNative() || GC.getUnitInfo(pAttacker->getUnitType()).getCasteAttribute() == 7)
		{
			iExtraModifier = GET_PLAYER(getOwnerINLINE()).getNativeCombatModifier();
			if (!isHuman())
			{
				iExtraModifier += GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAINativeCombatModifier();
			}
			iModifier += iExtraModifier;
			if (pCombatDetails != NULL)
			{
				pCombatDetails->iNativeCombatModifierAB = iExtraModifier;
			}
		}
        ///TKe
		iExtraModifier = rebelModifier(pAttacker->getOwnerINLINE()) - pAttacker->rebelModifier(getOwnerINLINE());
		iModifier += iExtraModifier;
		if (pCombatDetails != NULL)
		{
			pCombatDetails->iRebelPercentModifier = iExtraModifier;
		}
	}

	// add defensive bonuses (leaving these out for bAttackingUnknownDefender case)
	if (pPlot != NULL)
	{
		if (!noDefensiveBonus())
		{
			iExtraModifier = pPlot->defenseModifier(getTeam());
			iModifier += iExtraModifier;
			if (pCombatDetails != NULL)
			{
				pCombatDetails->iPlotDefenseModifier = iExtraModifier;
			}
		}

		iExtraModifier = fortifyModifier();
		iModifier += iExtraModifier;
		if (pCombatDetails != NULL)
		{
			pCombatDetails->iFortifyModifier = iExtraModifier;
		}

		if (pPlot->isCity(true, getTeam()))
		{
			iExtraModifier = cityDefenseModifier();
			iModifier += iExtraModifier;
			if (pCombatDetails != NULL)
			{
				pCombatDetails->iCityDefenseModifier = iExtraModifier;
			}
		}

		if (pPlot->isHills() || pPlot->isPeak())
		{
			iExtraModifier = hillsDefenseModifier();
			iModifier += iExtraModifier;
			if (pCombatDetails != NULL)
			{
				pCombatDetails->iHillsDefenseModifier = iExtraModifier;
			}
		}

		if (pPlot->getFeatureType() != NO_FEATURE)
		{
			iExtraModifier = featureDefenseModifier(pPlot->getFeatureType());
			iModifier += iExtraModifier;
			if (pCombatDetails != NULL)
			{
				pCombatDetails->iFeatureDefenseModifier = iExtraModifier;
			}
		}
		else
		{
			iExtraModifier = terrainDefenseModifier(pPlot->getTerrainType());
			iModifier += iExtraModifier;
			if (pCombatDetails != NULL)
			{
				pCombatDetails->iTerrainDefenseModifier = iExtraModifier;
			}
		}
	}

	// if we are attacking to an plot with an unknown defender, the calc the modifier in reverse
	if (bAttackingUnknownDefender)
	{
		pAttacker = this;
	}

	// calc attacker bonueses
	if (pAttacker != NULL)
	{
		int iTempModifier = 0;
		///Tks Civics
		//iExtraModifier = GET_PLAYER(pAttacker->getOwnerINLINE()).calculateCivicCombatBonuses(getOwnerINLINE());
	    ///TK Med
		if (pAttackedPlot->isCity(true, getTeam()))
		{
			iExtraModifier = -pAttacker->cityAttackModifier();
			iTempModifier += iExtraModifier;
			if (pCombatDetails != NULL)
			{
				pCombatDetails->iCityAttackModifier = iExtraModifier;
			}
		}

		if (pAttackedPlot->isHills() || pAttackedPlot->isPeak())
		{
			iExtraModifier = -pAttacker->hillsAttackModifier();
			iTempModifier += iExtraModifier;
			if (pCombatDetails != NULL)
			{
				pCombatDetails->iHillsAttackModifier = iExtraModifier;
			}
		}

		if (pAttackedPlot->getFeatureType() != NO_FEATURE)
		{
			iExtraModifier = -pAttacker->featureAttackModifier(pAttackedPlot->getFeatureType());
			iTempModifier += iExtraModifier;
			if (pCombatDetails != NULL)
			{
				pCombatDetails->iFeatureAttackModifier = iExtraModifier;
			}
		}
		else
		{
			iExtraModifier = -pAttacker->terrainAttackModifier(pAttackedPlot->getTerrainType());
			iModifier += iExtraModifier;
			if (pCombatDetails != NULL)
			{
				pCombatDetails->iTerrainAttackModifier = iExtraModifier;
			}
		}

		// only compute comparisions if we are the defender with a known attacker
		if (!bAttackingUnknownDefender)
		{
			FAssertMsg(pAttacker != this, "pAttacker is not expected to be equal with this");

			iExtraModifier = unitClassDefenseModifier(pAttacker->getUnitClassType());
			iTempModifier += iExtraModifier;
			if (pCombatDetails != NULL)
			{
				pCombatDetails->iClassDefenseModifier = iExtraModifier;
			}

			iExtraModifier = -pAttacker->unitClassAttackModifier(getUnitClassType());
			iTempModifier += iExtraModifier;
			if (pCombatDetails != NULL)
			{
				pCombatDetails->iClassAttackModifier = iExtraModifier;
			}

			// CombatGearTypes - start - Nightinggale
			{
				int iDefendsModifier = 0;
				int iDefendsModifierMax = -9999;
				int iAttackModifier = 0;
				int iAttackModifierMax = -9999;

				for (int i = 0; i < GC.getNumUnitCombatInfos(); i++)
				{
					UnitCombatTypes eUnitCombat = (UnitCombatTypes) i;

					if (pAttacker->hasUnitCombatType(eUnitCombat))
					{
						int iModifier = unitCombatModifier(eUnitCombat);
						if (iModifier != 0)
						{
							iDefendsModifier += iModifier;
							iDefendsModifierMax = std::max(iDefendsModifierMax, iModifier);
						}
					}

					if (hasUnitCombatType(eUnitCombat))
					{
						int iModifier = pAttacker->unitCombatModifier(eUnitCombat);
						if (iModifier != 0)
						{
							iAttackModifier += iModifier;
							iAttackModifierMax = std::max(iAttackModifierMax, iModifier);
						}
					}
				}

				if (GC.getXMLval(XML_UNITCOMBAT_USE_ALL_BONUS) == 0)
				{
					if (iDefendsModifierMax != -9999)
					{
						iDefendsModifier = iDefendsModifierMax;
					}
					if (iAttackModifierMax != -9999)
					{
						iAttackModifier = iAttackModifierMax;
					}
				}

				iAttackModifier = -iAttackModifier;

				iTempModifier += iDefendsModifier + iAttackModifier;
				if (pCombatDetails != NULL)
				{
					pCombatDetails->iCombatModifierA = iDefendsModifier;
					pCombatDetails->iCombatModifierT = iAttackModifier;
				}
			}
			// CombatGearTypes - end - Nightinggale
#if 0
			if (pAttacker->getUnitCombatType() != NO_UNITCOMBAT)
			{
				iExtraModifier = unitCombatModifier(pAttacker->getUnitCombatType());
				iTempModifier += iExtraModifier;
				if (pCombatDetails != NULL)
				{
					pCombatDetails->iCombatModifierA = iExtraModifier;
				}
			}
			if (getUnitCombatType() != NO_UNITCOMBAT)
			{
				iExtraModifier = -pAttacker->unitCombatModifier(getUnitCombatType());
				iTempModifier += iExtraModifier;
				if (pCombatDetails != NULL)
				{
					pCombatDetails->iCombatModifierT = iExtraModifier;
				}
			}
#endif

			iExtraModifier = domainModifier(pAttacker->getDomainType());
			iTempModifier += iExtraModifier;
			if (pCombatDetails != NULL)
			{
				pCombatDetails->iDomainModifierA = iExtraModifier;
			}

			iExtraModifier = -pAttacker->domainModifier(getDomainType());
			iTempModifier += iExtraModifier;
			if (pCombatDetails != NULL)
			{
				pCombatDetails->iDomainModifierT = iExtraModifier;
			}
		}

		if (!(pAttacker->isRiver()))
		{
			if (pAttacker->plot()->isRiverCrossing(directionXY(pAttacker->plot(), pAttackedPlot)))
			{
				iExtraModifier = -GC.getRIVER_ATTACK_MODIFIER();
				iTempModifier += iExtraModifier;
				if (pCombatDetails != NULL)
				{
					pCombatDetails->iRiverAttackModifier = iExtraModifier;
				}
			}
		}

		if (!(pAttacker->isAmphib()))
		{
			if (!(pAttackedPlot->isWater()) && pAttacker->plot()->isWater())
			{
				iExtraModifier = -GC.getAMPHIB_ATTACK_MODIFIER();
				iTempModifier += iExtraModifier;
				if (pCombatDetails != NULL)
				{
					pCombatDetails->iAmphibAttackModifier = iExtraModifier;
				}
			}
		}

		// if we are attacking an unknown defender, then use the reverse of the modifier
		if (bAttackingUnknownDefender)
		{
			iModifier -= iTempModifier;
		}
		else
		{
			iModifier += iTempModifier;
		}
	}

	if (pCombatDetails != NULL)
	{
		pCombatDetails->iModifierTotal = iModifier;
		pCombatDetails->iBaseCombatStr = baseCombatStr();
	}

	if (iModifier > 0)
	{
		iCombat = (baseCombatStr() * (iModifier + 100));
	}
	else
	{
		iCombat = ((baseCombatStr() * 10000) / (100 - iModifier));
	}

	if (pCombatDetails != NULL)
	{
		pCombatDetails->iCombat = iCombat;
		pCombatDetails->iMaxCombatStr = std::max(1, iCombat);
		pCombatDetails->iCurrHitPoints = currHitPoints();
		pCombatDetails->iMaxHitPoints = maxHitPoints();
		pCombatDetails->iCurrCombatStr = ((pCombatDetails->iMaxCombatStr * pCombatDetails->iCurrHitPoints) / pCombatDetails->iMaxHitPoints);
	}

	return std::max(1, iCombat);
}


int CvUnit::currCombatStr(const CvPlot* pPlot, const CvUnit* pAttacker, CombatDetails* pCombatDetails) const
{
	return ((maxCombatStr(pPlot, pAttacker, pCombatDetails) * currHitPoints()) / maxHitPoints());
}


int CvUnit::currFirepower(const CvPlot* pPlot, const CvUnit* pAttacker) const
{
	return ((maxCombatStr(pPlot, pAttacker) + currCombatStr(pPlot, pAttacker) + 1) / 2);
}

// this nomalizes str by firepower, useful for quick odds calcs
// the effect is that a damaged unit will have an effective str lowered by firepower/maxFirepower
// doing the algebra, this means we mulitply by 1/2(1 + currHP)/maxHP = (maxHP + currHP) / (2 * maxHP)
int CvUnit::currEffectiveStr(const CvPlot* pPlot, const CvUnit* pAttacker, CombatDetails* pCombatDetails) const
{
	int currStr = currCombatStr(pPlot, pAttacker, pCombatDetails);

	currStr *= (maxHitPoints() + currHitPoints());
	currStr /= (2 * maxHitPoints());

	return currStr;
}

float CvUnit::maxCombatStrFloat(const CvPlot* pPlot, const CvUnit* pAttacker) const
{
	return (((float)(maxCombatStr(pPlot, pAttacker))) / 100.0f);
}


float CvUnit::currCombatStrFloat(const CvPlot* pPlot, const CvUnit* pAttacker) const
{
	return (((float)(currCombatStr(pPlot, pAttacker))) / 100.0f);
}

bool CvUnit::isUnarmed() const
{
	if (baseCombatStr() == 0)
	{
		return true;
	}

	if (getUnarmedCount() > 0)
	{
		return true;
	}

	return false;
}

int CvUnit::getPower() const
{
	int iPower = m_pUnitInfo->getPowerValue();
	if (getProfession() != NO_PROFESSION)
	{
		iPower += GC.getProfessionInfo(getProfession()).getPowerValue();

		if (GET_PLAYER(getOwnerINLINE()).hasContentsYieldEquipmentAmount(getProfession())) // cache CvPlayer::getYieldEquipmentAmount - Nightinggale
		{
			for (int i = 0; i < NUM_YIELD_TYPES; ++i)
			{
				YieldTypes eYield = (YieldTypes) i;
				iPower += GC.getYieldInfo(eYield).getPowerValue() * GET_PLAYER(getOwnerINLINE()).getYieldEquipmentAmount(getProfession(), eYield);
			}
		}
	}

	YieldTypes eYield = getYield();
	if (eYield != NO_YIELD)
	{
		iPower += GC.getYieldInfo(eYield).getPowerValue() * getYieldStored();
	}

	return iPower;
}

int CvUnit::getAsset() const
{
	int iAsset = m_pUnitInfo->getAssetValue();
	if (getProfession() != NO_PROFESSION)
	{
		iAsset += GC.getProfessionInfo(getProfession()).getAssetValue();

		if (GET_PLAYER(getOwnerINLINE()).hasContentsYieldEquipmentAmount(getProfession())) // cache CvPlayer::getYieldEquipmentAmount - Nightinggale
		{
			for (int i = 0; i < NUM_YIELD_TYPES; ++i)
			{
				YieldTypes eYield = (YieldTypes) i;
				iAsset += GC.getYieldInfo(eYield).getAssetValue() * GET_PLAYER(getOwnerINLINE()).getYieldEquipmentAmount(getProfession(), eYield);
			}
		}
	}
	YieldTypes eYield = getYield();
	if (eYield != NO_YIELD)
	{
		iAsset += GC.getYieldInfo(eYield).getAssetValue() * getYieldStored();
	}
	return iAsset;
}

bool CvUnit::canFight() const
{
	return (baseCombatStr() > 0);
}


bool CvUnit::canAttack() const
{
	if (!canFight())
	{
		return false;
	}

	if (isOnlyDefensive())
	{
		return false;
	}

	if (isUnarmed())
	{
		return false;
	}

	return true;
}


bool CvUnit::canDefend(const CvPlot* pPlot) const
{
	if (pPlot == NULL)
	{
		pPlot = plot();
	}

	if (!canFight())
	{
		return false;
	}

	if (getCapturingPlayer() != NO_PLAYER)
	{
		return false;
	}

	if (!pPlot->isValidDomainForAction(*this))
	{
		return false;
	}

	return true;
}


bool CvUnit::canSiege(TeamTypes eTeam) const
{
	if (!canDefend())
	{
		return false;
	}

	if (!isEnemy(eTeam))
	{
		return false;
	}

	if (!isNeverInvisible())
	{
		return false;
	}
	///TKs Med
	if (isBarbarian())
	{
	    if (isOnlyDefensive())
	    {
	        return false;
	    }
	}
	///TKe

	return true;
}

bool CvUnit::isAutomated() const
{
	return getGroup()->isAutomated();
}

bool CvUnit::isWaiting() const
{
	return getGroup()->isWaiting();
}

bool CvUnit::isFortifyable() const
{
	if (!canFight())
	{
		return false;
	}

	if (noDefensiveBonus())
	{
		return false;
	}

	if (!isOnMap())
	{
		return false;
	}

	if (getDomainType() == DOMAIN_SEA)
	{
		return false;
	}

	//Tks Med
	if (isOnlyDefensive())
	{
		return false;
	}

	if (isUnarmed())
	{
		return false;
	}
	//tke

	return true;
}


int CvUnit::fortifyModifier() const
{
	if (!isFortifyable())
	{
		return 0;
	}

	return (getFortifyTurns() * GC.getFORTIFY_MODIFIER_PER_TURN());
}


int CvUnit::experienceNeeded() const
{
	// Use python to determine pillage amounts...
	int iExperienceNeeded;
	long lExperienceNeeded;

	lExperienceNeeded = 0;
	iExperienceNeeded = 0;

	CyArgsList argsList;
	argsList.add(getLevel());	// pass in the units level
	argsList.add(getOwner());	// pass in the units

	gDLL->getPythonIFace()->callFunction(PYGameModule, "getExperienceNeeded", argsList.makeFunctionArgs(),&lExperienceNeeded);

	iExperienceNeeded = (int)lExperienceNeeded;

	return iExperienceNeeded;
}


int CvUnit::attackXPValue() const
{
	return m_pUnitInfo->getXPValueAttack();
}

int CvUnit::defenseXPValue() const
{
	return m_pUnitInfo->getXPValueDefense();
}

int CvUnit::maxXPValue() const
{
	int iMaxValue;

	iMaxValue = MAX_INT;

	return iMaxValue;
}

///TK FS
int CvUnit::firstStrikes() const
{
	//return std::max(0, (m_pUnitInfo->getFirstStrikes() + getExtraFirstStrikes()));
	return getExtraFirstStrikes();
}


int CvUnit::chanceFirstStrikes() const
{
	//return std::max(0, (m_pUnitInfo->getChanceFirstStrikes() + getExtraChanceFirstStrikes()));
	return getExtraChanceFirstStrikes();
}


int CvUnit::maxFirstStrikes() const
{
	return (firstStrikes() + chanceFirstStrikes());
}

bool CvUnit::immuneToFirstStrikes() const
{
	//return (m_pUnitInfo->isFirstStrikeImmune() || (getImmuneToFirstStrikesCount() > 0));
	return (getImmuneToFirstStrikesCount() > 0);
}


int CvUnit::getCombatFirstStrikes() const
{
	return m_iCombatFirstStrikes;
}

int CvUnit::getTrainCounter() const
{
	return m_iTrainCounter;
}

void CvUnit::changeTrainCounter(int iChange)
{
    if (iChange > 0)
    {
        m_iTrainCounter += iChange;
    }
    else
	{
	    m_iTrainCounter = iChange;
	}
}

bool CvUnit::canTrainUnit() const
{
    if (!isOnMap())
    {
        return false;
    }
	if (isNative() || GET_PLAYER(getOwner()).isEurope())
	{
		return false;
	}
    bool bIsCity = plot()->isCity(true, getTeam());
    if (bIsCity && !m_pUnitInfo->isMechUnit() && getTrainCounter() >= 0)
    {
        ProfessionTypes eProfession = getProfession();
        if (eProfession != NO_PROFESSION)
        {
            if (!isHasPromotion((PromotionTypes)GC.getXMLval(XML_DEFAULT_TRAINED_PROMOTION)) && GC.getProfessionInfo(eProfession).getCombatChange() >= GC.getXMLval(XML_DEFAULT_COMBAT_FOR_TRAINING))
            {
                return true;
            }
        }
    }

    return false;
}

int CvUnit::getTraderCode() const
{
	return m_iTraderCode;
}
///TKs Trader Codes = 1 Randsom Knight, 2 = Great General Spent Lead ability, 3 = Peasant Ambushed
void CvUnit::changeTraderCode(int iChange)
{
	//if (iChange > 0)
   // {
   //     m_iTraderCode += iChange;
   // }
   // else
	//{
	    m_iTraderCode = iChange;
	//}
}

void CvUnit::setCombatFirstStrikes(int iNewValue)
{
	m_iCombatFirstStrikes = iNewValue;
	FAssert(getCombatFirstStrikes() >= 0);
}

void CvUnit::changeCombatFirstStrikes(int iChange)
{
	setCombatFirstStrikes(getCombatFirstStrikes() + iChange);
}

int CvUnit::getImmuneToFirstStrikesCount() const
{
	return m_iImmuneToFirstStrikesCount;
}

void CvUnit::changeImmuneToFirstStrikesCount(int iChange)
{
	m_iImmuneToFirstStrikesCount += iChange;
	FAssert(getImmuneToFirstStrikesCount() >= 0);
}

int CvUnit::getExtraFirstStrikes() const
{
	return m_iExtraFirstStrikes;
}

void CvUnit::changeExtraFirstStrikes(int iChange)
{
	m_iExtraFirstStrikes += iChange;
	FAssert(getExtraFirstStrikes() >= 0);
}

int CvUnit::getExtraChanceFirstStrikes() const
{
	return m_iExtraChanceFirstStrikes;
}

void CvUnit::changeExtraChanceFirstStrikes(int iChange)
{
	m_iExtraChanceFirstStrikes += iChange;
	FAssert(getExtraChanceFirstStrikes() >= 0);
}

void CvUnit::setUpCombatBlockParrys(CvUnit* Defender)
{
    if (getDomainType() == DOMAIN_SEA || getProfession() == NO_PROFESSION)
    {
        return;
    }
    setCombatBlockParrys(0);


    if (Defender != NULL && Defender->getProfession() != NO_PROFESSION)
    {
        Defender->setCombatBlockParrys(0);
        int pAttackerBlockParrys = 0;
        if (getProfession() != NO_PROFESSION)
        {
            if (GC.getProfessionInfo(getProfession()).getCombatGearTypes(GC.getXMLval(XML_UNITTACTIC_PARRY)))
            {
                pAttackerBlockParrys++;
            }
            if (GC.getProfessionInfo(getProfession()).getCombatGearTypes(GC.getXMLval(XML_UNITARMOR_SHIELD)))
            {
                pAttackerBlockParrys++;
            }
        }
        int pDefenderBlockParrys = 0;
        if (Defender->getProfession() != NO_PROFESSION)
        {
            if (GC.getProfessionInfo(Defender->getProfession()).getCombatGearTypes(GC.getXMLval(XML_UNITTACTIC_PARRY)))
            {
                pDefenderBlockParrys++;
            }
            if (GC.getProfessionInfo(Defender->getProfession()).getCombatGearTypes(GC.getXMLval(XML_UNITARMOR_SHIELD)))
            {
                pDefenderBlockParrys++;
            }
        }

        int iTotal = pAttackerBlockParrys - pDefenderBlockParrys;
        if (iTotal > 0)
        {
            setCombatBlockParrys(iTotal);
        }
        else if (iTotal < 0)
        {
            iTotal *= -1;
            Defender->setCombatBlockParrys(iTotal);
        }
    }
}

void CvUnit::setCombatBlockParrys(int iNewValue)
{
	m_iCombatBlockParrys = iNewValue;
}

int CvUnit::getCombatBlockParrys()
{
    return m_iCombatBlockParrys;
}

bool CvUnit::setCombatAttackBlows(CvUnit* Defender)
{
    if (getDomainType() == DOMAIN_SEA || getProfession() == NO_PROFESSION)
    {
        return false;
    }
    setCombatCrushingBlow(false);
    setCombatGlancingBlow(false);

    if (Defender != NULL && Defender->getProfession() != NO_PROFESSION)
    {
        Defender->setCombatCrushingBlow(false);
        Defender->setCombatGlancingBlow(false);
        if (getProfession() != NO_PROFESSION)
        {
            if (GC.getProfessionInfo(Defender->getProfession()).getCombatGearTypes(GC.getXMLval(XML_UNITARMOR_PLATE)))
            {
                 if (GC.getProfessionInfo(getProfession()).getCombatGearTypes(GC.getXMLval(XML_UNITWEAPON_BLUNT)))
                 {
                     setCombatCrushingBlow(true);
                 }
                 else
                 {
                     setCombatGlancingBlow(true);
                 }
            }

        }

        if (Defender->getProfession() != NO_PROFESSION)
        {
            if (GC.getProfessionInfo(getProfession()).getCombatGearTypes(GC.getXMLval(XML_UNITARMOR_PLATE)))
            {
                 if (GC.getProfessionInfo(Defender->getProfession()).getCombatGearTypes(GC.getXMLval(XML_UNITWEAPON_BLUNT)))
                 {
                     Defender->setCombatCrushingBlow(true);
                 }
                 else
                 {
                     Defender->setCombatGlancingBlow(true);
                 }
            }

        }

        return true;
    }

	return false;

}

void CvUnit::setCombatCrushingBlow(bool iNewValue)
{
	m_bCrushingBlows = iNewValue;
}

void CvUnit::setCombatGlancingBlow(bool iNewValue)
{
	m_bGlancingBlows = iNewValue;
}

bool CvUnit::isCombatCrushingBlow()
{
    return m_bCrushingBlows;
}

void CvUnit::setAddedFreeBuilding(bool iNewValue)
{
	m_bFreeBuilding = iNewValue;
}

bool CvUnit::isAddedFreeBuilding()
{
    return m_bFreeBuilding;
}

bool CvUnit::isCombatGlancingBlow()
{
    return m_bGlancingBlows;
}
///TKe

bool CvUnit::isRanged() const
{
	CvUnitInfo * pkUnitInfo = &getUnitInfo();
	for (int i = 0; i < pkUnitInfo->getGroupDefinitions(getProfession()); i++ )
	{
		if ( !getArtInfo(i)->getActAsRanged() )
		{
			return false;
		}
	}
	return true;
}

bool CvUnit::alwaysInvisible() const
{
	if (!isOnMap())
	{
		return true;
	}

	return m_pUnitInfo->isInvisible();
}

bool CvUnit::noDefensiveBonus() const
{
	ProfessionTypes eProfession = getProfession();
	if (eProfession != NO_PROFESSION && GC.getProfessionInfo(eProfession).isNoDefensiveBonus())
	{
		return true;
	}

	if (m_pUnitInfo->isNoDefensiveBonus())
	{
		return true;
	}

	return false;
}

bool CvUnit::canMoveImpassable() const
{
	return m_pUnitInfo->isCanMoveImpassable();
}

bool CvUnit::flatMovementCost() const
{
	return m_pUnitInfo->isFlatMovementCost();
}


bool CvUnit::ignoreTerrainCost() const
{
	return m_pUnitInfo->isIgnoreTerrainCost();
}


bool CvUnit::isNeverInvisible() const
{
	return (!alwaysInvisible() && (getInvisibleType() == NO_INVISIBLE));
}


bool CvUnit::isInvisible(TeamTypes eTeam, bool bDebug, bool bCheckCargo) const
{
	if (!isOnMap())
	{
		return true;
	}

	if (bDebug && GC.getGameINLINE().isDebugMode())
	{
		return false;
	}

	if (getTeam() == eTeam)
	{
		return false;
	}

	if (alwaysInvisible())
	{
		return true;
	}

	if (bCheckCargo && isCargo())
	{
		return true;
	}

	if (getInvisibleType() == NO_INVISIBLE)
	{
		return false;
	}

	return !(plot()->isInvisibleVisible(eTeam, getInvisibleType()));
}


int CvUnit::withdrawalProbability() const
{
	return std::max(0, (m_pUnitInfo->getWithdrawalProbability() + getExtraWithdrawal()));
}

int CvUnit::getEvasionProbability(const CvUnit& kAttacker) const
{
	CvCity* pEvasionCity = getEvasionCity();
	if (pEvasionCity == NULL)
	{
		return 0;
	}

	return 100 * maxMoves() / std::max(1, maxMoves() + kAttacker.maxMoves());
}
///TK Med 1.4c
CvCity* CvUnit::getEvasionCity(int iWaylayed) const
{
	if (!isOnMap())
	{
		return NULL;
	}

	CvCity* pBestCity = NULL;
	int iBestDistance = MAX_INT;
	bool bRansomingKnight = (!GET_PLAYER(getOwner()).isEurope() && (m_pUnitInfo->getKnightDubbingWeight() == -1 || isHasRealPromotion((PromotionTypes)GC.getXMLval(XML_DEFAULT_KNIGHT_PROMOTION))));
	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iPlayer);
		if (kPlayer.isAlive() && kPlayer.getTeam() == getTeam())
		{
			int iLoop;
			for (CvCity* pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				if (pLoopCity->getArea() == getArea() || pLoopCity->plot()->isAdjacentToArea(getArea()) || bRansomingKnight || iWaylayed >= 0)
				{
					if (pLoopCity->plot()->isFriendlyCity(*this, false))
					{
					    if (bRansomingKnight || iWaylayed >= 0)
                        {
                            int iDistance = ::plotDistance(getX_INLINE(), getY_INLINE(), pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE());
                            if (iDistance < iBestDistance)
                            {
                                iBestDistance = iDistance;
                                pBestCity = pLoopCity;
                            }
                        }
                        else
                        {
                            for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); ++iBuildingClass)
                            {
                                if (m_pUnitInfo->isEvasionBuilding(iBuildingClass))
                                {
                                    if (pLoopCity->isHasBuildingClass((BuildingClassTypes) iBuildingClass))
                                    {
                                        int iDistance = ::plotDistance(getX_INLINE(), getY_INLINE(), pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE());
                                        if (iDistance < iBestDistance)
                                        {
                                            iBestDistance = iDistance;
                                            pBestCity = pLoopCity;
                                        }
                                    }
                                }
                            }
                        }
					}
				}
			}
		}
	}

	return pBestCity;
}
///TK end
int CvUnit::cityAttackModifier() const
{
	return (m_pUnitInfo->getCityAttackModifier() + getExtraCityAttackPercent());
}


int CvUnit::cityDefenseModifier() const
{
	return (m_pUnitInfo->getCityDefenseModifier() + getExtraCityDefensePercent());
}

int CvUnit::hillsAttackModifier() const
{
	return (m_pUnitInfo->getHillsAttackModifier() + getExtraHillsAttackPercent());
}


int CvUnit::hillsDefenseModifier() const
{
	return (m_pUnitInfo->getHillsDefenseModifier() + getExtraHillsDefensePercent());
}


int CvUnit::terrainAttackModifier(TerrainTypes eTerrain) const
{
	FAssertMsg(eTerrain >= 0, "eTerrain is expected to be non-negative (invalid Index)");
	FAssertMsg(eTerrain < GC.getNumTerrainInfos(), "eTerrain is expected to be within maximum bounds (invalid Index)");
	return (m_pUnitInfo->getTerrainAttackModifier(eTerrain) + getExtraTerrainAttackPercent(eTerrain));
}


int CvUnit::terrainDefenseModifier(TerrainTypes eTerrain) const
{
	FAssertMsg(eTerrain >= 0, "eTerrain is expected to be non-negative (invalid Index)");
	FAssertMsg(eTerrain < GC.getNumTerrainInfos(), "eTerrain is expected to be within maximum bounds (invalid Index)");
	return (m_pUnitInfo->getTerrainDefenseModifier(eTerrain) + getExtraTerrainDefensePercent(eTerrain));
}


int CvUnit::featureAttackModifier(FeatureTypes eFeature) const
{
	FAssertMsg(eFeature >= 0, "eFeature is expected to be non-negative (invalid Index)");
	FAssertMsg(eFeature < GC.getNumFeatureInfos(), "eFeature is expected to be within maximum bounds (invalid Index)");
	return (m_pUnitInfo->getFeatureAttackModifier(eFeature) + getExtraFeatureAttackPercent(eFeature));
}

int CvUnit::featureDefenseModifier(FeatureTypes eFeature) const
{
	FAssertMsg(eFeature >= 0, "eFeature is expected to be non-negative (invalid Index)");
	FAssertMsg(eFeature < GC.getNumFeatureInfos(), "eFeature is expected to be within maximum bounds (invalid Index)");
	return (m_pUnitInfo->getFeatureDefenseModifier(eFeature) + getExtraFeatureDefensePercent(eFeature));
}

int CvUnit::unitClassAttackModifier(UnitClassTypes eUnitClass) const
{
	FAssertMsg(eUnitClass >= 0, "eUnitClass is expected to be non-negative (invalid Index)");
	FAssertMsg(eUnitClass < GC.getNumUnitClassInfos(), "eUnitClass is expected to be within maximum bounds (invalid Index)");
	return m_pUnitInfo->getUnitClassAttackModifier(eUnitClass) + getExtraUnitClassAttackModifier(eUnitClass);
}


int CvUnit::unitClassDefenseModifier(UnitClassTypes eUnitClass) const
{
	FAssertMsg(eUnitClass >= 0, "eUnitClass is expected to be non-negative (invalid Index)");
	FAssertMsg(eUnitClass < GC.getNumUnitClassInfos(), "eUnitClass is expected to be within maximum bounds (invalid Index)");
	return m_pUnitInfo->getUnitClassDefenseModifier(eUnitClass) + getExtraUnitClassDefenseModifier(eUnitClass);
}


int CvUnit::unitCombatModifier(UnitCombatTypes eUnitCombat) const
{
	FAssertMsg(eUnitCombat >= 0, "eUnitCombat is expected to be non-negative (invalid Index)");
	FAssertMsg(eUnitCombat < GC.getNumUnitCombatInfos(), "eUnitCombat is expected to be within maximum bounds (invalid Index)");
	return (m_pUnitInfo->getUnitCombatModifier(eUnitCombat) + getExtraUnitCombatModifier(eUnitCombat));
}


int CvUnit::domainModifier(DomainTypes eDomain) const
{
	FAssertMsg(eDomain >= 0, "eDomain is expected to be non-negative (invalid Index)");
	FAssertMsg(eDomain < NUM_DOMAIN_TYPES, "eDomain is expected to be within maximum bounds (invalid Index)");
	return (m_pUnitInfo->getDomainModifier(eDomain) + getExtraDomainModifier(eDomain));
}

int CvUnit::rebelModifier(PlayerTypes eOtherPlayer) const
{
	if (GET_PLAYER(getOwnerINLINE()).getParent() != eOtherPlayer)
	{
		return 0;
	}

	int iModifier = std::max(0, GET_TEAM(getTeam()).getRebelPercent() - GC.getXMLval(XML_REBEL_PERCENT_FOR_REVOLUTION));

	iModifier *= GET_PLAYER(getOwnerINLINE()).getRebelCombatPercent();
	iModifier /= 100;

	if (!isHuman())
	{
		iModifier += GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIKingCombatModifier();
	}

	return iModifier;
}


int CvUnit::bombardRate() const
{
	return (m_pUnitInfo->getBombardRate() + getExtraBombardRate());
}


SpecialUnitTypes CvUnit::specialCargo() const
{
	return ((SpecialUnitTypes)(m_pUnitInfo->getSpecialCargo()));
}


DomainTypes CvUnit::domainCargo() const
{
	return ((DomainTypes)(m_pUnitInfo->getDomainCargo()));
}


int CvUnit::cargoSpace() const
{
	return m_iCargoCapacity;
}

void CvUnit::changeCargoSpace(int iChange)
{
	if (iChange != 0)
	{
		m_iCargoCapacity += iChange;
		FAssert(m_iCargoCapacity >= 0);
		setInfoBarDirty(true);
	}
}

bool CvUnit::isFull() const
{
	return (getCargo() >= cargoSpace());
}


int CvUnit::cargoSpaceAvailable(SpecialUnitTypes eSpecialCargo, DomainTypes eDomainCargo) const
{
	if (specialCargo() != NO_SPECIALUNIT)
	{
		if (specialCargo() != eSpecialCargo)
		{
			return 0;
		}
	}

	if (domainCargo() != NO_DOMAIN)
	{
		if (domainCargo() != eDomainCargo)
		{
			return 0;
		}
	}

	return std::max(0, (cargoSpace() - getCargo()));
}


bool CvUnit::hasCargo() const
{
	return (getCargo() > 0);
}


bool CvUnit::canCargoAllMove() const
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvPlot* pPlot;

	pPlot = plot();

	pUnitNode = pPlot->headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if (pLoopUnit->getTransportUnit() == this)
		{
			if (pLoopUnit->getDomainType() == DOMAIN_LAND)
			{
				if (!(pLoopUnit->canMove()))
				{
					return false;
				}
			}
		}
	}

	return true;
}

bool CvUnit::canCargoEnterArea(PlayerTypes ePlayer, const CvArea* pArea, bool bIgnoreRightOfPassage) const
{
	CvPlot* pPlot = plot();

	CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if (pLoopUnit->getTransportUnit() == this)
		{
			if (!pLoopUnit->canEnterArea(ePlayer, pArea, bIgnoreRightOfPassage))
			{
				return false;
			}
		}
	}

	return true;
}

int CvUnit::getUnitAICargo(UnitAITypes eUnitAI) const
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvPlot* pPlot;
	int iCount;

	iCount = 0;

	pPlot = plot();

	pUnitNode = pPlot->headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if (pLoopUnit->getTransportUnit() == this)
		{
			if (pLoopUnit->AI_getUnitAIType() == eUnitAI)
			{
				iCount++;
			}
		}
	}

	return iCount;
}

bool CvUnit::canAssignTradeRoute(int iRouteID, bool bReusePath) const
{
	PROFILE_FUNC();

	if (cargoSpace() < 1 || GET_PLAYER(getOwnerINLINE()).getNumTradeRoutes() < 1)
	{
		return false;
	}

	CvSelectionGroup* pGroup = getGroup();
	if (pGroup == NULL)
	{
		return false;
	}

	if (iRouteID == -1)
	{
		return true;
	}

	CLinkList<IDInfo> listCargo;
	pGroup->buildCargoUnitList(listCargo);
	CLLNode<IDInfo>* pUnitNode = listCargo.head();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = listCargo.next(pUnitNode);

		if (pLoopUnit->getYield() == NO_YIELD)
		{
			return false;
		}
	}

	PlayerTypes ePlayer = getOwnerINLINE();
	FAssert(ePlayer != NO_PLAYER);
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	CvTradeRoute* pTradeRoute = kPlayer.getTradeRoute(iRouteID);
	if (pTradeRoute == NULL)
	{
		return false;
	}

	if (pTradeRoute->getYield() == NO_YIELD)
	{
		return false;
	}

	if (pTradeRoute->getDestinationCity() == IDInfo(ePlayer, CvTradeRoute::EUROPE_CITY_ID))
	{
		if (getDomainType() != DOMAIN_SEA)
		{
			return false;
		}

		if (!kPlayer.isYieldEuropeTradable(pTradeRoute->getYield()))
		{
			return false;
		}
	}

	CvCity* pSource = ::getCity(pTradeRoute->getSourceCity());
	if (pSource == NULL || !generatePath(pSource->plot(), 0, bReusePath))
	{
		return false;
	}

	CvCity* pDestination = ::getCity(pTradeRoute->getDestinationCity());
	if (pDestination != NULL && !generatePath(pDestination->plot(), 0, bReusePath))
	{
		return false;
	}

	return true;
}


int CvUnit::getID() const
{
	return m_iID;
}


int CvUnit::getIndex() const
{
	return (getID() & FLTA_INDEX_MASK);
}


IDInfo CvUnit::getIDInfo() const
{
	IDInfo unit(getOwnerINLINE(), getID());
	return unit;
}


void CvUnit::setID(int iID)
{
	m_iID = iID;
}


int CvUnit::getGroupID() const
{
	return m_iGroupID;
}


bool CvUnit::isInGroup() const
{
	return(getGroupID() != FFreeList::INVALID_INDEX);
}


bool CvUnit::isGroupHead() const // XXX is this used???
{
	return (getGroup()->getHeadUnit() == this);
}


CvSelectionGroup* CvUnit::getGroup() const
{
	return GET_PLAYER(getOwnerINLINE()).getSelectionGroup(getGroupID());
}


bool CvUnit::canJoinGroup(const CvPlot* pPlot, CvSelectionGroup* pSelectionGroup) const
{
	CvUnit* pHeadUnit;

	// do not allow someone to join a group that is about to be split apart
	// this prevents a case of a never-ending turn
	if (pSelectionGroup->AI_isForceSeparate())
	{
		return false;
	}

	if (pSelectionGroup->getOwnerINLINE() == NO_PLAYER)
	{
		pHeadUnit = pSelectionGroup->getHeadUnit();

		if (pHeadUnit != NULL)
		{
			if (pHeadUnit->getOwnerINLINE() != getOwnerINLINE())
			{
				return false;
			}
		}
	}
	else
	{
		if (pSelectionGroup->getOwnerINLINE() != getOwnerINLINE())
		{
			return false;
		}
	}

	if (pSelectionGroup->getNumUnits() > 0)
	{
		if (!(pSelectionGroup->atPlot(pPlot)))
		{
			return false;
		}

		if (pSelectionGroup->getDomainType() != getDomainType())
		{
			return false;
		}
	}

	return true;
}


void CvUnit::joinGroup(CvSelectionGroup* pSelectionGroup, bool bRemoveSelected, bool bRejoin)
{
	CvSelectionGroup* pOldSelectionGroup;
	CvSelectionGroup* pNewSelectionGroup;
	CvPlot* pPlot;

	pOldSelectionGroup = GET_PLAYER(getOwnerINLINE()).getSelectionGroup(getGroupID());

	if ((pSelectionGroup != pOldSelectionGroup) || (pOldSelectionGroup == NULL))
	{
		pPlot = plot();

		if (pSelectionGroup != NULL)
		{
			pNewSelectionGroup = pSelectionGroup;
		}
		else
		{
			if (bRejoin)
			{
				pNewSelectionGroup = GET_PLAYER(getOwnerINLINE()).addSelectionGroup();
				pNewSelectionGroup->init(pNewSelectionGroup->getID(), getOwnerINLINE());
			}
			else
			{
				pNewSelectionGroup = NULL;
			}
		}

		if ((pNewSelectionGroup == NULL) || canJoinGroup(plot(), pNewSelectionGroup))
		{
			if (pOldSelectionGroup != NULL)
			{
				bool bWasHead = false;
				if (!isHuman())
				{
					if (pOldSelectionGroup->getNumUnits() > 1)
					{
						if (pOldSelectionGroup->getHeadUnit() == this)
						{
							bWasHead = true;
						}
					}
				}

				pOldSelectionGroup->removeUnit(this);

				// if we were the head, if the head unitAI changed, then force the group to separate (non-humans)
				if (bWasHead)
				{
					FAssert(pOldSelectionGroup->getHeadUnit() != NULL);
					if (pOldSelectionGroup->getHeadUnit()->AI_getUnitAIType() != AI_getUnitAIType())
					{
						pOldSelectionGroup->AI_makeForceSeparate();
					}
				}
			}

			if ((pNewSelectionGroup != NULL) && pNewSelectionGroup->addUnit(this, !isOnMap()))
			{
				m_iGroupID = pNewSelectionGroup->getID();
			}
			else
			{
				m_iGroupID = FFreeList::INVALID_INDEX;
			}

			if (getGroup() != NULL)
			{
				if (getGroup()->getNumUnits() > 1)
				{
					getGroup()->setActivityType(ACTIVITY_AWAKE);
				}
				else
				{
					GET_PLAYER(getOwnerINLINE()).updateGroupCycle(this);
				}
			}

			if (getTeam() == GC.getGameINLINE().getActiveTeam())
			{
				if (pPlot != NULL)
				{
					pPlot->setFlagDirty(true);
				}
			}

			if (pPlot == gDLL->getInterfaceIFace()->getSelectionPlot())
			{
				gDLL->getInterfaceIFace()->setDirty(PlotListButtons_DIRTY_BIT, true);
			}
		}

		if (bRemoveSelected)
		{
			if (IsSelected())
			{
				gDLL->getInterfaceIFace()->removeFromSelectionList(this);
			}
		}
	}
}


int CvUnit::getHotKeyNumber()
{
	return m_iHotKeyNumber;
}


void CvUnit::setHotKeyNumber(int iNewValue)
{
	CvUnit* pLoopUnit;
	int iLoop;

	FAssert(getOwnerINLINE() != NO_PLAYER);

	if (getHotKeyNumber() != iNewValue)
	{
		if (iNewValue != -1)
		{
			for(pLoopUnit = GET_PLAYER(getOwnerINLINE()).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(getOwnerINLINE()).nextUnit(&iLoop))
			{
				if (pLoopUnit->getHotKeyNumber() == iNewValue)
				{
					pLoopUnit->setHotKeyNumber(-1);
				}
			}
		}

		m_iHotKeyNumber = iNewValue;

		if (IsSelected())
		{
			gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
		}
	}
}


int CvUnit::getX() const
{
	return m_iX;
}


int CvUnit::getY() const
{
	return m_iY;
}


void CvUnit::setXY(int iX, int iY, bool bGroup, bool bUpdate, bool bShow, bool bCheckPlotVisible)
{
	CLLNode<IDInfo>* pUnitNode;
	CvCity* pOldCity;
	CvCity* pNewCity;
	CvCity* pWorkingCity;
	CvUnit* pTransportUnit;
	CvUnit* pLoopUnit;
	CvPlot* pOldPlot;
	CvPlot* pNewPlot;
	CvPlot* pLoopPlot;
	CLinkList<IDInfo> oldUnits;
	ActivityTypes eOldActivityType;
	int iI;

	// OOS!! Temporary for Out-of-Sync madness debugging...
	if (GC.getLogging())
	{
		if (gDLL->getChtLvl() > 0)
		{
			char szOut[1024];
			sprintf(szOut, "Player %d Unit %d (%S's %S) moving from %d:%d to %d:%d\n", getOwnerINLINE(), getID(), GET_PLAYER(getOwnerINLINE()).getNameKey(), getName().GetCString(), getX_INLINE(), getY_INLINE(), iX, iY);
			gDLL->messageControlLog(szOut);
		}
	}

	FAssert(!at(iX, iY) || (iX == INVALID_PLOT_COORD) || (iY == INVALID_PLOT_COORD));
	FAssert(!isFighting());
	FAssert((iX == INVALID_PLOT_COORD) || (GC.getMapINLINE().plotINLINE(iX, iY)->getX_INLINE() == iX));
	FAssert((iY == INVALID_PLOT_COORD) || (GC.getMapINLINE().plotINLINE(iX, iY)->getY_INLINE() == iY));

	if (getGroup() != NULL)
	{
		eOldActivityType = getGroup()->getActivityType();
	}
	else
	{
		eOldActivityType = NO_ACTIVITY;
	}

	if (!bGroup)
	{
		joinGroup(NULL, true);
	}

	pNewPlot = GC.getMapINLINE().plotINLINE(iX, iY);

	if (pNewPlot != NULL)
	{

		pTransportUnit = getTransportUnit();
        ///TKs Med TradeRoute Show Discover TradeRoute Video
        /*if (isHuman() && !GET_PLAYER(getOwner()).getHasTradeRouteType(TRADE_ROUTE_SPICE_ROUTE) && pTransportUnit == NULL)
        {
            if (pNewPlot->isEurope())
            {
                if (GC.getEuropeInfo(pNewPlot->getEurope()).getTradeScreensValid(TRADE_SCREEN_SPICE_ROUTE))
                {
                    CivicTypes eSpiceRoute = (CivicTypes)GC.getXMLval(XML_TRADE_ROUTE_SPICE);
                    CvPlayer& kPlayer = GET_PLAYER(getOwnerINLINE());
                    kPlayer.setHasTradeRouteType(TRADE_ROUTE_SPICE_ROUTE, true);
                    if (GC.getXMLval(XML_DIPLAY_NEW_VIDEOS) > 0)
                    {
                        if (!CvString(CvWString("ART_DEF_MOVIE_SPICE_ROUTE")).empty())
                        {
                            CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_MOVIE);
                            pInfo->setText(CvWString("ART_DEF_MOVIE_SPICE_ROUTE"));
                            gDLL->getInterfaceIFace()->addPopup(pInfo, GET_PLAYER(getOwner()).getID());
                        }
                    }
                    kPlayer.setStartingTradeRoutePlot(pNewPlot, TRADE_ROUTE_SPICE_ROUTE);
                }
            }

        }*/

		if (pTransportUnit != NULL)
		{
			if (!(pTransportUnit->atPlot(pNewPlot)))
			{
				setTransportUnit(NULL);
			}
			if (!isHuman())
            {
                if (isGoods())
                {
                    if (pNewPlot->isWater() && pTransportUnit->getDomainType() != DOMAIN_SEA)
                    {
                        FAssertMsg(false, "Goods transported over water by none Sea Vessel" );
                        kill(true);
                        return;
                    }
                }
            }
		}
        ///TKe
		if (canFight() && isOnMap())
		{
			oldUnits.clear();

			pUnitNode = pNewPlot->headUnitNode();

			while (pUnitNode != NULL)
			{
				oldUnits.insertAtEnd(pUnitNode->m_data);
				pUnitNode = pNewPlot->nextUnitNode(pUnitNode);
			}

			pUnitNode = oldUnits.head();

			while (pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = oldUnits.next(pUnitNode);

				if (pLoopUnit != NULL && pLoopUnit->isOnMap())
				{
					if (isEnemy(pLoopUnit->getTeam(), pNewPlot) || pLoopUnit->isEnemy(getTeam()))
					{
						if (!pLoopUnit->canCoexistWithEnemyUnit(getTeam()))
						{

							if (NO_UNITCLASS == pLoopUnit->getUnitInfo().getUnitCaptureClassType() && pLoopUnit->canDefend(pNewPlot))
							{
								pLoopUnit->jumpToNearestValidPlot(); // can kill unit
							}
							else
							{
							    ///TKs Med
							    if(canAttack())
							    {
//
                                    if (!m_pUnitInfo->isHiddenNationality() && !pLoopUnit->getUnitInfo().isHiddenNationality())
                                    {
                                        GET_TEAM(getTeam()).AI_changeWarSuccess(pLoopUnit->getTeam(), GC.getXMLval(XML_WAR_SUCCESS_UNIT_CAPTURING));
                                    }

                                    if (!isNoUnitCapture())
                                    {
                                        pLoopUnit->setCapturingPlayer(getOwnerINLINE());
                                    }

                                    pLoopUnit->kill(false, this);
							    }
								///TKe
							}
						}
					}
				}
			}
		}
	}
	pOldPlot = plot();
	///TK Med Marauders
	if (m_pUnitInfo->isCanMoveAllTerrain() && pOldPlot != NULL && pNewPlot != NULL && getDomainType() == DOMAIN_LAND)
	{
	    if (pNewPlot->isWater())
	    {
	        if (!GC.getProfessionInfo(getProfession()).isWater())
			{
			    //m_eProfession = (ProfessionTypes)GC.getDefineINT("DEFAULT_MARUADER_SEA_PROFESSION");
			    setProfession((ProfessionTypes)GC.getXMLval(XML_DEFAULT_MARUADER_SEA_PROFESSION), true);
			    //reloadEntity();
			}
	    }
	    else if (pOldPlot->isWater() && !pNewPlot->isWater())
	    {
	        if (GC.getProfessionInfo(getProfession()).isWater())
			{
			    //m_eProfession = (ProfessionTypes)GC.getUnitInfo(getUnitType()).getDefaultProfession();
			    //reloadEntity();
			    setProfession((ProfessionTypes)GC.getUnitInfo(getUnitType()).getDefaultProfession(), true);
			}
	    }
	}


	///TKe


	if (pOldPlot != NULL)
	{
		pOldPlot->removeUnit(this, bUpdate);

		pOldPlot->changeAdjacentSight(getTeam(), visibilityRange(), false, this, true);

		pOldPlot->area()->changeUnitsPerPlayer(getOwnerINLINE(), -1);
		pOldPlot->area()->changePower(getOwnerINLINE(), -getPower());

		if (AI_getUnitAIType() != NO_UNITAI)
		{
			pOldPlot->area()->changeNumAIUnits(getOwnerINLINE(), AI_getUnitAIType(), -1);
		}

		setLastMoveTurn(GC.getGameINLINE().getTurnSlice());

		pOldCity = pOldPlot->getPlotCity();

		pWorkingCity = pOldPlot->getWorkingCity();

		if (pWorkingCity != NULL)
		{
			if (canSiege(pWorkingCity->getTeam()))
			{
				pWorkingCity->AI_setAssignWorkDirty(true);
			}
		}

		if (pOldPlot->isWater())
		{
			for (iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				pLoopPlot = plotDirection(pOldPlot->getX_INLINE(), pOldPlot->getY_INLINE(), ((DirectionTypes)iI));

				if (pLoopPlot != NULL)
				{
					if (pLoopPlot->isWater())
					{
						pWorkingCity = pLoopPlot->getWorkingCity();

						if (pWorkingCity != NULL)
						{
							if (canSiege(pWorkingCity->getTeam()))
							{
								pWorkingCity->AI_setAssignWorkDirty(true);
							}
						}
					}
				}
			}
		}

		if (pOldPlot->isActiveVisible(true))
		{
			pOldPlot->updateMinimapColor();
		}

		if (pOldPlot == gDLL->getInterfaceIFace()->getSelectionPlot())
		{
			gDLL->getInterfaceIFace()->verifyPlotListColumn();

			gDLL->getInterfaceIFace()->setDirty(PlotListButtons_DIRTY_BIT, true);
		}
	}

	if (pNewPlot != NULL)
	{
	    ///TKs Med
	    if (gDLL->getChtLvl() > 0 && getYield() != NO_YIELD && gDLL->GetWorldBuilderMode())
		{
		    if (GC.getYieldInfo(getYield()).isCargo())
		    {
		        CvCity* pPlotCity = pNewPlot->getPlotCity();
		        if (pPlotCity != NULL)
		        {
		            pPlotCity->changeYieldStored(getYield(), 100);
		            kill(true);
		        }
		    }
		}


	    ///TKe

		m_iX = pNewPlot->getX_INLINE();
		m_iY = pNewPlot->getY_INLINE();
	}
	else
	{
		m_iX = INVALID_PLOT_COORD;
		m_iY = INVALID_PLOT_COORD;
		AI_setMovePriority(0);
	}

	FAssertMsg(plot() == pNewPlot, "plot is expected to equal pNewPlot");

	if (pNewPlot != NULL)
	{
		pNewCity = pNewPlot->getPlotCity();

		if (pNewCity != NULL)
		{
			if (isEnemy(pNewCity->getTeam()) && !canCoexistWithEnemyUnit(pNewCity->getTeam()) && canFight())
			{
				GET_TEAM(getTeam()).AI_changeWarSuccess(pNewCity->getTeam(), GC.getXMLval(XML_WAR_SUCCESS_CITY_CAPTURING));
				PlayerTypes eNewOwner = GET_PLAYER(getOwnerINLINE()).pickConqueredCityOwner(*pNewCity);

				if (NO_PLAYER != eNewOwner)
				{
					GET_PLAYER(eNewOwner).acquireCity(pNewCity, true, false); // will delete the pointer
					pNewCity = NULL;
				}
			}

		}

		//update facing direction
		if(pOldPlot != NULL)
		{
			DirectionTypes newDirection = estimateDirection(pOldPlot, pNewPlot);
			if(newDirection != NO_DIRECTION)
				m_eFacingDirection = newDirection;
		}

		//update cargo mission animations
		if (isCargo())
		{
			if (eOldActivityType != ACTIVITY_MISSION)
			{
				getGroup()->setActivityType(eOldActivityType);
			}
		}

		setFortifyTurns(0);

		pNewPlot->changeAdjacentSight(getTeam(), visibilityRange(), true, this, true); // needs to be here so that the square is considered visible when we move into it...

		pNewPlot->addUnit(this, bUpdate);

		pNewPlot->area()->changeUnitsPerPlayer(getOwnerINLINE(), 1);
		pNewPlot->area()->changePower(getOwnerINLINE(), getPower());

		if (AI_getUnitAIType() != NO_UNITAI)
		{
			pNewPlot->area()->changeNumAIUnits(getOwnerINLINE(), AI_getUnitAIType(), 1);
		}

        ///TKs Invention Core Mod v 1.0
//        if (getYield() != NO_YIELD)
//        {
//            if (GC.getYieldInfo(getYield()).isCargo())
//            {
//                if (shouldLoadOnMove(pNewPlot))
//                {
//                    load(false);
//                }
//                else if (getTransportUnit() == NULL)
//                {
//                    pNewCity->changeYieldStored(getYield(), getYieldStored());
//                    kill(true);
//                }
//            }
//        }
//			///TKe
		if (shouldLoadOnMove(pNewPlot))
		{
			load(false);
		}
         ///TK Med Autokill Bandits and Animals
        pWorkingCity = pNewPlot->getWorkingCity();

        if (isBarbarian() || isAlwaysHostile(pNewPlot))
        {
            if (pNewPlot->getImprovementType() != NO_IMPROVEMENT)
            {
                if (GC.getImprovementInfo(pNewPlot->getImprovementType()).isActsAsCity())
                {
                    bool bKill = true;
                    if (GC.getImprovementInfo(pNewPlot->getImprovementType()).getPatrolLevel() <= 1)
                    {
                        if (!m_pUnitInfo->isAnimal())
                        {
                            bKill = false;
                        }
                    }
                    if (bKill)
                    {
                        if (m_pUnitInfo->isAnimal() && pNewPlot->getOwner() != NO_PLAYER && GET_PLAYER(pNewPlot->getOwner()).canUseYield(YIELD_FROM_ANIMALS))
                        {
                            if (pWorkingCity != NULL)
                            {

                                int iYieldStored = GC.getXMLval(XML_CAPTURED_LUXURY_FOOD_RANDOM_AMOUNT);
                                iYieldStored = (GC.getGameINLINE().getSorenRandNum(iYieldStored, "Random City Kill") + 1);
                                int iCityYieldStore = pWorkingCity->getYieldStored(YIELD_FROM_ANIMALS) + iYieldStored;
                                pWorkingCity->setYieldStored(YIELD_FROM_ANIMALS, iCityYieldStore);
                                if (pWorkingCity->isHuman())
                                {
                                    PlayerTypes eCityOwner = pWorkingCity->getOwnerINLINE();
                                    CvWString szBuffer;
                                    szBuffer = gDLL->getText("TXT_KEY_CITY_CAPTURE_ANIMAL", GC.getImprovementInfo(pNewPlot->getImprovementType()).getTextKeyWide(), iYieldStored, GC.getYieldInfo(YIELD_FROM_ANIMALS).getChar(), pWorkingCity->getNameKey());
                                    gDLL->getInterfaceIFace()->addMessage(eCityOwner, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_UNITCAPTURE", MESSAGE_TYPE_INFO, getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pWorkingCity->getX_INLINE(), pWorkingCity->getY_INLINE());
                                }
                            }


                        }
                        if (pNewPlot->getOwner() != NO_PLAYER)
                        {
                            if (GET_PLAYER(pNewPlot->getOwner()).isHuman())
                            {
                                CvWString szBuffer;

                                if (m_pUnitInfo->isAnimal())
                                {
                                    szBuffer = gDLL->getText("TXT_KEY_MISC_ANIMAL_UNIT_DESTROYED_PATROL", getNameOrProfessionKey(), GC.getImprovementInfo(pNewPlot->getImprovementType()).getTextKeyWide());
                                }
                                else
                                {
                                    szBuffer = gDLL->getText("TXT_KEY_MISC_BANDIT_UNIT_DESTROYED_PATROL", getNameOrProfessionKey(), GC.getImprovementInfo(pNewPlot->getImprovementType()).getTextKeyWide());
                                }
                                gDLL->getInterfaceIFace()->addMessage(pNewPlot->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, GC.getEraInfo(GC.getGameINLINE().getCurrentEra()).getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pNewPlot->getX_INLINE(), pNewPlot->getY_INLINE());
                            }

                        }
                        kill(true);
                        return;
                    }

                }
            }

        }

		for (int iDX = -1; iDX <= 1; ++iDX)
		{
			for (int iDY = -1; iDY <= 1; ++iDY)
			{
				CvPlot* pLoopPlot = ::plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);
				if (pLoopPlot != NULL)
				{
				    if (isBarbarian() || isAlwaysHostile(pLoopPlot))
				    {
				        if (pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
                        {
                            if (GC.getImprovementInfo(pLoopPlot->getImprovementType()).isActsAsCity())
                            {
                                CvCity* pLoopCity = pLoopPlot->getWorkingCity();
                                bool bKill = true;
                                if (GC.getImprovementInfo(pLoopPlot->getImprovementType()).getPatrolLevel() <= 1)
                                {
                                    if (!m_pUnitInfo->isAnimal())
                                    {
                                        bKill = false;
                                    }
                                }
                                if (bKill)
                                {
                                    if (m_pUnitInfo->isAnimal() && pLoopCity != NULL)
                                    {
										PlayerTypes eCityOwner = pLoopCity->getOwnerINLINE();

										if (eCityOwner != NO_PLAYER && GET_PLAYER(eCityOwner).canUseYield(YIELD_FROM_ANIMALS))
                                        {

                                            int iYieldStored = GC.getXMLval(XML_CAPTURED_LUXURY_FOOD_RANDOM_AMOUNT);
                                            iYieldStored = (GC.getGameINLINE().getSorenRandNum(iYieldStored, "Random City Kill") + 1);
                                            int iCityYieldStore = pLoopCity->getYieldStored(YIELD_FROM_ANIMALS) + iYieldStored;
                                            pLoopCity->setYieldStored(YIELD_FROM_ANIMALS, iCityYieldStore);
                                            if (pLoopCity->isHuman())
                                            {
                                                //PlayerTypes eCityOwner = pLoopCity->getOwnerINLINE();
                                                CvWString szBuffer;
                                                szBuffer = gDLL->getText("TXT_KEY_CITY_CAPTURE_ANIMAL", GC.getImprovementInfo(pLoopPlot->getImprovementType()).getTextKeyWide(), iYieldStored, GC.getYieldInfo(YIELD_FROM_ANIMALS).getChar(), pLoopCity->getNameKey());
                                                gDLL->getInterfaceIFace()->addMessage(eCityOwner, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_UNITCAPTURE", MESSAGE_TYPE_INFO, getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE());
                                            }
                                        }


                                    }
                                    if (pLoopPlot->getOwner() != NO_PLAYER)
                                    {
                                        if (GET_PLAYER(pLoopPlot->getOwner()).isHuman())
                                        {
                                            CvWString szBuffer;
                                            if (m_pUnitInfo->isAnimal())
                                            {
                                                szBuffer = gDLL->getText("TXT_KEY_MISC_ANIMAL_UNIT_DESTROYED_PATROL", getNameOrProfessionKey(), GC.getImprovementInfo(pLoopPlot->getImprovementType()).getTextKeyWide());
                                            }
                                            else
                                            {
                                                szBuffer = gDLL->getText("TXT_KEY_MISC_BANDIT_UNIT_DESTROYED_PATROL", getNameOrProfessionKey(), GC.getImprovementInfo(pLoopPlot->getImprovementType()).getTextKeyWide());
                                            }
                                            gDLL->getInterfaceIFace()->addMessage(pLoopPlot->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, GC.getEraInfo(GC.getGameINLINE().getCurrentEra()).getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE());
                                        }

                                    }
                                    kill(true);
                                    return;
                                }
                            }
                        }

				    }
				    else
				    {
                        for (iI = 0; iI < MAX_TEAMS; iI++)
                        {
                            TeamTypes eLoopTeam = (TeamTypes) iI;
                            if (GET_TEAM(eLoopTeam).isAlive())
                            {
                                if (!isInvisible(eLoopTeam, false) && getVisualOwner(eLoopTeam) == getOwnerINLINE())
                                {
                                    if (pLoopPlot->plotCount(PUF_isVisualTeam, eLoopTeam, getTeam(), NO_PLAYER, eLoopTeam, PUF_isVisible, getOwnerINLINE(), -1) > 0)
                                    {
                                        GET_TEAM(eLoopTeam).meet(getTeam(), true);
                                    }
                                }
                            }
                        }

                        if (pLoopPlot->isOwned() && getVisualOwner(pLoopPlot->getTeam()) == getOwnerINLINE())
                        {
                            if (pLoopPlot->isCity() || !GET_PLAYER(pLoopPlot->getOwnerINLINE()).isAlwaysOpenBorders())
                            {
                                GET_TEAM(pLoopPlot->getTeam()).meet(getTeam(), true);
                            }
                        }
				    }
				}
			}
		}

		pNewCity = pNewPlot->getPlotCity();

		///TKend
		if (pWorkingCity != NULL)
		{
			if (canSiege(pWorkingCity->getTeam()))
			{
				pWorkingCity->verifyWorkingPlot(pWorkingCity->getCityPlotIndex(pNewPlot));
			}
		}

		if (pNewPlot->isWater())
		{
			for (iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				pLoopPlot = plotDirection(pNewPlot->getX_INLINE(), pNewPlot->getY_INLINE(), ((DirectionTypes)iI));

				if (pLoopPlot != NULL)
				{
					if (pLoopPlot->isWater())
					{
						pWorkingCity = pLoopPlot->getWorkingCity();

						if (pWorkingCity != NULL)
						{
							if (canSiege(pWorkingCity->getTeam()))
							{
								pWorkingCity->verifyWorkingPlot(pWorkingCity->getCityPlotIndex(pLoopPlot));
							}
						}
					}
				}
			}
		}

		if (pNewPlot->isActiveVisible(true))
		{
			pNewPlot->updateMinimapColor();
		}

		if (GC.IsGraphicsInitialized())
		{
			//override bShow if check plot visible
			if (bCheckPlotVisible)
			{
				if (!pNewPlot->isActiveVisible(true) && ((pOldPlot == NULL) || !pOldPlot->isActiveVisible(true)))
				{
					bShow = false;
				}
			}

			if (bShow)
			{
				QueueMove(pNewPlot);
			}
			else
			{
				SetPosition(pNewPlot);
			}
		}

		if (pNewPlot == gDLL->getInterfaceIFace()->getSelectionPlot())
		{
			gDLL->getInterfaceIFace()->verifyPlotListColumn();

			gDLL->getInterfaceIFace()->setDirty(PlotListButtons_DIRTY_BIT, true);
		}
	}

	if (pOldPlot != NULL)
	{
		if (hasCargo())
		{
			pUnitNode = pOldPlot->headUnitNode();

			while (pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = pOldPlot->nextUnitNode(pUnitNode);

				if (pLoopUnit->getTransportUnit() == this)
				{
					pLoopUnit->setXY(iX, iY, bGroup, bUpdate);
					if (pLoopUnit->getYield() != NO_YIELD)
					{
						pNewPlot->addCrumbs(10);
					}
				}
			}
		}
	}

	FAssert(pOldPlot != pNewPlot || pNewPlot == NULL);
	GET_PLAYER(getOwnerINLINE()).updateGroupCycle(this);

	setInfoBarDirty(true);

	if (IsSelected())
	{
		gDLL->getInterfaceIFace()->setDirty(ColoredPlots_DIRTY_BIT, true);
	}

	//update glow
	gDLL->getEntityIFace()->updateEnemyGlow(getUnitEntity());

	// report event to Python, along with some other key state
	gDLL->getEventReporterIFace()->unitSetXY(pNewPlot, this);
    ///Tks Med
	if (pNewPlot != NULL)
	{
		if (pNewPlot->isGoody(getTeam()) && (getProfession() != NO_PROFESSION || !isHuman()))
		{
			for (int i = 0; i < GC.getNumFatherPointInfos(); ++i)
			{
				FatherPointTypes ePointType = (FatherPointTypes) i;
				GET_PLAYER(getOwnerINLINE()).changeFatherPoints(ePointType, GC.getFatherPointInfo(ePointType).getGoodyPoints() * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getFatherPercent() / 100);
			}

			GET_PLAYER(getOwnerINLINE()).doGoody(pNewPlot, this);
		}
	}
	///TKe
}


bool CvUnit::at(int iX, int iY) const
{
	return((getX_INLINE() == iX) && (getY_INLINE() == iY));
}


bool CvUnit::atPlot(const CvPlot* pPlot) const
{
	return (plot() == pPlot);
}


CvPlot* CvUnit::plot() const
{
	if((getX_INLINE() == INVALID_PLOT_COORD) || (getY_INLINE() == INVALID_PLOT_COORD))
	{
		CvCity *pCity = GET_PLAYER(getOwnerINLINE()).getPopulationUnitCity(getID());
		if (pCity == NULL)
		{
			return NULL;
		}
		else
		{
			return pCity->plot();
		}
	}
	else
	{
		return GC.getMapINLINE().plotSorenINLINE(getX_INLINE(), getY_INLINE());
	}
}

CvCity* CvUnit::getCity() const
{
	CvPlot* pPlot = plot();
	if (pPlot != NULL)
	{
		return pPlot->getPlotCity();
	}
	return NULL;
}

int CvUnit::getArea() const
{
	CvPlot* pPlot = plot();
	if (pPlot == NULL)
	{
		return FFreeList::INVALID_INDEX;
	}

	return pPlot->getArea();
}


CvArea* CvUnit::area() const
{
	CvPlot* pPlot = plot();
	if (pPlot == NULL)
	{
		return NULL;
	}

	return pPlot->area();
}


int CvUnit::getLastMoveTurn() const
{
	return m_iLastMoveTurn;
}


void CvUnit::setLastMoveTurn(int iNewValue)
{
	m_iLastMoveTurn = iNewValue;
	FAssert(getLastMoveTurn() >= 0);
}


int CvUnit::getGameTurnCreated() const
{
	return m_iGameTurnCreated;
}


void CvUnit::setGameTurnCreated(int iNewValue)
{
	m_iGameTurnCreated = iNewValue;
	FAssert(getGameTurnCreated() >= 0);
}


int CvUnit::getDamage() const
{
	return m_iDamage;
}


void CvUnit::setDamage(int iNewValue, CvUnit* pAttacker, bool bNotifyEntity)
{
	int iOldValue;

	iOldValue = getDamage();

	m_iDamage = range(iNewValue, 0, maxHitPoints());

	FAssertMsg(currHitPoints() >= 0, "currHitPoints() is expected to be non-negative (invalid Index)");

	if ((iOldValue != getDamage()) && isOnMap())
	{
		if (GC.getGameINLINE().isFinalInitialized() && bNotifyEntity)
		{
			NotifyEntity(MISSION_DAMAGE);
		}

		setInfoBarDirty(true);

		if (IsSelected())
		{
			gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
		}

		if (plot() == gDLL->getInterfaceIFace()->getSelectionPlot())
		{
			gDLL->getInterfaceIFace()->setDirty(PlotListButtons_DIRTY_BIT, true);
		}
	}

	if (isDead())
	{
		kill(true, pAttacker);
	}
}

void CvUnit::changeDamage(int iChange, CvUnit* pAttacker)
{
	setDamage((getDamage() + iChange), pAttacker);
}


int CvUnit::getMoves() const
{
	return m_iMoves;
}


void CvUnit::setMoves(int iNewValue)
{
	CvPlot* pPlot;

	if (getMoves() != iNewValue)
	{
		pPlot = plot();

		m_iMoves = iNewValue;

		FAssert(getMoves() >= 0);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			if (pPlot != NULL)
			{
				pPlot->setFlagDirty(true);
			}
		}

		if (IsSelected())
		{
			gDLL->getFAStarIFace()->ForceReset(&GC.getInterfacePathFinder());

			gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
		}

		if (pPlot == gDLL->getInterfaceIFace()->getSelectionPlot())
		{
			gDLL->getInterfaceIFace()->setDirty(PlotListButtons_DIRTY_BIT, true);
		}
	}
}


void CvUnit::changeMoves(int iChange)
{
	setMoves(getMoves() + iChange);
}


void CvUnit::finishMoves()
{
	setMoves(maxMoves());
}


int CvUnit::getExperience() const
{
	return m_iExperience;
}


void CvUnit::setExperience(int iNewValue, int iMax)
{
	if ((getExperience() != iNewValue) && (getExperience() < ((iMax == -1) ? MAX_INT : iMax)))
	{
		m_iExperience = std::min(((iMax == -1) ? MAX_INT : iMax), iNewValue);
		FAssert(getExperience() >= 0);

		if (IsSelected())
		{
			gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
		}
	}
}


void CvUnit::changeExperience(int iChange, int iMax, bool bFromCombat, bool bInBorders, bool bUpdateGlobal)
{
	int iUnitExperience = iChange;

	if (bFromCombat)
	{
		CvPlayer& kPlayer = GET_PLAYER(getOwnerINLINE());

		int iCombatExperienceMod = 100 + kPlayer.getGreatGeneralRateModifier();

		if (bInBorders)
		{
			iCombatExperienceMod += kPlayer.getDomesticGreatGeneralRateModifier() + kPlayer.getExpInBorderModifier();
			iUnitExperience += (iChange * kPlayer.getExpInBorderModifier()) / 100;
		}

		if (bUpdateGlobal)
		{
			kPlayer.changeCombatExperience((iChange * iCombatExperienceMod) / 100);
		}

		if (getExperiencePercent() != 0)
		{
			iUnitExperience *= std::max(0, 100 + getExperiencePercent());
			iUnitExperience /= 100;
		}
	}

	setExperience((getExperience() + iUnitExperience), iMax);
}


int CvUnit::getLevel() const
{
	return m_iLevel;
}


void CvUnit::setLevel(int iNewValue)
{
	if (getLevel() != iNewValue)
	{
		m_iLevel = iNewValue;
		FAssert(getLevel() >= 0);

		if (getLevel() > GET_PLAYER(getOwnerINLINE()).getHighestUnitLevel())
		{
			GET_PLAYER(getOwnerINLINE()).setHighestUnitLevel(getLevel());
		}

		if (IsSelected())
		{
			gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
		}
	}
}


void CvUnit::changeLevel(int iChange)
{
	setLevel(getLevel() + iChange);
}


int CvUnit::getCargo() const
{
	return m_iCargo;
}


void CvUnit::changeCargo(int iChange)
{
	m_iCargo += iChange;
	FAssert(getCargo() >= 0);
}


CvPlot* CvUnit::getAttackPlot() const
{
	return GC.getMapINLINE().plotSorenINLINE(m_iAttackPlotX, m_iAttackPlotY);
}


void CvUnit::setAttackPlot(const CvPlot* pNewValue)
{
	if (getAttackPlot() != pNewValue)
	{
		if (pNewValue != NULL)
		{
			m_iAttackPlotX = pNewValue->getX_INLINE();
			m_iAttackPlotY = pNewValue->getY_INLINE();
		}
		else
		{
			m_iAttackPlotX = INVALID_PLOT_COORD;
			m_iAttackPlotY = INVALID_PLOT_COORD;
		}
	}
}

int CvUnit::getCombatTimer() const
{
	return m_iCombatTimer;
}


void CvUnit::setCombatTimer(int iNewValue)
{
	m_iCombatTimer = iNewValue;
	FAssert(getCombatTimer() >= 0);
}


void CvUnit::changeCombatTimer(int iChange)
{
	setCombatTimer(getCombatTimer() + iChange);
}

int CvUnit::getCombatDamage() const
{
	return m_iCombatDamage;
}


void CvUnit::setCombatDamage(int iNewValue)
{
	m_iCombatDamage = iNewValue;
	FAssert(getCombatDamage() >= 0);
}


int CvUnit::getFortifyTurns() const
{
	return m_iFortifyTurns;
}


void CvUnit::setFortifyTurns(int iNewValue)
{
	iNewValue = range(iNewValue, 0, GC.getXMLval(XML_MAX_FORTIFY_TURNS));

	if (iNewValue != getFortifyTurns())
	{
		m_iFortifyTurns = iNewValue;
		setInfoBarDirty(true);
	}
}


void CvUnit::changeFortifyTurns(int iChange)
{
	setFortifyTurns(getFortifyTurns() + iChange);
	///TKs Med
	if (isOnMap() && canTrainUnit())
	{
        changeTrainCounter(iChange);
        CvCity* pCity = plot()->getPlotCity();
        int iTrainingTimeMod = 0;
        if (pCity != NULL)
        {
           for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); ++iBuilding)
           {
                BuildingTypes eBuilding = (BuildingTypes) iBuilding;
                if (GC.getBuildingInfo(eBuilding).getTrainingTimeMod() > 0)
                {
                    if (pCity->isHasRealBuilding(eBuilding))
                    {
                        iTrainingTimeMod += GC.getBuildingInfo(eBuilding).getTrainingTimeMod();
                    }
                }
           }
        }
        iTrainingTimeMod = GC.getXMLval(XML_TURNS_TO_TRAIN) - iTrainingTimeMod;
        iTrainingTimeMod = std::max(1, iTrainingTimeMod);
        if (getTrainCounter() >= iTrainingTimeMod)
        {
            if (pCity != NULL)
            {
                CvWString szBuffer = gDLL->getText("TXT_KEY_UNIT_TRAINED_CITY", getNameKey(), pCity->getNameKey());
                gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_UNIT_GREATPEOPLE", MESSAGE_TYPE_INFO, GC.getUnitInfo(getUnitType()).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT"), getX_INLINE(), getY_INLINE(), true, true);
            }
            else
            {
                CvWString szBuffer = gDLL->getText("TXT_KEY_UNIT_TRAINED_TOWER", getNameKey());
                gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_UNIT_GREATPEOPLE", MESSAGE_TYPE_INFO, GC.getUnitInfo(getUnitType()).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT"), getX_INLINE(), getY_INLINE(), true, true);
            }

            changeTrainCounter(-1);
            int iExperience = GC.getXMLval(XML_MAX_TRAINED_EXPERIENCE);
            changeExperience(iExperience, -1, false, false, false);
            setHasRealPromotion(((PromotionTypes)GC.getXMLval(XML_DEFAULT_TRAINED_PROMOTION)), true);
        }
	}
	///TKe
}


int CvUnit::getBlitzCount() const
{
	return m_iBlitzCount;
}


bool CvUnit::isBlitz() const
{
	return (getBlitzCount() > 0);
}


void CvUnit::changeBlitzCount(int iChange)
{
	m_iBlitzCount = (m_iBlitzCount + iChange);
	FAssert(getBlitzCount() >= 0);
}


int CvUnit::getAmphibCount() const
{
	return m_iAmphibCount;
}


bool CvUnit::isAmphib() const
{
	return (getAmphibCount() > 0);
}


void CvUnit::changeAmphibCount(int iChange)
{
	m_iAmphibCount = (m_iAmphibCount + iChange);
	FAssert(getAmphibCount() >= 0);
}


int CvUnit::getRiverCount() const
{
	return m_iRiverCount;
}


bool CvUnit::isRiver() const
{
	return (getRiverCount() > 0);
}


void CvUnit::changeRiverCount(int iChange)
{
	m_iRiverCount = (m_iRiverCount + iChange);
	FAssert(getRiverCount() >= 0);
}


int CvUnit::getEnemyRouteCount() const
{
	return m_iEnemyRouteCount;
}


bool CvUnit::isEnemyRoute() const
{
	return (getEnemyRouteCount() > 0);
}


void CvUnit::changeEnemyRouteCount(int iChange)
{
	m_iEnemyRouteCount = (m_iEnemyRouteCount + iChange);
	FAssert(getEnemyRouteCount() >= 0);
}


int CvUnit::getAlwaysHealCount() const
{
	return m_iAlwaysHealCount;
}


bool CvUnit::isAlwaysHeal() const
{
	return (getAlwaysHealCount() > 0);
}


void CvUnit::changeAlwaysHealCount(int iChange)
{
	m_iAlwaysHealCount = (m_iAlwaysHealCount + iChange);
	FAssert(getAlwaysHealCount() >= 0);
}


int CvUnit::getHillsDoubleMoveCount() const
{
	return m_iHillsDoubleMoveCount;
}


bool CvUnit::isHillsDoubleMove() const
{
	return (getHillsDoubleMoveCount() > 0);
}


void CvUnit::changeHillsDoubleMoveCount(int iChange)
{
	m_iHillsDoubleMoveCount = (m_iHillsDoubleMoveCount + iChange);
	FAssert(getHillsDoubleMoveCount() >= 0);
}

int CvUnit::getExtraVisibilityRange() const
{
    ///TKs Med
//    int iExtra = m_iExtraVisibilityRange;
//    if (isOnMap() && plot()->getImprovementType() != NO_IMPROVEMENT)
//    {
//        iExtra += GC.getImprovementInfo(plot()->getImprovementType()).getVisibilityChange();
//    }
//
//	return iExtra;
	return m_iExtraVisibilityRange;
	///Tke
}


void CvUnit::changeExtraVisibilityRange(int iChange)
{
	if (iChange != 0)
	{
		plot()->changeAdjacentSight(getTeam(), visibilityRange(), false, this, true);

		m_iExtraVisibilityRange += iChange;
		FAssert(getExtraVisibilityRange() >= 0);

		plot()->changeAdjacentSight(getTeam(), visibilityRange(), true, this, true);
	}
}


int CvUnit::getExtraMoves() const
{
	return m_iExtraMoves;
}


void CvUnit::changeExtraMoves(int iChange)
{
	m_iExtraMoves += iChange;
	FAssert(getExtraMoves() >= 0);
}


int CvUnit::getExtraMoveDiscount() const
{
	return m_iExtraMoveDiscount;
}


void CvUnit::changeExtraMoveDiscount(int iChange)
{
	m_iExtraMoveDiscount = (m_iExtraMoveDiscount + iChange);
	FAssert(getExtraMoveDiscount() >= 0);
}

int CvUnit::getExtraWithdrawal() const
{
	return m_iExtraWithdrawal;
}


void CvUnit::changeExtraWithdrawal(int iChange)
{
	m_iExtraWithdrawal = (m_iExtraWithdrawal + iChange);
	FAssert(getExtraWithdrawal() >= 0);
}

int CvUnit::getExtraBombardRate() const
{
	return m_iExtraBombardRate;
}


void CvUnit::changeExtraBombardRate(int iChange)
{
	m_iExtraBombardRate = (m_iExtraBombardRate + iChange);
	FAssert(getExtraBombardRate() >= 0);
}


int CvUnit::getExtraEnemyHeal() const
{
	return m_iExtraEnemyHeal;
}


void CvUnit::changeExtraEnemyHeal(int iChange)
{
	m_iExtraEnemyHeal = (m_iExtraEnemyHeal + iChange);
	FAssert(getExtraEnemyHeal() >= 0);
}


int CvUnit::getExtraNeutralHeal() const
{
	return m_iExtraNeutralHeal;
}


void CvUnit::changeExtraNeutralHeal(int iChange)
{
	m_iExtraNeutralHeal = (m_iExtraNeutralHeal + iChange);
	FAssert(getExtraNeutralHeal() >= 0);
}


int CvUnit::getExtraFriendlyHeal() const
{
	return m_iExtraFriendlyHeal;
}


void CvUnit::changeExtraFriendlyHeal(int iChange)
{
	m_iExtraFriendlyHeal = (m_iExtraFriendlyHeal + iChange);
	FAssert(getExtraFriendlyHeal() >= 0);
}


int CvUnit::getSameTileHeal() const
{
	return m_iSameTileHeal;
}


void CvUnit::changeSameTileHeal(int iChange)
{
	m_iSameTileHeal = (m_iSameTileHeal + iChange);
	FAssert(getSameTileHeal() >= 0);
}


int CvUnit::getAdjacentTileHeal() const
{
	return m_iAdjacentTileHeal;
}


void CvUnit::changeAdjacentTileHeal(int iChange)
{
	m_iAdjacentTileHeal = (m_iAdjacentTileHeal + iChange);
	FAssert(getAdjacentTileHeal() >= 0);
}


int CvUnit::getExtraCombatPercent() const
{
	return m_iExtraCombatPercent + GET_PLAYER(getOwnerINLINE()).getUnitStrengthModifier(getUnitClassType());
}


void CvUnit::changeExtraCombatPercent(int iChange)
{
	if (iChange != 0)
	{
		m_iExtraCombatPercent += iChange;

		setInfoBarDirty(true);
	}
}


int CvUnit::getExtraCityAttackPercent() const
{
	return m_iExtraCityAttackPercent;
}


void CvUnit::changeExtraCityAttackPercent(int iChange)
{
	if (iChange != 0)
	{
		m_iExtraCityAttackPercent = (m_iExtraCityAttackPercent + iChange);

		setInfoBarDirty(true);
	}
}


int CvUnit::getExtraCityDefensePercent() const
{
	return m_iExtraCityDefensePercent;
}


void CvUnit::changeExtraCityDefensePercent(int iChange)
{
	if (iChange != 0)
	{
		m_iExtraCityDefensePercent = (m_iExtraCityDefensePercent + iChange);

		setInfoBarDirty(true);
	}
}


int CvUnit::getExtraHillsAttackPercent() const
{
	return m_iExtraHillsAttackPercent;
}


void CvUnit::changeExtraHillsAttackPercent(int iChange)
{
	if (iChange != 0)
	{
		m_iExtraHillsAttackPercent = (m_iExtraHillsAttackPercent + iChange);

		setInfoBarDirty(true);
	}
}


int CvUnit::getExtraHillsDefensePercent() const
{
	return m_iExtraHillsDefensePercent;
}


void CvUnit::changeExtraHillsDefensePercent(int iChange)
{
	if (iChange != 0)
	{
		m_iExtraHillsDefensePercent = (m_iExtraHillsDefensePercent + iChange);

		setInfoBarDirty(true);
	}
}

int CvUnit::getPillageChange() const
{
	return m_iPillageChange;
}

void CvUnit::changePillageChange(int iChange)
{
	if (iChange != 0)
	{
		m_iPillageChange += iChange;

		setInfoBarDirty(true);
	}
}

int CvUnit::getUpgradeDiscount() const
{
	return m_iUpgradeDiscount;
}

void CvUnit::changeUpgradeDiscount(int iChange)
{
	if (iChange != 0)
	{
		m_iUpgradeDiscount += iChange;

		setInfoBarDirty(true);
	}
}

int CvUnit::getExperiencePercent() const
{
	return m_iExperiencePercent;
}

void CvUnit::changeExperiencePercent(int iChange)
{
	if (iChange != 0)
	{
		m_iExperiencePercent += iChange;

		setInfoBarDirty(true);
	}
}

DirectionTypes CvUnit::getFacingDirection(bool checkLineOfSightProperty) const
{
	if (checkLineOfSightProperty)
	{
		if (m_pUnitInfo->isLineOfSight())
		{
			return m_eFacingDirection; //only look in facing direction
		}
		else
		{
			return NO_DIRECTION; //look in all directions
		}
	}
	else
	{
		return m_eFacingDirection;
	}
}

void CvUnit::setFacingDirection(DirectionTypes eFacingDirection)
{
	if (eFacingDirection != m_eFacingDirection)
	{
		if (m_pUnitInfo->isLineOfSight())
		{
			//remove old fog
			plot()->changeAdjacentSight(getTeam(), visibilityRange(), false, this, true);

			//change direction
			m_eFacingDirection = eFacingDirection;

			//clear new fog
			plot()->changeAdjacentSight(getTeam(), visibilityRange(), true, this, true);

			gDLL->getInterfaceIFace()->setDirty(ColoredPlots_DIRTY_BIT, true);
		}
		else
		{
			m_eFacingDirection = eFacingDirection;
		}

		if (isOnMap())
		{
			//update formation
			NotifyEntity(NO_MISSION);
		}
	}
}

void CvUnit::rotateFacingDirectionClockwise()
{
	//change direction
	DirectionTypes eNewDirection = (DirectionTypes) ((m_eFacingDirection + 1) % NUM_DIRECTION_TYPES);
	setFacingDirection(eNewDirection);
}

void CvUnit::rotateFacingDirectionCounterClockwise()
{
	//change direction
	DirectionTypes eNewDirection = (DirectionTypes) ((m_eFacingDirection + NUM_DIRECTION_TYPES - 1) % NUM_DIRECTION_TYPES);
	setFacingDirection(eNewDirection);
}

ProfessionTypes CvUnit::getProfession() const
{
	return m_eProfession;
}
///Tks Med
void CvUnit::setProfession(ProfessionTypes eProfession, bool bForce)
{
	if (!bForce && !canHaveProfession(eProfession, false))
	{
		FAssertMsg(false, "Unit can not have profession");
		return;
	}
	///TKs Invention Core Mod v 1.0
	if (isHuman() && isOnMap() && eProfession == (ProfessionTypes)GC.getXMLval(XML_PROFESSION_INVENTOR))
	{
	    CivicTypes eCivic = GET_PLAYER(getOwner()).getCurrentResearch();
	    if (eCivic == NO_CIVIC)
	    {
	        int iPlotCityID = 0;
	        if (plot()->getPlotCity() != NULL)
	        {
	            iPlotCityID = plot()->getPlotCity()->getID();
	        }
	        CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CHOOSE_INVENTION, CIVICOPTION_INVENTIONS, iPlotCityID);
            gDLL->getInterfaceIFace()->addPopup(pInfo, getOwner(), true);
	    }
	}

    if (isOnMap())
    {
        ///TKs Med
        unloadAll();
        ///TKe
    }
	if (getProfession() != eProfession)
	{
		if (getProfession() != NO_PROFESSION)
		{
			if (GC.getProfessionInfo(getProfession()).isCitizen())
			{
				AI_setOldProfession(getProfession());
			}
		}
		if (isOnMap() && eProfession != NO_PROFESSION && GC.getProfessionInfo(eProfession).isCitizen())
		{
			CvCity* pCity = plot()->getPlotCity();
			if (pCity != NULL)
			{
				if (canJoinCity(plot()))
				{
					pCity->addPopulationUnit(this, eProfession);
					bool bLock = true;
					if (GC.getProfessionInfo(eProfession).isWorkPlot())
					{
						int iPlotIndex = pCity->AI_bestProfessionPlot(eProfession, this);
						if (iPlotIndex != -1)
						{
							pCity->alterUnitWorkingPlot(iPlotIndex, getID(), false);
						}
						else
						{
							bLock = false;
						}
					}

					setColonistLocked(bLock);
					return;
				}
			}
		}

		processProfession(getProfession(), -1, false);

		// CombatGearTypes - start - Nightinggale
		PromotionArray<int> aiPromotionChange;

		if (getProfession() != NO_PROFESSION)
		{
			CvProfessionInfo& kProfession = GC.getProfessionInfo(getProfession());
			if (kProfession.hasCombatGearTypes())
			{
				for (int iPromotion = 0; iPromotion < GC.getNumPromotionInfos(); ++iPromotion)
				{
					if (isHasPromotion((PromotionTypes) iPromotion))
					{
						if (!GC.getPromotionInfo((PromotionTypes) iPromotion).isCivilian())
						{
							aiPromotionChange.set(-1, iPromotion);
						}
					}
				}
			}
		}
		// CombatGearTypes - end - Nightinggale

		ProfessionTypes eOldProfession = getProfession();
		m_eProfession = eProfession;

		///FAssert(eProfession != NO_PROFESSION);
		FAssert(eProfession != INVALID_PROFESSION);
		//FAssert(eOldProfession != NO_PROFESSION);
		FAssert(eOldProfession != INVALID_PROFESSION);
		if (GC.getLogging())
        {
            if (gDLL->getChtLvl() > 0)
            {
                CvWString szBuffer;
                CvWString szOldBuffer;
                if (eProfession != NO_PROFESSION && eProfession != INVALID_PROFESSION)
                {
                    szBuffer.Format(L"%s", GC.getProfessionInfo(eProfession).getDescription());
                }
                else if (eProfession == NO_PROFESSION)
                {
                    szBuffer.Format(L"NO_PROFESSION");
                }
                else
                {
                    szBuffer.Format(L"INVALID_PROFESSION");
                }

                if (eOldProfession != NO_PROFESSION && eOldProfession != INVALID_PROFESSION)
                {
                    szOldBuffer.Format(L"%s", GC.getProfessionInfo(eOldProfession).getDescription());
                }
                else if (eOldProfession == NO_PROFESSION)
                {
                    szOldBuffer.Format(L"NO_PROFESSION");
                }
                else
                {
                    szOldBuffer.Format(L"INVALID_PROFESSION");
                }
                char szOut[1024];

                sprintf(szOut, "Player %d Unit %d (%S's %S) New Profession = %S; Old Profession = %S \n", getOwnerINLINE(), getID(), GET_PLAYER(getOwnerINLINE()).getNameKey(), getName().GetCString(), szBuffer.GetCString(), szOldBuffer.GetCString());

                gDLL->messageControlLog(szOut);


            }
        }

		// CombatGearTypes - start - Nightinggale
		if (getProfession() != NO_PROFESSION)
		{
			CvProfessionInfo& kProfession = GC.getProfessionInfo(getProfession());

			if (kProfession.hasCombatGearTypes())
			{
				for (int iPromotion = 0; iPromotion < GC.getNumPromotionInfos(); ++iPromotion)
				{
					if (isHasPromotion((PromotionTypes) iPromotion))
					{
						if (!GC.getPromotionInfo((PromotionTypes) iPromotion).isCivilian())
						{
							aiPromotionChange.add(1, iPromotion);
						}
					}
				}
			}
		}

		if (aiPromotionChange.isAllocated())
		{
			for (int iPromotion = 0; iPromotion < GC.getNumPromotionInfos(); ++iPromotion)
			{
				int iChange = aiPromotionChange.get(iPromotion);
				if (iChange != 0)
				{
					processPromotion((PromotionTypes) iPromotion, iChange);
				}
			}
		}
		// CombatGearTypes - end - Nightinggale

        ///Tke
		processProfession(getProfession(), 1, true);

		//reload unit model
		///Tks med
//		if (eProfession != NO_PROFESSION && !isNative())
//		{
//            if (GC.getProfessionInfo(eProfession).getLeadUnit() != NO_UNITCLASS)
//            {
//                //m_eLeaderUnitType = (UnitTypes)GC.getProfessionInfo(eProfession).getLeadUnit();
//                m_eLeaderUnitType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits((UnitClassTypes)GC.getProfessionInfo(eProfession).getLeadUnit());
//               // m_eLeaderUnitType = getUnitType();
//            }
//            else
//            {
//                m_eLeaderUnitType = NO_UNIT;
//            }
//		}
		///Tke
		reloadEntity();
		gDLL->getInterfaceIFace()->setDirty(Domestic_Advisor_DIRTY_BIT, true);
	}

	if (eProfession != NO_PROFESSION)
	{
		CvProfessionInfo& kProfession = GC.getProfessionInfo(eProfession);
		if (!kProfession.isCitizen())
		{
			if (kProfession.getDefaultUnitAIType() != NO_UNITAI)
			{
				AI_setUnitAIType((UnitAITypes)kProfession.getDefaultUnitAIType());
			}
		}
	}
}
///TKe
bool CvUnit::canHaveProfession(ProfessionTypes eProfession, bool bBumpOther, const CvPlot* pPlot) const
{
	if (NO_PROFESSION == eProfession)
	{
		return true;
	}

	if (eProfession == getProfession())
	{
		return true;
	}

	// invention effect cache - start - Nightinggale
	CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());

	if (!kOwner.canUseProfession(eProfession))
	{
		return false;
	}
	// invention effect cache - end - Nightinggale

	///TK Viscos Mod
    if (GC.getUnitInfo(getUnitType()).getProfessionsNotAllowed(eProfession))
    {
        return false;
    }
	///Tk end

	///TKs Med
	if (isHuman())
	{
	    if (GC.getProfessionInfo(eProfession).getRequiredPromotion() != NO_PROMOTION)
        {
             if (!isHasPromotion((PromotionTypes)GC.getProfessionInfo(eProfession).getRequiredPromotion()))
             {
                 return false;
             }
        }
        if (GC.getProfessionInfo(eProfession).getExperenceLevel() != 0)
        {
            if (getLevel() < GC.getProfessionInfo(eProfession).getExperenceLevel())
            {
                if (!isHasPromotion((PromotionTypes)GC.getXMLval(XML_DEFAULT_TRAINED_PROMOTION)) && !isHasRealPromotion((PromotionTypes)GC.getXMLval(XML_DEFAULT_KNIGHT_PROMOTION)))
                {
                    if (!isNoBadGoodies())
                    {
                        return false;
                    }
                }

            }
        }
	}

	if (GC.getProfessionInfo(eProfession).getCombatChange() > 2 && m_pUnitInfo->getCasteAttribute() == 2)
	{
	    return false;
	}

	if (m_pUnitInfo->getCasteAttribute() == 1)
	{
	    if (GC.getProfessionInfo(eProfession).isCitizen())
	    {
	        if (!GC.getProfessionInfo(eProfession).isWorkPlot())
	        {
	            return false;
	        }
	    }
	}

    if (GC.getProfessionInfo(eProfession).getTaxCollectRate() > 0)
    {
        bool bFoundVassal = false;
        for (int iI = 0; iI < MAX_PLAYERS; iI++)
        {
            if (GET_PLAYER((PlayerTypes)iI).isAlive())
            {
                if (GET_PLAYER((PlayerTypes)iI).getVassalOwner() == getOwner())
                {
                    bFoundVassal = true;
                    break;
                }
            }
        }
        if (!bFoundVassal)
        {
            return false;
        }

    }

	///Tke

	CvProfessionInfo& kNewProfession = GC.getProfessionInfo(eProfession);

	if (!kOwner.isProfessionValid(eProfession, getUnitType()))
	{
		return false;
	}
	///TKs Med Removed to Produce Education for Culture
	// MultipleYieldsProduced Start by Aymerick 22/01/2010
//	for (int i = 0; i < kNewProfession.getNumYieldsProduced(); i++)
//	{
//		if (kNewProfession.getYieldsProduced(i) == YIELD_EDUCATION)
//        {
//            if (m_pUnitInfo->getStudentWeight() <= 0)
//            {
//                return false;
//            }
//        }
//	}
	// MultipleYieldsProduced End
	///TKe
	if (pPlot == NULL)
	{
		pPlot = plot();
	}

	CvCity* pCity = NULL;
	if (pPlot != NULL)
	{
		if (pPlot->getOwnerINLINE() == getOwnerINLINE())
		{
			pCity = pPlot->getPlotCity();
		}
	}
	if (pCity == NULL)
	{
		pCity = kOwner.getPopulationUnitCity(getID());
	}

	bool bEuropeUnit = false;
	if (pCity == NULL)
	{
		CvUnit* pUnit = kOwner.getEuropeUnitById(getID());
		bEuropeUnit = (pUnit != NULL);
		FAssert(pUnit == this || pUnit == NULL);
	}

	if (pCity != NULL)
	{
		//make sure all equipment is available
		///TKs Med
		if (kNewProfession.getRequiredBuilding() != NO_BUILDING)
		{
		    if (!pCity->isHasBuildingClass((BuildingClassTypes)kNewProfession.getRequiredBuilding()))
		    {
		        return false;
		    }
		}

		if (!pCity->AI_isWorkforceHack())
		{
			// restructured to fully exploit speed gain from cache - Nightinggale
			if (kOwner.hasContentsYieldEquipmentAmount(eProfession))
			{
				for (int i=0; i < NUM_YIELD_TYPES; ++i)
				{
					YieldTypes eYieldType = (YieldTypes) i;
					int iYieldCarried = 0;
					if (getProfession() != NO_PROFESSION)
					{
						iYieldCarried = kOwner.getYieldEquipmentAmount(getProfession(), eYieldType);
					}
					int iYieldRequired = kOwner.getYieldEquipmentAmount(eProfession, eYieldType);
					if (iYieldRequired > 0)
					{
						int iMissing = iYieldRequired - iYieldCarried;
						if (iMissing > pCity->getYieldStored(eYieldType))
						{
							return false;
						}
					}
				}
			}
			// end of reconstruction - Nightinggale
#if 0
					if (bTryAlt)
					{
						if (kOwner.getAltYieldEquipmentAmount(eProfession, eYieldType) < 0)
						{
//					        int iAltYieldArray = kNewProfession.getAltEquipmentTypesArray();
//					        int iSize = (int)iAltYieldArray.size();
//					        int iYield = (int)iAltYieldArray[1];
							 int iYield = 0;
						   // YieldTypes eFirstAltYield = (YieldTypes)iAltYieldArray[0];
						   YieldTypes eAltYield = NO_YIELD;
							for (iYield=0;iYield< NUM_YIELD_TYPES; ++iYield)
							{
								//eAltYield = kNewProfession.getAltEquipmentAt(iYield);
								//YieldTypes eAltYield = (YieldTypes)iYield;
								if (kNewProfession.getAltEquipmentAt(iYield) > 0)
								{
									eAltYield = (YieldTypes)iYield;
									break;
								}

							}
							if (eAltYield != NO_YIELD)
							{
								iYieldCarried = 0;
								if (getProfession() != NO_PROFESSION)
								{
									iYieldCarried += kOwner.getYieldEquipmentAmount(getProfession(), eAltYield);
								}
								iYieldRequired = kOwner.getAltYieldEquipmentAmount(eProfession, eAltYield);
								if (iYieldRequired > 0)
								{
									int iMissing = iYieldRequired - iYieldCarried;
									if (iMissing <= pCity->getYieldStored(eAltYield))
									{
										bTryAlt = false;
									}
								}
							}

						}
					}

					if (bTryAlt)
					{
						return false;
					}
				}
			}
#endif

			if (!kNewProfession.isCitizen())
			{
				if (movesLeft() == 0)
				{
					return false;
				}
			}
		}
	}
///Tke
	if (bEuropeUnit && !kOwner.isEurope())
	{
		if (getEuropeProfessionChangeCost(eProfession) > kOwner.getGold())
		{
			return false;
		}

		if (kOwner.hasContentsYieldEquipmentAmount(eProfession)) // cache CvPlayer::getYieldEquipmentAmount - Nightinggale
		{
			for (int i=0; i < NUM_YIELD_TYPES; ++i)
			{
				YieldTypes eYield = (YieldTypes) i;
				if (!kOwner.isYieldEuropeTradable(eYield))
				{
					if (kOwner.getYieldEquipmentAmount(eProfession, eYield) > kOwner.getYieldEquipmentAmount(getProfession(), eYield))
					{
						return false;
					}
				}
			}
		}
	}

	if (pCity != NULL)
	{
		 if (!pCity->AI_isWorkforceHack())
		 {
			//check if special building has been built
			if (kNewProfession.getSpecialBuilding() != NO_SPECIALBUILDING)
			{
				if (pCity->getProfessionOutput(eProfession, this) <= 0)
				{
					return false;
				}
			}

			// check against building max
			if (!bBumpOther)
			{
				if (!pCity->isAvailableProfessionSlot(eProfession, this))
				{
					return false;
				}
			}

			//do not allow leaving empty city
			if (!kNewProfession.isCitizen() && !isOnMap())
			{
				if (pCity->getPopulation() <= 1)
				{
					return false;
				}
			}

			if (kNewProfession.isCitizen() && isOnMap())
			{
				if (!canJoinCity(pPlot))
				{
					return false;
				}
			}
		 }
	}
	else
	{
		if (kNewProfession.isCitizen())
		{
			return false;
		}

		if (isOnMap())
		{
			return false;
		}
	}

	return true;
}
 ///TKs Med
void CvUnit::processProfession(ProfessionTypes eProfession, int iChange, bool bUpdateCity)
{
	CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());
    CvCity* pCity = NULL;
    CvPlot* pPlot = plot();
    if (pPlot != NULL)
    {
        pCity = pPlot->getPlotCity();
    }

	if (iChange != 0)
	{
	    //std::vector<YieldTypes> aAltEquipment;
	    ///TKs Med Free Promotion
		if (eProfession != NO_PROFESSION)
		{
			for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
			{
				if (GET_PLAYER(getOwnerINLINE()).hasTrait((TraitTypes)iI))
				{
					for (int iJ = 0; iJ < GC.getNumPromotionInfos(); iJ++)
					{
						if (GC.getTraitInfo((TraitTypes) iI).isFreePromotion(iJ))
						{
							if (GC.getTraitInfo((TraitTypes) iI).isFreePromotionUnitProfession(eProfession))
							{
								changeFreePromotionCount(((PromotionTypes)iJ), iChange);
							}
						}
					}
				}
			}
		}
		processProfessionStats(eProfession, iChange);

		if (eProfession != NO_PROFESSION)
		{
			CvProfessionInfo& kProfession = GC.getProfessionInfo(eProfession);

			kOwner.changeAssets(iChange * kProfession.getAssetValue());

			int iPower = iChange * kProfession.getPowerValue();
			if (GET_PLAYER(getOwnerINLINE()).hasContentsYieldEquipmentAmount(eProfession)) // cache CvPlayer::getYieldEquipmentAmount - Nightinggale
			{
				for (int i = 0; i < NUM_YIELD_TYPES; ++i)
				{
					YieldTypes eYield = (YieldTypes) i;
					int iYieldAmount = GET_PLAYER(getOwnerINLINE()).getYieldEquipmentAmount(eProfession, eYield);
//					int iAltEquipmentYield = getAltEquipmentTypes(eYield);
//					if (iAltEquipmentYield > 0)
//					{
//					    iYieldAmount += iAltEquipmentYield;
//					}
//					if (iaAltEquipment[i] > 0)
//					{
//					    iYieldAmount -= iaAltEquipment[i];
//					}
					iPower += iChange * GC.getYieldInfo(eYield).getPowerValue() * iYieldAmount;
					kOwner.changeAssets(iChange * GC.getYieldInfo(eYield).getAssetValue() * iYieldAmount);
				}
			}

			kOwner.changePower(iPower);
			CvArea* pArea = area();
			if (pArea != NULL)
			{
				pArea->changePower(getOwnerINLINE(), iPower);
			}
		}
	}

    pCity = kOwner.getPopulationUnitCity(getID());
	if (pCity == NULL)
	{
		CvPlot* pPlot = plot();
		if (pPlot != NULL)
		{
			pCity = pPlot->getPlotCity();
		}
	}
	if (pCity != NULL && pCity->getOwnerINLINE() == getOwnerINLINE())
	{
		if (iChange != 0)
		{
			if (eProfession != NO_PROFESSION && (pCity->getPopulation() > 0 || GC.getXMLval(XML_CONSUME_EQUIPMENT_ON_FOUND) != 0))
			{
				for (int i = 0; i < NUM_YIELD_TYPES; i++)
				{
					YieldTypes eYield = (YieldTypes) i;
					pCity->changeYieldStored(eYield, -iChange * kOwner.getYieldEquipmentAmount(eProfession, eYield));
				}
			}
		}

		if (bUpdateCity)
		{
			pCity->setYieldRateDirty();
			pCity->updateYield();
			CvPlot* pPlot = pCity->getPlotWorkedByUnit(this);
			if(pPlot != NULL)
			{
				pCity->verifyWorkingPlot(pCity->getCityPlotIndex(pPlot));
			}
			pCity->AI_setAssignWorkDirty(true);
		}
	}
}
  ///TKe
void CvUnit::processProfessionStats(ProfessionTypes eProfession, int iChange)
{
	if (iChange != 0)
	{
		CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());
		if (eProfession != NO_PROFESSION)
		{
		    CvProfessionInfo& kProfession = GC.getProfessionInfo(eProfession);
			setBaseCombatStr(baseCombatStr() + iChange * (kProfession.getCombatChange() + kOwner.getProfessionCombatChange(eProfession)));
			changeExtraMoves(iChange * kProfession.getMovesChange());
			changeExtraWorkRate(iChange *  kProfession.getWorkRate());
			if (!kProfession.isCityDefender())
			{
				changeBadCityDefenderCount(iChange);
			}
			if (kProfession.isUnarmed())
			{
				changeUnarmedCount(iChange);
			}

			for (int iPromotion = 0; iPromotion < GC.getNumPromotionInfos(); iPromotion++)
			{
				if (kProfession.isFreePromotion(iPromotion))
				{
					changeFreePromotionCount((PromotionTypes) iPromotion, iChange);
				}
			}
		}

		processUnitCombatType(getProfessionUnitCombatType(eProfession), iChange);
	}
}
 ///Tke

int CvUnit::getProfessionChangeYieldRequired(ProfessionTypes eProfession, YieldTypes eYield) const
{
	CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());
	int iYieldCarried = 0;
	if (getProfession() != NO_PROFESSION)
	{
		iYieldCarried += kOwner.getYieldEquipmentAmount(getProfession(), eYield);
	}
	return (kOwner.getYieldEquipmentAmount(eProfession, eYield) - iYieldCarried);
}


int CvUnit::getEuropeProfessionChangeCost(ProfessionTypes eProfession) const
{
	CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());
	FAssert(kOwner.getParent() != NO_PLAYER);
	CvPlayer& kEurope = GET_PLAYER(kOwner.getParent());

	int iGoldCost = 0;
	for (int i=0; i < NUM_YIELD_TYPES; ++i)
	{
		YieldTypes eYieldType = (YieldTypes) i;
		int iMissing = getProfessionChangeYieldRequired(eProfession, eYieldType);
		if (iMissing > 0)
		{
			iGoldCost += kEurope.getYieldSellPrice(eYieldType) * iMissing;
		}
		else if (iMissing < 0)
		{
			iGoldCost -= kOwner.getSellToEuropeProfit(eYieldType, -iMissing);
		}
	}

	return iGoldCost;
}

int CvUnit::getImmobileTimer() const
{
	return m_iImmobileTimer;
}

void CvUnit::setImmobileTimer(int iNewValue)
{
	if (iNewValue != getImmobileTimer())
	{
		m_iImmobileTimer = iNewValue;

		setInfoBarDirty(true);
	}
}

void CvUnit::changeImmobileTimer(int iChange)
{
	if (iChange != 0)
	{
		setImmobileTimer(std::max(0, getImmobileTimer() + iChange));
	}
}

bool CvUnit::isMadeAttack() const
{
	return m_bMadeAttack;
}


void CvUnit::setMadeAttack(bool bNewValue)
{
	m_bMadeAttack = bNewValue;
}


bool CvUnit::isPromotionReady() const
{
	return m_bPromotionReady;
}

void CvUnit::setPromotionReady(bool bNewValue)
{
    ///TKs Med
	if (isPromotionReady() != bNewValue && getGroup() != NULL)
	{
	    ///TKe
		m_bPromotionReady = bNewValue;

		if (m_bPromotionReady)
		{
			getGroup()->setAutomateType(NO_AUTOMATE);
			getGroup()->clearMissionQueue();
			getGroup()->setActivityType(ACTIVITY_AWAKE);
		}

		gDLL->getEntityIFace()->showPromotionGlow(getUnitEntity(), bNewValue);

		if (IsSelected())
		{
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		}
	}
}


void CvUnit::testPromotionReady()
{
	setPromotionReady((getExperience() >= experienceNeeded()) && canAcquirePromotionAny());
}


bool CvUnit::isDelayedDeath() const
{
	return m_bDeathDelay;
}


void CvUnit::startDelayedDeath()
{
	m_bDeathDelay = true;
}


// Returns true if killed...
bool CvUnit::doDelayedDeath()
{
	if (m_bDeathDelay && !isFighting())
	{
		kill(false);
		return true;
	}

	return false;
}


bool CvUnit::isCombatFocus() const
{
	return m_bCombatFocus;
}


bool CvUnit::isInfoBarDirty() const
{
	return m_bInfoBarDirty;
}


void CvUnit::setInfoBarDirty(bool bNewValue)
{
	m_bInfoBarDirty = bNewValue;
}

PlayerTypes CvUnit::getOwner() const
{
	return getOwnerINLINE();
}

PlayerTypes CvUnit::getVisualOwner(TeamTypes eForTeam) const
{
	if (NO_TEAM == eForTeam)
	{
		eForTeam = GC.getGameINLINE().getActiveTeam();
	}

	if (getTeam() != eForTeam)
	{
		if (m_pUnitInfo->isHiddenNationality())
		{
			if (!plot()->isCity(true, getTeam()))
			{
				return UNKNOWN_PLAYER;
			}
		}
	}

	return getOwnerINLINE();
}


PlayerTypes CvUnit::getCombatOwner(TeamTypes eForTeam, const CvPlot* pPlot) const
{
	if (eForTeam != UNKNOWN_TEAM && getTeam() != eForTeam && eForTeam != NO_TEAM)
	{
		if (isAlwaysHostile(pPlot))
		{
			return UNKNOWN_PLAYER;
		}
	}

	return getOwnerINLINE();
}

TeamTypes CvUnit::getTeam() const
{
	return GET_PLAYER(getOwnerINLINE()).getTeam();
}

TeamTypes CvUnit::getCombatTeam(TeamTypes eForTeam, const CvPlot* pPlot) const
{
	TeamTypes eTeam;
	PlayerTypes eOwner = getCombatOwner(eForTeam, pPlot);
	switch (eOwner)
	{
	case UNKNOWN_PLAYER:
		eTeam = UNKNOWN_TEAM;
		break;
	case NO_PLAYER:
		eTeam = NO_TEAM;
		break;
	default:
		eTeam = GET_PLAYER(eOwner).getTeam();
		break;
	}

	return eTeam;
}

CivilizationTypes CvUnit::getVisualCiv(TeamTypes eForTeam) const
{
	PlayerTypes eOwner = getVisualOwner(eForTeam);
	if (eOwner == UNKNOWN_PLAYER)
	{
		return (CivilizationTypes) GC.getXMLval(XML_BARBARIAN_CIVILIZATION);
	}

	return GET_PLAYER(eOwner).getCivilizationType();
}

PlayerColorTypes CvUnit::getPlayerColor(TeamTypes eForTeam) const
{
	PlayerTypes eOwner = getVisualOwner(eForTeam);
	if (eOwner == UNKNOWN_PLAYER || eOwner == NO_PLAYER)
	{
		return (PlayerColorTypes) GC.getCivilizationInfo(getVisualCiv(eForTeam)).getDefaultPlayerColor();
	}

	return GET_PLAYER(eOwner).getPlayerColor();
}

PlayerTypes CvUnit::getCapturingPlayer() const
{
	return m_eCapturingPlayer;
}


void CvUnit::setCapturingPlayer(PlayerTypes eNewValue)
{
	m_eCapturingPlayer = eNewValue;
}


UnitTypes CvUnit::getUnitType() const
{
	return m_eUnitType;
}

CvUnitInfo &CvUnit::getUnitInfo() const
{
	return *m_pUnitInfo;
}


UnitClassTypes CvUnit::getUnitClassType() const
{
	return (UnitClassTypes)m_pUnitInfo->getUnitClassType();
}

UnitTypes CvUnit::getLeaderUnitType() const
{
	return m_eLeaderUnitType;
}

void CvUnit::setLeaderUnitType(UnitTypes leaderUnitType)
{
	if(m_eLeaderUnitType != leaderUnitType)
	{
		m_eLeaderUnitType = leaderUnitType;
		reloadEntity();
	}
}

CvUnit* CvUnit::getCombatUnit() const
{
	return getUnit(m_combatUnit);
}


void CvUnit::setCombatUnit(CvUnit* pCombatUnit, bool bAttacking)
{
	if (isCombatFocus())
	{
		gDLL->getInterfaceIFace()->setCombatFocus(false);
	}

	if (pCombatUnit != NULL)
	{
		if (bAttacking)
		{
			if (GC.getLogging())
			{
				if (gDLL->getChtLvl() > 0)
				{
					// Log info about this combat...
					char szOut[1024];
					sprintf( szOut, "*** KOMBAT!\n     ATTACKER: Player %d Unit %d (%S's %S), CombatStrength=%d\n     DEFENDER: Player %d Unit %d (%S's %S), CombatStrength=%d\n",
						getOwnerINLINE(), getID(), GET_PLAYER(getOwnerINLINE()).getName(), getName().GetCString(), currCombatStr(NULL, NULL),
						pCombatUnit->getOwnerINLINE(), pCombatUnit->getID(), GET_PLAYER(pCombatUnit->getOwnerINLINE()).getName(), pCombatUnit->getName().GetCString(), pCombatUnit->currCombatStr(pCombatUnit->plot(), this));
					gDLL->messageControlLog(szOut);
				}
			}
		}

		FAssertMsg(getCombatUnit() == NULL, "Combat Unit is not expected to be assigned");
		FAssertMsg(!(plot()->isFighting()), "(plot()->isFighting()) did not return false as expected");
		m_bCombatFocus = (bAttacking && !(gDLL->getInterfaceIFace()->isFocusedWidget()) && ((getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) || ((pCombatUnit->getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) && !(GC.getGameINLINE().isMPOption(MPOPTION_SIMULTANEOUS_TURNS)))));
		m_combatUnit = pCombatUnit->getIDInfo();
		///TK FS

        setCombatFirstStrikes((pCombatUnit->immuneToFirstStrikes()) ? 0 : (firstStrikes() + GC.getGameINLINE().getSorenRandNum(chanceFirstStrikes() + 1, "First Strike")));
        setUpCombatBlockParrys(pCombatUnit);
        setCombatAttackBlows(pCombatUnit);

		///TKe
		setCombatDamage(0);
		setPostCombatPlot(getX_INLINE(), getY_INLINE());
	}
	else
	{
		if(getCombatUnit() != NULL)
		{
			FAssertMsg(getCombatUnit() != NULL, "getCombatUnit() is not expected to be equal with NULL");
			FAssertMsg(plot()->isFighting(), "plot()->isFighting is expected to be true");
			m_bCombatFocus = false;
			m_combatUnit.reset();
			///TK FS

            setCombatFirstStrikes(0);
            setUpCombatBlockParrys(NULL);
            setCombatAttackBlows(NULL);

			///TKe
			setCombatDamage(0);

			if (IsSelected())
			{
				gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
			}

			if (plot() == gDLL->getInterfaceIFace()->getSelectionPlot())
			{
				gDLL->getInterfaceIFace()->setDirty(PlotListButtons_DIRTY_BIT, true);
			}

			CvPlot* pPlot = getPostCombatPlot();
			if (pPlot != plot())
			{
				if (pPlot->isFriendlyCity(*this, true))
				{
					setXY(pPlot->getX_INLINE(), pPlot->getY_INLINE());
					finishMoves();
				}
			}
			setPostCombatPlot(INVALID_PLOT_COORD, INVALID_PLOT_COORD);
		}
	}

	setCombatTimer(0);
	setInfoBarDirty(true);

	if (isCombatFocus())
	{
		gDLL->getInterfaceIFace()->setCombatFocus(true);
	}
}

CvPlot* CvUnit::getPostCombatPlot() const
{
	return GC.getMapINLINE().plotByIndexINLINE(m_iPostCombatPlotIndex);
}

void CvUnit::setPostCombatPlot(int iX, int iY)
{
	m_iPostCombatPlotIndex = GC.getMapINLINE().isPlotINLINE(iX, iY) ? GC.getMapINLINE().plotNumINLINE(iX, iY) : -1;
}

CvUnit* CvUnit::getTransportUnit() const
{
	return getUnit(m_transportUnit);
}


bool CvUnit::isCargo() const
{
	return (getTransportUnit() != NULL);
}

// returns false if the unit is killed
bool CvUnit::setTransportUnit(CvUnit* pTransportUnit, bool bUnload)
{
	CvUnit* pOldTransportUnit = getTransportUnit();

	if (pOldTransportUnit != pTransportUnit)
	{
		if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		}

		CvPlot* pPlot = plot();

		if (pOldTransportUnit != NULL)
		{
			pOldTransportUnit->changeCargo(-1);
		}
		m_transportUnit.reset();

		if (pTransportUnit != NULL)
		{
			FAssertMsg(pTransportUnit->cargoSpaceAvailable(getSpecialUnitType(), getDomainType()) > 0 || getYield() != NO_YIELD, "Cargo space is expected to be available");

			setUnitTravelState(pTransportUnit->getUnitTravelState(), false);

			//check if combining cargo
			YieldTypes eYield = getYield();
			if (eYield != NO_YIELD)
			{
				CvPlot* pPlot = pTransportUnit->plot();
				if (pPlot != NULL)
				{
					for (int i = 0; i < pPlot->getNumUnits(); i++)
					{
						CvUnit* pLoopUnit = pPlot->getUnitByIndex(i);
						if(pLoopUnit != NULL)
						{
							if (pLoopUnit->getTransportUnit() == pTransportUnit)
							{
								if (pLoopUnit->getYield() == eYield)
								{
									//merge yields
									int iTotalYields = pLoopUnit->getYieldStored() + getYieldStored();
									int iYield1 = std::min(iTotalYields, GC.getGameINLINE().getCargoYieldCapacity());
									int iYield2 = iTotalYields - iYield1;
									pLoopUnit->setYieldStored(iYield1);
									setYieldStored(iYield2);

									//all yields have been transferred to another unit
									if (getYieldStored() == 0)
									{
										kill(true);
										return false;
									}

									//check if load anymore of this cargo
									if (pTransportUnit->getLoadYieldAmount(eYield) == 0)
									{
										return true;
									}
								}
							}
						}
					}
				}
			}

			joinGroup(NULL, true); // Because what if a group of 3 tries to get in a transport which can hold 2...

			m_transportUnit = pTransportUnit->getIDInfo();

			getGroup()->setActivityType(ACTIVITY_SLEEP);

			if (pPlot != pTransportUnit->plot())
			{
				FAssert(getUnitTravelState() != NO_UNIT_TRAVEL_STATE);
				setXY(pTransportUnit->getX_INLINE(), pTransportUnit->getY_INLINE());
			}

			pTransportUnit->changeCargo(1);
			pTransportUnit->getGroup()->setActivityType(ACTIVITY_AWAKE);
		}
		else //dropped off of vehicle
		{
			if (!isHuman() && (getMoves() < maxMoves()))
			{
				if (pOldTransportUnit != NULL)
				{
					AI_setMovePriority(pOldTransportUnit->AI_getMovePriority() + 1);
				}
			}
			else
			{
				if (getGroup()->getActivityType() != ACTIVITY_MISSION)
				{
					getGroup()->setActivityType(ACTIVITY_AWAKE);
				}
			}

			//place yields into city
			if (bUnload && getYield() != NO_YIELD)
			{
				doUnloadYield(getYieldStored());
			}
		}

		if (pPlot != NULL)
		{
			pPlot->updateCenterUnit();
		}
	}

	return true;
}


int CvUnit::getExtraDomainModifier(DomainTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiExtraDomainModifier[eIndex];
}


void CvUnit::changeExtraDomainModifier(DomainTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiExtraDomainModifier[eIndex] = (m_aiExtraDomainModifier[eIndex] + iChange);
}


const CvWString CvUnit::getName(uint uiForm) const
{
	CvWString szBuffer;

	if (isEmpty(m_szName))
	{
		return m_pUnitInfo->getDescription(uiForm);
	}

	szBuffer.Format(L"%s (%s)", m_szName.GetCString(), m_pUnitInfo->getDescription(uiForm));

	return szBuffer;
}


const wchar* CvUnit::getNameKey() const
{
	if (isEmpty(m_szName))
	{
		return m_pUnitInfo->getTextKeyWide();
	}
	else
	{
		return m_szName.GetCString();
	}
}


const CvWString CvUnit::getNameNoDesc() const
{
	return m_szName.GetCString();
}

const CvWString CvUnit::getNameAndProfession() const
{
	CvWString szText;

	if (NO_PROFESSION != getProfession())
	{
		szText.Format(L"%s (%s)", GC.getProfessionInfo(getProfession()).getDescription(), getName().GetCString());
	}
	else
	{
		szText = getName();
	}

	return szText;
}

const wchar* CvUnit::getNameOrProfessionKey() const
{
	if(getProfession() != NO_PROFESSION)
	{
		return GC.getProfessionInfo(getProfession()).getTextKeyWide();
	}
	else
	{
		return getNameKey();
	}
}

void CvUnit::setName(CvWString szNewValue)
{
	gDLL->stripSpecialCharacters(szNewValue);

	m_szName = szNewValue;

	if (IsSelected())
	{
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
	}
}


std::string CvUnit::getScriptData() const
{
	return m_szScriptData;
}


void CvUnit::setScriptData(std::string szNewValue)
{
	m_szScriptData = szNewValue;
}


int CvUnit::getTerrainDoubleMoveCount(TerrainTypes eIndex) const
{
	return m_ja_iTerrainDoubleMoveCount.get(eIndex);
}


bool CvUnit::isTerrainDoubleMove(TerrainTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumTerrainInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return (getTerrainDoubleMoveCount(eIndex) > 0);
}


void CvUnit::changeTerrainDoubleMoveCount(TerrainTypes eIndex, int iChange)
{
	m_ja_iTerrainDoubleMoveCount.add(iChange, eIndex);
	FAssert(getTerrainDoubleMoveCount(eIndex) >= 0);
}


int CvUnit::getFeatureDoubleMoveCount(FeatureTypes eIndex) const
{
	return m_ja_iFeatureDoubleMoveCount.get(eIndex);
}


bool CvUnit::isFeatureDoubleMove(FeatureTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumFeatureInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return (getFeatureDoubleMoveCount(eIndex) > 0);
}


void CvUnit::changeFeatureDoubleMoveCount(FeatureTypes eIndex, int iChange)
{
	m_ja_iFeatureDoubleMoveCount.add(iChange, eIndex);
	FAssert(getFeatureDoubleMoveCount(eIndex) >= 0);
}


int CvUnit::getExtraTerrainAttackPercent(TerrainTypes eIndex) const
{
	return m_ja_iExtraTerrainAttackPercent.get(eIndex);
}


void CvUnit::changeExtraTerrainAttackPercent(TerrainTypes eIndex, int iChange)
{
	if (iChange != 0)
	{
		m_ja_iExtraTerrainAttackPercent.add(iChange, eIndex);

		setInfoBarDirty(true);
	}
}

int CvUnit::getExtraTerrainDefensePercent(TerrainTypes eIndex) const
{
	return m_ja_iExtraTerrainDefensePercent.get(eIndex);
}


void CvUnit::changeExtraTerrainDefensePercent(TerrainTypes eIndex, int iChange)
{
	if (iChange != 0)
	{
		m_ja_iExtraTerrainDefensePercent.add(iChange, eIndex);

		setInfoBarDirty(true);
	}
}

int CvUnit::getExtraFeatureAttackPercent(FeatureTypes eIndex) const
{
	return m_ja_iExtraFeatureAttackPercent.get(eIndex);
}


void CvUnit::changeExtraFeatureAttackPercent(FeatureTypes eIndex, int iChange)
{
	if (iChange != 0)
	{
		m_ja_iExtraFeatureAttackPercent.add(iChange, eIndex);

		setInfoBarDirty(true);
	}
}

int CvUnit::getExtraFeatureDefensePercent(FeatureTypes eIndex) const
{
	return m_ja_iExtraFeatureDefensePercent.get(eIndex);
}


void CvUnit::changeExtraFeatureDefensePercent(FeatureTypes eIndex, int iChange)
{
	if (iChange != 0)
	{
		m_ja_iExtraFeatureDefensePercent.add(iChange, eIndex);

		setInfoBarDirty(true);
	}
}

int CvUnit::getExtraUnitClassAttackModifier(UnitClassTypes eIndex) const
{
	return m_ja_iExtraUnitClassAttackModifier.get(eIndex);
}

void CvUnit::changeExtraUnitClassAttackModifier(UnitClassTypes eIndex, int iChange)
{
	m_ja_iExtraUnitClassAttackModifier.add(iChange, eIndex);
}

int CvUnit::getExtraUnitClassDefenseModifier(UnitClassTypes eIndex) const
{
	return m_ja_iExtraUnitClassDefenseModifier.get(eIndex);
}

void CvUnit::changeExtraUnitClassDefenseModifier(UnitClassTypes eIndex, int iChange)
{
	m_ja_iExtraUnitClassDefenseModifier.add(iChange, eIndex);
}

int CvUnit::getExtraUnitCombatModifier(UnitCombatTypes eIndex) const
{
	return m_ja_iExtraUnitCombatModifier.get(eIndex);
}

void CvUnit::changeExtraUnitCombatModifier(UnitCombatTypes eIndex, int iChange)
{
	m_ja_iExtraUnitCombatModifier.add(iChange, eIndex);
}

bool CvUnit::canAcquirePromotion(PromotionTypes ePromotion) const
{
	FAssertMsg(ePromotion >= 0, "ePromotion is expected to be non-negative (invalid Index)");
	FAssertMsg(ePromotion < GC.getNumPromotionInfos(), "ePromotion is expected to be within maximum bounds (invalid Index)");

	if (isHasPromotion(ePromotion))
	{
		return false;
	}

	///TKs Med
	if (GC.getPromotionInfo(ePromotion).isNonePromotion())
	{
	    return false;
	}

//    if (!m_pUnitInfo->isMechUnit() && !isHasPromotion((PromotionTypes)GC.getDefineINT("DEFAULT_TRAINED_PROMOTION")) && ePromotion != (PromotionTypes)GC.getDefineINT("DEFAULT_TRAINED_PROMOTION"))
//    {
//        return false;
//    }
	//Tke

	///TKs Invention Core Mod v 1.0
    if (!isNative() && !GET_PLAYER(getOwner()).isEurope())
	{
        for (int iCivic = 0; iCivic < GC.getNumCivicInfos(); ++iCivic)
        {
            if (GC.getCivicInfo((CivicTypes) iCivic).getCivicOptionType() == CIVICOPTION_INVENTIONS)
            {
                CvCivicInfo& kCivicInfo = GC.getCivicInfo((CivicTypes) iCivic);
                if (ePromotion != NO_PROMOTION && kCivicInfo.getAllowsPromotions(ePromotion) > 0)
                {
                    if (GET_PLAYER(getOwner()).getIdeasResearched((CivicTypes) iCivic) == 0)
                    {
                        return false;
                    }
                }
            }
        }
	}
	///TKe

	if (GC.getPromotionInfo(ePromotion).getPrereqPromotion() != NO_PROMOTION)
	{
		if (!isHasPromotion((PromotionTypes)(GC.getPromotionInfo(ePromotion).getPrereqPromotion())))
		{
			return false;
		}
	}

	if (GC.getPromotionInfo(ePromotion).getPrereqOrPromotion1() != NO_PROMOTION)
	{
		if (!isHasPromotion((PromotionTypes)(GC.getPromotionInfo(ePromotion).getPrereqOrPromotion1())))
		{
			if ((GC.getPromotionInfo(ePromotion).getPrereqOrPromotion2() == NO_PROMOTION) || !isHasPromotion((PromotionTypes)(GC.getPromotionInfo(ePromotion).getPrereqOrPromotion2())))
			{
				return false;
			}
		}
	}
	if (!isPromotionValid(ePromotion))
	{
		return false;
	}

	return true;
}

bool CvUnit::isPromotionValid(PromotionTypes ePromotion) const
{
	CvPromotionInfo& kPromotion = GC.getPromotionInfo(ePromotion);

	if (kPromotion.isGraphicalOnly() && !kPromotion.isLeader())
	{
		return false;
	}

	if (isOnlyDefensive())
	{
		if (kPromotion.getCityAttackPercent() != 0)
		{
			return false;
		}
		if (kPromotion.getWithdrawalChange() != 0)
		{
			return false;
		}
		if (kPromotion.isBlitz())
		{
			return false;
		}
		if (kPromotion.isAmphib())
		{
			return false;
		}
		if (kPromotion.isRiver())
		{
			return false;
		}
		if (kPromotion.getHillsAttackPercent() != 0)
		{
			return false;
		}
		for (int iTerrain = 0; iTerrain < GC.getNumTerrainInfos(); ++iTerrain)
		{
			if (kPromotion.getTerrainAttackPercent(iTerrain) != 0)
			{
				return false;
			}
		}
		for (int iFeature = 0; iFeature < GC.getNumFeatureInfos(); ++iFeature)
		{
			if (kPromotion.getFeatureAttackPercent(iFeature) != 0)
			{
				return false;
			}
		}
		if (kPromotion.getWithdrawalChange() != 0)
		{
			return false;
		}
	}

	if (NO_PROMOTION != kPromotion.getPrereqPromotion())
	{
		if (!isPromotionValid((PromotionTypes)kPromotion.getPrereqPromotion()))
		{
			return false;
		}
	}

	PromotionTypes ePrereq1 = (PromotionTypes)kPromotion.getPrereqOrPromotion1();
	PromotionTypes ePrereq2 = (PromotionTypes)kPromotion.getPrereqOrPromotion2();
	if (NO_PROMOTION != ePrereq1 || NO_PROMOTION != ePrereq2)
	{
		bool bValid = false;
		if (!bValid)
		{
			if (NO_PROMOTION != ePrereq1 && isPromotionValid(ePrereq1))
			{
				bValid = true;
			}
		}

		if (!bValid)
		{
			if (NO_PROMOTION != ePrereq2 && isPromotionValid(ePrereq2))
			{
				bValid = true;
			}
		}

		if (!bValid)
		{
			return false;
		}
	}

	if (getUnitCombatType() == NO_UNITCOMBAT)
	{
		return false;
	}

	// CombatGearTypes - start - Nightinggale
	bool bAllowedCombatType = kPromotion.getUnitCombat(getUnitCombatType()); // allowed promotion according to combat type

	if (!bAllowedCombatType)
	{
		ProfessionTypes eProfession = this->getProfession();
		if (eProfession != NO_PROFESSION)
		{
			CvProfessionInfo& kProfession = GC.getProfessionInfo(eProfession);

			if (kProfession.hasCombatGearTypes())
			{
				// check all gear types for the current profession
				for (int iType = 0; iType < GC.getNumUnitCombatInfos(); iType++)
				{
					if (kPromotion.getUnitCombat(iType) && kProfession.getCombatGearTypes(iType))
					{
						bAllowedCombatType = true;
						break;
					}
				}
			}
		}
		if (!bAllowedCombatType)
		{
			// Neighter the current unit's UnitCombatType or the unit's profession allows this promotion
			return false;
		}
	}
	// CombatGearTypes - end - Nightinggale

	if (kPromotion.getWithdrawalChange() + withdrawalProbability() > GC.getXMLval(XML_MAX_WITHDRAWAL_PROBABILITY))
	{
		return false;
	}

	return true;
}


bool CvUnit::canAcquirePromotionAny() const
{
	int iI;

	for (iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		if (canAcquirePromotion((PromotionTypes)iI))
		{
			return true;
		}
	}

	return false;
}


bool CvUnit::isHasPromotion(PromotionTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumPromotionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	CvPromotionInfo& kPromotion = GC.getPromotionInfo(eIndex);

	UnitCombatTypes eUnitCombat = getUnitCombatType();
	if (eUnitCombat == NO_UNITCOMBAT)
	{
		return false;
	}

	// CombatGearTypes - start - Nightinggale
	bool bNotAllowed = true;
	for (int iType = 0; iType < GC.getNumUnitCombatInfos(); iType++)
	{
		UnitCombatTypes eUnitCombat = (UnitCombatTypes) iType;
		if (kPromotion.getUnitCombat(eUnitCombat) && this->hasUnitCombatType(eUnitCombat))
		{
			bNotAllowed = false;
			break;
		}
	}
	if (bNotAllowed)
	{
		return false;
	}
	// CombatGearTypes - end - Nightinggale

	if (getFreePromotionCount(eIndex) <= 0 && !isHasRealPromotion(eIndex))
	{
		return false;
	}

	return true;
}

bool CvUnit::isHasRealPromotion(PromotionTypes eIndex) const
{
	return m_ja_bHasRealPromotion.get(eIndex);
}

void CvUnit::setHasRealPromotion(PromotionTypes eIndex, bool bValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumPromotionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (isHasRealPromotion(eIndex) != bValue)
	{
		/// unit promotion effect cache - start - Nightinggale
		bool bHasbefore = isHasPromotion(eIndex);
		/// unit promotion effect cache - end - Nightinggale

		if (bHasbefore)
		{
			processPromotion(eIndex, -1);
		}

		m_ja_bHasRealPromotion.set(bValue, eIndex);

		if (isHasPromotion(eIndex))
		{
			processPromotion(eIndex, 1);
		}

		if (IsSelected())
		{
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
		}

		/// unit promotion effect cache - start - Nightinggale
		if (bHasbefore && !isHasPromotion(eIndex))
		{
			// removing a promotion might allow releasing memory
			reclaimCacheMemory();
		}
		/// unit promotion effect cache - end - Nightinggale
	}
}

void CvUnit::changeFreePromotionCount(PromotionTypes eIndex, int iChange)
{
	if (iChange != 0)
	{
		int iCurrentFree = getFreePromotionCount(eIndex);
		setFreePromotionCount(eIndex, iCurrentFree + iChange);
	}
}

void CvUnit::processPromotion(PromotionTypes ePromotion, int iChange, bool bLoading)
{
    if (!bLoading && GC.getPromotionInfo(ePromotion).getEscortUnitClass() != NO_UNITCLASS)
    {
        UnitTypes eEscortUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits((UnitClassTypes)GC.getPromotionInfo(ePromotion).getEscortUnitClass());

        if (eEscortUnit != NO_UNIT)
        {
            if (iChange < 0)
            {
                m_eLeaderUnitType = NO_UNIT;
            }
            else if (iChange > 0)
            {
                m_eLeaderUnitType = eEscortUnit;
            }
            reloadEntity();
        }
    }
	//TK Civilian Promotions
	changePlotWorkedBonus(GC.getPromotionInfo(ePromotion).getPlotWorkedBonus() * iChange);
	changeBuildingWorkedBonus(GC.getPromotionInfo(ePromotion).getBuildingWorkedBonus() * iChange);
	//Tke
	changeBlitzCount((GC.getPromotionInfo(ePromotion).isBlitz()) ? iChange : 0);
	changeAmphibCount((GC.getPromotionInfo(ePromotion).isAmphib()) ? iChange : 0);
	changeRiverCount((GC.getPromotionInfo(ePromotion).isRiver()) ? iChange : 0);
	changeEnemyRouteCount((GC.getPromotionInfo(ePromotion).isEnemyRoute()) ? iChange : 0);
	changeAlwaysHealCount((GC.getPromotionInfo(ePromotion).isAlwaysHeal()) ? iChange : 0);
	changeHillsDoubleMoveCount((GC.getPromotionInfo(ePromotion).isHillsDoubleMove()) ? iChange : 0);
    ///TK FS
    changeImmuneToFirstStrikesCount((GC.getPromotionInfo(ePromotion).isImmuneToFirstStrikes()) ? iChange : 0);
    changeExtraFirstStrikes(GC.getPromotionInfo(ePromotion).getFirstStrikesChange() * iChange);
    changeExtraChanceFirstStrikes(GC.getPromotionInfo(ePromotion).getChanceFirstStrikesChange() * iChange);
    ///TKe
	changeExtraVisibilityRange(GC.getPromotionInfo(ePromotion).getVisibilityChange() * iChange);
	changeExtraMoves(GC.getPromotionInfo(ePromotion).getMovesChange() * iChange);
	changeExtraMoveDiscount(GC.getPromotionInfo(ePromotion).getMoveDiscountChange() * iChange);
	changeExtraWithdrawal(GC.getPromotionInfo(ePromotion).getWithdrawalChange() * iChange);
	changeExtraBombardRate(GC.getPromotionInfo(ePromotion).getBombardRateChange() * iChange);
	changeExtraEnemyHeal(GC.getPromotionInfo(ePromotion).getEnemyHealChange() * iChange);
	changeExtraNeutralHeal(GC.getPromotionInfo(ePromotion).getNeutralHealChange() * iChange);
	changeExtraFriendlyHeal(GC.getPromotionInfo(ePromotion).getFriendlyHealChange() * iChange);
	changeSameTileHeal(GC.getPromotionInfo(ePromotion).getSameTileHealChange() * iChange);
	changeAdjacentTileHeal(GC.getPromotionInfo(ePromotion).getAdjacentTileHealChange() * iChange);
	changeExtraCombatPercent(GC.getPromotionInfo(ePromotion).getCombatPercent() * iChange);
	changeExtraCityAttackPercent(GC.getPromotionInfo(ePromotion).getCityAttackPercent() * iChange);
	changeExtraCityDefensePercent(GC.getPromotionInfo(ePromotion).getCityDefensePercent() * iChange);
	changeExtraHillsAttackPercent(GC.getPromotionInfo(ePromotion).getHillsAttackPercent() * iChange);
	changeExtraHillsDefensePercent(GC.getPromotionInfo(ePromotion).getHillsDefensePercent() * iChange);
	changePillageChange(GC.getPromotionInfo(ePromotion).getPillageChange() * iChange);
	changeUpgradeDiscount(GC.getPromotionInfo(ePromotion).getUpgradeDiscount() * iChange);
	changeExperiencePercent(GC.getPromotionInfo(ePromotion).getExperiencePercent() * iChange);
	changeCargoSpace(GC.getPromotionInfo(ePromotion).getCargoChange() * iChange);

	for (int iI = 0; iI < GC.getNumTerrainInfos(); iI++)
	{
		changeExtraTerrainAttackPercent(((TerrainTypes)iI), (GC.getPromotionInfo(ePromotion).getTerrainAttackPercent(iI) * iChange));
		changeExtraTerrainDefensePercent(((TerrainTypes)iI), (GC.getPromotionInfo(ePromotion).getTerrainDefensePercent(iI) * iChange));
		changeTerrainDoubleMoveCount(((TerrainTypes)iI), ((GC.getPromotionInfo(ePromotion).getTerrainDoubleMove(iI)) ? iChange : 0));
	}

	for (int iI = 0; iI < GC.getNumFeatureInfos(); iI++)
	{
		changeExtraFeatureAttackPercent(((FeatureTypes)iI), (GC.getPromotionInfo(ePromotion).getFeatureAttackPercent(iI) * iChange));
		changeExtraFeatureDefensePercent(((FeatureTypes)iI), (GC.getPromotionInfo(ePromotion).getFeatureDefensePercent(iI) * iChange));
		changeFeatureDoubleMoveCount(((FeatureTypes)iI), ((GC.getPromotionInfo(ePromotion).getFeatureDoubleMove(iI)) ? iChange : 0));
	}

	for (int iI = 0; iI < GC.getNumUnitClassInfos(); ++iI)
	{
		changeExtraUnitClassAttackModifier((UnitClassTypes)iI, GC.getPromotionInfo(ePromotion).getUnitClassAttackModifier(iI) * iChange);
		changeExtraUnitClassDefenseModifier((UnitClassTypes)iI, GC.getPromotionInfo(ePromotion).getUnitClassDefenseModifier(iI) * iChange);
	}

	for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
	{
		changeExtraUnitCombatModifier(((UnitCombatTypes)iI), (GC.getPromotionInfo(ePromotion).getUnitCombatModifierPercent(iI) * iChange));
	}

	for (int iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		changeExtraDomainModifier(((DomainTypes)iI), (GC.getPromotionInfo(ePromotion).getDomainModifierPercent(iI) * iChange));
	}
}


void CvUnit::setFreePromotionCount(PromotionTypes eIndex, int iValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumPromotionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(iValue >= 0, "promotion value going negative");

	if (getFreePromotionCount(eIndex) != iValue)
	{
		/// unit promotion effect cache - start - Nightinggale
		bool bHasBefore = isHasPromotion(eIndex);
		/// unit promotion effect cache - end - Nightinggale
		if (bHasBefore)
		{
			processPromotion(eIndex, -1);
		}

		m_ja_iFreePromotionCount.set(iValue, eIndex);

		if (isHasPromotion(eIndex))
		{
			processPromotion(eIndex, 1);
		}

		if (IsSelected())
		{
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
		}

		/// unit promotion effect cache - start - Nightinggale
		if (bHasBefore && !isHasPromotion(eIndex))
		{
			// removing a promotion might allow releasing memory
			reclaimCacheMemory();
		}
		/// unit promotion effect cache - end - Nightinggale
	}
}

int CvUnit::getFreePromotionCount(PromotionTypes eIndex) const
{
	return m_ja_iFreePromotionCount.get(eIndex);
}

int CvUnit::getSubUnitCount() const
{
	return m_pUnitInfo->getGroupSize(getProfession());
}


int CvUnit::getSubUnitsAlive() const
{
	return getSubUnitsAlive( getDamage());
}


int CvUnit::getSubUnitsAlive(int iDamage) const
{
	if (iDamage >= maxHitPoints())
	{
		return 0;
	}
	else
	{
		return std::max(1, (((m_pUnitInfo->getGroupSize(getProfession()) * (maxHitPoints() - iDamage)) + (maxHitPoints() / ((m_pUnitInfo->getGroupSize(getProfession()) * 2) + 1))) / maxHitPoints()));
	}
}
// returns true if unit can initiate a war action with plot (possibly by declaring war)
bool CvUnit::potentialWarAction(const CvPlot* pPlot) const
{
	TeamTypes ePlotTeam = pPlot->getTeam();
	TeamTypes eUnitTeam = getTeam();

	if (ePlotTeam == NO_TEAM)
	{
		return false;
	}

	if (isEnemy(ePlotTeam, pPlot))
	{
		return true;
	}

	if (getGroup()->AI_isDeclareWar(pPlot) && GET_TEAM(eUnitTeam).AI_getWarPlan(ePlotTeam) != NO_WARPLAN)
	{
		return true;
	}

	return false;
}

void CvUnit::read(FDataStreamBase* pStream)
{
	// Init data before load
	reset();

	uint uiFlag=0;
	pStream->Read(&uiFlag);	// flags for expansion

	pStream->Read(&m_iID);
	pStream->Read(&m_iGroupID);
	pStream->Read(&m_iHotKeyNumber);
	pStream->Read(&m_iX);
	pStream->Read(&m_iY);
	pStream->Read(&m_iLastMoveTurn);
	pStream->Read(&m_iGameTurnCreated);
	pStream->Read(&m_iDamage);
	pStream->Read(&m_iMoves);
	pStream->Read(&m_iExperience);
	pStream->Read(&m_iLevel);
	pStream->Read(&m_iCargo);
	pStream->Read(&m_iAttackPlotX);
	pStream->Read(&m_iAttackPlotY);
	pStream->Read(&m_iCombatTimer);

	pStream->Read(&m_iCombatDamage);
	pStream->Read(&m_iFortifyTurns);
	pStream->Read(&m_iBaseCombat);
	pStream->Read((int*)&m_eFacingDirection);
	pStream->Read(&m_iImmobileTimer);
	pStream->Read(&m_iYieldStored);
	pStream->Read(&m_iExtraWorkRate);
	pStream->Read(&m_eProfession);
	pStream->Read(&m_iUnitTravelTimer);
	pStream->Read(&m_iBadCityDefenderCount);
	pStream->Read(&m_iUnarmedCount);
	pStream->Read((int*)&m_eUnitTravelState);
    ///TKs Med **TradeMarket**
	pStream->Read(&m_eUnitTradeMarket);
	pStream->Read(&m_iInvisibleTimer);
	pStream->Read(&m_iTravelPlotX);
	pStream->Read(&m_iTravelPlotY);
	pStream->Read(&m_iCombatFirstStrikes);
	pStream->Read(&m_iTrainCounter);
	pStream->Read(&m_iTraderCode);
	pStream->Read(&m_iCombatBlockParrys);
	pStream->Read(&m_iEscortPromotion);
	pStream->Read(&m_bCrushingBlows);
	pStream->Read(&m_bGlancingBlows);
	pStream->Read(&m_bFreeBuilding);
	///TKe
	pStream->Read(&m_bMadeAttack);
	pStream->Read(&m_bPromotionReady);
	pStream->Read(&m_bDeathDelay);
	pStream->Read(&m_bCombatFocus);
	// m_bInfoBarDirty not saved...
	pStream->Read(&m_bColonistLocked);
	// < JAnimals Mod Start >
	pStream->Read(&m_bBarbarian);
	// < JAnimals Mod End >

	pStream->Read((int*)&m_eOwner);
	pStream->Read((int*)&m_eCapturingPlayer);
	pStream->Read(&m_eUnitType);
	FAssert(NO_UNIT != m_eUnitType);
	m_pUnitInfo = (NO_UNIT != m_eUnitType) ? &GC.getUnitInfo(m_eUnitType) : NULL;
	pStream->Read(&m_eLeaderUnitType);


	m_combatUnit.read(pStream);
	///TKs Invention Core Mod v 1.0
	pStream->Read(&m_ConvertToUnit);
	///TKe
	pStream->Read(&m_iPostCombatPlotIndex);
	m_transportUnit.read(pStream);
	m_homeCity.read(pStream);

	pStream->ReadString(m_szName);
	pStream->ReadString(m_szScriptData);

	m_ja_bHasRealPromotion.read(pStream);
	m_ja_iFreePromotionCount.read(pStream);

	/// unit promotion effect cache - start - Nightinggale
	updatePromotionCache();
	/// unit promotion effect cache - end - Nightinggale
}


void CvUnit::write(FDataStreamBase* pStream)
{
	uint uiFlag=0;
	pStream->Write(uiFlag);		// flag for expansion

	pStream->Write(m_iID);
	pStream->Write(m_iGroupID);
	pStream->Write(m_iHotKeyNumber);
	pStream->Write(m_iX);
	pStream->Write(m_iY);
	pStream->Write(m_iLastMoveTurn);
	pStream->Write(m_iGameTurnCreated);
	pStream->Write(m_iDamage);
	pStream->Write(m_iMoves);
	pStream->Write(m_iExperience);
	pStream->Write(m_iLevel);
	pStream->Write(m_iCargo);
	pStream->Write(m_iAttackPlotX);
	pStream->Write(m_iAttackPlotY);
	pStream->Write(m_iCombatTimer);

	pStream->Write(m_iCombatDamage);
	pStream->Write(m_iFortifyTurns);
	pStream->Write(m_iBaseCombat);
	pStream->Write(m_eFacingDirection);
	pStream->Write(m_iImmobileTimer);
	pStream->Write(m_iYieldStored);
	pStream->Write(m_iExtraWorkRate);
	pStream->Write(m_eProfession);
	pStream->Write(m_iUnitTravelTimer);
	pStream->Write(m_iBadCityDefenderCount);
	pStream->Write(m_iUnarmedCount);
	pStream->Write(m_eUnitTravelState);
	//Tks Med
	pStream->Write(m_eUnitTradeMarket);
	pStream->Write(m_iInvisibleTimer);
	pStream->Write(m_iTravelPlotX);
	pStream->Write(m_iTravelPlotY);
	pStream->Write(m_iCombatFirstStrikes);
	pStream->Write(m_iTrainCounter);
	pStream->Write(m_iTraderCode);
	pStream->Write(m_iCombatBlockParrys);
	pStream->Write(m_iEscortPromotion);
	pStream->Write(m_bCrushingBlows);
	pStream->Write(m_bGlancingBlows);
	pStream->Write(m_bFreeBuilding);
	///TKe
	pStream->Write(m_bMadeAttack);
	pStream->Write(m_bPromotionReady);
	pStream->Write(m_bDeathDelay);
	pStream->Write(m_bCombatFocus);
	// m_bInfoBarDirty not saved...
	pStream->Write(m_bColonistLocked);
	// < JAnimals Mod Start >
	pStream->Write(m_bBarbarian);
	// < JAnimals Mod End >

	pStream->Write(m_eOwner);
	pStream->Write(m_eCapturingPlayer);
	pStream->Write(m_eUnitType);
	pStream->Write(m_eLeaderUnitType);


	m_combatUnit.write(pStream);
	///TKs Invention Core Mod v 1.0
	pStream->Write(m_ConvertToUnit);
	///TKe
	pStream->Write(m_iPostCombatPlotIndex);
	m_transportUnit.write(pStream);
	m_homeCity.write(pStream);

	pStream->WriteString(m_szName);
	pStream->WriteString(m_szScriptData);

	m_ja_bHasRealPromotion.write(pStream);
	m_ja_iFreePromotionCount.write(pStream);
}

// Protected Functions...

bool CvUnit::canAdvance(const CvPlot* pPlot, int iThreshold) const
{
	FAssert(canFight());
	FAssert(getDomainType() != DOMAIN_IMMOBILE);

	if (pPlot->getNumVisibleEnemyDefenders(this) > iThreshold)
	{
		return false;
	}

	if (isNoCityCapture() && pPlot->isEnemyCity(*this))
	{
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------
// FUNCTION:    CvUnit::planBattle
//! \brief      Determines in general how a battle will progress.
//!
//!				Note that the outcome of the battle is not determined here. This function plans
//!				how many sub-units die and in which 'rounds' of battle.
//! \param      kBattleDefinition The battle definition, which receives the battle plan.
//! \retval     The number of game turns that the battle should be given.
//------------------------------------------------------------------------------------------------
int CvUnit::planBattle( CvBattleDefinition & kBattleDefinition ) const
{
#define BATTLE_TURNS_SETUP 4
#define BATTLE_TURNS_ENDING 4
#define BATTLE_TURNS_MELEE 6
#define BATTLE_TURNS_RANGED 6
#define BATTLE_TURN_RECHECK 4

	int								aiUnitsBegin[BATTLE_UNIT_COUNT];
	int								aiUnitsEnd[BATTLE_UNIT_COUNT];
	int								aiToKillMelee[BATTLE_UNIT_COUNT];
	int								aiToKillRanged[BATTLE_UNIT_COUNT];
	CvBattleRoundVector::iterator	iIterator;
	int								i, j;
	bool							bIsLoser;
	int								iRoundIndex;
	int								iRoundCheck = BATTLE_TURN_RECHECK;

	// Initial conditions
	kBattleDefinition.setNumRangedRounds(0);
	kBattleDefinition.setNumMeleeRounds(0);
    ///TK FS
//    int iFirstStrikesDelta = kBattleDefinition.getFirstStrikes(BATTLE_UNIT_ATTACKER) - kBattleDefinition.getFirstStrikes(BATTLE_UNIT_DEFENDER);
//	if (iFirstStrikesDelta > 0) // Attacker first strikes
//	{
//		int iKills = computeUnitsToDie( kBattleDefinition, true, BATTLE_UNIT_DEFENDER );
//		kBattleDefinition.setNumRangedRounds(std::max(iFirstStrikesDelta, iKills / iFirstStrikesDelta));
//	}
//	else if (iFirstStrikesDelta < 0) // Defender first strikes
//	{
//		int iKills = computeUnitsToDie( kBattleDefinition, true, BATTLE_UNIT_ATTACKER );
//		iFirstStrikesDelta = -iFirstStrikesDelta;
//		kBattleDefinition.setNumRangedRounds(std::max(iFirstStrikesDelta, iKills / iFirstStrikesDelta));
//	}
    ///TKe
	increaseBattleRounds( kBattleDefinition);

	// Keep randomizing until we get something valid
	do
	{
		iRoundCheck++;
		if (( iRoundCheck >= BATTLE_TURN_RECHECK ) && !kBattleDefinition.isOneStrike())
		{
			increaseBattleRounds( kBattleDefinition);
			iRoundCheck = 0;
		}

		// Make sure to clear the battle plan, we may have to do this again if we can't find a plan that works.
		kBattleDefinition.clearBattleRounds();

		// Create the round list
		CvBattleRound kRound;
		int iTotalRounds = kBattleDefinition.getNumRangedRounds() + kBattleDefinition.getNumMeleeRounds();
		kBattleDefinition.setBattleRound(iTotalRounds, kRound);

		// For the attacker and defender
		for ( i = 0; i < BATTLE_UNIT_COUNT; i++ )
		{
			// Gather some initial information
			BattleUnitTypes unitType = (BattleUnitTypes) i;
			aiUnitsBegin[unitType] = kBattleDefinition.getUnit(unitType)->getSubUnitsAlive(kBattleDefinition.getDamage(unitType, BATTLE_TIME_BEGIN));
			aiToKillRanged[unitType] = computeUnitsToDie( kBattleDefinition, true, unitType);
			aiToKillMelee[unitType] = computeUnitsToDie( kBattleDefinition, false, unitType);
			aiUnitsEnd[unitType] = aiUnitsBegin[unitType] - aiToKillMelee[unitType] - aiToKillRanged[unitType];

			// Make sure that if they aren't dead at the end, they have at least one unit left
			if ( aiUnitsEnd[unitType] == 0 && !kBattleDefinition.getUnit(unitType)->isDead() )
			{
				aiUnitsEnd[unitType]++;
				if ( aiToKillMelee[unitType] > 0 )
				{
					aiToKillMelee[unitType]--;
				}
				else
				{
					aiToKillRanged[unitType]--;
				}
			}

			// If one unit is the loser, make sure that at least one of their units dies in the last round
			if ( aiUnitsEnd[unitType] == 0 )
			{
				kBattleDefinition.getBattleRound(iTotalRounds - 1).addNumKilled(unitType, 1);
				if ( aiToKillMelee[unitType] > 0)
				{
					aiToKillMelee[unitType]--;
				}
				else
				{
					aiToKillRanged[unitType]--;
				}
			}

			// Randomize in which round each death occurs
			bIsLoser = aiUnitsEnd[unitType] == 0;

			// Randomize the ranged deaths
			for ( j = 0; j < aiToKillRanged[unitType]; j++ )
			{
				iRoundIndex = GC.getGameINLINE().getSorenRandNum( range( kBattleDefinition.getNumRangedRounds(), 0, kBattleDefinition.getNumRangedRounds()), "Ranged combat death");
				kBattleDefinition.getBattleRound(iRoundIndex).addNumKilled(unitType, 1);
			}

			// Randomize the melee deaths
			for ( j = 0; j < aiToKillMelee[unitType]; j++ )
			{
				iRoundIndex = GC.getGameINLINE().getSorenRandNum( range( kBattleDefinition.getNumMeleeRounds() - (bIsLoser ? 1 : 2 ), 0, kBattleDefinition.getNumMeleeRounds()), "Melee combat death");
				kBattleDefinition.getBattleRound(kBattleDefinition.getNumRangedRounds() + iRoundIndex).addNumKilled(unitType, 1);
			}

			// Compute alive sums
			int iNumberKilled = 0;
			for(int j=0;j<kBattleDefinition.getNumBattleRounds();j++)
			{
				CvBattleRound &round = kBattleDefinition.getBattleRound(j);
				round.setRangedRound(j < kBattleDefinition.getNumRangedRounds());
				iNumberKilled += round.getNumKilled(unitType);
				round.setNumAlive(unitType, aiUnitsBegin[unitType] - iNumberKilled);
			}
		}

		// Now compute wave sizes
		for(int i=0;i<kBattleDefinition.getNumBattleRounds();i++)
		{
			CvBattleRound &round = kBattleDefinition.getBattleRound(i);
			round.setWaveSize(computeWaveSize(round.isRangedRound(), round.getNumAlive(BATTLE_UNIT_ATTACKER) + round.getNumKilled(BATTLE_UNIT_ATTACKER), round.getNumAlive(BATTLE_UNIT_DEFENDER) + round.getNumKilled(BATTLE_UNIT_DEFENDER)));
		}

		if ( iTotalRounds > 400 )
		{
			kBattleDefinition.setNumMeleeRounds(1);
			kBattleDefinition.setNumRangedRounds(0);
			break;
		}
	}
	while ( !verifyRoundsValid( kBattleDefinition ) && !kBattleDefinition.isOneStrike());

	//add a little extra time for leader to surrender
	bool attackerLeader = false;
	bool defenderLeader = false;
	bool attackerDie = false;
	bool defenderDie = false;
	int lastRound = kBattleDefinition.getNumBattleRounds() - 1;
	if(kBattleDefinition.getUnit(BATTLE_UNIT_ATTACKER)->getLeaderUnitType() != NO_UNIT)
		attackerLeader = true;
	if(kBattleDefinition.getUnit(BATTLE_UNIT_DEFENDER)->getLeaderUnitType() != NO_UNIT)
		defenderLeader = true;
	if(kBattleDefinition.getBattleRound(lastRound).getNumAlive(BATTLE_UNIT_ATTACKER) == 0)
		attackerDie = true;
	if(kBattleDefinition.getBattleRound(lastRound).getNumAlive(BATTLE_UNIT_DEFENDER) == 0)
		defenderDie = true;

	int extraTime = 0;
	if((attackerLeader && attackerDie) || (defenderLeader && defenderDie))
		extraTime = BATTLE_TURNS_MELEE;

	return BATTLE_TURNS_SETUP + BATTLE_TURNS_ENDING + kBattleDefinition.getNumMeleeRounds() * BATTLE_TURNS_MELEE + kBattleDefinition.getNumRangedRounds() * BATTLE_TURNS_MELEE + extraTime;
}

//------------------------------------------------------------------------------------------------
// FUNCTION:	CvBattleManager::computeDeadUnits
//! \brief		Computes the number of units dead, for either the ranged or melee portion of combat.
//! \param		kDefinition The battle definition.
//! \param		bRanged true if computing the number of units that die during the ranged portion of combat,
//!					false if computing the number of units that die during the melee portion of combat.
//! \param		iUnit The index of the unit to compute (BATTLE_UNIT_ATTACKER or BATTLE_UNIT_DEFENDER).
//! \retval		The number of units that should die for the given unit in the given portion of combat
//------------------------------------------------------------------------------------------------
int CvUnit::computeUnitsToDie( const CvBattleDefinition & kDefinition, bool bRanged, BattleUnitTypes iUnit ) const
{
	FAssertMsg( iUnit == BATTLE_UNIT_ATTACKER || iUnit == BATTLE_UNIT_DEFENDER, "Invalid unit index");

	BattleTimeTypes iBeginIndex = bRanged ? BATTLE_TIME_BEGIN : BATTLE_TIME_RANGED;
	BattleTimeTypes iEndIndex = bRanged ? BATTLE_TIME_RANGED : BATTLE_TIME_END;
	return kDefinition.getUnit(iUnit)->getSubUnitsAlive(kDefinition.getDamage(iUnit, iBeginIndex)) -
		kDefinition.getUnit(iUnit)->getSubUnitsAlive( kDefinition.getDamage(iUnit, iEndIndex));
}

//------------------------------------------------------------------------------------------------
// FUNCTION:    CvUnit::verifyRoundsValid
//! \brief      Verifies that all rounds in the battle plan are valid
//! \param      vctBattlePlan The battle plan
//! \retval     true if the battle plan (seems) valid, false otherwise
//------------------------------------------------------------------------------------------------
bool CvUnit::verifyRoundsValid( const CvBattleDefinition & battleDefinition ) const
{
	for(int i=0;i<battleDefinition.getNumBattleRounds();i++)
	{
		if(!battleDefinition.getBattleRound(i).isValid())
			return false;
	}
	return true;
}

//------------------------------------------------------------------------------------------------
// FUNCTION:    CvUnit::increaseBattleRounds
//! \brief      Increases the number of rounds in the battle.
//! \param      kBattleDefinition The definition of the battle
//------------------------------------------------------------------------------------------------
void CvUnit::increaseBattleRounds( CvBattleDefinition & kBattleDefinition ) const
{
	if(kBattleDefinition.isOneStrike())
	{
		kBattleDefinition.addNumRangedRounds(1);
	}
	else if ( kBattleDefinition.getUnit(BATTLE_UNIT_ATTACKER)->isRanged() && kBattleDefinition.getUnit(BATTLE_UNIT_DEFENDER)->isRanged())
	{
		kBattleDefinition.addNumRangedRounds(1);
	}
	else
	{
		kBattleDefinition.addNumMeleeRounds(1);
	}
}

//------------------------------------------------------------------------------------------------
// FUNCTION:    CvUnit::computeWaveSize
//! \brief      Computes the wave size for the round.
//! \param      bRangedRound true if the round is a ranged round
//! \param		iAttackerMax The maximum number of attackers that can participate in a wave (alive)
//! \param		iDefenderMax The maximum number of Defenders that can participate in a wave (alive)
//! \retval     The desired wave size for the given parameters
//------------------------------------------------------------------------------------------------
int CvUnit::computeWaveSize( bool bRangedRound, int iAttackerMax, int iDefenderMax ) const
{
	FAssertMsg( getCombatUnit() != NULL, "You must be fighting somebody!" );
	int aiDesiredSize[BATTLE_UNIT_COUNT];
	if ( bRangedRound )
	{
		aiDesiredSize[BATTLE_UNIT_ATTACKER] = getUnitInfo().getRangedWaveSize(getProfession());
		aiDesiredSize[BATTLE_UNIT_DEFENDER] = getCombatUnit()->getUnitInfo().getRangedWaveSize(getProfession());
	}
	else
	{
		aiDesiredSize[BATTLE_UNIT_ATTACKER] = getUnitInfo().getMeleeWaveSize(getProfession());
		aiDesiredSize[BATTLE_UNIT_DEFENDER] = getCombatUnit()->getUnitInfo().getMeleeWaveSize(getProfession());
	}

	aiDesiredSize[BATTLE_UNIT_DEFENDER] = aiDesiredSize[BATTLE_UNIT_DEFENDER] <= 0 ? iDefenderMax : aiDesiredSize[BATTLE_UNIT_DEFENDER];
	aiDesiredSize[BATTLE_UNIT_ATTACKER] = aiDesiredSize[BATTLE_UNIT_ATTACKER] <= 0 ? iDefenderMax : aiDesiredSize[BATTLE_UNIT_ATTACKER];
	return std::min( std::min( aiDesiredSize[BATTLE_UNIT_ATTACKER], iAttackerMax ), std::min( aiDesiredSize[BATTLE_UNIT_DEFENDER],
		iDefenderMax) );
}

bool CvUnit::isEnemy(TeamTypes eTeam, const CvPlot* pPlot) const
{
	if (NULL == pPlot)
	{
		pPlot = plot();
	}

	return (::atWar(getCombatTeam(eTeam, pPlot), eTeam));
}

bool CvUnit::isPotentialEnemy(TeamTypes eTeam, const CvPlot* pPlot) const
{
	if (NULL == pPlot)
	{
		pPlot = plot();
	}

	return (::isPotentialEnemy(getCombatTeam(eTeam, pPlot), eTeam));
}

void CvUnit::getDefenderCombatValues(CvUnit& kDefender, const CvPlot* pPlot, int iOurStrength, int iOurFirepower, int& iTheirOdds, int& iTheirStrength, int& iOurDamage, int& iTheirDamage, CombatDetails* pTheirDetails) const
{
	iTheirStrength = kDefender.currCombatStr(pPlot, this, pTheirDetails);
	int iTheirFirepower = kDefender.currFirepower(pPlot, this);

	FAssert((iOurStrength + iTheirStrength) > 0);
	FAssert((iOurFirepower + iTheirFirepower) > 0);

	iTheirOdds = ((GC.getXMLval(XML_COMBAT_DIE_SIDES) * iTheirStrength) / (iOurStrength + iTheirStrength));
	int iStrengthFactor = ((iOurFirepower + iTheirFirepower + 1) / 2);

	iOurDamage = std::max(1, ((GC.getXMLval(XML_COMBAT_DAMAGE) * (iTheirFirepower + iStrengthFactor)) / (iOurFirepower + iStrengthFactor)));
	iTheirDamage = std::max(1, ((GC.getXMLval(XML_COMBAT_DAMAGE) * (iOurFirepower + iStrengthFactor)) / (iTheirFirepower + iStrengthFactor)));
}

int CvUnit::getTriggerValue(EventTriggerTypes eTrigger, const CvPlot* pPlot, bool bCheckPlot) const
{
	CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(eTrigger);

	///TKs Med
	if (kTrigger.isOnUnitTrained())
	{
//	    if (isHasPromotion((PromotionTypes)GC.getDefineINT("DEFAULT_TRAINED_PROMOTION")) && getTrainCounter() >= 0 && getTrainCounter() >= GC.getDefineINT("TURNS_TO_TRAIN"))
//        {
//            if (getProfession() != NO_PROFESSION)
//            {
//                bool bNoVeteran = true;
//                for (int iPromotion = 0; iPromotion < GC.getNumPromotionInfos(); ++iPromotion)
//                {
//                    if (isHasPromotion((PromotionTypes) iPromotion))
//                    {
//                        if (GC.getPromotionInfo((PromotionTypes) iPromotion).getExperiencePercent() > 0)
//                        {
//                            bNoVeteran = false;
//                        }
//                    }
//                }
//
//                if (bNoVeteran)
//                {
//                    CvProfessionInfo& kProfession = GC.getProfessionInfo(getProfession());
//                    int iCombat = kProfession.getCombatChange();
//                    return iCombat;
//                }
//            }
//        }
        if (isHuman())
        {
            if (isHasRealPromotion((PromotionTypes)GC.getXMLval(XML_DEFAULT_KNIGHT_PROFESSION_PROMOTION)))
            {
                return 100;
            }
        }

        return MIN_INT;
	}
	//Tke

	if (kTrigger.getNumUnits() <= 0)
	{
		return MIN_INT;
	}

	if (!isEmpty(kTrigger.getPythonCanDoUnit()))
	{
		long lResult;

		CyArgsList argsList;
		argsList.add(eTrigger);
		argsList.add(getOwnerINLINE());
		argsList.add(getID());

		gDLL->getPythonIFace()->callFunction(PYRandomEventModule, kTrigger.getPythonCanDoUnit(), argsList.makeFunctionArgs(), &lResult);

		if (0 == lResult)
		{
			return MIN_INT;
		}
	}
	///TKs Med
    int iFoundValue = 0;
	if (kTrigger.getNumUnitsRequired() > 0)
	{
		bool bFoundValid = false;
		for (int i = 0; i < kTrigger.getNumUnitsRequired(); ++i)
		{
			if (getUnitClassType() == kTrigger.getUnitRequired(i))
			{
				bFoundValid = true;
				break;
			}
		}

		if (!bFoundValid)
		{
			return MIN_INT;
		}
		else
		{
		    iFoundValue = 100;
		}
	}

	if (bCheckPlot)
	{
		if (kTrigger.isUnitsOnPlot())
		{
			if (!plot()->canTrigger(eTrigger, getOwnerINLINE()))
			{
				return MIN_INT;
			}
		}
	}

	int iValue = iFoundValue;
    ///TKe
	if (0 == getDamage() && kTrigger.getUnitDamagedWeight() > 0)
	{
		return MIN_INT;
	}

	iValue += getDamage() * kTrigger.getUnitDamagedWeight();

	iValue += getExperience() * kTrigger.getUnitExperienceWeight();

	if (NULL != pPlot)
	{
		iValue += plotDistance(getX_INLINE(), getY_INLINE(), pPlot->getX_INLINE(), pPlot->getY_INLINE()) * kTrigger.getUnitDistanceWeight();
	}


	return iValue;
}

bool CvUnit::canApplyEvent(EventTypes eEvent) const
{
	CvEventInfo& kEvent = GC.getEventInfo(eEvent);

	if (0 != kEvent.getUnitExperience())
	{
		if (!canAcquirePromotionAny())
		{
			return false;
		}
	}

	if (NO_PROMOTION != kEvent.getUnitPromotion())
	{
		if (!canAcquirePromotion((PromotionTypes)kEvent.getUnitPromotion()))
		{
			return false;
		}
	}

	if (kEvent.getUnitImmobileTurns() > 0)
	{
		if (!canAttack())
		{
			return false;
		}
	}

	return true;
}

void CvUnit::applyEvent(EventTypes eEvent)
{
	if (!canApplyEvent(eEvent))
	{
		return;
	}

	CvEventInfo& kEvent = GC.getEventInfo(eEvent);

	if (0 != kEvent.getUnitExperience())
	{
		setDamage(0);
		changeExperience(kEvent.getUnitExperience());
	}

	if (NO_PROMOTION != kEvent.getUnitPromotion())
	{
		setHasRealPromotion((PromotionTypes)kEvent.getUnitPromotion(), true);
	}

	if (kEvent.getUnitImmobileTurns() > 0)
	{
		changeImmobileTimer(kEvent.getUnitImmobileTurns());
		CvWString szText = gDLL->getText("TXT_KEY_EVENT_UNIT_IMMOBILE", getNameOrProfessionKey(), kEvent.getUnitImmobileTurns());
		gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szText, "AS2D_UNITGIFTED", MESSAGE_TYPE_INFO, getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT"), getX_INLINE(), getY_INLINE(), true, true);
	}

	CvWString szNameKey(kEvent.getUnitNameKey());

	if (!szNameKey.empty())
	{
		setName(gDLL->getText(kEvent.getUnitNameKey()));
	}

	if (kEvent.isDisbandUnit())
	{
		kill(false);
	}
}

const CvArtInfoUnit* CvUnit::getArtInfo(int i) const
{
	//Androrc UnitArtStyles
//	return m_pUnitInfo->getArtInfo(i, getProfession());
	return m_pUnitInfo->getUnitArtStylesArtInfo(i, getProfession(), (UnitArtStyleTypes) GC.getCivilizationInfo(getCivilizationType()).getUnitArtStyleType());
	//Androrc End
}

const TCHAR* CvUnit::getButton() const
{
	const CvArtInfoUnit* pArtInfo = getArtInfo(0);

	if (NULL != pArtInfo)
	{
		return pArtInfo->getButton();
	}

	return m_pUnitInfo->getButton();
}

const TCHAR* CvUnit::getFullLengthIcon() const
{
	const CvArtInfoUnit* pArtInfo = getArtInfo(0);

	if (NULL != pArtInfo)
	{
		return pArtInfo->getFullLengthIcon();
	}

	return NULL;
}

bool CvUnit::isAlwaysHostile(const CvPlot* pPlot) const
{
	if (!m_pUnitInfo->isAlwaysHostile())
	{
		return false;
	}
	///TKs Med
	if (NULL != pPlot && pPlot->isCity(true, getTeam()) && getDomainType() == DOMAIN_SEA)
	{
		return false;
	}
	///TKe
	return true;
}

bool CvUnit::verifyStackValid()
{
	if (plot()->isVisibleEnemyUnit(this))
	{
		return jumpToNearestValidPlot();
	}

	return true;
}
///Tks Med
void CvUnit::setYieldStored(int iYieldAmount)
{
	int iChange = (iYieldAmount - getYieldStored());
	if (iChange != 0)
	{
		FAssert(iYieldAmount >= 0);
		m_iYieldStored = iYieldAmount;


		YieldTypes eYield = getYield();
		if (eYield != NO_YIELD)
		{
			GET_PLAYER(getOwnerINLINE()).changePower(iChange * GC.getYieldInfo(eYield).getPowerValue());
			GET_PLAYER(getOwnerINLINE()).changeAssets(iChange * GC.getYieldInfo(eYield).getAssetValue());
			CvArea* pArea = area();
			if (pArea  != NULL)
			{
				pArea->changePower(getOwnerINLINE(), iChange * GC.getYieldInfo(eYield).getPowerValue());
			}
			if (getYieldStored() == 0)
			{
				kill(true);
			}
		}
		else
		{
			if (!m_pUnitInfo->isTreasure() && getYieldStored() > 0)
			{
				CvPlayer& kPlayer = GET_PLAYER(getOwnerINLINE());
				CvCity* pCity = kPlayer.getPopulationUnitCity(getID());
				if (pCity != NULL)
				{
				    ///TKs Med Update 1.1g
				    int iEducationThreshold = pCity->educationThreshold();
				    if (m_pUnitInfo->getEducationUnitClass() != NO_UNITCLASS)
				    {
                        iEducationThreshold = GC.getXMLval(XML_EDUCATION_THRESHOLD);
				    }

					if (getYieldStored() >= iEducationThreshold)
					{
   
					    ///Tks Med
					    if (m_pUnitInfo->getKnightDubbingWeight() > 0 && !isHasRealPromotion((PromotionTypes)GC.getXMLval(XML_DEFAULT_KNIGHT_PROMOTION)))
					    {
					        if (pCity->getPopulation() == 1)
                            {
                                m_iYieldStored = iYieldAmount + iChange;
                                return;
                            }
                            setHasRealPromotion((PromotionTypes)GC.getXMLval(XML_DEFAULT_KNIGHT_PROMOTION), true);
                            setYieldStored(0);
					        if (m_pUnitInfo->getEducationUnitClass() != NO_UNITCLASS)
                            {
                                if (pCity->removePopulationUnit(this, false, (ProfessionTypes) GC.getCivilizationInfo(GET_PLAYER(getOwnerINLINE()).getCivilizationType()).getDefaultProfession()))
                                {
                                    UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits((UnitClassTypes)m_pUnitInfo->getEducationUnitClass());
                                    if (eUnit != NO_UNIT)
                                    {
                                        CvUnit* pLearnUnit = GET_PLAYER(getOwnerINLINE()).initUnit(eUnit, NO_PROFESSION, getX_INLINE(), getY_INLINE(), AI_getUnitAIType());
                                        FAssert(pLearnUnit != NULL);
                                        pLearnUnit->convert(this, true);
										CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_PAGE_GRADUATED", getNameKey(), pLearnUnit->getNameKey(), pCity->getNameKey());
										gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getUnitInfo(getUnitType()).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getX_INLINE(), getY_INLINE(), true, true);
                                    }
                                }


                            }
							else
							{
								CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_PAGE_GRADUATED", getNameKey(), pCity->getNameKey());
								gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getUnitInfo(getUnitType()).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getX_INLINE(), getY_INLINE(), true, true);
							}
					    }
					    else if (m_pUnitInfo->getEducationUnitClass() != NO_UNITCLASS)
					    {
					        if (pCity->getPopulation() == 1)
                            {
                                m_iYieldStored = iYieldAmount + iChange;
                                return;
                            }
					        UnitTypes eEduUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits((UnitClassTypes)m_pUnitInfo->getEducationUnitClass());
					        if (eEduUnit != NO_UNIT)
					        {
					            pCity->educateStudent(getID(), eEduUnit);
					        }

					    }
						else if (isHuman())
						///Tke
						{
							// Teacher List - start - Nightinggale
							CvPlayer& kPlayer = GET_PLAYER(GC.getGameINLINE().getActivePlayer());
							std::vector<UnitTypes> ordered_units;
							// make a list of ordered units, where the owner can affort training them.
							for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
							{
								int iPrice = pCity->getSpecialistTuition((UnitTypes) iI);
								if (iPrice >= 0 && iPrice <= kPlayer.getGold())
								{
									UnitTypes eUnitType = (UnitTypes) iI;
									if (kPlayer.canUseUnit(eUnitType))
									{
										for(int count = 0; count <	pCity->getOrderedStudents(eUnitType); count++)
										{
											// add one for each unit ordered, not just one for each type as a random one is selected in the end.
											ordered_units.push_back(eUnitType);
										}
									}
								}
							}
							
							if (!ordered_units.empty())
							{
								// Train the unit into
								int random_num = ordered_units.size();
								if (random_num == 1)
								{
									// The vector contains only one unit. The "random" unit has to be the first.
									random_num = 0;
								} else {
									random_num = GC.getGameINLINE().getSorenRandNum(random_num, "Pick unit for training");
								}
								pCity->educateStudent(this->getID(), ordered_units[random_num]);
							}
							else
							{
								// no ordered units can be trained
								// original code to open the popup to pick a unit
								CvPopupInfo* pPopupInfo = new CvPopupInfo(BUTTONPOPUP_CHOOSE_EDUCATION, pCity->getID(), getID());
								gDLL->getInterfaceIFace()->addPopup(pPopupInfo, getOwnerINLINE());
							}
							// Teacher List - end - Nightinggale
						}
						else
						{
							pCity->AI_educateStudent(getID());
						}
					}
				}
			}
		}
	}
}

int CvUnit::getYieldStored() const
{
	return m_iYieldStored;
}

YieldTypes CvUnit::getYield() const
{
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		YieldTypes eYield = (YieldTypes) iYield;
		if(getUnitClassType() == GC.getYieldInfo(eYield).getUnitClass())
		{
			return eYield;
		}
	}

	return NO_YIELD;
}

bool CvUnit::isGoods() const
{
	if (getYieldStored() > 0)
	{
		if (m_pUnitInfo->isTreasure())
		{
			return true;
		}

		if (getYield() != NO_YIELD)
		{
			if (GC.getYieldInfo(getYield()).isCargo())
			{
				return true;
			}
		}
	}

	return false;
}


// Private Functions...

//check if quick combat
bool CvUnit::isCombatVisible(const CvUnit* pDefender) const
{
	bool bVisible = false;

	if (!m_pUnitInfo->isQuickCombat())
	{
		if (NULL == pDefender || !pDefender->getUnitInfo().isQuickCombat())
		{
			if (isHuman())
			{
				if (!GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_QUICK_ATTACK))
				{
					bVisible = true;
				}
				///TK Med
				if (bVisible && NULL != pDefender)
				{
				    if (GC.getUnitInfo(pDefender->getUnitType()).isAnimal())
				    {
				        if (GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_MODDER_3))
				        {
				            bVisible = false;
				        }
				    }

				}
				///TKe
			}
			else if (NULL != pDefender && pDefender->isHuman())
			{
				if (!GET_PLAYER(pDefender->getOwnerINLINE()).isOption(PLAYEROPTION_QUICK_DEFENSE))
				{
					bVisible = true;
				}
			}
		}
	}

	return bVisible;
}

void CvUnit::changeBadCityDefenderCount(int iChange)
{
	m_iBadCityDefenderCount += iChange;
	FAssert(getBadCityDefenderCount() >= 0);
}

int CvUnit::getBadCityDefenderCount() const
{
	return m_iBadCityDefenderCount;
}

bool CvUnit::isCityDefender() const
{
	return (getBadCityDefenderCount() == 0);
}


void CvUnit::changeUnarmedCount(int iChange)
{
	m_iUnarmedCount += iChange;
	FAssert(getUnarmedCount() >= 0);
}

int CvUnit::getUnarmedCount() const
{
	return m_iUnarmedCount;
}

int CvUnit::getUnitTravelTimer() const
{
	return m_iUnitTravelTimer;
}

void CvUnit::setUnitTravelTimer(int iValue)
{
	m_iUnitTravelTimer = iValue;
	FAssert(getUnitTravelTimer() >= 0);
}

UnitTravelStates CvUnit::getUnitTravelState() const
{
	return m_eUnitTravelState;
}
///TKs Med
EuropeTypes CvUnit::getUnitTradeMarket() const
{
	return m_eUnitTradeMarket;
}

bool CvUnit::canGarrison() const
{
	if (!isOnMap())
	{
		return false;
	}

	if (cargoSpace() > 0)
	{
		return false;
	}

	if (!canAttack())
	{
		return false;
	}

	if (workRate(true) > 0)
	{
		return false;
	}

	if (getProfession() == NO_PROFESSION)
	{
		return false;
	}

	if (getDomainType() == DOMAIN_SEA)
	{
		return false;
	}

	return true;
}
void CvUnit::setUnitTradeMarket(EuropeTypes eMarket)
{
	m_eUnitTradeMarket = eMarket;
}
///Tks Med
void CvUnit::setUnitTravelState(UnitTravelStates eState, bool bShowEuropeScreen)
{
	if (getUnitTravelState() != eState)
	{
		CvPlot* pPlot = plot();
		if (pPlot != NULL)
		{
			pPlot->changeAdjacentSight(getTeam(), visibilityRange(), false, this, true);
		}


		UnitTravelStates eFromState = getUnitTravelState();
		m_eUnitTravelState = eState;

		if (pPlot != NULL)
		{
		    ///Tks Med
		    bool bMoveCargo = false;
            CvPlot* pTravelPlot = NULL;

			if (eFromState == UNIT_TRAVEL_STATE_FROM_EUROPE)
			{
			    ///Tke
				EuropeTypes eEurope = pPlot->getEurope();
				if (eEurope != NO_EUROPE)
				{
					switch (GC.getEuropeInfo(eEurope).getCardinalDirection())
					{
					case CARDINALDIRECTION_EAST:
						setFacingDirection(DIRECTION_WEST);
						break;
					case CARDINALDIRECTION_WEST:
						setFacingDirection(DIRECTION_EAST);
						break;
					case CARDINALDIRECTION_NORTH:
						setFacingDirection(DIRECTION_SOUTH);
						break;
					case CARDINALDIRECTION_SOUTH:
						setFacingDirection(DIRECTION_NORTH);
						break;
					}
				}
            ///Tks Med
                if (isHuman())
                {
                    pTravelPlot = getTravelPlot();
                    if (pTravelPlot != NULL)
                    {
                        if (pPlot != pTravelPlot)
                        {
                            GET_PLAYER(getOwnerINLINE()).setStartingPlot(pTravelPlot, false);
                           //setXY(pTravelPlot->getX_INLINE(), pTravelPlot->getY_INLINE());
    //                        if (hasCargo())
    //                        {
    //                            bMoveCargo = true;
    //                        }
                        }

                    }
                }
			}
			
			if (isHuman() && eState == UNIT_TRAVEL_STATE_TO_EUROPE)
			{
				if ((EuropeTypes)getUnitTradeMarket() != NO_EUROPE && GC.getEuropeInfo((EuropeTypes)getUnitTradeMarket()).isNoEuropePlot())
				{
					setTravelPlot();
				}
			}
			///Tke

			pPlot->changeAdjacentSight(getTeam(), visibilityRange(), true, this, true);

			if (hasCargo())
			{
				for(CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode(); pUnitNode != NULL; pUnitNode = pPlot->nextUnitNode(pUnitNode))
				{
					CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
					if (pLoopUnit->getTransportUnit() == this)
					{
						pLoopUnit->setUnitTravelState(eState, false);
					}
				}
			}
		}

		if (getGroup() != NULL)
		{
			getGroup()->splitGroup(1, this);
		}

		if (!isOnMap())
		{
			if (IsSelected())
			{
				gDLL->getInterfaceIFace()->removeFromSelectionList(this);
			}
		}
		else
		{
			GET_PLAYER(getOwnerINLINE()).updateGroupCycle(this);
			setUnitTradeMarket(NO_EUROPE);
		}

		//popup europe screen
		if (bShowEuropeScreen && (EuropeTypes)getUnitTradeMarket() != NO_EUROPE)
		{

			if (getUnitTravelState() == UNIT_TRAVEL_STATE_IN_EUROPE)
			{
				if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
				{
					bool bFound = false;
					const CvPopupQueue& kPopups = GET_PLAYER(getOwnerINLINE()).getPopups();
					FAssert((EuropeTypes)getUnitTradeMarket() != NO_EUROPE);
					CvWString szTradeRoute = GC.getEuropeInfo((EuropeTypes)getUnitTradeMarket()).getPythonTradeScreen();
					for (CvPopupQueue::const_iterator it = kPopups.begin(); it != kPopups.end(); it++)
					{
						CvPopupInfo* pInfo = *it;
						if (NULL != pInfo)
						{
							if (pInfo->getButtonPopupType() == BUTTONPOPUP_PYTHON_SCREEN && pInfo->getText() == szTradeRoute)
							{
								bFound = true;
								break;
							}
						}
					}

					if(!bFound)
					{
						CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_PYTHON_SCREEN);
						pInfo->setText(szTradeRoute);
						gDLL->getInterfaceIFace()->addPopup(pInfo, getOwnerINLINE(), false);
					}
				}
			}
		}

		if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(EuropeScreen_DIRTY_BIT, true);
		}

		gDLL->getEventReporterIFace()->unitTravelStateChanged(getOwnerINLINE(), eState, getID());

		if (pPlot != NULL)
		{
			pPlot->updateCenterUnit();
		}
	}
}
///TKe
bool CvUnit::setSailEurope(EuropeTypes eEurope)
{
	CvPlot* pBestPlot = NULL;

	if (eEurope == NO_EUROPE)
	{
		return true;
	}

	if (plot()->getEurope() == eEurope)
	{
		return true;
	}

	CvPlayerAI& kLoopPlayer = GET_PLAYER(getOwnerINLINE());
	for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		CvPlot* pPlot = GC.getMapINLINE().plotByIndexINLINE(iI);
		int iAvgDistance = 0;
		int iBestDistance = 100000;
		if (pPlot->getEurope() == eEurope)
		{
			if (pPlot->isRevealed(getTeam(), false))
			{
				if (kLoopPlayer.getNumCities() > 0)
				{
					int iLoop;
					for (CvCity* pLoopCity = kLoopPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kLoopPlayer.nextCity(&iLoop))
					{
						iAvgDistance += stepDistance(pPlot->getX_INLINE(), pPlot->getY_INLINE(), pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE());
					}
				}
				else
				{
					iAvgDistance += stepDistance(pPlot->getX_INLINE(), pPlot->getY_INLINE(), kLoopPlayer.getStartingPlot()->getX_INLINE(), kLoopPlayer.getStartingPlot()->getY_INLINE());
				}

				if (iAvgDistance > 0 && iAvgDistance < iBestDistance)
				{
					iBestDistance = iAvgDistance;
					pBestPlot = pPlot;
				}
			}
		}
	}
	if (pBestPlot != NULL)
	{
		setXY(pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE());
		return true;
	}
	return false;
}
///TKe Med
bool CvUnit::canSailEurope(EuropeTypes eEurope)
{
	if (eEurope == NO_EUROPE)
	{
		return true;
	}
	if (!isHuman())
	{
		if (plot()->getEurope() == eEurope)
		{
			return true;
		}
	}
	else
	{
		if (plot()->isTradeScreenAccessPlot(eEurope))
		{
			return true;
		}
	}

	for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		CvPlot* pPlot = GC.getMapINLINE().plotByIndexINLINE(iI);
		if (pPlot->isRevealed(getTeam(), false))
		{
			if (!isHuman())
			{
				if (pPlot->getEurope() == eEurope)
				{
					return true;
				}
			}
			else
			{
				if (pPlot->isTradeScreenAccessPlot(eEurope))
				{
					return true;
				}
			}
		}
	}

	
	if(isHuman() && !GC.getCivilizationInfo(getCivilizationType()).isWaterStart())
	{
	    if (cargoSpace() > 0 && getDomainType() == DOMAIN_LAND)
	    {
            return true;
	    }
	}
	///TKe
	return false;
}

void CvUnit::setHomeCity(CvCity* pNewValue)
{
	if (pNewValue == NULL)
	{
		m_homeCity.reset();
	}
	else
	{
		if (AI_getUnitAIType() == UNITAI_WORKER)
		{
			CvCity* pExistingCity = getHomeCity();
			if (pExistingCity != NULL && pExistingCity != pNewValue)
			{
				getHomeCity()->AI_changeWorkersHave(-1);
			}
			pNewValue->AI_changeWorkersHave(+1);
		}
		m_homeCity = pNewValue->getIDInfo();
	}
}

CvCity* CvUnit::getHomeCity() const
{
	return ::getCity(m_homeCity);
}

bool CvUnit::isOnMap() const
{
	if (getUnitTravelState() != NO_UNIT_TRAVEL_STATE)
	{
		return false;
	}

	if((getX_INLINE() == INVALID_PLOT_COORD) || (getY_INLINE() == INVALID_PLOT_COORD))
	{
		return false;
	}

	return true;
}


void CvUnit::doUnitTravelTimer()
{
	if (getUnitTravelTimer() > 0)
	{
		setUnitTravelTimer(getUnitTravelTimer() - 1);

		if (getUnitTravelTimer() == 0)
		{
		    ///Tks Med
		    UnitTravelStates eFromState = getUnitTravelState();
		    ///TKe

			switch (getUnitTravelState())
			{
			case UNIT_TRAVEL_STATE_FROM_EUROPE:
				setUnitTravelState(NO_UNIT_TRAVEL_STATE, false);
				break;
			case UNIT_TRAVEL_STATE_TO_EUROPE:
				setUnitTravelState(UNIT_TRAVEL_STATE_IN_EUROPE, true);
				break;
			case UNIT_TRAVEL_STATE_LIVE_AMONG_NATIVES:
				setUnitTravelState(NO_UNIT_TRAVEL_STATE, false);
				doLearn();
				break;
            ///Tks Med
            case UNIT_TRAVEL_STATE_FROM_SPICE_ROUTE:
				setUnitTravelState(NO_UNIT_TRAVEL_STATE, false);
				break;
			case UNIT_TRAVEL_STATE_TO_SPICE_ROUTE:
				setUnitTravelState(UNIT_TRAVEL_STATE_IN_SPICE_ROUTE, true);
				break;

            case UNIT_TRAVEL_STATE_FROM_SILK_ROAD:
                setUnitTravelState(NO_UNIT_TRAVEL_STATE, false);
                break;
			case UNIT_TRAVEL_STATE_TO_SILK_ROAD:
				setUnitTravelState(UNIT_TRAVEL_STATE_IN_SILK_ROAD, true);
				break;
             case UNIT_TRAVEL_STATE_FROM_TRADE_FAIR:
                setUnitTravelState(NO_UNIT_TRAVEL_STATE, false);
                break;
			case UNIT_TRAVEL_STATE_TO_TRADE_FAIR:
				setUnitTravelState(UNIT_TRAVEL_STATE_IN_TRADE_FAIR, true);
				break;
            case UNIT_TRAVEL_STATE_FROM_IMMIGRATION:
				setUnitTravelState(NO_UNIT_TRAVEL_STATE, false);
				break;



            case UNIT_TRAVEL_STATE_HIDE_UNIT:
				setUnitTravelState(NO_UNIT_TRAVEL_STATE, false);
				break;
            case UNIT_TRAVEL_LOST_AT_SEA:
				setUnitTravelState(NO_UNIT_TRAVEL_STATE, false);
				break;
            ///TKe
			default:
				FAssertMsg(false, "Unit arriving from nowhere");
				break;
			}

			if (getUnitTradeMarket() != NO_EUROPE)
			{
				if (GC.getEuropeInfo((EuropeTypes)getUnitTradeMarket()).isNoEuropePlot())
				{
					CvPlot* pTravelPlot = getTravelPlot();
					if (pTravelPlot != NULL)
					{
						if (plot() != pTravelPlot)
						{
							//GET_PLAYER(getOwnerINLINE()).setStartingPlot(pTravelPlot, false);
						   setXY(pTravelPlot->getX_INLINE(), pTravelPlot->getY_INLINE());
	//                        if (hasCargo())
	//                        {
	//                            bMoveCargo = true;
	//                        }
						}

					}
				}
			}
		}
	}
}

bool CvUnit::isColonistLocked()
{
	return m_bColonistLocked;
}

void CvUnit::setColonistLocked(bool bNewValue)
{
	if (m_bColonistLocked != bNewValue)
	{
		m_bColonistLocked = bNewValue;

		if (bNewValue == true)
		{
			CvCity* pCity = GET_PLAYER(getOwnerINLINE()).getPopulationUnitCity(getID());

			FAssert(pCity != NULL);

			CvPlot* pPlot = pCity->getPlotWorkedByUnit(this);

			if (pPlot != NULL)
			{
				//Ensure it is not stolen.
				pPlot->setWorkingCityOverride(pCity);
			}
		}
	}
}

// < JAnimals Mod Start >
bool CvUnit::isBarbarian() const
{
	return m_bBarbarian;
}

void CvUnit::setBarbarian(bool bNewValue)
{
    if (bNewValue != isBarbarian())
    {
        m_bBarbarian = bNewValue;
    }
}
// < JAnimals Mod End >

bool CvUnit::raidWeapons(std::vector<int>& aYields)
{
	CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());
	ProfessionTypes eCurrentProfession = getProfession();
	std::vector<ProfessionTypes> aProfessions;
	for (int iProfession = 0; iProfession < GC.getNumProfessionInfos(); ++iProfession)
	{
		ProfessionTypes eProfession = (ProfessionTypes) iProfession;
		if (canHaveProfession(eProfession, false))
		{
			if (eCurrentProfession == NO_PROFESSION || GC.getProfessionInfo(eProfession).getCombatChange() > GC.getProfessionInfo(eCurrentProfession).getCombatChange())
			{
				bool bCanHaveProfession = false;
				for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
				{
					YieldTypes eYield = (YieldTypes) iYield;
					int iYieldRequired = kOwner.getYieldEquipmentAmount(eProfession, eYield);
					if (iYieldRequired > 0)
					{
						bCanHaveProfession = true;
						if (eCurrentProfession != NO_PROFESSION)
						{
							iYieldRequired -= kOwner.getYieldEquipmentAmount(eCurrentProfession, eYield);
						}

						if (iYieldRequired > 0 && aYields[iYield] == 0)
						{
							bCanHaveProfession = false;
							break;
						}
					}
				}

				if (bCanHaveProfession)
				{
					aProfessions.push_back(eProfession);
				}
			}
		}
	}

	if (aProfessions.empty())
	{
		return false;
	}

	ProfessionTypes eProfession = aProfessions[GC.getGameINLINE().getSorenRandNum(aProfessions.size(), "Choose raid weapons")];
	setProfession(eProfession);

	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		YieldTypes eYield = (YieldTypes) iYield;
		int iYieldRequired = kOwner.getYieldEquipmentAmount(eProfession, eYield);
		if (eCurrentProfession != NO_PROFESSION)
		{
			iYieldRequired -= kOwner.getYieldEquipmentAmount(eCurrentProfession, eYield);
		}

		if (iYieldRequired > 0)
		{
			aYields[iYield] = iYieldRequired;
		}
		else
		{
			aYields[iYield] = 0;
		}
	}

	return true;

}

bool CvUnit::raidWeapons(CvCity* pCity)
{
	if (!isNative())
	{
	    ///TKs Med 1.2
	    if (isBarbarian())
	    {
	        if (m_pUnitInfo->isAnimal())
	        {
                return false;
	        }
	    }
	    else
	    {
	        return false;
	    }
	}
	///TKe

	if (!isEnemy(pCity->getTeam()))
	{
		return false;
	}

	if (GC.getGameINLINE().getSorenRandNum(100, "Weapons raid") < pCity->getDefenseModifier())
	{
		return false;
	}

	std::vector<int> aYields(NUM_YIELD_TYPES);
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		aYields[iYield] = pCity->getYieldStored((YieldTypes) iYield);
	}

	if (!raidWeapons(aYields))
	{
		return false;
	}

	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		YieldTypes eYield = (YieldTypes) iYield;
		if (aYields[iYield] > 0)
		{
			pCity->changeYieldStored(eYield, -aYields[iYield]);

			CvWString szString = gDLL->getText("TXT_KEY_GOODS_RAIDED", GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey(), pCity->getNameKey(), aYields[iYield], GC.getYieldInfo(eYield).getTextKeyWide());
			gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_UNITCAPTURE", MESSAGE_TYPE_INFO, GC.getYieldInfo(eYield).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pCity->getX_INLINE(), pCity->getY_INLINE());
			gDLL->getInterfaceIFace()->addMessage(pCity->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_UNITCAPTURE", MESSAGE_TYPE_INFO, GC.getYieldInfo(eYield).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pCity->getX_INLINE(), pCity->getY_INLINE());
		}
	}
	return true;
}

bool CvUnit::raidWeapons(CvUnit* pUnit)
{
	if (!isNative())
	{
	    ///TKs Med 1.2
	    if (isBarbarian())
	    {
	        if (m_pUnitInfo->isAnimal())
	        {
                return false;
	        }
	    }
	    else
	    {
	        return false;
	    }
	}
	///TKe

	FAssert(pUnit->isDead());

	if (!isEnemy(pUnit->getTeam()))
	{
		return false;
	}

	std::vector<int> aYields(NUM_YIELD_TYPES, 0);
	CvPlayer& kOwner = GET_PLAYER(pUnit->getOwnerINLINE());
	if (kOwner.hasContentsYieldEquipmentAmountSecure(pUnit->getProfession())) // cache CvPlayer::getYieldEquipmentAmount - Nightinggale
	{
		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			aYields[iYield] += kOwner.getYieldEquipmentAmount(pUnit->getProfession(), (YieldTypes) iYield);
		}
	}

	if (!raidWeapons(aYields))
	{
		return false;
	}

	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		YieldTypes eYield = (YieldTypes) iYield;
		if (aYields[iYield] > 0)
		{
			CvWString szString = gDLL->getText("TXT_KEY_WEAPONS_CAPTURED", GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey(), pUnit->getNameOrProfessionKey(), aYields[iYield], GC.getYieldInfo(eYield).getTextKeyWide());
			gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_UNITCAPTURE", MESSAGE_TYPE_INFO, GC.getYieldInfo(eYield).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pUnit->getX_INLINE(), pUnit->getY_INLINE());
			gDLL->getInterfaceIFace()->addMessage(pUnit->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_UNITCAPTURE", MESSAGE_TYPE_INFO, GC.getYieldInfo(eYield).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pUnit->getX_INLINE(), pUnit->getY_INLINE());
		}
	}
	return true;
}

bool CvUnit::raidGoods(CvCity* pCity)
{
	if (!isNative())
	{
	    ///TKs Med 1.2
	    if (isBarbarian())
	    {
	        if (m_pUnitInfo->isAnimal())
	        {
                return false;
	        }
	    }
	    else
	    {
	        return false;
	    }
	}
	///TKe
///TKe
	if (!isEnemy(pCity->getTeam()))
	{
		return false;
	}

	if (GC.getGameINLINE().getSorenRandNum(100, "Goods raid") < pCity->getDefenseModifier())
	{
		return false;
	}

	std::vector<YieldTypes> aYields;
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		YieldTypes eYield = (YieldTypes) iYield;
		if (pCity->getYieldStored(eYield) > 0 && GC.getYieldInfo(eYield).isCargo())
		{
			aYields.push_back(eYield);
		}
	}

	if (aYields.empty())
	{
		return false;
	}

	YieldTypes eYield = aYields[GC.getGameINLINE().getSorenRandNum(aYields.size(), "Choose raid goods")];
	int iYieldsStolen = std::min(pCity->getYieldStored(eYield), GC.getGameINLINE().getCargoYieldCapacity() * GC.getXMLval(XML_NATIVE_GOODS_RAID_PERCENT) / 100);

	FAssert(iYieldsStolen > 0);
	if (iYieldsStolen <= 0)
	{
		return false;
	}

	pCity->changeYieldStored(eYield, -iYieldsStolen);

	GET_TEAM(getTeam()).AI_changeDamages(pCity->getTeam(), -GET_PLAYER(getOwnerINLINE()).AI_yieldValue(eYield, true, iYieldsStolen));

	CvCity* pHomeCity = getHomeCity();
	if (pHomeCity == NULL)
	{
		pHomeCity = GC.getMapINLINE().findCity(pCity->getX_INLINE(), pCity->getY_INLINE(), getOwnerINLINE());
	}
	if (pHomeCity != NULL)
	{
		pHomeCity->changeYieldStored(eYield, iYieldsStolen);
	}

	CvWString szString = gDLL->getText("TXT_KEY_GOODS_RAIDED", GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey(), pCity->getNameKey(), iYieldsStolen, GC.getYieldInfo(eYield).getTextKeyWide());
	gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_UNITCAPTURE", MESSAGE_TYPE_INFO, GC.getYieldInfo(eYield).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pCity->getX_INLINE(), pCity->getY_INLINE());
	gDLL->getInterfaceIFace()->addMessage(pCity->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_UNITCAPTURE", MESSAGE_TYPE_INFO, GC.getYieldInfo(eYield).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pCity->getX_INLINE(), pCity->getY_INLINE());
	return true;
}
///TKs Invention Core Mod v 1.0

UnitTypes CvUnit::getConvertToUnit() const
{
	return m_ConvertToUnit;
}

void CvUnit::setConvertToUnit(UnitTypes eConvertToUnit)
{
	if(m_ConvertToUnit != eConvertToUnit)
	{
		m_ConvertToUnit = eConvertToUnit;
		//reloadEntity();
	}
}

bool CvUnit::doUnitPilgram()
{
    CvCity* ePlotCity = plot()->getPlotCity();
    if (ePlotCity == NULL)
    {
        return false;
    }

    if (ePlotCity->getOwner() == getOwner())
    {
        return false;
    }

    if (ePlotCity->isHasBuildingClass((BuildingClassTypes)GC.getXMLval(XML_DEFAULT_SHRINE_CLASS)))
    {
        CvCity* pHome = getHomeCity();
        int iDistanceMod = 1;
        if (pHome != NULL)
        {
            iDistanceMod = ::plotDistance(getX_INLINE(), getY_INLINE(), pHome->getX_INLINE(), pHome->getY_INLINE());
            iDistanceMod *= GC.getXMLval(XML_PILGRAM_OFFER_GOLD_DISTANCE_MOD);

        }
        int iRand = GC.getGameINLINE().getSorenRandNum(GC.getXMLval(XML_PILGRAM_OFFER_GOLD), "Random Pilgram 1") + 5;
        iRand = iRand + (iRand * iDistanceMod / 100);
       //Tks Civics
		int iRandMod = 0;
		for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
		{
			if (GET_PLAYER(ePlotCity->getOwner()).getCivic((CivicOptionTypes)iI) != NO_CIVIC)
			{
				iRandMod += GC.getCivicInfo(GET_PLAYER(ePlotCity->getOwner()).getCivic((CivicOptionTypes)iI)).getPilgramYieldPercent();
			}
		}
		iRand = iRand + (iRand * iRandMod / 100);
        GET_PLAYER(ePlotCity->getOwner()).changeGold(iRand);
        ePlotCity->changeCulture(ePlotCity->getOwner(), (iRand / 2), true);
        CvWString szBuffer = gDLL->getText("TXT_KEY_UNIT_PILGRAMS_ARRIVE", ePlotCity->getNameKey(), iRand);
        gDLL->getInterfaceIFace()->addMessage(ePlotCity->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getUnitInfo(getUnitType()).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getX_INLINE(), getY_INLINE(), true, true);
        plot()->addCrumbs(10);
        kill(true);
        return true;
    }

    return false;
}

CvPlot* CvUnit::getTravelPlot() const
{
	return GC.getMapINLINE().plotSorenINLINE(m_iTravelPlotX, m_iTravelPlotY);
}
void CvUnit::setTravelPlot()
{
	m_iTravelPlotX = getX_INLINE();
	m_iTravelPlotY = getY_INLINE();
}
CvPlot* CvUnit::findNearestValidMarauderPlot(CvCity* pSpawnCity, CvCity* pVictimCity, bool bNoCitySpawn, bool bMustBeSameArea)
{
	FAssertMsg(!isAttacking(), "isAttacking did not return false as expected");
	FAssertMsg(!isFighting(), "isFighting did not return false as expected");

	//CvCity* pNearestCity = GC.getMapINLINE().findCity(pCity->getX_INLINE(), pCity->getY_INLINE(), getOwnerINLINE());
	bool bSameArea = true;
	if (pSpawnCity == NULL || pVictimCity == NULL)
	{
	    bSameArea = false;
	}
	else if (pSpawnCity != NULL && pVictimCity != NULL)
	{
	    bSameArea = pSpawnCity->area() == pVictimCity->area();
	}

	int iBestValue = MAX_INT;
	CvPlot* pBestPlot = NULL;

	for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);
		if (pLoopPlot->isValidDomainForLocation(*this))
		{
		    if ((bMustBeSameArea && bSameArea) || !bMustBeSameArea)
		    {
                if ((pLoopPlot->isCity() && !bNoCitySpawn) || !pLoopPlot->isCity())
                {

                    int iValue = (::plotDistance(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), pSpawnCity->getX_INLINE(), pSpawnCity->getY_INLINE()));

                    if (iValue < iBestValue)
                    {
                        iBestValue = iValue;
                        pBestPlot = pLoopPlot;
                    }

                }
		    }
		}
	}

//	bool bValid = true;
//	if (pBestPlot != NULL)
//	{
//		return pBestPlot;
//	}
//	else
//	{
//		kill(false);
//		bValid = false;
//	}

	return pBestPlot;
}

bool CvUnit::doRansomKnight(int iKillerPlayerID)
{
// TODO (Teddy Mac#1#): Restart Ransom Knights\

    int iOwnerID = GET_PLAYER(getOwnerINLINE()).getID();
    PlayerTypes eCapturePlayer = (PlayerTypes)iKillerPlayerID;
    CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_RANSOM_KNIGHT, getID(), iKillerPlayerID, iOwnerID);
    if (pInfo)
    {
        if (isHuman())
        {
            gDLL->getInterfaceIFace()->addPopup(pInfo, getOwnerINLINE(), true);
        }
        else if (GET_PLAYER(eCapturePlayer).isHuman())
        {
            gDLL->getInterfaceIFace()->addPopup(pInfo, eCapturePlayer, true);
        }

    }
    return true;
}
///Tks Return Home
bool CvUnit::checkHasHomeCity(bool bTestVisible) const
{
    int iUnitId = getID();

    CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_RETURN_HOME, iUnitId);
    if (pInfo)
    {
        gDLL->getInterfaceIFace()->addPopup(pInfo, getOwnerINLINE(), true);

    }
    return false;
}

bool CvUnit::canHireGuard(bool bTestVisible)
{

    if (plot()->isCity())
    {
        if (!isOnMap())
        {
            return false;
        }

		if (canSpeakWithChief(plot()))
		{
			return false;
		}

        if (isDelayedDeath())
        {
            return false;
        }

         CvCity* ePlotCity = plot()->getPlotCity();
         if (ePlotCity != NULL)
         {
             if (!ePlotCity->isNative())
             {
                 return false;
             }
             else
             {
                 if (!isUnarmed())
                 {
                     if (!m_pUnitInfo->isTreasure())
                     {
                        return false;
                     }
                 }
                 if (getDomainType() != DOMAIN_LAND)
                 {
                     return false;
                 }
                 if (m_pUnitInfo->isMechUnit())
                 {
                     return false;
                 }
                 //if (m_pUnitInfo->isTreasure())
                 //{
                     //return false;
                 //}

                 if (!bTestVisible)
                 {
                     if (GET_PLAYER(getOwnerINLINE()).getGold() < GC.getXMLval(XML_HIRE_GUARD_COST))
                     {
                         return false;
                     }

                    if (movesLeft() == 0)
                    {
                        return false;
                    }
                 }
             }
         }
    }
    else
    {
        return false;
    }


    return true;
}
void CvUnit::hireGuard()
{
    if (!canHireGuard(false))
    {
        return;
    }
    CvCity* ePlotCity = plot()->getPlotCity();
    PromotionTypes ePromotion = (PromotionTypes) GC.getXMLval(XML_HIRE_GUARD_PROMOTION);

    if (getEscortPromotion() == ePromotion)
    {
        setHasRealPromotion(ePromotion, false);
        setEscortPromotion(NO_PROMOTION);
        for (int iPromotion = 0; iPromotion < GC.getNumPromotionInfos(); ++iPromotion)
        {
            if (isHasPromotion((PromotionTypes) iPromotion))
            {
                if (GC.getPromotionInfo((PromotionTypes) iPromotion).getEscortUnitClass() != NO_UNITCLASS)
                {
                    UnitTypes eEscortUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits((UnitClassTypes)GC.getPromotionInfo((PromotionTypes) iPromotion).getEscortUnitClass());

                    if (eEscortUnit != NO_UNIT)
                    {
                        m_eLeaderUnitType = eEscortUnit;
                        reloadEntity();
                    }
                }
            }
        }
        //reloadEntity();
        return;
    }

    if (ePromotion != NO_PROMOTION && GC.getPromotionInfo(ePromotion).getEscortUnitClass() != NO_UNITCLASS)
    {
         UnitTypes eEscortUnit = (UnitTypes)GC.getCivilizationInfo(GET_PLAYER(ePlotCity->getOwnerINLINE()).getCivilizationType()).getCivilizationUnits((UnitClassTypes)GC.getPromotionInfo(ePromotion).getEscortUnitClass());
        if (eEscortUnit != NO_UNIT)
        {
            GET_PLAYER(getOwnerINLINE()).changeGold(-GC.getXMLval(XML_HIRE_GUARD_COST));

            setHasRealPromotion(ePromotion, true);
            setEscortPromotion(ePromotion);
            //reloadEntity();
        }
    }

}

int CvUnit::getEscortPromotion() const
{
	return m_iEscortPromotion;
}
void CvUnit::setEscortPromotion(PromotionTypes ePromotion)
{
    m_iEscortPromotion = ePromotion;
}

bool CvUnit::canCollectTaxes(bool bTestVisible)
{
    CvCity* ePlotCity = plot()->getPlotCity();
    if (ePlotCity == NULL)
    {
        return false;
    }

    if (ePlotCity->getVassalOwner() != getOwner())
    {
        return false;
    }

    ProfessionTypes eProfession = getProfession();

    if (eProfession == NO_PROFESSION)
    {
        return false;
    }

    if (GC.getProfessionInfo(eProfession).getTaxCollectRate() <= 0)
    {
        return false;
    }
    CvUnit* pLoopUnit;
    CLLNode<IDInfo>* pUnitNode = plot()->headUnitNode();;
    while (pUnitNode != NULL)
    {
        pLoopUnit = ::getUnit(pUnitNode->m_data);
        pUnitNode = plot()->nextUnitNode(pUnitNode);

        if (pLoopUnit != this && pLoopUnit->getGroup()->getActivityType() == ACTIVITY_COLLECT_TAXES)
        {
            return false;
        }
    }

    return true;
}
bool CvUnit::canCallBanners(bool bTestVisible)
{
//    CvCity* ePlotCity = plot()->getPlotCity();
//    if (ePlotCity == NULL)
//    {
//        return false;
//    }
//
//    if (ePlotCity->getVassalOwner() != getOwner())
//    {
//        return false;
//    }

    return false;
}
bool CvUnit::canBuildTradingPost(bool bTestVisible)
{
    CvCity* pCity = plot()->getPlotCity();
    if (pCity == NULL)
    {
        return false;
    }

	if (getDomainType() != DOMAIN_LAND)
	{
		return false;
	}

	// TODO check this code
    BuildingTypes eBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(GC.getXMLval(XML_NATIVE_TRADING_TRADEPOST));
    if (!GET_PLAYER(getOwner()).canConstruct(eBuilding, false, false, true))
    {
        return false;
    }

    if (!canTradeYield(plot()))
    {
        return false;
    }

    if (isHuman() && canSpeakWithChief(plot()))
    {
        return false;
    }

    if (pCity->isTradePostBuilt(getTeam()))
    {
        return false;
    }

    if (!bTestVisible)
    {
        if (GET_PLAYER(getOwner()).getGold() < GC.getXMLval(XML_ESTABLISH_TRADEPOST_COST))
        {
            return false;
        }
    }

    return true;

}

void CvUnit::buildTradingPost(bool bTestVisible)
{
    if (!canBuildTradingPost(bTestVisible))
    {
        return;
    }
    GET_PLAYER(getOwnerINLINE()).changeGold(-GC.getXMLval(XML_ESTABLISH_TRADEPOST_COST));
    CvCity* pCity = plot()->getPlotCity();
    pCity->setTradePostBuilt(getTeam(), true);
    //BuildingTypes eBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(GC.getXMLval(XML_NATIVE_TRADING_TRADEPOST));
    //pCity->setHasRealBuilding(eBuilding, true);
	//Civic Reset
	GET_PLAYER(getOwnerINLINE()).changeTradingPostCount(pCity->getOwnerINLINE(), 1);
	CvPlayer& kPlayer = GET_PLAYER(getOwnerINLINE());
	kPlayer.resetConnectedPlayerYieldBonus();
    CvWString szBuffer = gDLL->getText("TXT_KEY_ESTABLISH_TRADEPOST_MESSAGE", pCity->getNameKey());
    //gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BUILD_BANK", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE());
	//Tks Civic Screen
	if (GET_PLAYER(getOwnerINLINE()).getTradingPostHide() > 0)
	{
		setUnitTravelState(UNIT_TRAVEL_STATE_HIDE_UNIT, false);
		setUnitTravelTimer(GET_PLAYER(getOwnerINLINE()).getTradingPostHide());
	}
	else
	{
		CLLNode<IDInfo>* pUnitNode =  plot()->headUnitNode();
		CvPlayer& kPlayerEurope = GET_PLAYER(GET_PLAYER(getOwnerINLINE()).getParent());
		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = plot()->nextUnitNode(pUnitNode);

			if (pLoopUnit->getTransportUnit() == this)
			{						
				int iPrice = kPlayerEurope.getYieldBuyPrice(pLoopUnit->getYield());
				iPrice = iPrice * pLoopUnit->getYieldStored() / 2;
				GET_PLAYER(getOwnerINLINE()).changeGold(iPrice);
				szBuffer.append(gDLL->getText("TXT_KEY_TRADING_POST_GOODS_SOLD", iPrice));
				//szBuffer.append(szFirstBuffer);
			}
		}

		kill(true);
	}
	gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BUILD_BANK", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE());
	//Tke
}


bool CvUnit::canBuildHome(bool bTestVisible)
{
    if (isAddedFreeBuilding())
    {
        return false;
    }
    if (isNative() || GET_PLAYER(getOwnerINLINE()).isEurope())
    {
        return false;
    }
    BuildingTypes eBuilding = NO_BUILDING;
    if (m_pUnitInfo->getFreeBuildingClass() != NO_BUILDINGCLASS)
    {
		eBuilding = ((BuildingTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings((BuildingClassTypes)m_pUnitInfo->getFreeBuildingClass())));
        if (eBuilding == NO_BUILDING)
        {
            return false;
        }
    }
    else
    {
        return false;
    }
    if (!bTestVisible)
    {
        if (!canJoinCity(plot()))
        {
            return false;
        }
        //return true;
        CvCity* pCity = plot()->getPlotCity();
        if (!pCity->canConstruct(eBuilding, false, true, false))
        {
            return false;
        }
    }

    return true;
}

void CvUnit::buildHome()
{
    if (!canBuildHome(plot()))
	{
		return;
	}
	BuildingTypes eBuilding = ((BuildingTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings((BuildingClassTypes)m_pUnitInfo->getFreeBuildingClass())));
	CvCity* ePlotCity = plot()->getPlotCity();
    if (ePlotCity != NULL && eBuilding != NO_BUILDING)
    {
        ePlotCity->setHasRealBuilding(eBuilding, true);
        setAddedFreeBuilding(true);
        PromotionTypes eHomeBoy = (PromotionTypes) GC.getXMLval(XML_PROMOTION_BUILD_HOME);
        setHasRealPromotion(eHomeBoy, false);
        CvWString szBuffer = gDLL->getText("TXT_KEY_UNIT_BUILT_HOME", getNameKey(), GC.getBuildingInfo(eBuilding).getDescription(), ePlotCity->getNameKey());
        gDLL->getInterfaceIFace()->addMessage(ePlotCity->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getUnitInfo(getUnitType()).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getX_INLINE(), getY_INLINE(), true, true);
    }
    if (isHuman())
    {
        joinCity();
    }

}
bool CvUnit::collectTaxes()
{
    if (!canCollectTaxes(false))
    {
        return false;
    }
    CvCity* pCity = plot()->getPlotCity();
    ProfessionTypes eProfession = getProfession();
    int iTaxCollected = GC.getProfessionInfo(eProfession).getTaxCollectRate() * pCity->getPopulation();
    CvWString szBuffer = gDLL->getText("TXT_KEY_COLLECTED_TAXES", iTaxCollected, pCity->getNameKey());
    gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BUILD_BANK", MESSAGE_TYPE_MINOR_EVENT, getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE(), true, true);
    GET_PLAYER(getOwner()).changeGold(iTaxCollected);
    finishMoves();

    return true;
}

void CvUnit::callBanners()
{

}
int CvUnit::getPlotWorkedBonus() const
{
	return m_iPlotWorkedBonus;
}

void CvUnit::changePlotWorkedBonus(int iChange)
{
	if (iChange != 0)
	{
		m_iPlotWorkedBonus += iChange;
	}
}
int CvUnit::getBuildingWorkedBonus() const
{
	return m_iBuildingWorkedBonus;
}
void CvUnit::changeBuildingWorkedBonus(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingWorkedBonus += iChange;
	}
}
int CvUnit::getInvisibleTimer() const
{
	return m_iInvisibleTimer;
}

void CvUnit::setInvisibleTimer(int iNewValue)
{
	if (iNewValue != getInvisibleTimer())
	{
		m_iInvisibleTimer = iNewValue;

		setInfoBarDirty(true);
	}
}

void CvUnit::changeInvisibleTimer(int iChange)
{
	if (iChange != 0)
	{
		setInvisibleTimer(std::max(0, getInvisibleTimer() + iChange));
	}
}
//int CvUnit::getEventTimer() const
//{
//	return m_iEventTimer;
//}
//
//void CvUnit::setEventTimer(int iNewValue)
//{
//	if (iNewValue != getEventTimer())
//	{
//		m_iEventTimer = iNewValue;
//
//		//setInfoBarDirty(true);
//	}
//}
//
//void CvUnit::changeEventTimer(int iChange)
//{
//	if (iChange != 0)
//	{
//		setImmobileTimer(std::max(0, getImmobileTimer() + iChange));
//	}
//}
///TKe

/// Expert working - start - Nightinggale
bool CvUnit::isCitizenExpertWorking() const
{
	// tell if a citizen is producing yields where the unit type gains a bonus
	ProfessionTypes eProfession = this->getProfession();
	if (eProfession != NO_PROFESSION)
	{
		CvProfessionInfo& kProfession = GC.getProfessionInfo(eProfession);
		if (kProfession.isCitizen())
		{
			CvUnitInfo& kUnit = getUnitInfo();

			bool bWater = kProfession.isWater();
			if (kProfession.isWorkPlot())
			{
				if(kProfession.isWater())
				{
					if (!kUnit.isWaterYieldChanges())
					{
						return false;
					}
				}
				else if (!kUnit.isLandYieldChanges())
				{
					return false;
				}
			}

			for (int iIndex = 0; iIndex < kProfession.getNumYieldsProduced(); iIndex++)
			{
				int iYield = kProfession.getYieldsProduced(iIndex);
				if (kUnit.getYieldChange(iYield) > 0 || kUnit.getYieldModifier(iYield) > 0)
				{
					return true;
				}
			}
		}
	}
	return false;
}
/// Expert working - end - Nightinggale

/// unit promotion effect cache - start - Nightinggale
void CvUnit::updatePromotionCache()
{
	for (int iPromotion = 0; iPromotion < GC.getNumPromotionInfos(); iPromotion++)
	{
		PromotionTypes ePromotion = (PromotionTypes)iPromotion;
		if (isHasPromotion(ePromotion))
		{
			processPromotion(ePromotion, 1, true);
		}
	}

	// update profession cache for variables set by both profession and promotion
	if (getProfession() != NO_PROFESSION)
	{
		CvProfessionInfo& kProfession = GC.getProfessionInfo(getProfession());
		changeExtraMoves(kProfession.getMovesChange());
	}
}

void CvUnit::reclaimCacheMemory()
{
	m_ja_iTerrainDoubleMoveCount.resetContent();
	m_ja_iFeatureDoubleMoveCount.resetContent();
	m_ja_iExtraTerrainAttackPercent.resetContent();
	m_ja_iExtraTerrainDefensePercent.resetContent();
	m_ja_iExtraFeatureAttackPercent.resetContent();
	m_ja_iExtraFeatureDefensePercent.resetContent();
	m_ja_iExtraUnitClassAttackModifier.resetContent();
	m_ja_iExtraUnitClassDefenseModifier.resetContent();
	m_ja_iExtraUnitCombatModifier.resetContent();
}
/// unit promotion effect cache - end - Nightinggale
