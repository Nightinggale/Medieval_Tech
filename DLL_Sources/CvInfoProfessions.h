#pragma once

#ifndef CV_INFO_PROFESSION_H
#define CV_INFO_PROFESSION_H

#include "CvInfos.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvProfessionInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/// info subclass - start - Nightinggale
enum PROFESSION_INFO_BM 
{
	PROFESSION_INFO_BM_PARENT_START             = 0,
	PROFESSION_INFO_BM_PARENT_NUM_BITS          = 9,
	PROFESSION_INFO_BM_NUM_CHILDREN_START       = PROFESSION_INFO_BM_PARENT_START + PROFESSION_INFO_BM_PARENT_NUM_BITS,
	PROFESSION_INFO_BM_NUM_CHILDREN_NUM_BITS    = 4,
	PROFESSION_INFO_BM_NATIVE_INVALID           = PROFESSION_INFO_BM_NUM_CHILDREN_START + PROFESSION_INFO_BM_NUM_CHILDREN_NUM_BITS,
	PROFESSION_INFO_BM_EUROPE_INVALID,
	PROFESSION_INFO_BM_COLONIAL_INVALID,
	PROFESSION_INFO_BM_WORK_PLOT,
	PROFESSION_INFO_BM_CITIZEN,
	PROFESSION_INFO_BM_WATER,
	PROFESSION_INFO_BM_SCOUT,
	PROFESSION_INFO_BM_CITY_DFENDER,
	PROFESSION_INFO_BM_CAN_FOUND,
	PROFESSION_INFO_BM_UNARMED,
	PROFESSION_INFO_BM_NO_DEFENSIVE_BONUS,

	NUM_PROFESSION_INFO_BM,
};
/// info subclass - end - Nightinggale

class CvProfessionInfo: public CvInfoBase
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvProfessionInfo();

	///TK Professions Pedia

    DllExport bool isNativesInvalid() const;
    DllExport bool isEuropeInvalid() const;
    DllExport bool isColonialInvalid() const;
	DllExport int getArtTagUnitClass() const;
	DllExport int getTaxCollectRate() const;
	DllExport int getExperenceLevel() const;
	///TKe
	DllExport virtual ~CvProfessionInfo();
	DllExport int getUnitCombatType() const;
	DllExport int getDefaultUnitAIType() const;
	DllExport int getSpecialBuilding() const;
	DllExport int getCombatChange() const;
	DllExport int getMovesChange() const;
	DllExport int getWorkRate() const;
	DllExport int getMissionaryRate() const;
	DllExport int getPowerValue() const;
	DllExport int getAssetValue() const;
	DllExport bool isWorkPlot() const;
	DllExport bool isCitizen() const;
	DllExport bool isWater() const;
	DllExport bool isScout() const;
	DllExport bool isCityDefender() const;
	DllExport bool canFound() const;
	DllExport bool isUnarmed() const;
	DllExport bool isNoDefensiveBonus() const;
	DllExport ProfessionYieldCost getYieldEquipmentAmount(int iYield) const;
	DllExport bool isFreePromotion(int i) const;
	///TKs Med BM
	DllExport bool getCombatGearTypes(int i) const;
	DllExport bool hasCombatGearTypes() const; // CombatGearTypes - Nightinggale
	DllExport int getRequiredBuilding() const;
	DllExport int getUpgradeProfession() const;
	DllExport int getLeadUnit() const;
	DllExport int getRequiredPromotion() const;
	DllExport int getCivilizationType() const;
	DllExport int getFoundCityType() const;
	DllExport int getNumCaptureCargoTypes() const;
	DllExport int getCaptureCargoTypes(int index) const;
	DllExport int getCaptureCargoTypeAmount(int index) const;
	///TKe
	// MultipleYieldsProduced Start by Aymerick 22/01/2010
	DllExport int getYieldsProduced(int i) const;
	DllExport int getNumYieldsProduced() const;
	// MultipleYieldsProduced End
	// MultipleYieldsConsumed Start by Aymerick 05/01/2010
	///TKs Invention Core Mod v 1.0
	DllExport int getYieldsConsumedPedia(int i) const;
	DllExport int getNumYieldsConsumedPedia() const;
	DllExport int getYieldsConsumed(int i, PlayerTypes eCurrentPlayer=NO_PLAYER) const;
	///TKe
	DllExport int getNumYieldsConsumed(PlayerTypes eCurrentPlayer=NO_PLAYER) const;
	// MultipleYieldsConsumed End
	DllExport void read(FDataStreamBase* stream);
	DllExport void write(FDataStreamBase* stream);
	DllExport bool read(CvXMLLoadUtility* pXML);
	///TKs Med
    //DllExport bool readPass2(CvXMLLoadUtility* pXML);
    ///Tke
	//DllExport bool readPass3();

	/// info subclass - start - Nightinggale
	bool readSub(CvXMLLoadUtility* pXML, int* pSub);
	TCHAR* getSubTag() { return "ProfessionSubTypes";};

	bool isParent() const;
	bool isSubType() const;
	ProfessionTypes getParent() const;
	int getNumSubTypes() const;
	/// info subclass - end - Nightinggale

	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	int m_iUnitCombatType;
	int m_iDefaultUnitAIType;

	/// info subclass - start - Nightinggale
	unsigned int m_bfA;
	/// info subclass - end - Nightinggale

	///TK Professions Pedia
	//bool m_ibNativesInvalid;
	//bool m_ibEuropeInvalid;
	//bool m_ibColonialInvalid;
	int m_iArtTagUnit;
	int m_iTaxCollectRate;
	int m_iExperenceLevel;
	///TKe

	int m_iYieldProduced;
	int m_iYieldConsumed;
	int m_iSpecialBuilding;
	int m_iCombatChange;
	int m_iMovesChange;
	int m_iWorkRate;
	int m_iMissionaryRate;
	int m_iPowerValue;
	int m_iAssetValue;
	//bool m_bWorkPlot;
	/*bool m_bCitizen;
	bool m_bWater;
	bool m_bScout;
	bool m_bCityDefender;
	bool m_bCanFound;
	bool m_bUnarmed;
	bool m_bNoDefensiveBonus;*/

	struct YieldEquipment
	{
		int iYieldType;
		int iYieldAmount;
	};

	//std::vector<YieldEquipment> m_aYieldEquipments;
	YieldArray<ProfessionYieldCost> m_acYieldEquipments;
	//bool* m_abFreePromotions;
	BoolArray m_ba_FreePromotions;
	///TKs Med BM
	BoolArray m_ba_CombatGearTypes; // CombatGearTypes - Nightinggale
	int m_iRequiredBuilding;
	int m_iUpgradeProfession;
	int m_iLeadUnit;
	int iRequiredPromotion;
	int iCivType;
	int m_iFoundCityType;
	///TKe
	// MultipleYieldsProduced Start by Aymerick 22/01/2010
	std::vector<int> m_aiYieldsProduced;
	// MultipleYieldsProduced End
	// MultipleYieldsConsumed Start by Aymerick 05/01/2010
	std::vector<int> m_aiYieldsConsumed;
	// MultipleYieldsConsumed End
	std::vector< std::pair<UnitClassTypes, int> > m_aiCaptureCargoTypes; //Tk Civics
};

// CombatGearTypes - start - Nightinggale
inline bool CvProfessionInfo::getCombatGearTypes(int i) const
{
	return m_ba_CombatGearTypes.get(i);
}

inline bool CvProfessionInfo::hasCombatGearTypes() const
{
	return m_ba_CombatGearTypes.isAllocated();
}
// CombatGearTypes - end - Nightinggale

///TKs Professions Pedia
inline int CvProfessionInfo::getArtTagUnitClass() const
{
	return m_iArtTagUnit;
}
inline bool CvProfessionInfo::isNativesInvalid() const
{
	return HasBit(m_bfA, PROFESSION_INFO_BM_NATIVE_INVALID);
}
inline bool CvProfessionInfo::isEuropeInvalid() const
{
	return HasBit(m_bfA, PROFESSION_INFO_BM_EUROPE_INVALID);
}
inline bool CvProfessionInfo::isColonialInvalid() const
{
	return HasBit(m_bfA, PROFESSION_INFO_BM_COLONIAL_INVALID);
}
///TKs Med
inline bool CvProfessionInfo::isWorkPlot() const
{
	return HasBit(m_bfA, PROFESSION_INFO_BM_WORK_PLOT);
}
inline bool CvProfessionInfo::isCitizen() const
{
	return HasBit(m_bfA, PROFESSION_INFO_BM_CITIZEN);
}
inline bool CvProfessionInfo::isWater() const
{
	return HasBit(m_bfA, PROFESSION_INFO_BM_WATER);
}
inline bool CvProfessionInfo::isScout() const
{
	return HasBit(m_bfA, PROFESSION_INFO_BM_SCOUT);
}
inline bool CvProfessionInfo::isCityDefender() const
{
	return HasBit(m_bfA, PROFESSION_INFO_BM_CITY_DFENDER);
}
inline bool CvProfessionInfo::canFound() const
{
	return HasBit(m_bfA, PROFESSION_INFO_BM_CAN_FOUND);
}
inline bool CvProfessionInfo::isUnarmed() const
{
	return HasBit(m_bfA, PROFESSION_INFO_BM_UNARMED);
}
inline bool CvProfessionInfo::isNoDefensiveBonus() const
{
	return HasBit(m_bfA, PROFESSION_INFO_BM_NO_DEFENSIVE_BONUS);
}

/// info subclass - start - Nightinggale
inline bool CvProfessionInfo::isParent() const
{
	return getNumSubTypes() > 0;
}

inline bool CvProfessionInfo::isSubType() const
{
	return getParent() != NO_PROFESSION;
}

inline ProfessionTypes CvProfessionInfo::getParent() const
{
	ProfessionTypes eProfession = (ProfessionTypes)GETBITS(m_bfA, PROFESSION_INFO_BM_PARENT_START, PROFESSION_INFO_BM_PARENT_NUM_BITS);
	return eProfession == SETBITS(PROFESSION_INFO_BM_PARENT_NUM_BITS, 0) ? NO_PROFESSION : eProfession;
}

inline int CvProfessionInfo::getNumSubTypes() const
{
	return GETBITS(m_bfA, PROFESSION_INFO_BM_NUM_CHILDREN_START, PROFESSION_INFO_BM_NUM_CHILDREN_NUM_BITS);
}
/// info subclass - end - Nightinggale

inline ProfessionYieldCost CvProfessionInfo::getYieldEquipmentAmount(int iYield) const
{
	return m_acYieldEquipments.get(iYield);
}

inline bool CvProfessionInfo::isFreePromotion(int i) const
{
	return m_ba_FreePromotions.get(i);
}
#endif
