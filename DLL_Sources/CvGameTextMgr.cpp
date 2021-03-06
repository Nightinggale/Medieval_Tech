//---------------------------------------------------------------------------------------
//
//  *****************   Civilization IV   ********************
//
//  FILE:    CvGameTextMgr.cpp
//
//  PURPOSE: Interfaces with GameText XML Files to manage the paths of art files
//
//---------------------------------------------------------------------------------------
//  Copyright (c) 2004 Firaxis Games, Inc. All rights reserved.
//---------------------------------------------------------------------------------------

#include "CvGameCoreDLL.h"
#include "CvGameTextMgr.h"
#include "CvGameCoreUtils.h"
#include "CvDLLUtilityIFaceBase.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLSymbolIFaceBase.h"
#include "CvInfos.h"
#include "CvXMLLoadUtility.h"
#include "CvCity.h"
#include "CvPlayerAI.h"
#include "CvTeamAI.h"
#include "CvGameAI.h"
#include "CvSelectionGroup.h"
#include "CvMap.h"
#include "CvArea.h"
#include "CvPlot.h"
#include "CvUnitAI.h"
#include "CvPopupInfo.h"
#include "FProfiler.h"
#include "CyArgsList.h"
#include "CvDLLPythonIFaceBase.h"

#include "CvInfoProfessions.h"

int shortenID(int iId)
{
	return iId;
}

// For displaying Asserts and error messages
static char* szErrorMsg;

//----------------------------------------------------------------------------
//
//	FUNCTION:	GetInstance()
//
//	PURPOSE:	Get the instance of this class.
//
//----------------------------------------------------------------------------
CvGameTextMgr& CvGameTextMgr::GetInstance()
{
	static CvGameTextMgr gs_GameTextMgr;
	return gs_GameTextMgr;
}

//----------------------------------------------------------------------------
//
//	FUNCTION:	CvGameTextMgr()
//
//	PURPOSE:	Constructor
//
//----------------------------------------------------------------------------
CvGameTextMgr::CvGameTextMgr()
{

}

CvGameTextMgr::~CvGameTextMgr()
{
}

//----------------------------------------------------------------------------
//
//	FUNCTION:	Initialize()
//
//	PURPOSE:	Allocates memory
//
//----------------------------------------------------------------------------
void CvGameTextMgr::Initialize()
{

}

//----------------------------------------------------------------------------
//
//	FUNCTION:	DeInitialize()
//
//	PURPOSE:	Clears memory
//
//----------------------------------------------------------------------------
void CvGameTextMgr::DeInitialize()
{
	for(int i=0;i<(int)m_apbPromotion.size();i++)
	{
		delete [] m_apbPromotion[i];
	}
}

//----------------------------------------------------------------------------
//
//	FUNCTION:	Reset()
//
//	PURPOSE:	Accesses CvXMLLoadUtility to clean global text memory and
//				reload the XML files
//
//----------------------------------------------------------------------------
void CvGameTextMgr::Reset()
{
	CvXMLLoadUtility pXML;
	pXML.LoadGlobalText();
}


// Returns the current language
int CvGameTextMgr::getCurrentLanguage()
{
	return gDLL->getCurrentLanguage();
}

void CvGameTextMgr::setYearStr(CvWString& szString, int iGameTurn, bool bSave, CalendarTypes eCalendar, int iStartYear, GameSpeedTypes eSpeed)
{
	int iTurnYear = getTurnYearForGame(iGameTurn, iStartYear, eCalendar, eSpeed);

	if (iTurnYear < 0)
	{
		if (bSave)
		{
			szString = gDLL->getText("TXT_KEY_TIME_BC_SAVE", CvWString::format(L"%04d", -iTurnYear).GetCString());
		}
		else
		{
			szString = gDLL->getText("TXT_KEY_TIME_BC", -(iTurnYear));
		}
	}
	else if (iTurnYear > 0)
	{
		if (bSave)
		{
			szString = gDLL->getText("TXT_KEY_TIME_AD_SAVE", CvWString::format(L"%04d", iTurnYear).GetCString());
		}
		else
		{
			szString = gDLL->getText("TXT_KEY_TIME_AD", iTurnYear);
		}
	}
	else
	{
		if (bSave)
		{
			szString = gDLL->getText("TXT_KEY_TIME_AD_SAVE", L"0001");
		}
		else
		{
			szString = gDLL->getText("TXT_KEY_TIME_AD", 1);
		}
	}
}


void CvGameTextMgr::setDateStr(CvWString& szString, int iGameTurn, bool bSave, CalendarTypes eCalendar, int iStartYear, GameSpeedTypes eSpeed)
{
	CvWString szYearBuffer;
	CvWString szWeekBuffer;

	setYearStr(szYearBuffer, iGameTurn, bSave, eCalendar, iStartYear, eSpeed);

	switch (eCalendar)
	{
	case CALENDAR_DEFAULT:
		if (0 == (getTurnMonthForGame(iGameTurn + 1, iStartYear, eCalendar, eSpeed) - getTurnMonthForGame(iGameTurn, iStartYear, eCalendar, eSpeed)) % GC.getNumMonthInfos())
		{
			szString = szYearBuffer;
		}
		else
		{
			int iMonth = getTurnMonthForGame(iGameTurn, iStartYear, eCalendar, eSpeed) % GC.getNumMonthInfos();
			if(iMonth < 0)
			{
				iMonth += GC.getNumMonthInfos();
			}

			if (bSave)
			{
				szString = (szYearBuffer + "-" + GC.getMonthInfo((MonthTypes)iMonth).getDescription());
			}
			else
			{
				szString = (GC.getMonthInfo((MonthTypes)iMonth).getDescription() + CvString(", ") + szYearBuffer);
			}
		}
		break;
	case CALENDAR_YEARS:
	case CALENDAR_BI_YEARLY:
		szString = szYearBuffer;
		break;

	case CALENDAR_TURNS:
		szString = gDLL->getText("TXT_KEY_TIME_TURN", (iGameTurn + 1));
		break;

	case CALENDAR_SEASONS:
		if (bSave)
		{
			szString = (szYearBuffer + "-" + GC.getSeasonInfo((SeasonTypes)(iGameTurn % GC.getNumSeasonInfos())).getDescription());
		}
		else
		{
			szString = (GC.getSeasonInfo((SeasonTypes)(iGameTurn % GC.getNumSeasonInfos())).getDescription() + CvString(", ") + szYearBuffer);
		}
		break;

	case CALENDAR_MONTHS:
		if (bSave)
		{
			szString = (szYearBuffer + "-" + GC.getMonthInfo((MonthTypes)(iGameTurn % GC.getNumMonthInfos())).getDescription());
		}
		else
		{
			szString = (GC.getMonthInfo((MonthTypes)(iGameTurn % GC.getNumMonthInfos())).getDescription() + CvString(", ") + szYearBuffer);
		}
		break;

	case CALENDAR_WEEKS:
		szWeekBuffer = gDLL->getText("TXT_KEY_TIME_WEEK", ((iGameTurn % GC.getXMLval(XML_WEEKS_PER_MONTHS)) + 1));

		if (bSave)
		{
			szString = (szYearBuffer + "-" + GC.getMonthInfo((MonthTypes)((iGameTurn / GC.getXMLval(XML_WEEKS_PER_MONTHS)) % GC.getNumMonthInfos())).getDescription() + "-" + szWeekBuffer);
		}
		else
		{
			szString = (szWeekBuffer + ", " + GC.getMonthInfo((MonthTypes)((iGameTurn / GC.getXMLval(XML_WEEKS_PER_MONTHS)) % GC.getNumMonthInfos())).getDescription() + ", " + szYearBuffer);
		}
		break;

	default:
		FAssert(false);
	}
}


void CvGameTextMgr::setTimeStr(CvWString& szString, int iGameTurn, bool bSave)
{
	setDateStr(szString, iGameTurn, bSave, GC.getGameINLINE().getCalendar(), GC.getGameINLINE().getStartYear(), GC.getGameINLINE().getGameSpeedType());
}


void CvGameTextMgr::setInterfaceTime(CvWString& szString, PlayerTypes ePlayer)
{
	CvWString szTempBuffer;

	clear(szString);

	setTimeStr(szTempBuffer, GC.getGameINLINE().getGameTurn(), false);
	szString += CvWString(szTempBuffer);
}


void CvGameTextMgr::setGoldStr(CvWString& szString, PlayerTypes ePlayer)
{
	if (GET_PLAYER(ePlayer).getGold() < 0)
	{
		szString.Format(SETCOLR L"%d" SETCOLR, TEXT_COLOR("COLOR_NEGATIVE_TEXT"), GET_PLAYER(ePlayer).getGold());
	}
	else
	{
		szString.Format(L"%d", GET_PLAYER(ePlayer).getGold());
	}
}

void CvGameTextMgr::setOOSSeeds(CvWString& szString, PlayerTypes ePlayer)
{
	if (GET_PLAYER(ePlayer).isHuman())
	{
		int iNetID = GET_PLAYER(ePlayer).getNetID();
		if (gDLL->isConnected(iNetID))
		{
			szString = gDLL->getText("TXT_KEY_PLAYER_OOS", gDLL->GetSyncOOS(iNetID), gDLL->GetOptionsOOS(iNetID));
		}
	}
}

void CvGameTextMgr::setNetStats(CvWString& szString, PlayerTypes ePlayer)
{
	if (ePlayer != GC.getGameINLINE().getActivePlayer())
	{
		if (GET_PLAYER(ePlayer).isHuman())
		{
			if (gDLL->getInterfaceIFace()->isNetStatsVisible())
			{
				int iNetID = GET_PLAYER(ePlayer).getNetID();
				if (gDLL->isConnected(iNetID))
				{
					szString = gDLL->getText("TXT_KEY_MISC_NUM_MS", gDLL->GetLastPing(iNetID));
				}
				else
				{
					szString = gDLL->getText("TXT_KEY_MISC_DISCONNECTED");
				}
			}
		}
		else
		{
			szString = gDLL->getText("TXT_KEY_MISC_AI");
		}
	}
}


void CvGameTextMgr::setMinimizePopupHelp(CvWString& szString, const CvPopupInfo & info)
{
	switch (info.getButtonPopupType())
	{
	case BUTTONPOPUP_CHOOSEPRODUCTION:
		{
			CvCity* pCity = GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getCity(info.getData1());
			if (pCity != NULL)
			{
				UnitTypes eTrainUnit = NO_UNIT;
				BuildingTypes eConstructBuilding = NO_BUILDING;

				switch (info.getData2())
				{
				case (ORDER_TRAIN):
					eTrainUnit = (UnitTypes)info.getData3();
					break;
				case (ORDER_CONSTRUCT):
					eConstructBuilding = (BuildingTypes)info.getData3();
					break;
				default:
					break;
				}

				if (eTrainUnit != NO_UNIT)
				{
					szString += gDLL->getText("TXT_KEY_MINIMIZED_CHOOSE_PRODUCTION_UNIT", GC.getUnitInfo(eTrainUnit).getTextKeyWide(), pCity->getNameKey());
				}
				else if (eConstructBuilding != NO_BUILDING)
				{
					szString += gDLL->getText("TXT_KEY_MINIMIZED_CHOOSE_PRODUCTION_BUILDING", GC.getBuildingInfo(eConstructBuilding).getTextKeyWide(), pCity->getNameKey());
				}
				else
				{
					szString += gDLL->getText("TXT_KEY_MINIMIZED_CHOOSE_PRODUCTION", pCity->getNameKey());
				}
			}
		}
		break;
	case BUTTONPOPUP_CHOOSE_EDUCATION:
		{
			CvPlayer& kPlayer = GET_PLAYER(GC.getGameINLINE().getActivePlayer());
			CvCity* pCity = kPlayer.getCity(info.getData1());
			CvUnit* pUnit = kPlayer.getUnit(info.getData2());
			if (pCity != NULL && pUnit != NULL)
			{
				BuildingTypes eSchoolBuilding = pCity->getYieldBuilding(YIELD_EDUCATION);
				if (eSchoolBuilding != NO_BUILDING)
				{
					szString += gDLL->getText("TXT_KEY_MINIMIZED_CHOOSE_EDUCATION", pUnit->getNameOrProfessionKey(), pCity->getNameKey(), GC.getBuildingInfo(eSchoolBuilding).getTextKeyWide());
				}
			}

		}
		break;
	case BUTTONPOPUP_CHOOSE_YIELD_BUILD:
		{
			CvPlayer& kPlayer = GET_PLAYER(GC.getGameINLINE().getActivePlayer());
			CvCity* pCity = kPlayer.getCity(info.getData1());
			if (pCity != NULL)
			{
				szString += gDLL->getText("TXT_KEY_MINIMIZED_CHOOSE_YIELD_BUILD", pCity->getNameKey(), GC.getYieldInfo((YieldTypes) info.getData2()));
			}

		}
		break;
	}
}

void CvGameTextMgr::setUnitHelp(CvWStringBuffer &szString, const CvUnit* pUnit, bool bOneLine, bool bShort)
{
	PROFILE_FUNC();

	CvWString szTempBuffer;
	BuildTypes eBuild;
	int iI;
	bool bShift = gDLL->shiftKey();
	bool bAlt = gDLL->altKey();

	szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR("COLOR_UNIT_TEXT"), pUnit->getNameAndProfession().GetCString());
	szString.append(szTempBuffer);

	if (pUnit->canFight())
	{
		szString.append(L", ");

		if (pUnit->isFighting())
		{
			szTempBuffer.Format(L"?/%d%c", pUnit->baseCombatStr(), gDLL->getSymbolID(STRENGTH_CHAR));
		}
		else if (pUnit->isHurt())
		{
			szTempBuffer.Format(L"%.1f/%d%c", (((float)(pUnit->baseCombatStr() * pUnit->currHitPoints())) / ((float)(pUnit->maxHitPoints()))), pUnit->baseCombatStr(), gDLL->getSymbolID(STRENGTH_CHAR));
		}
		else
		{
			szTempBuffer.Format(L"%d%c", pUnit->baseCombatStr(), gDLL->getSymbolID(STRENGTH_CHAR));
		}
		szString.append(szTempBuffer);
	}

	if (pUnit->maxMoves() > 0)
	{
		szString.append(L", ");
		int iCurrMoves = ((pUnit->movesLeft() / GC.getMOVE_DENOMINATOR()) + (((pUnit->movesLeft() % GC.getMOVE_DENOMINATOR()) > 0) ? 1 : 0));
		if ((pUnit->baseMoves() == iCurrMoves) || (pUnit->getTeam() != GC.getGameINLINE().getActiveTeam()))
		{
			szTempBuffer.Format(L"%d%c", pUnit->baseMoves(), gDLL->getSymbolID(MOVES_CHAR));
		}
		else
		{
			szTempBuffer.Format(L"%d/%d%c", iCurrMoves, pUnit->baseMoves(), gDLL->getSymbolID(MOVES_CHAR));
		}
		szString.append(szTempBuffer);
	}

	if (pUnit->getYield() != NO_YIELD)
	{
		szString.append(L", ");
		szTempBuffer.Format(L"%d%c", pUnit->getYieldStored(), GC.getYieldInfo(pUnit->getYield()).getChar());
		szString.append(szTempBuffer);
		int iValue = GET_PLAYER(pUnit->getOwnerINLINE()).getSellToEuropeProfit(pUnit->getYield(), pUnit->getYieldStored());
		if (iValue > 0)
		{
			szTempBuffer.Format(L" (%d%c)", iValue, gDLL->getSymbolID(GOLD_CHAR));
			szString.append(szTempBuffer);
		}
		///TKs Med
		if (pUnit->isCargo() && (!bShort || bShift))
		{
		    szString.append(NEWLINE);
		    setYieldPriceHelp(szString, pUnit->getOwnerINLINE(), pUnit->getYield());
		}
		if (pUnit->plot()->getPlotCity() != NULL)
		{
			CvCity* pCity = pUnit->plot()->getPlotCity();
			if (pCity->getOwner() == pUnit->getOwner())
			{
				int iYieldStored = pCity->getYieldStored(pUnit->getYield());
				if (iYieldStored > 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_YIELD_STORED_HELP", iYieldStored));
				}
			}
		}
		///Tke
	}
	eBuild = pUnit->getBuildType();

	if (eBuild != NO_BUILD)
	{
		szString.append(L", ");
		szTempBuffer.Format(L"%s (%d)", GC.getBuildInfo(eBuild).getDescription(), pUnit->plot()->getBuildTurnsLeft(eBuild, 0, 0));
		szString.append(szTempBuffer);
	}
    if (gDLL->getChtLvl() > 0)
    {
        if (pUnit->getImmobileTimer() > 0)
        {
            szString.append(L", ");
            szString.append(gDLL->getText("TXT_KEY_UNIT_HELP_IMMOBILE", pUnit->getImmobileTimer()));
        }
    }

	if (GC.getGameINLINE().isDebugMode() && !bAlt && !bShift && (pUnit->AI_getUnitAIType() != NO_UNITAI))
	{
	    szTempBuffer.Format(L" (%s (%d))", GC.getUnitAIInfo(pUnit->AI_getUnitAIType()).getDescription(), pUnit->AI_getUnitAIState());
		szString.append(szTempBuffer);
	}

	if ((pUnit->getTeam() == GC.getGameINLINE().getActiveTeam()) || GC.getGameINLINE().isDebugMode())
	{
		if ((pUnit->getExperience() > 0) && !(pUnit->isFighting()))
		{
			szString.append(gDLL->getText("TXT_KEY_UNIT_HELP_LEVEL", pUnit->getExperience(), pUnit->experienceNeeded()));
		}
	}

	if (pUnit->getOwnerINLINE() != GC.getGameINLINE().getActivePlayer() && !pUnit->getUnitInfo().isHiddenNationality())
	{
		szString.append(L", ");
		szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, GET_PLAYER(pUnit->getOwnerINLINE()).getPlayerTextColorR(), GET_PLAYER(pUnit->getOwnerINLINE()).getPlayerTextColorG(), GET_PLAYER(pUnit->getOwnerINLINE()).getPlayerTextColorB(), GET_PLAYER(pUnit->getOwnerINLINE()).getPlayerTextColorA(), GET_PLAYER(pUnit->getOwnerINLINE()).getName());
		szString.append(szTempBuffer);
	}

	for (iI = 0; iI < GC.getNumPromotionInfos(); ++iI)
	{
		if (!GC.getPromotionInfo((PromotionTypes)iI).isGraphicalOnly() && pUnit->isHasPromotion((PromotionTypes)iI))
		{
			szTempBuffer.Format(L"<img=%S size=16></img>", GC.getPromotionInfo((PromotionTypes)iI).getButton());
			szString.append(szTempBuffer);
		}
	}
    if (bAlt && (gDLL->getChtLvl() > 0))
    {
		CvSelectionGroup* eGroup = pUnit->getGroup();
		if (eGroup != NULL)
		{
			if (pUnit->isGroupHead())
				szString.append(CvWString::format(L"\nLeading "));
			else
				szString.append(L"\n");

			szTempBuffer.Format(L"Group(%d), %d units", eGroup->getID(), eGroup->getNumUnits());
			szString.append(szTempBuffer);
		}
    }
    ///TKs Med
    if (pUnit->isGoods() && GC.getUnitInfo(pUnit->getUnitType()).getRequiredTransportSize() > 1)
    {
        szString.append(NEWLINE);
        szString.append(gDLL->getText("TXT_KEY_UNIT_CARGO_YIELD_SPACE", GC.getUnitInfo(pUnit->getUnitType()).getRequiredTransportSize()));

    }


	if (!bOneLine)
	{
	    if (!pUnit->isGoods() && pUnit->getHomeCity() != NULL)
		{
			szString.append(NEWLINE);
			szString.append(gDLL->getText("TXT_KEY_GET_HOME_CITY", pUnit->getHomeCity()->getNameKey()));
		}
        ///TKe
		if (pUnit->cargoSpace() > 0)
		{
			if (pUnit->getTeam() == GC.getGameINLINE().getActiveTeam())
			{
				szTempBuffer = NEWLINE + gDLL->getText("TXT_KEY_UNIT_HELP_CARGO_SPACE", pUnit->getCargo(), pUnit->cargoSpace());
			}
			else
			{
				szTempBuffer = NEWLINE + gDLL->getText("TXT_KEY_UNIT_CARGO_SPACE", pUnit->cargoSpace());
			}
			szString.append(szTempBuffer);

			if (pUnit->specialCargo() != NO_SPECIALUNIT)
			{
				szString.append(gDLL->getText("TXT_KEY_UNIT_CARRIES", GC.getSpecialUnitInfo(pUnit->specialCargo()).getTextKeyWide()));
			}
		}
        ///TKs Med

        if ( pUnit->canTrainUnit())
        {
            int iTrainCounter = pUnit->getTrainCounter();
            bool bKnighted = (pUnit->getUnitInfo().getKnightDubbingWeight() == -1 || pUnit->isHasRealPromotion((PromotionTypes)GC.getXMLval(XML_DEFAULT_KNIGHT_PROMOTION)));
            if (!bKnighted)
            {
                if (iTrainCounter == 0)
                {
                    szString.append(NEWLINE);
                    szString.append(gDLL->getText("TXT_KEY_UNITS_TRAINED_HELP_TEXT", GC.getXMLval(XML_TURNS_TO_TRAIN)));
                }
                else if (iTrainCounter > 0)
                {
                    szString.append(NEWLINE);
                    szString.append(gDLL->getText("TXT_KEY_UNITS_TRAINED_HELP_FORTIFIED_TEXT", GC.getXMLval(XML_TURNS_TO_TRAIN) - iTrainCounter));
                }
            }
        }
        ///TKe
		if (pUnit->fortifyModifier() != 0)
		{
			szString.append(NEWLINE);
			szString.append(gDLL->getText("TXT_KEY_UNIT_HELP_FORTIFY_BONUS", pUnit->fortifyModifier()));
		}

		if (!bShort)
		{
			if (pUnit->alwaysInvisible())
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_INVISIBLE_ALL"));
			}
			else if (pUnit->getInvisibleType() != NO_INVISIBLE)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_INVISIBLE_MOST"));
			}

			for (iI = 0; iI < pUnit->getNumSeeInvisibleTypes(); ++iI)
			{
				if (pUnit->getSeeInvisibleType(iI) != pUnit->getInvisibleType())
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_UNIT_SEE_INVISIBLE", GC.getInvisibleInfo(pUnit->getSeeInvisibleType(iI)).getTextKeyWide()));
				}
			}

			if (pUnit->canMoveImpassable())
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_CAN_MOVE_IMPASSABLE"));
			}
		}

		if (!bShort)
		{
			if (pUnit->noDefensiveBonus())
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_NO_DEFENSE_BONUSES"));
			}

			if (pUnit->flatMovementCost())
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_FLAT_MOVEMENT"));
			}

			if (pUnit->ignoreTerrainCost())
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_IGNORE_TERRAIN"));
			}

			if (pUnit->isBlitz())
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_PROMOTION_BLITZ_TEXT"));
			}

			if (pUnit->isAmphib())
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_PROMOTION_AMPHIB_TEXT"));
			}

			if (pUnit->isRiver())
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_PROMOTION_RIVER_ATTACK_TEXT"));
			}

			if (pUnit->isEnemyRoute())
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_PROMOTION_ENEMY_ROADS_TEXT"));
			}

			if (pUnit->isAlwaysHeal())
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_PROMOTION_ALWAYS_HEAL_TEXT"));
			}

			if (pUnit->isHillsDoubleMove())
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_PROMOTION_HILLS_MOVE_TEXT"));
			}

			for (iI = 0; iI < GC.getNumTerrainInfos(); ++iI)
			{
				if (pUnit->isTerrainDoubleMove((TerrainTypes)iI))
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_PROMOTION_DOUBLE_MOVE_TEXT", GC.getTerrainInfo((TerrainTypes) iI).getTextKeyWide()));
				}
			}

			for (iI = 0; iI < GC.getNumFeatureInfos(); ++iI)
			{
				if (pUnit->isFeatureDoubleMove((FeatureTypes)iI))
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_PROMOTION_DOUBLE_MOVE_TEXT", GC.getFeatureInfo((FeatureTypes) iI).getTextKeyWide()));
				}
			}

			if (pUnit->getExtraVisibilityRange() != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_PROMOTION_VISIBILITY_TEXT", pUnit->getExtraVisibilityRange()));
			}

			if (pUnit->getExtraMoveDiscount() != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_PROMOTION_MOVE_DISCOUNT_TEXT", -(pUnit->getExtraMoveDiscount())));
			}

			if (pUnit->getExtraEnemyHeal() != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_PROMOTION_HEALS_EXTRA_TEXT", pUnit->getExtraEnemyHeal()) + gDLL->getText("TXT_KEY_PROMOTION_ENEMY_LANDS_TEXT"));
			}

			if (pUnit->getExtraNeutralHeal() != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_PROMOTION_HEALS_EXTRA_TEXT", pUnit->getExtraNeutralHeal()) + gDLL->getText("TXT_KEY_PROMOTION_NEUTRAL_LANDS_TEXT"));
			}

			if (pUnit->getExtraFriendlyHeal() != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_PROMOTION_HEALS_EXTRA_TEXT", pUnit->getExtraFriendlyHeal()) + gDLL->getText("TXT_KEY_PROMOTION_FRIENDLY_LANDS_TEXT"));
			}

			if (pUnit->getSameTileHeal() != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_PROMOTION_HEALS_SAME_TEXT", pUnit->getSameTileHeal()) + gDLL->getText("TXT_KEY_PROMOTION_DAMAGE_TURN_TEXT"));
			}

			if (pUnit->getAdjacentTileHeal() != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_PROMOTION_HEALS_ADJACENT_TEXT", pUnit->getAdjacentTileHeal()) + gDLL->getText("TXT_KEY_PROMOTION_DAMAGE_TURN_TEXT"));
			}
		}

		if (pUnit->withdrawalProbability() > 0)
		{
			szString.append(NEWLINE);
			szString.append(gDLL->getText("TXT_KEY_UNIT_WITHDRAWL_PROBABILITY_SHORT", pUnit->withdrawalProbability()));
		}

		CvCity* pEvasionCity = pUnit->getEvasionCity();
		if (pEvasionCity != NULL && pEvasionCity->isRevealed(pUnit->getTeam(), true))
		{
			if (bShort)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_EVASION_SHORT", pEvasionCity->getNameKey()));
			}
			else
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_EVASION", pEvasionCity->getNameKey()));
			}
		}

		if (pUnit->getExtraCombatPercent() != 0)
		{
			szString.append(NEWLINE);
			szString.append(gDLL->getText("TXT_KEY_PROMOTION_STRENGTH_TEXT", pUnit->getExtraCombatPercent()));
		}

		if (pUnit->cityAttackModifier() == pUnit->cityDefenseModifier())
		{
			if (pUnit->cityAttackModifier() != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_CITY_STRENGTH_MOD", pUnit->cityAttackModifier()));
			}
		}
		else
		{
			if (pUnit->cityAttackModifier() != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_PROMOTION_CITY_ATTACK_TEXT", pUnit->cityAttackModifier()));
			}

			if (pUnit->cityDefenseModifier() != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_PROMOTION_CITY_DEFENSE_TEXT", pUnit->cityDefenseModifier()));
			}
		}

		if (pUnit->hillsAttackModifier() == pUnit->hillsDefenseModifier())
		{
			if (pUnit->hillsAttackModifier() != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_HILLS_STRENGTH", pUnit->hillsAttackModifier()));
			}
		}
		else
		{
			if (pUnit->hillsAttackModifier() != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_HILLS_ATTACK", pUnit->hillsAttackModifier()));
			}

			if (pUnit->hillsDefenseModifier() != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_HILLS_DEFENSE", pUnit->hillsDefenseModifier()));
			}
		}

		for (iI = 0; iI < GC.getNumTerrainInfos(); ++iI)
		{
			if (pUnit->terrainAttackModifier((TerrainTypes)iI) == pUnit->terrainDefenseModifier((TerrainTypes)iI))
			{
				if (pUnit->terrainAttackModifier((TerrainTypes)iI) != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_UNIT_STRENGTH", pUnit->terrainAttackModifier((TerrainTypes)iI), GC.getTerrainInfo((TerrainTypes) iI).getTextKeyWide()));
				}
			}
			else
			{
				if (pUnit->terrainAttackModifier((TerrainTypes)iI) != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_UNIT_ATTACK", pUnit->terrainAttackModifier((TerrainTypes)iI), GC.getTerrainInfo((TerrainTypes) iI).getTextKeyWide()));
				}

				if (pUnit->terrainDefenseModifier((TerrainTypes)iI) != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_UNIT_DEFENSE", pUnit->terrainDefenseModifier((TerrainTypes)iI), GC.getTerrainInfo((TerrainTypes) iI).getTextKeyWide()));
				}
			}
		}

		for (iI = 0; iI < GC.getNumFeatureInfos(); ++iI)
		{
			if (pUnit->featureAttackModifier((FeatureTypes)iI) == pUnit->featureDefenseModifier((FeatureTypes)iI))
			{
				if (pUnit->featureAttackModifier((FeatureTypes)iI) != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_UNIT_STRENGTH", pUnit->featureAttackModifier((FeatureTypes)iI), GC.getFeatureInfo((FeatureTypes) iI).getTextKeyWide()));
				}
			}
			else
			{
				if (pUnit->featureAttackModifier((FeatureTypes)iI) != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_UNIT_ATTACK", pUnit->featureAttackModifier((FeatureTypes)iI), GC.getFeatureInfo((FeatureTypes) iI).getTextKeyWide()));
				}

				if (pUnit->featureDefenseModifier((FeatureTypes)iI) != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_UNIT_DEFENSE", pUnit->featureDefenseModifier((FeatureTypes)iI), GC.getFeatureInfo((FeatureTypes) iI).getTextKeyWide()));
				}
			}
		}

		for (iI = 0; iI < GC.getNumUnitClassInfos(); ++iI)
		{
			UnitClassTypes eUnitClass = (UnitClassTypes) iI;
			int iAttackModifier = pUnit->unitClassAttackModifier(eUnitClass);
			int iDefenseModifier = pUnit->unitClassDefenseModifier(eUnitClass);
			if (iAttackModifier == iDefenseModifier)
			{
				if (iAttackModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_UNIT_MOD_VS_TYPE", iAttackModifier, GC.getUnitClassInfo(eUnitClass).getTextKeyWide()));
				}
			}
			else
			{
				if (iAttackModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_UNIT_ATTACK_MOD_VS_CLASS", iAttackModifier, GC.getUnitClassInfo(eUnitClass).getTextKeyWide()));
				}

				if (iDefenseModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_UNIT_DEFENSE_MOD_VS_CLASS", iDefenseModifier, GC.getUnitClassInfo(eUnitClass).getTextKeyWide()));
				}
			}
		}

		for (iI = 0; iI < GC.getNumUnitCombatInfos(); ++iI)
		{
			if (pUnit->unitCombatModifier((UnitCombatTypes)iI) != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_MOD_VS_TYPE_NO_LINK", pUnit->unitCombatModifier((UnitCombatTypes)iI), GC.getUnitCombatInfo((UnitCombatTypes) iI).getTextKeyWide()));
			}
		}

		for (iI = 0; iI < NUM_DOMAIN_TYPES; ++iI)
		{
			if (pUnit->domainModifier((DomainTypes)iI) != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_MOD_VS_TYPE_NO_LINK", pUnit->domainModifier((DomainTypes)iI), GC.getDomainInfo((DomainTypes)iI).getTextKeyWide()));
			}
		}

		if (pUnit->bombardRate() > 0)
		{
			if (bShort)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_BOMBARD_RATE_SHORT", ((pUnit->bombardRate() * 100) / GC.getMAX_CITY_DEFENSE_DAMAGE())));
			}
			else
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_BOMBARD_RATE", ((pUnit->bombardRate() * 100) / GC.getMAX_CITY_DEFENSE_DAMAGE())));
			}
		}

		if (pUnit->getUnitInfo().isTreasure())
		{
			szString.append(NEWLINE);
			szString.append(gDLL->getText("TXT_KEY_UNIT_TREASURE_NUMBER_HELP", pUnit->getYieldStored()));
		}

		if (!isEmpty(pUnit->getUnitInfo().getHelp()))
		{
			szString.append(NEWLINE);
			szString.append(pUnit->getUnitInfo().getHelp());
		}

        if (bAlt && (gDLL->getChtLvl() > 0))
        {
            ///Tks Med
//            szTempBuffer.Format(L"\nUnitAI Type = %s.", pUnit->AI_getUnitAIState());
//            szString.append(szTempBuffer);

            szTempBuffer.Format(L"\nUnitAI Type = %s(%d).", GC.getUnitAIInfo(pUnit->AI_getUnitAIType()).getDescription(), pUnit->AI_getUnitAIState());
            szString.append(szTempBuffer);
			if (pUnit->getUnitTradeMarket() != NO_EUROPE)
			{
				szTempBuffer.Format(L"\nTradeScreen = %s.", GC.getEuropeInfo(pUnit->getUnitTradeMarket()).getHelp());
				szString.append(szTempBuffer);
			}
			else
			{
				szTempBuffer.Format(L"\nNo TradeScreen.");
				szString.append(szTempBuffer);
			}
            ///TKe
            szTempBuffer.Format(L"\nSacrifice Value = %d.", pUnit->AI_sacrificeValue(NULL));
            szString.append(szTempBuffer);
            if (pUnit->getHomeCity() != NULL)
            {
				szTempBuffer.Format(L"\nHome City = %s.", pUnit->getHomeCity()->getName().GetCString());
				szString.append(szTempBuffer);
            }
        }
	}
}

void CvGameTextMgr::setUnitPromotionHelp(CvWStringBuffer &szString, const CvUnit* pUnit)
{
	std::vector<PromotionTypes> aPromotions;

	for (int iPromotion = 0; iPromotion < GC.getNumPromotionInfos(); ++iPromotion)
	{
		if (!GC.getPromotionInfo((PromotionTypes) iPromotion).isGraphicalOnly())
		{
			if (pUnit->isHasPromotion((PromotionTypes) iPromotion))
			{
				aPromotions.push_back((PromotionTypes) iPromotion);
			}
		}
	}
	///Tks Med
//	PromotionTypes eHomeBoy = (PromotionTypes) GC.getXMLval(XML_PROMOTION_BUILD_HOME);
//	if (eHomeBoy != NO_PROMOTION)
//	{
//        if (pUnit->isHasPromotion(eHomeBoy))
//        {
//            aPromotions.push_back(eHomeBoy);
//        }
//    }
    ///Tke

	for (uint i = 0; i < aPromotions.size(); ++i)
	{
		if (!szString.isEmpty())
		{
			szString.append(NEWLINE);
		}
		if (aPromotions.size() > 10)
		{
			szString.append(CvWString::format(SETCOLR L"%s" ENDCOLR , TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), GC.getPromotionInfo(aPromotions[i]).getDescription()));
		}
		else
		{
			setPromotionHelp(szString, aPromotions[i], false);
		}
	}
}


void CvGameTextMgr::setProfessionHelp(CvWStringBuffer &szBuffer, ProfessionTypes eProfession, bool bCivilopediaText, bool bStrategyText)
{
	CvProfessionInfo& kProfession = GC.getProfessionInfo(eProfession);

	CvWString szTempBuffer;

	if (!bCivilopediaText)
	{
		szTempBuffer.Format(SETCOLR L"%s" ENDCOLR , TEXT_COLOR("COLOR_UNIT_TEXT"), GC.getProfessionInfo(eProfession).getDescription());
		szBuffer.append(szTempBuffer);
	}

	/// info subclass - start - Nightinggale
	if (kProfession.isParent() || kProfession.isSubType())
	{
		// print links to all sub professions and/or parent
		ProfessionTypes eLoopProfession = kProfession.isParent() ? eProfession : kProfession.getParent();
		int iMax = eLoopProfession + GC.getProfessionInfo(eLoopProfession).getNumSubTypes();
		for (int iSub = eLoopProfession; iSub <= iMax; iSub++)
		{
			if (iSub != eProfession)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getSymbolID(BULLET_CHAR));
                szBuffer.append(gDLL->getText("TXT_KEY_ALT_EQUIPMENT_LINK", GC.getProfessionInfo((ProfessionTypes)iSub).getTextKeyWide()));
			}
		}
	}
	/// info subclass - end - Nightinggale

	if (!bCivilopediaText)
	{
	    ///Tks Med
	    if (!bStrategyText)
	    {
            // MultipleYieldsConsumed Start by Aymerick 05/01/2010
            std::vector<YieldTypes> eYieldsConsumed;
            for (int i = 0; i < kProfession.getNumYieldsConsumed(); i++)
            {
                YieldTypes eYieldConsumed = (YieldTypes) kProfession.getYieldsConsumed(i);
                if (eYieldConsumed != NO_YIELD)
                {
                    eYieldsConsumed.push_back((YieldTypes) eYieldConsumed);
                }
            }

            if (!eYieldsConsumed.empty())
            {
                CvWString szYieldsConsumedList;
                for (std::vector<YieldTypes>::iterator it = eYieldsConsumed.begin(); it != eYieldsConsumed.end(); ++it)
                {
                    if (!szYieldsConsumedList.empty())
                    {
                        if (*it == eYieldsConsumed.back())
                        {
                            szYieldsConsumedList += CvWString::format(gDLL->getText("TXT_KEY_AND"));
                        }
                        else
                        {
                            szYieldsConsumedList += L", ";
                        }
                    }
                    szYieldsConsumedList += CvWString::format(L"%c", GC.getYieldInfo(*it).getChar());
                }
                szTempBuffer.Format(gDLL->getText("TXT_KEY_YIELDS_CONSUMED_WITH_CHAR", szYieldsConsumedList.GetCString()));
                szBuffer.append(NEWLINE);
                szBuffer.append(szTempBuffer);
            }
            // MultipleYieldsConsumed End
            // MultipleYieldsProduced Start by Aymerick 22/01/2010
            std::vector<YieldTypes> eYieldsProduced;
            for (int i = 0; i < kProfession.getNumYieldsProduced(); i++)
            {
                YieldTypes eYieldProduced = (YieldTypes) kProfession.getYieldsProduced(i);
                if (eYieldProduced != NO_YIELD)
                {
                    eYieldsProduced.push_back((YieldTypes) eYieldProduced);
                }
            }

            if (!eYieldsProduced.empty())
            {
                CvWString szYieldsProducedList;
                for (std::vector<YieldTypes>::iterator it = eYieldsProduced.begin(); it != eYieldsProduced.end(); ++it)
                {
                    if (!szYieldsProducedList.empty())
                    {
                        if (*it == eYieldsProduced.back())
                        {
                            szYieldsProducedList += CvWString::format(gDLL->getText("TXT_KEY_AND"));
                        }
                        else
                        {
                            szYieldsProducedList += L", ";
                        }
                    }
                    szYieldsProducedList += CvWString::format(L"%c", GC.getYieldInfo(*it).getChar());
                }
                szTempBuffer.Format(gDLL->getText("TXT_KEY_YIELDS_PRODUCED_WITH_CHAR", szYieldsProducedList.GetCString()));
                szBuffer.append(NEWLINE);
                szBuffer.append(szTempBuffer);
            }
            // MultipleYieldsProduced End
	    }



		int iCombatChange = kProfession.getCombatChange();
		if (GC.getGameINLINE().getActivePlayer() != NO_PLAYER)
		{
			iCombatChange += GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getProfessionCombatChange(eProfession);
		}
		int iMovesChange = kProfession.getMovesChange();

		if (iCombatChange != 0 || iMovesChange != 0)
		{
			szBuffer.append(NEWLINE);
			szTempBuffer.Format(L"%c", gDLL->getSymbolID(BULLET_CHAR));
			szBuffer.append(szTempBuffer);

			if (iCombatChange != 0)
			{
				szTempBuffer.Format(L"%d%c ", iCombatChange, gDLL->getSymbolID(STRENGTH_CHAR));
				szBuffer.append(szTempBuffer);
			}

			if (iMovesChange != 0)
			{
				szTempBuffer.Format(L"%d%c ", iMovesChange, gDLL->getSymbolID(MOVES_CHAR));
				szBuffer.append(szTempBuffer);
			}
		}
	}
    ///Tks Med
    if (!bStrategyText)
    {
        for (int i = 0; i < kProfession.getNumYieldsProduced(); i++)
        {
            YieldTypes eYieldProduced = (YieldTypes) kProfession.getYieldsProduced(i);
            if (eYieldProduced != NO_YIELD)
            {
                if (GC.getYieldInfo(eYieldProduced).getLatitudeModifiers() > 0)
                {
                    szBuffer.append(NEWLINE);
                    szBuffer.append(gDLL->getText("TXT_KEY_YIELD_LATITUDE_BONUS_TEXT", GC.getYieldInfo(eYieldProduced).getLatitudeModifiers()));
                }
            }
        }
        ///Tke
        if (kProfession.getWorkRate() != 0)
        {
            szBuffer.append(NEWLINE);
            szBuffer.append(gDLL->getText("TXT_KEY_UNIT_CAN_IMPROVE_LAND"));
        }

        if (kProfession.getMissionaryRate() != 0)
        {
            szBuffer.append(NEWLINE);
            szBuffer.append(gDLL->getText("TXT_KEY_UNIT_CAN_ESTABLISH_MISSIONS"));
        }
///TKs Med
        if (kProfession.getExperenceLevel() != 0)
        {
            szBuffer.append(NEWLINE);
            szBuffer.append(gDLL->getText("TXT_KEY_PROFESSION_REQUIRES_TRAINING"));
        }
///TKe
        if (kProfession.isNoDefensiveBonus())
        {
            szBuffer.append(NEWLINE);
            szBuffer.append(gDLL->getText("TXT_KEY_UNIT_NO_DEFENSE_BONUSES"));
        }

        if (kProfession.isUnarmed())
        {
            szBuffer.append(NEWLINE);
            szBuffer.append(gDLL->getText("TXT_KEY_UNIT_ONLY_DEFENSIVE"));
        }

        for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
        {
            int iYieldAmount = GC.getGameINLINE().getActivePlayer() != NO_PLAYER ? GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getYieldEquipmentAmount(eProfession, (YieldTypes) iYield) : kProfession.getYieldEquipmentAmount((YieldTypes) iYield);
            if (iYieldAmount != 0)
            {
                szTempBuffer.Format(gDLL->getText("TXT_KEY_UNIT_REQUIRES_YIELD_QUANTITY_STRING", iYieldAmount, GC.getYieldInfo((YieldTypes) iYield).getChar()));
                szBuffer.append(NEWLINE);
                szBuffer.append(szTempBuffer);
            }
        }
    }
    ///TKs Civic Screen
	for (int i = 0; i < kProfession.getNumCaptureCargoTypes(); i++)
        {
			if (kProfession.getCaptureCargoTypes(i) >= 0)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_PROFESSION_CAPTURE_CARGO", GC.getUnitClassInfo((UnitClassTypes)kProfession.getCaptureCargoTypes(i)).getDescription()));
			}
		}
    ///TKe

    ///Tks Med
    if (kProfession.getTaxCollectRate() != 0)
    {
        szBuffer.append(NEWLINE);
        szBuffer.append(gDLL->getText("TXT_KEY_TAX_COLLECTOR_PROFESSION", kProfession.getTaxCollectRate()));
    }

	// generic loop by Nightinggale
	for (int i = 0; i < GC.getNumUnitCombatInfos(); i++)
	{
		UnitCombatTypes eUnitCombat = (UnitCombatTypes) i;
		if (kProfession.getCombatGearTypes(eUnitCombat))
		{
			CvInfoBase& kInfo = GC.getUnitCombatInfo(eUnitCombat);

			if ( wcslen(kInfo.getCivilopedia()) > 0)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText(kInfo.getCivilopedia()));
			}
		}
	}
    ///Tke
	for (int iPromotion = 0; iPromotion < GC.getNumPromotionInfos(); ++iPromotion)
	{
		if (kProfession.isFreePromotion(iPromotion))
		{
			setPromotionHelp(szBuffer, (PromotionTypes) iPromotion, true);
			//parsePromotionHelp(szBuffer, (PromotionTypes) iPromotion, NEWLINE, bCivilopediaText);
		}
	}

	///TKs Invention Core Mod v 1.0
	if (bCivilopediaText)
	{


        int iAllows = 0;
        for (int iCivic = 0; iCivic < GC.getNumCivicInfos(); iCivic++)
        {
            iAllows = GC.getCivicInfo((CivicTypes)iCivic).getAllowsProfessions(eProfession);
            if (iAllows > 0)
            {
                if ((CivicTypes)GC.getCivicInfo((CivicTypes)iCivic).getInventionCategory() == (CivicTypes)GC.getXMLval(XML_MEDIEVAL_TRADE_TECH))
                {
                    szBuffer.append(NEWLINE);
                    szBuffer.append(gDLL->getText("TXT_KEY_TRADE_PERK_MAKES_AVAILABLE", GC.getCivicInfo((CivicTypes)iCivic).getDescription()));
                    break;
                }
                else
                {
                    szBuffer.append(NEWLINE);
                    szBuffer.append(gDLL->getText("TXT_KEY_TECH_MAKES_AVAILABLE", GC.getCivicInfo((CivicTypes)iCivic).getDescription()));
                    break;
                }
            }
            else if (iAllows < 0)
            {
                szBuffer.append(NEWLINE);
                szBuffer.append(gDLL->getText("TXT_KEY_TECH_MAKES_OBSOLETE", GC.getCivicInfo((CivicTypes)iCivic).getDescription()));
                break;
            }
        }
	}
	///TKe
}

void CvGameTextMgr::setPlotListHelp(CvWStringBuffer &szString, const CvPlot* pPlot, bool bOneLine, bool bShort)
{
	PROFILE_FUNC();

	int numPromotionInfos = GC.getNumPromotionInfos();

	// if cheatmode and ctrl, display grouping info instead
	if ((gDLL->getChtLvl() > 0) && gDLL->ctrlKey())
	{
		if (pPlot->isVisible(GC.getGameINLINE().getActiveTeam(), GC.getGameINLINE().isDebugMode()))
		{
			CvWString szTempString;

			CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
			while(pUnitNode != NULL)
			{
				CvUnit* pHeadUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = pPlot->nextUnitNode(pUnitNode);

				// is this unit the head of a group, not cargo, and visible?
				if (pHeadUnit && pHeadUnit->isGroupHead() && !pHeadUnit->isCargo() && !pHeadUnit->isInvisible(GC.getGameINLINE().getActiveTeam(), GC.getGameINLINE().isDebugMode()))
				{
					// head unit name and unitai
					szString.append(CvWString::format(L" (%d)", shortenID(pHeadUnit->getID())));

					getUnitAIString(szTempString, pHeadUnit->AI_getUnitAIType());
					szString.append(CvWString::format(SETCOLR L" %s " ENDCOLR, GET_PLAYER(pHeadUnit->getOwnerINLINE()).getPlayerTextColorR(), GET_PLAYER(pHeadUnit->getOwnerINLINE()).getPlayerTextColorG(), GET_PLAYER(pHeadUnit->getOwnerINLINE()).getPlayerTextColorB(), GET_PLAYER(pHeadUnit->getOwnerINLINE()).getPlayerTextColorA(), szTempString.GetCString()));

					// promotion icons
					for (int iPromotionIndex = 0; iPromotionIndex < numPromotionInfos; iPromotionIndex++)
					{
						PromotionTypes ePromotion = (PromotionTypes)iPromotionIndex;
						if (!GC.getPromotionInfo(ePromotion).isGraphicalOnly() && pHeadUnit->isHasPromotion(ePromotion))
						{
							szString.append(CvWString::format(L"<img=%S size=16></img>", GC.getPromotionInfo(ePromotion).getButton()));
						}
					}

					///TKs
					if (pHeadUnit->getLeaderUnitType() != NO_UNIT)
					{
                       // szString.append(CvWString::format(L" (%s)", GC.getUnitInfo(pHeadUnit->getLeaderUnitType()).getDescription));
                        szString.append(CvWString::format(SETCOLR L"\n%s" ENDCOLR, TEXT_COLOR("COLOR_UNIT_TEXT"), GC.getUnitInfo(pHeadUnit->getLeaderUnitType()).getDescription()));
					}
//					else
//					{
//					    szString.append(CvWString::format(L" (No Leader)"));
//					}

                    if (pHeadUnit->getEscortPromotion() != NO_PROMOTION)
                    {
                        szString.append(CvWString::format(L"\n(Being Escorted)"));
                    }
                    else
                    {
                        szString.append(CvWString::format(L"\n(NO Escort)"));
                    }
					///TKe

					// group
					CvSelectionGroup* pHeadGroup = pHeadUnit->getGroup();
					FAssertMsg(pHeadGroup != NULL, "unit has NULL group");
					if (pHeadGroup->getNumUnits() > 1)
					{
						szString.append(CvWString::format(L"\nGroup:%d [%d units]", shortenID(pHeadGroup->getID()), pHeadGroup->getNumUnits()));

						// get average damage
						int iAverageDamage = 0;
						CLLNode<IDInfo>* pUnitNode = pHeadGroup->headUnitNode();
						while (pUnitNode != NULL)
						{
							CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
							pUnitNode = pHeadGroup->nextUnitNode(pUnitNode);

							iAverageDamage += (pLoopUnit->getDamage() * pLoopUnit->maxHitPoints()) / 100;
						}
						iAverageDamage /= pHeadGroup->getNumUnits();
						if (iAverageDamage > 0)
						{
							szString.append(CvWString::format(L" %d%%", 100 - iAverageDamage));
						}
					}

					// mission ai
					MissionAITypes eMissionAI = pHeadGroup->AI_getMissionAIType();
					if (eMissionAI != NO_MISSIONAI)
					{
						getMissionAIString(szTempString, eMissionAI);
						szString.append(CvWString::format(SETCOLR L"\n%s" ENDCOLR, TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), szTempString.GetCString()));
					}

					// mission
					MissionTypes eMissionType = (MissionTypes) pHeadGroup->getMissionType(0);
					if (eMissionType != NO_MISSION)
					{
						getMissionTypeString(szTempString, eMissionType);
						szString.append(CvWString::format(SETCOLR L"\n%s" ENDCOLR, TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), szTempString.GetCString()));
					}

					// mission unit
					CvUnit* pMissionUnit = pHeadGroup->AI_getMissionAIUnit();
					if (pMissionUnit != NULL && (eMissionAI != NO_MISSIONAI || eMissionType != NO_MISSION))
					{
						// mission unit
						szString.append(L"\n to ");
						szString.append(CvWString::format(SETCOLR L"%s" ENDCOLR, GET_PLAYER(pMissionUnit->getOwnerINLINE()).getPlayerTextColorR(), GET_PLAYER(pMissionUnit->getOwnerINLINE()).getPlayerTextColorG(), GET_PLAYER(pMissionUnit->getOwnerINLINE()).getPlayerTextColorB(), GET_PLAYER(pMissionUnit->getOwnerINLINE()).getPlayerTextColorA(), pMissionUnit->getName().GetCString()));
						szString.append(CvWString::format(L"(%d) G:%d", shortenID(pMissionUnit->getID()), shortenID(pMissionUnit->getGroupID())));
						getUnitAIString(szTempString, pMissionUnit->AI_getUnitAIType());
						szString.append(CvWString::format(SETCOLR L" %s" ENDCOLR, GET_PLAYER(pMissionUnit->getOwnerINLINE()).getPlayerTextColorR(), GET_PLAYER(pMissionUnit->getOwnerINLINE()).getPlayerTextColorG(), GET_PLAYER(pMissionUnit->getOwnerINLINE()).getPlayerTextColorB(), GET_PLAYER(pMissionUnit->getOwnerINLINE()).getPlayerTextColorA(), szTempString.GetCString()));
					}

					// mission plot
					if (eMissionAI != NO_MISSIONAI || eMissionType != NO_MISSION)
					{
						// first try the plot from the missionAI
						CvPlot* pMissionPlot = pHeadGroup->AI_getMissionAIPlot();

						// if MissionAI does not have a plot, get one from the mission itself
						if (pMissionPlot == NULL && eMissionType != NO_MISSION)
						{
							switch (eMissionType)
							{
							case MISSION_MOVE_TO:
							case MISSION_ROUTE_TO:
								pMissionPlot =  GC.getMapINLINE().plotINLINE(pHeadGroup->getMissionData1(0), pHeadGroup->getMissionData2(0));
								break;

							case MISSION_MOVE_TO_UNIT:
								if (pMissionUnit != NULL)
								{
									pMissionPlot = pMissionUnit->plot();
								}
								break;
							}
						}

						if (pMissionPlot != NULL)
						{
							szString.append(CvWString::format(L"\n [%d,%d]", pMissionPlot->getX_INLINE(), pMissionPlot->getY_INLINE()));

							CvCity* pCity = pMissionPlot->getWorkingCity();
							if (pCity != NULL)
							{
								szString.append(L" (");

								if (!pMissionPlot->isCity())
								{
									DirectionTypes eDirection = estimateDirection(dxWrap(pMissionPlot->getX_INLINE() - pCity->plot()->getX_INLINE()), dyWrap(pMissionPlot->getY_INLINE() - pCity->plot()->getY_INLINE()));

									getDirectionTypeString(szTempString, eDirection);
									szString.append(CvWString::format(L"%s of ", szTempString.GetCString()));
								}

								szString.append(CvWString::format(SETCOLR L"%s" ENDCOLR L")", GET_PLAYER(pCity->getOwnerINLINE()).getPlayerTextColorR(), GET_PLAYER(pCity->getOwnerINLINE()).getPlayerTextColorG(), GET_PLAYER(pCity->getOwnerINLINE()).getPlayerTextColorB(), GET_PLAYER(pCity->getOwnerINLINE()).getPlayerTextColorA(), pCity->getName().GetCString()));
							}
							else
							{
								if (pMissionPlot != pPlot)
								{
									DirectionTypes eDirection = estimateDirection(dxWrap(pMissionPlot->getX_INLINE() - pPlot->getX_INLINE()), dyWrap(pMissionPlot->getY_INLINE() - pPlot->getY_INLINE()));

									getDirectionTypeString(szTempString, eDirection);
									szString.append(CvWString::format(L" (%s)", szTempString.GetCString()));
								}

								PlayerTypes eMissionPlotOwner = pMissionPlot->getOwnerINLINE();
								if (eMissionPlotOwner != NO_PLAYER)
								{
									szString.append(CvWString::format(L", " SETCOLR L"%s" ENDCOLR, GET_PLAYER(eMissionPlotOwner).getPlayerTextColorR(), GET_PLAYER(eMissionPlotOwner).getPlayerTextColorG(), GET_PLAYER(eMissionPlotOwner).getPlayerTextColorB(), GET_PLAYER(eMissionPlotOwner).getPlayerTextColorA(), GET_PLAYER(eMissionPlotOwner).getName()));
								}
							}
						}
					}

					// display cargo for head unit
					CLLNode<IDInfo>* pUnitNode2 = pPlot->headUnitNode();
					while(pUnitNode2 != NULL)
					{
						CvUnit* pCargoUnit = ::getUnit(pUnitNode2->m_data);
						pUnitNode2 = pPlot->nextUnitNode(pUnitNode2);

						// is this unit visible?
						if (pCargoUnit && (pCargoUnit != pHeadUnit) && !pCargoUnit->isInvisible(GC.getGameINLINE().getActiveTeam(), GC.getGameINLINE().isDebugMode()))
						{
							// is this unit in cargo of the headunit?
							if (pCargoUnit->getTransportUnit() == pHeadUnit)
							{
								// name and unitai
								szString.append(CvWString::format(SETCOLR L"\n %s" ENDCOLR, TEXT_COLOR("COLOR_ALT_HIGHLIGHT_TEXT"), pCargoUnit->getName().GetCString()));
								szString.append(CvWString::format(L"(%d)", shortenID(pCargoUnit->getID())));
								getUnitAIString(szTempString, pCargoUnit->AI_getUnitAIType());
								szString.append(CvWString::format(SETCOLR L" %s " ENDCOLR, GET_PLAYER(pCargoUnit->getOwnerINLINE()).getPlayerTextColorR(), GET_PLAYER(pCargoUnit->getOwnerINLINE()).getPlayerTextColorG(), GET_PLAYER(pCargoUnit->getOwnerINLINE()).getPlayerTextColorB(), GET_PLAYER(pCargoUnit->getOwnerINLINE()).getPlayerTextColorA(), szTempString.GetCString()));

								// promotion icons
								for (int iPromotionIndex = 0; iPromotionIndex < numPromotionInfos; iPromotionIndex++)
								{
									PromotionTypes ePromotion = (PromotionTypes)iPromotionIndex;
									if (!GC.getPromotionInfo(ePromotion).isGraphicalOnly() && pCargoUnit->isHasPromotion(ePromotion))
									{
										szString.append(CvWString::format(L"<img=%S size=16></img>", GC.getPromotionInfo(ePromotion).getButton()));
									}
								}
							}
						}
					}

					// display grouped units
					CLLNode<IDInfo>* pUnitNode3 = pPlot->headUnitNode();
					while(pUnitNode3 != NULL)
					{
						CvUnit* pUnit = ::getUnit(pUnitNode3->m_data);
						pUnitNode3 = pPlot->nextUnitNode(pUnitNode3);

						// is this unit not head, in head's group and visible?
						if (pUnit && (pUnit != pHeadUnit) && (pUnit->getGroupID() == pHeadUnit->getGroupID()) && !pUnit->isInvisible(GC.getGameINLINE().getActiveTeam(), GC.getGameINLINE().isDebugMode()))
						{
							FAssertMsg(!pUnit->isCargo(), "unit is cargo but head unit is not cargo");
							// name and unitai
							szString.append(CvWString::format(SETCOLR L"\n-%s" ENDCOLR, TEXT_COLOR("COLOR_UNIT_TEXT"), pUnit->getName().GetCString()));
							szString.append(CvWString::format(L" (%d)", shortenID(pUnit->getID())));
							getUnitAIString(szTempString, pUnit->AI_getUnitAIType());
							szString.append(CvWString::format(SETCOLR L" %s " ENDCOLR, GET_PLAYER(pUnit->getOwnerINLINE()).getPlayerTextColorR(), GET_PLAYER(pUnit->getOwnerINLINE()).getPlayerTextColorG(), GET_PLAYER(pUnit->getOwnerINLINE()).getPlayerTextColorB(), GET_PLAYER(pUnit->getOwnerINLINE()).getPlayerTextColorA(), szTempString.GetCString()));

							// promotion icons
							for (int iPromotionIndex = 0; iPromotionIndex < numPromotionInfos; iPromotionIndex++)
							{
								PromotionTypes ePromotion = (PromotionTypes)iPromotionIndex;
								if (!GC.getPromotionInfo(ePromotion).isGraphicalOnly() && pUnit->isHasPromotion(ePromotion))
								{
									szString.append(CvWString::format(L"<img=%S size=16></img>", GC.getPromotionInfo(ePromotion).getButton()));
								}
							}

							// display cargo for loop unit
							CLLNode<IDInfo>* pUnitNode4 = pPlot->headUnitNode();
							while(pUnitNode4 != NULL)
							{
								CvUnit* pCargoUnit = ::getUnit(pUnitNode4->m_data);
								pUnitNode4 = pPlot->nextUnitNode(pUnitNode4);

								// is this unit visible?
								if (pCargoUnit && (pCargoUnit != pUnit) && !pCargoUnit->isInvisible(GC.getGameINLINE().getActiveTeam(), GC.getGameINLINE().isDebugMode()))
								{
									// is this unit in cargo of unit?
									if (pCargoUnit->getTransportUnit() == pUnit)
									{
										// name and unitai
										szString.append(CvWString::format(SETCOLR L"\n %s" ENDCOLR, TEXT_COLOR("COLOR_ALT_HIGHLIGHT_TEXT"), pCargoUnit->getName().GetCString()));
										szString.append(CvWString::format(L"(%d)", shortenID(pCargoUnit->getID())));
										getUnitAIString(szTempString, pCargoUnit->AI_getUnitAIType());
										szString.append(CvWString::format(SETCOLR L" %s " ENDCOLR, GET_PLAYER(pCargoUnit->getOwnerINLINE()).getPlayerTextColorR(), GET_PLAYER(pCargoUnit->getOwnerINLINE()).getPlayerTextColorG(), GET_PLAYER(pCargoUnit->getOwnerINLINE()).getPlayerTextColorB(), GET_PLAYER(pCargoUnit->getOwnerINLINE()).getPlayerTextColorA(), szTempString.GetCString()));

										// promotion icons
										for (int iPromotionIndex = 0; iPromotionIndex < numPromotionInfos; iPromotionIndex++)
										{
											PromotionTypes ePromotion = (PromotionTypes)iPromotionIndex;
											if (!GC.getPromotionInfo(ePromotion).isGraphicalOnly() && pCargoUnit->isHasPromotion(ePromotion))
											{
												szString.append(CvWString::format(L"<img=%S size=16></img>", GC.getPromotionInfo(ePromotion).getButton()));
											}
										}
									}
								}
							}
						}
					}

					// double space non-empty groups
					if (pHeadGroup->getNumUnits() > 1 || pHeadUnit->hasCargo())
					{
						szString.append(NEWLINE);
					}

					szString.append(NEWLINE);
				}
			}
		}

		return;
	}


	CvUnit* pLoopUnit;
	static const uint iMaxNumUnits = 10;
	static std::vector<CvUnit*> apUnits;
	static std::vector<int> aiUnitNumbers;
	static std::vector<int> aiUnitStrength;
	static std::vector<int> aiUnitMaxStrength;
	static std::vector<CvUnit *> plotUnits;

	GC.getGame().getPlotUnits(pPlot, plotUnits);

	int iNumVisibleUnits = 0;
	if (pPlot->isVisible(GC.getGameINLINE().getActiveTeam(), GC.getGameINLINE().isDebugMode()))
	{
		CLLNode<IDInfo>* pUnitNode5 = pPlot->headUnitNode();
		while(pUnitNode5 != NULL)
		{
			CvUnit* pUnit = ::getUnit(pUnitNode5->m_data);
			pUnitNode5 = pPlot->nextUnitNode(pUnitNode5);

			if (pUnit && !pUnit->isInvisible(GC.getGameINLINE().getActiveTeam(), GC.getGameINLINE().isDebugMode()))
			{
				++iNumVisibleUnits;
			}
		}
	}

	apUnits.erase(apUnits.begin(), apUnits.end());

	if (iNumVisibleUnits > iMaxNumUnits)
	{
		aiUnitNumbers.erase(aiUnitNumbers.begin(), aiUnitNumbers.end());
		aiUnitStrength.erase(aiUnitStrength.begin(), aiUnitStrength.end());
		aiUnitMaxStrength.erase(aiUnitMaxStrength.begin(), aiUnitMaxStrength.end());

		if (m_apbPromotion.size() == 0)
		{
			for (int iI = 0; iI < (GC.getNumUnitInfos() * MAX_PLAYERS); ++iI)
			{
				m_apbPromotion.push_back(new int[numPromotionInfos]);
			}
		}

		for (int iI = 0; iI < (GC.getNumUnitInfos() * MAX_PLAYERS); ++iI)
		{
			aiUnitNumbers.push_back(0);
			aiUnitStrength.push_back(0);
			aiUnitMaxStrength.push_back(0);
			for (int iJ = 0; iJ < numPromotionInfos; iJ++)
			{
				m_apbPromotion[iI][iJ] = 0;
			}
		}
	}

	int iCount = 0;
	for (int iI = iMaxNumUnits; iI < iNumVisibleUnits && iI < (int) plotUnits.size(); ++iI)
	{
		pLoopUnit = plotUnits[iI];

		if (pLoopUnit != NULL && pLoopUnit != pPlot->getCenterUnit())
		{
			apUnits.push_back(pLoopUnit);

			if (iNumVisibleUnits > iMaxNumUnits)
			{
				int iIndex = pLoopUnit->getUnitType() * MAX_PLAYERS + pLoopUnit->getOwner();
				if (aiUnitNumbers[iIndex] == 0)
				{
					++iCount;
				}
				++aiUnitNumbers[iIndex];

				int iBase = pLoopUnit->baseCombatStr();
				if (iBase > 0 && pLoopUnit->maxHitPoints() > 0)
				{
					aiUnitMaxStrength[iIndex] += 100 * iBase;
					aiUnitStrength[iIndex] += (100 * iBase * pLoopUnit->currHitPoints()) / pLoopUnit->maxHitPoints();
				}

				for (int iJ = 0; iJ < numPromotionInfos; iJ++)
				{
					if (!GC.getPromotionInfo((PromotionTypes)iJ).isGraphicalOnly() && pLoopUnit->isHasPromotion((PromotionTypes)iJ))
					{
						++m_apbPromotion[iIndex][iJ];
					}
				}
			}
		}
	}


	if (iNumVisibleUnits > 0)
	{
		if (pPlot->getCenterUnit())
		{
			setUnitHelp(szString, pPlot->getCenterUnit(), iNumVisibleUnits > iMaxNumUnits, true);
		}

		uint iNumShown = std::min<uint>(iMaxNumUnits, iNumVisibleUnits);
		for (uint iI = 0; iI < iNumShown && iI < (int) plotUnits.size(); ++iI)
		{
			CvUnit* pLoopUnit = plotUnits[iI];
			if (pLoopUnit != pPlot->getCenterUnit())
			{
				szString.append(NEWLINE);
				setUnitHelp(szString, pLoopUnit, true, true);
			}
		}

		bool bFirst = true;
		if (iNumVisibleUnits > iMaxNumUnits)
		{
			for (int iI = 0; iI < GC.getNumUnitInfos(); ++iI)
			{
				for (int iJ = 0; iJ < MAX_PLAYERS; iJ++)
				{
					int iIndex = iI * MAX_PLAYERS + iJ;

					if (aiUnitNumbers[iIndex] > 0)
					{
						if (iCount < 5 || bFirst)
						{
							szString.append(NEWLINE);
							bFirst = false;
						}
						else
						{
							szString.append(L", ");
						}
						szString.append(CvWString::format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR("COLOR_UNIT_TEXT"), GC.getUnitInfo((UnitTypes)iI).getDescription()));

						szString.append(CvWString::format(L" (%d)", aiUnitNumbers[iIndex]));

						if (aiUnitMaxStrength[iIndex] > 0)
						{
							int iBase = (aiUnitMaxStrength[iIndex] / aiUnitNumbers[iIndex]) / 100;
							int iCurrent = (aiUnitStrength[iIndex] / aiUnitNumbers[iIndex]) / 100;
							int iCurrent100 = (aiUnitStrength[iIndex] / aiUnitNumbers[iIndex]) % 100;
							if (0 == iCurrent100)
							{
								if (iBase == iCurrent)
								{
									szString.append(CvWString::format(L" %d", iBase));
								}
								else
								{
									szString.append(CvWString::format(L" %d/%d", iCurrent, iBase));
								}
							}
							else
							{
								szString.append(CvWString::format(L" %d.%02d/%d", iCurrent, iCurrent100, iBase));
							}
							szString.append(CvWString::format(L"%c", gDLL->getSymbolID(STRENGTH_CHAR)));
						}


						for (int iK = 0; iK < numPromotionInfos; iK++)
						{
							if (m_apbPromotion[iIndex][iK] > 0)
							{
								szString.append(CvWString::format(L"%d<img=%S size=16></img>", m_apbPromotion[iIndex][iK], GC.getPromotionInfo((PromotionTypes)iK).getButton()));
							}
						}

						if (iJ != GC.getGameINLINE().getActivePlayer() && !GC.getUnitInfo((UnitTypes)iI).isHiddenNationality())
						{
							szString.append(L", ");
							szString.append(CvWString::format(SETCOLR L"%s" ENDCOLR, GET_PLAYER((PlayerTypes)iJ).getPlayerTextColorR(), GET_PLAYER((PlayerTypes)iJ).getPlayerTextColorG(), GET_PLAYER((PlayerTypes)iJ).getPlayerTextColorB(), GET_PLAYER((PlayerTypes)iJ).getPlayerTextColorA(), GET_PLAYER((PlayerTypes)iJ).getName()));
						}
					}
				}
			}
		}
	}
}


// Returns true if help was given...
bool CvGameTextMgr::setCombatPlotHelp(CvWStringBuffer &szString, CvPlot* pPlot)
{
	PROFILE_FUNC();

	CvUnit* pAttacker;
	CvUnit* pDefender;
	CvWString szTempBuffer;
	CvWString szOffenseOdds;
	CvWString szDefenseOdds;
	bool bValid;
	int iModifier;

	if (gDLL->getInterfaceIFace()->getLengthSelectionList() == 0)
	{
		return false;
	}

	bValid = false;

	switch (gDLL->getInterfaceIFace()->getSelectionList()->getDomainType())
	{
	case DOMAIN_SEA:
		bValid = pPlot->isWater();
		break;

	case DOMAIN_LAND:
		bValid = !(pPlot->isWater());
		break;

	case DOMAIN_IMMOBILE:
		break;

	default:
		FAssert(false);
		break;
	}

	if (!bValid)
	{
		return false;
	}

	int iOdds;
	pAttacker = gDLL->getInterfaceIFace()->getSelectionList()->AI_getBestGroupAttacker(pPlot, false, iOdds);

	if (pAttacker == NULL)
	{
		pAttacker = gDLL->getInterfaceIFace()->getSelectionList()->AI_getBestGroupAttacker(pPlot, false, iOdds, true);
	}

	if (pAttacker != NULL)
	{
		pDefender = pPlot->getBestDefender(NO_PLAYER, pAttacker->getOwnerINLINE(), pAttacker, false, (NO_TEAM == pAttacker->getDeclareWarUnitMove(pPlot)));

		ProfessionTypes eProfession = NO_PROFESSION;
		if(pDefender != NULL)
		{
			eProfession = pDefender->getProfession();
		}

		CvCity* pCity = pPlot->getPlotCity();
		if(pCity != NULL)
		{
			pDefender = pCity->getBestDefender(&eProfession, pDefender, pAttacker);
		}

		CvUnitTemporaryStrengthModifier kTemporaryStrength(pDefender, eProfession);
		if (pDefender != NULL && pDefender != pAttacker && pDefender->canDefend(pPlot))
		{
			int iCombatOdds = getCombatOdds(pAttacker, pDefender);

			if (iCombatOdds > 999)
			{
				szTempBuffer = L"&gt; 99.9";
			}
			else if (iCombatOdds < 1)
			{
				szTempBuffer = L"&lt; 0.1";
			}
			else
			{
				szTempBuffer.Format(L"%.1f", ((float)iCombatOdds) / 10.0f);
			}
			szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_ODDS", szTempBuffer.GetCString()));

			int iWithdrawal = 0;

			iWithdrawal += std::min(100, pAttacker->withdrawalProbability()) * (1000 - iCombatOdds);

			if (iWithdrawal > 0)
			{
				if (iWithdrawal > 99900)
				{
					szTempBuffer = L"&gt; 99.9";
				}
				else if (iWithdrawal < 100)
				{
					szTempBuffer = L"&lt; 0.1";
				}
				else
				{
					szTempBuffer.Format(L"%.1f", iWithdrawal / 1000.0f);
				}

				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_ODDS_RETREAT", szTempBuffer.GetCString()));
			}

			szOffenseOdds.Format(L"%.2f", pAttacker->currCombatStrFloat(NULL, NULL));
			szDefenseOdds.Format(L"%.2f", pDefender->currCombatStrFloat(pPlot, pAttacker));
			szString.append(NEWLINE);
			szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_ODDS_VS", szOffenseOdds.GetCString(), szDefenseOdds.GetCString()));

			szString.append(L' ');//XXX

			szString.append(gDLL->getText("TXT_KEY_COLOR_POSITIVE"));

			szString.append(L' ');//XXX

			iModifier = pAttacker->getExtraCombatPercent();

			if (iModifier != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_EXTRA_STRENGTH", iModifier));
			}

			iModifier = pAttacker->unitClassAttackModifier(pDefender->getUnitClassType());

			if (iModifier != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_MOD_VS_TYPE", iModifier, GC.getUnitClassInfo(pDefender->getUnitClassType()).getTextKeyWide()));
			}

			if (pDefender->getUnitCombatType() != NO_UNITCOMBAT)
			{
				iModifier = pAttacker->unitCombatModifier(pDefender->getUnitCombatType());

				if (iModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_MOD_VS_TYPE", iModifier, GC.getUnitCombatInfo(pDefender->getUnitCombatType()).getTextKeyWide()));
				}
			}
			///Tks Civics
			iModifier = GET_PLAYER(pAttacker->getOwnerINLINE()).calculateCivicCombatBonuses(pDefender->getOwnerINLINE());
			if (iModifier != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_MOD_VS_CIVIC", iModifier));
			}
			//TKe
			iModifier = pAttacker->domainModifier(pDefender->getDomainType());

			if (iModifier != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_MOD_VS_TYPE", iModifier, GC.getDomainInfo(pDefender->getDomainType()).getTextKeyWide()));
			}

			if (pPlot->isCity(true, pDefender->getTeam()))
			{
				iModifier = pAttacker->cityAttackModifier();

				if (iModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_CITY_MOD", iModifier));
				}
			}

			if (pPlot->isHills() || pPlot->isPeak())
			{
				iModifier = pAttacker->hillsAttackModifier();

				if (iModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_HILLS_MOD", iModifier));
				}
			}

			if (pPlot->getFeatureType() != NO_FEATURE)
			{
				iModifier = pAttacker->featureAttackModifier(pPlot->getFeatureType());

				if (iModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_UNIT_MOD", iModifier, GC.getFeatureInfo(pPlot->getFeatureType()).getTextKeyWide()));
				}
			}
			else
			{
				iModifier = pAttacker->terrainAttackModifier(pPlot->getTerrainType());

				if (iModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_UNIT_MOD", iModifier, GC.getTerrainInfo(pPlot->getTerrainType()).getTextKeyWide()));
				}
			}

			iModifier = pAttacker->rebelModifier(pDefender->getOwnerINLINE());
			if (iModifier != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_COMBAT_REBEL_MOD", iModifier));
			}
            ///TK Med
			if (pDefender->isNative() || GC.getUnitInfo(pDefender->getUnitType()).getCasteAttribute() == 7)
			{
				iModifier = GET_PLAYER(pAttacker->getOwnerINLINE()).getNativeCombatModifier();

				if (iModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_UNIT_NATIVE_COMBAT_MOD", iModifier));
				}
			}

			if (pAttacker->isHurt())
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_HP", pAttacker->currHitPoints(), pAttacker->maxHitPoints()));
			}

			szString.append(gDLL->getText("TXT_KEY_COLOR_REVERT"));

			szString.append(L' ');//XXX

			szString.append(gDLL->getText("TXT_KEY_COLOR_NEGATIVE"));

			szString.append(L' ');//XXX

			if (!(pAttacker->isRiver()))
			{
				if (pAttacker->plot()->isRiverCrossing(directionXY(pAttacker->plot(), pPlot)))
				{
					iModifier = GC.getRIVER_ATTACK_MODIFIER();

					if (iModifier != 0)
					{
						szString.append(NEWLINE);
						szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_RIVER_MOD", -(iModifier)));
					}
				}
			}

			if (!(pAttacker->isAmphib()))
			{
				if (!(pPlot->isWater()) && pAttacker->plot()->isWater())
				{
					iModifier = GC.getAMPHIB_ATTACK_MODIFIER();

					if (iModifier != 0)
					{
						szString.append(NEWLINE);
						szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_AMPHIB_MOD", -(iModifier)));
					}
				}
			}

			iModifier = pDefender->getExtraCombatPercent();

			if (iModifier != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_EXTRA_STRENGTH", iModifier));
			}

			iModifier = pDefender->unitClassDefenseModifier(pAttacker->getUnitClassType());

			if (iModifier != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_MOD_VS_TYPE", iModifier, GC.getUnitClassInfo(pAttacker->getUnitClassType()).getTextKeyWide()));
			}

			if (pAttacker->getUnitCombatType() != NO_UNITCOMBAT)
			{
				iModifier = pDefender->unitCombatModifier(pAttacker->getUnitCombatType());

				if (iModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_MOD_VS_TYPE", iModifier, GC.getUnitCombatInfo(pAttacker->getUnitCombatType()).getTextKeyWide()));
				}
			}

			iModifier = pDefender->domainModifier(pAttacker->getDomainType());

			if (iModifier != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_MOD_VS_TYPE", iModifier, GC.getDomainInfo(pAttacker->getDomainType()).getTextKeyWide()));
			}

			if (!(pDefender->noDefensiveBonus()))
			{
				iModifier = pPlot->defenseModifier(pDefender->getTeam());

				if (iModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_TILE_MOD", iModifier));
				}
			}

			iModifier = pDefender->fortifyModifier();

			if (iModifier != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_FORTIFY_MOD", iModifier));
			}

			if (pPlot->isCity(true, pDefender->getTeam()))
			{
				iModifier = pDefender->cityDefenseModifier();

				if (iModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_CITY_MOD", iModifier));
				}
			}

			if (pPlot->isHills() || pPlot->isPeak())
			{
				iModifier = pDefender->hillsDefenseModifier();

				if (iModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_HILLS_MOD", iModifier));
				}
			}

			if (pPlot->getFeatureType() != NO_FEATURE)
			{
				iModifier = pDefender->featureDefenseModifier(pPlot->getFeatureType());

				if (iModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_UNIT_MOD", iModifier, GC.getFeatureInfo(pPlot->getFeatureType()).getTextKeyWide()));
				}
			}
			else
			{
				iModifier = pDefender->terrainDefenseModifier(pPlot->getTerrainType());

				if (iModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_UNIT_MOD", iModifier, GC.getTerrainInfo(pPlot->getTerrainType()).getTextKeyWide()));
				}
			}

			iModifier = pDefender->rebelModifier(pAttacker->getOwnerINLINE());
			if (iModifier != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_COMBAT_REBEL_MOD", iModifier));
			}

			if (pAttacker->isNative()  || GC.getUnitInfo(pAttacker->getUnitType()).getCasteAttribute() == 7)
			{
				iModifier = GET_PLAYER(pDefender->getOwnerINLINE()).getNativeCombatModifier();

				if (iModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_UNIT_NATIVE_COMBAT_MOD", iModifier));
				}
			}

			if (pDefender->isHurt())
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_HP", pDefender->currHitPoints(), pDefender->maxHitPoints()));
			}

			if ((gDLL->getChtLvl() > 0))
			{
				szTempBuffer.Format(L"\nStack Compare Value = %d",
					gDLL->getInterfaceIFace()->getSelectionList()->AI_compareStacks(pPlot, false));
				szString.append(szTempBuffer);

				int iOurStrengthDefense = GET_PLAYER(GC.getGameINLINE().getActivePlayer()).AI_getOurPlotStrength(pPlot, 1, true, false);
				int iOurStrengthOffense = GET_PLAYER(GC.getGameINLINE().getActivePlayer()).AI_getOurPlotStrength(pPlot, 1, false, false);
				szTempBuffer.Format(L"\nPlot Strength(Ours)= d%d, o%d", iOurStrengthDefense, iOurStrengthOffense);
				szString.append(szTempBuffer);
				int iEnemyStrengthDefense = GET_PLAYER(GC.getGameINLINE().getActivePlayer()).AI_getEnemyPlotStrength(pPlot, 1, true, false);
				int iEnemyStrengthOffense = GET_PLAYER(GC.getGameINLINE().getActivePlayer()).AI_getEnemyPlotStrength(pPlot, 1, false, false);
				szTempBuffer.Format(L"\nPlot Strength(Enemy)= d%d, o%d", iEnemyStrengthDefense, iEnemyStrengthOffense);
				szString.append(szTempBuffer);
			}

			szString.append(gDLL->getText("TXT_KEY_COLOR_REVERT"));

			return true;
		}
	}

	return false;
}

///Tks Civics
void CvGameTextMgr::setRevolutionHelp(CvWStringBuffer& szBuffer, PlayerTypes ePlayer)
{
	szBuffer.assign(gDLL->getText("TXT_KEY_MISC_CANNOT_CHANGE_CIVICS"));

	if (GET_PLAYER(ePlayer).isAnarchy())
	{
		szBuffer.append(gDLL->getText("TXT_KEY_MISC_WHILE_IN_ANARCHY"));
	}
	else if (GET_PLAYER(ePlayer).getRevolutionTimer() > 0)
	{
		szBuffer.append(gDLL->getText("TXT_KEY_MISC_ANOTHER_REVOLUTION_RECENTLY"));
		szBuffer.append(L" : ");
		szBuffer.append(gDLL->getText("TXT_KEY_MISC_WAIT_MORE_TURNS", GET_PLAYER(ePlayer).getRevolutionTimer()));
	}
}
///Te

// DO NOT REMOVE - needed for font testing - Moose
void createTestFontString(CvWStringBuffer& szString)
{
	int iI;
	szString.assign(L"!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[?]^_`abcdefghijklmnopqrstuvwxyz\n");
	szString.append(L"{}~\\????G????T??????????S??F?O????????a??de??????�???p??st?f???????????���???????��?�??????");
	szString.append(L"\n");
	for (iI=0;iI<NUM_YIELD_TYPES;++iI)
		szString.append(CvWString::format(L"%c", GC.getYieldInfo((YieldTypes) iI).getChar()));
	szString.append(L"\n");
	for (iI=0;iI<GC.getNumSpecialBuildingInfos();++iI)
		szString.append(CvWString::format(L"%c", GC.getSpecialBuildingInfo((SpecialBuildingTypes) iI).getChar()));
	szString.append(L"\n");
	for (iI=0;iI<GC.getNumFatherPointInfos();++iI)
		szString.append(CvWString::format(L"%c", GC.getFatherPointInfo((FatherPointTypes) iI).getChar()));
	szString.append(L"\n");
	for (iI=0;iI<GC.getNumCivilizationInfos();++iI)
		szString.append(CvWString::format(L"%c", GC.getCivilizationInfo((CivilizationTypes) iI).getMissionaryChar()));
	szString.append(L"\n");
	for (iI = 0; iI < GC.getNumBonusInfos(); ++iI)
		szString.append(CvWString::format(L"%c", GC.getBonusInfo((BonusTypes) iI).getChar()));
	szString.append(L"\n");
	for (iI=0; iI<MAX_NUM_SYMBOLS; ++iI)
		szString.append(CvWString::format(L"%c", gDLL->getSymbolID(iI)));
}

void CvGameTextMgr::setPlotHelp(CvWStringBuffer& szString, CvPlot* pPlot)
{
	PROFILE_FUNC();

	int iI;

	// DO NOT REMOVE - needed for font testing - Moose
	if (gDLL->getTestingFont())
	{
		createTestFontString(szString);
		return;
	}

	CvWString szTempBuffer;
	ImprovementTypes eImprovement;
	PlayerTypes eRevealOwner;
	BonusTypes eBonus;
	bool bShift;
	bool bAlt;
	bool bCtrl;
	int iDefenseModifier;
	int iYield;
	int iTurns;

	bShift = gDLL->shiftKey();
	bAlt = gDLL->altKey();
	bCtrl = gDLL->ctrlKey();

	if ((gDLL->getChtLvl() > 0) && (bCtrl || bAlt || bShift))
	{
		szTempBuffer.Format(L"\n(%d, %d) (Oc: %d  / Crumbs: %d)", pPlot->getX_INLINE(), pPlot->getY_INLINE(), pPlot->getDistanceToOcean(), pPlot->getCrumbs());
		szString.append(szTempBuffer);
	}
	///TKs Med
//	if ((gDLL->getChtLvl() > 0) && (bCtrl))
//	{
//		szTempBuffer.Format(L"\n AI Strategy: %d)", );
//		szString.append(szTempBuffer);
//	}

	if (bShift && !bAlt && (gDLL->getChtLvl() > 0))
	{
		szString.append(L"\n");
		szString.append(GC.getTerrainInfo(pPlot->getTerrainType()).getDescription());

        ///TKs Med
//        szTempBuffer.Format(L"\nCrumbs: %d", pPlot->getCrumbs());
//        szString.append(szTempBuffer);

        szTempBuffer.Format(L"\nLatitude: %d", pPlot->getLatitude());
        szString.append(szTempBuffer);
            //Tke

		szTempBuffer.Format(L"\nArea: %d", pPlot->getArea());
		szString.append(szTempBuffer);

		char tempChar = 'x';
		if(pPlot->getRiverNSDirection() == CARDINALDIRECTION_NORTH)
		{
			tempChar = 'N';
		}
		else if(pPlot->getRiverNSDirection() == CARDINALDIRECTION_SOUTH)
		{
			tempChar = 'S';
		}
		szTempBuffer.Format(L"\nNSRiverFlow: %c", tempChar);
		szString.append(szTempBuffer);

		tempChar = 'x';
		if(pPlot->getRiverWEDirection() == CARDINALDIRECTION_WEST)
		{
			tempChar = 'W';
		}
		else if(pPlot->getRiverWEDirection() == CARDINALDIRECTION_EAST)
		{
			tempChar = 'E';
		}
		szTempBuffer.Format(L"\nWERiverFlow: %c", tempChar);
		szString.append(szTempBuffer);

		//Coast
		tempChar = 'x';
		///TKs Med Trade Routes
		//EuropeTypes eNearestEurope = pPlot->getNearestEurope();
		//EuropeTypes eNearestEurope = pPlot->getEurope();
		if (pPlot->getDistanceToOcean() != MAX_SHORT)
		{
			szTempBuffer.Format(L"\nDistance to Ocean: %d", pPlot->getDistanceToOcean());
			szString.append(szTempBuffer);
		}
		for (int iJ = 0; iJ < GC.getNumEuropeInfos(); iJ++)
		{
			if (pPlot->isTradeScreenAccessPlot((EuropeTypes)iJ))
			{
				szTempBuffer.Format(L"\nTrade Screen: %s", GC.getEuropeInfo((EuropeTypes)iJ).getHelp());
				szString.append(szTempBuffer);
			}
			else if (!GC.getEuropeInfo((EuropeTypes)iJ).isNoEuropePlot())
			{
				szTempBuffer.Format(L"\nDistance to %s: %d", GC.getEuropeInfo((EuropeTypes)iJ).getHelp(), pPlot->getDistanceToTradeScreen((EuropeTypes)iJ));
				szString.append(szTempBuffer);
			}
		}
        ///TKe
		if(pPlot->getRouteType() != NO_ROUTE)
		{
			szTempBuffer.Format(L"\nRoute: %s", GC.getRouteInfo(pPlot->getRouteType()).getDescription());
			szString.append(szTempBuffer);
		}

		if(pPlot->getRouteSymbol() != NULL)
		{
			szTempBuffer.Format(L"\nConnection: %i", gDLL->getRouteIFace()->getConnectionMask(pPlot->getRouteSymbol()));
			szString.append(szTempBuffer);
		}

		for (iI = 0; iI < MAX_PLAYERS; ++iI)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if (pPlot->getCulture((PlayerTypes)iI) > 0)
				{
					szTempBuffer.Format(L"\n%s Culture: %d", GET_PLAYER((PlayerTypes)iI).getName(), pPlot->getCulture((PlayerTypes)iI));
					szString.append(szTempBuffer);
				}
			}
		}

		PlayerTypes eActivePlayer = GC.getGameINLINE().getActivePlayer();
		int iActualFoundValue = pPlot->getFoundValue(eActivePlayer);
		int iCalcFoundValue = GET_PLAYER(eActivePlayer).AI_foundValue(pPlot->getX_INLINE(), pPlot->getY_INLINE(), -1, false);
		int iStartingFoundValue = GET_PLAYER(eActivePlayer).AI_foundValue(pPlot->getX_INLINE(), pPlot->getY_INLINE(), -1, true);

		szTempBuffer.Format(L"\nFound Value: %d, (%d, %d)", iActualFoundValue, iCalcFoundValue, iStartingFoundValue);
		szString.append(szTempBuffer);


		CvCity* pWorkingCity = pPlot->getWorkingCity();
		if (NULL != pWorkingCity)
		{
		    int iPlotIndex = pWorkingCity->getCityPlotIndex(pPlot);
            int iBuildValue = pWorkingCity->AI_getBestBuildValue(iPlotIndex);
            BuildTypes eBestBuild = pWorkingCity->AI_getBestBuild(iPlotIndex);
			int iCurrentValue = 0;
			BuildTypes eCurrentBuild = NO_BUILD;
            static_cast<CvCityAI*>(pWorkingCity)->AI_bestPlotBuild(pPlot, &iCurrentValue, &eCurrentBuild);
			//Tks Civics
			for (iI = 0; iI < NUM_YIELD_TYPES; ++iI)
			{
				if (pWorkingCity->getConnectedMissionYield((YieldTypes)iI) > 0)
				{
					szTempBuffer.Format(L"\nMission: %s (%d)", GC.getYieldInfo((YieldTypes)iI).getDescription(), pWorkingCity->getConnectedMissionYield((YieldTypes)iI));
					szString.append(szTempBuffer);
				}
				if (pWorkingCity->getConnectedTradeYield((YieldTypes)iI) > 0)
				{
					szTempBuffer.Format(L"\nTradePost: %s (%d)", GC.getYieldInfo((YieldTypes)iI).getDescription(), pWorkingCity->getConnectedTradeYield((YieldTypes)iI));
					szString.append(szTempBuffer);
				}
			}

		    if (NO_BUILD != eBestBuild)
            {
                szTempBuffer.Format(L"\nBest Build: %s (%d)", GC.getBuildInfo(eBestBuild).getDescription(), iBuildValue);
                szString.append(szTempBuffer);
			}

			if (NO_BUILD != eCurrentBuild)
			{
				szTempBuffer.Format(L"\nCurr Build: %s (%d)", GC.getBuildInfo(eCurrentBuild).getDescription(), iCurrentValue);
                szString.append(szTempBuffer);
            }

            ///TKs Invention Core Mod v 1.0
            if (pWorkingCity->getOwner() != NO_PLAYER)
            {
                if (GET_PLAYER(pWorkingCity->getOwner()).getCurrentResearch() != NO_CIVIC)
                {
                    CivicTypes eCivic = GET_PLAYER(pWorkingCity->getOwner()).getCurrentResearch();
                    szTempBuffer.Format(L"\nCurrent Research: %s", GC.getCivicInfo(eCivic).getDescription());

                    szString.append(szTempBuffer);
                    szString.append(CvWString::format(L"\nIdeas Stored: %d", GET_PLAYER(pWorkingCity->getOwner()).getIdeaProgress(eCivic)));
                    szString.append(CvWString::format(L"\nIdeas Experience: %d", GET_PLAYER(pWorkingCity->getOwner()).getIdeasExperience()));
                    szString.append(CvWString::format(L"\nIdeas Produced: %d", pWorkingCity->getRawYieldProduced(YIELD_IDEAS)));
                    szString.append(CvWString::format(L"\nIdeas Total Produced: %d", pWorkingCity->calculateActualYieldProduced(YIELD_IDEAS)));
                }
            }

            ///TKe
		}

		{
			szTempBuffer.Format(L"\nStack Str: land=%d(%d), sea=%d(%d)",
				pPlot->AI_sumStrength(NO_PLAYER, NO_PLAYER, DOMAIN_LAND, false, false, false),
				pPlot->AI_sumStrength(NO_PLAYER, NO_PLAYER, DOMAIN_LAND, true, false, false),
				pPlot->AI_sumStrength(NO_PLAYER, NO_PLAYER, DOMAIN_SEA, false, false, false),
				pPlot->AI_sumStrength(NO_PLAYER, NO_PLAYER, DOMAIN_SEA, true, false, false));
			szString.append(szTempBuffer);
		}
	}
	else if (!bShift && bAlt && (gDLL->getChtLvl() > 0))
	{
		CvUnit* pSelectedUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();

		if (pSelectedUnit != NULL)
		{
			int iPathTurns;
			if (pSelectedUnit->generatePath(pPlot, 0, false, &iPathTurns))
			{
				int iPathCost = pSelectedUnit->getPathCost();
				szString.append(CvWString::format(L"\nPathturns = %d, cost = %d", iPathTurns, iPathCost));
			}
		}

		//Distances to various things.
		if (pPlot->isOwned())
		{
			CvPlayerAI& kPlayer = GET_PLAYER(pPlot->getOwnerINLINE());
			CvPlayerAI& kActivePlayer = GET_PLAYER(GC.getGameINLINE().getActivePlayer());
			CvTeamAI& kTeam = GET_TEAM(kPlayer.getTeam());
            ///Tks Med
			szString.append(CvWString::format(L"\n FriendDist = %d, ECityDist = %d, EUnitDist = %d", kPlayer.AI_cityDistance(pPlot), kTeam.AI_enemyCityDistance(pPlot), kTeam.AI_enemyUnitDistance(pPlot)));
			int iCulture = kPlayer.countTotalCulture();
			int iActiveCulture = kActivePlayer.countTotalCulture();
            int iLand = kPlayer.getTotalLand();
            int iActiveLand = kActivePlayer.getTotalLand();

            szString.append(CvWString::format(L"\n Player Culture = %d, Player Land = %d\n Total = %d", iActiveCulture, iActiveLand, iActiveCulture + iActiveLand));
            if (pPlot->getOwnerINLINE() != GC.getGameINLINE().getActivePlayer())
			{
                iActiveCulture = (iActiveCulture + iActiveLand) * GC.getXMLval(XML_ALLIANCE_CULTURE_PERCENT_DENIAL) / 100;
                szString.append(CvWString::format(L"\n Vassal Percent = %d", iActiveCulture));
			}


			if (pPlot->getOwnerINLINE() != GC.getGameINLINE().getActivePlayer())
			{
                szString.append(CvWString::format(L"\n Total Culture = %d, Total Land = %d\n Total = %d", iCulture, iLand, iCulture + iLand));
                iActiveCulture = iActiveCulture - (iLand + iCulture);
                szString.append(CvWString::format(L"\n Become Vassal = %d", iActiveCulture));
			}
			///Tke
		}


		//Found Values
		szString.append(CvWString::format(L"\nFound Values"));
		for (int i = 0; i < MAX_PLAYERS; ++i)
		{
			CvPlayerAI& kLoopPlayer = GET_PLAYER((PlayerTypes)i);
			if (kLoopPlayer.isAlive() && !kLoopPlayer.isNative())
			{
				int iFoundValue = kLoopPlayer.AI_foundValue(pPlot->getX_INLINE(), pPlot->getY_INLINE());
				if (iFoundValue > 0)
				{
					szTempBuffer.Format(SETCOLR L"%s " ENDCOLR L"%d\n", kLoopPlayer.getPlayerTextColorR(), kLoopPlayer.getPlayerTextColorG(), kLoopPlayer.getPlayerTextColorB(), kLoopPlayer.getPlayerTextColorA(), kLoopPlayer.getCivilizationAdjective(), iFoundValue);
					szString.append(szTempBuffer);
				}
			}
		}
	}
	else if (bShift && bAlt && (gDLL->getChtLvl() > 0))
	{
		CvCity*	pCity = pPlot->getWorkingCity();
		if (pCity != NULL)
		{
			// some functions we want to call are not in CvCity, worse some are protected, so made us a friend
			CvCityAI* pCityAI = static_cast<CvCityAI*>(pCity);

			bool bAvoidGrowth = pCity->AI_avoidGrowth();
			bool bIgnoreGrowth = pCityAI->AI_ignoreGrowth();

			// if we over the city, then do an array of all the plots
			if (pPlot->getPlotCity() != NULL)
			{

				// check avoid growth
				if (bAvoidGrowth || bIgnoreGrowth)
				{
					szString.append(L"\n");

					// red color
					szString.append(CvWString::format(SETCOLR, TEXT_COLOR("COLOR_NEGATIVE_TEXT")));

					if (bAvoidGrowth)
					{
						szString.append(CvWString::format(L"AvoidGrowth"));

						if (bIgnoreGrowth)
							szString.append(CvWString::format(L", "));
					}

					if (bIgnoreGrowth)
						szString.append(CvWString::format(L"IgnoreGrowth"));

					// end color
					szString.append(CvWString::format( ENDCOLR ));
				}

				// if control key is down, ignore food
				bool bIgnoreFood = gDLL->ctrlKey();

				// line one is: blank, 20, 9, 10, blank
				szString.append(L"\n");
				setCityPlotYieldValueString(szString, pCity, -1, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 20, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 9, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 10, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);

				// line two is: 19, 8, 1, 2, 11
				szString.append(L"\n");
				setCityPlotYieldValueString(szString, pCity, 19, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 8, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 1, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 2, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 11, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);

				// line three is: 18, 7, 0, 3, 12
				szString.append(L"\n");
				setCityPlotYieldValueString(szString, pCity, 18, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 7, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 0, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 3, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 12, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);

				// line four is: 17, 6, 5, 4, 13
				szString.append(L"\n");
				setCityPlotYieldValueString(szString, pCity, 17, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 6, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 5, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 4, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 13, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);

				// line five is: blank, 16, 15, 14, blank
				szString.append(L"\n");
				setCityPlotYieldValueString(szString, pCity, -1, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 16, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 15, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 14, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
			}
			else
			{
				bool bWorkingPlot = pCity->isUnitWorkingPlot(pPlot);

				if (bWorkingPlot)
					szTempBuffer.Format( SETCOLR L"\n%s is working" ENDCOLR, TEXT_COLOR("COLOR_ALT_HIGHLIGHT_TEXT"), pCity->getName().GetCString());
				else
					szTempBuffer.Format( SETCOLR L"\n%s not working" ENDCOLR, TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), pCity->getName().GetCString());
				szString.append(szTempBuffer);

				int iValue = pCityAI->AI_plotValue(pPlot, bAvoidGrowth, /*bRemove*/ bWorkingPlot, /*bIgnoreFood*/ false, bIgnoreGrowth);
				int iJuggleValue = pCityAI->AI_plotValue(pPlot, bAvoidGrowth, /*bRemove*/ bWorkingPlot, false, bIgnoreGrowth, true);
				int iMagicValue = pCityAI->AI_getPlotMagicValue(pPlot);

				szTempBuffer.Format(L"\nvalue = %d\njuggle value = %d\nmagic value = %d", iValue, iJuggleValue, iMagicValue);
				szString.append(szTempBuffer);
			}
		}
	}
	else
	{
		eRevealOwner = pPlot->getRevealedOwner(GC.getGameINLINE().getActiveTeam(), true);

		if (eRevealOwner != NO_PLAYER)
		{
			if (pPlot->isActiveVisible(true))
			{
				szTempBuffer.Format(L"%d%% " SETCOLR L"%s" ENDCOLR, pPlot->calculateCulturePercent(eRevealOwner), GET_PLAYER(eRevealOwner).getPlayerTextColorR(), GET_PLAYER(eRevealOwner).getPlayerTextColorG(), GET_PLAYER(eRevealOwner).getPlayerTextColorB(), GET_PLAYER(eRevealOwner).getPlayerTextColorA(), GET_PLAYER(eRevealOwner).getCivilizationAdjective());
				szString.append(szTempBuffer);
				szString.append(NEWLINE);
				 ///TKs Med
				CvCity* pWorkingCity =  pPlot->getWorkingCity();
                if (pWorkingCity != NULL && pWorkingCity->getVassalOwner() != NO_PLAYER)
                {
                    //szTempBuffer.Format(L"\nVassal Owner: %s", GET_PLAYER(pWorkingCity->getVassalOwner()).getName());
                    //szString.append(szTempBuffer);
                    szString.append(gDLL->getText("TXT_KEY_GT_VASSAL_OWNER"));
                    PlayerTypes eVassalOwner = pWorkingCity->getVassalOwner();
                    szTempBuffer.Format(SETCOLR L" %s" ENDCOLR, GET_PLAYER(eVassalOwner).getPlayerTextColorR(), GET_PLAYER(eVassalOwner).getPlayerTextColorG(), GET_PLAYER(eVassalOwner).getPlayerTextColorB(), GET_PLAYER(eVassalOwner).getPlayerTextColorA(), GET_PLAYER(eVassalOwner).getCivilizationAdjective());
                    szString.append(szTempBuffer);
                    szString.append(NEWLINE);
                }
                ///TKe

				for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
				{
					if (iPlayer != eRevealOwner)
					{
						CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);
						if (kPlayer.isAlive() && pPlot->getCulture((PlayerTypes)iPlayer) > 0)
						{
							szTempBuffer.Format(L"%d%% " SETCOLR L"%s" ENDCOLR, pPlot->calculateCulturePercent((PlayerTypes)iPlayer), kPlayer.getPlayerTextColorR(), kPlayer.getPlayerTextColorG(), kPlayer.getPlayerTextColorB(), kPlayer.getPlayerTextColorA(), kPlayer.getCivilizationAdjective());
							szString.append(szTempBuffer);
							szString.append(NEWLINE);
						}
					}
				}

			}
			else
			{
				szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, GET_PLAYER(eRevealOwner).getPlayerTextColorR(), GET_PLAYER(eRevealOwner).getPlayerTextColorG(), GET_PLAYER(eRevealOwner).getPlayerTextColorB(), GET_PLAYER(eRevealOwner).getPlayerTextColorA(), GET_PLAYER(eRevealOwner).getCivilizationDescription());
				szString.append(szTempBuffer);
				szString.append(NEWLINE);
			}

		}

        ///Tks Med
        if (!pPlot->isCity())
        {
            if (GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getNumCities() > 0 && (eRevealOwner == NO_PLAYER || (eRevealOwner != GC.getGameINLINE().getActivePlayer() && GET_PLAYER(eRevealOwner).isNative())))
            {
                int iCost = 0;
                for (int i = 0; i < NUM_CITY_PLOTS; ++i)
                {
                    CvPlot* pLoopPlot = ::plotCity(pPlot->getX_INLINE(), pPlot->getY_INLINE(), i);
                    if (pLoopPlot != NULL)
                    {
                        if (!pLoopPlot->isRevealed(GC.getGameINLINE().getActiveTeam(), false))
                        {
                            iCost = 0;
                            break;
                        }
                        if (pLoopPlot->isOwned() && !pLoopPlot->isCity())
                        {
                            if (pLoopPlot->getRevealedOwner(GC.getGameINLINE().getActiveTeam(), true) != NO_PLAYER)
                            {
                                if (GET_PLAYER(pLoopPlot->getOwnerINLINE()).isNative() && !GET_TEAM(pLoopPlot->getTeam()).isAtWar(GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getTeam()))
                                {
                                    iCost += pLoopPlot->getBuyPrice(GC.getGameINLINE().getActivePlayer());
                                }
                            }

                        }
                    }
                }
                if (iCost > 0)
                {
					CvUnit* pSelectedUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();
					if (pSelectedUnit != NULL)
					{
						iCost -= iCost * GC.getUnitInfo(pSelectedUnit->getUnitType()).getTradeBonus() / 100;
					}
                    szString.append(gDLL->getText("TXT_KEY_PLOT_NATIVE_PURCHASE_COST", iCost));
                    szString.append(NEWLINE);
                }
            }
        }
        ///Tke
		iDefenseModifier = pPlot->defenseModifier((eRevealOwner != NO_PLAYER ? GET_PLAYER(eRevealOwner).getTeam() : NO_TEAM), true);

		if (iDefenseModifier != 0)
		{
			szString.append(gDLL->getText("TXT_KEY_PLOT_BONUS", iDefenseModifier));
			szString.append(NEWLINE);
		}

		if (pPlot->getTerrainType() != NO_TERRAIN)
		{
			if (pPlot->isPeak())
			{
				szString.append(gDLL->getText("TXT_KEY_PLOT_PEAK"));
			}
			else
			{
				if (pPlot->isWater())
				{
					szTempBuffer.Format(SETCOLR, TEXT_COLOR("COLOR_WATER_TEXT"));
					szString.append(szTempBuffer);
				}

				if (pPlot->isHills())
				{
					szString.append(gDLL->getText("TXT_KEY_PLOT_HILLS"));
				}

				if (pPlot->getFeatureType() != NO_FEATURE)
				{
					szTempBuffer.Format(L"%s/", GC.getFeatureInfo(pPlot->getFeatureType()).getDescription());
					szString.append(szTempBuffer);
				}

				szString.append(GC.getTerrainInfo(pPlot->getTerrainType()).getDescription());

				if (pPlot->isWater())
				{
					szString.append(ENDCOLR);
				}
			}
		}

		if (pPlot->hasYield())
		{
			for (iI = 0; iI < NUM_YIELD_TYPES; ++iI)
			{
				iYield = pPlot->calculatePotentialYield(((YieldTypes)iI), NULL, true);

				if (iYield != 0)
				{
					szTempBuffer.Format(L", %d%c", iYield, GC.getYieldInfo((YieldTypes) iI).getChar());
					szString.append(szTempBuffer);
				}
			}
		}

		if (pPlot->isLake())
		{
			szString.append(NEWLINE);
			szString.append(gDLL->getText("TXT_KEY_PLOT_FRESH_WATER_LAKE"));
		}

		if (pPlot->isImpassable())
		{
			szString.append(NEWLINE);
			szString.append(gDLL->getText("TXT_KEY_PLOT_IMPASSABLE"));
		}

		if (pPlot->getEurope() != NO_EUROPE)
		{
			szString.append(NEWLINE);
			szString.append(gDLL->getText("TXT_KEY_PLOT_EUROPE"));
		}

		eBonus = pPlot->getBonusType();
		if (eBonus != NO_BONUS)
		{
		     ///TKs Invention Core Mod v 1.0
		    bool bYieldNeedsResearched = false;
		     YieldTypes eBonusYield = NO_YIELD;
                for (int iCivic = 0; iCivic < GC.getNumCivicInfos(); ++iCivic)
                {
                    if (GC.getCivicInfo((CivicTypes) iCivic).getCivicOptionType() == CIVICOPTION_INVENTIONS)
                    {
                        for (int i = 0; i < GC.getNUM_YIELD_TYPES(); ++i)
                        {
                            if (GC.getBonusInfo(eBonus).getYieldChange((YieldTypes)i) > 0)
                            {
                                eBonusYield = (YieldTypes)i;
                                break;
                            }

                        }
                    }
                }
                if (eBonusYield != NO_YIELD)
                {
                    for (int iCivic = 0; iCivic < GC.getNumCivicInfos(); ++iCivic)
                    {
                        if (GC.getCivicInfo((CivicTypes) iCivic).getCivicOptionType() == CIVICOPTION_INVENTIONS)
                        {
                            CvCivicInfo& kCivicInfo = GC.getCivicInfo((CivicTypes) iCivic);
                            if (kCivicInfo.getAllowsYields(eBonusYield) > 0 || kCivicInfo.getAllowsBonuses(eBonus) > 0)
                            {
                                if (GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getIdeasResearched((CivicTypes) iCivic) == 0)
                                {
                                    bYieldNeedsResearched = true;
                                }
                            }
                        }
                    }
                }

            if (!bYieldNeedsResearched)
            {
                szTempBuffer.Format(L"%c " SETCOLR L"%s" ENDCOLR, GC.getBonusInfo(eBonus).getChar(), TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), GC.getBonusInfo(eBonus).getDescription());
            }
            else
            {
                szTempBuffer.Format(L"%c " SETCOLR L"%s" ENDCOLR, GC.getBonusInfo(eBonus).getChar(), TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), gDLL->getText("TXT_KEY_UNIDENTIFIED_PLANT").GetCString());
            }

		    ///TKe
			szString.append(NEWLINE);
			szString.append(szTempBuffer);
		}

		eImprovement = pPlot->getRevealedImprovementType(GC.getGameINLINE().getActiveTeam(), true);
		if (eImprovement != NO_IMPROVEMENT)
		{
			szString.append(NEWLINE);
			szString.append(GC.getImprovementInfo(eImprovement).getDescription());

			if (GC.getImprovementInfo(eImprovement).getImprovementUpgrade() != NO_IMPROVEMENT)
			{
				if ((pPlot->getUpgradeProgress() > 0) || pPlot->isBeingWorked())
				{
					iTurns = pPlot->getUpgradeTimeLeft(eImprovement, eRevealOwner);

					szString.append(gDLL->getText("TXT_KEY_PLOT_IMP_UPGRADE", iTurns, GC.getImprovementInfo((ImprovementTypes) GC.getImprovementInfo(eImprovement).getImprovementUpgrade()).getTextKeyWide()));
				}
				else
				{
					szString.append(gDLL->getText("TXT_KEY_PLOT_WORK_TO_UPGRADE", GC.getImprovementInfo((ImprovementTypes) GC.getImprovementInfo(eImprovement).getImprovementUpgrade()).getTextKeyWide()));
				}
			}
		}

		if (pPlot->getRevealedRouteType(GC.getGameINLINE().getActiveTeam(), true) != NO_ROUTE)
		{
			szString.append(NEWLINE);
			szString.append(GC.getRouteInfo(pPlot->getRevealedRouteType(GC.getGameINLINE().getActiveTeam(), true)).getDescription());
		}
	}
}


void CvGameTextMgr::setCityPlotYieldValueString(CvWStringBuffer &szString, CvCity* pCity, int iIndex, bool bAvoidGrowth, bool bIgnoreGrowth, bool bIgnoreFood)
{
	PROFILE_FUNC();

	CvPlot* pPlot = NULL;

	if (iIndex >= 0 && iIndex < NUM_CITY_PLOTS)
		pPlot = pCity->getCityIndexPlot(iIndex);

	if (pPlot != NULL)
	{
		CvCityAI* pCityAI = static_cast<CvCityAI*>(pCity);
		bool bWorkingPlot = pCity->isUnitWorkingPlot(iIndex);

		int iValue = pCityAI->AI_plotValue(pPlot, bAvoidGrowth, /*bRemove*/ bWorkingPlot, bIgnoreFood, bIgnoreGrowth);

		setYieldValueString(szString, iValue, /*bActive*/ bWorkingPlot);
	}
	else
	{
		setYieldValueString(szString, 0, /*bActive*/ false, /*bMakeWhitespace*/ true);
	}
}

void CvGameTextMgr::setYieldValueString(CvWStringBuffer &szString, int iValue, bool bActive, bool bMakeWhitespace)
{
	PROFILE_FUNC();

	static bool bUseFloats = false;

	if (bActive)
		szString.append(CvWString::format(SETCOLR, TEXT_COLOR("COLOR_ALT_HIGHLIGHT_TEXT")));
	else
		szString.append(CvWString::format(SETCOLR, TEXT_COLOR("COLOR_HIGHLIGHT_TEXT")));

	if (!bMakeWhitespace)
	{
		if (bUseFloats)
		{
			float fValue = ((float) iValue) / 10000;
			szString.append(CvWString::format(L"%2.3f " ENDCOLR, fValue));
		}
		else
			szString.append(CvWString::format(L"%04d  " ENDCOLR, iValue/10));
	}
	else
		szString.append(CvWString::format(L"         " ENDCOLR));
}

void CvGameTextMgr::setCityBarHelp(CvWStringBuffer &szString, CvCity* pCity)
{
	PROFILE_FUNC();


	szString.append(pCity->getName());

	bool bFirst = true;
	CvWStringBuffer szTempBuffer;
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		YieldTypes eYield = (YieldTypes) iYield;
		if (GC.getYieldInfo(eYield).isCargo())
		{
			int iStored = pCity->getYieldStored(eYield);
			if (iStored > 0)
			{
				if (bFirst)
				{
					bFirst = false;
				}
				else
				{
					szTempBuffer.append(L", ");
				}
				szTempBuffer.append(CvWString::format(L"%d%c", iStored, GC.getYieldInfo(eYield).getChar()));
			}
		}
	}

	if (!bFirst)
	{
		szString.append(NEWLINE);
		szString.append(gDLL->getText("TXT_KEY_CITY_BAR_STORED", szTempBuffer.getCString()));
	}

	bFirst = true;
	szTempBuffer.clear();
	int aiYields[NUM_YIELD_TYPES];
	pCity->calculateNetYields(aiYields);
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		if (aiYields[iYield] > 0)
		{
			if (bFirst)
			{
				bFirst = false;
			}
			else
			{
				szTempBuffer.append(L", ");
			}
			szTempBuffer.append(CvWString::format(L"%d%c", aiYields[iYield], GC.getYieldInfo((YieldTypes) iYield).getChar()));
		}
	}

	if (!bFirst)
	{
		szString.append(NEWLINE);
		szString.append(gDLL->getText("TXT_KEY_CITY_BAR_PRODUCING", szTempBuffer.getCString()));
	}

	szString.append(NEWLINE);
	int iFoodDifference = aiYields[YIELD_FOOD];
	if (iFoodDifference <= 0)
	{
		szString.append(gDLL->getText("TXT_KEY_CITY_BAR_GROWTH", pCity->getFood(), pCity->growthThreshold()));
	}
	else
	{
		szString.append(gDLL->getText("TXT_KEY_CITY_BAR_FOOD_GROWTH", pCity->getFood(), pCity->growthThreshold(), pCity->getFoodTurnsLeft()));
	}

	if (pCity->getProductionNeeded(YIELD_HAMMERS) != MAX_INT)
	{
		szString.append(NEWLINE);
		if (aiYields[YIELD_HAMMERS] > 0)
		{
			//szString.append(gDLL->getText("TXT_KEY_CITY_BAR_HAMMER_PRODUCTION", pCity->getProductionName(), pCity->getProduction(), pCity->getProductionNeeded(YIELD_HAMMERS), pCity->getProductionTurnsLeft()));
			///ray Hammer Icon Fix
			szString.append(gDLL->getText("TXT_KEY_CITY_BAR_HAMMER_PRODUCTION", pCity->getProductionName(), pCity->getProduction(), pCity->getProductionNeeded(YIELD_HAMMERS), pCity->getProductionTurnsLeft(), GC.getYieldInfo(YIELD_HAMMERS).getChar()));
		}
		else
		{
			//szString.append(gDLL->getText("TXT_KEY_CITY_BAR_PRODUCTION", pCity->getProductionName(), pCity->getProduction(), pCity->getProductionNeeded(YIELD_HAMMERS)));
			///ray Hammer Icon Fix
			szString.append(gDLL->getText("TXT_KEY_CITY_BAR_PRODUCTION", pCity->getProductionName(), pCity->getProduction(), pCity->getProductionNeeded(YIELD_HAMMERS), GC.getYieldInfo(YIELD_HAMMERS).getChar()));
		}
	}

	bFirst = true;
	for (int iI = 0; iI < GC.getNumBuildingInfos(); ++iI)
	{
		if (pCity->isHasRealBuilding((BuildingTypes)iI))
		{
			setListHelp(szString, NEWLINE, GC.getBuildingInfo((BuildingTypes)iI).getDescription(), L", ", bFirst);
			bFirst = false;
		}
	}

	if (pCity->getCultureLevel() != NO_CULTURELEVEL)
	{
		szString.append(NEWLINE);
		szString.append(gDLL->getText("TXT_KEY_CITY_BAR_CULTURE", pCity->getCulture(pCity->getOwnerINLINE()), pCity->getCultureThreshold(), GC.getCultureLevelInfo(pCity->getCultureLevel()).getTextKeyWide()));
	}

	szString.append(NEWLINE);

	szString.append(NEWLINE);
	szString.append(gDLL->getText("TXT_KEY_CITY_BAR_SELECT", pCity->getNameKey()));
	szString.append(NEWLINE);
	szString.append(gDLL->getText("TXT_KEY_CITY_BAR_SELECT_CTRL"));
	szString.append(NEWLINE);
	szString.append(gDLL->getText("TXT_KEY_CITY_BAR_SELECT_ALT"));
}


void CvGameTextMgr::parseTraits(CvWStringBuffer &szHelpString, TraitTypes eTrait, CivilizationTypes eCivilization, bool bDawnOfMan, bool bIndent)
{
	PROFILE_FUNC();

	CvWString szTempBuffer;

	CvTraitInfo& kTrait = GC.getTraitInfo(eTrait);

	// Trait Name
	if (bIndent)
	{
		CvWString szText = kTrait.getDescription();
		if (bDawnOfMan)
		{
			szTempBuffer.Format(L"%s", szText.GetCString());
		}
		else
		{
			szTempBuffer.Format(NEWLINE SETCOLR L"%s" ENDCOLR, TEXT_COLOR("COLOR_ALT_HIGHLIGHT_TEXT"), szText.GetCString());
		}
		szHelpString.append(szTempBuffer);
	}

	if (!bDawnOfMan)
	{
		if (!isEmpty(kTrait.getHelp()))
		{
			szHelpString.append(kTrait.getHelp());
		}

		//TK Tech Categories
		for (int iI = 0; iI < kTrait.getNumBonusTechCategories(); ++iI)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_TECH_CATEGORY_BONUS", kTrait.getTechCategoryBonus(iI), GC.getCivicInfo((CivicTypes)kTrait.getBonusTechCategory(iI)).getDescription()));
		}

		// iLevelExperienceModifier
		if (kTrait.getLevelExperienceModifier() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_CIVIC_LEVEL_MODIFIER", kTrait.getLevelExperienceModifier()));
		}

		// iGreatGeneralRateModifier
		if (kTrait.getGreatGeneralRateModifier() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_GREAT_GENERAL_MODIFIER", kTrait.getGreatGeneralRateModifier()));
		}

		if (kTrait.getDomesticGreatGeneralRateModifier() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_DOMESTIC_GREAT_GENERAL_MODIFIER", kTrait.getDomesticGreatGeneralRateModifier()));
		}

		if (kTrait.getNativeAngerModifier() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			if (kTrait.getNativeAngerModifier() > 0)
			{
				szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_NATIVE_ANGER_MODIFIER_PLUS"));
			}
			else
			{
				szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_NATIVE_ANGER_MODIFIER_MINUS"));
			}
		}

		if (kTrait.getLearnTimeModifier() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_LEARN_TIME_MODIFIER", kTrait.getLearnTimeModifier()));
		}

		if (kTrait.getMercantileFactor() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			if (kTrait.getMercantileFactor() > 0)
			{
				szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_MARKET_SENSITIVE_HELP"));
			}
			else
			{
				szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_MERCANTILE_HELP"));
			}
		}

		int iTreasureModifier = 100 + kTrait.getTreasureModifier();
		if (iTreasureModifier != 100)
		{
			if (eCivilization != NO_CIVILIZATION)
			{
				//TKs Civics Effect
				if (GC.getCivilizationInfo(eCivilization).getTreasure() > 0)
				{
					iTreasureModifier *= GC.getCivilizationInfo(eCivilization).getTreasure();
					iTreasureModifier /= 100;
				}
				//Tke
			}

			if ((iTreasureModifier > 0) && (iTreasureModifier != 100))
			{
				szHelpString.append(NEWLINE);
				if (bIndent)
				{
					szHelpString.append(L"  ");
				}
				szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_TREASURE_MODIFIER", iTreasureModifier - 100));
			}
		}

		if (kTrait.getChiefGoldModifier() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_CHIEF_GOLD_MODIFIER", kTrait.getChiefGoldModifier()));
		}

		// native combat
		if (kTrait.getNativeCombatModifier() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_UNIT_NATIVE_COMBAT_MOD", kTrait.getNativeCombatModifier()));
		}

		if (kTrait.getMissionaryModifier() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_FATHER_EXTRA_MISSIONARY_RATE", kTrait.getMissionaryModifier()));
		}

		if (kTrait.getRebelCombatModifier() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_REBEL_COMBAT_MOD", kTrait.getRebelCombatModifier()));
		}

		if (kTrait.getTaxRateThresholdModifier() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_TAX_RATE_THRESHOLD_MOD", kTrait.getTaxRateThresholdModifier()));
		}

		// CityExtraYield
		for (int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
		{
			if (kTrait.getCityExtraYield(iI) > 0)
			{
				szHelpString.append(NEWLINE);
				if (bIndent)
				{
					szHelpString.append(L"  ");
				}
				szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_CITY_EXTRA_YIELD", kTrait.getCityExtraYield(iI), GC.getYieldInfo((YieldTypes) iI).getChar()));
			}
		}

		// ExtraYieldThresholds
		for (int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
		{
			if (kTrait.getExtraYieldThreshold(iI) > 0)
			{
				szHelpString.append(NEWLINE);
				if (bIndent)
				{
					szHelpString.append(L"  ");
				}
				szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_EXTRA_YIELD_THRESHOLDS", GC.getYieldInfo((YieldTypes) iI).getChar(), kTrait.getExtraYieldThreshold(iI), GC.getYieldInfo((YieldTypes) iI).getChar()));
			}
		}

		for (int iI = 0; iI < GC.getNumProfessionInfos(); ++iI)
		{
			if (kTrait.getProfessionEquipmentModifier(iI) != 0)
			{
				szHelpString.append(NEWLINE);
				if (bIndent)
				{
					szHelpString.append(L"  ");
				}
				szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_PROFESSION_YIELD_EQUIPMENT_MOD", kTrait.getProfessionEquipmentModifier(iI), GC.getProfessionInfo((ProfessionTypes) iI).getTextKeyWide()));
			}
		}

		// Free Promotions
		bool bFoundPromotion = false;
		szTempBuffer.clear();
		for (int iI = 0; iI < GC.getNumPromotionInfos(); ++iI)
		{
			if (kTrait.isFreePromotion(iI))
			{
				if (bFoundPromotion)
				{
					szTempBuffer += L", ";
				}
///Tks Med
				szTempBuffer += CvWString::format(L"<link=literal>%s</link>", GC.getPromotionInfo((PromotionTypes) iI).getDescription());
                szHelpString.append(NEWLINE);
                if (bIndent)
                {
                    szHelpString.append(L"  ");
                }
                szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_FREE_PROMOTIONS", szTempBuffer.GetCString()));
                //szHelpString.append(gDLL->getText("Free Promotion"));
                setPromotionHelp(szHelpString, (PromotionTypes)iI, true);
//				///TKs Med
//                szHelpString.append(NEWLINE);
//                parsePromotionHelp(szTempBuffer, (PromotionTypes)kUnitInfo.getLeaderPromotion(), L"\n   ");
//                szHelpString.append(szTempBuffer);
//                ///TKe
				bFoundPromotion = true;
			}
		}

		if (bFoundPromotion)
		{
//			szHelpString.append(NEWLINE);
//			if (bIndent)
//			{
//				szHelpString.append(L"  ");
//			}
//			szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_FREE_PROMOTIONS", szTempBuffer.GetCString()));

			for (int iJ = 0; iJ < GC.getNumUnitCombatInfos(); iJ++)
			{
				if (kTrait.isFreePromotionUnitCombat(iJ))
				{
					szTempBuffer.Format(L"\n        %c%s", gDLL->getSymbolID(BULLET_CHAR), GC.getUnitCombatInfo((UnitCombatTypes)iJ).getDescription());
					szHelpString.append(szTempBuffer);
				}
			}

			for (int iJ = 0; iJ < GC.getNumUnitClassInfos(); iJ++)
			{
				if (kTrait.isFreePromotionUnitClass(iJ))
				{
					szTempBuffer.Format(L"\n        %c%s", gDLL->getSymbolID(BULLET_CHAR), GC.getUnitClassInfo((UnitClassTypes)iJ).getDescription());
					szHelpString.append(szTempBuffer);
				}
			}

			for (int iJ = 0; iJ < GC.getNumProfessionInfos(); iJ++)
			{
				if (kTrait.isFreePromotionUnitProfession(iJ))
				{
					szTempBuffer.Format(L"\n        %c%s(Profession)", gDLL->getSymbolID(BULLET_CHAR), GC.getProfessionInfo((ProfessionTypes)iJ).getDescription());
					szHelpString.append(szTempBuffer);
				}
			}
			///TKe
		}

		// Increase Building/Unit Production Speeds
		int iLast = 0;
		for (int iI = 0; iI < GC.getNumSpecialUnitInfos(); ++iI)
		{
			if (GC.getSpecialUnitInfo((SpecialUnitTypes) iI).getProductionTraits(eTrait) != 0)
			{
				CvWString szText(NEWLINE);
				if (bIndent)
				{
					szText += L"  ";
				}

				if (GC.getSpecialUnitInfo((SpecialUnitTypes) iI).getProductionTraits(eTrait) == 100)
				{
					szText += gDLL->getText("TXT_KEY_TRAIT_DOUBLE_SPEED");
				}
				else
				{
					szText += gDLL->getText("TXT_KEY_TRAIT_PRODUCTION_MODIFIER", GC.getSpecialUnitInfo((SpecialUnitTypes) iI).getProductionTraits(eTrait));
				}
				setListHelp(szHelpString, szText.GetCString(), GC.getSpecialUnitInfo((SpecialUnitTypes) iI).getDescription(), L", ", (GC.getSpecialUnitInfo((SpecialUnitTypes) iI).getProductionTraits(eTrait) != iLast));
				iLast = GC.getSpecialUnitInfo((SpecialUnitTypes) iI).getProductionTraits(eTrait);
			}
		}

		// Unit Classes
		iLast = 0;
		for (int iI = 0; iI < GC.getNumUnitClassInfos();++iI)
		{
			UnitTypes eLoopUnit;
			if (eCivilization == NO_CIVILIZATION)
			{
				eLoopUnit = ((UnitTypes)(GC.getUnitClassInfo((UnitClassTypes)iI).getDefaultUnitIndex()));
			}
			else
			{
				eLoopUnit = ((UnitTypes)(GC.getCivilizationInfo(eCivilization).getCivilizationUnits(iI)));
			}

			if (eLoopUnit != NO_UNIT)
			{
				if (GC.getUnitInfo(eLoopUnit).getProductionTraits(eTrait) != 0)
				{
					CvWString szText(NEWLINE);
					if (bIndent)
					{
						szText += L"  ";
					}
					if (GC.getUnitInfo(eLoopUnit).getProductionTraits(eTrait) == 100)
					{
						szText += gDLL->getText("TXT_KEY_TRAIT_DOUBLE_SPEED");
					}
					else
					{
						szText += gDLL->getText("TXT_KEY_TRAIT_PRODUCTION_MODIFIER", GC.getUnitInfo(eLoopUnit).getProductionTraits(eTrait));
					}

					CvWString szUnit;
					szUnit.Format(L"<link=literal>%s</link>", GC.getUnitInfo(eLoopUnit).getDescription());
					setListHelp(szHelpString, szText.GetCString(), szUnit, L", ", (GC.getUnitInfo(eLoopUnit).getProductionTraits(eTrait) != iLast));
					iLast = GC.getUnitInfo(eLoopUnit).getProductionTraits(eTrait);
				}
			}
		}

		// SpecialBuildings
		iLast = 0;
		for (int iI = 0; iI < GC.getNumSpecialBuildingInfos(); ++iI)
		{
			if (GC.getSpecialBuildingInfo((SpecialBuildingTypes) iI).getProductionTraits(eTrait) != 0)
			{
				CvWString szText(NEWLINE);
				if (bIndent)
				{
					szText += L"  ";
				}
				if (GC.getSpecialBuildingInfo((SpecialBuildingTypes) iI).getProductionTraits(eTrait) == 100)
				{
					szText += gDLL->getText("TXT_KEY_TRAIT_DOUBLE_SPEED");
				}
				else
				{
					szText += gDLL->getText("TXT_KEY_TRAIT_PRODUCTION_MODIFIER", GC.getSpecialBuildingInfo((SpecialBuildingTypes) iI).getProductionTraits(eTrait));
				}
				setListHelp(szHelpString, szText.GetCString(), GC.getSpecialBuildingInfo((SpecialBuildingTypes) iI).getDescription(), L", ", (GC.getSpecialBuildingInfo((SpecialBuildingTypes) iI).getProductionTraits(eTrait) != iLast));
				iLast = GC.getSpecialBuildingInfo((SpecialBuildingTypes) iI).getProductionTraits(eTrait);
			}
		}

		// Buildings
		iLast = 0;
		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); ++iI)
		{
			BuildingTypes eLoopBuilding;
			if (eCivilization == NO_CIVILIZATION)
			{
				eLoopBuilding = ((BuildingTypes)(GC.getBuildingClassInfo((BuildingClassTypes)iI).getDefaultBuildingIndex()));
			}
			else
			{
				eLoopBuilding = ((BuildingTypes)(GC.getCivilizationInfo(eCivilization).getCivilizationBuildings(iI)));
			}

			if (eLoopBuilding != NO_BUILDING)
			{
				if (GC.getBuildingInfo(eLoopBuilding).getProductionTraits(eTrait) != 0)
				{
					CvWString szText(NEWLINE);
					if (bIndent)
					{
						szText += L"  ";
					}
					if (GC.getBuildingInfo(eLoopBuilding).getProductionTraits(eTrait) == 100)
					{
						szText += gDLL->getText("TXT_KEY_TRAIT_DOUBLE_SPEED");
					}
					else
					{
						szText += gDLL->getText("TXT_KEY_TRAIT_PRODUCTION_MODIFIER", GC.getBuildingInfo(eLoopBuilding).getProductionTraits(eTrait));
					}

					CvWString szBuilding;
					szBuilding.Format(L"<link=literal>%s</link>", GC.getBuildingInfo(eLoopBuilding).getDescription());
					setListHelp(szHelpString, szText.GetCString(), szBuilding, L", ", (GC.getBuildingInfo(eLoopBuilding).getProductionTraits(eTrait) != iLast));
					iLast = GC.getBuildingInfo(eLoopBuilding).getProductionTraits(eTrait);
				}
			}
		}

		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			if (kTrait.getYieldModifier(iYield) != 0)
			{
				szTempBuffer = gDLL->getText("TXT_KEY_PERCENT_CHANGE_ALL_CITIES", kTrait.getYieldModifier(iYield), GC.getYieldInfo((YieldTypes)iYield).getChar());
				szHelpString.append(NEWLINE);
				if (bIndent)
				{
					szHelpString.append(L"  ");
				}
				szHelpString.append(szTempBuffer);
			}
		}

		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			if (kTrait.isTaxYieldModifier(iYield))
			{
				szTempBuffer = gDLL->getText("TXT_KEY_TAX_RATE_YIELD_INCREASE", GC.getYieldInfo((YieldTypes)iYield).getChar());
				szHelpString.append(NEWLINE);
				if (bIndent)
				{
					szHelpString.append(L"  ");
				}
				szHelpString.append(szTempBuffer);
			}
		}

		for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); ++iBuildingClass)
		{
			BuildingTypes eBuilding = (BuildingTypes) GC.getBuildingClassInfo((BuildingClassTypes) iBuildingClass).getDefaultBuildingIndex();

			if (eCivilization != NO_CIVILIZATION)
			{
				eBuilding = (BuildingTypes) GC.getCivilizationInfo(eCivilization).getCivilizationBuildings(iBuildingClass);
			}

			if (eBuilding != NO_BUILDING)
			{
				for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
				{
					if (kTrait.getBuildingYieldChange(iBuildingClass, iYield) != 0)
					{
						szTempBuffer = gDLL->getText("TXT_KEY_BUILDING_YIELD_INCREASE", kTrait.getBuildingYieldChange(iBuildingClass, iYield), GC.getYieldInfo((YieldTypes)iYield).getChar(), GC.getBuildingInfo(eBuilding).getTextKeyWide());
						szHelpString.append(NEWLINE);
						if (bIndent)
						{
							szHelpString.append(L"  ");
						}
						szHelpString.append(szTempBuffer);
					}
				}
			}
		}

		for (int iUnitClass = 0; iUnitClass < GC.getNumUnitClassInfos(); ++iUnitClass)
		{
			UnitTypes eUnit = (UnitTypes) GC.getUnitClassInfo((UnitClassTypes) iUnitClass).getDefaultUnitIndex();

			if (eCivilization != NO_CIVILIZATION)
			{
				eUnit = (UnitTypes) GC.getCivilizationInfo(eCivilization).getCivilizationUnits(iUnitClass);
			}

			if (eUnit != NO_UNIT)
			{
				if (kTrait.getUnitMoveChange(iUnitClass) != 0)
				{
					szTempBuffer = gDLL->getText("TXT_KEY_UNIT_MOVES_INCREASE", kTrait.getUnitMoveChange(iUnitClass), GC.getUnitInfo(eUnit).getTextKeyWide());
					szHelpString.append(NEWLINE);
					if (bIndent)
					{
						szHelpString.append(L"  ");
					}
					szHelpString.append(szTempBuffer);
				}

				if (kTrait.getUnitStrengthModifier(iUnitClass) != 0)
				{
					szTempBuffer = gDLL->getText("TXT_KEY_UNIT_STRENGTH_INCREASE", kTrait.getUnitStrengthModifier(iUnitClass), GC.getUnitInfo(eUnit).getTextKeyWide());
					szHelpString.append(NEWLINE);
					if (bIndent)
					{
						szHelpString.append(L"  ");
					}
					szHelpString.append(szTempBuffer);
				}
			}
		}

		for (int iProfession = 0; iProfession < GC.getNumProfessionInfos(); ++iProfession)
		{
			if (eCivilization == NO_CIVILIZATION || GC.getCivilizationInfo(eCivilization).isValidProfession(iProfession))
			{
				if (kTrait.getProfessionMoveChange(iProfession) != 0)
				{
					szTempBuffer = gDLL->getText("TXT_KEY_UNIT_MOVES_INCREASE", kTrait.getProfessionMoveChange(iProfession), GC.getProfessionInfo((ProfessionTypes)iProfession).getTextKeyWide());
					szHelpString.append(NEWLINE);
					if (bIndent)
					{
						szHelpString.append(L"  ");
					}
					szHelpString.append(szTempBuffer);
				}
			}
		}

		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); ++iI)
		{
			if (kTrait.isFreeBuildingClass(iI))
			{
				BuildingTypes eFreeBuilding;
				if (eCivilization != NO_CIVILIZATION)
				{
					eFreeBuilding = ((BuildingTypes)(GC.getCivilizationInfo(eCivilization).getCivilizationBuildings(iI)));
				}
				else
				{
					eFreeBuilding = (BuildingTypes)GC.getBuildingClassInfo((BuildingClassTypes)iI).getDefaultBuildingIndex();
				}

				if (NO_BUILDING != eFreeBuilding)
				{
					szHelpString.append(NEWLINE);
					if (bIndent)
					{
						szHelpString.append(L"  ");
					}
					szHelpString.append(gDLL->getText("TXT_KEY_BUILDING_FREE_IN_CITY", GC.getBuildingInfo(eFreeBuilding).getTextKeyWide()));
				}
			}
		}

		iLast = 0;
		for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); ++iBuildingClass)
		{
			BuildingTypes eBuilding = (BuildingTypes) GC.getBuildingClassInfo((BuildingClassTypes) iBuildingClass).getDefaultBuildingIndex();

			if (eCivilization != NO_CIVILIZATION)
			{
				eBuilding = (BuildingTypes) GC.getCivilizationInfo(eCivilization).getCivilizationBuildings(iBuildingClass);
			}

			if (eBuilding != NO_BUILDING)
			{
				int iModifier = kTrait.getBuildingProductionModifier(iBuildingClass);
				if (iModifier != 0)
				{
					CvWString szText = NEWLINE;
					if (bIndent)
					{
						szText += L"  ";
					}

					if (iModifier == 100)
					{
						szText += gDLL->getText("TXT_KEY_TRAIT_DOUBLE_SPEED");
					}
					else
					{
						szText += gDLL->getText("TXT_KEY_TRAIT_PRODUCTION_MODIFIER", iModifier);
					}

					CvWString szBuilding;
					szBuilding.Format(L"<link=literal>%s</link>", GC.getBuildingInfo(eBuilding).getDescription());
					setListHelp(szHelpString, szText.GetCString(), szBuilding, L", ", iModifier != iLast);
					iLast = iModifier;
				}
			}
		}

		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			int iModifier = kTrait.getBuildingRequiredYieldModifier(iYield);
			if (iModifier != 0)
			{
				szTempBuffer = gDLL->getText("TXT_KEY_REQUIRED_YIELD_MODIFIER", iModifier, GC.getYieldInfo((YieldTypes)iYield).getChar());
				szHelpString.append(NEWLINE);
				if (bIndent)
				{
					szHelpString.append(L"  ");
				}
				szHelpString.append(szTempBuffer);
			}
		}

		if (kTrait.getNativeAttitudeChange() > 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_FATHER_NATIVE_ATTITUDE_GOOD"));
			szHelpString.append(NEWLINE);
			szHelpString.append(gDLL->getText("TXT_KEY_FATHER_NATIVE_ATTITUDE_GOOD2"));
		}
		else if (kTrait.getNativeAttitudeChange() < 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_FATHER_NATIVE_ATTITUDE_BAD"));
		}

		if (kTrait.getCityDefense() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_FATHER_CITY_DEFENSE", kTrait.getCityDefense()));
		}

		if (kTrait.getLandPriceDiscount() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_FATHER_LAND_DISCOUNT", kTrait.getLandPriceDiscount()));
		}

		if (kTrait.getRecruitPriceDiscount() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_FATHER_RECRUIT_DISCOUNT", -kTrait.getRecruitPriceDiscount()));
		}

		if (kTrait.getEuropeTravelTimeModifier() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_FATHER_EUROPE_TRAVEL_MODIFIER", kTrait.getEuropeTravelTimeModifier()));
		}

		if (kTrait.getImmigrationThresholdModifier() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_FATHER_IMMIGRATION_THRESHOLD_MODIFIER", kTrait.getImmigrationThresholdModifier(), GC.getYieldInfo(YIELD_CROSSES).getChar()));
		}

		for (int iGoody = 0; iGoody < GC.getNumGoodyInfos(); ++iGoody)
		{
			CvGoodyInfo& kGoodyInfo = GC.getGoodyInfo((GoodyTypes) iGoody);
			if (kTrait.getGoodyFactor(iGoody) > 1)
			{
				szHelpString.append(NEWLINE);
				if (bIndent)
				{
					szHelpString.append(L"  ");
				}
				szHelpString.append(gDLL->getText("TXT_KEY_FATHER_EXTRA_GOODY", kTrait.getGoodyFactor(iGoody), kGoodyInfo.getTextKeyWide()));
			}
			else if (kTrait.getGoodyFactor(iGoody) == 0)
			{
				szHelpString.append(NEWLINE);
				if (bIndent)
				{
					szHelpString.append(L"  ");
				}
				szHelpString.append(gDLL->getText("TXT_KEY_FATHER_NO_GOODY", kGoodyInfo.getTextKeyWide()));
			}
		}

	}
}

void CvGameTextMgr::parseLeaderTraits(CvWStringBuffer &szHelpString, LeaderHeadTypes eLeader, CivilizationTypes eCivilization, bool bDawnOfMan, bool bCivilopediaText)
{
	PROFILE_FUNC();

	CvWString szTempBuffer;	// Formatting
	int iI;

	//	Build help string
	if (eLeader != NO_LEADER)
	{
		if (!bDawnOfMan && !bCivilopediaText)
		{
			szTempBuffer.Format( SETCOLR L"%s" ENDCOLR , TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), GC.getLeaderHeadInfo(eLeader).getDescription());
			szHelpString.append(szTempBuffer);
		}

		FAssert((GC.getNumTraitInfos() > 0) &&
			"GC.getNumTraitInfos() is less than or equal to zero but is expected to be larger than zero in CvSimpleCivPicker::setLeaderText");
        ///Tks Med
        if (!bDawnOfMan)
        {
            int EconomyType = GC.getLeaderHeadInfo(eLeader).getEconomyType();
            if (eCivilization != NO_CIVILIZATION)
            {
                if (GC.getCivilizationInfo(eCivilization).isWaterStart())
                {
                    szHelpString.append(NEWLINE);
                    szHelpString.append(gDLL->getText("TXT_KEY_CIVILIZATION_WATER_START"));
                }
                else
                {
                    szHelpString.append(NEWLINE);
                    szHelpString.append(gDLL->getText("TXT_KEY_CIVILIZATION_LAND_START"));
                }
            }

            if (EconomyType == 1)
            {
                szHelpString.append(NEWLINE);
                szHelpString.append(gDLL->getText("TXT_KEY_CIVILIZATION_INVADER"));
            }
//            for (int iEra; iEra < GC.getNumEraInfos(); iEra++)
//            {
//                if (GC.getLeaderHeadInfo(eLeader).getEraResearchMod((EraTypes)iEra) > 0)
//                {
//                    szHelpString.append(NEWLINE);
//                    szHelpString.append(gDLL->getText("TXT_KEY_ERA_RESEARCH_MOD", GC.getLeaderHeadInfo(eLeader).getEraResearchMod((EraTypes)iEra), GC.getEraInfo((EraTypes)iEra).getTextKeyWide()));
//                }
//            }
        }

		///TKs Invention Core Mod v 1.0
		bool bFirst = true;
		for (iI = 0; iI < GC.getNumTraitInfos(); ++iI)
		{
			if (GC.getLeaderHeadInfo(eLeader).hasTrait(iI) || (eCivilization != NO_CIVILIZATION && GC.getCivilizationInfo(eCivilization).hasTrait(iI)))
			{
				if (!bFirst)
				{
					if (bDawnOfMan)
					{
						szHelpString.append(L", ");
					}
				}
				else
				{
					bFirst = false;
				}

				parseTraits(szHelpString, ((TraitTypes)iI), eCivilization, bDawnOfMan);
			}
		}
		if (!bDawnOfMan && eCivilization != NO_CIVILIZATION)
		{
            bool bFoundTech = false;
			bool bInGame = GC.getGameINLINE().getActivePlayer() == NO_PLAYER;
            for (int iLoopCivic = 0; iLoopCivic < GC.getNumCivicInfos(); ++iLoopCivic)
            {
                if (GC.getCivilizationInfo(eCivilization).getCivilizationTechs(iLoopCivic) != -1)
                {
                    if (bFoundTech == false)
                    {
                        szHelpString.append(NEWLINE);
                        szHelpString.append(gDLL->getText("TXT_KEYCIVILIZATION_FREE_TECHS"));
                        bFoundTech = true;
                    }
                    szHelpString.append(NEWLINE);
                    CvWStringBuffer szBuffer;
                    CvCivicInfo& kLoopCivicInfo = GC.getCivicInfo((CivicTypes) iLoopCivic);
                    szHelpString.append(gDLL->getText("TXT_KEY_YELLOW_NAME", kLoopCivicInfo.getDescription()));
                    GAMETEXT.parseCivicInfo(szBuffer, (CivicTypes) iLoopCivic, bInGame, false, true, false, eCivilization);
                    szHelpString.append(szBuffer);
                }
            }
			bFoundTech = false;
			for (int iLoopCivic = 0; iLoopCivic < GC.getLeaderHeadInfo(eLeader).getNumCivicDiplomacyAttitudes(); ++iLoopCivic)
            {
				if (GC.getLeaderHeadInfo(eLeader).getCivicDiplomacyAttitudesValue(iLoopCivic) > 0)
				{
                
					if (bFoundTech == false)
					{
						szHelpString.append(NEWLINE);
						szHelpString.append(gDLL->getText("TXT_KEYCIVILIZATION_FAVORED_CIVICS"));
						bFoundTech = true;
					}
					szHelpString.append(NEWLINE);
					CvWStringBuffer szBuffer;
					CvCivicInfo& kLoopCivicInfo = GC.getCivicInfo((CivicTypes)GC.getLeaderHeadInfo(eLeader).getCivicDiplomacyAttitudes(iLoopCivic));
					szHelpString.append(gDLL->getText("TXT_KEY_YELLOW_NAME", kLoopCivicInfo.getDescription()));
					//GAMETEXT.parseCivicInfo(szBuffer, (CivicTypes) iLoopCivic, bInGame, false, true, false, eCivilization);
					//szHelpString.append(szBuffer);
				}
                
            }
			bFoundTech = false;
			for (int iLoopCivic = 0; iLoopCivic < GC.getLeaderHeadInfo(eLeader).getNumCivicDiplomacyAttitudes(); ++iLoopCivic)
            {
				
				if (GC.getLeaderHeadInfo(eLeader).getCivicDiplomacyAttitudesValue(iLoopCivic) < 0)
				{
                
					if (bFoundTech == false)
					{
						szHelpString.append(NEWLINE);
						szHelpString.append(gDLL->getText("TXT_KEYCIVILIZATION_DISTAINED_CIVICS"));
						bFoundTech = true;
					}
					szHelpString.append(NEWLINE);
					CvWStringBuffer szBuffer;
					CvCivicInfo& kLoopCivicInfo = GC.getCivicInfo((CivicTypes) GC.getLeaderHeadInfo(eLeader).getCivicDiplomacyAttitudes(iLoopCivic));
					szHelpString.append(gDLL->getText("TXT_KEY_YELLOW_NAME", kLoopCivicInfo.getDescription()));
					//GAMETEXT.parseCivicInfo(szBuffer, (CivicTypes) iLoopCivic, bInGame, false, true, false, eCivilization);
					//szHelpString.append(szBuffer);
				}
                
            }
		}

		///TKe
	}
	else
	{
		//	Random leader
		szTempBuffer.Format( SETCOLR L"%s" ENDCOLR , TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), gDLL->getText("TXT_KEY_TRAIT_PLAYER_UNKNOWN").c_str());
		szHelpString.append(szTempBuffer);
	}
}

void CvGameTextMgr::parseLeaderShortTraits(CvWStringBuffer &szHelpString, LeaderHeadTypes eLeader)
{
	//	Build help string
	if (eLeader != NO_LEADER)
	{
		FAssert((GC.getNumTraitInfos() > 0) && "GC.getNumTraitInfos() is less than or equal to zero but is expected to be larger than zero in CvSimpleCivPicker::setLeaderText");

		bool bFirst = true;
		for (int iI = 0; iI < GC.getNumTraitInfos(); ++iI)
		{
			if (GC.getLeaderHeadInfo(eLeader).hasTrait(iI))
			{
				if (!bFirst)
				{
					szHelpString.append(L"/");
				}
				else
				{
					szHelpString.append(L"[");
				}
				szHelpString.append(gDLL->getText(GC.getTraitInfo((TraitTypes)iI).getShortDescription()));
				bFirst = false;
			}
		}
		if (!bFirst)
		{
			szHelpString.append(L"]");
		}
	}
	else
	{
		//	Random leader
		szHelpString.append(CvWString("[???/???]"));
	}
}

void CvGameTextMgr::parseCivShortTraits(CvWStringBuffer &szHelpString, CivilizationTypes eCiv)
{
	//	Build help string
	if (eCiv != NO_CIVILIZATION)
	{
		FAssert((GC.getNumTraitInfos() > 0) && "GC.getNumTraitInfos() is less than or equal to zero but is expected to be larger than zero in CvSimpleCivPicker::setLeaderText");

		bool bFirst = true;
		for (int iI = 0; iI < GC.getNumTraitInfos(); ++iI)
		{
			if (GC.getCivilizationInfo(eCiv).hasTrait(iI))
			{
				if (!bFirst)
				{
					szHelpString.append(L"/");
				}
				else
				{
					szHelpString.append(L"[");
				}
				szHelpString.append(gDLL->getText(GC.getTraitInfo((TraitTypes)iI).getShortDescription()));
				bFirst = false;
			}
		}

		if (!bFirst)
		{
			szHelpString.append(L"]");
		}
	}
	else
	{
		//	Random leader
		szHelpString.append(CvWString("[???/???]"));
	}
}

//
// Build Civilization Info Help Text
//
void CvGameTextMgr::parseCivInfos(CvWStringBuffer &szInfoText, CivilizationTypes eCivilization, bool bDawnOfMan, bool bLinks)
{
	PROFILE_FUNC();

	CvWString szBuffer;
	CvWStringBuffer szTempString;
	CvWString szText;
	UnitTypes eDefaultUnit;
	UnitTypes eUniqueUnit;
	BuildingTypes eDefaultBuilding;
	BuildingTypes eUniqueBuilding;

	if (eCivilization != NO_CIVILIZATION)
	{
		CvCivilizationInfo& kCivilizationInfo = GC.getCivilizationInfo(eCivilization);

		if (!bDawnOfMan)
		{
			// Civ Name
			szBuffer.Format(SETCOLR L"%s" ENDCOLR , TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), kCivilizationInfo.getDescription());
			szInfoText.append(szBuffer);
		}

		// Free Units
		szText = gDLL->getText("TXT_KEY_FREE_UNITS");
		if (bDawnOfMan)
		{
			szTempString.assign(CvWString::format(L"%s: ", szText.GetCString()));
		}
		else
		{
			szTempString.assign(CvWString::format(NEWLINE SETCOLR L"%s" ENDCOLR , TEXT_COLOR("COLOR_ALT_HIGHLIGHT_TEXT"), szText.GetCString()));
		}

		bool bFound = false;
		for (int iI = 0; iI < GC.getNumUnitClassInfos(); ++iI)
		{
			eDefaultUnit = ((UnitTypes)(kCivilizationInfo.getCivilizationUnits(iI)));
			eUniqueUnit = ((UnitTypes)(GC.getUnitClassInfo((UnitClassTypes) iI).getDefaultUnitIndex()));
			if ((eDefaultUnit != NO_UNIT) && (eUniqueUnit != NO_UNIT))
			{
				if (eDefaultUnit != eUniqueUnit)
				{
					// Add Unit
					if (bDawnOfMan)
					{
						if (bFound)
						{
							szInfoText.append(L", ");
						}
						szBuffer.Format((bLinks ? L"<link=literal>%s</link> - (<link=literal>%s</link>)" : L"%s - (%s)"),
							GC.getUnitInfo(eDefaultUnit).getDescription(),
							GC.getUnitInfo(eUniqueUnit).getDescription());
					}
					else
					{
						szBuffer.Format(L"\n  %c%s - (%s)", gDLL->getSymbolID(BULLET_CHAR),
							GC.getUnitInfo(eDefaultUnit).getDescription(),
							GC.getUnitInfo(eUniqueUnit).getDescription());
					}
					szTempString.append(szBuffer);
					bFound = true;
				}
			}
		}
		if (bFound)
		{
			szInfoText.append(szTempString);
		}

		// Free Buildings
		szText = gDLL->getText("TXT_KEY_UNIQUE_BUILDINGS");
		if (bDawnOfMan)
		{
			if (bFound)
			{
				szInfoText.append(NEWLINE);
			}
			szTempString.assign(CvWString::format(L"%s: ", szText.GetCString()));
		}
		else
		{
			szTempString.assign(CvWString::format(NEWLINE SETCOLR L"%s" ENDCOLR , TEXT_COLOR("COLOR_ALT_HIGHLIGHT_TEXT"), szText.GetCString()));
		}

		bFound = false;
		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); ++iI)
		{
			eDefaultBuilding = ((BuildingTypes)(kCivilizationInfo.getCivilizationBuildings(iI)));
			eUniqueBuilding = ((BuildingTypes)(GC.getBuildingClassInfo((BuildingClassTypes) iI).getDefaultBuildingIndex()));
			if ((eDefaultBuilding != NO_BUILDING) && (eUniqueBuilding != NO_BUILDING))
			{
				if (eDefaultBuilding != eUniqueBuilding)
				{
					// Add Building
					if (bDawnOfMan)
					{
						if (bFound)
						{
							szInfoText.append(L", ");
						}
						szBuffer.Format((bLinks ? L"<link=literal>%s</link> - (<link=literal>%s</link>)" : L"%s - (%s)"),
							GC.getBuildingInfo(eDefaultBuilding).getDescription(),
							GC.getBuildingInfo(eUniqueBuilding).getDescription());
					}
					else
					{
						szBuffer.Format(L"\n  %c%s - (%s)", gDLL->getSymbolID(BULLET_CHAR),
							GC.getBuildingInfo(eDefaultBuilding).getDescription(),
							GC.getBuildingInfo(eUniqueBuilding).getDescription());
					}
					szTempString.append(szBuffer);
					bFound = true;
				}
			}
		}
		if (bFound)
		{
			szInfoText.append(szTempString);
		}

		if (!bDawnOfMan)
		{
			CvWString szDesc;
			for (int iI = 0; iI < kCivilizationInfo.getNumCivilizationFreeUnits(); iI++)
			{
				int iLoopUnitClass = kCivilizationInfo.getCivilizationFreeUnitsClass(iI);
				ProfessionTypes eLoopUnitProfession = (ProfessionTypes) kCivilizationInfo.getCivilizationFreeUnitsProfession(iI);
				UnitTypes eLoopUnit = (UnitTypes)kCivilizationInfo.getCivilizationUnits(iLoopUnitClass);

				if (eLoopUnit != NO_UNIT)
				{
					if (eLoopUnitProfession != NO_PROFESSION)
					{
						szDesc += CvWString::format(L"\n  %c%s (%s)", gDLL->getSymbolID(BULLET_CHAR), GC.getProfessionInfo(eLoopUnitProfession).getDescription(), GC.getUnitInfo(eLoopUnit).getDescription());
					}
					else
					{
						szDesc += CvWString::format(L"\n  %c%s", gDLL->getSymbolID(BULLET_CHAR), GC.getUnitInfo(eLoopUnit).getDescription());
					}
				}
			}
			if (!szDesc.empty())
			{
				szInfoText.append(NEWLINE);
				szInfoText.append(gDLL->getText("TXT_KEY_DAWN_OF_MAN_SCREEN_STARTING_UNITS"));
				szInfoText.append(szDesc);
			}
		}
	}
	else
	{
		//	This is a random civ, let us know here...
		szInfoText.append(gDLL->getText("TXT_KEY_MAIN_MENU_RANDOM"));
	}
}


//
// Promotion Help
//
///TKs Invention Core Mod v 1.0
void CvGameTextMgr::parsePromotionHelp(CvWStringBuffer &szBuffer, PromotionTypes ePromotion, const wchar* pcNewline, bool bCivilopediaText)
{
	PROFILE_FUNC();

	CvWString szText, szText2;
	CvWString szFirstBuffer;
	int iI;

	if (NO_PROMOTION == ePromotion)
	{
		return;
	}

	CvPromotionInfo& kPromotion = GC.getPromotionInfo(ePromotion);
	//TKs Civilian Promotions
	if (kPromotion.isCivilian())
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_IS_CIVILIAN_PROMOTION"));
	}
	if (kPromotion.getPlotWorkedBonus() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_FIELD_LABOR_BONUS", kPromotion.getPlotWorkedBonus()));
	}
	if (kPromotion.getBuildingWorkedBonus() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_LABOR_BONUS", kPromotion.getBuildingWorkedBonus()));
	}
	//Tk end Civilian Promotions
	if (kPromotion.isBlitz())
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_BLITZ_TEXT"));
	}

	if (kPromotion.isAmphib())
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_AMPHIB_TEXT"));
	}

	if (kPromotion.isRiver())
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_RIVER_ATTACK_TEXT"));
	}

	if (kPromotion.isEnemyRoute())
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_ENEMY_ROADS_TEXT"));
	}

	if (kPromotion.isAlwaysHeal())
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_ALWAYS_HEAL_TEXT"));
	}

	if (kPromotion.isHillsDoubleMove())
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_HILLS_MOVE_TEXT"));
	}

	for (iI = 0; iI < GC.getNumTerrainInfos(); ++iI)
	{
		if (kPromotion.getTerrainDoubleMove(iI))
		{
			szBuffer.append(pcNewline);
			szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_DOUBLE_MOVE_TEXT", GC.getTerrainInfo((TerrainTypes) iI).getTextKeyWide()));
		}
	}

	for (iI = 0; iI < GC.getNumFeatureInfos(); ++iI)
	{
		if (kPromotion.getFeatureDoubleMove(iI))
		{
			szBuffer.append(pcNewline);
			szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_DOUBLE_MOVE_TEXT", GC.getFeatureInfo((FeatureTypes) iI).getTextKeyWide()));
		}
	}

	if (kPromotion.getVisibilityChange() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_VISIBILITY_TEXT", kPromotion.getVisibilityChange()));
	}

	if (kPromotion.getMovesChange() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_MOVE_TEXT", kPromotion.getMovesChange()));
	}

	if (kPromotion.getMoveDiscountChange() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_MOVE_DISCOUNT_TEXT", -(kPromotion.getMoveDiscountChange())));
	}

	if (kPromotion.getWithdrawalChange() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_WITHDRAWAL_TEXT", kPromotion.getWithdrawalChange()));
	}

	if (kPromotion.getCargoChange() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_CARGO_TEXT", kPromotion.getCargoChange()));
	}

	if (kPromotion.getBombardRateChange() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_BOMBARD_TEXT", kPromotion.getBombardRateChange()));
	}

    ///Tks Med
    if (kPromotion.isNoBadGoodies())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_NO_BAD_GOODIES"));
	}
    if (kPromotion.getFirstStrikesChange() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_FIRSTSTRIKE_TEXT", kPromotion.getFirstStrikesChange()));
	}

    if (kPromotion.getChanceFirstStrikesChange() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_FIRSTSTRIKE_CHANGE_TEXT", kPromotion.getChanceFirstStrikesChange()));
	}

	if (kPromotion.isImmuneToFirstStrikes() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_IMMUNE_FIRSTSTRIKE_TEXT", kPromotion.isImmuneToFirstStrikes()));
	}
    ///Tke


	if (kPromotion.getEnemyHealChange() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_HEALS_EXTRA_TEXT", kPromotion.getEnemyHealChange()) + gDLL->getText("TXT_KEY_PROMOTION_ENEMY_LANDS_TEXT"));
	}

	if (kPromotion.getNeutralHealChange() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_HEALS_EXTRA_TEXT", kPromotion.getNeutralHealChange()) + gDLL->getText("TXT_KEY_PROMOTION_NEUTRAL_LANDS_TEXT"));
	}

	if (kPromotion.getFriendlyHealChange() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_HEALS_EXTRA_TEXT", kPromotion.getFriendlyHealChange()) + gDLL->getText("TXT_KEY_PROMOTION_FRIENDLY_LANDS_TEXT"));
	}

	if (kPromotion.getSameTileHealChange() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_HEALS_SAME_TEXT", kPromotion.getSameTileHealChange()) + gDLL->getText("TXT_KEY_PROMOTION_DAMAGE_TURN_TEXT"));
	}

	if (kPromotion.getAdjacentTileHealChange() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_HEALS_ADJACENT_TEXT", kPromotion.getAdjacentTileHealChange()) + gDLL->getText("TXT_KEY_PROMOTION_DAMAGE_TURN_TEXT"));
	}

	if (kPromotion.getCombatPercent() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_STRENGTH_TEXT", kPromotion.getCombatPercent()));
	}

	if (kPromotion.getCityAttackPercent() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_CITY_ATTACK_TEXT", kPromotion.getCityAttackPercent()));
	}

	if (kPromotion.getCityDefensePercent() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_CITY_DEFENSE_TEXT", kPromotion.getCityDefensePercent()));
	}

	if (kPromotion.getHillsAttackPercent() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_HILLS_ATTACK", kPromotion.getHillsAttackPercent()));
	}

	if (kPromotion.getHillsDefensePercent() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_HILLS_DEFENSE_TEXT", kPromotion.getHillsDefensePercent()));
	}

	if (kPromotion.getPillageChange() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_PILLAGE_CHANGE_TEXT", kPromotion.getPillageChange()));
	}

	if (kPromotion.getUpgradeDiscount() != 0)
	{
		if (100 == kPromotion.getUpgradeDiscount())
		{
			szBuffer.append(pcNewline);
			szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_UPGRADE_DISCOUNT_FREE_TEXT"));
		}
		else
		{
			szBuffer.append(pcNewline);
			szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_UPGRADE_DISCOUNT_TEXT", kPromotion.getUpgradeDiscount()));
		}
	}

	if (kPromotion.getExperiencePercent() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_FASTER_EXPERIENCE_TEXT", kPromotion.getExperiencePercent()));
	}

	for (iI = 0; iI < GC.getNumTerrainInfos(); ++iI)
	{
		if (kPromotion.getTerrainAttackPercent(iI) != 0)
		{
			szBuffer.append(pcNewline);
			szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_ATTACK_TEXT", kPromotion.getTerrainAttackPercent(iI), GC.getTerrainInfo((TerrainTypes) iI).getTextKeyWide()));
		}

		if (kPromotion.getTerrainDefensePercent(iI) != 0)
		{
			szBuffer.append(pcNewline);
			szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_DEFENSE_TEXT", kPromotion.getTerrainDefensePercent(iI), GC.getTerrainInfo((TerrainTypes) iI).getTextKeyWide()));
		}
	}

	for (iI = 0; iI < GC.getNumFeatureInfos(); ++iI)
	{
		if (kPromotion.getFeatureAttackPercent(iI) != 0)
		{
			szBuffer.append(pcNewline);
			szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_ATTACK_TEXT", kPromotion.getFeatureAttackPercent(iI), GC.getFeatureInfo((FeatureTypes) iI).getTextKeyWide()));
		}

		if (kPromotion.getFeatureDefensePercent(iI) != 0)
		{
			szBuffer.append(pcNewline);
			szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_DEFENSE_TEXT", kPromotion.getFeatureDefensePercent(iI), GC.getFeatureInfo((FeatureTypes) iI).getTextKeyWide()));
		}
	}

	for (iI = 0; iI < GC.getNumUnitClassInfos(); ++iI)
	{
		if (kPromotion.getUnitClassAttackModifier(iI) == kPromotion.getUnitClassDefenseModifier(iI))
		{
			if (kPromotion.getUnitClassAttackModifier(iI) != 0)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_UNIT_MOD_VS_TYPE", kPromotion.getUnitClassAttackModifier(iI), GC.getUnitClassInfo((UnitClassTypes)iI).getTextKeyWide()));
			}
		}
		else
		{
			if (kPromotion.getUnitClassAttackModifier(iI) != 0)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_UNIT_ATTACK_MOD_VS_CLASS", kPromotion.getUnitClassAttackModifier(iI), GC.getUnitClassInfo((UnitClassTypes)iI).getTextKeyWide()));
			}

			if (kPromotion.getUnitClassDefenseModifier(iI) != 0)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_UNIT_DEFENSE_MOD_VS_CLASS", kPromotion.getUnitClassDefenseModifier(iI), GC.getUnitClassInfo((UnitClassTypes) iI).getTextKeyWide()));
			}
		}
	}

	for (iI = 0; iI < GC.getNumUnitCombatInfos(); ++iI)
	{
		if (kPromotion.getUnitCombatModifierPercent(iI) != 0)
		{
			szBuffer.append(pcNewline);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_MOD_VS_TYPE_NO_LINK", kPromotion.getUnitCombatModifierPercent(iI), GC.getUnitCombatInfo((UnitCombatTypes)iI).getTextKeyWide()));
		}
	}

	for (iI = 0; iI < NUM_DOMAIN_TYPES; ++iI)
	{
		if (kPromotion.getDomainModifierPercent(iI) != 0)
		{
			szBuffer.append(pcNewline);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_MOD_VS_TYPE_NO_LINK", kPromotion.getDomainModifierPercent(iI), GC.getDomainInfo((DomainTypes)iI).getTextKeyWide()));
		}
	}

	///TKs Invention Core Mod v 1.0
	if (bCivilopediaText)
	{
        int iAllows = 0;
        for (int iCivic = 0; iCivic < GC.getNumCivicInfos(); iCivic++)
        {
            iAllows = GC.getCivicInfo((CivicTypes)iCivic).getAllowsPromotions(ePromotion);
            if (iAllows > 0)
            {
                if ((CivicTypes)GC.getCivicInfo((CivicTypes)iCivic).getInventionCategory() == (CivicTypes)GC.getXMLval(XML_MEDIEVAL_TRADE_TECH))
                {
                    szBuffer.append(NEWLINE);
                    szBuffer.append(gDLL->getText("TXT_KEY_TRADE_PERK_MAKES_AVAILABLE", GC.getCivicInfo((CivicTypes)iCivic).getDescription()));
                    break;
                }
                else
                {
                    szBuffer.append(NEWLINE);
                    szBuffer.append(gDLL->getText("TXT_KEY_TECH_MAKES_AVAILABLE", GC.getCivicInfo((CivicTypes)iCivic).getDescription()));
                    break;
                }
            }
            else if (iAllows < 0)
            {
                szBuffer.append(NEWLINE);
                szBuffer.append(gDLL->getText("TXT_KEY_TECH_MAKES_OBSOLETE", GC.getCivicInfo((CivicTypes)iCivic).getDescription()));
                break;
            }
        }
	}
	///TKe

	if (wcslen(kPromotion.getHelp()) > 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(kPromotion.getHelp());
	}
}

//	Function:			parseCivicInfo()
//	Description:	Will parse the civic info help
//	Parameters:		szHelpText -- the text to put it into
//								civicInfo - what to parse
//	Returns:			nothing
///TK Med
void CvGameTextMgr::parseCivicInfo(CvWStringBuffer &szHelpText, CivicTypes eCivic, bool bCivilopediaText, bool bPlayerContext, bool bSkipName, bool bOnlyCost, CivilizationTypes eCivilization)
{
	PROFILE_FUNC();

	CvWString szFirstBuffer;
	bool bFirst;
	int iLast;
	int iI, iJ;

	if (NO_CIVIC == eCivic)
	{
		return;
	}
	CvCivicInfo& kCivicInfo = GC.getCivicInfo(eCivic);

	szHelpText.clear();

	FAssert(GC.getGameINLINE().getActivePlayer() != NO_PLAYER || !bPlayerContext);
    if (bPlayerContext || !bCivilopediaText)
    {
		if (GC.getGameINLINE().getActivePlayer() != NO_PLAYER)
		{
			eCivilization = GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getCivilizationType();
		}
    }

	if (!bSkipName)
	{
	    szFirstBuffer.Format( SETCOLR L"%s" ENDCOLR , TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), kCivicInfo.getDescription());
		szHelpText.append(szFirstBuffer);
	}
	///TKs Invention Core Mod v 1.0

    ///Costs
    int iCost = GC.getCostToResearch(eCivic);
    bool bFoundYields = false;
    int iFoundFathers = -1;
    FatherPointTypes eFatherPoints = NO_FATHER_POINT_TYPE;
    if ((bOnlyCost || bCivilopediaText || bPlayerContext) && iCost > 0)
    {

        for(int iResearch = 0; iResearch < GC.getNumFatherPointInfos(); iResearch++)
		{
			//FatherPointTypes ePointType = (FatherPointTypes) i;
            if (kCivicInfo.getRequiredFatherPoints(iResearch) > 0)
            {
               iFoundFathers = kCivicInfo.getRequiredFatherPoints(iResearch);
               eFatherPoints = (FatherPointTypes)iResearch;
               break;
            }

        }

        if (iFoundFathers == -1)
        {
            std::vector<YieldTypes> eRequiredYield;
            for (int iResearch = 0; iResearch < NUM_YIELD_TYPES; iResearch++)
            {
                if (kCivicInfo.getRequiredYields(iResearch) > 0)
                {
                    bFoundYields = true;
                    eRequiredYield.push_back((YieldTypes)iResearch);
                }

            }

            if (bFoundYields == false)
            {
                 eRequiredYield.push_back(YIELD_IDEAS);
            }

            CvWString szYieldsList;
            for (std::vector<YieldTypes>::iterator it = eRequiredYield.begin(); it != eRequiredYield.end(); ++it)
            {
                if (!szYieldsList.empty())
                {
                    if (*it == eRequiredYield.back())
                    {
                        szYieldsList += CvWString::format(gDLL->getText("TXT_KEY_AND"));
                    }
                    else
                    {
                        szYieldsList += L", ";
                    }
                }
                szYieldsList += CvWString::format(L"%c", GC.getYieldInfo(*it).getChar());
            }

//        if (!bSkipName)
//        {
//            szHelpText.append(NEWLINE);
//        }
             ///TK Update 1.1b
            if (GC.getGameINLINE().getActivePlayer() != NO_PLAYER)
            {
                iCost *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getFatherPercent();
                iCost /= 100;
            }
            ///TK end update
            if (bOnlyCost)
            {
                szHelpText.append(gDLL->getText("TXT_KEY_YIELDS_CONVERTED_IDEAS_COST_NOBULL", iCost, szYieldsList.GetCString()));
                return;
            }

            if (!bSkipName || bCivilopediaText)
            {
                  szHelpText.append(NEWLINE);
            }
            szHelpText.append(gDLL->getText("TXT_KEY_YIELDS_CONVERTED_IDEAS_COST", iCost, szYieldsList.GetCString()));
        }
        else
        {
            iCost = iFoundFathers;
            iCost *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getFatherPercent();
            iCost /= 100;
            if (bOnlyCost)
            {
                szHelpText.append(gDLL->getText("TXT_KEY_TRADEPOINTS_CONVERTED_IDEAS_COST_NOBULLET", iCost, GC.getFatherPointInfo(eFatherPoints).getChar()));
                return;
            }
            if (!bSkipName || bCivilopediaText)
            {
                  szHelpText.append(NEWLINE);
            }
            szHelpText.append(gDLL->getText("TXT_KEY_TRADEPOINTS_CONVERTED_IDEAS_COST", iCost, GC.getFatherPointInfo(eFatherPoints).getChar()));
        }
    }

    if (bOnlyCost)
    {
        return;
    }
   //Tk Civics Screen
	/*for (iI = 0; iI < kCivicInfo.getNumCivicTreasuryBonus(); iI++)
    {
        szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_TREASURY_BUILDING", GC.getCivicInfo((BuildingClassTypes)kCivicInfo.getCivicTreasury(iI)).getDescription(), kCivicInfo.getCivicTreasuryBonus(iI)));
    }*/
	for (iI = 0; iI < kCivicInfo.getProhibitsCivicsSize(); iI++)
	{
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_PROHIBITED_CIVIC_EFFECT", GC.getCivicInfo(kCivicInfo.getProhibitsCivics(iI)).getDescription()));
	}

	for (iI = 0; iI < kCivicInfo.getNumCivicCombatBonus(); iI++)
	{
		if ((CivicTypes)kCivicInfo.getCivicCombat(iI) == eCivic)
		{
			szHelpText.append(NEWLINE);
			szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_HOLY_COMBAT_BONUS", kCivicInfo.getCivicCombatBonus(iI), GC.getCivicInfo((CivicTypes)kCivicInfo.getCivicCombat(iI)).getDescription()));
		}
		else
		{
			szHelpText.append(NEWLINE);
			szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_COMBAT_BONUS", kCivicInfo.getCivicCombatBonus(iI), GC.getCivicInfo((CivicTypes)kCivicInfo.getCivicCombat(iI)).getDescription()));
		}
	}
	if (kCivicInfo.getDiplomacyAttitudeChange() > 0)
	{
	    szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_DIPLOMACY_BONUS", kCivicInfo.getDiplomacyAttitudeChange()));
	}
	else if (kCivicInfo.getDiplomacyAttitudeChange() < 0)
	{
	    szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_DIPLOMACY_NEGATIVE", kCivicInfo.getDiplomacyAttitudeChange()));
	}
	if (kCivicInfo.getNumCivicTreasuryBonus() > 0)
	{
	    szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_BUILDING_TREASURY_BONUS"));
	}
	for (iI = 0; iI < kCivicInfo.getNumConnectedTradeYields(); iI++)
	{
		YieldTypes eConnectedYield = (YieldTypes)kCivicInfo.getConnectedTradeYields(iI);
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_CONNECTED_TRADE", kCivicInfo.getConnectedTradeYieldsBonus(iI), GC.getYieldInfo(eConnectedYield).getChar()));
	}
	//}
	for (iI = 0; iI < kCivicInfo.getNumConnectedMissonYields(); iI++)
	{
		YieldTypes eConnectedYield = (YieldTypes)kCivicInfo.getConnectedMissonYields(iI);
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_CONNECTED_MISSION", kCivicInfo.getConnectedMissonYieldsBonus(iI), GC.getYieldInfo(eConnectedYield).getChar()));
	}

	for (iI = 0; iI < kCivicInfo.getNumUnitClassFoodCosts(); iI++)
	{
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_UNIT_FOOD_CHANGE", kCivicInfo.getUnitClassFoodCosts(iI), GC.getUnitClassInfo((UnitClassTypes)kCivicInfo.getFoodCostsUnits(iI)).getDescription()));
	}

	for (iI = 0; iI < kCivicInfo.getNumRandomGrowthUnits(); iI++)
	{
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_GROWTH_UNITS", kCivicInfo.getRandomGrowthUnitsPercent(iI), GC.getUnitClassInfo((UnitClassTypes)kCivicInfo.getRandomGrowthUnits(iI)).getDescription()));
	}

    YieldTypes eVictoryYield = (YieldTypes) GC.getXMLval(XML_INDUSTRIAL_VICTORY_SINGLE_YIELD);
    if (kCivicInfo.getIndustrializationVictory(eVictoryYield) > 0)
	{
	    iCost = kCivicInfo.getIndustrializationVictory(eVictoryYield);
	    if (GC.getGameINLINE().getActivePlayer() != NO_PLAYER)
        {
            iCost *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getFatherPercent();
            iCost /= 100;
        }

        szHelpText.append(NEWLINE);
        szHelpText.append(gDLL->getText("TXT_KEY_TECH_INDUSTRIAL_VICTORY", iCost, GC.getYieldInfo(eVictoryYield).getChar()));
	}

	if (kCivicInfo.getAllowsTrait() != NO_TRAIT)
	{
	    CvWStringBuffer szHelpString;
	    parseTraits(szHelpString, ((TraitTypes)kCivicInfo.getAllowsTrait()), eCivilization, false, false);
	    szHelpText.append(szHelpString);
	}
	///TKe
	// Special Building Not Required...
	for (iI = 0; iI < GC.getNumSpecialBuildingInfos(); ++iI)
	{
		if (kCivicInfo.isSpecialBuildingNotRequired(iI))
		{
			// XXX "Missionaries"??? - Now in XML
			szHelpText.append(NEWLINE);
			szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_BUILD_MISSIONARIES", GC.getSpecialBuildingInfo((SpecialBuildingTypes)iI).getTextKeyWide()));
		}
	}

	//	Great General Modifier...
	if (kCivicInfo.getGreatGeneralRateModifier() != 0)
	{
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_GREAT_GENERAL_MOD", kCivicInfo.getGreatGeneralRateModifier()));
	}

	if (kCivicInfo.getDomesticGreatGeneralRateModifier() != 0)
	{
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_DOMESTIC_GREAT_GENERAL_MODIFIER", kCivicInfo.getDomesticGreatGeneralRateModifier()));
	}

	//	Free Experience
	if (kCivicInfo.getFreeExperience() != 0)
	{
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_FREE_XP", kCivicInfo.getFreeExperience()));
	}

	//	Worker speed modifier
	if (kCivicInfo.getWorkerSpeedModifier() != 0)
	{
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_WORKER_SPEED", kCivicInfo.getWorkerSpeedModifier()));
	}

	//	Improvement upgrade rate modifier
	if (kCivicInfo.getImprovementUpgradeRateModifier() != 0)
	{
		bFirst = true;

		for (iI = 0; iI < GC.getNumImprovementInfos(); ++iI)
		{
			if (GC.getImprovementInfo((ImprovementTypes)iI).getImprovementUpgrade() != NO_IMPROVEMENT)
			{
				szFirstBuffer.Format(L"%s%s", NEWLINE, gDLL->getText("TXT_KEY_CIVIC_IMPROVEMENT_UPGRADE", kCivicInfo.getImprovementUpgradeRateModifier()).c_str());
				CvWString szImprovement;
				szImprovement.Format(L"<link=literal>%s</link>", GC.getImprovementInfo((ImprovementTypes)iI).getDescription());
				setListHelp(szHelpText, szFirstBuffer, szImprovement, L", ", bFirst);
				bFirst = false;
			}
		}
	}

	//	Military unit production modifier
	if (kCivicInfo.getMilitaryProductionModifier() != 0)
	{
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_MILITARY_PRODUCTION", kCivicInfo.getMilitaryProductionModifier()));
	}

	//	Experience
	if (0 != kCivicInfo.getExpInBorderModifier())
	{
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_EXPERIENCE_IN_BORDERS", kCivicInfo.getExpInBorderModifier()));
	}

	//	Cross Conversion
	if (kCivicInfo.getImmigrationConversion() != YIELD_CROSSES && kCivicInfo.getImmigrationConversion() != NO_YIELD)
	{
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_IMMIGRATION_CONVERSION", GC.getYieldInfo(YIELD_CROSSES).getChar(), GC.getYieldInfo((YieldTypes) kCivicInfo.getImmigrationConversion()).getChar()));
		///TKs Invention Core Mod v 1.0
		//if (!bCivilopediaText)
		//{
			szHelpText.append(NEWLINE);
			szHelpText.append(gDLL->getText("TXT_KEY_WAIT_TILL_REVOLUTION"));
		//}
		///TKe
	}

	// native attitude
	if (kCivicInfo.getNativeAttitudeChange() > 0)
	{
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_FATHER_NATIVE_ATTITUDE_GOOD"));
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_FATHER_NATIVE_ATTITUDE_GOOD2"));
	}
	else if (kCivicInfo.getNativeAttitudeChange() < 0)
	{
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_FATHER_NATIVE_ATTITUDE_BAD"));
	}

	// native combat
	if (kCivicInfo.getNativeCombatModifier() != 0)
	{
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_UNIT_NATIVE_COMBAT_MOD", kCivicInfo.getNativeCombatModifier()));
	}

	// native combat
	if (kCivicInfo.getFatherPointModifier() != 0)
	{
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_UNIT_FATHER_POINT_MOD", kCivicInfo.getFatherPointModifier()));
	}

	// native borders
	if (kCivicInfo.isDominateNativeBorders())
	{
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_NATIVE_BORDERS"));
	}

	// Always trade with Europe
	if (kCivicInfo.isRevolutionEuropeTrade())
	{
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_REVOLUTION_EUROPE_TRADE"));
	}

	//	Yield Modifiers
	setYieldChangeHelp(szHelpText, L"", L"", gDLL->getText("TXT_KEY_BUILDING_ALL_CITIES").GetCString(), kCivicInfo.getYieldModifierArray(), true);

	//	Capital Yield Modifiers
	setYieldChangeHelp(szHelpText, L"", L"", gDLL->getText("TXT_KEY_CIVIC_IN_CAPITAL").GetCString(), kCivicInfo.getCapitalYieldModifierArray(), true);

	// Civic Arrays
	//Garrison Unit Array
	setYieldChangeHelp(szHelpText, L"", L"", gDLL->getText("TXT_KEY_CIVIC_GARRISON_BONUS").GetCString(), kCivicInfo.getGarrisonUnitArray(), false);

	//	Improvement Yields
	for (iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		iLast = 0;

		for (iJ = 0; iJ < GC.getNumImprovementInfos(); iJ++)
		{
			if (kCivicInfo.getImprovementYieldChanges(iJ, iI) != 0)
			{
				szFirstBuffer.Format(L"%s%s", NEWLINE, gDLL->getText("TXT_KEY_CIVIC_IMPROVEMENT_YIELD_CHANGE", kCivicInfo.getImprovementYieldChanges(iJ, iI), GC.getYieldInfo((YieldTypes)iI).getChar()).c_str());
				CvWString szImprovement;
				szImprovement.Format(L"<link=literal>%s</link>", GC.getImprovementInfo((ImprovementTypes)iJ).getDescription());
				setListHelp(szHelpText, szFirstBuffer, szImprovement, L", ", (kCivicInfo.getImprovementYieldChanges(iJ, iI) != iLast));
				iLast = kCivicInfo.getImprovementYieldChanges(iJ, iI);
			}
		}

	}

	//	Hurry types
	for (iI = 0; iI < GC.getNumHurryInfos(); ++iI)
	{
		if (kCivicInfo.isHurry(iI))
		{
			szHelpText.append(CvWString::format(L"%s%c%s", NEWLINE, gDLL->getSymbolID(BULLET_CHAR), GC.getHurryInfo((HurryTypes)iI).getDescription()));
		}
	}
    ///TKs Civic Screen
	///START
	//for (iI = 0; iI < GC.getNumCivicOptionInfos(); ++iI)
	//{

	//}
	if (kCivicInfo.getAllowsCivic() != NO_CIVIC)
	{
	    szHelpText.append(NEWLINE);
        szHelpText.append(gDLL->getText("TXT_KEY_TECH_ALLOWS_CIVIC", GC.getCivicInfo((CivicTypes)kCivicInfo.getAllowsCivic()).getDescription()));
	}

	if (kCivicInfo.getIncreasedImmigrants() > 0)
	{
	    szHelpText.append(NEWLINE);
        szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_INCREASED_IMMIGRANTS", kCivicInfo.getIncreasedImmigrants()));
	}
	if (kCivicInfo.getMissionariesNotCosumed() > 0)
	{
	    szHelpText.append(NEWLINE);
        szHelpText.append(gDLL->getText("TXT_KEY_MISSIONS_NOTCONSUMED_COUNT", kCivicInfo.getMissionariesNotCosumed()));
	}
	if (kCivicInfo.getMissionariesNotCosumed() < 0)
	{
	    szHelpText.append(NEWLINE);
        szHelpText.append(gDLL->getText("TXT_KEY_MISSIONS_NOTCONSUMED_CONVERT"));
	}
	if (kCivicInfo.getTradingPostNotCosumed() > 0)
    {
        szHelpText.append(NEWLINE);
        szHelpText.append(gDLL->getText("TXT_KEY_TRADERS_NOTCONSUMED", kCivicInfo.getTradingPostNotCosumed()));
    }
	if (kCivicInfo.getHuntingYieldPercent() > 0)
    {
        szHelpText.append(NEWLINE);
        szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_BONUS_HUNTING", kCivicInfo.getHuntingYieldPercent()));
    }
	if (kCivicInfo.getPilgramYieldPercent() > 0)
    {
        szHelpText.append(NEWLINE);
        szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_BONUS_PILGRAM", kCivicInfo.getPilgramYieldPercent()));
    }
   /* ModCodeTypes iModdersCode = (ModCodeTypes)kCivicInfo.getModdersCode1();
	if (iModdersCode != NO_MOD_CODE)
	{

	    if (iModdersCode == MODER_CODE_SPICE_ROUTE || iModdersCode == MODER_CODE_SILK_ROAD_ROUTE)
	    {
	        szHelpText.append(NEWLINE);
            szHelpText.append(gDLL->getText("TXT_KEY_NEW_TRADE_ROUTE", kCivicInfo.getDescription()));
	    }
	}*/

	if (kCivicInfo.getAllowsTradeScreen() != NO_EUROPE)
    {
		szHelpText.append(NEWLINE);
        szHelpText.append(gDLL->getText("TXT_KEY_NEW_TRADE_ROUTE", kCivicInfo.getDescription()));
	}

	if (kCivicInfo.getConvertsResearchYield() != NO_YIELD)
	{
	    szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_RESEARCH_CONVERTS_YIELD", GC.getYieldInfo((YieldTypes)kCivicInfo.getConvertsResearchYield()).getChar()));
	}
	bool bFreeUnitOnResearch = false;
	if (kCivicInfo.getFreeUnitFirstToResearch() != NO_UNITCLASS)
    {
        //UnitTypes eUnit = (UnitTypes) GC.getCivilizationInfo(eCivilization).getCivilizationUnits((UnitClassTypes)iI);

        szHelpText.append(NEWLINE);
        szHelpText.append(gDLL->getText("TXT_KEY_FRIST_TO_RESEARCH_UNIT_CLASS", GC.getUnitClassInfo((UnitClassTypes)kCivicInfo.getFreeUnitFirstToResearch()).getTextKeyWide()));
        bFreeUnitOnResearch = true;
    }

	if (kCivicInfo.getNumFreeUnitClasses() > 0 && bFreeUnitOnResearch == false)
	{
	    if (kCivicInfo.isFreeUnitsAreNonePopulation())
        {
            for (iI = 0; iI < kCivicInfo.getNumFreeUnitClasses(); iI++)
            {
                UnitClassTypes eFreeUnit = (UnitClassTypes)kCivicInfo.getFreeUnitClass(iI);
                if (eFreeUnit != NO_UNITCLASS)
                {
                    for (int i = 0; i < GC.getNumUnitInfos(); ++i)
                    {
                        CvUnitInfo& kUnitInfo = GC.getUnitInfo((UnitTypes) i);

                        if (kUnitInfo.getUnitClassType() == eFreeUnit)
                        {
                            szHelpText.append(NEWLINE);
                            szHelpText.append(gDLL->getText("TXT_KEY_INVENTION_ALLOWS_FREE_UNITCLASS", kCivicInfo.getNumFreeUnitClasses(), kUnitInfo.getDescription()));
                            break;
                        }
                    }
                }
            }
        }
//        else if (kCivicInfo.isFreeUnitsNotAllCities())
//        {
//
//        }
        else
        {
            szHelpText.append(NEWLINE);
            szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_EXTRA_POPULATION", kCivicInfo.getNumFreeUnitClasses()));
			UnitClassTypes eFreeUnit = (UnitClassTypes)kCivicInfo.getFreeUnitClass(0);
			for (int i = 0; i < GC.getNumUnitInfos(); ++i)
            {	CvUnitInfo& kUnitInfo = GC.getUnitInfo((UnitTypes) i);
                if (kUnitInfo.getUnitClassType() == eFreeUnit)
                {
					szHelpText.append(gDLL->getText("TXT_KEY_IN_PARENTHASIS", kUnitInfo.getDescription()));
					break;
				}
			}
        }
	}

	for (int iProfession = 0; iProfession < GC.getNumProfessionInfos(); ++iProfession)
	{
		if (kCivicInfo.getProfessionCombatChange(iProfession) != 0)
		{
			CvProfessionInfo& kProfession = GC.getProfessionInfo((ProfessionTypes) iProfession);
			szHelpText.append(NEWLINE);
			szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_PROFESSION_EXTRA_STRENGTH", kCivicInfo.getProfessionCombatChange(iProfession), kProfession.getTextKeyWide()));
		}
	}

	CvWStringBuffer szBuffer;
	for (iI = 0; iI < GC.getNumPromotionInfos(); iI++)
    {
        if (kCivicInfo.getAllowsPromotions(iI) > 0)
        {
            CvPromotionInfo& kPromotion = GC.getPromotionInfo((PromotionTypes)iI);
            szHelpText.append(gDLL->getText("TXT_KEY_INVENTION_ALLOWS_PROMOTION", kPromotion.getDescription()));
            //parsePromotionHelp(szBuffer, (PromotionTypes)iI);
            //szHelpText.append(szBuffer);
        }
    }

    for (iI = 0; iI < GC.getNumProfessionInfos(); iI++)
    {
        if (kCivicInfo.getAllowsProfessions(iI) > 0)
        {
            CvProfessionInfo& kProfession = GC.getProfessionInfo((ProfessionTypes)iI);
            szHelpText.append(NEWLINE);
            szHelpText.append(gDLL->getText("TXT_KEY_INVENTION_ALLOWS_PROFESSION", kProfession.getDescription()));

        }

        if (kCivicInfo.getAllowsProfessions(iI) < 0)
        {
            CvProfessionInfo& kProfession = GC.getProfessionInfo((ProfessionTypes)iI);
            szHelpText.append(NEWLINE);
            if (kCivicInfo.getInventionCategory() == GC.getXMLval(XML_MEDIEVAL_CENSURE))
            {
                szHelpText.append(gDLL->getText("TXT_KEY_INVENTION_DISALLOWS_CENSURE", kProfession.getDescription()));
            }
            else
            {
                szHelpText.append(gDLL->getText("TXT_KEY_INVENTION_OBSOLETES", kProfession.getDescription()));
            }


        }
    }

	for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
    {
        if (kCivicInfo.getAllowsYields(iI) > 0)
        {
            szHelpText.append(NEWLINE);
            szHelpText.append(gDLL->getText("TXT_KEY_RESEARCH_ALLOWS_YIELD", GC.getYieldInfo((YieldTypes)iI).getDescription()));
        }
	}
    ///Building Types is Used Here
    for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
    {
        if (kCivicInfo.getAllowsBuildingTypes(GC.getBuildingInfo((BuildingTypes)iI).getBuildingClassType()) > 0)
        {
            BuildingClassTypes eBuildingClass = (BuildingClassTypes)GC.getBuildingInfo((BuildingTypes)iI).getBuildingClassType();
            bool bShowBuilding = true;
            if (eCivilization != NO_CIVILIZATION)
            {
                BuildingTypes eBuilding = (BuildingTypes)GC.getCivilizationInfo(eCivilization).getCivilizationBuildings(eBuildingClass);
                if (eBuilding != (BuildingTypes)iI)
                {
                    bShowBuilding = false;
                }
            }
            if (bShowBuilding)
            {
               szHelpText.append(NEWLINE);
               BuildingTypes eBuilding = (BuildingTypes)iI;
               //GAMETEXT.setBuildingHelp(szBuffer, eBuilding, false, false, NULL);
               szHelpText.append(gDLL->getText("TXT_KEY_IDEA_ALLOWS_BUILDING", GC.getBuildingInfo(eBuilding).getDescription()));
               //szHelpText.append(szBuffer);
            }
        }

        if (kCivicInfo.getAllowsBuildingTypes(GC.getBuildingInfo((BuildingTypes)iI).getBuildingClassType()) < 0)
        {
            BuildingClassTypes eBuildingClass = (BuildingClassTypes)GC.getBuildingInfo((BuildingTypes)iI).getBuildingClassType();
            bool bShowBuilding = true;
            if (eCivilization != NO_CIVILIZATION)
            {
                BuildingTypes eBuilding = (BuildingTypes)GC.getCivilizationInfo(eCivilization).getCivilizationBuildings(eBuildingClass);
                if (eBuilding != (BuildingTypes)iI)
                {
                    bShowBuilding = false;
                }
            }
            if (bShowBuilding)
            {
               szHelpText.append(NEWLINE);
               BuildingTypes eBuilding = (BuildingTypes)iI;
               //GAMETEXT.setBuildingHelp(szBuffer, eBuilding, false, false, NULL);
               szHelpText.append(gDLL->getText("TXT_KEY_INVENTION_DISALLOWS_CENSURE", GC.getBuildingInfo(eBuilding).getDescription()));
               //szHelpText.append(szBuffer);
            }
        }
    }

    for (iI = 0; iI < GC.getNumImprovementInfos(); iI++)
    {
        if (kCivicInfo.getAllowsBuildTypes(iI) > 0)
        {

            std::vector<TerrainTypes> eAllowedTerrains;
            for (int iJ = 0; iJ < GC.getNumTerrainInfos(); iJ++)
            {
                if (kCivicInfo.getAllowsBuildTypesTerrain(iJ) > 0)
                {
                    eAllowedTerrains.push_back((TerrainTypes)iJ);
                }
            }

            CvWString szTerrainList;
            for (std::vector<TerrainTypes>::iterator it = eAllowedTerrains.begin(); it != eAllowedTerrains.end(); ++it)
            {
                if (!szTerrainList.empty())
                {
                    if (*it == eAllowedTerrains.back())
                    {
                        szTerrainList += CvWString::format(gDLL->getText("TXT_KEY_AND"));
                    }
                    else
                    {
                        szTerrainList += L", ";
                    }
                }
                szTerrainList += CvWString::format(L"%s", GC.getTerrainInfo((TerrainTypes)*it).getDescription());
            }

            if (!szTerrainList.empty())
            {
                szHelpText.append(NEWLINE);
                szHelpText.append(gDLL->getText("TXT_KEY_INVENTION_ALLOWS_BUILDTYPE_TERRAIN", GC.getImprovementInfo((ImprovementTypes)iI).getDescription()));

                szHelpText.append(NEWLINE);
                szHelpText.append(gDLL->getText("TXT_KEY_INVENTION_ALLOWS_BUILDTYPE_TERRAIN_LIST", szTerrainList.GetCString()));
            }
            else
            {
                szHelpText.append(NEWLINE);
                szHelpText.append(gDLL->getText("TXT_KEY_INVENTION_ALLOWS_BUILDTYPE", GC.getImprovementInfo((ImprovementTypes)iI).getDescription()));
            }

        }

        if (kCivicInfo.getAllowsBuildTypes(iI) < 0)
        {
            szHelpText.append(NEWLINE);
            szHelpText.append(gDLL->getText("TXT_KEY_INVENTION_DISALLOWS_BUILDTYPE", GC.getImprovementInfo((ImprovementTypes)iI).getDescription()));
        }

        if (kCivicInfo.getFasterBuildTypes(iI) > 0)
        {
            szHelpText.append(NEWLINE);
            szHelpText.append(gDLL->getText("TXT_KEY_INVENTION_FASTER_BUILDTYPE", GC.getImprovementInfo((ImprovementTypes)iI).getDescription(), kCivicInfo.getFasterBuildTypes(iI)));
        }
    }

    //if (kCivicInfo.getModdersCode1() == 1)
   // {
        for (iI = 0; iI < GC.getNumFeatureInfos(); ++iI)
        {
            if (kCivicInfo.getFasterBuildFeatureTypes(iI) > 0)
            {
                szHelpText.append(NEWLINE);
                szHelpText.append(gDLL->getText("TXT_KEY_INVENTION_FASTER_CLEAR_FOREST", GC.getFeatureInfo((FeatureTypes)iI).getDescription(), kCivicInfo.getFasterBuildFeatureTypes(iI)));
            }
        }
   // }
	for (iI = 0; iI < GC.getNumFatherPointInfos(); ++iI)
    {
        if (kCivicInfo.getFartherPointChanges(iI) > 0)
        {
            szHelpText.append(NEWLINE);
            szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_FATHER_BONUS", kCivicInfo.getFartherPointChanges(iI), GC.getFatherPointInfo((FatherPointTypes)iI).getDescription()));
        }
    }
    for (iI = 0; iI < GC.getNumRouteInfos(); iI++)
    {
        if (kCivicInfo.getRouteMovementMod(iI) != 0)
        {
            //int iRouteMod = (kCivicInfo.getRouteMovementMod(iI) * -1) / 10;
            szHelpText.append(NEWLINE);
            szHelpText.append(gDLL->getText("TXT_KEY_INVENTION_ROUTETYPE_MOD", GC.getRouteInfo((RouteTypes)iI).getDescription(), kCivicInfo.getRouteMovementMod(iI)));
        }
        if (kCivicInfo.getAllowsRoute(iI) > 0)
        {
            szHelpText.append(NEWLINE);
            szHelpText.append(gDLL->getText("TXT_KEY_INVENTION_ALLOWS_ROUTETYPE", GC.getRouteInfo((RouteTypes)iI).getDescription()));
        }
    }

    for (iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
    {
		if (kCivicInfo.getAllowedUnitClassImmigration(iI) != 0)
        {
            for (int i = 0; i < GC.getNumUnitInfos(); ++i)
            {
                CvUnitInfo& kUnitInfo = GC.getUnitInfo((UnitTypes) i);
                if (kUnitInfo.getUnitClassType() == (UnitClassTypes)iI)
                {
                    bool bShowUnit = true;
                    if (eCivilization != NO_CIVILIZATION)
                    {
                        UnitTypes eUnit = (UnitTypes) GC.getCivilizationInfo(eCivilization).getCivilizationUnits((UnitClassTypes)iI);
                        if (eUnit != (UnitTypes) i)
                        {
                            bShowUnit = false;
                        }
                    }
                    if (bShowUnit)
                    {
                        szHelpText.append(NEWLINE);
						if (kCivicInfo.getAllowedUnitClassImmigration(iI) > 0)
						{
							szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_ALLOW_UNITCLASS_IMMIGRATION", kUnitInfo.getDescription()));
						}
						else
						{
							szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_PREVENT_UNITCLASS_IMMIGRATION", kUnitInfo.getDescription()));
						}
                    }
                }
            }
        }

        if (kCivicInfo.getAllowsUnitClasses(iI) > 0)
        {
            for (int i = 0; i < GC.getNumUnitInfos(); ++i)
            {
                CvUnitInfo& kUnitInfo = GC.getUnitInfo((UnitTypes) i);
                if (kUnitInfo.getUnitClassType() == (UnitClassTypes)iI)
                {
                    bool bShowUnit = true;
                    if (eCivilization != NO_CIVILIZATION)
                    {
                        UnitTypes eUnit = (UnitTypes) GC.getCivilizationInfo(eCivilization).getCivilizationUnits((UnitClassTypes)iI);
                        if (eUnit != (UnitTypes) i)
                        {
                            bShowUnit = false;
                        }
                    }
                    if (bShowUnit)
                    {
                        szHelpText.append(NEWLINE);
                        szHelpText.append(gDLL->getText("TXT_KEY_INVENTION_ALLOWS_UNITCLASS", kUnitInfo.getDescription()));
                    }
                    //break;
                }
            }
        }

        if (kCivicInfo.getAllowsUnitClasses(iI) < 0)
        {
            for (int i = 0; i < GC.getNumUnitInfos(); ++i)
            {
                CvUnitInfo& kUnitInfo = GC.getUnitInfo((UnitTypes) i);
                if (kUnitInfo.getUnitClassType() == (UnitClassTypes)iI)
                {
                    bool bShowUnit = true;
                    if (eCivilization != NO_CIVILIZATION)
                    {
                        UnitTypes eUnit = (UnitTypes) GC.getCivilizationInfo(eCivilization).getCivilizationUnits((UnitClassTypes)iI);
                        if (eUnit != (UnitTypes) i)
                        {
                            bShowUnit = false;
                        }
                    }
                    if (bShowUnit)
                    {
                        szHelpText.append(NEWLINE);
                        szHelpText.append(gDLL->getText("TXT_KEY_INVENTION_OBSOLETES", kUnitInfo.getDescription()));
                    }
                    //break;
                }
            }
        }
    }

    for (iI = 0; iI < GC.getNumBonusInfos(); iI++)
    {
        if (kCivicInfo.getAllowsBonuses(iI) > 0)
        {
            CvBonusInfo& kBonusInfo = GC.getBonusInfo((BonusTypes)iI);
            szHelpText.append(NEWLINE);
            szHelpText.append(gDLL->getText("TXT_KEY_ALLOWS_BONUS_TEXT", kBonusInfo.getDescription()));

        }

        if (kCivicInfo.getAllowsBonuses(iI) < 0)
        {
            CvBonusInfo& kBonusInfo = GC.getBonusInfo((BonusTypes)iI);
            szHelpText.append(NEWLINE);
            szHelpText.append(gDLL->getText("TXT_KEY_OBSOLETES_BONUS_TEXT", kBonusInfo.getDescription()));
        }
    }


    if (kCivicInfo.isWorkersBuildAfterMove())
	{
	   szHelpText.append(NEWLINE);
	   szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_WORKERS_BUILD"));
	}

	if (kCivicInfo.isAllowsMapTrade())
	{
	   szHelpText.append(NEWLINE);
	   szHelpText.append(gDLL->getText("TXT_KEY_ALLOWS_TRADE_MAPS"));
	}

	if (kCivicInfo.getNewDefaultUnitClass() != NO_UNITCLASS)
	{
		UnitClassTypes eClass = (UnitClassTypes)kCivicInfo.getNewDefaultUnitClass();

		for (int i = 0; i < GC.getNumUnitInfos(); ++i)
        {
            CvUnitInfo& kUnitInfo = GC.getUnitInfo((UnitTypes) i);
            if (kUnitInfo.getUnitClassType() == eClass)
            {
                bool bShowUnit = true;
                if (eCivilization != NO_CIVILIZATION)
                {
                    UnitTypes eUnit = (UnitTypes) GC.getCivilizationInfo(eCivilization).getCivilizationUnits(eClass);
                    if (eUnit != (UnitTypes) i)
                    {
                        bShowUnit = false;
                    }
                }
                if (bShowUnit)
                {
                    szHelpText.append(NEWLINE);
                    szHelpText.append(gDLL->getText("TXT_KEY_ALLOWS_NEW_DEFAULT_UNIT", kUnitInfo.getDescription()));
                }
            }
        }
	}

	if (kCivicInfo.getNewLuxuryUnitClass() != NO_UNITCLASS)
	{
		UnitClassTypes eClass = (UnitClassTypes)kCivicInfo.getNewLuxuryUnitClass();

		for (int i = 0; i < GC.getNumUnitInfos(); ++i)
        {
            CvUnitInfo& kUnitInfo = GC.getUnitInfo((UnitTypes) i);
            if (kUnitInfo.getUnitClassType() == eClass)
            {
                bool bShowUnit = true;
                if (eCivilization != NO_CIVILIZATION)
                {
                    UnitTypes eUnit = (UnitTypes) GC.getCivilizationInfo(eCivilization).getCivilizationUnits(eClass);
                    if (eUnit != (UnitTypes) i)
                    {
                        bShowUnit = false;
                    }
                }
                if (bShowUnit)
                {
                    szHelpText.append(NEWLINE);
                    szHelpText.append(gDLL->getText("TXT_KEY_ALLOWS_LUXURY_UNIT", kUnitInfo.getDescription()));
                }
            }
        }
	}

    if (kCivicInfo.getConvertsUnitsTo() != NO_UNITCLASS && kCivicInfo.getConvertsUnitsFrom() != NO_UNITCLASS)
	{
        UnitTypes eUnitTo = NO_UNIT;
        UnitTypes eUnitFrom = NO_UNIT;
        int iFoundUnits = 0;
	    for (int i = 0; i < GC.getNumUnitInfos(); ++i)
        {
            CvUnitInfo& kUnitInfo = GC.getUnitInfo((UnitTypes) i);

            if (kUnitInfo.getUnitClassType() == (UnitClassTypes)kCivicInfo.getConvertsUnitsTo())
            {
                iFoundUnits++;
                eUnitTo = (UnitTypes) i;
            }

            if (kUnitInfo.getUnitClassType() == (UnitClassTypes)kCivicInfo.getConvertsUnitsFrom())
            {
                iFoundUnits++;
                eUnitFrom = (UnitTypes) i;
            }
            if (iFoundUnits >= 2)
            {
                break;
            }
        }
        if (eUnitTo != NO_UNIT && eUnitFrom != NO_UNIT)
        {
           CvUnitInfo& kUnitInfoTo = GC.getUnitInfo(eUnitTo);
           CvUnitInfo& kUnitInfoFrom = GC.getUnitInfo(eUnitFrom);
           szHelpText.append(NEWLINE);
           szHelpText.append(gDLL->getText("TXT_KEY_CONVERTS_UNITS", kUnitInfoFrom.getDescription(), kUnitInfoTo.getDescription()));
        }
	}

	/*if (kCivicInfo.getNewDefaultUnitClass() != NO_UNITCLASS)
	{
           szHelpText.append(NEWLINE);
           szHelpText.append(gDLL->getText("TXT_KEY_CONVERTS_TO_COLONIAL"));
	}*/

	if (kCivicInfo.getNewConvertUnitClass() != NO_UNITCLASS)
	{
        szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_NEW_MISSIONARY_CLASS", GC.getUnitClassInfo((UnitClassTypes)kCivicInfo.getNewConvertUnitClass()).getDescription()));
	}

	if (kCivicInfo.getIncreasedEnemyHealRate() > 0)
	{
	   szHelpText.append(NEWLINE);
	   szHelpText.append(gDLL->getText("TXT_KEY_INCRESEASED_ENEMY_HEAL", kCivicInfo.getIncreasedEnemyHealRate()));
	}

    if (kCivicInfo.getFreeHurriedImmigrants() > 0)
    {
        szHelpText.append(NEWLINE);
        szHelpText.append(gDLL->getText("TXT_KEY_FREE_IMMIGRANTS", kCivicInfo.getFreeHurriedImmigrants()));
    }

	if (kCivicInfo.isStartConstitution())
	{
	   szHelpText.append(NEWLINE);
	   szHelpText.append(gDLL->getText("TXT_KEY_ALLOWS_CONSTITUTION"));
	}

	if (kCivicInfo.getProlificInventorRateChange() != 0)
	{
	   //szHelpText.append(NEWLINE);
	   //szHelpText.append(gDLL->getText("TXT_KEY_INCRESEASED_PROLIFIC_INVENTOR_TEXT"));
        szHelpText.append(NEWLINE);
        szHelpText.append(gDLL->getText("TXT_KEY_TRADE_POINTS_CONVERTS_YIELD", GC.getYieldInfo(YIELD_GOLD).getChar()));
	}

    if (kCivicInfo.getKingTreasureTransportMod() > 0)
    {
	   szHelpText.append(NEWLINE);
	   szHelpText.append(gDLL->getText("TXT_KEY_KINGS_COMMISION_MOD", kCivicInfo.getKingTreasureTransportMod()));
	}

	if (kCivicInfo.getFoundCityType() > 0)
    {
        if ((MedCityTypes)kCivicInfo.getFoundCityType() == CITYTYPE_OUTPOST)
        {
           szHelpText.append(NEWLINE);
           szHelpText.append(gDLL->getText("TXT_KEY_FOUND_OUTPOST"));
        }
	}
	if (kCivicInfo.getGlobalFoodCostMod() != 0)
	{
	   szHelpText.append(NEWLINE);
	   szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_GLOBAL_FOOD_COST", kCivicInfo.getGlobalFoodCostMod()));
	}
	if (kCivicInfo.getCheaperPopulationGrowth() != 0)
	{
	   szHelpText.append(NEWLINE);
	   szHelpText.append(gDLL->getText("TXT_KEY_RESEARCH_GROWTH_INCREASE", kCivicInfo.getCheaperPopulationGrowth()));
	}

	if (kCivicInfo.getCenterPlotFoodBonus() != 0)
	{
	   szHelpText.append(NEWLINE);
	   szHelpText.append(gDLL->getText("TXT_KEY_RESEARCH_CENTER_FOOD_PLOT", kCivicInfo.getCenterPlotFoodBonus()));
	}

    if ((ModCodeTypes)kCivicInfo.getModdersCode1() == MODER_CODE_TRADING_POST)
    {
        szHelpText.append(NEWLINE);
        szHelpText.append(gDLL->getText("TXT_KEY_ALLOWS_ESTABLISH_TRADEPOST"));
    }
    if ((ModCodeTypes)kCivicInfo.getModdersCode1() == MODER_CODE_ALLOWS_TRADE_FAIR)
    {
        szHelpText.append(NEWLINE);
        szHelpText.append(gDLL->getText("TXT_KEY_ALLOWS_THE_TRADE_FAIR"));
    }

    if (bCivilopediaText)
    {
        for (int iLoopCivic = 0; iLoopCivic < GC.getNumCivicInfos(); ++iLoopCivic)
        {
            CvCivicInfo& kLoopCivicInfo = GC.getCivicInfo((CivicTypes) iLoopCivic);
            if ((CivicTypes)iLoopCivic != eCivic && kLoopCivicInfo.getCivicOptionType() == CIVICOPTION_INVENTIONS)
            {
                if ((CivicTypes)kLoopCivicInfo.getRequiredInvention() == eCivic)
                {
                   CvCivicInfo& kRequiredCivicInfo = GC.getCivicInfo((CivicTypes)kLoopCivicInfo.getRequiredInvention());
                   szHelpText.append(NEWLINE);
                   szHelpText.append(gDLL->getText("TXT_KEY_LEADSTO_INVENTION", kLoopCivicInfo.getDescription()));
                }
                if ((CivicTypes)kLoopCivicInfo.getRequiredInvention2() == eCivic)
                {

                    //GC.getYieldInfo(eYield).getChar()
                   CvCivicInfo& kRequiredCivicInfo = GC.getCivicInfo((CivicTypes)kLoopCivicInfo.getRequiredInvention2());
                   szHelpText.append(NEWLINE);
                   szHelpText.append(gDLL->getText("TXT_KEY_LEADSTO_INVENTION", kLoopCivicInfo.getDescription()));
                }
            }
        }
    }

    if (eCivic == (CivicTypes)GC.getXMLval(XML_CONTACT_YIELD_GIFT_TECH))
    {
        szHelpText.append(NEWLINE);
        szHelpText.append(gDLL->getText("TXT_KEY_ENCOMEDIA_HELP"));
    }

	if (kCivicInfo.getFreeTechs() > 0)
    {
        szHelpText.append(NEWLINE);
        szHelpText.append(gDLL->getText("TXT_KEY_FREE_TECHS", kCivicInfo.getFreeTechs()));
    }

    if (kCivicInfo.getGoldBonus() > 0)
	{
	    szHelpText.append(NEWLINE);
        szHelpText.append(gDLL->getText("TXT_KEY_INVENTION_BONUS_GOLD", kCivicInfo.getGoldBonus()));
	}

	if (kCivicInfo.getGoldBonusForFirstToResearch() > 0)
	{
	    szHelpText.append(NEWLINE);
        szHelpText.append(gDLL->getText("TXT_KEY_INVENTION_BONUS_GOLD_FIRST", kCivicInfo.getGoldBonusForFirstToResearch()));
	}

    ///Requirements Here

    if (bCivilopediaText && (kCivicInfo.getRequiredInvention() != NO_CIVIC || kCivicInfo.getRequiredInvention2() != NO_CIVIC))
	{

	    if (kCivicInfo.getRequiredInvention() != NO_CIVIC && kCivicInfo.getRequiredInvention2() != NO_CIVIC && kCivicInfo.getRequiredInventionOr() == NO_CIVIC)
        {
            CvCivicInfo& kRequiredCivicInfo = GC.getCivicInfo((CivicTypes)kCivicInfo.getRequiredInvention());
            CvCivicInfo& kRequiredCivicInfo2 = GC.getCivicInfo((CivicTypes)kCivicInfo.getRequiredInvention2());
            szHelpText.append(NEWLINE);
            szHelpText.append(gDLL->getText("TXT_KEY_REQUIRED_INVENTION_TWO", kRequiredCivicInfo.getDescription(), kRequiredCivicInfo2.getDescription()));

        }
        else if (kCivicInfo.getRequiredInvention() != NO_CIVIC && kCivicInfo.getRequiredInvention2() != NO_CIVIC && kCivicInfo.getRequiredInventionOr() != NO_CIVIC)
        {
           CvCivicInfo& kRequiredCivicInfo = GC.getCivicInfo((CivicTypes)kCivicInfo.getRequiredInvention());
           CvCivicInfo& kRequiredCivicInfo2 = GC.getCivicInfo((CivicTypes)kCivicInfo.getRequiredInvention2());
           CvCivicInfo& kRequiredCivicInfoOr = GC.getCivicInfo((CivicTypes)kCivicInfo.getRequiredInventionOr());
           szHelpText.append(NEWLINE);
           szHelpText.append(gDLL->getText("TXT_KEY_REQUIRED_INVENTION_TWO_OR", kRequiredCivicInfo.getDescription(), kRequiredCivicInfo2.getDescription(), kRequiredCivicInfoOr.getDescription()));
        }
        else if (kCivicInfo.getRequiredInvention() != NO_CIVIC)
        {
           CvCivicInfo& kRequiredCivicInfo = GC.getCivicInfo((CivicTypes)kCivicInfo.getRequiredInvention());
           szHelpText.append(NEWLINE);
           szHelpText.append(gDLL->getText("TXT_KEY_REQUIRED_INVENTION_ONE", kRequiredCivicInfo.getDescription()));
        }
        else if (kCivicInfo.getRequiredInvention2() != NO_CIVIC)
        {
           CvCivicInfo& kRequiredCivicInfo = GC.getCivicInfo((CivicTypes)kCivicInfo.getRequiredInvention2());
           szHelpText.append(NEWLINE);
           szHelpText.append(gDLL->getText("TXT_KEY_REQUIRED_INVENTION_ONE", kRequiredCivicInfo.getDescription()));
        }



	}

	UnitClassTypes eRequiredUnit = (UnitClassTypes)kCivicInfo.getRequiredUnitType();
	if (eRequiredUnit != NO_UNITCLASS)
	{
        for (int i = 0; i < GC.getNumUnitInfos(); ++i)
        {
            CvUnitInfo& kRequiredUnitInfo = GC.getUnitInfo((UnitTypes) i);

            if (kRequiredUnitInfo.getUnitClassType() == eRequiredUnit)
            {
                szHelpText.append(NEWLINE);
                szHelpText.append(gDLL->getText("TXT_KEY_REQUIRED_UNITCLASS", kRequiredUnitInfo.getDescription()));
                break;
            }
        }
	}

    if (kCivicInfo.getDisallowsTech() != NO_CIVIC && (CivicTypes)kCivicInfo.getInventionCategory() != (CivicTypes)GC.getXMLval(XML_MEDIEVAL_TRADE_TECH))
    {
        szHelpText.append(NEWLINE);
        szHelpText.append(gDLL->getText("TXT_KEY_DISALLOWS_TECH", GC.getCivicInfo((CivicTypes)kCivicInfo.getDisallowsTech()).getDescription()));
    }

    if (kCivicInfo.isNoneTradeable() && (CivicTypes)kCivicInfo.getInventionCategory() != (CivicTypes)GC.getXMLval(XML_MEDIEVAL_TRADE_TECH) && (CivicTypes)kCivicInfo.getInventionCategory() != (CivicTypes)GC.getXMLval(XML_MEDIEVAL_CENSURE))
    {
        szHelpText.append(NEWLINE);
        szHelpText.append(gDLL->getText("TXT_KEY_NON_TRADEABLE"));
    }

	///TKe

	if (!isEmpty(kCivicInfo.getHelp()))
	{
		szHelpText.append(CvWString::format(L"%s%s", NEWLINE, kCivicInfo.getHelp()).c_str());
	}
}

void CvGameTextMgr::setBasicUnitHelp(CvWStringBuffer &szBuffer, UnitTypes eUnit, bool bCivilopediaText)
{
	PROFILE_FUNC();

	CvWString szTempBuffer;
	bool bFirst;
	int iCount;
	int iI;

	if (NO_UNIT == eUnit)
	{
		return;
	}

	CvUnitInfo& kUnitInfo = GC.getUnitInfo(eUnit);

	if (!bCivilopediaText)
	{
		szBuffer.append(NEWLINE);
		if (kUnitInfo.getCombat() > 0)
		{
			szTempBuffer.Format(L"%d%c, ", kUnitInfo.getCombat(), gDLL->getSymbolID(STRENGTH_CHAR));
			szBuffer.append(szTempBuffer);
		}
		szTempBuffer.Format(L"%d%c", kUnitInfo.getMoves(), gDLL->getSymbolID(MOVES_CHAR));
		szBuffer.append(szTempBuffer);
	}

	if (kUnitInfo.getLeaderExperience() > 0)
	{
		if (0 == GC.getXMLval(XML_WARLORD_EXTRA_EXPERIENCE_PER_UNIT_PERCENT))
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_LEADER", kUnitInfo.getLeaderExperience()));
		}
		else
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_LEADER_EXPERIENCE", kUnitInfo.getLeaderExperience()));
		}
	}

	if (NO_PROMOTION != kUnitInfo.getLeaderPromotion())
	{
		szBuffer.append(CvWString::format(L"%s%c%s", NEWLINE, gDLL->getSymbolID(BULLET_CHAR), gDLL->getText("TXT_KEY_PROMOTION_WHEN_LEADING").GetCString()));
		parsePromotionHelp(szBuffer, (PromotionTypes)kUnitInfo.getLeaderPromotion(), L"\n   ");
	}

	if (kUnitInfo.getCargoSpace() > 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_CARGO_SPACE", kUnitInfo.getCargoSpace()));
		if (kUnitInfo.getSpecialCargo() != NO_SPECIALUNIT)
		{
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_CARRIES", GC.getSpecialUnitInfo((SpecialUnitTypes) kUnitInfo.getSpecialCargo()).getTextKeyWide()));
		}
	}

	if (kUnitInfo.getRequiredTransportSize() > 1)
	{
		for (int i = 0; i < GC.getNumUnitInfos(); ++i)
		{
			CvUnitInfo& kTransportUnitInfo = GC.getUnitInfo((UnitTypes) i);
			if (kTransportUnitInfo.getCargoSpace() >= kUnitInfo.getRequiredTransportSize())
			{
			    if (kTransportUnitInfo.getSpecialCargo() == NO_SPECIALUNIT || kUnitInfo.getSpecialCargo() == kTransportUnitInfo.getSpecialCargo())
			    {
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_UNIT_CARGO", kTransportUnitInfo.getTextKeyWide()));
			    }
			}
		}
	}
    ///TKs Med
	// Tk New Food
	if (kUnitInfo.getFoodConsumed() != GC.getFOOD_CONSUMPTION_PER_POPULATION())
	{
		if (kUnitInfo.getFoodConsumed() > 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_CONSUMED_FOOD", kUnitInfo.getFoodConsumed()));
		}
		else
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_CONSUMED_NO_FOOD"));
		}
	}

    if (kUnitInfo.isPreventTraveling())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_DOMESTIC_TRANSPORT"));
	}

	if (kUnitInfo.isPreventFounding())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_CAN_NOT_FOUND"));
	}
	bool bAlso = false;
	if (kUnitInfo.getLaborForceUnitClass() != NO_UNITCLASS)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_WORKFORCE_EDUCATION_UNIT", GC.getUnitClassInfo((UnitClassTypes)kUnitInfo.getLaborForceUnitClass()).getTextKeyWide()));
		bAlso = true;
	}

    if (kUnitInfo.getEducationUnitClass() != NO_UNITCLASS)
	{
		szBuffer.append(NEWLINE);
		if (bAlso && bCivilopediaText)
		{
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_EDUCATION_CLASS_ALSO", GC.getUnitClassInfo((UnitClassTypes)kUnitInfo.getEducationUnitClass()).getTextKeyWide(), GC.getYieldInfo(YIELD_EDUCATION).getChar()));
		}
		else
		{
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_EDUCATION_CLASS", GC.getUnitClassInfo((UnitClassTypes)kUnitInfo.getEducationUnitClass()).getTextKeyWide(), GC.getYieldInfo(YIELD_EDUCATION).getChar()));
		}
		bAlso = true;
	}

	if (kUnitInfo.getKnightDubbingWeight() > 0)
	{
		szBuffer.append(NEWLINE);
		if (bAlso && bCivilopediaText)
		{
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_EDUCATION_CLASS_ALSO", GC.getUnitClassInfo((UnitClassTypes)kUnitInfo.getEducationUnitClass()).getTextKeyWide(), GC.getYieldInfo(YIELD_BELLS).getChar()));
		}
		else
		{
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_EDUCATION_CLASS", GC.getUnitClassInfo((UnitClassTypes)kUnitInfo.getEducationUnitClass()).getTextKeyWide(), GC.getYieldInfo(YIELD_BELLS).getChar()));
		}
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_EDUCATE_HONORED_CLASS", GC.getUnitClassInfo((UnitClassTypes)kUnitInfo.getEducationUnitClass()).getTextKeyWide()));
	}

	if (kUnitInfo.getRehibilitateUnitClass() != NO_UNITCLASS)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_REHIBLILITATE_EDUCATION_UNIT", GC.getUnitClassInfo((UnitClassTypes)kUnitInfo.getRehibilitateUnitClass()).getTextKeyWide()));
	}

	if (kUnitInfo.getTradeBonus() > 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_TRADE_BONUS", kUnitInfo.getTradeBonus()));
	}
    ///TKe
	szTempBuffer.Format(L"%s%s ", NEWLINE, gDLL->getText("TXT_KEY_UNIT_CANNOT_ENTER").GetCString());

	bFirst = true;
	for (iI = 0; iI < GC.getNumTerrainInfos(); ++iI)
	{
		if (kUnitInfo.getTerrainImpassable(iI))
		{
			CvWString szTerrain;
			szTerrain.Format(L"<link=literal>%s</link>", GC.getTerrainInfo((TerrainTypes)iI).getDescription());
			setListHelp(szBuffer, szTempBuffer, szTerrain, L", ", bFirst);
			bFirst = false;
		}
	}

	for (iI = 0; iI < GC.getNumFeatureInfos(); ++iI)
	{
		if (kUnitInfo.getFeatureImpassable(iI))
		{
			CvWString szFeature;
			szFeature.Format(L"<link=literal>%s</link>", GC.getFeatureInfo((FeatureTypes)iI).getDescription());
			setListHelp(szBuffer, szTempBuffer, szFeature, L", ", bFirst);
			bFirst = false;
		}
	}

	// < JAnimals Mod Start >
	if (kUnitInfo.isAnimal())
	{
	    szBuffer.append(NEWLINE);
        szBuffer.append(gDLL->getText("TXT_KEY_UNIT_ANIMAL"));
        /*if (kUnitInfo.getAnimalPatrolWeight() < 1)
        {
            szBuffer.append(NEWLINE);
            szBuffer.append(gDLL->getText("TXT_KEY_UNIT_ANIMAL_AI_NO_PATROL_WEIGHT"));
        }
        else
        {
            szBuffer.append(NEWLINE);
            szBuffer.append(gDLL->getText("TXT_KEY_UNIT_ANIMAL_AI_PATROL_WEIGHT", kUnitInfo.getAnimalPatrolWeight()));
        }*/
	}

	for (iI = 0; iI < GC.getNumTerrainInfos(); ++iI)
	{
		if (kUnitInfo.getTerrainNative(iI))
		{
            szBuffer.append(NEWLINE);
            szBuffer.append(gDLL->getText("TXT_KEY_UNIT_TERRAIN_NATIVE", GC.getTerrainInfo((TerrainTypes) iI).getDescription()));
		}
	}
	for (iI = 0; iI < GC.getNumFeatureInfos(); ++iI)
	{
		if (kUnitInfo.getFeatureNative(iI))
		{
            szBuffer.append(NEWLINE);
            szBuffer.append(gDLL->getText("TXT_KEY_UNIT_FEATURE_NATIVE", GC.getFeatureInfo((FeatureTypes) iI).getDescription()));
		}
	}
	for (iI = 0; iI < GC.getNumBonusInfos(); ++iI)
	{
		if (kUnitInfo.getBonusNative(iI))
		{
            szBuffer.append(NEWLINE);
            szBuffer.append(gDLL->getText("TXT_KEY_UNIT_BONUS_NATIVE", GC.getBonusInfo((BonusTypes) iI).getDescription()));
		}
	}
	// < JAnimals Mod End >

	if (kUnitInfo.isInvisible())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_INVISIBLE_ALL"));
	}
	else if (kUnitInfo.getInvisibleType() != NO_INVISIBLE)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_INVISIBLE_MOST"));
	}
	for (iI = 0; iI < kUnitInfo.getNumSeeInvisibleTypes(); ++iI)
	{
		if (bCivilopediaText || (kUnitInfo.getSeeInvisibleType(iI) != kUnitInfo.getInvisibleType()))
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_SEE_INVISIBLE", GC.getInvisibleInfo((InvisibleTypes) kUnitInfo.getSeeInvisibleType(iI)).getTextKeyWide()));
		}
	}
	if (kUnitInfo.isCanMoveImpassable())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_CAN_MOVE_IMPASSABLE"));
	}

	if (kUnitInfo.isNoBadGoodies())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_NO_BAD_GOODIES"));
	}
	if (kUnitInfo.isHiddenNationality())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_HIDDEN_NATIONALITY"));
	}
	if (kUnitInfo.isAlwaysHostile())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_ALWAYS_HOSTILE"));
	}
	if (kUnitInfo.isCapturesCargo())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_CAPTURES_CARGO"));
	}
	if (kUnitInfo.isTreasure())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_TREASURE_HELP"));
	}
	if (kUnitInfo.isOnlyDefensive())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_ONLY_DEFENSIVE"));
	}
	if (kUnitInfo.isNoCapture())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_CANNOT_CAPTURE"));
	}
	if (kUnitInfo.isRivalTerritory())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_EXPLORE_RIVAL"));
	}
	///TKs
	if (kUnitInfo.isFound() && !kUnitInfo.isPreventTraveling() && !kUnitInfo.isPreventFounding())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_FOUND_CITY"));
	}

    if (kUnitInfo.getCasteAttribute() == 1)
    {
        szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_ONLY_WORKS_FIELD"));
    }

    if (kUnitInfo.getCasteAttribute() == 2)
    {
        szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_NONE_MILITARY"));
    }

    if (kUnitInfo.getCasteAttribute() == 4)
    {
        szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_NOBLE_LABOR_PENALTY", GC.getXMLval(XML_NOBLE_FIELD_LABOR_PENALTY)));
    }

    if (kUnitInfo.getCasteAttribute() == 6)
    {
        szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_NO_LEARN_NATIVES"));
    }
    ///Tke
	if (kUnitInfo.getWorkRate() > 0)
	{
		iCount = 0;
		for (iI = 0; iI < GC.getNumBuildInfos(); ++iI)
		{
			if (kUnitInfo.getBuilds(iI))
			{
				iCount++;
			}
		}
		if (iCount > 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_IMPROVE_PLOTS"));
		}
		else
		{
			bFirst = true;
			for (iI = 0; iI < GC.getNumBuildInfos(); ++iI)
			{
				if (kUnitInfo.getBuilds(iI))
				{
					szTempBuffer.Format(L"%s%s ", NEWLINE, gDLL->getText("TXT_KEY_UNIT_CAN").c_str());
					setListHelp(szBuffer, szTempBuffer, GC.getBuildInfo((BuildTypes) iI).getDescription(), L", ", bFirst);
					bFirst = false;
				}
			}
		}
	}
	if (kUnitInfo.getWorkRateModifier() != 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_FASTER_WORK_RATE", kUnitInfo.getWorkRateModifier()));
	}
	if (kUnitInfo.getMissionaryRateModifier() != 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_BETTER_MISSION_RATE", kUnitInfo.getMissionaryRateModifier()));
	}
	if (kUnitInfo.isNoDefensiveBonus())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_NO_DEFENSE_BONUSES"));
	}
	if (kUnitInfo.isFlatMovementCost())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_FLAT_MOVEMENT"));
	}
	if (kUnitInfo.isIgnoreTerrainCost())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_IGNORE_TERRAIN"));
	}
	if (kUnitInfo.getWithdrawalProbability() > 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_WITHDRAWL_PROBABILITY", kUnitInfo.getWithdrawalProbability()));
	}
	szTempBuffer.clear();
	for (int i = 0; i < GC.getNumBuildingClassInfos(); ++i)
	{
		if (kUnitInfo.isEvasionBuilding(i))
		{
			if (!szTempBuffer.empty())
			{
				szTempBuffer += gDLL->getText("TXT_KEY_OR");
			}
			szTempBuffer += GC.getBuildingClassInfo((BuildingClassTypes) i).getDescription();
		}
	}
	if (!szTempBuffer.empty())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_EVASION_BUILDINGS", szTempBuffer.GetCString()));
	}
	if (kUnitInfo.getCityAttackModifier() == kUnitInfo.getCityDefenseModifier())
	{
		if (kUnitInfo.getCityAttackModifier() != 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_CITY_STRENGTH_MOD", kUnitInfo.getCityAttackModifier()));
		}
	}
	else
	{
		if (kUnitInfo.getCityAttackModifier() != 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_CITY_ATTACK_MOD", kUnitInfo.getCityAttackModifier()));
		}

		if (kUnitInfo.getCityDefenseModifier() != 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_CITY_DEFENSE_MOD", kUnitInfo.getCityDefenseModifier()));
		}
	}

	if (kUnitInfo.getHillsDefenseModifier() == kUnitInfo.getHillsAttackModifier())
	{
		if (kUnitInfo.getHillsAttackModifier() != 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_HILLS_STRENGTH", kUnitInfo.getHillsAttackModifier()));
		}
	}
	else
	{
		if (kUnitInfo.getHillsAttackModifier() != 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_HILLS_ATTACK", kUnitInfo.getHillsAttackModifier()));
		}

		if (kUnitInfo.getHillsDefenseModifier() != 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_HILLS_DEFENSE", kUnitInfo.getHillsDefenseModifier()));
		}
	}

	for (iI = 0; iI < GC.getNumTerrainInfos(); ++iI)
	{
		if (kUnitInfo.getTerrainDefenseModifier(iI) != 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_DEFENSE", kUnitInfo.getTerrainDefenseModifier(iI), GC.getTerrainInfo((TerrainTypes) iI).getTextKeyWide()));
		}

		if (kUnitInfo.getTerrainAttackModifier(iI) != 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_ATTACK", kUnitInfo.getTerrainAttackModifier(iI), GC.getTerrainInfo((TerrainTypes) iI).getTextKeyWide()));
		}
	}

	for (iI = 0; iI < GC.getNumFeatureInfos(); ++iI)
	{
		if (kUnitInfo.getFeatureDefenseModifier(iI) != 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_DEFENSE", kUnitInfo.getFeatureDefenseModifier(iI), GC.getFeatureInfo((FeatureTypes) iI).getTextKeyWide()));
		}

		if (kUnitInfo.getFeatureAttackModifier(iI) != 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_ATTACK", kUnitInfo.getFeatureAttackModifier(iI), GC.getFeatureInfo((FeatureTypes) iI).getTextKeyWide()));
		}
	}

	for (iI = 0; iI < GC.getNumUnitClassInfos(); ++iI)
	{
		if (kUnitInfo.getUnitClassAttackModifier(iI) == kUnitInfo.getUnitClassDefenseModifier(iI))
		{
			if (kUnitInfo.getUnitClassAttackModifier(iI) != 0)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_UNIT_MOD_VS_TYPE", kUnitInfo.getUnitClassAttackModifier(iI), GC.getUnitClassInfo((UnitClassTypes)iI).getTextKeyWide()));
			}
		}
		else
		{
			if (kUnitInfo.getUnitClassAttackModifier(iI) != 0)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_UNIT_ATTACK_MOD_VS_CLASS", kUnitInfo.getUnitClassAttackModifier(iI), GC.getUnitClassInfo((UnitClassTypes)iI).getTextKeyWide()));
			}

			if (kUnitInfo.getUnitClassDefenseModifier(iI) != 0)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_UNIT_DEFENSE_MOD_VS_CLASS", kUnitInfo.getUnitClassDefenseModifier(iI), GC.getUnitClassInfo((UnitClassTypes) iI).getTextKeyWide()));
			}
		}
	}

	for (iI = 0; iI < GC.getNumUnitCombatInfos(); ++iI)
	{
		if (kUnitInfo.getUnitCombatModifier(iI) != 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_MOD_VS_TYPE_NO_LINK", kUnitInfo.getUnitCombatModifier(iI), GC.getUnitCombatInfo((UnitCombatTypes) iI).getTextKeyWide()));
		}
	}

	for (iI = 0; iI < NUM_DOMAIN_TYPES; ++iI)
	{
		if (kUnitInfo.getDomainModifier(iI) != 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_MOD_VS_TYPE_NO_LINK", kUnitInfo.getDomainModifier(iI), GC.getDomainInfo((DomainTypes)iI).getTextKeyWide()));
		}
	}

	if (kUnitInfo.getBombardRate() > 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_BOMBARD_RATE", ((kUnitInfo.getBombardRate() * 100) / GC.getMAX_CITY_DEFENSE_DAMAGE())));
	}

	bFirst = true;

	for (iI = 0; iI < GC.getNumPromotionInfos(); ++iI)
	{
		if (kUnitInfo.getFreePromotions(iI))
		{
			szTempBuffer.Format(L"%s%s", NEWLINE, gDLL->getText("TXT_KEY_UNIT_STARTS_WITH").c_str());
			setListHelp(szBuffer, szTempBuffer, CvWString::format(L"<link=literal>%s</link>", GC.getPromotionInfo((PromotionTypes) iI).getDescription()), L", ", bFirst);
			bFirst = false;
		}
	}

	if (bCivilopediaText)
	{
		// Trait
		if ((kUnitInfo.getDomainType() == DOMAIN_SEA || kUnitInfo.isMechUnit()) && kUnitInfo.getEuropeCost() > 0)
        {
            szBuffer.append(NEWLINE);
            szBuffer.append(gDLL->getText("TXT_KEY_PURCHASE_UNIT_AFTER_BUILD"));
        }
		for (int i = 0; i < GC.getNumTraitInfos(); ++i)
		{
			if (kUnitInfo.getProductionTraits((TraitTypes)i) != 0)
			{
				if (kUnitInfo.getProductionTraits((TraitTypes)i) == 100)
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_DOUBLE_SPEED_TRAIT", GC.getTraitInfo((TraitTypes)i).getTextKeyWide()));
				}
				else
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_PRODUCTION_MODIFIER_TRAIT", kUnitInfo.getProductionTraits((TraitTypes)i), GC.getTraitInfo((TraitTypes)i).getTextKeyWide()));
				}
			}
		}

        ///TKs Invention Core Mod v 1.0
        if (kUnitInfo.getFreeBuildingClass() != NO_BUILDINGCLASS)
        {
            BuildingTypes eFoundBuilding = NO_BUILDING;
            for (iI  = 0; iI < GC.getNumCivilizationInfos(); ++iI)
            {
				if (!(GC.getCivilizationInfo((CivilizationTypes)iI).isNative() && GC.getCivilizationInfo((CivilizationTypes)iI).isEurope()))
				{
					BuildingTypes eBuilding = (BuildingTypes) GC.getCivilizationInfo((CivilizationTypes)iI).getCivilizationBuildings(kUnitInfo.getFreeBuildingClass());
					if (eBuilding != NO_BUILDING && eBuilding != eFoundBuilding)
					{
						eFoundBuilding = eBuilding;
						szBuffer.append(NEWLINE);
						//szBuffer.append(gDLL->getText("TXT_KEY_UNIT_FREE_BUILDINGCLASS"));
						szBuffer.append(gDLL->getText("TXT_KEY_UNIT_FREE_BUILDINGCLASS_PLAYER", GC.getBuildingInfo(eBuilding).getDescription()));
					}
				}
            }
        }
        int iAllows = 0;
        for (int iCivic = 0; iCivic < GC.getNumCivicInfos(); iCivic++)
        {

            iAllows = GC.getCivicInfo((CivicTypes)iCivic).getAllowsUnitClasses(kUnitInfo.getUnitClassType());
            if (iAllows > 0)
            {
                if ((CivicTypes)GC.getCivicInfo((CivicTypes)iCivic).getInventionCategory() == (CivicTypes)GC.getXMLval(XML_MEDIEVAL_TRADE_TECH))
                {
                    szBuffer.append(NEWLINE);
                    szBuffer.append(gDLL->getText("TXT_KEY_TRADE_PERK_MAKES_AVAILABLE", GC.getCivicInfo((CivicTypes)iCivic).getDescription()));
                    break;
                }
                else
                {
                    szBuffer.append(NEWLINE);
                    szBuffer.append(gDLL->getText("TXT_KEY_TECH_MAKES_AVAILABLE", GC.getCivicInfo((CivicTypes)iCivic).getDescription()));
                    break;
                }
            }
            else if (iAllows < 0)
            {
                szBuffer.append(NEWLINE);
                szBuffer.append(gDLL->getText("TXT_KEY_TECH_MAKES_OBSOLETE", GC.getCivicInfo((CivicTypes)iCivic).getDescription()));
                break;
            }
        }
	}
	///TKe

	if (!isEmpty(kUnitInfo.getHelp()))
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(kUnitInfo.getHelp());
	}
}

void CvGameTextMgr::setUnitHelp(CvWStringBuffer &szBuffer, UnitTypes eUnit, bool bCivilopediaText, bool bStrategyText, CvCity* pCity)
{
	PROFILE_FUNC();

	CvWString szTempBuffer;
	PlayerTypes ePlayer;
	int iProduction;
	int iI;

	if (NO_UNIT == eUnit)
	{
		return;
	}

	if (pCity != NULL)
	{
		ePlayer = pCity->getOwnerINLINE();
	}
	else
	{
		ePlayer = GC.getGameINLINE().getActivePlayer();
	}

	if (!bCivilopediaText)
	{
		szTempBuffer.Format(SETCOLR L"%s" ENDCOLR , TEXT_COLOR("COLOR_UNIT_TEXT"), GC.getUnitInfo(eUnit).getDescription());
		szBuffer.append(szTempBuffer);

		if (GC.getUnitInfo(eUnit).getUnitCombatType() != NO_UNITCOMBAT)
		{
			szTempBuffer.Format(L" (%s)", GC.getUnitCombatInfo((UnitCombatTypes) GC.getUnitInfo(eUnit).getUnitCombatType()).getDescription());
			szBuffer.append(szTempBuffer);
		}
	}

	// test for unique unit
	UnitClassTypes eUnitClass = (UnitClassTypes)GC.getUnitInfo(eUnit).getUnitClassType();
	UnitTypes eDefaultUnit = (UnitTypes)GC.getUnitClassInfo(eUnitClass).getDefaultUnitIndex();
	if (ePlayer != NO_PLAYER && GET_PLAYER(ePlayer).getUnitClassCount(eUnitClass) > 0)
	{
		if (GC.getUnitInfo(eUnit).isMechUnit() || GC.getUnitInfo(eUnit).getDomainType() == DOMAIN_SEA)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_NUMBER_UNICLASS_COUNT_MECHS", GET_PLAYER(ePlayer).getUnitClassCount(eUnitClass)));
		}
		else
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_NUMBER_UNICLASS_COUNT", GET_PLAYER(ePlayer).getUnitClassCount(eUnitClass)));
		}
	}
    ///TKs Med Update 1.1g
	if (bCivilopediaText && NO_UNIT != eDefaultUnit && eDefaultUnit != eUnit)
	{
		for (iI  = 0; iI < GC.getNumCivilizationInfos(); ++iI)
		{
			UnitTypes eUniqueUnit = (UnitTypes)GC.getCivilizationInfo((CivilizationTypes)iI).getCivilizationUnits((int)eUnitClass);

			if (eUniqueUnit == eUnit)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_UNIQUE_UNIT", GC.getCivilizationInfo((CivilizationTypes)iI).getTextKeyWide()));
			}
		}

		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_REPLACES_UNIT", GC.getUnitInfo(eDefaultUnit).getTextKeyWide()));

	}
    ///TKe Update
	setBasicUnitHelp(szBuffer, eUnit, bCivilopediaText);

	std::map<int, CvWString> mapModifiers;
	std::map<int, CvWString> mapChanges;
	std::map<int, CvWString> mapBonus;
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		YieldTypes eYield = (YieldTypes) iYield;

		int iModifier = GC.getUnitInfo(eUnit).getYieldModifier(eYield);
		if (iModifier != 0)
		{
			mapModifiers[iModifier] += CvWString::format(L"%c", GC.getYieldInfo(eYield).getChar());
		}

		int iChange = GC.getUnitInfo(eUnit).getYieldChange(eYield);
		if (iChange != 0)
		{
			mapChanges[iChange] += CvWString::format(L"%c", GC.getYieldInfo(eYield).getChar());
		}

		iChange = GC.getUnitInfo(eUnit).getBonusYieldChange(eYield);
		if (iChange != 0)
		{
			mapBonus[iChange] += CvWString::format(L"%c", GC.getYieldInfo(eYield).getChar());
		}
	}

	if (GC.getUnitInfo(eUnit).isLandYieldChanges() && GC.getUnitInfo(eUnit).isWaterYieldChanges())
	{
		for (std::map<int, CvWString>::iterator it = mapModifiers.begin(); it != mapModifiers.end(); ++it)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_YIELD_MODIFIER", it->first, it->second.GetCString()));
		}

		for (std::map<int, CvWString>::iterator it = mapChanges.begin(); it != mapChanges.end(); ++it)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_YIELD_CHANGE", it->first, it->second.GetCString()));
		}
	}
	else if (GC.getUnitInfo(eUnit).isLandYieldChanges())
	{
		for (std::map<int, CvWString>::iterator it = mapModifiers.begin(); it != mapModifiers.end(); ++it)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_YIELD_MODIFIER_LAND", it->first, it->second.GetCString()));
		}

		for (std::map<int, CvWString>::iterator it = mapChanges.begin(); it != mapChanges.end(); ++it)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_YIELD_CHANGE_LAND", it->first, it->second.GetCString()));
		}
	}
	else if (GC.getUnitInfo(eUnit).isWaterYieldChanges())
	{
		for (std::map<int, CvWString>::iterator it = mapModifiers.begin(); it != mapModifiers.end(); ++it)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_YIELD_MODIFIER_WATER", it->first, it->second.GetCString()));
		}

		for (std::map<int, CvWString>::iterator it = mapChanges.begin(); it != mapChanges.end(); ++it)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_YIELD_CHANGE_WATER", it->first, it->second.GetCString()));
		}
	}

	if (GC.getUnitInfo(eUnit).isLandYieldChanges() || GC.getUnitInfo(eUnit).isWaterYieldChanges())
	{
		for (std::map<int, CvWString>::iterator it = mapBonus.begin(); it != mapBonus.end(); ++it)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_BONUS_YIELD_CHANGE", it->first, it->second.GetCString()));
		}
	}

	// R&R, Androrc, Domestic Market
	// R&R, ray, adjustment Domestic Markets, displaying as list
	CvWString szYieldsDemandedList;
	for (iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		if(GC.getUnitInfo(eUnit).getYieldDemand((YieldTypes) iI) != 0)
		{
			szYieldsDemandedList += CvWString::format(L"%c", GC.getYieldInfo((YieldTypes) iI).getChar());
		}
	}
	if(!isEmpty(szYieldsDemandedList))
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getSymbolID(BULLET_CHAR));
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_YIELD_DEMAND", szYieldsDemandedList.GetCString()));
	}
	//Androrc End

	if ((pCity == NULL) || !(pCity->canTrain(eUnit)))
	{
		if (GC.getUnitInfo(eUnit).getPrereqBuilding() != NO_BUILDINGCLASS)
		{
			BuildingTypes eBuilding;
			if (ePlayer == NO_PLAYER)
			{
				eBuilding = (BuildingTypes) GC.getBuildingClassInfo((BuildingClassTypes) GC.getUnitInfo(eUnit).getPrereqBuilding()).getDefaultBuildingIndex();
			}
			else
			{
				eBuilding = (BuildingTypes) GC.getCivilizationInfo(GET_PLAYER(ePlayer).getCivilizationType()).getCivilizationBuildings(GC.getUnitInfo(eUnit).getPrereqBuilding());
			}
			if(eBuilding != NO_BUILDING)
			{
				if ((pCity == NULL) || (!pCity->isHasConceptualBuilding(eBuilding)))
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_UNIT_REQUIRES_STRING", GC.getBuildingInfo(eBuilding).getTextKeyWide()));
				}
			}
		}

		bool bValid = true;
		bool bFirst = true;
		szTempBuffer.clear();
		for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); ++iBuildingClass)
		{
			if (GC.getUnitInfo(eUnit).isPrereqOrBuilding(iBuildingClass))
			{
				bValid = false;
				BuildingTypes eBuilding = (BuildingTypes) GC.getBuildingClassInfo((BuildingClassTypes) iBuildingClass).getDefaultBuildingIndex();
				if (ePlayer != NO_PLAYER)
				{
					eBuilding = (BuildingTypes) GC.getCivilizationInfo(GET_PLAYER(ePlayer).getCivilizationType()).getCivilizationBuildings(iBuildingClass);
				}

				if (NO_BUILDING != eBuilding)
				{
					if (pCity != NULL && pCity->isHasConceptualBuilding(eBuilding) )
					{
						bValid = true;
						break;
					}

					if (!bFirst)
					{
						szTempBuffer += gDLL->getText("TXT_KEY_OR");
					}
					szTempBuffer += GC.getBuildingInfo(eBuilding).getDescription();
					bFirst = false;
				}
			}
		}

		if (!bValid)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_REQUIRES_STRING", szTempBuffer.GetCString()));
		}


		if (!bCivilopediaText)
		{
			for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
			{
				if (GC.getUnitInfo(eUnit).getYieldCost(iYield) > 0)
				{
					YieldTypes eYield = (YieldTypes) iYield;
					if (GC.getYieldInfo(eYield).isCargo())
					{
						int iCost = (ePlayer == NO_PLAYER ? GC.getUnitInfo(eUnit).getYieldCost(iYield) : GET_PLAYER(ePlayer).getYieldProductionNeeded(eUnit, eYield));
						if (NULL == pCity || pCity->getYieldStored(eYield) + pCity->getYieldRushed(eYield) < iCost)
						{
							szBuffer.append(NEWLINE);
							szBuffer.append(gDLL->getText("TXT_KEY_BUILD_CANNOT_AFFORD", iCost, GC.getYieldInfo(eYield).getChar()));
						}
					}
				}
			}
		}
	}

	if (!bCivilopediaText && GC.getGameINLINE().getActivePlayer() != NO_PLAYER)
	{
		if (pCity == NULL)
		{
			int iCost = GET_PLAYER(ePlayer).getYieldProductionNeeded(eUnit, YIELD_HAMMERS);
			if (iCost > 0)
			{
				szTempBuffer.Format(L"%s%d%c", NEWLINE, iCost, GC.getYieldInfo(YIELD_HAMMERS).getChar());
				szBuffer.append(szTempBuffer);
			}
		}
		else
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_TURNS", pCity->getProductionTurnsLeft(eUnit, ((gDLL->ctrlKey() || !(gDLL->shiftKey())) ? 0 : pCity->getOrderQueueLength())), pCity->getYieldProductionNeeded(eUnit, YIELD_HAMMERS), GC.getYieldInfo(YIELD_HAMMERS).getChar()));

			iProduction = pCity->getUnitProduction(eUnit);

			if (iProduction > 0)
			{
				szTempBuffer.Format(L" - %d/%d%c", iProduction, pCity->getYieldProductionNeeded(eUnit, YIELD_HAMMERS), GC.getYieldInfo(YIELD_HAMMERS).getChar());
				szBuffer.append(szTempBuffer);
			}
			else
			{
				szTempBuffer.Format(L" - %d%c", pCity->getYieldProductionNeeded(eUnit, YIELD_HAMMERS), GC.getYieldInfo(YIELD_HAMMERS).getChar());
				szBuffer.append(szTempBuffer);
			}

			for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
			{
				if (GC.getUnitInfo(eUnit).getYieldCost(iYield) > 0)
				{
					YieldTypes eYield = (YieldTypes) iYield;
					if (GC.getYieldInfo(eYield).isCargo())
					{
						int iCost = GET_PLAYER(pCity->getOwnerINLINE()).getYieldProductionNeeded(eUnit, eYield);
						if (iCost > 0)
						{
							szTempBuffer.Format(L" - %d/%d%c", pCity->getYieldStored(eYield) + pCity->getYieldRushed(eYield), iCost, GC.getYieldInfo(eYield).getChar());
							szBuffer.append(szTempBuffer);
						}
					}
				}
			}
		}
	}

	if (bStrategyText)
	{
		if (!isEmpty(GC.getUnitInfo(eUnit).getStrategy()))
		{
			if ((ePlayer == NO_PLAYER) || GET_PLAYER(ePlayer).isOption(PLAYEROPTION_ADVISOR_HELP))
			{
				szBuffer.append(SEPARATOR);
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_SIDS_TIPS"));
				szBuffer.append(L'\"');
				szBuffer.append(GC.getUnitInfo(eUnit).getStrategy());
				szBuffer.append(L'\"');
			}
		}
	}

	if (bCivilopediaText)
	{
		if(NO_UNIT != eDefaultUnit && eDefaultUnit == eUnit)
		{
			for(iI = 0; iI < GC.getNumUnitInfos(); ++iI)
			{
				if(((UnitTypes)iI) == eUnit)
				{
					continue;
				}

				if(eUnitClass == ((UnitClassTypes)GC.getUnitInfo((UnitTypes)iI).getUnitClassType()))
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_REPLACED_BY_UNIT", GC.getUnitInfo((UnitTypes)iI).getTextKeyWide()));
				}
			}
		}
	}

	///TKs Med Update 1.1g
//	if (GC.getUnitInfo(eUnit).getUnitClassType() == (UnitClassTypes)GC.getDefineINT("DEFAULT_SLAVE_CLASS"))
//	{
//	    szBuffer.append(NEWLINE);
//        szBuffer.append(gDLL->getText("TXT_KEY_SLAVE_FOOD_HELP", GC.getDefineINT("SLAVE_FOOD_CONSUMPTION_PER_POPULATION")));
//	}
	///Tke Update

	if (pCity != NULL)
	{
		if ((gDLL->getChtLvl() > 0) && gDLL->ctrlKey())
		{
			szBuffer.append(NEWLINE);
			for (int iUnitAI = 0; iUnitAI < NUM_UNITAI_TYPES; iUnitAI++)
			{
				int iTempValue = GET_PLAYER(pCity->getOwner()).AI_unitValue(eUnit, (UnitAITypes)iUnitAI, pCity->area());
				if (iTempValue != 0)
				{
					CvWString szTempString;
					getUnitAIString(szTempString, (UnitAITypes)iUnitAI);
					szBuffer.append(CvWString::format(L"(%s : %d) ", szTempString.GetCString(), iTempValue));
				}
			}
		}
	}
}
void CvGameTextMgr::setBuildingHelp(CvWStringBuffer &szBuffer, BuildingTypes eBuilding, bool bCivilopediaText, bool bStrategyText, CvCity* pCity)
{
	PROFILE_FUNC();

	CvWString szFirstBuffer;
	CvWString szTempBuffer;
	BuildingTypes eLoopBuilding;
	PlayerTypes ePlayer;
	bool bFirst;
	int iProduction;
	int iLast;
	int iI;

	if (NO_BUILDING == eBuilding)
	{
		return;
	}

	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);


	if (pCity != NULL)
	{
		ePlayer = pCity->getOwnerINLINE();
	}
	else
	{
		ePlayer = GC.getGameINLINE().getActivePlayer();
	}
    ///Tks Med
    bool bCrosses = false;
	if (!bCivilopediaText)
	{
		// MultipleYieldsConsumed Start by Aymerick 05/01/2010
		szTempBuffer.Format( SETCOLR L"<link=literal>%s</link>" ENDCOLR , TEXT_COLOR("COLOR_BUILDING_TEXT"), kBuilding.getDescription());
		szBuffer.append(szTempBuffer);

		std::vector<YieldTypes> eBuildingYieldsConversion;

		if(kBuilding.getProfessionOutput() != 0)
		{
			for (iI = 0; iI < GC.getNumProfessionInfos(); ++iI)
			{
				if (ePlayer == NO_PLAYER || GC.getCivilizationInfo(GET_PLAYER(ePlayer).getCivilizationType()).isValidProfession(iI))
				{
					CvProfessionInfo& kProfession = GC.getProfessionInfo((ProfessionTypes) iI);
					if (kProfession.getSpecialBuilding() == kBuilding.getSpecialBuildingType())
					{
						// MultipleYieldsProduced Start by Aymerick 22/01/2010*
						///TKs Med
						YieldTypes eYieldProduced = (YieldTypes)kProfession.getYieldsProduced(0);
						if (eYieldProduced == YIELD_CROSSES)
						{
						    bCrosses = true;
						}
						if (eYieldProduced != NO_YIELD)
						{
                            if (pCity != NULL)
                            {
                                if (GC.getYieldInfo(eYieldProduced).isArmor())
                                {
                                   if (pCity->getSelectedArmor() != eYieldProduced)
                                   {
                                       eYieldProduced = pCity->getSelectedArmor();
                                   }
                                }
                            }
							for (int j = 0; j < kProfession.getNumYieldsConsumed(ePlayer); j++)
							{
								YieldTypes eYieldConsumed = (YieldTypes) kProfession.getYieldsConsumed(j, ePlayer);
								if (eYieldConsumed != NO_YIELD)
								{
									eBuildingYieldsConversion.push_back((YieldTypes) eYieldConsumed);
								}
							}

							if (!eBuildingYieldsConversion.empty())
							{
								CvWString szYieldsList;
								for (std::vector<YieldTypes>::iterator it = eBuildingYieldsConversion.begin(); it != eBuildingYieldsConversion.end(); ++it)
								{
									if (!szYieldsList.empty())
									{
										if (*it == eBuildingYieldsConversion.back())
										{
											szYieldsList += CvWString::format(gDLL->getText("TXT_KEY_AND"));
										}
										else
										{
											szYieldsList += L", ";
										}
									}
									szYieldsList += CvWString::format(L"%c", GC.getYieldInfo(*it).getChar());
								}
								szBuffer.append(NEWLINE);
								szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_YIELDS_CONVERSION", szYieldsList.GetCString(), GC.getYieldInfo(eYieldProduced).getChar()));
							}

							szBuffer.append(NEWLINE);
							szBuffer.append(gDLL->getSymbolID(BULLET_CHAR));
							szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_PROFESSION_OUTPUT", kBuilding.getProfessionOutput(), GC.getYieldInfo(eYieldProduced).getChar()));
						}
						// MultipleYieldsProduced End
					}
				}
			}
		}
		// MultipleYieldsConsumed End
		int aiYields[NUM_YIELD_TYPES];
		//bool bAutoSale = false;
		for (iI = 0; iI < NUM_YIELD_TYPES; ++iI)
		{


            if (kBuilding.getYieldChange(iI) > 0 && kBuilding.getAutoSellYieldChange() != NO_YIELD)
            {
                aiYields[iI] = 0;
            }
            else
            {
                aiYields[iI] = kBuilding.getYieldChange(iI);
            }

			if (NULL != pCity)
			{
				aiYields[iI] += pCity->getBuildingYieldChange((BuildingClassTypes)kBuilding.getBuildingClassType(), (YieldTypes)iI);
			}

			if (ePlayer != NO_PLAYER)
			{
				aiYields[iI] += GET_PLAYER(ePlayer).getBuildingYieldChange((BuildingClassTypes)kBuilding.getBuildingClassType(), (YieldTypes)iI);
			}
			//bAutoSale = false;
		}
		setYieldChangeHelp(szBuffer, L", ", L"", L"", aiYields, false, false);
		setYieldChangeHelp(szBuffer, L", ", L"", L"", kBuilding.getYieldModifierArray(), true, bCivilopediaText);
//		for (iI = 0; iI < NUM_YIELD_TYPES; ++iI)
//		{
//            if (kBuilding.getYieldChange(iI) > 0 && kBuilding.getAutoSellYieldChange() != NO_YIELD)
//            {
//                YieldTypes eAutoSell = (YieldTypes)kBuilding.getAutoSellYieldChange();
//                szBuffer.append(NEWLINE);
//                szBuffer.append(gDLL->getSymbolID(BULLET_CHAR));
//                szBuffer.append(gDLL->getText("TXT_KEY_MUST_AUTO_SELL", GC.getYieldInfo(eAutoSell).getChar(),  kBuilding.getYieldChange(iI), GC.getYieldInfo((YieldTypes)iI).getChar()));
//                //szBuffer.append(NEWLINE);
//                //bAutoSale = true;
//            }
//		}
	}
	for (iI = 0; iI < NUM_YIELD_TYPES; ++iI)
    {
        if (kBuilding.getYieldChange(iI) > 0 && kBuilding.getAutoSellYieldChange() != NO_YIELD)
        {
            YieldTypes eAutoSell = (YieldTypes)kBuilding.getAutoSellYieldChange();
            szBuffer.append(NEWLINE);
            szBuffer.append(gDLL->getSymbolID(BULLET_CHAR));
            szBuffer.append(gDLL->getText("TXT_KEY_MUST_AUTO_SELL", GC.getYieldInfo(eAutoSell).getChar(),  kBuilding.getYieldChange(iI), GC.getYieldInfo((YieldTypes)iI).getChar()));
            //szBuffer.append(NEWLINE);
            //bAutoSale = true;
        }
    }
    ///TKs
    if (!bCivilopediaText)
    {
        if (kBuilding.isArmory())
        {
            szBuffer.append(NEWLINE);
            szBuffer.append(gDLL->getText("TXT_KEY_ARMORSMITH_BUILDING_CLICK_HELP"));
        }
        if ((ModCodeTypes)kBuilding.getWhoCanBuildTypes() == MODER_CODE_TRADING_POST || (ModCodeTypes)kBuilding.getWhoCanBuildTypes() == MODER_CODE_ALLOWS_TRADE_FAIR)
        {
            szBuffer.append(NEWLINE);
            szBuffer.append(gDLL->getText("TXT_KEY_CLICK_MARKET_SELECT"));
        }

        if (ePlayer != NO_PLAYER)
        {
            if (GET_PLAYER(ePlayer).getCensureType(CENSURE_INTERDICT) > 0 || GET_PLAYER(ePlayer).getCensureType(CENSURE_ANATHEMA) > 0)
            {
                //YieldTypes eYieldProduced = (YieldTypes)kProfession.getYieldsProduced(0);
                if (bCrosses)
                {
                    szBuffer.append(NEWLINE);
                    szBuffer.append(gDLL->getText("TXT_KEY_CENSURE_EXCOMMUNICATION_TURNS", GET_PLAYER(ePlayer).getCensureType(CENSURE_EXCOMMUNICATION)));
                }
            }
        }
    }
    if ((ModCodeTypes)kBuilding.getWhoCanBuildTypes() == MODER_CODE_ALLOWS_TRADE_FAIR)
    {
        szBuffer.append(NEWLINE);
        szBuffer.append(gDLL->getText("TXT_KEY_CLICK_TRADE_FAIR_DEPART"));
    }
    if (kBuilding.getSpecialBuildingType() == (SpecialBuildingTypes)GC.getXMLval(XML_DEFAULT_SPECIALBUILDING_COURTHOUSE))
    {
        szBuffer.append(NEWLINE);
        szBuffer.append(gDLL->getText("TXT_KEY_UNIT_REHIBILITATE_HELP"));
    }
	//Tks Civics
	for (iI = 0; iI < kBuilding.getNumCivicTreasuryBonus(); iI++)
    {
        szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_CIVIC_TREASURY_BUILDING", GC.getCivicInfo((CivicTypes)kBuilding.getCivicTreasury(iI)).getDescription(), kBuilding.getCivicTreasuryBonus(iI)));
    }
    ///TKs Med Update 1.1g
    if (bCivilopediaText)
    {
        MedCityTypes eCityType = (MedCityTypes)kBuilding.getCityType();
        if (eCityType == CITYTYPE_COMMERCE)
        {
            szBuffer.append(NEWLINE);
            szBuffer.append(gDLL->getText("TXT_KEY_UNIT_BUILDING_CITYTYPE_HELP1"));
        }
        else if (eCityType == CITYTYPE_MONASTERY)
        {
            szBuffer.append(NEWLINE);
            szBuffer.append(gDLL->getText("TXT_KEY_UNIT_BUILDING_CITYTYPE_HELP2"));
        }
        else if (eCityType == CITYTYPE_OUTPOST || eCityType == CITYTYPE_BAILEY || eCityType == CITYTYPE_CASTLE || eCityType == CITYTYPE_MILITARY)
        {
            szBuffer.append(NEWLINE);
            szBuffer.append(gDLL->getText("TXT_KEY_UNIT_BUILDING_CITYTYPE_HELP3"));
        }
        else if (eCityType == CITYTYPE_NONE_MONASTERY)
        {
            szBuffer.append(NEWLINE);
            szBuffer.append(gDLL->getText("TXT_KEY_UNIT_BUILDING_CITYTYPE_HELP4"));
        }
    }



    ///TKe

	// domestic yield demand - start - Nightinggale
	// based heavily on Androrc Domestic Market
	CvWString szYieldsDemandedList;
	for (iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		if(kBuilding.getYieldDemand((YieldTypes) iI) != 0)
		{
			szYieldsDemandedList += CvWString::format(L"%c", GC.getYieldInfo((YieldTypes) iI).getChar());
		}
	}
	if(!isEmpty(szYieldsDemandedList))
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getSymbolID(BULLET_CHAR));
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_YIELD_DEMAND", szYieldsDemandedList.GetCString()));
	}
	/*for (iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		if(kBuilding.getYieldDemand((YieldTypes) iI) != 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getSymbolID(BULLET_CHAR));
			szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_YIELD_DEMAND", kBuilding.getYieldDemand((YieldTypes) iI), GC.getYieldInfo((YieldTypes) iI).getChar()));
		}
	}*/
	// domestic yield demand - end - Nightinggale
	// test for unique building
	BuildingClassTypes eBuildingClass = (BuildingClassTypes)kBuilding.getBuildingClassType();
	BuildingTypes eDefaultBuilding = (BuildingTypes)GC.getBuildingClassInfo(eBuildingClass).getDefaultBuildingIndex();
    ///TKs Med Update 1.1g
	if (bCivilopediaText && NO_BUILDING != eDefaultBuilding && eDefaultBuilding != eBuilding)
	{
		for (iI  = 0; iI < GC.getNumCivilizationInfos(); ++iI)
		{
			BuildingTypes eUniqueBuilding = (BuildingTypes)GC.getCivilizationInfo((CivilizationTypes)iI).getCivilizationBuildings((int)eBuildingClass);

			if (bCivilopediaText && eUniqueBuilding == eBuilding)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_UNIQUE_BUILDING", GC.getCivilizationInfo((CivilizationTypes)iI).getTextKeyWide()));
			}
		}

        szBuffer.append(NEWLINE);
        szBuffer.append(gDLL->getText("TXT_KEY_REPLACES_UNIT", GC.getBuildingInfo(eDefaultBuilding).getTextKeyWide()));

	}
    if (bCivilopediaText)
    {
        BuildingTypes eNextBuilding = (BuildingTypes) kBuilding.getNextSpecialBuilding();
        while (eNextBuilding != eBuilding)
        {
            CvBuildingInfo& kNextBuilding = GC.getBuildingInfo(eNextBuilding);

            if (kBuilding.getSpecialBuildingPriority() > kNextBuilding.getSpecialBuildingPriority())
            {
                szBuffer.append(NEWLINE);
                szBuffer.append(gDLL->getSymbolID(BULLET_CHAR));
                szBuffer.append(gDLL->getText("TXT_KEY_REPLACES_UNIT", kNextBuilding.getTextKeyWide()));
            }

            eNextBuilding = (BuildingTypes) kNextBuilding.getNextSpecialBuilding();
        }
    }
///TKe Update
	if (kBuilding.getFreePromotion() != NO_PROMOTION)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_FREE_PROMOTION", GC.getPromotionInfo((PromotionTypes)(kBuilding.getFreePromotion())).getTextKeyWide()));
	}

	if (kBuilding.isCapital())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_CAPITAL"));
	}

	int iYieldStorage = kBuilding.getYieldStorage();
	if (iYieldStorage > 0)
	{
		if (ePlayer != NO_PLAYER)
		{
			iYieldStorage *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getStoragePercent();
			iYieldStorage /= 100;
		}
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_YIELD_STORAGE", iYieldStorage));
	}

	if (kBuilding.getOverflowSellPercent() > 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_YIELD_OVERFLOW_SELL_PERCENT", kBuilding.getOverflowSellPercent()));
	}

    ///TK COAL
//    if (kBuilding.getBuildingClassType()  == (BuildingClassTypes)GC.getDefineINT("STEAMWORKS_CLASS_TYPE"))
//	{
//		szBuffer.append(NEWLINE);
//		szBuffer.append(gDLL->getText("TXT_KEY_PRODUCTION_FROM_STEAMWORKS_TEXT", GC.getDefineINT("TK_STEAMWORKS_MODIFIER"), GC.getYieldInfo(YIELD_COAL).getChar()));
//	}

    ///Tks Med
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
    {
        YieldTypes eYield = (YieldTypes) iYield;
        if (kBuilding.getAutoSellsYields(eYield) > 0)
        {
            szBuffer.append(NEWLINE);
            szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_AUTOSELLS", GC.getYieldInfo(eYield).getChar(), kBuilding.getAutoSellsYields(eYield)));
        }

	}

	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
    {
        YieldTypes eYield = (YieldTypes) iYield;
        if (kBuilding.getMaxYieldModifiers(eYield) > 0)
        {
            szBuffer.append(NEWLINE);
            szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_INCREASE_STORAGE", GC.getYieldInfo(eYield).getChar(), kBuilding.getMaxYieldModifiers(eYield)));
        }

	}

	if (kBuilding.getCenterPlotBonus() != 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_CENTER_PLOT_BONUS", kBuilding.getCenterPlotBonus()));
	}


    if (kBuilding.isIncreasesCityPopulation() != 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_RESEARCH_INCREASE_POPULATION", kBuilding.isIncreasesCityPopulation()));
	}

	if (kBuilding.getTrainingTimeMod() != 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_TRAINING_TIME_MOD", kBuilding.getTrainingTimeMod()));
	}
    ///TKe

	if (kBuilding.isWorksWater())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_MISC_WATER_WORK"));
	}

	if (kBuilding.getFreeExperience() != 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_FREE_XP_UNITS", kBuilding.getFreeExperience()));
	}

	if (kBuilding.getFoodKept() > 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_STORES_FOOD", kBuilding.getFoodKept()));
	}

	if (kBuilding.getHealRateChange() != 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_HEAL_MOD", kBuilding.getHealRateChange()));
	}

	if (kBuilding.getMilitaryProductionModifier() != 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_MILITARY_MOD", kBuilding.getMilitaryProductionModifier()));
	}

	if (kBuilding.getDefenseModifier() != 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_DEFENSE_MOD", kBuilding.getDefenseModifier()));
	}

	if (kBuilding.getBombardDefenseModifier() != 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_BOMBARD_DEFENSE_MOD", -kBuilding.getBombardDefenseModifier()));
	}

	setYieldChangeHelp(szBuffer, gDLL->getText("TXT_KEY_BUILDING_WATER_PLOTS").c_str(), L": ", L"", kBuilding.getSeaPlotYieldChangeArray());

	setYieldChangeHelp(szBuffer, gDLL->getText("TXT_KEY_BUILDING_RIVER_PLOTS").c_str(), L": ", L"", kBuilding.getRiverPlotYieldChangeArray());

	iLast = 0;

	for (iI = 0; iI < GC.getNumUnitCombatInfos(); ++iI)
	{
		if (kBuilding.getUnitCombatFreeExperience(iI) != 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_FREE_XP", GC.getUnitCombatInfo((UnitCombatTypes)iI).getTextKeyWide(), kBuilding.getUnitCombatFreeExperience(iI)));
		}
	}

	for (iI = 0; iI < NUM_DOMAIN_TYPES; ++iI)
	{
		if (kBuilding.getDomainFreeExperience(iI) != 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_FREE_XP", GC.getDomainInfo((DomainTypes)iI).getTextKeyWide(), kBuilding.getDomainFreeExperience(iI)));
		}
	}

	for (iI = 0; iI < NUM_DOMAIN_TYPES; ++iI)
	{
		if (kBuilding.getDomainProductionModifier(iI) != 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_BUILDS_FASTER_DOMAIN", GC.getDomainInfo((DomainTypes)iI).getTextKeyWide(), kBuilding.getDomainProductionModifier(iI)));
		}
	}

	bFirst = true;

	for (iI = 0; iI < GC.getNumUnitInfos(); ++iI)
	{
		if (GC.getUnitInfo((UnitTypes)iI).getPrereqBuilding() == kBuilding.getBuildingClassType() || GC.getUnitInfo((UnitTypes)iI).isPrereqOrBuilding(kBuilding.getBuildingClassType()))
		{
			szFirstBuffer.Format(L"%s%s", NEWLINE, gDLL->getText("TXT_KEY_BUILDING_REQUIRED_TO_TRAIN").c_str());
			szTempBuffer.Format( SETCOLR L"<link=literal>%s</link>" ENDCOLR , TEXT_COLOR("COLOR_UNIT_TEXT"), GC.getUnitInfo((UnitTypes)iI).getDescription());
			setListHelp(szBuffer, szFirstBuffer, szTempBuffer, L", ", bFirst);
			bFirst = false;
		}
	}

	bFirst = true;

	for (iI = 0; iI < GC.getNumBuildingClassInfos(); ++iI)
	{
		if (ePlayer != NO_PLAYER)
		{
			eLoopBuilding = ((BuildingTypes)(GC.getCivilizationInfo(GET_PLAYER(ePlayer).getCivilizationType()).getCivilizationBuildings(iI)));
		}
		else
		{
			eLoopBuilding = (BuildingTypes)GC.getBuildingClassInfo((BuildingClassTypes)iI).getDefaultBuildingIndex();
		}

		if (eLoopBuilding != NO_BUILDING)
		{
			if (GC.getBuildingInfo(eLoopBuilding).isBuildingClassNeededInCity(kBuilding.getBuildingClassType()))
			{
				if ((pCity == NULL) || pCity->canConstruct(eLoopBuilding, false, true))
				{
					szFirstBuffer.Format(L"%s%s", NEWLINE, gDLL->getText("TXT_KEY_BUILDING_REQUIRED_TO_BUILD").c_str());
					szTempBuffer.Format(SETCOLR L"<link=literal>%s</link>" ENDCOLR, TEXT_COLOR("COLOR_BUILDING_TEXT"), GC.getBuildingInfo(eLoopBuilding).getDescription());
					setListHelp(szBuffer, szFirstBuffer, szTempBuffer, L", ", bFirst);
					bFirst = false;
				}
			}
		}
	}



	if (bCivilopediaText)
	{
	    ///TKs Invention Core Mod v 1.0
        int iAllows = 0;
        for (int iCivic = 0; iCivic < GC.getNumCivicInfos(); iCivic++)
        {
            iAllows = GC.getCivicInfo((CivicTypes)iCivic).getAllowsBuildingTypes(GC.getBuildingInfo(eBuilding).getBuildingClassType());
            if (iAllows > 0)
            {
                if ((CivicTypes)GC.getCivicInfo((CivicTypes)iCivic).getInventionCategory() == (CivicTypes)GC.getXMLval(XML_MEDIEVAL_TRADE_TECH))
                {
                    szBuffer.append(NEWLINE);
                    szBuffer.append(gDLL->getText("TXT_KEY_TRADE_PERK_MAKES_AVAILABLE", GC.getCivicInfo((CivicTypes)iCivic).getDescription()));
                    break;
                }
                else
                {
                    szBuffer.append(NEWLINE);
                    szBuffer.append(gDLL->getText("TXT_KEY_TECH_MAKES_AVAILABLE", GC.getCivicInfo((CivicTypes)iCivic).getDescription()));
                    break;
                }
            }
            else if (iAllows < 0)
            {
                szBuffer.append(NEWLINE);
                szBuffer.append(gDLL->getText("TXT_KEY_TECH_MAKES_OBSOLETE", GC.getCivicInfo((CivicTypes)iCivic).getDescription()));
                break;
            }
        }
        ///TKe
		// Trait
		for (int i = 0; i < GC.getNumTraitInfos(); ++i)
		{
			if (kBuilding.getProductionTraits((TraitTypes)i) != 0)
			{
				if (kBuilding.getProductionTraits((TraitTypes)i) == 100)
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_DOUBLE_SPEED_TRAIT", GC.getTraitInfo((TraitTypes)i).getTextKeyWide()));
				}
				else
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_PRODUCTION_MODIFIER_TRAIT", kBuilding.getProductionTraits((TraitTypes)i), GC.getTraitInfo((TraitTypes)i).getTextKeyWide()));
				}
			}
		}
	}

	if (bCivilopediaText)
	{
		if (kBuilding.getFreeStartEra() != NO_ERA)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_FREE_START_ERA", GC.getEraInfo((EraTypes)kBuilding.getFreeStartEra()).getTextKeyWide()));
		}
	}

	if (!isEmpty(kBuilding.getHelp()))
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(kBuilding.getHelp());
	}
	buildBuildingRequiresString(szBuffer, eBuilding, bCivilopediaText, pCity);

	if ((pCity == NULL) || !pCity->isHasRealBuilding(eBuilding))
	{
		if (!bCivilopediaText)
		{
			if (pCity == NULL)
			{
				for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
				{
					YieldTypes eYield = (YieldTypes) iYield;
					if (kBuilding.getYieldCost(eYield) > 0)
					{
						szTempBuffer.Format(L"\n%d%c", (ePlayer != NO_PLAYER ? GET_PLAYER(ePlayer).getYieldProductionNeeded(eBuilding, eYield) : kBuilding.getYieldCost(eYield)), GC.getYieldInfo(eYield).getChar());
						szBuffer.append(szTempBuffer);
					}
				}
			}
			else
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_NUM_TURNS", pCity->getProductionTurnsLeft(eBuilding, ((gDLL->ctrlKey() || !(gDLL->shiftKey())) ? 0 : pCity->getOrderQueueLength()))));

				iProduction = pCity->getBuildingProduction(eBuilding);

				int iProductionNeeded = pCity->getYieldProductionNeeded(eBuilding, YIELD_HAMMERS);
				if (iProduction > 0)
				{
					szTempBuffer.Format(L" - %d/%d%c", iProduction, iProductionNeeded, GC.getYieldInfo(YIELD_HAMMERS).getChar());
					szBuffer.append(szTempBuffer);
				}
				else
				{
					szTempBuffer.Format(L" - %d%c", iProductionNeeded, GC.getYieldInfo(YIELD_HAMMERS).getChar());
					szBuffer.append(szTempBuffer);
				}

				for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
				{
					if (GC.getBuildingInfo(eBuilding).getYieldCost(iYield) > 0)
					{
						YieldTypes eYield = (YieldTypes) iYield;
						if (GC.getYieldInfo(eYield).isCargo())
						{
							int iCost = GET_PLAYER(pCity->getOwnerINLINE()).getYieldProductionNeeded(eBuilding, eYield);
							if (iCost > 0)
							{
								szTempBuffer.Format(L" - %d/%d%c", pCity->getYieldStored(eYield) + pCity->getYieldRushed(eYield), iCost, GC.getYieldInfo(eYield).getChar());
								szBuffer.append(szTempBuffer);
							}
						}
					}
				}
			}
		}

		if ((gDLL->getChtLvl() > 0) && gDLL->ctrlKey() && (pCity != NULL))
		{
			int iBuildingValue = pCity->AI_buildingValue(eBuilding);
			szBuffer.append(CvWString::format(L"\nAI Building Value = %d", iBuildingValue));
		}
	}

	if (bStrategyText)
	{
		if (!isEmpty(kBuilding.getStrategy()))
		{
			if ((ePlayer == NO_PLAYER) || GET_PLAYER(ePlayer).isOption(PLAYEROPTION_ADVISOR_HELP))
			{
				szBuffer.append(SEPARATOR);
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_SIDS_TIPS"));
				szBuffer.append(L'\"');
				szBuffer.append(kBuilding.getStrategy());
				szBuffer.append(L'\"');
			}
		}
	}
}
void CvGameTextMgr::buildBuildingRequiresString(CvWStringBuffer& szBuffer, BuildingTypes eBuilding, bool bCivilopediaText, const CvCity* pCity)
{
	bool bFirst;
	PlayerTypes ePlayer;
	CvWString szTempBuffer;
	CvWString szFirstBuffer;
	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
	BuildingTypes eLoopBuilding;

	if (pCity != NULL)
	{
		ePlayer = pCity->getOwnerINLINE();
	}
	else
	{
		ePlayer = GC.getGameINLINE().getActivePlayer();
	}

	if (NULL == pCity || (!pCity->canConstruct(eBuilding) && !pCity->isHasConceptualBuilding(eBuilding)))
	{
		bFirst = true;

		if (!bFirst)
		{
			szBuffer.append(ENDCOLR);
		}
		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); ++iI)
		{
			if (ePlayer == NO_PLAYER && kBuilding.getPrereqNumOfBuildingClass((BuildingClassTypes)iI) > 0)
			{
				eLoopBuilding = (BuildingTypes)GC.getBuildingClassInfo((BuildingClassTypes)iI).getDefaultBuildingIndex();
				szTempBuffer.Format(L"%s%s", NEWLINE, gDLL->getText("TXT_KEY_BUILDING_REQUIRES_NUM_SPECIAL_BUILDINGS_NO_CITY", GC.getBuildingInfo(eLoopBuilding).getTextKeyWide(), kBuilding.getPrereqNumOfBuildingClass((BuildingClassTypes)iI)).c_str());

				szBuffer.append(szTempBuffer);
			}
			else if (ePlayer != NO_PLAYER && GET_PLAYER(ePlayer).getBuildingClassPrereqBuilding(eBuilding, ((BuildingClassTypes)iI)) > 0)
			{
				if ((pCity == NULL) || (GET_PLAYER(ePlayer).getBuildingClassCount((BuildingClassTypes)iI) < GET_PLAYER(ePlayer).getBuildingClassPrereqBuilding(eBuilding, ((BuildingClassTypes)iI))))
				{
					eLoopBuilding = ((BuildingTypes)(GC.getCivilizationInfo(GET_PLAYER(ePlayer).getCivilizationType()).getCivilizationBuildings(iI)));

					if (eLoopBuilding != NO_BUILDING)
					{
						if (pCity != NULL)
						{
							szTempBuffer.Format(L"%s%s", NEWLINE, gDLL->getText("TXT_KEY_BUILDING_REQUIRES_NUM_SPECIAL_BUILDINGS", GC.getBuildingInfo(eLoopBuilding).getTextKeyWide(), GET_PLAYER(ePlayer).getBuildingClassCount((BuildingClassTypes)iI), GET_PLAYER(ePlayer).getBuildingClassPrereqBuilding(eBuilding, ((BuildingClassTypes)iI))).c_str());
						}
						else
						{
							szTempBuffer.Format(L"%s%s", NEWLINE, gDLL->getText("TXT_KEY_BUILDING_REQUIRES_NUM_SPECIAL_BUILDINGS_NO_CITY", GC.getBuildingInfo(eLoopBuilding).getTextKeyWide(), GET_PLAYER(ePlayer).getBuildingClassPrereqBuilding(eBuilding, ((BuildingClassTypes)iI))).c_str());
						}

						szBuffer.append(szTempBuffer);
					}
				}
			}
			else if (kBuilding.isBuildingClassNeededInCity(iI))
			{
				if (NO_PLAYER != ePlayer)
				{
					eLoopBuilding = ((BuildingTypes)(GC.getCivilizationInfo(GET_PLAYER(ePlayer).getCivilizationType()).getCivilizationBuildings(iI)));
				}
				else
				{
					eLoopBuilding = (BuildingTypes)GC.getBuildingClassInfo((BuildingClassTypes)iI).getDefaultBuildingIndex();
				}

				if (eLoopBuilding != NO_BUILDING)
				{
					if ((pCity == NULL) || (!pCity->isHasConceptualBuilding(eLoopBuilding)))
					{
						szBuffer.append(NEWLINE);
						szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_REQUIRES_STRING", GC.getBuildingInfo(eLoopBuilding).getTextKeyWide()));
					}
				}
			}
		}

		if (kBuilding.getNumCitiesPrereq() > 0)
		{
			if (NO_PLAYER == ePlayer || GET_PLAYER(ePlayer).getNumCities() < kBuilding.getNumCitiesPrereq())
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_REQUIRES_NUM_CITIES", kBuilding.getNumCitiesPrereq()));
			}
		}

		if (kBuilding.getUnitLevelPrereq() > 0)
		{
			if (NO_PLAYER == ePlayer || GET_PLAYER(ePlayer).getHighestUnitLevel() < kBuilding.getUnitLevelPrereq())
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_REQUIRES_UNIT_LEVEL", kBuilding.getUnitLevelPrereq()));
			}
		}

		if (kBuilding.getMinLatitude() > 0)
		{
			if (NULL == pCity || pCity->plot()->getLatitude() < kBuilding.getMinLatitude())
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_MIN_LATITUDE", kBuilding.getMinLatitude()));
			}
		}

		if (kBuilding.getMaxLatitude() < 90)
		{
			if (NULL == pCity || pCity->plot()->getLatitude() > kBuilding.getMaxLatitude())
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_MAX_LATITUDE", kBuilding.getMaxLatitude()));
			}
		}

		if (!bCivilopediaText)
		{
			for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
			{
				if (kBuilding.getYieldCost(iYield) > 0)
				{
					YieldTypes eYield = (YieldTypes) iYield;
					if (GC.getYieldInfo(eYield).isCargo())
					{
						int iCost = (NO_PLAYER == ePlayer ? GC.getBuildingInfo(eBuilding).getYieldCost(iYield) : GET_PLAYER(ePlayer).getYieldProductionNeeded(eBuilding, eYield));
						if (NULL == pCity || pCity->getYieldStored(eYield) + pCity->getYieldRushed(eYield) < iCost)
						{
							szBuffer.append(NEWLINE);
							szBuffer.append(gDLL->getText("TXT_KEY_BUILD_CANNOT_AFFORD", iCost, GC.getYieldInfo(eYield).getChar()));
						}
					}
				}
			}
		}

		if (bCivilopediaText)
		{
			if (kBuilding.getVictoryPrereq() != NO_VICTORY)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_REQUIRES_VICTORY", GC.getVictoryInfo((VictoryTypes)(kBuilding.getVictoryPrereq())).getTextKeyWide()));
			}

			if (kBuilding.getMaxStartEra() != NO_ERA)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_MAX_START_ERA", GC.getEraInfo((EraTypes)kBuilding.getMaxStartEra()).getTextKeyWide()));
			}

			if (kBuilding.getNumTeamsPrereq() > 0)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_REQUIRES_NUM_TEAMS", kBuilding.getNumTeamsPrereq()));
			}
		}

		if (pCity != NULL)
		{
			if (pCity->getFirstBuildingOrder(eBuilding) != -1)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_IN_QUEUE", kBuilding.getNumTeamsPrereq()));
			}
		}
	}
}

void CvGameTextMgr::setFatherPointHelp(CvWStringBuffer &szBuffer, FatherPointTypes eFatherPointType)
{
	CvFatherPointInfo& kFatherPoint = GC.getFatherPointInfo(eFatherPointType);

	szBuffer.append(CvWString::format( SETCOLR L"%s" ENDCOLR , TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), kFatherPoint.getDescription()));

	if (kFatherPoint.getProductionConversionPoints() != 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_FATHER_POINT_PRODUCTION", kFatherPoint.getProductionConversionPoints(), GC.getYieldInfo(YIELD_HAMMERS).getChar()));
	}

	std::vector<FatherTypes> aFathers;
	if (GC.getGameINLINE().getRemainingFathers(eFatherPointType, aFathers))
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_FATHER_POINT_FATHERS_ALL"));
	}
	else if (!aFathers.empty())
	{
		CvWStringBuffer szTemp;
		for (uint i = 0; i < aFathers.size(); ++i)
		{
			if (i > 0)
			{
				szTemp.append(L", ");
			}
			szTemp.append(GC.getFatherInfo(aFathers[i]).getDescription());
		}
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_FATHER_POINT_FATHERS", szTemp.getCString()));
	}
}

void CvGameTextMgr::setYieldChangeHelp(CvWStringBuffer &szBuffer, const CvWString& szStart, const CvWString& szSpace, const CvWString& szEnd, const int* piYieldChange, bool bPercent, bool bNewLine)
{
	bool bAllTheSame = true;
	int iPrevChange = 0;
	for (int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		if (piYieldChange[iI] != 0)
		{
			if (iPrevChange != 0 && piYieldChange[iI] != iPrevChange)
			{
				bAllTheSame = false;
				break;
			}

			iPrevChange = piYieldChange[iI];
		}
	}

	bool bStarted = false;
	for (int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		if (piYieldChange[iI] != 0)
		{
			CvWString szTempBuffer;

			if (!bStarted)
			{
				if (bNewLine)
				{
					szTempBuffer.Format(L"\n%c", gDLL->getSymbolID(BULLET_CHAR));
				}
				szTempBuffer += CvWString::format(L"%s%s%s%d%s%c",
					szStart.GetCString(),
					szSpace.GetCString(),
					piYieldChange[iI] > 0 ? L"+" : L"",
					piYieldChange[iI],
					bPercent ? L"%" : L"",
					GC.getYieldInfo((YieldTypes)iI).getChar());
			}
			else
			{
				if (bAllTheSame)
				{
					szTempBuffer.Format(L",%c", GC.getYieldInfo((YieldTypes)iI).getChar());
				}
				else
				{
					szTempBuffer.Format(L", %s%d%s%c",
						piYieldChange[iI] > 0 ? L"+" : L"",
						piYieldChange[iI],
						bPercent ? L"%" : L"",
						GC.getYieldInfo((YieldTypes)iI).getChar());
				}
			}
			szBuffer.append(szTempBuffer);
			bStarted = true;
		}
	}
	if (bStarted)
	{
		szBuffer.append(szEnd);
	}
}
void CvGameTextMgr::setBonusHelp(CvWStringBuffer &szBuffer, BonusTypes eBonus, bool bCivilopediaText)
{
	if (NO_BONUS == eBonus)
	{
		return;
	}
	if (!bCivilopediaText)
	{
		szBuffer.append(CvWString::format( SETCOLR L"%s" ENDCOLR , TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), GC.getBonusInfo(eBonus).getDescription()));
		setYieldChangeHelp(szBuffer, L"", L"", L"", GC.getBonusInfo(eBonus).getYieldChangeArray());
	}
	ImprovementTypes eImprovement = NO_IMPROVEMENT;
	for (int iLoopImprovement = 0; iLoopImprovement < GC.getNumImprovementInfos(); iLoopImprovement++)
	{
		if (GC.getImprovementInfo((ImprovementTypes)iLoopImprovement).isImprovementBonusMakesValid(eBonus))
		{
			eImprovement = (ImprovementTypes)iLoopImprovement;
			break;
		}
	}
	CivilizationTypes eCivilization = GC.getGameINLINE().getActiveCivilizationType();
	for (int i = 0; i < GC.getNumBuildingClassInfos(); i++)
	{
		BuildingTypes eLoopBuilding;
		if (eCivilization == NO_CIVILIZATION)
		{
			eLoopBuilding = ((BuildingTypes)(GC.getBuildingClassInfo((BuildingClassTypes)i).getDefaultBuildingIndex()));
		}
		else
		{
			eLoopBuilding = ((BuildingTypes)(GC.getCivilizationInfo(eCivilization).getCivilizationBuildings(i)));
		}
	}
	if (!isEmpty(GC.getBonusInfo(eBonus).getHelp()))
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(GC.getBonusInfo(eBonus).getHelp());
	}
	///TKs Invention Core Mod v 1.0
    if (bCivilopediaText)
    {
        int iAllows = 0;
        for (int iCivic = 0; iCivic < GC.getNumCivicInfos(); iCivic++)
        {
            iAllows = GC.getCivicInfo((CivicTypes)iCivic).getAllowsBonuses(eBonus);
            if (iAllows > 0)
            {
                if ((CivicTypes)GC.getCivicInfo((CivicTypes)iCivic).getInventionCategory() == (CivicTypes)GC.getXMLval(XML_MEDIEVAL_TRADE_TECH))
                {
                    szBuffer.append(NEWLINE);
                    szBuffer.append(gDLL->getText("TXT_KEY_TRADE_PERK_MAKES_AVAILABLE", GC.getCivicInfo((CivicTypes)iCivic).getDescription()));
                    break;
                }
                else
                {
                    szBuffer.append(NEWLINE);
                    szBuffer.append(gDLL->getText("TXT_KEY_TECH_MAKES_AVAILABLE", GC.getCivicInfo((CivicTypes)iCivic).getDescription()));
                    break;
                }
            }
            else if (iAllows < 0)
            {
                szBuffer.append(NEWLINE);
                szBuffer.append(gDLL->getText("TXT_KEY_TECH_MAKES_OBSOLETE", GC.getCivicInfo((CivicTypes)iCivic).getDescription()));
                break;
            }
        }
    }
	///TKe


}

void CvGameTextMgr::setPromotionHelp(CvWStringBuffer &szBuffer, PromotionTypes ePromotion, bool bCivilopediaText)
{
	if (!bCivilopediaText)
	{
		CvWString szTempBuffer;

		if (NO_PROMOTION == ePromotion)
		{
			return;
		}
		CvPromotionInfo& promo = GC.getPromotionInfo(ePromotion);

		szTempBuffer.Format( SETCOLR L"%s" ENDCOLR , TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), promo.getDescription());
		szBuffer.append(szTempBuffer);
	}
    ///TKs Invention Core Mod v 1.0
	parsePromotionHelp(szBuffer, ePromotion, NEWLINE, bCivilopediaText);
	///TKe
}

void CvGameTextMgr::setImprovementHelp(CvWStringBuffer &szBuffer, ImprovementTypes eImprovement, bool bCivilopediaText)
{
	CvWString szTempBuffer;
	CvWString szFirstBuffer;
	int iTurns;

	if (NO_IMPROVEMENT == eImprovement)
	{
		return;
	}

	CvImprovementInfo& info = GC.getImprovementInfo(eImprovement);
	if (!bCivilopediaText)
	{
		szTempBuffer.Format( SETCOLR L"%s" ENDCOLR, TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), info.getDescription());
		szBuffer.append(szTempBuffer);

		setYieldChangeHelp(szBuffer, L", ", L"", L"", info.getYieldIncreaseArray(), false, false);

		setYieldChangeHelp(szBuffer, L"", L"", gDLL->getText("TXT_KEY_MISC_ON_HILLS").c_str(), info.getHillsYieldChangeArray());
		setYieldChangeHelp(szBuffer, L"", L"", gDLL->getText("TXT_KEY_MISC_ALONG_RIVER").c_str(), info.getRiverSideYieldChangeArray());
		//	Civics
		for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
		{
			for (int iCivic = 0; iCivic < GC.getNumCivicInfos(); iCivic++)
			{
				int iChange = GC.getCivicInfo((CivicTypes)iCivic).getImprovementYieldChanges(eImprovement, iYield);
				if (0 != iChange)
				{
					szTempBuffer.Format( SETCOLR L"%s" ENDCOLR , TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), GC.getCivicInfo((CivicTypes)iCivic).getDescription());
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_CIVIC_IMPROVEMENT_YIELD_CHANGE", iChange, GC.getYieldInfo((YieldTypes)iYield).getChar()));
					szBuffer.append(szTempBuffer);
				}
			}
		}
	}

	if (info.isRequiresRiverSide())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_IMPROVEMENT_REQUIRES_RIVER"));
	}
	if (bCivilopediaText)
	{
		if (info.isWater())
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_IMPROVEMENT_BUILD_ONLY_WATER"));
		}
		if (info.isRequiresFlatlands())
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_IMPROVEMENT_ONLY_BUILD_FLATLANDS"));
		}
	}

	if (info.getImprovementUpgrade() != NO_IMPROVEMENT)
	{
		iTurns = GC.getGameINLINE().getImprovementUpgradeTime(eImprovement);

		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_IMPROVEMENT_EVOLVES", GC.getImprovementInfo((ImprovementTypes) info.getImprovementUpgrade()).getTextKeyWide(), iTurns));
	}

	int iLast = -1;
	for (int iBonus = 0; iBonus < GC.getNumBonusInfos(); iBonus++)
	{
		int iRand = info.getImprovementBonusDiscoverRand(iBonus);
		if (iRand > 0)
		{
			szFirstBuffer.Format(L"%s%s", NEWLINE, gDLL->getText("TXT_KEY_IMPROVEMENT_CHANCE_DISCOVER").c_str());
			szTempBuffer.Format(L"%c", GC.getBonusInfo((BonusTypes) iBonus).getChar());
			setListHelp(szBuffer, szFirstBuffer, szTempBuffer, L", ", iRand != iLast);
			iLast = iRand;
		}
	}

	if (0 != info.getDefenseModifier())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_IMPROVEMENT_DEFENSE_MODIFIER", info.getDefenseModifier()));
	}

	if (info.isActsAsCity())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_IMPROVEMENT_DEFENSE_MODIFIER_EXTRA"));
		///TKs Med
		if (info.getPatrolLevel() > 1)
		{
            szBuffer.append(NEWLINE);
            szBuffer.append(gDLL->getText("TXT_KEY_IMPROVEMENT_WATCH_TOWER_HELP", GC.getXMLval(XML_MARAUDERS_TOWER_RANGE)));
		}
		else
		{
            szBuffer.append(NEWLINE);
            szBuffer.append(gDLL->getText("TXT_KEY_IMPROVEMENT_LODGE_HUNT_HELP", GC.getXMLval(XML_MARAUDERS_TOWER_RANGE)));
		}
		///TKe
	}

	if (info.getFeatureGrowthProbability() > 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_IMPROVEMENT_MORE_GROWTH"));
	}
	else if (info.getFeatureGrowthProbability() < 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_IMPROVEMENT_LESS_GROWTH"));
	}

	if (bCivilopediaText)
	{
		if (info.getPillageGold() > 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_IMPROVEMENT_PILLAGE_YIELDS", info.getPillageGold()));
		}

		///TKs Invention Core Mod v 1.0
        int iAllows = 0;
        for (int iCivic = 0; iCivic < GC.getNumCivicInfos(); iCivic++)
        {
            iAllows = GC.getCivicInfo((CivicTypes)iCivic).getAllowsBuildTypes(eImprovement);
            if (iAllows > 0)
            {
                if ((CivicTypes)GC.getCivicInfo((CivicTypes)iCivic).getInventionCategory() == (CivicTypes)GC.getXMLval(XML_MEDIEVAL_TRADE_TECH))
                {
                    szBuffer.append(NEWLINE);
                    szBuffer.append(gDLL->getText("TXT_KEY_TRADE_PERK_MAKES_AVAILABLE", GC.getCivicInfo((CivicTypes)iCivic).getDescription()));
                    break;
                }
                else
                {
                    szBuffer.append(NEWLINE);
                    szBuffer.append(gDLL->getText("TXT_KEY_TECH_MAKES_AVAILABLE", GC.getCivicInfo((CivicTypes)iCivic).getDescription()));
                    break;
                }
            }
            else if (iAllows < 0)
            {
                szBuffer.append(NEWLINE);
                szBuffer.append(gDLL->getText("TXT_KEY_TECH_MAKES_OBSOLETE", GC.getCivicInfo((CivicTypes)iCivic).getDescription()));
                break;
            }
        }
        ///TKe

	}


}


void CvGameTextMgr::getDealString(CvWStringBuffer& szBuffer, CvDeal& deal, PlayerTypes ePlayerPerspective)
{
	PlayerTypes ePlayer1 = deal.getFirstPlayer();
	PlayerTypes ePlayer2 = deal.getSecondPlayer();

	const CLinkList<TradeData>* pListPlayer1 = deal.getFirstTrades();
	const CLinkList<TradeData>* pListPlayer2 = deal.getSecondTrades();

	getDealString(szBuffer, ePlayer1, ePlayer2, pListPlayer1,  pListPlayer2, ePlayerPerspective);
}

void CvGameTextMgr::getDealString(CvWStringBuffer& szBuffer, PlayerTypes ePlayer1, PlayerTypes ePlayer2, const CLinkList<TradeData>* pListPlayer1, const CLinkList<TradeData>* pListPlayer2, PlayerTypes ePlayerPerspective)
{
	if (NO_PLAYER == ePlayer1 || NO_PLAYER == ePlayer2)
	{
		FAssertMsg(false, "Deal needs two parties");
		return;
	}

	CvWStringBuffer szDealOne;
	if (NULL != pListPlayer1 && pListPlayer1->getLength() > 0)
	{
		CLLNode<TradeData>* pTradeNode;
		bool bFirst = true;
		for (pTradeNode = pListPlayer1->head(); pTradeNode; pTradeNode = pListPlayer1->next(pTradeNode))
		{
			CvWStringBuffer szTrade;
			getTradeString(szTrade, pTradeNode->m_data, ePlayer1, ePlayer2);
			setListHelp(szDealOne, L"", szTrade.getCString(), L", ", bFirst);
			bFirst = false;
		}
	}

	CvWStringBuffer szDealTwo;
	if (NULL != pListPlayer2 && pListPlayer2->getLength() > 0)
	{
		CLLNode<TradeData>* pTradeNode;
		bool bFirst = true;
		for (pTradeNode = pListPlayer2->head(); pTradeNode; pTradeNode = pListPlayer2->next(pTradeNode))
		{
			CvWStringBuffer szTrade;
			getTradeString(szTrade, pTradeNode->m_data, ePlayer2, ePlayer1);
			setListHelp(szDealTwo, L"", szTrade.getCString(), L", ", bFirst);
			bFirst = false;
		}
	}

	if (!szDealOne.isEmpty())
	{
		if (!szDealTwo.isEmpty())
		{
			if (ePlayerPerspective == ePlayer1)
			{
				szBuffer.append(gDLL->getText("TXT_KEY_MISC_OUR_DEAL", szDealOne.getCString(), GET_PLAYER(ePlayer2).getNameKey(), szDealTwo.getCString()));
			}
			else if (ePlayerPerspective == ePlayer2)
			{
				szBuffer.append(gDLL->getText("TXT_KEY_MISC_OUR_DEAL", szDealTwo.getCString(), GET_PLAYER(ePlayer1).getNameKey(), szDealOne.getCString()));
			}
			else
			{
				szBuffer.append(gDLL->getText("TXT_KEY_MISC_DEAL", GET_PLAYER(ePlayer1).getNameKey(), szDealOne.getCString(), GET_PLAYER(ePlayer2).getNameKey(), szDealTwo.getCString()));
			}
		}
		else
		{
			if (ePlayerPerspective == ePlayer1)
			{
				szBuffer.append(gDLL->getText("TXT_KEY_MISC_DEAL_ONESIDED_OURS", szDealOne.getCString(), GET_PLAYER(ePlayer2).getNameKey()));
			}
			else if (ePlayerPerspective == ePlayer2)
			{
				szBuffer.append(gDLL->getText("TXT_KEY_MISC_DEAL_ONESIDED_THEIRS", szDealOne.getCString(), GET_PLAYER(ePlayer1).getNameKey()));
			}
			else
			{
				szBuffer.append(gDLL->getText("TXT_KEY_MISC_DEAL_ONESIDED", GET_PLAYER(ePlayer1).getNameKey(), szDealOne.getCString(), GET_PLAYER(ePlayer2).getNameKey()));
			}
		}
	}
	else if (!szDealTwo.isEmpty())
	{
		if (ePlayerPerspective == ePlayer1)
		{
			szBuffer.append(gDLL->getText("TXT_KEY_MISC_DEAL_ONESIDED_THEIRS", szDealTwo.getCString(), GET_PLAYER(ePlayer2).getNameKey()));
		}
		else if (ePlayerPerspective == ePlayer2)
		{
			szBuffer.append(gDLL->getText("TXT_KEY_MISC_DEAL_ONESIDED_OURS", szDealTwo.getCString(), GET_PLAYER(ePlayer1).getNameKey()));
		}
		else
		{
			szBuffer.append(gDLL->getText("TXT_KEY_MISC_DEAL_ONESIDED", GET_PLAYER(ePlayer2).getNameKey(), szDealTwo.getCString(), GET_PLAYER(ePlayer1).getNameKey()));
		}
	}
}

void CvGameTextMgr::getWarplanString(CvWStringBuffer& szString, WarPlanTypes eWarPlan)
{
	switch (eWarPlan)
	{
		case WARPLAN_ATTACKED_RECENT: szString.assign(L"new defensive war"); break;
		case WARPLAN_ATTACKED: szString.assign(L"defensive war"); break;
		case WARPLAN_PREPARING_LIMITED: szString.assign(L"preparing limited war"); break;
		case WARPLAN_PREPARING_TOTAL: szString.assign(L"preparing total war"); break;
		case WARPLAN_LIMITED: szString.assign(L"limited war"); break;
		case WARPLAN_TOTAL: szString.assign(L"total war"); break;
		case WARPLAN_DOGPILE: szString.assign(L"dogpile war"); break;
		case WARPLAN_EXTORTION: szString.assign(L"extortion war"); break;
		case NO_WARPLAN: szString.assign(L"unplanned war"); break;
		default:  szString.assign(L"unknown war"); break;
	}
}
///Tks Med
void CvGameTextMgr::getAttitudeString(CvWStringBuffer& szBuffer, PlayerTypes ePlayer, PlayerTypes eTargetPlayer)
{
	CvWString szTempBuffer;
	int iAttitudeChange;
	int iTotalChange = 0;
	int iPass;
	int iI;
	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	TeamTypes eTeam = (TeamTypes) kPlayer.getTeam();
	CvTeamAI& kTeam = GET_TEAM(eTeam);
    ///TKs Med
    if (kPlayer.getVassalOwner() == eTargetPlayer)
    {
        szBuffer.append(gDLL->getText("TXT_KEY_VASSAL_ATTITUDE_TOWARDS", GET_PLAYER(eTargetPlayer).getNameKey()));
    }
    else
    {
        szBuffer.append(gDLL->getText("TXT_KEY_ATTITUDE_TOWARDS", GC.getAttitudeInfo(GET_PLAYER(ePlayer).AI_getAttitude(eTargetPlayer)).getTextKeyWide(), GET_PLAYER(eTargetPlayer).getNameKey()));
    }
    ///Tke
	for (iPass = 0; iPass < 2; iPass++)
	{
		iAttitudeChange = kPlayer.AI_getCloseBordersAttitude(eTargetPlayer);
		iTotalChange += iAttitudeChange;
		if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
		{
			szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText("TXT_KEY_MISC_ATTITUDE_LAND_TARGET", iAttitudeChange).GetCString());
			szBuffer.append(NEWLINE);
			szBuffer.append(szTempBuffer);
		}

		iAttitudeChange = kPlayer.AI_getStolenPlotsAttitude(eTargetPlayer);
		iTotalChange += iAttitudeChange;
		if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
		{
			szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText("TXT_KEY_MISC_ATTITUDE_STOLEN_LAND", iAttitudeChange).GetCString());
			szBuffer.append(NEWLINE);
			szBuffer.append(szTempBuffer);
		}

		iAttitudeChange = kPlayer.AI_getAlarmAttitude(eTargetPlayer);
		iTotalChange += iAttitudeChange;
		if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
		{
			szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText("TXT_KEY_MISC_ATTITUDE_ALARM", iAttitudeChange).GetCString());
			szBuffer.append(NEWLINE);
			szBuffer.append(szTempBuffer);
		}

		iAttitudeChange = kPlayer.AI_getRebelAttitude(eTargetPlayer);
		iTotalChange += iAttitudeChange;
		if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
		{
			szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText("TXT_KEY_MISC_ATTITUDE_REBEL", iAttitudeChange).GetCString());
			szBuffer.append(NEWLINE);
			szBuffer.append(szTempBuffer);
		}

		iAttitudeChange = kPlayer.AI_getWarAttitude(eTargetPlayer);
		iTotalChange += iAttitudeChange;
		if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
		{
			szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText("TXT_KEY_MISC_ATTITUDE_WAR", iAttitudeChange).GetCString());
			szBuffer.append(NEWLINE);
			szBuffer.append(szTempBuffer);
		}

		iAttitudeChange = kPlayer.AI_getPeaceAttitude(eTargetPlayer);
		iTotalChange += iAttitudeChange;
		if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
		{
			szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText("TXT_KEY_MISC_ATTITUDE_PEACE", iAttitudeChange).GetCString());
			szBuffer.append(NEWLINE);
			szBuffer.append(szTempBuffer);
		}

		iAttitudeChange = kPlayer.AI_getOpenBordersAttitude(eTargetPlayer);
		iTotalChange += iAttitudeChange;
		if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
		{
			szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText("TXT_KEY_MISC_ATTITUDE_OPEN_BORDERS", iAttitudeChange).GetCString());
			szBuffer.append(NEWLINE);
			szBuffer.append(szTempBuffer);
		}

		iAttitudeChange = kPlayer.AI_getDefensivePactAttitude(eTargetPlayer);
		iTotalChange += iAttitudeChange;
		if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
		{
			szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText("TXT_KEY_MISC_ATTITUDE_DEFENSIVE_PACT", iAttitudeChange).GetCString());
			szBuffer.append(NEWLINE);
			szBuffer.append(szTempBuffer);
		}

		iAttitudeChange = kPlayer.AI_getRivalDefensivePactAttitude(eTargetPlayer);
		iTotalChange += iAttitudeChange;
		if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
		{
			szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText("TXT_KEY_MISC_ATTITUDE_RIVAL_DEFENSIVE_PACT", iAttitudeChange).GetCString());
			szBuffer.append(NEWLINE);
			szBuffer.append(szTempBuffer);
		}

		iAttitudeChange = kPlayer.AI_getShareWarAttitude(eTargetPlayer);
		iTotalChange += iAttitudeChange;
		if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
		{
			szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText("TXT_KEY_MISC_ATTITUDE_SHARE_WAR", iAttitudeChange).GetCString());
			szBuffer.append(NEWLINE);
			szBuffer.append(szTempBuffer);
		}

		iAttitudeChange = kPlayer.AI_getTradeAttitude(eTargetPlayer);
		iTotalChange += iAttitudeChange;
		if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
		{
			szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText("TXT_KEY_MISC_ATTITUDE_TRADE", iAttitudeChange).GetCString());
			szBuffer.append(NEWLINE);
			szBuffer.append(szTempBuffer);
		}

		iAttitudeChange = kPlayer.AI_getRivalTradeAttitude(eTargetPlayer);
		iTotalChange += iAttitudeChange;
		if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
		{
			szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText("TXT_KEY_MISC_ATTITUDE_RIVAL_TRADE", iAttitudeChange).GetCString());
			szBuffer.append(NEWLINE);
			szBuffer.append(szTempBuffer);
		}

		///TKs Med
//		iAttitudeChange = kPlayer.AI_getInsultedAttitude(eTargetPlayer);
//		if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
//		{
//			szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText("TXT_KEY_MISC_ATTITUDE_INSULT", iAttitudeChange).GetCString());
//			szBuffer.append(NEWLINE);
//			szBuffer.append(szTempBuffer);
//		}
		iAttitudeChange = GET_PLAYER(eTargetPlayer).getDiplomacyAttitudeModifier(ePlayer);
		iTotalChange += iAttitudeChange;
		if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
		{
			szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText(((iAttitudeChange > 0) ? "TXT_KEY_CIVIC_ATTITUDES_EFFECT_GOOD" : "TXT_KEY_CIVIC_ATTITUDES_EFFECT_BAD"), iAttitudeChange).GetCString());
			szBuffer.append(NEWLINE);
			szBuffer.append(szTempBuffer);
		}

		int iAttitudeChange = 0;
		CvWString szCivicBuffer;
		for (int iX=0; iX < GC.getLeaderHeadInfo(GET_PLAYER(ePlayer).getPersonalityType()).getNumCivicDiplomacyAttitudes(); iX++)
		{
			CivicTypes eCivic = (CivicTypes)GC.getLeaderHeadInfo(GET_PLAYER(ePlayer).getPersonalityType()).getCivicDiplomacyAttitudes(iX);
			if (GET_PLAYER(eTargetPlayer).isCivic(eCivic))
			{
				iAttitudeChange += GC.getLeaderHeadInfo(GET_PLAYER(ePlayer).getPersonalityType()).getCivicDiplomacyAttitudesValue(iX);
				//iAttitudeChange /= std::max(1, GC.getLeaderHeadInfo(GET_PLAYER(ePlayer).getPersonalityType()).getCivicDiplomacyDivisor());
				/*int iChangelimit = GC.getLeaderHeadInfo(GET_PLAYER(ePlayer).getPersonalityType()).getCivicDiplomacyChangeLimit();
				if (iChangelimit > 0)
				{
					iAttitudeChange = range(iAttitudeChange, -(abs(iChangelimit)), abs(iChangelimit));
				}*/
				iTotalChange += iAttitudeChange;
			}
			
			if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
			{
				szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText(((iAttitudeChange > 0) ? "TXT_KEY_CIVIC_ATTITUDES_GOOD" : "TXT_KEY_CIVIC_ATTITUDES_BAD"), iAttitudeChange).GetCString());
				szCivicBuffer.Format(SETCOLR L"(%s)" ENDCOLR, TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), GC.getCivicInfo(eCivic).getDescription());
				szBuffer.append(NEWLINE);
				szBuffer.append(szTempBuffer);
				szBuffer.append(szCivicBuffer);
			}
		}
		//tkend

		iAttitudeChange = GET_PLAYER(ePlayer).AI_getAttitudeExtra(eTargetPlayer);
		iTotalChange += iAttitudeChange;
		if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
		{
			szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText(((iAttitudeChange > 0) ? "TXT_KEY_MISC_ATTITUDE_EXTRA_GOOD" : "TXT_KEY_MISC_ATTITUDE_EXTRA_BAD"), iAttitudeChange).GetCString());
			szBuffer.append(NEWLINE);
			szBuffer.append(szTempBuffer);
		}

		for (iI = 0; iI < NUM_MEMORY_TYPES; ++iI)
		{
		    ///Tks Med
			if ((MemoryTypes)iI == MEMORY_MADE_VASSAL_DEMAND)
			{
			    if (kPlayer.AI_getMemoryCount(eTargetPlayer, MEMORY_MADE_VASSAL_DEMAND) != 0)
                {
                    int iDecay = kPlayer.AI_getMemoryCount(eTargetPlayer, MEMORY_MADE_VASSAL_DEMAND);
                    szTempBuffer.Format(SETCOLR L"%s Turns Left= %d" ENDCOLR, TEXT_COLOR("COLOR_NEGATIVE_TEXT"), GC.getMemoryInfo((MemoryTypes)iI).getDescription(), iDecay);
                    szBuffer.append(NEWLINE);
                    szBuffer.append(szTempBuffer);
                }
                continue;
			}
			///Tke
			iAttitudeChange = kPlayer.AI_getMemoryAttitude(eTargetPlayer, ((MemoryTypes)iI));
			iTotalChange += iAttitudeChange;
			if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
			{
				szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText("TXT_KEY_MISC_ATTITUDE_MEMORY", iAttitudeChange, GC.getMemoryInfo((MemoryTypes)iI).getDescription()).GetCString());
				szBuffer.append(NEWLINE);
				szBuffer.append(szTempBuffer);
			}
		}
		//int iAttitudeVal = kPlayer.AI_getAttitudeVal(eTargetPlayer, false);
		//FAssert(iTotalChange == iAttitudeVal);

	}
}
//TKe
void CvGameTextMgr::getTradeString(CvWStringBuffer& szBuffer, const TradeData& tradeData, PlayerTypes ePlayer1, PlayerTypes ePlayer2)
{
	switch (tradeData.m_eItemType)
	{
	case TRADE_GOLD:
		szBuffer.append(gDLL->getText("TXT_KEY_MISC_GOLD", tradeData.m_iData1));
		break;
	case TRADE_YIELD:
		szBuffer.assign(CvWString::format(L"%s", GC.getYieldInfo((YieldTypes)tradeData.m_iData1).getDescription()));
		break;
	case TRADE_MAPS:
		szBuffer.append(gDLL->getText("TXT_KEY_MISC_WORLD_MAP"));
		break;
	case TRADE_OPEN_BORDERS:
		szBuffer.append(gDLL->getText("TXT_KEY_MISC_OPEN_BORDERS"));
		break;
	case TRADE_DEFENSIVE_PACT:
		szBuffer.append(gDLL->getText("TXT_KEY_MISC_DEFENSIVE_PACT"));
		break;
	case TRADE_PERMANENT_ALLIANCE:
	    ///TKs Med
//	    if (ePlayer1 != NO_PLAYER && GET_PLAYER(ePlayer1).isHuman())
//        {
//            szBuffer.append(gDLL->getText("TXT_KEY_MISC_PERMANENT_VASSAL_LORD"));
//        }
        if (GET_PLAYER(ePlayer1).isHuman())
        {
            szBuffer.append(gDLL->getText("TXT_KEY_MISC_PERMANENT_VASSAL_LORD"));
        }
        else
        {
            szBuffer.append(gDLL->getText("TXT_KEY_MISC_PERMANENT_ALLIANCE"));
        }
	    ///Tke

		break;
	case TRADE_PEACE_TREATY:
		szBuffer.append(gDLL->getText("TXT_KEY_MISC_PEACE_TREATY", GC.getXMLval(XML_PEACE_TREATY_LENGTH)));
		break;
    ///TKs Invention Core Mod v 1.0
    case TRADE_RESEARCH:
        szBuffer.append(gDLL->getText("TXT_KEY_TRADE_CURRENT_RESEARCH"));
//        if (GET_PLAYER(ePlayer1).getCurrentResearch() != NO_CIVIC)
//        {
//            CvCivicInfo& kCivicInfo = GC.getCivicInfo(GET_PLAYER(ePlayer1).getCurrentResearch());
////            szBuffer.assign(CvWString::format(L"%s", kCivicInfo.getDescription().GetCString()));
//            szBuffer.append(NEWLINE);
//            szBuffer.append(gDLL->getText("TXT_KEY_TECHONOLOGY_TRADE", kCivicInfo.getDescription()));
//        }
//        else
//        {
//            szBuffer.append(NEWLINE);
//            szBuffer.assign(CvWString::format(L"NO Civic"));
//        }
		break;
    case TRADE_IDEAS:
        if ((CivicTypes)tradeData.m_iData1 != NO_CIVIC)
        {
            CvCivicInfo& kCivicInfo = GC.getCivicInfo((CivicTypes)tradeData.m_iData1);
//            szBuffer.assign(CvWString::format(L"%s", kCivicInfo.getDescription().GetCString()));
            szBuffer.append(gDLL->getText("TXT_KEY_TECHONOLOGY_TRADE", kCivicInfo.getDescription()));
        }
        else
        {
            szBuffer.assign(CvWString::format(L"NO Civic"));
        }
		break;
    ///TKe
	case TRADE_CITIES:
		szBuffer.assign(CvWString::format(L"%s", GET_PLAYER(ePlayer1).getCity(tradeData.m_iData1)->getName().GetCString()));
		break;
	case TRADE_PEACE:
	case TRADE_WAR:
	case TRADE_EMBARGO:
		szBuffer.assign(CvWString::format(L"%s", GET_TEAM((TeamTypes)tradeData.m_iData1).getName().GetCString()));
		break;
	default:
		FAssert(false);
		break;
	}
}

void CvGameTextMgr::setFeatureHelp(CvWStringBuffer &szBuffer, FeatureTypes eFeature, bool bCivilopediaText)
{
	if (NO_FEATURE == eFeature)
	{
		return;
	}
	CvFeatureInfo& feature = GC.getFeatureInfo(eFeature);

	int aiYields[NUM_YIELD_TYPES];
	if (!bCivilopediaText)
	{
		szBuffer.append(feature.getDescription());

		for (int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
		{
			aiYields[iI] = feature.getYieldChange(iI);
		}
		setYieldChangeHelp(szBuffer, L"", L"", L"", aiYields);
	}

	if (feature.getMovementCost() != 1)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_TERRAIN_MOVEMENT_COST", feature.getMovementCost()));
	}

	if (feature.getDefenseModifier() != 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_TERRAIN_DEFENSE_MODIFIER", feature.getDefenseModifier()));
	}

	if (feature.isImpassable())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_TERRAIN_IMPASSABLE"));
	}

	if (feature.isNoCity())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_TERRAIN_NO_CITIES"));
	}

	if (feature.isNoImprovement())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_FEATURE_NO_IMPROVEMENT"));
	}
}


void CvGameTextMgr::setTerrainHelp(CvWStringBuffer &szBuffer, TerrainTypes eTerrain, bool bCivilopediaText)
{
	if (NO_TERRAIN == eTerrain)
	{
		return;
	}
	CvTerrainInfo& terrain = GC.getTerrainInfo(eTerrain);

	int aiYields[NUM_YIELD_TYPES];
	if (!bCivilopediaText)
	{
		szBuffer.append(terrain.getDescription());

		for (int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
		{
			aiYields[iI] = terrain.getYield(iI);
		}
		setYieldChangeHelp(szBuffer, L"", L"", L"", aiYields);
	}

	if (terrain.getMovementCost() != 1)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_TERRAIN_MOVEMENT_COST", terrain.getMovementCost()));
	}

	if (terrain.getBuildModifier() != 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_TERRAIN_BUILD_MODIFIER", terrain.getBuildModifier()));
	}

	if (terrain.getDefenseModifier() != 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_TERRAIN_DEFENSE_MODIFIER", terrain.getDefenseModifier()));
	}

	if (terrain.isImpassable())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_TERRAIN_IMPASSABLE"));
	}
	if (!terrain.isFound())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_TERRAIN_NO_CITIES"));
		bool bFirst = true;
		if (terrain.isFoundCoast())
		{
			szBuffer.append(gDLL->getText("TXT_KEY_TERRAIN_COASTAL_CITIES"));
			bFirst = false;
		}
		if (!bFirst)
		{
			szBuffer.append(gDLL->getText("TXT_KEY_OR"));
		}
	}
}

void CvGameTextMgr::setYieldsHelp(CvWStringBuffer &szBuffer, YieldTypes eYield, bool bCivilopediaText)
{
	if (NO_YIELD == eYield)
	{
		return;
	}

	CvYieldInfo& yield = GC.getYieldInfo(eYield);
	szBuffer.append(yield.getDescription());
}

void CvGameTextMgr::setProductionHelp(CvWStringBuffer &szBuffer, CvCity& city)
{
	FAssertMsg(NO_PLAYER != city.getOwnerINLINE(), "City must have an owner");

	if (city.getCurrentProductionDifference(true) == 0)
	{
		return;
	}

	setYieldHelp(szBuffer, city, YIELD_HAMMERS);

	int iPastOverflow = city.isProductionConvince() ? 0 : city.getOverflowProduction();
	if (iPastOverflow != 0)
	{
		szBuffer.append(NEWLINE);
		///ray Hammer Icon Fix
		szBuffer.append(gDLL->getText("TXT_KEY_MISC_HELP_PROD_OVERFLOW", iPastOverflow, GC.getYieldInfo(YIELD_HAMMERS).getChar()));
	}

	int iBaseProduction = city.calculateNetYield(YIELD_HAMMERS) + iPastOverflow;
	int iBaseModifier = 100;

	UnitTypes eUnit = city.getProductionUnit();
	if (NO_UNIT != eUnit)
	{
		CvUnitInfo& unit = GC.getUnitInfo(eUnit);

		// Domain
		int iDomainMod = city.getDomainProductionModifier((DomainTypes)unit.getDomainType());
		if (0 != iDomainMod)
		{
			szBuffer.append(NEWLINE);
			///ray Hammer Icon Fix
			szBuffer.append(gDLL->getText("TXT_KEY_MISC_HELP_PROD_DOMAIN", iDomainMod, GC.getDomainInfo((DomainTypes)unit.getDomainType()).getTextKeyWide(), GC.getYieldInfo(YIELD_HAMMERS).getChar()));
			iBaseModifier += iDomainMod;
		}

		// Military
		if (unit.isMilitaryProduction())
		{
			int iMilitaryMod = city.getMilitaryProductionModifier() + GET_PLAYER(city.getOwnerINLINE()).getMilitaryProductionModifier();
			if (0 != iMilitaryMod)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_MISC_HELP_PROD_MILITARY", iMilitaryMod));
				iBaseModifier += iMilitaryMod;
			}
		}

		// Trait
		for (int i = 0; i < GC.getNumTraitInfos(); i++)
		{
			if (city.hasTrait((TraitTypes)i))
			{
				int iTraitMod = unit.getProductionTraits(i);

				if (unit.getSpecialUnitType() != NO_SPECIALUNIT)
				{
					iTraitMod += GC.getSpecialUnitInfo((SpecialUnitTypes) unit.getSpecialUnitType()).getProductionTraits(i);
				}
				if (0 != iTraitMod)
				{
					szBuffer.append(NEWLINE);
					///ray Hammer Icon Fix
					szBuffer.append(gDLL->getText("TXT_KEY_MISC_HELP_PROD_TRAIT", iTraitMod, unit.getTextKeyWide(), GC.getTraitInfo((TraitTypes)i).getTextKeyWide(), GC.getYieldInfo(YIELD_HAMMERS).getChar()));
					iBaseModifier += iTraitMod;
				}
			}
		}
	}

	BuildingTypes eBuilding = city.getProductionBuilding();
	if (NO_BUILDING != eBuilding)
	{
		CvBuildingInfo& building = GC.getBuildingInfo(eBuilding);

		// Trait
		for (int i = 0; i < GC.getNumTraitInfos(); i++)
		{
			if (city.hasTrait((TraitTypes)i))
			{
				int iTraitMod = building.getProductionTraits(i);

				if (building.getSpecialBuildingType() != NO_SPECIALBUILDING)
				{
					iTraitMod += GC.getSpecialBuildingInfo((SpecialBuildingTypes) building.getSpecialBuildingType()).getProductionTraits(i);
				}
				iTraitMod += GC.getTraitInfo((TraitTypes) i).getBuildingProductionModifier(building.getBuildingClassType());

				if (0 != iTraitMod)
				{
					szBuffer.append(NEWLINE);
					///ray Hammer Icon Fix
					szBuffer.append(gDLL->getText("TXT_KEY_MISC_HELP_PROD_TRAIT", iTraitMod, building.getTextKeyWide(), GC.getTraitInfo((TraitTypes)i).getTextKeyWide(), GC.getYieldInfo(YIELD_HAMMERS).getChar()));
					iBaseModifier += iTraitMod;
				}
			}
		}
	}

	int iModProduction = (iBaseModifier * iBaseProduction) / 100;

	FAssertMsg(iModProduction == city.getCurrentProductionDifference(true), "Modified Production does not match actual value");

	szBuffer.append(NEWLINE);
	///ray Hammer Icon Fix
	szBuffer.append(gDLL->getText("TXT_KEY_MISC_HELP_PROD_FINAL_YIELD", iModProduction, GC.getYieldInfo(YIELD_HAMMERS).getChar()));
}


void CvGameTextMgr::parsePlayerTraits(CvWStringBuffer &szBuffer, PlayerTypes ePlayer)
{
	CvCivilizationInfo& kCiv = GC.getCivilizationInfo(GET_PLAYER(ePlayer).getCivilizationType());
	CvLeaderHeadInfo& kLeader = GC.getLeaderHeadInfo(GET_PLAYER(ePlayer).getLeaderType());
	bool bFirst = true;

	for (int iTrait = 0; iTrait < GC.getNumTraitInfos(); ++iTrait)
	{
		TraitTypes eTrait = (TraitTypes) iTrait;
		if (kLeader.hasTrait(eTrait) || kCiv.hasTrait(eTrait))
		{
			if (bFirst)
			{
				szBuffer.append(L" (");
				bFirst = false;
			}
			else
			{
				szBuffer.append(L", ");
			}
			szBuffer.append(GC.getTraitInfo(eTrait).getDescription());
		}
	}

	if (!bFirst)
	{
		szBuffer.append(L")");
	}
}

void CvGameTextMgr::parseLeaderHeadHelp(CvWStringBuffer &szBuffer, PlayerTypes eThisPlayer, PlayerTypes eOtherPlayer)
{
	if (NO_PLAYER == eThisPlayer)
	{
		return;
	}

	szBuffer.append(CvWString::format(L"%s (%s)", GET_PLAYER(eThisPlayer).getName(), GC.getCivilizationInfo(GET_PLAYER(eThisPlayer).getCivilizationType()).getDescription()));

	parsePlayerTraits(szBuffer, eThisPlayer);

	szBuffer.append(L"\n");

	if (eOtherPlayer != NO_PLAYER)
	{
		CvTeam& kThisTeam = GET_TEAM(GET_PLAYER(eThisPlayer).getTeam());
		if (eOtherPlayer != eThisPlayer && kThisTeam.isHasMet(GET_PLAYER(eOtherPlayer).getTeam()))
		{
			getAttitudeString(szBuffer, eThisPlayer, eOtherPlayer);

			getActiveDealsString(szBuffer, eThisPlayer, eOtherPlayer);
		}
	}
}


void CvGameTextMgr::parseLeaderLineHelp(CvWStringBuffer &szBuffer, PlayerTypes eThisPlayer, PlayerTypes eOtherPlayer)
{
	if (NO_PLAYER == eThisPlayer || NO_PLAYER == eOtherPlayer)
	{
		return;
	}
	CvTeam& thisTeam = GET_TEAM(GET_PLAYER(eThisPlayer).getTeam());
	CvTeam& otherTeam = GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam());

	if (thisTeam.getID() == otherTeam.getID())
	{
		szBuffer.append(gDLL->getText("TXT_KEY_MISC_PERMANENT_ALLIANCE_HELD"));
		szBuffer.append(NEWLINE);
	}
	else if (thisTeam.isAtWar(otherTeam.getID()))
	{
		szBuffer.append(gDLL->getText("TXT_KEY_WAR"));
		szBuffer.append(NEWLINE);
	}
	else
	{
		if (thisTeam.isDefensivePact(otherTeam.getID()))
		{
			szBuffer.append(gDLL->getText("TXT_KEY_MISC_DEFENSIVE_PACT"));
			szBuffer.append(NEWLINE);
		}
		if (thisTeam.isOpenBorders(otherTeam.getID()))
		{
			szBuffer.append(gDLL->getText("TXT_KEY_MISC_OPEN_BORDERS"));
			szBuffer.append(NEWLINE);
		}
	}
}


void CvGameTextMgr::getActiveDealsString(CvWStringBuffer &szBuffer, PlayerTypes eThisPlayer, PlayerTypes eOtherPlayer)
{
	int iIndex;
	CvDeal* pDeal = GC.getGameINLINE().firstDeal(&iIndex);
	while (NULL != pDeal)
	{
		if ((pDeal->getFirstPlayer() == eThisPlayer && pDeal->getSecondPlayer() == eOtherPlayer)
			|| (pDeal->getFirstPlayer() == eOtherPlayer && pDeal->getSecondPlayer() == eThisPlayer))
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(CvWString::format(L"%c", gDLL->getSymbolID(BULLET_CHAR)));
			getDealString(szBuffer, *pDeal, eThisPlayer);
		}
		pDeal = GC.getGameINLINE().nextDeal(&iIndex);
	}
}

void CvGameTextMgr::buildHintsList(CvWStringBuffer& szBuffer)
{
	for (int i = 0; i < GC.getNumHints(); i++)
	{
		szBuffer.append(CvWString::format(L"%c%s", gDLL->getSymbolID(BULLET_CHAR), GC.getHints(i).getText()));
		szBuffer.append(NEWLINE);
		szBuffer.append(NEWLINE);
	}
}
///TKs Med
void CvGameTextMgr::setYieldPriceHelp(CvWStringBuffer &szBuffer, PlayerTypes ePlayer, YieldTypes eYield, int iAmount)
{
	CvYieldInfo& info = GC.getYieldInfo(eYield);
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	szBuffer.append(CvWString::format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), info.getDescription()));
	if (info.isCargo() && kPlayer.isYieldEuropeTradable(eYield))
	{
		CvPlayer& kParent = GET_PLAYER(kPlayer.getParent());
		if (iAmount == 0)
		{
            szBuffer.append(NEWLINE);
            szBuffer.append(gDLL->getText("TXT_KEY_BUY_AND_SELL_YIELD", kParent.getYieldBuyPrice(eYield), kParent.getYieldSellPrice(eYield)));
			for (int i = 1; i < GC.getNumEuropeInfos(); ++i)
			{
				if (kPlayer.getHasTradeRouteType((EuropeTypes)i) && !GC.getEuropeInfo((EuropeTypes)i).isLeaveFromOwnedCity())
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_BUY_AND_SELL_YIELD_TRADE_SCREEN", GC.getEuropeInfo((EuropeTypes)i).getHelp(), kParent.getYieldBuyPrice(eYield, (EuropeTypes)i), kParent.getYieldSellPrice(eYield, (EuropeTypes)i)));
				}
			}
           
		}
		else if (iAmount > 0)
		{
		    FAssert(iAmount > 0);
		    int iProfit = 0;
		    iProfit = kParent.getYieldBuyPrice(eYield) * iAmount;
		    iProfit -= (iAmount * kPlayer.getTaxRate()) / 100;
            szBuffer.append(NEWLINE);
            szBuffer.append(gDLL->getText("TXT_KEY_BUY_AND_SELL_YIELD_AMOUNT", iProfit, kParent.getYieldBuyPrice(eYield), kParent.getYieldSellPrice(eYield)));

			for (int i = 1; i < GC.getNumEuropeInfos(); ++i)
			{
				if (kPlayer.getHasTradeRouteType((EuropeTypes)i) && !GC.getEuropeInfo((EuropeTypes)i).isLeaveFromOwnedCity())
				{
					iProfit = kParent.getYieldBuyPrice(eYield,(EuropeTypes)i) * iAmount;
					iProfit -= (iAmount * kPlayer.getTaxRate()) / 100;
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_BUY_AND_SELL_YIELD_TRADE_SCREEN_AMOUNT", GC.getEuropeInfo((EuropeTypes)i).getHelp(), iProfit, kParent.getYieldBuyPrice(eYield, (EuropeTypes)i), kParent.getYieldSellPrice(eYield, (EuropeTypes)i)));
				}
			}
		}
	}
	szBuffer.append(ENDCOLR);
}

void CvGameTextMgr::setYieldHelp(CvWStringBuffer &szBuffer, CvCity& city, YieldTypes eYieldType, bool bSelectedArmor)
{
	FAssertMsg(NO_PLAYER != city.getOwnerINLINE(), "City must have an owner");

	if (bSelectedArmor && GC.isEquipmentType(eYieldType, EQUIPMENT_HEAVY_ARMOR))
	{
		if (city.getSelectedArmor() != eYieldType)
		{
			eYieldType = city.getSelectedArmor();
		}
	}
	///TKe
	if (NO_YIELD == eYieldType)
	{
		return;
	}
	CvYieldInfo& info = GC.getYieldInfo(eYieldType);

	if (NO_PLAYER == city.getOwnerINLINE())
	{
		return;
	}
	CvPlayer& owner = GET_PLAYER(city.getOwnerINLINE());

	setYieldPriceHelp(szBuffer, city.getOwnerINLINE(), eYieldType);

	if (city.isOccupation())
	{
		return;
	}

	int iBaseProduction = 0;
    ///TKs Invention Core Mod v 1.0
    bool iGetFromBuilding = true;
    if (eYieldType == YIELD_IDEAS)
    {
       if (owner.getCurrentResearch() == NO_CIVIC)
       {
            iGetFromBuilding = false;
       }
    }
	int iBuildingYield = 0;
	if (iGetFromBuilding)
    {
        for (int i = 0; i < GC.getNumBuildingInfos(); ++i)
        {
            if (city.isHasBuilding((BuildingTypes)i))
            {
                ///Tks Med
                if (GC.getBuildingInfo((BuildingTypes)i).getYieldChange(eYieldType) > 0)
                {
                    YieldTypes eMustSaleYield = (YieldTypes)GC.getBuildingInfo((BuildingTypes)i).getAutoSellYieldChange();
                    if (eMustSaleYield == NO_YIELD)
                    {
                        iBuildingYield += GC.getBuildingInfo((BuildingTypes)i).getYieldChange(eYieldType);
                    }
                    else if (city.isMarket(eMustSaleYield) && city.getYieldStored(eMustSaleYield) >= (city.getMaintainLevel(eMustSaleYield) + 1))
                    {

                        int iLoss = std::max(GC.getXMLval(XML_CITY_YIELD_DECAY_PERCENT) * city.getYieldStored(eMustSaleYield) / 100, GC.getXMLval(XML_MIN_CITY_YIELD_DECAY));

                        iLoss = std::min(city.getYieldStored(eMustSaleYield) - city.getMaintainLevel(eMustSaleYield), iLoss);
                        if (iLoss >= GC.getBuildingInfo((BuildingTypes)i).getYieldChange(eYieldType))
                        {
                            iBuildingYield += GC.getBuildingInfo((BuildingTypes)i).getYieldChange(eYieldType);
                        }
                        else if ((GC.getBuildingInfo((BuildingTypes)i).getYieldChange(eYieldType) - iLoss) > 0)
                        {
                            iBuildingYield += iLoss;
                        }

                    }

                }
                else
                {
                    iBuildingYield += GC.getBuildingInfo((BuildingTypes) i).getYieldChange(eYieldType);
                }

                //iBuildingYield += GC.getBuildingInfo((BuildingTypes) i).getYieldChange(eYieldType);
                ///Tke
                iBuildingYield += city.getBuildingYieldChange((BuildingClassTypes)GC.getBuildingInfo((BuildingTypes) i).getBuildingClassType(), eYieldType);
                iBuildingYield += owner.getBuildingYieldChange((BuildingClassTypes)GC.getBuildingInfo((BuildingTypes) i).getBuildingClassType(), eYieldType);
            }
        }

        if (iBuildingYield != 0)
        {
            szBuffer.append(NEWLINE);
            szBuffer.append(gDLL->getText("TXT_KEY_MISC_HELP_BUILDING_YIELD", iBuildingYield, info.getChar()));

            iBaseProduction += iBuildingYield;
        }
    }

	int Bonus = 0;
//	if (city.isHasRealBuilding((BuildingTypes)GC.getDefineINT("STEAMWORKS_BUILDING")) && eYieldType != YIELD_HAMMERS && eYieldType != YIELD_COAL)
//	{
//	    int iConsumedCoal = city.getRawYieldConsumed(YIELD_COAL);
//        int iCoalMod = city.getYieldStored(YIELD_COAL) + city.getBaseRawYieldProduced(YIELD_COAL) * city.getBaseYieldRateModifier(YIELD_COAL) / 100 - iConsumedCoal;
//
//        if (iConsumedCoal > 0)
//        {
//            int SteamWorksMod = std::max(1, GC.getDefineINT("TK_STEAMWORKS_MODIFIER"));
//            if (iCoalMod != -iConsumedCoal)
//            {
//                if (iCoalMod > iConsumedCoal && iCoalMod != 0)
//                {
//                    Bonus = iConsumedCoal / SteamWorksMod;
//                }
//                else if (iCoalMod < iConsumedCoal)
//                {
//                    Bonus = (iConsumedCoal + iCoalMod) / SteamWorksMod;
//                }
//                else if (iCoalMod == 0)
//                {
//                    Bonus = iConsumedCoal / SteamWorksMod;
//                }
//            }
//
//        }
//	}

	// MultipleYieldsProduced Start by Aymerick 22/01/2010
	std::vector< std::vector<int> > aaiProfessionYields;
	aaiProfessionYields.resize(GC.getNumProfessionInfos());

	// Indoor professions

	int iNoReasearch = 0;
    int iCitizenYield = 0;
	for (int i = 0; i < city.getPopulation(); ++i)
	{
		CvUnit* pUnit = city.getPopulationUnitByIndex(i);
		if (NULL != pUnit)
		{
			ProfessionTypes eProfession = pUnit->getProfession();
			if (NO_PROFESSION != eProfession)
			{
				CvProfessionInfo& kProfessionInfo = GC.getProfessionInfo(eProfession);
				aaiProfessionYields[eProfession].resize(kProfessionInfo.getNumYieldsProduced(), 0);
					YieldTypes eYieldProduced = (YieldTypes) kProfessionInfo.getYieldsProduced(0);
					YieldTypes eYieldConsumed = (YieldTypes) kProfessionInfo.getYieldsConsumed(0);
				for (int j = 0; j < kProfessionInfo.getNumYieldsProduced(); j++)
				{
					if (kProfessionInfo.getYieldsProduced(j) == eYieldType)
					{
						int iCityYieldProduction = city.getProfessionOutput(eProfession, pUnit);

						if (iCityYieldProduction != 0)
						{

                            if (eYieldProduced == YIELD_IDEAS)
                            {
                                if (city.canResearch() <= 0)
                                {
                                    iCityYieldProduction = 0;
                                }
                            }

                            if (eYieldConsumed != NO_YIELD && GC.getYieldInfo(eYieldProduced).getUnitClass() != NO_UNITCLASS && eYieldProduced != YIELD_HAMMERS)
                            {
                                iCitizenYield = city.getProfessionOutput(eProfession, pUnit) + Bonus;
                            }
                            else
                            {
                                iCitizenYield = city.getProfessionOutput(eProfession, pUnit);
                            }


                            aaiProfessionYields[eProfession][j] += iCitizenYield;
                            iBaseProduction += iCitizenYield;
						}
					}
				}
			}
		}
	}
     ///TKe
	// From plots
	int iPlotYield = 0;
	int iCityPlotYield = 0;
	for (int i = 0; i < NUM_CITY_PLOTS; ++i)
	{
		CvPlot* pPlot = city.getCityIndexPlot(i);
		if (pPlot != NULL)
		{
			if (i == CITY_HOME_PLOT)
			{
				iCityPlotYield = pPlot->getYield(eYieldType);
			}
			else
			{
				CvUnit* pUnit = city.getUnitWorkingPlot(i);
				if (NULL != pUnit && pUnit->getOwnerINLINE() == city.getOwnerINLINE())
				{
					ProfessionTypes eProfession = pUnit->getProfession();
					if (NO_PROFESSION != eProfession)
					{
						for (int j = 0; j < GC.getProfessionInfo(eProfession).getNumYieldsProduced(); j++)
						{
							if (GC.getProfessionInfo(eProfession).getYieldsProduced(j) == eYieldType)
							{
								int iPlotYield = pPlot->getYield(eYieldType);
								aaiProfessionYields[eProfession][j] += iPlotYield;
								iBaseProduction += iPlotYield;
							}
						}
					}
				}
			}
		}
	}

	for (uint i = 0; i < aaiProfessionYields.size(); ++i)
	{
		for (uint j = 0; j < aaiProfessionYields[i].size(); ++j)
		{
		///TKs Invention Core Mod v 1.0
        if ((YieldTypes)GC.getProfessionInfo((ProfessionTypes)i).getYieldsProduced(0) == YIELD_IDEAS && eYieldType == YIELD_IDEAS)
        {
            if (owner.getCurrentResearch() != NO_CIVIC)
            {
                CivicTypes eCivic = owner.getCurrentResearch();
                CvCivicInfo& kCivicInfo = GC.getCivicInfo(eCivic);

                int iProgress = 0;

                if (GC.getCostToResearch(eCivic) > 0)
                {
                    iProgress = owner.getCurrentResearchProgress(true);

                }
                std::vector<YieldTypes> eRequiredYield;
                for (int iResearch = 0; iResearch < NUM_YIELD_TYPES; iResearch++)
                {
                    if (kCivicInfo.getRequiredYields(iResearch) > 0)
                    {
                        eRequiredYield.push_back((YieldTypes)iResearch);
                    }

                }

                CvWString szYieldsList;
                for (std::vector<YieldTypes>::iterator it = eRequiredYield.begin(); it != eRequiredYield.end(); ++it)
                {
                    if (!szYieldsList.empty())
                    {
                        if (*it == eRequiredYield.back())
                        {
                            szYieldsList += CvWString::format(gDLL->getText("TXT_KEY_AND"));
                        }
                        else
                        {
                            szYieldsList += L", ";
                        }
                    }
                    szYieldsList += CvWString::format(L"%c", GC.getYieldInfo(*it).getChar());
                }
                int iCanResearch = city.canResearch();
                if (kCivicInfo.getConvertsResearchYield() == NO_YIELD)
                {
                    if (iCanResearch > 0)
                    {
                        szBuffer.append(gDLL->getText("TXT_KEY_CURRENT_PLAYER_RESEARCH_COUNT", kCivicInfo.getDescription(), iProgress));
                    }
                    else if (iCanResearch == -1)
                    {
                       szBuffer.append(gDLL->getText("TXT_KEY_CURRENT_PLAYER_RESEARCH_NO_UNITCLASS", kCivicInfo.getDescription()));
                    }
                    else if (iCanResearch == 0)
                    {
                       szBuffer.append(gDLL->getText("TXT_KEY_CURRENT_PLAYER_RESEARCH_NO_PROEJECT", kCivicInfo.getDescription()));
                    }
                }
                else
                {
                   YieldTypes eConvertedYield =  (YieldTypes)kCivicInfo.getConvertsResearchYield();
                    szBuffer.append(NEWLINE);
                    szBuffer.append(gDLL->getText("TXT_KEY_RESEARCH_CONVERTS_YIELD", GC.getYieldInfo(eConvertedYield).getChar()));
                }
                if (!szYieldsList.empty())
                {
                    szBuffer.append(NEWLINE);
                    szBuffer.append(gDLL->getText("TXT_KEY_YIELDS_CONVERTED_IDEAS", szYieldsList.GetCString()));
                }

                szBuffer.append(L"\n=======================");
            }

        }
        ///TKe
			if (aaiProfessionYields[i][j] > 0)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_MISC_HELP_BASE_CITIZEN_YIELD", aaiProfessionYields[i][j], info.getChar(), GC.getProfessionInfo((ProfessionTypes)i).getTextKeyWide()));
			}
		}
	}
	// MultipleYieldsProduced End

	//city plot
	if (iCityPlotYield > 0)
	{
		iBaseProduction += iCityPlotYield;
		szBuffer.append(NEWLINE);
		szBuffer.append(CvWString::format(gDLL->getText("TXT_KEY_MISC_FROM_CITY_YIELD", iCityPlotYield, info.getChar())));
	}
	//Tks Civics Screen
	if (city.getConnectedTradeYield(eYieldType) > 0)
	{
		szBuffer.append(NEWLINE);
        //szBuffer.append(CvWString::format(gDLL->getText("TXT_KEY_CONNECTED_YIELD_BONUS", city.getConnectedTradeYield(eYieldType), info.getChar(), GC.getCivicInfo(YIELD_IDEAS).getChar
		szBuffer.append(CvWString::format(gDLL->getText("TXT_KEY_CONNECTED_YIELD_BONUS", city.getConnectedTradeYield(eYieldType), info.getChar())));
		iBaseProduction += city.getConnectedTradeYield(eYieldType);
	}

	if (owner.getGarrisonUnitBonus(eYieldType) > 0)
	{
		int iGarrisonBonus = 0;
		CvPlot* pPlot = city.plot();
		CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = pPlot->nextUnitNode(pUnitNode);
			if (pLoopUnit->getOwnerINLINE() == city.getOwnerINLINE())
			{
				if (pLoopUnit->canGarrison())
				{
					iGarrisonBonus += owner.getGarrisonUnitBonus(eYieldType);
				}
			}
		}
		szBuffer.append(NEWLINE);
        szBuffer.append(CvWString::format(gDLL->getText("TXT_KEY_CIVIC_GARRISONED_BONUS", iGarrisonBonus, info.getChar())));
		iBaseProduction += iGarrisonBonus;
	}

	if (city.getConnectedMissionYield(eYieldType) > 0)
	{
		szBuffer.append(NEWLINE);
        szBuffer.append(CvWString::format(gDLL->getText("TXT_KEY_CONNECTED_YIELD_BONUS", city.getConnectedMissionYield(eYieldType), info.getChar())));
		iBaseProduction += city.getConnectedMissionYield(eYieldType);
	}

	///TKs Med Update 1.1h
	int iConvertedResearch = 0;
	if (GET_PLAYER(city.getOwnerINLINE()).getCurrentResearch() != NO_CIVIC)
	{
	    CvCivicInfo& kCivicInfo = GC.getCivicInfo((CivicTypes)GET_PLAYER(city.getOwnerINLINE()).getCurrentResearch());
	    YieldTypes eConvertYield = (YieldTypes)kCivicInfo.getConvertsResearchYield();
	    if (eConvertYield != NO_YIELD && eConvertYield == eYieldType)
	    {
	        iConvertedResearch = city.calculateNetYield(YIELD_IDEAS);
//	        CvCivilizationInfo& civilizationInfo = GC.getCivilizationInfo(city.getCivilizationType());
//            iConvertedResearch += civilizationInfo.getFreeYields(YIELD_IDEAS);
	        //iBaseProduction += iConvertedResearch;
	        if (iConvertedResearch > 0)
	        {
                szBuffer.append(NEWLINE);
                szBuffer.append(CvWString::format(gDLL->getText("TXT_KEY_RESEARCH_CONVERTS_YIELD_HELP", iConvertedResearch, info.getChar(), GC.getYieldInfo(YIELD_IDEAS).getChar())));
	        }
	    }
	}
	///TKe Update
    ///TK Coal
//	if (Bonus > 0)
//	{
//	    int iConsumedCoal = city.getRawYieldConsumed(YIELD_COAL);
//        int iCoalMod = city.getYieldStored(YIELD_COAL) + city.getBaseRawYieldProduced(YIELD_COAL) * city.getBaseYieldRateModifier(YIELD_COAL) / 100 - iConsumedCoal;
//        if (iConsumedCoal > 0)
//        {
//            int SteamWorksMod = std::max(1, GC.getDefineINT("TK_STEAMWORKS_MODIFIER"));
//            if (iCoalMod != -iConsumedCoal)
//            {
//                if (iCoalMod > iConsumedCoal && iCoalMod != 0)
//                {
//                    Bonus = iConsumedCoal / SteamWorksMod;
//                }
//                else if (iCoalMod < iConsumedCoal)
//                {
//                    Bonus = (iConsumedCoal + iCoalMod) / SteamWorksMod;
//                }
//                else if (iCoalMod == 0)
//                {
//                    Bonus = iConsumedCoal / SteamWorksMod;
//                }
//            }
//
//        }
//
//        szBuffer.append(NEWLINE);
//		szBuffer.append(CvWString::format(gDLL->getText("TXT_KEY_PRODUCTION_FROM_STEAMWORKS", Bonus, info.getChar())));
//	}
    ///Tks Med
//    if (eYieldType == YIELD_IDEAS && owner.getCurrentResearch() != NO_CIVIC)
//    {
//        CvCivilizationInfo& civilizationInfo = GC.getCivilizationInfo(city.getCivilizationType());
//        iBaseProduction += civilizationInfo.getFreeYields(eYieldType);
//    }
	///TKe
	FAssert(iBaseProduction == city.getBaseRawYieldProduced(eYieldType));

	int aiYields[NUM_YIELD_TYPES];
	int aiRawProducedYields[NUM_YIELD_TYPES];
	int aiRawConsumedYields[NUM_YIELD_TYPES];
	city.calculateNetYields(aiYields, aiRawProducedYields, aiRawConsumedYields);
	int iUnproduced = city.getBaseRawYieldProduced(eYieldType) - aiRawProducedYields[eYieldType];
	if (iUnproduced > 0)
	{
		// MultipleYieldsConsumed Start by Aymerick 05/01/2010
		std::vector<YieldTypes> eMissing;

		for (int i = 0; i < city.getPopulation(); ++i)
		{
			CvUnit* pUnit = city.getPopulationUnitByIndex(i);
			if (NULL != pUnit)
			{
				ProfessionTypes eProfession = pUnit->getProfession();
				if (eProfession != NO_PROFESSION)
				{
					// MultipleYieldsProduced Start by Aymerick 22/01/2010
					for (int j = 0; j < GC.getProfessionInfo(eProfession).getNumYieldsProduced(); j++)
					{
						if (GC.getProfessionInfo(eProfession).getYieldsProduced(j) == eYieldType)
						{
							for (int k = 0; k < GC.getProfessionInfo(eProfession).getNumYieldsConsumed(owner.getID()); k++)
							{
								YieldTypes eYieldConsumed = (YieldTypes) GC.getProfessionInfo(eProfession).getYieldsConsumed(k, owner.getID());
								if (GC.getProfessionInfo(eProfession).getYieldsConsumed(k, owner.getID()) != NO_YIELD)
								{
									if (city.getYieldStored(eYieldConsumed) < city.getRawYieldConsumed(eYieldConsumed))
									{
										eMissing.push_back((YieldTypes) GC.getProfessionInfo(eProfession).getYieldsConsumed(k, owner.getID()));
									}
								}
							}
						}
					}
					// MultipleYieldsProduced End
				}
			}
		}
		///TKs Invention Core Mod v 1.0
		if ( eYieldType == YIELD_IDEAS && GET_PLAYER(city.getOwnerINLINE()).getCurrentResearch() == NO_CIVIC)
        {
            iBaseProduction = 0;
            szBuffer.append(NEWLINE);
            szBuffer.append(gDLL->getText("TXT_KEY_RESEARCH_CANNOT_CONTINUE_NO_RESEARCH_CITY"));
        }
        ///TKe

		if (!eMissing.empty())
		{
			CvWString szYieldsList;
			for (std::vector<YieldTypes>::iterator it = eMissing.begin(); it != eMissing.end(); ++it)
			{
				if (!szYieldsList.empty())
				{
					if (*it == eMissing.back())
					{
						szYieldsList += CvWString::format(gDLL->getText("TXT_KEY_AND"));
					}
					else
					{
						szYieldsList += L", ";
					}
				}
				szYieldsList += CvWString::format(L"%c", GC.getYieldInfo(*it).getChar());
			}
			iBaseProduction -= iUnproduced;
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_MISC_UNPRODUCED_CITY_YIELD_SPECIFIC", -iUnproduced, info.getChar(), szYieldsList.GetCString()));
		}
		// MultipleYieldsConsumed End
	}

	int iModifiedProduction = iBaseProduction;
	if (iBaseProduction != 0)
	{
		int iModifier = setCityYieldModifierString(szBuffer, eYieldType, city);
		if (iModifier != 100)
		{
			iModifiedProduction *= iModifier;
			iModifiedProduction /= 100;
		}

//		if (iModifiedProduction != iBaseProduction)
//		{
//			szBuffer.append(SEPARATOR);
//			szBuffer.append(NEWLINE);
//			szBuffer.append(gDLL->getText("TXT_KEY_MISC_HELP_TOTAL_YIELD_PRODUCED", info.getTextKeyWide(), iModifiedProduction, info.getChar()));
//		}
	}

	// from immigration
	int iImmigration = 0;
	if (eYieldType != YIELD_CROSSES)
	{
		if (owner.getImmigrationConversion() == eYieldType)
		{
			iImmigration += aiYields[YIELD_CROSSES];
		}
	}
	///TKs Med Update 1.1h
	iModifiedProduction += iConvertedResearch;
	///TKe
	if (iImmigration > 0)
	{
		iModifiedProduction += iImmigration;
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_MISC_HELP_BASE_CITIZEN_IMMIGRATION", iImmigration, info.getChar(), GC.getYieldInfo(YIELD_CROSSES).getChar()));
	}


	int iConsumed = aiRawConsumedYields[eYieldType];
	if (iConsumed > 0)
	{
		// MultipleYieldsConsumed Start by Aymerick 05/01/2010
		std::vector<YieldTypes> eConverted;

		for (int i = 0; i < city.getPopulation(); ++i)
		{
			CvUnit* pUnit = city.getPopulationUnitByIndex(i);
			if (NULL != pUnit)
			{
				ProfessionTypes eProfession = pUnit->getProfession();
				if (eProfession != NO_PROFESSION)
				{

					for (int j = 0; j < GC.getProfessionInfo(eProfession).getNumYieldsConsumed(owner.getID()); j++)
					{

					    ///Tk Med 3.1
					    //if (GC.getProfessionInfo(eProfession).getNumYieldsConsumed(owner.getID()) > 1)
					    //{
					    //    int iMultiYieldStored = 0;
					    //    //int iMultiYieldProduced = 0;
         //                   for (int n = 0; n < GC.getProfessionInfo(eProfession).getNumYieldsConsumed(owner.getID()); n++)
         //                   {
         //                       YieldTypes eYieldConsumed = (YieldTypes) GC.getProfessionInfo(eProfession).getYieldsConsumed(n);
         //                       iMultiYieldStored = city.getYieldStored(eYieldConsumed) - iConsumed + iModifiedProduction;
         //                       if (iMultiYieldStored < 0)
         //                       {
         //                          iConsumed = 0;
         //                       }

         //                   }
					    //}
					    if (iConsumed > 0)
					    {
                            if (GC.getProfessionInfo(eProfession).getYieldsConsumed(j, owner.getID()) == eYieldType)
                            {
                                // MultipleYieldsProduced Start by Aymerick 22/01/2010
                                for (int k = 0; k < GC.getProfessionInfo(eProfession).getNumYieldsProduced(); k++)
                                {
                                    if (GC.getProfessionInfo(eProfession).getYieldsProduced(k) != NO_YIELD)
                                    {
                                        eConverted.push_back((YieldTypes) GC.getProfessionInfo(eProfession).getYieldsProduced(k));
                                    }
                                }
                                // MultipleYieldsProduced End
                            }
					    }
					}
				}
			}
		}

		//food consumed
		int iTempConsumed = iConsumed;
		if (eYieldType == YIELD_FOOD)
		{
			int iFoodConsumed = city.foodConsumption();
			iTempConsumed -= iFoodConsumed;
			if (iFoodConsumed != 0)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_MISC_CONSUMED_FOOD", -iFoodConsumed));
			}
		}

		if (!eConverted.empty())
		{
			CvWString szYieldsList;
			for (std::vector<YieldTypes>::iterator it = eConverted.begin(); it != eConverted.end(); ++it)
			{
				if (!szYieldsList.empty())
				{
					//if (*it == eConverted.back())
					//{
						//szYieldsList += CvWString::format(gDLL->getText("TXT_KEY_AND"));
					//}
					//else
					//{
						szYieldsList += L", ";
					//}
				}
				szYieldsList += CvWString::format(L"%c", GC.getYieldInfo(*it).getChar());
			}
			iBaseProduction -= iUnproduced;
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_MISC_CONSUMED_CITY_YIELD_SPECIFIC", -iTempConsumed, info.getChar(), szYieldsList.GetCString()));
		}
		// MultipleYieldsConsumed End
	}

	iModifiedProduction -= iConsumed;

	szBuffer.append(SEPARATOR);
	szBuffer.append(NEWLINE);
	///TKs Med
	if (eYieldType == YIELD_CULTURE)
	{
//	    int iCrossRate = city.getYieldRate(YIELD_CROSSES);
//	    int iBellsRate = city.getYieldRate(YIELD_BELLS);
//	    int iEducationRate = city.getYieldRate(YIELD_EDUCATION);
        int iCrossRate = city.calculateNetYield(YIELD_CROSSES);
	    int iBellsRate = city.calculateNetYield(YIELD_BELLS);
	    int iEducationRate = city.calculateNetYield(YIELD_EDUCATION);
	    iCrossRate = iCrossRate + (iBellsRate + iEducationRate) / 2;
	    szBuffer.append(gDLL->getText("TXT_KEY_YIELD_TOTAL_CITY_IMMIGRATION", iCrossRate));
	    szBuffer.append(NEWLINE);
	    szBuffer.append(gDLL->getText("TXT_KEY_YIELD_TOTAL_THREES", GC.getYieldInfo(YIELD_CROSSES).getChar(), GC.getYieldInfo(YIELD_BELLS).getChar(), GC.getYieldInfo(YIELD_EDUCATION).getChar()));
        //iCrossRate = (iCrossRate + owner.getYieldRate(YIELD_BELLS)) / 2;
	}
	else
	{
        szBuffer.append(gDLL->getText("TXT_KEY_YIELD_TOTAL", info.getTextKeyWide(), iModifiedProduction, info.getChar()));
	}

	FAssert(iModifiedProduction == aiYields[eYieldType]);

	if (eYieldType == YIELD_CULTURE)
	{
		szBuffer.append(SEPARATOR);
		szBuffer.append(NEWLINE);
		///TKs Med
		int iCulture = city.getCulture(city.getOwnerINLINE());
		szBuffer.append(gDLL->getText("TXT_KEY_MISC_CULTURE", iCulture, city.getCultureThreshold()));

		int iCultureRate = city.getCultureRate();

		if (iCultureRate > 0)
		{
			int iCultureLeft = city.getCultureThreshold() - iCulture;

			if (iCultureLeft > 0)
			{
				int iTurnsLeft = (iCultureLeft  + iCultureRate - 1) / iCultureRate;

				szBuffer.append(L' ');
				szBuffer.append(gDLL->getText("INTERFACE_CITY_TURNS", std::max(1, iTurnsLeft)));
			}
		}
		szBuffer.append(NEWLINE);
        szBuffer.append(gDLL->getText("TXT_KEY_YIELD_TOTAL_CULTURE", iCultureRate, info.getChar()));
        szBuffer.append(NEWLINE);
	    szBuffer.append(gDLL->getText("TXT_KEY_YIELD_TOTAL_THREES", info.getChar(), GC.getYieldInfo(YIELD_CROSSES).getChar(), GC.getYieldInfo(YIELD_EDUCATION).getChar()));
        ///TKe
		szBuffer.append(SEPARATOR);

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				int iCulturePercent = city.plot()->calculateCulturePercent((PlayerTypes)iI);
				if (iCulturePercent > 0)
				{
					CvWString szTempBuffer;
					szTempBuffer.Format(L"\n%d%% " SETCOLR L"%s" ENDCOLR, iCulturePercent, GET_PLAYER((PlayerTypes)iI).getPlayerTextColorR(), GET_PLAYER((PlayerTypes)iI).getPlayerTextColorG(), GET_PLAYER((PlayerTypes)iI).getPlayerTextColorB(), GET_PLAYER((PlayerTypes)iI).getPlayerTextColorA(), GET_PLAYER((PlayerTypes)iI).getCivilizationAdjective());
					szBuffer.append(szTempBuffer);
				}
			}
		}

		szBuffer.append(L"\n=======================\n");
	}
	///TKs MEd
	UnitClassTypes eUnitClassTypes = (UnitClassTypes)GC.getYieldInfo(eYieldType).getUnitClass();
	if (eUnitClassTypes != NO_UNITCLASS)
	{
        UnitTypes eUnit = (UnitTypes) GC.getUnitClassInfo(eUnitClassTypes).getDefaultUnitIndex();
        if (eUnit != NO_UNIT && GC.getUnitInfo(eUnit).getRequiredTransportSize() > 1)
        {
            szBuffer.append(L"\n=======================\n");
            //szBuffer.append(NEWLINE);
            szBuffer.append(gDLL->getText("TXT_KEY_UNIT_CARGO_YIELD_SPACE", GC.getUnitInfo(eUnit).getRequiredTransportSize()));

        }
	}
    ///TKe

	if (gDLL->shiftKey() && (gDLL->getChtLvl() > 0))
	{
		szBuffer.append(CvWString::format(L"\nValue : %d", GET_PLAYER(city.getOwnerINLINE()).AI_yieldValue(eYieldType)));
		szBuffer.append(CvWString::format(L"\nLevel: %d", city.getMaintainLevel(eYieldType)));
		szBuffer.append(CvWString::format(L"\nTrade: %d", city.AI_getTradeBalance(eYieldType)));
		szBuffer.append(CvWString::format(L"\nAdvant: %d", city.AI_getYieldAdvantage(eYieldType)));
	}
}

int CvGameTextMgr::setCityYieldModifierString(CvWStringBuffer& szBuffer, YieldTypes eYieldType, const CvCity& kCity)
{
	CvYieldInfo& info = GC.getYieldInfo(eYieldType);
	CvPlayer& kOwner = GET_PLAYER(kCity.getOwnerINLINE());

	int iBaseModifier = 100;

	// Buildings
	int iBuildingMod = 0;
	for (int i = 0; i < GC.getNumBuildingInfos(); i++)
	{
		CvBuildingInfo& infoBuilding = GC.getBuildingInfo((BuildingTypes)i);
		if (kCity.isHasBuilding((BuildingTypes)i))
		{
		    iBuildingMod += infoBuilding.getYieldModifier(eYieldType);
		}
	}
	if (NULL != kCity.area())
	{
		iBuildingMod += kCity.area()->getYieldRateModifier(kCity.getOwnerINLINE(), eYieldType);
	}
	if (0 != iBuildingMod)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_MISC_HELP_YIELD_BUILDINGS", iBuildingMod, info.getChar()));
		iBaseModifier += iBuildingMod;
	}

	// Capital
	if (kCity.isCapital())
	{
		int iCapitalMod = kOwner.getCapitalYieldRateModifier(eYieldType);
		if (0 != iCapitalMod)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_MISC_HELP_YIELD_CAPITAL", iCapitalMod, info.getChar()));
			iBaseModifier += iCapitalMod;
		}
	}

	// Civics
	int iCivicMod = 0;
	for (int i = 0; i < GC.getNumCivicOptionInfos(); i++)
	{
		if (NO_CIVIC != kOwner.getCivic((CivicOptionTypes)i))
		{
			iCivicMod += GC.getCivicInfo(kOwner.getCivic((CivicOptionTypes)i)).getYieldModifier(eYieldType);
		}
	}

	if (0 != iCivicMod)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_MISC_HELP_YIELD_CIVICS", iCivicMod, info.getChar()));
		iBaseModifier += iCivicMod;
	}
	///TKs Med
	iCivicMod = 0;
	for (int iCivic = 0; iCivic < GC.getNumCivicInfos(); iCivic++)
    {
        if (GC.getCivicInfo((CivicTypes)iCivic).getCivicOptionType() == CIVICOPTION_INVENTIONS)
        {
            if (kOwner.getIdeasResearched((CivicTypes) iCivic) > 0)
            {
                iCivicMod += GC.getCivicInfo((CivicTypes)iCivic).getYieldModifier(eYieldType);
                if (0 != iCivicMod)
                {
                    szBuffer.append(NEWLINE);
                    szBuffer.append(gDLL->getText("TXT_KEY_MISC_HELP_YIELD_RESEARCH", iCivicMod, info.getChar(), GC.getCivicInfo((CivicTypes)iCivic).getDescription()));
                    iBaseModifier += iCivicMod;
                    iCivicMod = 0;
                }
            }
        }
    }
	///TKe

	// Founding Fathers
	for (int i = 0; i < GC.getNumTraitInfos(); i++)
	{
		if (GET_PLAYER(kCity.getOwnerINLINE()).hasTrait((TraitTypes) i))
		{
			CvTraitInfo& kTraitInfo = GC.getTraitInfo((TraitTypes) i);

			int iTraitMod = kTraitInfo.getYieldModifier(eYieldType);
			if (kTraitInfo.isTaxYieldModifier(eYieldType))
			{
				iTraitMod += kOwner.getTaxRate();
			}

			if (0 != iTraitMod)
			{
				iBaseModifier += iTraitMod;
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_MISC_HELP_YIELD_FATHER", iTraitMod, info.getChar(), kTraitInfo.getTextKeyWide()));
			}
		}
	}

	int iRebelMod = kCity.getRebelPercent() * GC.getMAX_REBEL_YIELD_MODIFIER() / 100;
	if (0 != iRebelMod)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_MISC_HELP_YIELD_REBEL", iRebelMod, info.getChar()));
		iBaseModifier += iRebelMod;
	}


	FAssertMsg(iBaseModifier == kCity.getBaseYieldRateModifier(eYieldType), "Yield Modifier in setProductionHelp does not agree with actual value");

	return iBaseModifier;
}

void CvGameTextMgr::parseGreatGeneralHelp(CvWStringBuffer &szBuffer, CvPlayer& kPlayer)
{
	szBuffer.assign(gDLL->getText("TXT_KEY_MISC_GREAT_GENERAL", kPlayer.getCombatExperience(), kPlayer.greatGeneralThreshold()));
}


//------------------------------------------------------------------------------------------------

void CvGameTextMgr::buildCityBillboardIconString( CvWStringBuffer& szBuffer, CvCity* pCity)
{
	szBuffer.clear();
	///TKs Med
	PlayerTypes ePlayer = pCity->getMissionaryPlayer();
	//FAssert(ePlayer != UNKNOWN_PLAYER);

	if (ePlayer != NO_PLAYER)
	{
		//FAssert(GET_PLAYER(ePlayer).isAlive())
		//LeaderHeadTypes eLeader = GET_PLAYER(ePlayer).getLeaderType();
		//FAssert(eLeader != NO_LEADER);
		if (GET_PLAYER(ePlayer).isAlive())
		{
			szBuffer.append(CvWString::format(L" %c", GC.getCivilizationInfo(GET_PLAYER(pCity->getMissionaryPlayer()).getCivilizationType()).getMissionaryChar()));
		}
	}

	// XXX out this in bottom bar???
	if (pCity->isOccupation())
	{
		szBuffer.append(CvWString::format(L" (%c:%d)", gDLL->getSymbolID(OCCUPATION_CHAR), pCity->getOccupationTimer()));
	}

	if (true)
	{
		//stored arms
		CvWStringBuffer szTemp;
		TeamTypes eTeam = GC.getGameINLINE().getActiveTeam();
		bool bTradeCity = pCity->isTradePostBuilt(eTeam);
		std::vector<int> aYieldShown(NUM_YIELD_TYPES, 0);
		if (!pCity->isNative() && pCity->isVisible(GC.getGameINLINE().getActiveTeam(), true))
		{
            for (int iProfession = 0; iProfession < GC.getNumProfessionInfos(); ++iProfession)
            {
                ProfessionTypes eProfession = (ProfessionTypes) iProfession;

                if ((GC.getProfessionInfo(eProfession).isCityDefender() && GC.getCivilizationInfo(pCity->getCivilizationType()).isValidProfession(eProfession)))
                {
                    for(int iYield=0;iYield<NUM_YIELD_TYPES;iYield++)
                    {
                        YieldTypes eYield = (YieldTypes) iYield;
                        int iYieldEquipment = GET_PLAYER(pCity->getOwnerINLINE()).getYieldEquipmentAmount(eProfession, eYield);
                        if (iYieldEquipment > 0 && pCity->getYieldStored(eYield) >= iYieldEquipment)
                        {
                            if (aYieldShown[iYield] == 0)
                            {
                                aYieldShown[iYield] = 1;
                                iYield -= 1;
                                YieldTypes eFixedYield = (YieldTypes) iYield;
                                if (iYield > 0 && eFixedYield != NO_YIELD)
                                {
                                    szTemp.append(CvWString::format(L"%c", GC.getYieldInfo(eFixedYield).getChar()));
                                }
                            }
                        }
                    }
                }
            }
		}
		if (bTradeCity)
		{
            for(int iYield=0;iYield<NUM_YIELD_TYPES;iYield++)
            {
                YieldTypes eYield = (YieldTypes) iYield;
                if (YieldGroup_City_Billboard(eYield) && pCity->getYieldStored(eYield) > 0)
                {
                    if (aYieldShown[iYield] == 0)
                    {
                        aYieldShown[iYield] = 1;
                        YieldTypes eFixedYield = (YieldTypes) iYield;
                        if (YieldGroup_City_Billboard_Offset_Fix(eYield))
                        {
                            iYield -= 1;
                           eFixedYield = (YieldTypes) iYield;
                        }
                        if (iYield > 0 && eFixedYield != NO_YIELD)
                        {
                            szTemp.append(CvWString::format(L"%c", GC.getYieldInfo(eFixedYield).getChar()));
                        }
                    }

                }
            }

		}

		if(!szTemp.isEmpty())
		{
			szBuffer.append(L" ");
			szBuffer.append(szTemp);
		}
		if (pCity->isVisible(GC.getGameINLINE().getActiveTeam(), true))
		{
			int iDefenseModifier = pCity->getDefenseModifier();
			if (iDefenseModifier != 0)
			{
				szBuffer.append(CvWString::format(L" %c:%s%d%%", gDLL->getSymbolID(HEALTHY_CHAR), ((iDefenseModifier > 0) ? "+" : ""), iDefenseModifier));
			}

		
			if (!pCity->isNative())
			{
				if (pCity->getRebelPercent() > 0)
				{
					szBuffer.append(CvWString::format(L" %c:%d%%", gDLL->getSymbolID(BAD_GOLD_CHAR), pCity->getRebelPercent()));
				}
				if (pCity->getCityType() == CITYTYPE_COMMERCE)
				{
					szBuffer.append(CvWString::format(L" %c", gDLL->getSymbolID(BAD_FOOD_CHAR)));
				}
				else if (pCity->getCityType() == CITYTYPE_MONASTERY)
				{
					szBuffer.append(CvWString::format(L" %c", gDLL->getSymbolID(UNHEALTHY_CHAR)));
				}
				else if (pCity->getCityType() == CITYTYPE_OUTPOST)
				{
					szBuffer.append(CvWString::format(L" %c", gDLL->getSymbolID(BULLET_CHAR)));
				}
			}
		}

        if (bTradeCity)
        {
            szBuffer.append(CvWString::format(L" %c", gDLL->getSymbolID(RELIGION_CHAR)));
        }

        if (pCity->getVassalOwner() != NO_PLAYER)
        {
            szBuffer.append(CvWString::format(L" %c", gDLL->getSymbolID(STRENGTH_CHAR)));
        }

	}
}

void CvGameTextMgr::buildCityBillboardCityNameString( CvWStringBuffer& szBuffer, CvCity* pCity)
{
    //szBuffer.assign(CvWString::format(L" %s %c", pCity->getName(), gDLL->getSymbolID(OCCUPATION_CHAR)));
	szBuffer.assign(pCity->getName());
}
///TKe
void CvGameTextMgr::buildCityBillboardProductionString( CvWStringBuffer& szBuffer, CvCity* pCity)
{
	szBuffer.clear();

	PlayerTypes ePlayer = GC.getGameINLINE().getActivePlayer();
	if (pCity->isNative() && ePlayer != pCity->getOwnerINLINE() && ePlayer != NO_PLAYER)
	{
		UnitClassTypes eUnitClass = pCity->getTeachUnitClass();
		if (eUnitClass != NO_UNITCLASS)
		{
			UnitTypes eUnit = (UnitTypes) GC.getCivilizationInfo(GET_PLAYER(ePlayer).getCivilizationType()).getCivilizationUnits(eUnitClass);
			if (eUnit != NO_UNIT)
			{
				szBuffer.append(GC.getUnitInfo(eUnit).getDescription());
			}
		}
	}
	else if (pCity->getOrderQueueLength() > 0)
	{
		szBuffer.append(pCity->getProductionName());
	}
}


void CvGameTextMgr::buildCityBillboardCitySizeString( CvWStringBuffer& szBuffer, CvCity* pCity, const NiColorA& kColor)
{
#define CAPARAMS(c) (int)((c).r * 255.0f), (int)((c).g * 255.0f), (int)((c).b * 255.0f), (int)((c).a * 255.0f)
	szBuffer.assign(CvWString::format(SETCOLR L"%d" ENDCOLR, CAPARAMS(kColor), pCity->getPopulation()));
#undef CAPARAMS
}

void CvGameTextMgr::setScoreHelp(CvWStringBuffer &szString, PlayerTypes ePlayer)
{
	if (NO_PLAYER != ePlayer)
	{
		CvPlayer& player = GET_PLAYER(ePlayer);

		int iPopScore = 0;
		int iPop = player.getPopScore();
		int iMaxPop = GC.getGameINLINE().getMaxPopulation();
		if (iMaxPop > 0)
		{
			iPopScore = (GC.getXMLval(XML_SCORE_POPULATION_FACTOR) * iPop) / iMaxPop;
		}

		int iLandScore = 0;
		int iLand = player.getLandScore();
		int iMaxLand = GC.getGameINLINE().getMaxLand();
		if (iMaxLand > 0)
		{
			iLandScore = (GC.getXMLval(XML_SCORE_LAND_FACTOR) * iLand) / iMaxLand;
		}

		int iFatherScore = 0;
		int iFather = player.getFatherScore();
		int iMaxFather = GC.getGameINLINE().getMaxFather();
		iFatherScore = (GC.getXMLval(XML_SCORE_FATHER_FACTOR) * iFather) / iMaxFather;

		int iScoreTaxFactor = player.getScoreTaxFactor();
		int iSubTotal = iPopScore + iLandScore + iFatherScore;
		int iTotalScore = iSubTotal * iScoreTaxFactor / 100;

		int iVictoryScore = player.calculateScore(true, true);
		if (iTotalScore == player.calculateScore())
		{
			if (GC.getXMLval(XML_SCORE_POPULATION_FACTOR) > 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_SCORE_BREAKDOWN_POPULATION", iPopScore, iPop, iMaxPop));
			}
			if (GC.getXMLval(XML_SCORE_LAND_FACTOR) > 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_SCORE_BREAKDOWN_LAND", iLandScore, iLand, iMaxLand));
			}
			if (GC.getXMLval(XML_SCORE_LAND_FACTOR) > 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_SCORE_BREAKDOWN_FATHERS", iFatherScore, iFather, iMaxFather));
			}

			if (iScoreTaxFactor < 100)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_SCORE_BREAKDOWN_TAX", iTotalScore - iSubTotal));
			}

			szString.append(NEWLINE);
			szString.append(gDLL->getText("TXT_KEY_SCORE_BREAKDOWN_TOTAL", iTotalScore, iVictoryScore));
		}
	}
}

void CvGameTextMgr::setCitizenHelp(CvWStringBuffer &szString, const CvCity& kCity, const CvUnit& kUnit)
{
	szString.append(kUnit.getName());

	PlayerTypes ePlayer = kCity.getOwnerINLINE();
	if (ePlayer == NO_PLAYER)
	{
		return;
	}

	for (int iI = 0; iI < GC.getNumPromotionInfos(); ++iI)
	{
		if (!GC.getPromotionInfo((PromotionTypes)iI).isGraphicalOnly() && kUnit.isHasRealPromotion((PromotionTypes) iI))
		{
			szString.append(CvWString::format(L"<img=%S size=16></img>", GC.getPromotionInfo((PromotionTypes)iI).getButton()));
		}
	}

	ProfessionTypes eProfession = kUnit.getProfession();
	if (NO_PROFESSION != eProfession)
	{
		CvProfessionInfo& kProfession = GC.getProfessionInfo(eProfession);
		// MultipleYieldsProduced Start by Aymerick 22/01/2010
		for (int i = 0; i < kProfession.getNumYieldsProduced(); i++)
		{
			YieldTypes eProfessionYield = (YieldTypes) kProfession.getYieldsProduced(i);
			if (NO_YIELD != eProfessionYield)
			{
			    ///TKs Med
			    if (GC.getYieldInfo(eProfessionYield).isArmor())
			    {
			        if (kCity.getSelectedArmor() != eProfessionYield)
			        {
                        continue;
			        }
			    }
			    ///TKe
				int iProfessionYieldChar = GC.getYieldInfo(eProfessionYield).getChar();

				int iYieldAmount = 0;
				CvPlot* pWorkingPlot = kCity.getPlotWorkedByUnit(&kUnit);
				if (NULL != pWorkingPlot)
				{
					iYieldAmount = pWorkingPlot->calculatePotentialYield(eProfessionYield, &kUnit, false);
				}
				else
				{
					iYieldAmount = kCity.getProfessionOutput(eProfession, &kUnit);
				}

				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_MISC_HELP_BASE_CITIZEN_YIELD", iYieldAmount, iProfessionYieldChar, kUnit.getNameKey()));

				int iModifier = setCityYieldModifierString(szString, eProfessionYield, kCity);

				int iTotalYieldTimes100 = iModifier * iYieldAmount;
				if (iTotalYieldTimes100 > 0)
				{
					szString.append(SEPARATOR);
					szString.append(NEWLINE);
					CvWString szNumber = CvWString::format(L"%d.%02d", iTotalYieldTimes100 / 100, iTotalYieldTimes100 % 100);
					szString.append(gDLL->getText("TXT_KEY_YIELD_TOTAL_FLOAT", GC.getYieldInfo(eProfessionYield).getTextKeyWide(), szNumber.GetCString(), iProfessionYieldChar));
					szString.append(SEPARATOR);
				}
				///TKs Med
                UnitClassTypes eEducationClass = (UnitClassTypes)GC.getUnitInfo(kUnit.getUnitType()).getEducationUnitClass();
                UnitClassTypes eRehibilitationClass = (UnitClassTypes)GC.getUnitInfo(kUnit.getUnitType()).getRehibilitateUnitClass();
                UnitClassTypes eLaborForceClass = (UnitClassTypes)GC.getUnitInfo(kUnit.getUnitType()).getLaborForceUnitClass();
                bool bSkipTuition = false;
                if (eEducationClass != NO_UNITCLASS || eRehibilitationClass != NO_UNITCLASS || eLaborForceClass != NO_UNITCLASS)
                {
                    bSkipTuition = true;
                }
                ///TKs Med Update 1.1g
                int iEducationThreshold = kCity.educationThreshold();
                if (bSkipTuition)
                {
                    iEducationThreshold = GC.getXMLval(XML_EDUCATION_THRESHOLD);
                }
                ///TKe Update 1.1g
                ///TKs Med Update 1.1c
				if (eProfessionYield == YIELD_EDUCATION && kUnit.getUnitInfo().getStudentWeight() > 0 && GC.getUnitInfo(kUnit.getUnitType()).getKnightDubbingWeight() == 0)
				{
					int iEducationProduced = kCity.calculateNetYield(YIELD_EDUCATION);

					if (iEducationProduced > 0)
					{
					    ///TKs Med Update 1.1g
                        int iEducationNeeded = iEducationThreshold - kUnit.getYieldStored();
                        ///TKe Update
                        int iStudentOutput = 0;
                        if (eLaborForceClass != NO_UNITCLASS)
                        {
                            iStudentOutput = (kUnit.getUnitInfo().getStudentWeight() / 200 * kCity.getBaseYieldRateModifier(YIELD_EDUCATION) / 100);
                        }
                        else
                        {
                            iStudentOutput = kCity.getProfessionOutput(kUnit.getProfession(), &kUnit, NULL) * kCity.getBaseYieldRateModifier(YIELD_EDUCATION) / 100;
                        }
                        iStudentOutput = std::max(iStudentOutput, 1);

                        int iTurns = std::max(0, (iEducationNeeded + iStudentOutput - 1) / iStudentOutput);  // round up
                        //szString.append(SEPARATOR);
                        szString.append(NEWLINE);
                        if (eEducationClass == NO_UNITCLASS)
                        {
                            szString.append(gDLL->getText("TXT_KEY_MISC_HELP_STUDENT", iTurns));
                        }
                        else
                        {
                            szString.append(gDLL->getText("TXT_KEY_MISC_HELP_STUDENT_EDUCATION_CLASS", GC.getUnitClassInfo(eEducationClass).getDescription(), iTurns));
                        }
					}
                    if (!bSkipTuition)
                    {
                        for (int iUnit = 0; iUnit < GC.getNumUnitInfos(); ++iUnit)
                        {
                            int iPrice = kCity.getSpecialistTuition((UnitTypes) iUnit);
                            if (iPrice >= 0)
                            {
                                szString.append(NEWLINE);
                                szString.append(gDLL->getText("TXT_KEY_MISC_HELP_GRADUATION_CHANCE", GC.getUnitInfo((UnitTypes) iUnit).getTextKeyWide(), iPrice));
                            }
                        }
                    }
					///TKe Update
				}
				else if (eRehibilitationClass != NO_UNITCLASS)
				{
				    int iStudentOutput = 0;
				    for (int i = 0; i < kCity.getPopulation(); ++i)
                    {
                        CvUnit* pUnit = kCity.getPopulationUnitByIndex(i);
                        if (NULL != pUnit)
                        {
                            ProfessionTypes eProfession = pUnit->getProfession();
                            if (NO_PROFESSION != eProfession)
                            {
                                if (GC.getProfessionInfo(eProfession).getSpecialBuilding() == (SpecialBuildingTypes)GC.getXMLval(XML_DEFAULT_SPECIALBUILDING_COURTHOUSE))
                                {
                                    iStudentOutput += kCity.getProfessionOutput(eProfession, pUnit, NULL) * kCity.getBaseYieldRateModifier(YIELD_CULTURE) / 100;
                                }

                            }
                        }
                    }
                    if (iStudentOutput > 0)
                    {
                        ///TKs Med Update 1.1g
                        int iEducationNeeded = iEducationThreshold - kUnit.getYieldStored();
                        ///TKe UPdate
                        iStudentOutput = std::max(iStudentOutput, 1);
                        int iTurns = std::max(0, (iEducationNeeded + iStudentOutput - 1) / iStudentOutput);
                          // round up
                        //szString.append(SEPARATOR);
                        szString.append(NEWLINE);
                        if (iTurns > 0)
                        {
                            szString.append(gDLL->getText("TXT_KEY_MISC_HELP_STUDENT_EDUCATION_CLASS", GC.getUnitClassInfo(eRehibilitationClass).getDescription(), iTurns));

                        }
                    }
				}
				else if (eLaborForceClass != NO_UNITCLASS)
				{
					int iStudentOutput = 0;
//					CvPlot* pWorkingPlot = kCity.getPlotWorkedByUnit(&kUnit);
//					if (NULL != pWorkingPlot)
//					{
//						iStudentOutput = pWorkingPlot->calculatePotentialYield(eProfessionYield, &kUnit, false);
//					}
//					else
//					{
//					    iStudentOutput = kCity.getProfessionOutput(eProfession, &kUnit);
//					}
                   iStudentOutput = (kUnit.getUnitInfo().getStudentWeight() / 200 * kCity.getBaseYieldRateModifier(YIELD_EDUCATION) / 100);
				   if (iStudentOutput > 0)
				   {
					   //int iStudentOutput = (kUnit.getUnitInfo().getStudentWeight() / 200 * kCity.getBaseYieldRateModifier(YIELD_EDUCATION) / 100);
					   ///Tks Med Update 1.1g
					   int iEducationNeeded = iEducationThreshold - kUnit.getYieldStored();
					   ///TKe Update
						iStudentOutput = std::max(iStudentOutput, 1);
						int iTurns = std::max(0, (iEducationNeeded + iStudentOutput - 1) / iStudentOutput);  // round up
						//szString.append(SEPARATOR);
						szString.append(NEWLINE);
						szString.append(gDLL->getText("TXT_KEY_MISC_HELP_STUDENT_EDUCATION_CLASS", GC.getUnitClassInfo(eLaborForceClass).getDescription(), iTurns));
				   }

				}
				else if (eEducationClass != NO_UNITCLASS && GC.getUnitInfo(kUnit.getUnitType()).getKnightDubbingWeight() > 0 && eProfessionYield == YIELD_BELLS)
				{
                   int iStudentOutput = kCity.getProfessionOutput(kUnit.getProfession(), &kUnit, NULL) * kCity.getBaseYieldRateModifier(YIELD_BELLS) / 100;
                   if (iStudentOutput > 0)
				   {
				       ///TKs Med Update 1.1g
                       int iEducationNeeded = iEducationThreshold - kUnit.getYieldStored();
                       ///TKe Update
                        //int iStudentOutput = getProfessionOutput(eProfession, pLoopUnit, NULL) * getBaseYieldRateModifier(YIELD_BELLS) / 100;
                        iStudentOutput = std::max(iStudentOutput, 1);
                        int iTurns = std::max(0, (iEducationNeeded + iStudentOutput - 1) / iStudentOutput);  // round up
                        //szString.append(SEPARATOR);
                        szString.append(NEWLINE);
                        szString.append(gDLL->getText("TXT_KEY_MISC_HELP_STUDENT_EDUCATION_CLASS", GC.getUnitClassInfo(eEducationClass).getDescription(), iTurns));
				   }
				}
				///TKe
			}
		}
		// MultipleYieldsProduced End
	}

	if ((gDLL->getChtLvl() > 0) && gDLL->shiftKey())
	{
		CvPlayer& kOwner = GET_PLAYER(kCity.getOwnerINLINE());
		szString.append(CvWString::format(L"\nID = %d Count = %d", kUnit.getID(), kOwner.getUnitClassCount(kUnit.getUnitClassType())));
                ///TKs Med
				szString.append(CvWString::format(L"\n Yield Stored = %d", kUnit.getYieldStored()));
				///TKe
		for (int iI = 0; iI < GC.getNumProfessionInfos(); iI++)
		{
			ProfessionTypes eLoopProfession = (ProfessionTypes) iI;
			if (GC.getCivilizationInfo(kCity.getCivilizationType()).isValidProfession(eLoopProfession))
			{
				int iValue = kCity.AI_professionValue(eLoopProfession, &kUnit, GC.getProfessionInfo(eLoopProfession).isWorkPlot() ? kCity.getPlotWorkedByUnit(&kUnit) : NULL, NULL);
				int iViability = GET_PLAYER(kUnit.getOwnerINLINE()).AI_professionSuitability(&kUnit, eLoopProfession, GC.getProfessionInfo(eLoopProfession).isWorkPlot() ? kCity.getPlotWorkedByUnit(&kUnit) : kCity.plot());

				if (iValue > 0)
				{
					// MultipleYieldsProduced Start by Aymerick 22/01/2010**
					int iYieldChar = GC.getYieldInfo((YieldTypes)GC.getProfessionInfo(eLoopProfession).getYieldsProduced(0)).getChar();
					szString.append(CvWString::format(L"\n %s (%c) = %d (V:%d)", GC.getProfessionInfo(eLoopProfession).getDescription(), iYieldChar, iValue, iViability));
					// MultipleYieldsProduced End
				}
				else if (iViability > 0)
				{
					szString.append(CvWString::format(L"\n %s = %d (V:%d)", GC.getProfessionInfo(eLoopProfession).getDescription(), -1, iViability));
				}
			}
		}
	}

	if ((gDLL->getChtLvl() > 0) && gDLL->shiftKey())
	{
		CvUnit* pSelectedUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();

		if ((pSelectedUnit != NULL) && (pSelectedUnit != &kUnit))
		{
			int iValue = kCity.AI_professionValue(kUnit.getProfession(), pSelectedUnit, kCity.getPlotWorkedByUnit(&kUnit), &kUnit);
			szString.append(CvWString::format(L"\n Selected = %d", iValue));
		}
	}
}
///TKs Med
void CvGameTextMgr::setEuropeYieldSoldHelp(CvWStringBuffer &szString, const CvPlayer& kPlayer, YieldTypes eYield, int iAmount, int iCommission, EuropeTypes eTradeScreen)
{
	FAssert(kPlayer.getParent() != NO_PLAYER);
	CvPlayer& kPlayerEurope = GET_PLAYER(kPlayer.getParent());
	int iGross = iAmount;
	if (eYield != NO_YIELD)
	{
		iGross *= kPlayerEurope.getYieldBuyPrice(eYield, eTradeScreen);
		szString.append(gDLL->getText("TXT_KEY_YIELD_SOLD", iAmount, GC.getYieldInfo(eYield).getChar(), kPlayerEurope.getYieldBuyPrice(eYield, eTradeScreen), iGross));
	}
	else
	{
		szString.append(gDLL->getText("TXT_KEY_TREASURE_DELIVERED", iGross));
	}
    int iTotalGross = iGross;
	if (iCommission != 0)
	{
		int iCommissionGold = iGross * iCommission / 100;
		iGross -= iCommissionGold;
		szString.append(NEWLINE);
		szString.append(gDLL->getText("TXT_KEY_YIELD_COMMISSION", iCommission, iCommissionGold));
	}

	if (kPlayer.getTaxRate() != 0)
	{
		int iTaxGold = iGross * kPlayer.getTaxRate() / 100;
		iGross -= iTaxGold;
		szString.append(NEWLINE);
		szString.append(gDLL->getText("TXT_KEY_YIELD_TAX", kPlayer.getTaxRate(), iTaxGold));
	}

    FAssert(eYield == NO_YIELD || kPlayer.getSellToEuropeProfit(eYield, iAmount * (100 - iCommission) / 100, eTradeScreen) == iGross);
    szString.append(NEWLINE);
    szString.append(gDLL->getText("TXT_KEY_YIELD_NET_PROFIT", iGross));

   /* CivicTypes ePlayerResearch = kPlayer.getCurrentResearch();
    if (ePlayerResearch != NO_CIVIC && iTotalGross >= GC.getXMLval(XML_TRADE_STIMULATES_RESEARCH_MIN_VALUE))
    {
        iTotalGross = iTotalGross * GC.getXMLval(XML_TRADE_STIMULATES_RESEARCH_PERCENT) / 100;
        szString.append(NEWLINE);
        szString.append(gDLL->getText("TXT_KEY_YIELD_SOLD_RESEARCH_STIMULATED", iTotalGross));
    }*/
}

void CvGameTextMgr::setEuropeYieldBoughtHelp(CvWStringBuffer &szString, const CvPlayer& kPlayer, YieldTypes eYield, int iAmount)
{
	FAssert(kPlayer.getParent() != NO_PLAYER);
	CvPlayer& kPlayerEurope = GET_PLAYER(kPlayer.getParent());
	int iGross = kPlayerEurope.getYieldSellPrice(eYield) * iAmount;
//	CivicTypes ePlayerResearch = kPlayer.getCurrentResearch();
//    if (ePlayerResearch != NO_CIVIC && iGross >= GC.getDefineINT("TRADE_STIMULATES_RESEARCH_MIN_VALUE"))
//    {
//       int iExtraResearch = iGross * GC.getDefineINT("TRADE_STIMULATES_RESEARCH_PERCENT") / 100;
//       szString.append(gDLL->getText("TXT_KEY_YIELD_BOUGHT_RESEARCH_STIMULATED", iAmount, GC.getYieldInfo(eYield).getChar(), kPlayerEurope.getYieldSellPrice(eYield), iGross, iExtraResearch));
//    }
//    else
//    {
        szString.append(gDLL->getText("TXT_KEY_YIELD_BOUGHT", iAmount, GC.getYieldInfo(eYield).getChar(), kPlayerEurope.getYieldSellPrice(eYield), iGross));
    //}
}
///TKe
void CvGameTextMgr::setEventHelp(CvWStringBuffer& szBuffer, EventTypes eEvent, int iEventTriggeredId, PlayerTypes ePlayer)
{
	if (NO_EVENT == eEvent || NO_PLAYER == ePlayer)
	{
		return;
	}

	CvEventInfo& kEvent = GC.getEventInfo(eEvent);
	CvPlayer& kActivePlayer = GET_PLAYER(ePlayer);
	EventTriggeredData* pTriggeredData = kActivePlayer.getEventTriggered(iEventTriggeredId);

	if (NULL == pTriggeredData)
	{
		return;
	}

	CvCity* pCity = kActivePlayer.getCity(pTriggeredData->m_iCityId);
	CvCity* pOtherPlayerCity = NULL;
	CvPlot* pPlot = GC.getMapINLINE().plot(pTriggeredData->m_iPlotX, pTriggeredData->m_iPlotY);
	CvUnit* pUnit = kActivePlayer.getUnit(pTriggeredData->m_iUnitId);

	if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
	{
		pOtherPlayerCity = GET_PLAYER(pTriggeredData->m_eOtherPlayer).getCity(pTriggeredData->m_iOtherPlayerCityId);
	}

	CvWString szCity = gDLL->getText("TXT_KEY_EVENT_THE_CITY");
	if (NULL != pCity && kEvent.isCityEffect())
	{
		szCity = pCity->getNameKey();
	}
	else if (NULL != pOtherPlayerCity && kEvent.isOtherPlayerCityEffect())
	{
		szCity = pOtherPlayerCity->getNameKey();
	}

	CvWString szUnit = gDLL->getText("TXT_KEY_EVENT_THE_UNIT");
	if (NULL != pUnit)
	{
		szUnit = pUnit->getNameOrProfessionKey();
	}

	eventGoldHelp(szBuffer, eEvent, ePlayer, pTriggeredData->m_eOtherPlayer);

	if (kEvent.getFood() != 0)
	{
		if (kEvent.isCityEffect() || kEvent.isOtherPlayerCityEffect())
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_FOOD_CITY", kEvent.getFood(), szCity.GetCString()));
		}
		else
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_FOOD", kEvent.getFood()));
		}
	}

	if (kEvent.getFoodPercent() != 0)
	{
		if (kEvent.isCityEffect() || kEvent.isOtherPlayerCityEffect())
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_FOOD_PERCENT_CITY", kEvent.getFoodPercent(), szCity.GetCString()));
		}
		else
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_FOOD_PERCENT", kEvent.getFoodPercent()));
		}
	}

	if (kEvent.getRevoltTurns() > 0)
	{
		if (kEvent.isCityEffect() || kEvent.isOtherPlayerCityEffect())
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_REVOLT_TURNS", kEvent.getRevoltTurns(), szCity.GetCString()));
		}
	}

	if (kEvent.getMaxPillage() > 0)
	{
		if (kEvent.isCityEffect() || kEvent.isOtherPlayerCityEffect())
		{
			if (kEvent.getMaxPillage() == kEvent.getMinPillage())
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_EVENT_PILLAGE_CITY", kEvent.getMinPillage(), szCity.GetCString()));
			}
			else
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_EVENT_PILLAGE_RANGE_CITY", kEvent.getMinPillage(), kEvent.getMaxPillage(), szCity.GetCString()));
			}
		}
		else
		{
			if (kEvent.getMaxPillage() == kEvent.getMinPillage())
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_EVENT_PILLAGE", kEvent.getMinPillage()));
			}
			else
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_EVENT_PILLAGE_RANGE", kEvent.getMinPillage(), kEvent.getMaxPillage()));
			}
		}
	}

	if (kEvent.getPopulationChange() != 0)
	{
		if (kEvent.isCityEffect() || kEvent.isOtherPlayerCityEffect())
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_POPULATION_CHANGE_CITY", kEvent.getPopulationChange(), szCity.GetCString()));
		}
		else
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_POPULATION_CHANGE", kEvent.getPopulationChange()));
		}
	}

	if (kEvent.getCulture() != 0)
	{
		if (kEvent.isCityEffect() || kEvent.isOtherPlayerCityEffect())
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_CULTURE_CITY", kEvent.getCulture(), szCity.GetCString()));
		}
		else
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_CULTURE", kEvent.getCulture()));
		}
	}

	if (kEvent.getUnitClass() != NO_UNITCLASS)
	{
		CivilizationTypes eCiv = kActivePlayer.getCivilizationType();
		if (NO_CIVILIZATION != eCiv)
		{
			UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(eCiv).getCivilizationUnits(kEvent.getUnitClass());
			if (eUnit != NO_UNIT)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_EVENT_BONUS_UNIT", kEvent.getNumUnits(), GC.getUnitInfo(eUnit).getTextKeyWide()));
			}
		}
	}

	if (kEvent.getBuildingClass() != NO_BUILDINGCLASS)
	{
		CivilizationTypes eCiv = kActivePlayer.getCivilizationType();
		if (NO_CIVILIZATION != eCiv)
		{
			BuildingTypes eBuilding = (BuildingTypes)GC.getCivilizationInfo(eCiv).getCivilizationBuildings(kEvent.getBuildingClass());
			if (eBuilding != NO_BUILDING)
			{
				if (kEvent.getBuildingChange() > 0)
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_EVENT_BONUS_BUILDING", GC.getBuildingInfo(eBuilding).getTextKeyWide()));
				}
				else if (kEvent.getBuildingChange() < 0)
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_EVENT_REMOVE_BUILDING", GC.getBuildingInfo(eBuilding).getTextKeyWide()));
				}
			}
		}
	}

	if (kEvent.getNumBuildingYieldChanges() > 0)
	{
		CvWStringBuffer szYield;
		for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); ++iBuildingClass)
		{
			CivilizationTypes eCiv = kActivePlayer.getCivilizationType();
			if (NO_CIVILIZATION != eCiv)
			{
				BuildingTypes eBuilding = (BuildingTypes)GC.getCivilizationInfo(eCiv).getCivilizationBuildings(iBuildingClass);
				if (eBuilding != NO_BUILDING)
				{
					int aiYields[NUM_YIELD_TYPES];
					for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
					{
						aiYields[iYield] = kEvent.getBuildingYieldChange(iBuildingClass, iYield);
					}

					szYield.clear();
					setYieldChangeHelp(szYield, L"", L"", L"", aiYields, false, false);
					if (!szYield.isEmpty())
					{
						szBuffer.append(NEWLINE);
						szBuffer.append(gDLL->getText("TXT_KEY_EVENT_YIELD_CHANGE_BUILDING", GC.getBuildingInfo(eBuilding).getTextKeyWide(), szYield.getCString()));
					}
				}
			}
		}
	}

	if (kEvent.getFeatureChange() > 0)
	{
		if (kEvent.getFeature() != NO_FEATURE)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_FEATURE_GROWTH", GC.getFeatureInfo((FeatureTypes)kEvent.getFeature()).getTextKeyWide()));
		}
	}
	else if (kEvent.getFeatureChange() < 0)
	{
		if (NULL != pPlot && NO_FEATURE != pPlot->getFeatureType())
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_FEATURE_REMOVE", GC.getFeatureInfo(pPlot->getFeatureType()).getTextKeyWide()));
		}
	}

	if (kEvent.getImprovementChange() > 0)
	{
		if (kEvent.getImprovement() != NO_IMPROVEMENT)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_IMPROVEMENT_GROWTH", GC.getImprovementInfo((ImprovementTypes)kEvent.getImprovement()).getTextKeyWide()));
		}
	}
	else if (kEvent.getImprovementChange() < 0)
	{
		if (NULL != pPlot && NO_IMPROVEMENT != pPlot->getImprovementType())
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_IMPROVEMENT_REMOVE", GC.getImprovementInfo(pPlot->getImprovementType()).getTextKeyWide()));
		}
	}

	if (kEvent.getRouteChange() > 0)
	{
		if (kEvent.getRoute() != NO_ROUTE)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_ROUTE_GROWTH", GC.getRouteInfo((RouteTypes)kEvent.getRoute()).getTextKeyWide()));
		}
	}
	else if (kEvent.getRouteChange() < 0)
	{
		if (NULL != pPlot && NO_ROUTE != pPlot->getRouteType())
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_ROUTE_REMOVE", GC.getRouteInfo(pPlot->getRouteType()).getTextKeyWide()));
		}
	}

	int aiYields[NUM_YIELD_TYPES];
	for (int i = 0; i < NUM_YIELD_TYPES; ++i)
	{
		aiYields[i] = kEvent.getPlotExtraYield(i);
	}

	CvWStringBuffer szYield;
	setYieldChangeHelp(szYield, L"", L"", L"", aiYields, false, false);
	if (!szYield.isEmpty())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_EVENT_YIELD_CHANGE_PLOT", szYield.getCString()));
	}

	if (NO_BONUS != kEvent.getBonusRevealed())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_EVENT_BONUS_REVEALED", GC.getBonusInfo((BonusTypes)kEvent.getBonusRevealed()).getTextKeyWide()));
	}

	if (0 != kEvent.getUnitExperience())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_EVENT_UNIT_EXPERIENCE", kEvent.getUnitExperience(), szUnit.GetCString()));
	}

	if (0 != kEvent.isDisbandUnit())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_EVENT_UNIT_DISBAND", szUnit.GetCString()));
	}

	if (NO_PROMOTION != kEvent.getUnitPromotion())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_EVENT_UNIT_PROMOTION", szUnit.GetCString(), GC.getPromotionInfo((PromotionTypes)kEvent.getUnitPromotion()).getTextKeyWide()));
	}

	for (int i = 0; i < GC.getNumUnitCombatInfos(); ++i)
	{
		if (NO_PROMOTION != kEvent.getUnitCombatPromotion(i))
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_UNIT_COMBAT_PROMOTION", GC.getUnitCombatInfo((UnitCombatTypes)i).getTextKeyWide(), GC.getPromotionInfo((PromotionTypes)kEvent.getUnitCombatPromotion(i)).getTextKeyWide()));
		}
	}

	for (int i = 0; i < GC.getNumUnitClassInfos(); ++i)
	{
		if (NO_PROMOTION != kEvent.getUnitClassPromotion(i))
		{
			UnitTypes ePromotedUnit = ((UnitTypes)(GC.getCivilizationInfo(kActivePlayer.getCivilizationType()).getCivilizationUnits(i)));
			if (NO_UNIT != ePromotedUnit)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_EVENT_UNIT_CLASS_PROMOTION", GC.getUnitInfo(ePromotedUnit).getTextKeyWide(), GC.getPromotionInfo((PromotionTypes)kEvent.getUnitClassPromotion(i)).getTextKeyWide()));
			}
		}
	}

	if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
	{
		if (kEvent.getAttitudeModifier() > 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_ATTITUDE_GOOD", kEvent.getAttitudeModifier(), GET_PLAYER(pTriggeredData->m_eOtherPlayer).getNameKey()));
		}
		else if (kEvent.getAttitudeModifier() < 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_ATTITUDE_BAD", kEvent.getAttitudeModifier(), GET_PLAYER(pTriggeredData->m_eOtherPlayer).getNameKey()));
		}
	}

	if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
	{
		TeamTypes eWorstEnemy = GET_TEAM(GET_PLAYER(pTriggeredData->m_eOtherPlayer).getTeam()).AI_getWorstEnemy();
		if (NO_TEAM != eWorstEnemy)
		{
			if (kEvent.getTheirEnemyAttitudeModifier() > 0)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_EVENT_ATTITUDE_GOOD", kEvent.getTheirEnemyAttitudeModifier(), GET_TEAM(eWorstEnemy).getName().GetCString()));
			}
			else if (kEvent.getTheirEnemyAttitudeModifier() < 0)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_EVENT_ATTITUDE_BAD", kEvent.getTheirEnemyAttitudeModifier(), GET_TEAM(eWorstEnemy).getName().GetCString()));
			}
		}
	}

	if (kEvent.isDeclareWar())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_EVENT_DECLARE_WAR", GET_PLAYER(pTriggeredData->m_eOtherPlayer).getCivilizationAdjectiveKey()));
	}

	if (kEvent.getUnitImmobileTurns() > 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_EVENT_IMMOBILE_UNIT", kEvent.getUnitImmobileTurns(), szUnit.GetCString()));
	}

	if (!isEmpty(kEvent.getPythonHelp()))
	{
		CvWString szHelp;
		CyArgsList argsList;
		argsList.add(eEvent);
		argsList.add(gDLL->getPythonIFace()->makePythonObject(pTriggeredData));

		gDLL->getPythonIFace()->callFunction(PYRandomEventModule, kEvent.getPythonHelp(), argsList.makeFunctionArgs(), &szHelp);

		szBuffer.append(NEWLINE);
		szBuffer.append(szHelp);
	}

	CvWStringBuffer szTemp;
	for (int i = 0; i < GC.getNumEventInfos(); ++i)
	{
		if (0 == kEvent.getAdditionalEventTime(i))
		{
			if (kEvent.getAdditionalEventChance(i) > 0)
			{
				if (GET_PLAYER(GC.getGameINLINE().getActivePlayer()).canDoEvent((EventTypes)i, *pTriggeredData))
				{
					szTemp.clear();
					setEventHelp(szTemp, (EventTypes)i, iEventTriggeredId, ePlayer);

					if (!szTemp.isEmpty())
					{
						szBuffer.append(NEWLINE);
						szBuffer.append(NEWLINE);
						szBuffer.append(gDLL->getText("TXT_KEY_EVENT_ADDITIONAL_CHANCE", kEvent.getAdditionalEventChance(i), L""));
						szBuffer.append(NEWLINE);
						szBuffer.append(szTemp);
					}
				}
			}
		}
		else
		{
			szTemp.clear();
			setEventHelp(szTemp, (EventTypes)i, iEventTriggeredId, ePlayer);

			if (!szTemp.isEmpty())
			{
				CvWString szDelay = gDLL->getText("TXT_KEY_EVENT_DELAY_TURNS", (GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent() * kEvent.getAdditionalEventTime((EventTypes)i)) / 100);

				if (kEvent.getAdditionalEventChance(i) > 0)
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_EVENT_ADDITIONAL_CHANCE", kEvent.getAdditionalEventChance(i), szDelay.GetCString()));
				}
				else
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_EVENT_DELAY", szDelay.GetCString()));
				}

				szBuffer.append(NEWLINE);
				szBuffer.append(szTemp);
			}
		}
	}
	bool done = false;
	while(!done)
	{
		done = true;
		if(!szBuffer.isEmpty())
		{
			const wchar* wideChar = szBuffer.getCString();
			if(wideChar[0] == L'\n')
			{
				CvWString tempString(&wideChar[1]);
				szBuffer.clear();
				szBuffer.append(tempString);
				done = false;
			}
		}
	}
}

void CvGameTextMgr::eventGoldHelp(CvWStringBuffer& szBuffer, EventTypes eEvent, PlayerTypes ePlayer, PlayerTypes eOtherPlayer)
{
	CvEventInfo& kEvent = GC.getEventInfo(eEvent);
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	int iGold1 = kPlayer.getEventCost(eEvent, eOtherPlayer, false);
	int iGold2 = kPlayer.getEventCost(eEvent, eOtherPlayer, true);

	if (0 != iGold1 || 0 != iGold2)
	{
		if (iGold1 == iGold2)
		{
			if (NO_PLAYER != eOtherPlayer && kEvent.isGoldToPlayer())
			{
				if (iGold1 > 0)
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_EVENT_GOLD_FROM_PLAYER", iGold1, GET_PLAYER(eOtherPlayer).getNameKey()));
				}
				else
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_EVENT_GOLD_TO_PLAYER", -iGold1, GET_PLAYER(eOtherPlayer).getNameKey()));
				}
			}
			else
			{
				if (iGold1 > 0)
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_EVENT_GOLD_GAINED", iGold1));
				}
				else
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_EVENT_GOLD_LOST", -iGold1));
				}
			}
		}
		else
		{
			if (NO_PLAYER != eOtherPlayer && kEvent.isGoldToPlayer())
			{
				if (iGold1 > 0)
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_EVENT_GOLD_RANGE_FROM_PLAYER", iGold1, iGold2, GET_PLAYER(eOtherPlayer).getNameKey()));
				}
				else
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_EVENT_GOLD_RANGE_TO_PLAYER", -iGold1, -iGold2, GET_PLAYER(eOtherPlayer).getNameKey()));
				}
			}
			else
			{
				if (iGold1 > 0)
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_EVENT_GOLD_RANGE_GAINED", iGold1, iGold2));
				}
				else
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_EVENT_GOLD_RANGE_LOST", -iGold1, iGold2));
				}
			}
		}
	}
}

void CvGameTextMgr::setFatherHelp(CvWStringBuffer &szBuffer, FatherTypes eFather, bool bCivilopediaText)
{
	CvWString szTempBuffer;
	CvFatherInfo& kFatherInfo = GC.getFatherInfo(eFather);
	PlayerTypes ePlayer = GC.getGameINLINE().getActivePlayer();
	TeamTypes eTeam = GC.getGameINLINE().getActiveTeam();

	if (!bCivilopediaText)
	{
		szTempBuffer.Format(SETCOLR L"%s" ENDCOLR , TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), kFatherInfo.getDescription());
		szBuffer.append(szTempBuffer);

		FatherCategoryTypes eCategory = (FatherCategoryTypes) kFatherInfo.getFatherCategory();
		if (eCategory != NO_FATHERCATEGORY)
		{
			szTempBuffer.Format(SETCOLR L" (%s)" ENDCOLR , TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), GC.getFatherCategoryInfo(eCategory).getDescription());
			szBuffer.append(szTempBuffer);
		}

		szTempBuffer.clear();

		for(int i=0;i<GC.getNumFatherPointInfos();i++)
		{
			FatherPointTypes ePointType = (FatherPointTypes) i;
			if (kFatherInfo.getPointCost(ePointType) > 0)
			{
				if (!szTempBuffer.empty())
				{
					szTempBuffer += L", ";
				}
				else
				{
					szTempBuffer += L"\n";
				}

				szTempBuffer += CvWString::format(L"%d%c", (eTeam != NO_TEAM ? GET_TEAM(eTeam).getFatherPointCost(eFather, ePointType) : kFatherInfo.getPointCost(ePointType)), GC.getFatherPointInfo(ePointType).getChar());
			}
		}

		szBuffer.append(szTempBuffer);
	}

	for (int iImprovement = 0; iImprovement < GC.getNumImprovementInfos(); ++iImprovement)
	{
		if (kFatherInfo.isRevealImprovement(iImprovement))
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_FATHER_REVEALS_IMPROVEMENT", GC.getImprovementInfo((ImprovementTypes) iImprovement).getTextKeyWide()));
		}
	}

	for (int iUnitClass = 0; iUnitClass < GC.getNumUnitClassInfos(); ++iUnitClass)
	{
		UnitTypes eUnit = (UnitTypes) GC.getUnitClassInfo((UnitClassTypes) iUnitClass).getDefaultUnitIndex();

		if (ePlayer != NO_PLAYER)
		{
			eUnit = (UnitTypes) GC.getCivilizationInfo(GET_PLAYER(ePlayer).getCivilizationType()).getCivilizationUnits(iUnitClass);
		}

		if (eUnit != NO_UNIT)
		{
			if (kFatherInfo.getFreeUnits(iUnitClass) > 0)
			{
				szTempBuffer = gDLL->getText("TXT_KEY_FATHER_FREE_UNITS", kFatherInfo.getFreeUnits(iUnitClass), GC.getUnitInfo(eUnit).getTextKeyWide());
				szBuffer.append(NEWLINE);
				szBuffer.append(szTempBuffer);
			}
		}
	}

	if (kFatherInfo.getTrait() != NO_TRAIT)
	{
		CivilizationTypes eCivilization = NO_CIVILIZATION;
		if (ePlayer != NO_PLAYER)
		{
			eCivilization = GET_PLAYER(ePlayer).getCivilizationType();
		}

		parseTraits(szBuffer, (TraitTypes) kFatherInfo.getTrait(), eCivilization, false, false);

	}

	///TK Med
    if (kFatherInfo.getCivic() > 0)
    {
        CivilizationTypes eCivilization = NO_CIVILIZATION;
		if (ePlayer != NO_PLAYER)
		{
			eCivilization = GET_PLAYER(ePlayer).getCivilizationType();
		}
		CvWStringBuffer szTradeBuffer;
        GAMETEXT.parseCivicInfo(szTradeBuffer, (CivicTypes) kFatherInfo.getCivic(), false, false, true, false, eCivilization);
        szBuffer.append(szTradeBuffer);
    }
    ///TKe

}

void CvGameTextMgr::getTradeScreenTitleIcon(CvString& szButton, CvWidgetDataStruct& widgetData, PlayerTypes ePlayer)
{
	szButton = GC.getCivilizationInfo(GET_PLAYER(ePlayer).getCivilizationType()).getButton();
}

void CvGameTextMgr::getTradeScreenIcons(std::vector< std::pair<CvString, CvWidgetDataStruct> >& aIconInfos, PlayerTypes ePlayer)
{
	aIconInfos.clear();
}

void CvGameTextMgr::getTradeScreenHeader(CvWString& szHeader, PlayerTypes ePlayer, PlayerTypes eOtherPlayer, bool bAttitude, CvCity* pCity)
{
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	if (pCity == NULL || !kPlayer.isNative())
	{
		szHeader.Format(L"%s - %s", kPlayer.getName(), kPlayer.getCivilizationDescription());
	}
	else
	{
		szHeader = gDLL->getText("TXT_KEY_TRADE_SCREEN_VILLAGE_CHIEF", pCity->getNameKey(), kPlayer.getCivilizationDescriptionKey());
	}

	if (bAttitude)
	{
		szHeader += CvWString::format(L" (%s)", GC.getAttitudeInfo(kPlayer.AI_getAttitude(eOtherPlayer)).getDescription());
	}
}

void CvGameTextMgr::setResourceLayerInfo(ResourceLayerOptions eOption, CvWString& szName, CvString& szButton)
{
	switch (eOption)
	{
	case RESOURCE_LAYER_NATIVE_TRADE:
		szName = gDLL->getText("TXT_KEY_GLOBELAYER_RESOURCES_NATIVE_TRADE");
		szButton = "XXX";
		break;
	case RESOURCE_LAYER_NATIVE_TRAIN:
		szName = gDLL->getText("TXT_KEY_GLOBELAYER_RESOURCES_NATIVE_TRAIN");
		szButton = "XXX";
		break;
	case RESOURCE_LAYER_RESOURCES:
		szName = gDLL->getText("TXT_KEY_GLOBELAYER_RESOURCES");
		szButton = "XXX";
		break;
	default:
		FAssertMsg(false, "Invalid option");
		break;
	}
}

void CvGameTextMgr::setUnitLayerInfo(UnitLayerOptionTypes eOption, CvWString& szName, CvString& szButton)
{
	switch (eOption)
	{
	case SHOW_ALL_MILITARY:
		szName = gDLL->getText("TXT_KEY_GLOBELAYER_UNITS_ALLMILITARY");
		szButton = "XXX";
		break;
	case SHOW_TEAM_MILITARY:
		szName = gDLL->getText("TXT_KEY_GLOBELAYER_UNITS_TEAMMILITARY");
		szButton = "XXX";
		break;
	case SHOW_ENEMIES_IN_TERRITORY:
		szName = gDLL->getText("TXT_KEY_GLOBELAYER_UNITS_ENEMY_TERRITORY_MILITARY");
		szButton = "XXX";
		break;
	case SHOW_ENEMIES:
		szName = gDLL->getText("TXT_KEY_GLOBELAYER_UNITS_ENEMYMILITARY");
		szButton = "XXX";
		break;
	case SHOW_PLAYER_DOMESTICS:
		szName = gDLL->getText("TXT_KEY_GLOBELAYER_UNITS_DOMESTICS");
		szButton = "XXX";
		break;
	default:
		FAssertMsg(false, "Invalid option");
		break;
	}
}

