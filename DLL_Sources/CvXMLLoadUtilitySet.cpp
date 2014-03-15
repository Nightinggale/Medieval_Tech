//
// XML Set functions
//

#include "CvGameCoreDLL.h"
#include "CvDLLXMLIFaceBase.h"
#include "CvXMLLoadUtility.h"
#include "CvGlobals.h"
#include "CvArtFileMgr.h"
#include "CvGameTextMgr.h"
#include <algorithm>
#include "CvInfoWater.h"
#include "FProfiler.h"
#include "FVariableSystem.h"
#include "CvGameCoreUtils.h"

#include "CvInfoProfessions.h"

// XML length check - start - Nightinggale
#ifdef FASSERT_ENABLE
char f_szXMLname[1024];
#endif
// XML length check - end - Nightinggale

/// XML load - start - Nightinggale
bool bFirstLoadRound;
bool bLoadOnce;

void CvXMLLoadUtility::loadXMLFiles()
{
	bLoadOnce = false;
	loadXMLFile(XML_FILE_CIV4BasicInfos);
	loadXMLFile(XML_FILE_CIV4CalendarInfos);
	loadXMLFile(XML_FILE_CIV4SeasonInfos);
	loadXMLFile(XML_FILE_CIV4MonthInfos);
	loadXMLFile(XML_FILE_CIV4DenialInfos);
	loadXMLFile(XML_FILE_CIV4InvisibleInfos);
	loadXMLFile(XML_FILE_CIV4UnitCombatInfos);
	loadXMLFile(XML_FILE_CIV4DomainInfos);
	loadXMLFile(XML_FILE_CIV4UnitAIInfos);
	loadXMLFile(XML_FILE_CIV4AttitudeInfos);
	loadXMLFile(XML_FILE_CIV4MemoryInfos);
	loadXMLFile(XML_FILE_CIV4FatherCategoryInfos);
	loadXMLFile(XML_FILE_CIV4ColorVals);
	loadXMLFile(XML_FILE_CIV4UnitClassInfos);
	loadXMLFile(XML_FILE_CIV4CultureLevelInfo);
	loadXMLFile(XML_FILE_CIV4VictoryInfo);
	loadXMLFile(XML_FILE_CIV4BuildingClassInfos);
	loadXMLFile(XML_FILE_CIV4PlayerColorInfos);
	loadXMLFile(XML_FILE_CIV4EuropeInfo);
	loadXMLFile(XML_FILE_CIV4YieldInfos);
	loadXMLFile(XML_FILE_CIV4AlarmInfos);
	loadXMLFile(XML_FILE_CIV4GameSpeedInfo);
	loadXMLFile(XML_FILE_CIV4TurnTimerInfo);
	loadXMLFile(XML_FILE_CIV4WorldInfo);
	loadXMLFile(XML_FILE_CIV4ClimateInfo);
	loadXMLFile(XML_FILE_CIV4SeaLevelInfo);
	loadXMLFile(XML_FILE_CIV4TerrainInfos);
	loadXMLFile(XML_FILE_CIV4EraInfos);
	loadXMLFile(XML_FILE_Civ4FeatureInfos);
	loadXMLFile(XML_FILE_CIV4PromotionInfos);
	loadXMLFile(XML_FILE_CIV4ProfessionInfos);
	loadXMLFile(XML_FILE_CIV4GoodyInfo);
	loadXMLFile(XML_FILE_CIV4TraitInfos);
	loadXMLFile(XML_FILE_CIV4SpecialBuildingInfos);
	loadXMLFile(XML_FILE_CIV4AnimationInfos);
	loadXMLFile(XML_FILE_CIV4AnimationPathInfos);
	loadXMLFile(XML_FILE_CIV4HandicapInfo);
	loadXMLFile(XML_FILE_CIV4CursorInfo);
	loadXMLFile(XML_FILE_CIV4CivicOptionInfos);
	loadXMLFile(XML_FILE_CIV4HurryInfo);
	loadXMLFile(XML_FILE_CIV4BuildingInfos);
	loadXMLFile(XML_FILE_CIV4BonusInfos);
	loadXMLFile(XML_FILE_Civ4RouteInfos);
	loadXMLFile(XML_FILE_CIV4ImprovementInfos);
	loadXMLFile(XML_FILE_CIV4FatherPointInfos);
	loadXMLFile(XML_FILE_CIV4FatherInfos);
	loadXMLFile(XML_FILE_CIV4SpecialUnitInfos);
	loadXMLFile(XML_FILE_CIV4CivicInfos);
	loadXMLFile(XML_FILE_CIV4LeaderHeadInfos);
	loadXMLFile(XML_FILE_CIV4EffectInfos);
	loadXMLFile(XML_FILE_CIV4EntityEventInfos);
	loadXMLFile(XML_FILE_CIV4BuildInfos);
	loadXMLFile(XML_FILE_CIV4UnitInfos);
	loadXMLFile(XML_FILE_CIV4UnitArtStyleTypeInfos);
	loadXMLFile(XML_FILE_CIV4CivilizationInfos);
	loadXMLFile(XML_FILE_CIV4MainMenus);
	loadXMLFile(XML_FILE_CIV4GameOptionInfos);
	loadXMLFile(XML_FILE_CIV4MPOptionInfos);
	loadXMLFile(XML_FILE_CIV4ForceControlInfos);
	loadXMLFile(XML_FILE_CIV4TerrainSettings);
	loadXMLFile(XML_FILE_CIV4EventInfos);
	loadXMLFile(XML_FILE_CIV4EventTriggerInfos);
	loadXMLFile(XML_FILE_CIV4EmphasizeInfo);
	loadXMLFile(XML_FILE_CIV4MissionInfos);
	loadXMLFile(XML_FILE_CIV4ControlInfos);
	loadXMLFile(XML_FILE_CIV4CommandInfos);
	loadXMLFile(XML_FILE_CIV4AttachableInfos);
	loadXMLFile(XML_FILE_CIV4DiplomacyInfos);


	if (!bFirstLoadRound)
	{
		// type less XML files
		// these files will produce an error if loaded more than once
		bLoadOnce = true;
		loadXMLFile(XML_FILE_CIV4Hints);
		loadXMLFile(XML_FILE_CIV4SlideShowInfos);
		loadXMLFile(XML_FILE_CIV4SlideShowRandomInfos);
		loadXMLFile(XML_FILE_CIV4WorldPickerInfos);
		loadXMLFile(XML_FILE_Civ4RouteModelInfos);
		loadXMLFile(XML_FILE_CIV4RiverModelInfos);
		loadXMLFile(XML_FILE_CIV4WaterPlaneInfos);
		loadXMLFile(XML_FILE_CIV4TerrainPlaneInfos);
		loadXMLFile(XML_FILE_CIV4CameraOverlayInfos);
		loadXMLFile(XML_FILE_CIV4AutomateInfos);
		loadXMLFile(XML_FILE_CIV4InterfaceModeInfos);
		loadXMLFile(XML_FILE_CIV4FormationInfos);
	}
}

void CvXMLLoadUtility::loadXMLFile(XMLFileNames eFile)
{
	if (eFile == XML_FILE_CIV4ArtDefines_Bonus)
	{
		LoadGlobalClassInfo(ARTFILEMGR.getBonusArtInfo(), "CIV4ArtDefines_Bonus", "Art", "Civ4ArtDefines/BonusArtInfos/BonusArtInfo", NULL);
	}
	else if (eFile == XML_FILE_CIV4ArtDefines_Building)
	{
		LoadGlobalClassInfo(ARTFILEMGR.getBuildingArtInfo(), "CIV4ArtDefines_Building", "Art", "Civ4ArtDefines/BuildingArtInfos/BuildingArtInfo", NULL);
	}
	else if (eFile == XML_FILE_CIV4ArtDefines_Civilization)
	{
		LoadGlobalClassInfo(ARTFILEMGR.getCivilizationArtInfo(), "CIV4ArtDefines_Civilization", "Art", "Civ4ArtDefines/CivilizationArtInfos/CivilizationArtInfo", NULL);
	}
	else if (eFile == XML_FILE_CIV4ArtDefines_Feature)
	{
		LoadGlobalClassInfo(ARTFILEMGR.getFeatureArtInfo(), "CIV4ArtDefines_Feature", "Art", "Civ4ArtDefines/FeatureArtInfos/FeatureArtInfo", NULL);
	}
	else if (eFile == XML_FILE_CIV4ArtDefines_Improvement)
	{
		LoadGlobalClassInfo(ARTFILEMGR.getImprovementArtInfo(), "CIV4ArtDefines_Improvement", "Art", "Civ4ArtDefines/ImprovementArtInfos/ImprovementArtInfo", NULL);
	}
	else if (eFile == XML_FILE_CIV4ArtDefines_Interface)
	{
		LoadGlobalClassInfo(ARTFILEMGR.getInterfaceArtInfo(), "CIV4ArtDefines_Interface", "Art", "Civ4ArtDefines/InterfaceArtInfos/InterfaceArtInfo", NULL);
	}
	else if (eFile == XML_FILE_CIV4ArtDefines_Leaderhead)
	{
		LoadGlobalClassInfo(ARTFILEMGR.getLeaderheadArtInfo(), "CIV4ArtDefines_Leaderhead", "Art", "Civ4ArtDefines/LeaderheadArtInfos/LeaderheadArtInfo", NULL);
	}
	else if (eFile == XML_FILE_CIV4ArtDefines_Misc)
	{
		LoadGlobalClassInfo(ARTFILEMGR.getMiscArtInfo(), "CIV4ArtDefines_Misc", "Art", "Civ4ArtDefines/MiscArtInfos/MiscArtInfo", NULL);
	}
	else if (eFile == XML_FILE_CIV4ArtDefines_Movie)
	{
		LoadGlobalClassInfo(ARTFILEMGR.getMovieArtInfo(), "CIV4ArtDefines_Movie", "Art", "Civ4ArtDefines/MovieArtInfos/MovieArtInfo", NULL);
	}
	else if (eFile == XML_FILE_CIV4ArtDefines_Terrain)
	{
		LoadGlobalClassInfo(ARTFILEMGR.getTerrainArtInfo(), "CIV4ArtDefines_Terrain", "Art", "Civ4ArtDefines/TerrainArtInfos/TerrainArtInfo", NULL);
	}
	else if (eFile == XML_FILE_CIV4ArtDefines_Unit)
	{
		LoadGlobalClassInfo(ARTFILEMGR.getUnitArtInfo(), "CIV4ArtDefines_Unit", "Art", "Civ4ArtDefines/UnitArtInfos/UnitArtInfo", NULL);
	}
	else if (eFile == XML_FILE_CIV4AlarmInfos)
	{
		LoadGlobalClassInfo(GC.getAlarmInfo(), "CIV4AlarmInfos", "Civilizations", "Civ4AlarmInfos/AlarmInfos/AlarmInfo", NULL);
		FAssertMsg(GC.getNumAlarmInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4AnimationInfos)
	{
		LoadGlobalClassInfo(GC.getAnimationCategoryInfo(), "CIV4AnimationInfos", "Units", "Civ4AnimationInfos/AnimationCategories/AnimationCategory", NULL);
		FAssertMsg(GC.getNumAnimationCategoryInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4AnimationPathInfos)
	{
		LoadGlobalClassInfo(GC.getAnimationPathInfo(), "CIV4AnimationPathInfos", "Units", "Civ4AnimationPathInfos/AnimationPaths/AnimationPath", NULL);
		FAssertMsg(GC.getNumAnimationPathInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4AttachableInfos)
	{
		LoadGlobalClassInfo(GC.getAttachableInfo(), "CIV4AttachableInfos", "Misc", "Civ4AttachableInfos/AttachableInfos/AttachableInfo", NULL);
		FAssertMsg(GC.getNumAttachableInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4AttitudeInfos)
	{
		LoadGlobalClassInfo(GC.getAttitudeInfo(), "CIV4AttitudeInfos", "BasicInfos", "Civ4AttitudeInfos/AttitudeInfos/AttitudeInfo", NULL);
		GC.CheckEnumAttitudeTypes(); // XML enum check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4AutomateInfos)
	{
		LoadGlobalClassInfo(GC.getAutomateInfo(), "CIV4AutomateInfos", "Units", "Civ4AutomateInfos/AutomateInfos/AutomateInfo", NULL);
		FAssertMsg(GC.getNumAutomateInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
		UpdateProgressCB("Global Interface");
	}
	else if (eFile == XML_FILE_CIV4BonusInfos)
	{
		LoadGlobalClassInfo(GC.getBonusInfo(), "CIV4BonusInfos", "Terrain", "Civ4BonusInfos/BonusInfos/BonusInfo", &CvDLLUtilityIFaceBase::createBonusInfoCacheObject);
		FAssertMsg(GC.getNumBonusInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4BuildInfos)
	{
		LoadGlobalClassInfo(GC.getBuildInfo(), "CIV4BuildInfos", "Units", "Civ4BuildInfos/BuildInfos/BuildInfo", NULL);
		FAssertMsg(GC.getNumBuildInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4BuildingClassInfos)
	{
		LoadGlobalClassInfo(GC.getBuildingClassInfo(), "CIV4BuildingClassInfos", "Buildings", "Civ4BuildingClassInfos/BuildingClassInfos/BuildingClassInfo", NULL);
		FAssertMsg(GC.getNumBuildingClassInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4BuildingInfos)
	{
		LoadGlobalClassInfo(GC.getBuildingInfo(), "CIV4BuildingInfos", "Buildings", "Civ4BuildingInfos/BuildingInfos/BuildingInfo", &CvDLLUtilityIFaceBase::createBuildingInfoCacheObject);
		FAssertMsg(GC.getNumBuildingInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4CalendarInfos)
	{
		LoadGlobalClassInfo(GC.getCalendarInfo(), "CIV4CalendarInfos", "BasicInfos", "Civ4CalendarInfos/CalendarInfos/CalendarInfo", NULL);
		FAssertMsg(GC.getNumCalendarInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4CameraOverlayInfos)
	{
		LoadGlobalClassInfo(GC.getCameraOverlayInfo(), "CIV4CameraOverlayInfos", "Misc", "Civ4CameraOverlayInfos/CameraOverlayInfos/CameraOverlayInfo", NULL);
		FAssertMsg(GC.getNumCameraOverlayInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
		UpdateProgressCB("Global Emphasize");
	}
	else if (eFile == XML_FILE_CIV4CivicInfos)
	{
		LoadGlobalClassInfo(GC.getCivicInfo(), "CIV4CivicInfos", "GameInfo", "Civ4CivicInfos/CivicInfos/CivicInfo", &CvDLLUtilityIFaceBase::createCivicInfoCacheObject);
		FAssertMsg(GC.getNumCivicInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4CivicOptionInfos)
	{
		LoadGlobalClassInfo(GC.getCivicOptionInfo(), "CIV4CivicOptionInfos", "GameInfo", "Civ4CivicOptionInfos/CivicOptionInfos/CivicOptionInfo", NULL);
		FAssertMsg(GC.getNumCivicOptionInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4CivilizationInfos)
	{
		LoadGlobalClassInfo(GC.getCivilizationInfo(), "CIV4CivilizationInfos", "Civilizations", "Civ4CivilizationInfos/CivilizationInfos/CivilizationInfo", &CvDLLUtilityIFaceBase::createCivilizationInfoCacheObject);
		FAssertMsg(GC.getNumCivilizationInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4ClimateInfo)
	{
		LoadGlobalClassInfo(GC.getClimateInfo(), "CIV4ClimateInfo", "GameInfo", "Civ4ClimateInfo/ClimateInfos/ClimateInfo", NULL);
		FAssertMsg(GC.getNumClimateInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4ColorVals)
	{
		LoadGlobalClassInfo(GC.getColorInfo(), "CIV4ColorVals", "Interface", "Civ4ColorVals/ColorVals/ColorVal", NULL);
		FAssertMsg(GC.getNumColorInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4CommandInfos)
	{
		LoadGlobalClassInfo(GC.getCommandInfo(), "CIV4CommandInfos", "Units", "Civ4CommandInfos/CommandInfos/CommandInfo", NULL);
		FAssertMsg(GC.getNumCommandInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4BasicInfos)
	{
		LoadGlobalClassInfo(GC.getConceptInfo(), "CIV4BasicInfos", "BasicInfos", "Civ4BasicInfos/ConceptInfos/ConceptInfo", NULL);
		FAssertMsg(GC.getNumConceptInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4ControlInfos)
	{
		LoadGlobalClassInfo(GC.getControlInfo(), "CIV4ControlInfos", "Units", "Civ4ControlInfos/ControlInfos/ControlInfo", NULL);
		FAssertMsg(GC.getNumControlInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4CultureLevelInfo)
	{
		LoadGlobalClassInfo(GC.getCultureLevelInfo(), "CIV4CultureLevelInfo", "GameInfo", "Civ4CultureLevelInfo/CultureLevelInfos/CultureLevelInfo", NULL);
		FAssertMsg(GC.getNumCultureLevelInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4CursorInfo)
	{
		LoadGlobalClassInfo(GC.getCursorInfo(), "CIV4CursorInfo", "GameInfo", "Civ4CursorInfo/CursorInfos/CursorInfo", NULL);
		FAssertMsg(GC.getNumCursorInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4DenialInfos)
	{
		LoadGlobalClassInfo(GC.getDenialInfo(), "CIV4DenialInfos", "BasicInfos", "Civ4DenialInfos/DenialInfos/DenialInfo", NULL);
		FAssertMsg(GC.getNumDenialInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4DiplomacyInfos)
	{
		if (bFirstLoadRound)
		{
			LoadGlobalClassInfo(GC.getDiplomacyInfo(), "CIV4DiplomacyInfos", "GameInfo", "Civ4DiplomacyInfos/DiplomacyInfos/DiplomacyInfo", NULL);
		} else {
			// Special Case Diplomacy Info due to double vectored nature and appending of Responses
			LoadDiplomacyInfo(GC.getDiplomacyInfo(), "CIV4DiplomacyInfos", "GameInfo", "Civ4DiplomacyInfos/DiplomacyInfos/DiplomacyInfo", &CvDLLUtilityIFaceBase::createDiplomacyInfoCacheObject);
		}
		FAssertMsg(GC.getNumDiplomacyInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4DomainInfos)
	{
		LoadGlobalClassInfo(GC.getDomainInfo(), "CIV4DomainInfos", "BasicInfos", "Civ4DomainInfos/DomainInfos/DomainInfo", NULL);
		GC.CheckEnumDomainTypes(); // XML enum check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4EffectInfos)
	{
		LoadGlobalClassInfo(GC.getEffectInfo(), "CIV4EffectInfos", "Misc", "Civ4EffectInfos/EffectInfos/EffectInfo", NULL);
		FAssertMsg(GC.getNumEffectInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4EmphasizeInfo)
	{
		UpdateProgressCB("Global Emphasize");
		LoadGlobalClassInfo(GC.getEmphasizeInfo(), "CIV4EmphasizeInfo", "GameInfo", "Civ4EmphasizeInfo/EmphasizeInfos/EmphasizeInfo", NULL);
		FAssertMsg(GC.getNumEmphasizeInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4EntityEventInfos)
	{
		LoadGlobalClassInfo(GC.getEntityEventInfo(), "CIV4EntityEventInfos", "Units", "Civ4EntityEventInfos/EntityEventInfos/EntityEventInfo", NULL);
		FAssertMsg(GC.getNumEntityEventInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4EraInfos)
	{
		LoadGlobalClassInfo(GC.getEraInfo(), "CIV4EraInfos", "GameInfo", "Civ4EraInfos/EraInfos/EraInfo", NULL);
		FAssertMsg(GC.getNumEraInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4EuropeInfo)
	{
		LoadGlobalClassInfo(GC.getEuropeInfo(), "CIV4EuropeInfo", "GameInfo", "Civ4EuropeInfo/EuropeInfos/EuropeInfo", NULL); // TK Mod XML load order change
		FAssertMsg(GC.getNumEuropeInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4EventInfos)
	{
		UpdateProgressCB("Global Events");
		LoadGlobalClassInfo(GC.getEventInfo(), "CIV4EventInfos", "Events", "Civ4EventInfos/EventInfos/EventInfo", &CvDLLUtilityIFaceBase::createEventInfoCacheObject);
		FAssertMsg(GC.getNumEventInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4EventTriggerInfos)
	{
		LoadGlobalClassInfo(GC.getEventTriggerInfo(), "CIV4EventTriggerInfos", "Events", "Civ4EventTriggerInfos/EventTriggerInfos/EventTriggerInfo", &CvDLLUtilityIFaceBase::createEventTriggerInfoCacheObject);
		FAssertMsg(GC.getNumEventTriggerInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4FatherCategoryInfos)
	{
		LoadGlobalClassInfo(GC.getFatherCategoryInfo(), "CIV4FatherCategoryInfos", "BasicInfos", "Civ4FatherCategoryInfos/FatherCategoryInfos/FatherCategoryInfo", NULL);
		FAssertMsg(GC.getNumFatherCategoryInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4FatherInfos)
	{
		LoadGlobalClassInfo(GC.getFatherInfo(), "CIV4FatherInfos", "GameInfo", "Civ4FatherInfos/FatherInfos/FatherInfo", &CvDLLUtilityIFaceBase::createFatherInfoCacheObject);
		FAssertMsg(GC.getNumFatherInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4FatherPointInfos)
	{
		LoadGlobalClassInfo(GC.getFatherPointInfo(), "CIV4FatherPointInfos", "GameInfo", "Civ4FatherPointInfos/FatherPointInfos/FatherPointInfo", NULL);
		FAssertMsg(GC.getNumFatherPointInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_Civ4FeatureInfos)
	{
		LoadGlobalClassInfo(GC.getFeatureInfo(), "Civ4FeatureInfos", "Terrain", "Civ4FeatureInfos/FeatureInfos/FeatureInfo", NULL);
		FAssertMsg(GC.getNumFeatureInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4ForceControlInfos)
	{
		LoadGlobalClassInfo(GC.getForceControlInfo(), "CIV4ForceControlInfos", "GameInfo", "Civ4ForceControlInfos/ForceControlInfos/ForceControlInfo", NULL);
		FAssertMsg(GC.getNumForceControlInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4GameOptionInfos)
	{
		LoadGlobalClassInfo(GC.getGameOptionInfo(), "CIV4GameOptionInfos", "GameInfo", "Civ4GameOptionInfos/GameOptionInfos/GameOptionInfo", NULL);
		FAssertMsg(GC.getNumGameOptionInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4GameSpeedInfo)
	{
		LoadGlobalClassInfo(GC.getGameSpeedInfo(), "CIV4GameSpeedInfo", "GameInfo", "Civ4GameSpeedInfo/GameSpeedInfos/GameSpeedInfo", NULL);
		FAssertMsg(GC.getNumGameSpeedInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4GoodyInfo)
	{
		LoadGlobalClassInfo(GC.getGoodyInfo(), "CIV4GoodyInfo", "GameInfo", "Civ4GoodyInfo/GoodyInfos/GoodyInfo", NULL);
		FAssertMsg(GC.getNumGoodyInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4GraphicOptionInfos)
	{
		LoadGlobalClassInfo(GC.getGraphicOptionInfo(), "CIV4GraphicOptionInfos", "GameInfo", "Civ4GraphicOptionInfos/GraphicOptionInfos/GraphicOptionInfo", NULL);
		FAssert(GC.getNumGraphicOptions() == NUM_GRAPHICOPTION_TYPES);
		GC.CheckEnumGraphicOptionTypes(); // XML enum check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4HandicapInfo)
	{
		LoadGlobalClassInfo(GC.getHandicapInfo(), "CIV4HandicapInfo", "GameInfo", "Civ4HandicapInfo/HandicapInfos/HandicapInfo", &CvDLLUtilityIFaceBase::createHandicapInfoCacheObject);
		FAssertMsg(GC.getNumHandicapInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4Hints)
	{
		LoadGlobalClassInfo(GC.getHints(), "CIV4Hints", "GameInfo", "Civ4Hints/HintInfos/HintInfo", NULL);
		FAssertMsg(GC.getNumHints() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4HurryInfo)
	{
		LoadGlobalClassInfo(GC.getHurryInfo(), "CIV4HurryInfo", "GameInfo", "Civ4HurryInfo/HurryInfos/HurryInfo", NULL);
		FAssertMsg(GC.getNumHurryInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4ImprovementInfos)
	{
		LoadGlobalClassInfo(GC.getImprovementInfo(), "CIV4ImprovementInfos", "Terrain", "Civ4ImprovementInfos/ImprovementInfos/ImprovementInfo", &CvDLLUtilityIFaceBase::createImprovementInfoCacheObject);
		FAssertMsg(GC.getNumImprovementInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4InterfaceModeInfos)
	{
		LoadGlobalClassInfo(GC.getInterfaceModeInfo(), "CIV4InterfaceModeInfos", "Interface", "Civ4InterfaceModeInfos/InterfaceModeInfos/InterfaceModeInfo", NULL);
		GC.CheckEnumInterfaceModeTypes(); // XML enum check - Nightinggale
		SetGlobalActionInfo();
	}
	else if (eFile == XML_FILE_CIV4InvisibleInfos)
	{
		LoadGlobalClassInfo(GC.getInvisibleInfo(), "CIV4InvisibleInfos", "BasicInfos", "Civ4InvisibleInfos/InvisibleInfos/InvisibleInfo", NULL);
		FAssertMsg(GC.getNumInvisibleInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4TerrainSettings)
	{
		LoadGlobalClassInfo(GC.getLandscapeInfo(), "CIV4TerrainSettings", "Terrain", "Civ4TerrainSettings/LandscapeInfos/LandscapeInfo", NULL);
		FAssertMsg(GC.getNumLandscapeInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4LeaderHeadInfos)
	{
		LoadGlobalClassInfo(GC.getLeaderHeadInfo(), "CIV4LeaderHeadInfos", "Civilizations", "Civ4LeaderHeadInfos/LeaderHeadInfos/LeaderHeadInfo", &CvDLLUtilityIFaceBase::createLeaderHeadInfoCacheObject);
		FAssertMsg(GC.getNumLeaderHeadInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4MPOptionInfos)
	{
		LoadGlobalClassInfo(GC.getMPOptionInfo(), "CIV4MPOptionInfos", "GameInfo", "Civ4MPOptionInfos/MPOptionInfos/MPOptionInfo", NULL);
		FAssertMsg(GC.getNumMPOptionInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4MainMenus)
	{
		LoadGlobalClassInfo(GC.getMainMenus(), "CIV4MainMenus", "Art", "Civ4MainMenus/MainMenus/MainMenu", NULL);
		FAssertMsg(GC.getNumMainMenus() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4MemoryInfos)
	{
		LoadGlobalClassInfo(GC.getMemoryInfo(), "CIV4MemoryInfos", "BasicInfos", "Civ4MemoryInfos/MemoryInfos/MemoryInfo", NULL);
		GC.CheckEnumMemoryTypes(); // XML enum check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4MissionInfos)
	{
		UpdateProgressCB("Global Other");
		LoadGlobalClassInfo(GC.getMissionInfo(), "CIV4MissionInfos", "Units", "Civ4MissionInfos/MissionInfos/MissionInfo", NULL);
		FAssertMsg(GC.getNumMissionInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4MonthInfos)
	{
		LoadGlobalClassInfo(GC.getMonthInfo(), "CIV4MonthInfos", "BasicInfos", "Civ4MonthInfos/MonthInfos/MonthInfo", NULL);
		FAssertMsg(GC.getNumMonthInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4PlayerColorInfos)
	{
		LoadGlobalClassInfo(GC.getPlayerColorInfo(), "CIV4PlayerColorInfos", "Interface", "Civ4PlayerColorInfos/PlayerColorInfos/PlayerColorInfo", NULL);
		FAssertMsg(GC.getNumPlayerColorInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4PlayerOptionInfos)
	{
		LoadGlobalClassInfo(GC.getPlayerOptionInfo(), "CIV4PlayerOptionInfos", "GameInfo", "Civ4PlayerOptionInfos/PlayerOptionInfos/PlayerOptionInfo", NULL);
		FAssertMsg(GC.getNumPlayerOptionInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4ProfessionInfos)
	{
		LoadGlobalClassInfo(GC.getProfessionInfo(), "CIV4ProfessionInfos", "Units", "Civ4ProfessionInfos/ProfessionInfos/ProfessionInfo", &CvDLLUtilityIFaceBase::createProfessionInfoCacheObject);
		FAssertMsg(GC.getNumProfessionInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4PromotionInfos)
	{
		LoadGlobalClassInfo(GC.getPromotionInfo(), "CIV4PromotionInfos", "Units", "Civ4PromotionInfos/PromotionInfos/PromotionInfo", &CvDLLUtilityIFaceBase::createPromotionInfoCacheObject);
		FAssertMsg(GC.getNumPromotionInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4RiverModelInfos)
	{
		UpdateProgressCB("Global Rivers");
		LoadGlobalClassInfo(GC.getRiverModelInfo(), "CIV4RiverModelInfos", "Art", "Civ4RiverModelInfos/RiverModelInfos/RiverModelInfo", NULL);
		FAssertMsg(GC.getNumRiverModelInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_Civ4RouteInfos)
	{
		LoadGlobalClassInfo(GC.getRouteInfo(), "Civ4RouteInfos", "Misc", "Civ4RouteInfos/RouteInfos/RouteInfo", NULL);
		FAssertMsg(GC.getNumRouteInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_Civ4RouteModelInfos)
	{
		UpdateProgressCB("Global Routes");
		LoadGlobalClassInfo(GC.getRouteModelInfo(), "Civ4RouteModelInfos", "Art", "Civ4RouteModelInfos/RouteModelInfos/RouteModelInfo", NULL);
		FAssertMsg(GC.getNumRouteModelInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4SeaLevelInfo)
	{
		LoadGlobalClassInfo(GC.getSeaLevelInfo(), "CIV4SeaLevelInfo", "GameInfo", "Civ4SeaLevelInfo/SeaLevelInfos/SeaLevelInfo", NULL);
		FAssertMsg(GC.getNumSeaLevelInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4SeasonInfos)
	{
		LoadGlobalClassInfo(GC.getSeasonInfo(), "CIV4SeasonInfos", "BasicInfos", "Civ4SeasonInfos/SeasonInfos/SeasonInfo", NULL);
		FAssertMsg(GC.getNumSeasonInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4SlideShowInfos)
	{
		LoadGlobalClassInfo(GC.getSlideShowInfo(), "CIV4SlideShowInfos", "Interface", "Civ4SlideShowInfos/SlideShowInfos/SlideShowInfo", NULL);
		FAssertMsg(GC.getNumSlideShowInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4SlideShowRandomInfos)
	{
		LoadGlobalClassInfo(GC.getSlideShowRandomInfo(), "CIV4SlideShowRandomInfos", "Interface", "Civ4SlideShowRandomInfos/SlideShowRandomInfos/SlideShowRandomInfo", NULL);
		FAssertMsg(GC.getNumSlideShowRandomInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4SpecialBuildingInfos)
	{
		LoadGlobalClassInfo(GC.getSpecialBuildingInfo(), "CIV4SpecialBuildingInfos", "Buildings", "Civ4SpecialBuildingInfos/SpecialBuildingInfos/SpecialBuildingInfo", NULL);
		FAssertMsg(GC.getNumSpecialBuildingInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4SpecialUnitInfos)
	{
		LoadGlobalClassInfo(GC.getSpecialUnitInfo(), "CIV4SpecialUnitInfos", "Units", "Civ4SpecialUnitInfos/SpecialUnitInfos/SpecialUnitInfo", NULL);
		FAssertMsg(GC.getNumSpecialUnitInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4TerrainInfos)
	{
		LoadGlobalClassInfo(GC.getTerrainInfo(), "CIV4TerrainInfos", "Terrain", "Civ4TerrainInfos/TerrainInfos/TerrainInfo", NULL);
		FAssertMsg(GC.getNumTerrainInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4TerrainPlaneInfos)
	{
		LoadGlobalClassInfo(GC.getTerrainPlaneInfo(), "CIV4TerrainPlaneInfos", "Misc", "Civ4TerrainPlaneInfos/TerrainPlaneInfos/TerrainPlaneInfo", NULL);
		FAssertMsg(GC.getNumTerrainPlaneInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4TraitInfos)
	{
		LoadGlobalClassInfo(GC.getTraitInfo(), "CIV4TraitInfos", "Civilizations", "Civ4TraitInfos/TraitInfos/TraitInfo", &CvDLLUtilityIFaceBase::createTraitInfoCacheObject);
		FAssertMsg(GC.getNumTraitInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4TurnTimerInfo)
	{
		LoadGlobalClassInfo(GC.getTurnTimerInfo(), "CIV4TurnTimerInfo", "GameInfo", "Civ4TurnTimerInfo/TurnTimerInfos/TurnTimerInfo", NULL);
		FAssertMsg(GC.getNumTurnTimerInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4UnitAIInfos)
	{
		LoadGlobalClassInfo(GC.getUnitAIInfo(), "CIV4UnitAIInfos", "BasicInfos", "Civ4UnitAIInfos/UnitAIInfos/UnitAIInfo", NULL);
		GC.CheckEnumUnitAITypes(); // XML enum check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4UnitArtStyleTypeInfos)
	{
		LoadGlobalClassInfo(GC.getUnitArtStyleTypeInfo(), "CIV4UnitArtStyleTypeInfos", "Civilizations", "Civ4UnitArtStyleTypeInfos/UnitArtStyleTypeInfos/UnitArtStyleTypeInfo", false);
		FAssertMsg(GC.getNumUnitArtStyleTypeInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4UnitClassInfos)
	{
		LoadGlobalClassInfo(GC.getUnitClassInfo(), "CIV4UnitClassInfos", "Units", "Civ4UnitClassInfos/UnitClassInfos/UnitClassInfo", NULL);
		FAssertMsg(GC.getNumUnitClassInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4UnitCombatInfos)
	{
		LoadGlobalClassInfo(GC.getUnitCombatInfo(), "CIV4UnitCombatInfos", "BasicInfos", "Civ4UnitCombatInfos/UnitCombatInfos/UnitCombatInfo", NULL);
		FAssertMsg(GC.getNumUnitCombatInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4FormationInfos)
	{
		LoadGlobalClassInfo(GC.getUnitFormationInfo(), "CIV4FormationInfos", "Units", "UnitFormations/UnitFormation", NULL);
		FAssertMsg(GC.getNumUnitFormationInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4UnitInfos)
	{
		LoadGlobalClassInfo(GC.getUnitInfo(), "CIV4UnitInfos", "Units", "Civ4UnitInfos/UnitInfos/UnitInfo", &CvDLLUtilityIFaceBase::createUnitInfoCacheObject);
		FAssertMsg(GC.getNumUnitInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4VictoryInfo)
	{
		LoadGlobalClassInfo(GC.getVictoryInfo(), "CIV4VictoryInfo", "GameInfo", "Civ4VictoryInfo/VictoryInfos/VictoryInfo", NULL);
		FAssertMsg(GC.getNumVictoryInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4WaterPlaneInfos)
	{
		UpdateProgressCB("Global Other");
		LoadGlobalClassInfo(GC.getWaterPlaneInfo(), "CIV4WaterPlaneInfos", "Misc", "Civ4WaterPlaneInfos/WaterPlaneInfos/WaterPlaneInfo", NULL);
		FAssertMsg(GC.getNumWaterPlaneInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4WorldInfo)
	{
		LoadGlobalClassInfo(GC.getWorldInfo(), "CIV4WorldInfo", "GameInfo", "Civ4WorldInfo/WorldInfos/WorldInfo", NULL);
		FAssertMsg(GC.getNumWorldInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
		GC.CheckEnumWorldSizeTypes(); // XML enum check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4WorldPickerInfos)
	{
		LoadGlobalClassInfo(GC.getWorldPickerInfo(), "CIV4WorldPickerInfos", "Interface", "Civ4WorldPickerInfos/WorldPickerInfos/WorldPickerInfo", NULL);
		FAssertMsg(GC.getNumWorldPickerInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	}
	else if (eFile == XML_FILE_CIV4YieldInfos)
	{
		LoadGlobalClassInfo(GC.getYieldInfo(), "CIV4YieldInfos", "Terrain", "Civ4YieldInfos/YieldInfos/YieldInfo", NULL);
		if (bFirstLoadRound)
		{
			// we only need to check this once
			FAssertMsg(GC.getYieldInfo().size() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
			FAssertMsg(NUM_YIELD_TYPES == GC.getYieldInfo().size(), "Wrong number of yields in XML (correct mod?)");
			GC.CheckEnumYieldTypes(); // XML enum check - Nightinggale
	
		}
	}
	else
	{
		FAssertMsg(false, "loadXMLFile() doesn't know how to handle eFile");
	}
}
/// XML load - end - Nightinggale

bool CvXMLLoadUtility::ReadGlobalDefines(const TCHAR* szXMLFileName, CvCacheObject* cache)
{
	bool bLoaded = false;	// used to make sure that the xml file was loaded correctly

	if (!gDLL->cacheRead(cache, szXMLFileName))			// src data file name
	{
		// load normally
		if (!CreateFXml())
		{
			return false;
		}

		// load the new FXml variable with the szXMLFileName file
		bLoaded = LoadCivXml(m_pFXml, szXMLFileName);
		if (!bLoaded)
		{
			char	szMessage[1024];
			sprintf( szMessage, "LoadXML call failed for %s \n Current XML file is: %s", szXMLFileName, GC.getCurrentXMLFile().GetCString());
			gDLL->MessageBox(szMessage, "XML Load Error");
		}

		// if the load succeeded we will continue
		if (bLoaded)
		{
			// if the xml is successfully validated
			if (Validate())
			{
				// locate the first define tag in the xml
				if (gDLL->getXMLIFace()->LocateNode(m_pFXml,"Civ4Defines/Define"))
				{
					int i;	// loop counter
					// get the number of other Define tags in the xml file
					int iNumDefines = gDLL->getXMLIFace()->GetNumSiblings(m_pFXml);
					// add one to the total in order to include the current Define tag
					iNumDefines++;

					// loop through all the Define tags
					for (i=0;i<iNumDefines;i++)
					{
						char szNodeType[256];	// holds the type of the current node
						char szName[256];

						// Skip any comments and stop at the next value we might want
						if (SkipToNextVal())
						{
							// call the function that sets the FXml pointer to the first non-comment child of
							// the current tag and gets the value of that new node
							if (GetChildXmlVal(szName))
							{
								// set the FXml pointer to the next sibling of the current tag``
								if (gDLL->getXMLIFace()->NextSibling(GetXML()))
								{
									// Skip any comments and stop at the next value we might want
									if (SkipToNextVal())
									{
										// if we successfuly get the node type for the current tag
										if (gDLL->getXMLIFace()->GetLastLocatedNodeType(GetXML(),szNodeType))
										{
											// if the node type of the current tag isn't null
											if (strcmp(szNodeType,"")!=0)
											{
												// if the node type of the current tag is a float then
												if (strcmp(szNodeType,"float")==0)
												{
													// get the float value for the define
													float fVal;
													GetXmlVal(&fVal);
													GC.getDefinesVarSystem()->SetValue(szName, fVal);
												}
												// else if the node type of the current tag is an int then
												else if (strcmp(szNodeType,"int")==0)
												{
													// get the int value for the define
													int iVal;
													GetXmlVal(&iVal);
													GC.getDefinesVarSystem()->SetValue(szName, iVal);
												}
												// else if the node type of the current tag is a boolean then
												else if (strcmp(szNodeType,"boolean")==0)
												{
													// get the boolean value for the define
													bool bVal;
													GetXmlVal(&bVal);
													GC.getDefinesVarSystem()->SetValue(szName, bVal);
												}
												// otherwise we will assume it is a string/text value
												else
												{
													char szVal[256];
													// get the string/text value for the define
													GetXmlVal(szVal);
													GC.getDefinesVarSystem()->SetValue(szName, szVal);
												}
											}
											// otherwise we will default to getting the string/text value for the define
											else
											{
												char szVal[256];
												// get the string/text value for the define
												GetXmlVal(szVal);
												GC.getDefinesVarSystem()->SetValue(szName, szVal);
											}
										}
									}
								}

								// since we are looking at the children of a Define tag we will need to go up
								// one level so that we can go to the next Define tag.
								// Set the FXml pointer to the parent of the current tag
								gDLL->getXMLIFace()->SetToParent(GetXML());
							}
						}

						// now we set the FXml pointer to the sibling of the current tag, which should be the next
						// Define tag
						if (!gDLL->getXMLIFace()->NextSibling(m_pFXml))
						{
							break;
						}
					}

					// write global defines info to cache
					bool bOk = gDLL->cacheWrite(cache);
					if (!bOk)
					{
						char	szMessage[1024];
						sprintf( szMessage, "Failed writing to global defines cache. \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
						gDLL->MessageBox(szMessage, "XML Caching Error");
					}
					else
					{
						logMsg("Wrote GlobalDefines to cache");
					}
				}
			}
		}

		// delete the pointer to the FXml variable
		gDLL->getXMLIFace()->DestroyFXml(m_pFXml);
	}
	else
	{
		logMsg("Read GobalDefines from cache");
	}

	return true;
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetGlobalDefines()
//
//  PURPOSE :   Initialize the variables located in globaldefines.cpp/h with the values in
//				GlobalDefines.xml
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::SetGlobalDefines()
{
	UpdateProgressCB("GlobalDefines");

	/////////////////////////////////
	//
	// use disk cache if possible.
	// if no cache or cache is older than xml file, use xml file like normal, else read from cache
	//

	CvCacheObject* cache = gDLL->createGlobalDefinesCacheObject("GlobalDefines.dat");	// cache file name

	if (!ReadGlobalDefines("xml\\Art\\GlobalArtDefines.xml", cache))  // read these first! Important to prevent cheating
	{
		return false;
	}

	if (!ReadGlobalDefines("xml\\GlobalDefines.xml", cache))
	{
		return false;
	}

	if (!ReadGlobalDefines("xml\\GlobalDefinesAlt.xml", cache))
	{
		return false;
	}

	if (!ReadGlobalDefines("xml\\PythonCallbackDefines.xml", cache))
	{
		return false;
	}

	if (gDLL->isModularXMLLoading())
	{
		std::vector<CvString> aszFiles;
		gDLL->enumerateFiles(aszFiles, "modules\\*_GlobalDefines.xml");

		for (std::vector<CvString>::iterator it = aszFiles.begin(); it != aszFiles.end(); ++it)
		{
			if (!ReadGlobalDefines(*it, cache))
			{
				return false;
			}
		}

		std::vector<CvString> aszModularFiles;
		gDLL->enumerateFiles(aszModularFiles, "modules\\*_PythonCallbackDefines.xml");

		for (std::vector<CvString>::iterator it = aszModularFiles.begin(); it != aszModularFiles.end(); ++it)
		{
			if (!ReadGlobalDefines(*it, cache))
			{
				return false;
			}
		}
	}


	gDLL->destroyCache(cache);
	////////////////////////////////////////////////////////////////////////

	GC.cacheGlobals();

	return true;
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetPostGlobalsGlobalDefines()
//
//  PURPOSE :   This function assumes that the SetGlobalDefines function has already been called
//							it then loads the few global defines that needed to reference a global variable that
//							hadn't been loaded in prior to the SetGlobalDefines call
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::SetPostGlobalsGlobalDefines()
{
	const char* szVal=NULL;		// holds the string value from the define queue
	int idx;

	if (GC.getDefinesVarSystem()->GetSize() > 0)
	{
		SetGlobalDefine("LAND_TERRAIN", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("LAND_TERRAIN", idx);

		SetGlobalDefine("DEEP_WATER_TERRAIN", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEEP_WATER_TERRAIN", idx);

		SetGlobalDefine("SHALLOW_WATER_TERRAIN", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("SHALLOW_WATER_TERRAIN", idx);

		SetGlobalDefine("LAND_IMPROVEMENT", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("LAND_IMPROVEMENT", idx);

		SetGlobalDefine("WATER_IMPROVEMENT", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("WATER_IMPROVEMENT", idx);

		SetGlobalDefine("RUINS_IMPROVEMENT", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("RUINS_IMPROVEMENT", idx);

		SetGlobalDefine("CAPITAL_BUILDINGCLASS", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("CAPITAL_BUILDINGCLASS", idx);

		SetGlobalDefine("DEFAULT_POPULATION_UNIT", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEFAULT_POPULATION_UNIT", idx);

		SetGlobalDefine("INITIAL_CITY_ROUTE_TYPE", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("INITIAL_CITY_ROUTE_TYPE", idx);

		SetGlobalDefine("STANDARD_HANDICAP", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("STANDARD_HANDICAP", idx);

		SetGlobalDefine("STANDARD_GAMESPEED", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("STANDARD_GAMESPEED", idx);

		SetGlobalDefine("STANDARD_TURNTIMER", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("STANDARD_TURNTIMER", idx);

		SetGlobalDefine("STANDARD_CLIMATE", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("STANDARD_CLIMATE", idx);

		SetGlobalDefine("STANDARD_SEALEVEL", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("STANDARD_SEALEVEL", idx);

		SetGlobalDefine("STANDARD_ERA", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("STANDARD_ERA", idx);

		SetGlobalDefine("STANDARD_CALENDAR", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("STANDARD_CALENDAR", idx);

		SetGlobalDefine("AI_HANDICAP", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("AI_HANDICAP", idx);

		SetGlobalDefine("BARBARIAN_CIVILIZATION", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("BARBARIAN_CIVILIZATION", idx);

		// < JAnimals Mod Start >
		SetGlobalDefine("BARBARIAN_LEADER", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("BARBARIAN_LEADER", idx);
		// < JAnimals Mod End >

		SetGlobalDefine("TREASURE_UNITCLASS", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("TREASURE_UNITCLASS", idx);

		///TKs Invention Core Mod v 1.0
		SetGlobalDefine("CIVICOPTION_INVENTIONS", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("CIVICOPTION_INVENTIONS", idx);

		SetGlobalDefine("DEFAULT_NATIVE_TRADE_PROFESSION", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEFAULT_NATIVE_TRADE_PROFESSION", idx);

		SetGlobalDefine("PROFESSION_INVENTOR", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("PROFESSION_INVENTOR", idx);

//		SetGlobalDefine("VICTORY_INDUSTRIALISATION", szVal);
//		idx = FindInInfoClass(szVal);
//		GC.getDefinesVarSystem()->SetValue("VICTORY_INDUSTRIALISATION", idx);

		SetGlobalDefine("INDUSTRIAL_VICTORY_SINGLE_YIELD", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("INDUSTRIAL_VICTORY_SINGLE_YIELD", idx);

		SetGlobalDefine("NATIVE_TECH", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("NATIVE_TECH", idx);

		SetGlobalDefine("DEFAULT_DAWN_POPULATION_UNIT", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEFAULT_DAWN_POPULATION_UNIT", idx);

		SetGlobalDefine("CONTACT_YIELD_GIFT_TECH", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("CONTACT_YIELD_GIFT_TECH", idx);

		SetGlobalDefine("DEFAULT_GRAIN_GROWTH_UNIT_CLASS", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEFAULT_GRAIN_GROWTH_UNIT_CLASS", idx);

		SetGlobalDefine("DEFAULT_NOBLE_GROWTH_UNIT_CLASS", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEFAULT_NOBLE_GROWTH_UNIT_CLASS", idx);

		SetGlobalDefine("DEFAULT_KNIGHT_PROMOTION", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEFAULT_KNIGHT_PROMOTION", idx);

		SetGlobalDefine("DEFAULT_KNIGHT_PROFESSION_PROMOTION", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEFAULT_KNIGHT_PROFESSION_PROMOTION", idx);

		SetGlobalDefine("DEFAULT_NOBLEMAN_CLASS", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEFAULT_NOBLEMAN_CLASS", idx);

		SetGlobalDefine("DEFAULT_TREASURE_YIELD", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEFAULT_TREASURE_YIELD", idx);

		SetGlobalDefine("DEFAULT_SHRINE_CLASS", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEFAULT_SHRINE_CLASS", idx);

		SetGlobalDefine("DEFAULT_PILGRAM_CLASS", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEFAULT_PILGRAM_CLASS", idx);

		SetGlobalDefine("DEFAULT_MARAUDER_CLASS", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEFAULT_MARAUDER_CLASS", idx);

		SetGlobalDefine("DEFAULT_SLAVE_CLASS", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEFAULT_SLAVE_CLASS", idx);

		SetGlobalDefine("DEFAULT_UNTRAINED_PROMOTION", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEFAULT_UNTRAINED_PROMOTION", idx);

		SetGlobalDefine("DEFAULT_TRAINED_PROMOTION", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEFAULT_TRAINED_PROMOTION", idx);

		SetGlobalDefine("DEFAULT_CRIMINAL_CLASS", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEFAULT_CRIMINAL_CLASS", idx);

		SetGlobalDefine("DEFAULT_SPECIALBUILDING_COURTHOUSE", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEFAULT_SPECIALBUILDING_COURTHOUSE", idx);

		SetGlobalDefine("FREE_PEASANT_CIVIC", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("FREE_PEASANT_CIVIC", idx);

		SetGlobalDefine("JUNGLE_FEATURE", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("JUNGLE_FEATURE", idx);

		SetGlobalDefine("DEFAULT_INVENTOR_CLASS", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEFAULT_INVENTOR_CLASS", idx);

		SetGlobalDefine("DEFAULT_RELIC_CLASS", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEFAULT_RELIC_CLASS", idx);

//		SetGlobalDefine("DEFAULT_BUILD_MOTTE_AND_BAILEY", szVal);
//		idx = FindInInfoClass(szVal);
//		GC.getDefinesVarSystem()->SetValue("DEFAULT_BUILD_MOTTE_AND_BAILEY", idx);
//
//		SetGlobalDefine("DEFAULT_BUILD_CASTLE", szVal);
//		idx = FindInInfoClass(szVal);
//		GC.getDefinesVarSystem()->SetValue("DEFAULT_BUILD_CASTLE", idx);
        ///TKs Med Update 1.1c
		SetGlobalDefine("DEFAULT_FUEDALISM_TECH", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEFAULT_FUEDALISM_TECH", idx);

		SetGlobalDefine("DEFAULT_HUNTSMAN_PROFESSION", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEFAULT_HUNTSMAN_PROFESSION", idx);

		SetGlobalDefine("DEFAULT_MARUADER_SEA_PROFESSION", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEFAULT_MARUADER_SEA_PROFESSION", idx);

		SetGlobalDefine("DEFAULT_VIKING_ERA", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEFAULT_VIKING_ERA", idx);

		SetGlobalDefine("BUILDINGCLASS_TRAVEL_TO_FAIR", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("BUILDINGCLASS_TRAVEL_TO_FAIR", idx);

		SetGlobalDefine("FATHER_POINT_REAL_TRADE", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("FATHER_POINT_REAL_TRADE", idx);

		SetGlobalDefine("MEDIEVAL_TRADE_TECH", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("MEDIEVAL_TRADE_TECH", idx);

		SetGlobalDefine("HIRE_GUARD_PROMOTION", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("HIRE_GUARD_PROMOTION", idx);

		SetGlobalDefine("VASSAL_LEADER", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("VASSAL_LEADER", idx);

		SetGlobalDefine("VASSAL_CIVILIZATION", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("VASSAL_CIVILIZATION", idx);

		SetGlobalDefine("DEFAULT_CENSURETYPE_EXCOMMUNICATION", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEFAULT_CENSURETYPE_EXCOMMUNICATION", idx);

		SetGlobalDefine("DEFAULT_CENSURETYPE_INTERDICT", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEFAULT_CENSURETYPE_INTERDICT", idx);

		SetGlobalDefine("DEFAULT_CENSURETYPE_ANATHEMA", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEFAULT_CENSURETYPE_ANATHEMA", idx);

		SetGlobalDefine("NATIVE_TRADING_TRADEPOST", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("NATIVE_TRADING_TRADEPOST", idx);

		SetGlobalDefine("MEDIEVAL_CENSURE", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("MEDIEVAL_CENSURE", idx);

		SetGlobalDefine("PROMOTION_BUILD_HOME", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("PROMOTION_BUILD_HOME", idx);

		SetGlobalDefine("TRADE_ROUTE_SPICE", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("TRADE_ROUTE_SPICE", idx);

        ///TK **************DEFAULT TEST DEFINE***************
		SetGlobalDefine("DEFAULT_TEST_DEFINE", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEFAULT_TEST_DEFINE", idx);

		///TKe

		// global cache - start - Nightinggale
		// some python use CULTURE_YIELD even though reading YIELD_CULTURE is faster
		// to avoid recoding a bunch of python we define both to be the same here
		GC.getDefinesVarSystem()->SetValue("CULTURE_YIELD", YIELD_CULTURE);

		for (int i = 0; i < GC.getNumEuropeInfos(); i++)
		{
			EuropeTypes eEurope = (EuropeTypes) i;
			CvEuropeInfo& eEuropeInfo = GC.getEuropeInfo(eEurope);
				
			if (!strcmp(eEuropeInfo.getType(), "EUROPE_EAST"))
			{
				GC.getDefinesVarSystem()->SetValue(eEuropeInfo.getType(), i);
				break;
			}
		}
		
		for (int i = 0; i < GC.getNumUnitCombatInfos(); i++)
		{
			UnitCombatTypes eUnitCombat = (UnitCombatTypes) i;
			CvInfoBase& eBase = GC.getUnitCombatInfo(eUnitCombat); 
			
			if (   !strcmp(eBase.getType(), "UNITARMOR_PLATE")
				|| !strcmp(eBase.getType(), "UNITARMOR_SHIELD")
				|| !strcmp(eBase.getType(), "UNITTACTIC_PARRY")
				|| !strcmp(eBase.getType(), "UNITWEAPON_BLUNT"))
			{
				GC.getDefinesVarSystem()->SetValue(eBase.getType(), i);
			}
		}
		// global cache - end - Nightinggale

		SetGlobalDefine("WATER_UNIT_FACING_DIRECTION", szVal);
		bool bFound = false;
		for(int iDirection=0; iDirection < NUM_DIRECTION_TYPES; ++iDirection)
		{
			CvWString szDirectionString;
			getDirectionTypeString(szDirectionString, (DirectionTypes) iDirection);
			if (szDirectionString == CvWString(szVal))
			{
				GC.getDefinesVarSystem()->SetValue("WATER_UNIT_FACING_DIRECTION", iDirection);
				bFound = true;
				break;
			}
		}
		if (!bFound)
		{
			FAssertMsg(false, "Could not match direction string.");
			GC.getDefinesVarSystem()->SetValue("WATER_UNIT_FACING_DIRECTION", DIRECTION_SOUTH);
		}

		GC.cacheXMLval(); // cache XML - Nightinggale

		return true;
	}

	char	szMessage[1024];
	sprintf( szMessage, "Size of Global Defines is not greater than 0. \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
	gDLL->MessageBox(szMessage, "XML Load Error");

	return false;
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetGlobalTypes()
//
//  PURPOSE :   Initialize the variables located in globaltypes.cpp/h with the values in
//				GlobalTypes.xml
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::SetGlobalTypes()
{
	UpdateProgressCB("GlobalTypes");

	bool bLoaded = false;	// used to make sure that the xml file was loaded correctly
	if (!CreateFXml())
	{
		return false;
	}

	// load the new FXml variable with the GlobalTypes.xml file
	bLoaded = LoadCivXml(m_pFXml, "xml/GlobalTypes.xml");
	if (!bLoaded)
	{
		char	szMessage[1024];
		sprintf( szMessage, "LoadXML call failed for GlobalTypes.xml. \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Load Error");
	}

	// if the load succeeded we will continue
	if (bLoaded)
	{
		// if the xml is successfully validated
		if (Validate())
		{
			SetGlobalStringArray(&GC.getAnimationOperatorTypes(), "Civ4Types/AnimationOperatorTypes/AnimationOperatorType", &GC.getNumAnimationOperatorTypes());
			int iEnumVal = NUM_FUNC_TYPES;
			SetGlobalStringArray(&GC.getFunctionTypes(), "Civ4Types/FunctionTypes/FunctionType", &iEnumVal, true);
			SetGlobalStringArray(&GC.getArtStyleTypes(), "Civ4Types/ArtStyleTypes/ArtStyleType", &GC.getNumArtStyleTypes());
			SetGlobalStringArray(&GC.getCitySizeTypes(), "Civ4Types/CitySizeTypes/CitySizeType", &GC.getNumCitySizeTypes());
			iEnumVal = NUM_CONTACT_TYPES;
			SetGlobalStringArray(&GC.getContactTypes(), "Civ4Types/ContactTypes/ContactType", &iEnumVal, true);
			iEnumVal = NUM_DIPLOMACYPOWER_TYPES;
			SetGlobalStringArray(&GC.getDiplomacyPowerTypes(), "Civ4Types/DiplomacyPowerTypes/DiplomacyPowerType", &iEnumVal, true);
			iEnumVal = NUM_AUTOMATE_TYPES;
			SetGlobalStringArray(&GC.getAutomateTypes(), "Civ4Types/AutomateTypes/AutomateType", &iEnumVal, true);
			///TKs Med
			iEnumVal = NUM_CITY_TYPES;
			SetGlobalStringArray(&GC.getMedCityTypes(), "Civ4Types/MedCityTypes/MedCityType", &iEnumVal, true);
			iEnumVal = NUM_TRADE_SCREEN_TYPES;
			SetGlobalStringArray(&GC.getTradeScreenTypes(), "Civ4Types/TradeScreenTypes/TradeScreenType", &iEnumVal, true);
			iEnumVal = NUM_MOD_CODE_TYPES;
			SetGlobalStringArray(&GC.getModCodeTypes(), "Civ4Types/ModCodeTypes/ModCodeType", &iEnumVal, true);
			///TKe
			iEnumVal = NUM_DIRECTION_TYPES;
			SetGlobalStringArray(&GC.getDirectionTypes(), "Civ4Types/DirectionTypes/DirectionType", &iEnumVal, true);
			SetGlobalStringArray(&GC.getFootstepAudioTypes(), "Civ4Types/FootstepAudioTypes/FootstepAudioType", &GC.getNumFootstepAudioTypes());

			gDLL->getXMLIFace()->SetToParent(m_pFXml);
			gDLL->getXMLIFace()->SetToParent(m_pFXml);
			SetVariableListTagPair<CvString>(&GC.getFootstepAudioTags(), "FootstepAudioTags", GC.getNumFootstepAudioTypes(), "");
		}
	}

	// delete the pointer to the FXml variable
	DestroyFXml();

	return true;
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetDiplomacyCommentTypes()
//
//  PURPOSE :   Creates a full list of Diplomacy Comments
//
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetDiplomacyCommentTypes(CvString** ppszString, int* iNumVals)
{
	FAssertMsg(false, "should never get here");
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetupGlobalLandscapeInfos()
//
//  PURPOSE :   Initialize the appropriate variables located in globals.cpp/h with the values in
//				Terrain\Civ4TerrainSettings.xml
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::SetupGlobalLandscapeInfo()
{
	// load order: 6
	// loads after main menu
#if 0
	if (!CreateFXml())
	{
		return false;
	}

	LoadGlobalClassInfo(GC.getLandscapeInfo(), "CIV4TerrainSettings", "Terrain", "Civ4TerrainSettings/LandscapeInfos/LandscapeInfo", NULL);
	FAssertMsg(GC.getNumLandscapeInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale

	// delete the pointer to the FXml variable
	DestroyFXml();
#endif
	return true;
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetGlobalArtDefines()
//
//  PURPOSE :   Initialize the appropriate variables located in globals.cpp/h with the values in
//				Civ4ArtDefines.xml
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::SetGlobalArtDefines()
{
	// load order: 3
#if 0
	if (!CreateFXml())
	{
		return false;
	}

	LoadGlobalClassInfo(ARTFILEMGR.getInterfaceArtInfo(), "CIV4ArtDefines_Interface", "Art", "Civ4ArtDefines/InterfaceArtInfos/InterfaceArtInfo", NULL);
	LoadGlobalClassInfo(ARTFILEMGR.getMovieArtInfo(), "CIV4ArtDefines_Movie", "Art", "Civ4ArtDefines/MovieArtInfos/MovieArtInfo", NULL);
	LoadGlobalClassInfo(ARTFILEMGR.getMiscArtInfo(), "CIV4ArtDefines_Misc", "Art", "Civ4ArtDefines/MiscArtInfos/MiscArtInfo", NULL);
	LoadGlobalClassInfo(ARTFILEMGR.getUnitArtInfo(), "CIV4ArtDefines_Unit", "Art", "Civ4ArtDefines/UnitArtInfos/UnitArtInfo", NULL);
	LoadGlobalClassInfo(ARTFILEMGR.getBuildingArtInfo(), "CIV4ArtDefines_Building", "Art", "Civ4ArtDefines/BuildingArtInfos/BuildingArtInfo", NULL);
	LoadGlobalClassInfo(ARTFILEMGR.getCivilizationArtInfo(), "CIV4ArtDefines_Civilization", "Art", "Civ4ArtDefines/CivilizationArtInfos/CivilizationArtInfo", NULL);
	LoadGlobalClassInfo(ARTFILEMGR.getLeaderheadArtInfo(), "CIV4ArtDefines_Leaderhead", "Art", "Civ4ArtDefines/LeaderheadArtInfos/LeaderheadArtInfo", NULL);
	LoadGlobalClassInfo(ARTFILEMGR.getBonusArtInfo(), "CIV4ArtDefines_Bonus", "Art", "Civ4ArtDefines/BonusArtInfos/BonusArtInfo", NULL);
	LoadGlobalClassInfo(ARTFILEMGR.getImprovementArtInfo(), "CIV4ArtDefines_Improvement", "Art", "Civ4ArtDefines/ImprovementArtInfos/ImprovementArtInfo", NULL);
	LoadGlobalClassInfo(ARTFILEMGR.getTerrainArtInfo(), "CIV4ArtDefines_Terrain", "Art", "Civ4ArtDefines/TerrainArtInfos/TerrainArtInfo", NULL);
	LoadGlobalClassInfo(ARTFILEMGR.getFeatureArtInfo(), "CIV4ArtDefines_Feature", "Art", "Civ4ArtDefines/FeatureArtInfos/FeatureArtInfo", NULL);

	DestroyFXml();
#endif

	return true;
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetGlobalText()
//
//  PURPOSE :   Handles all Global Text Infos
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::LoadGlobalText()
{
	CvCacheObject* cache = gDLL->createGlobalTextCacheObject("GlobalText.dat");	// cache file name
	if (!gDLL->cacheRead(cache))
	{
		bool bLoaded = false;

		if (!CreateFXml())
		{
			return false;
		}

		//
		// load all files in the xml text directory
		//
		std::vector<CvString> aszFiles;
		std::vector<CvString> aszModfiles;

		gDLL->enumerateFiles(aszFiles, "xml\\text\\*.xml");

		if (gDLL->isModularXMLLoading())
		{
			gDLL->enumerateFiles(aszModfiles, "modules\\*_CIV4GameText.xml");
			aszFiles.insert(aszFiles.end(), aszModfiles.begin(), aszModfiles.end());
		}

		for(std::vector<CvString>::iterator it = aszFiles.begin(); it != aszFiles.end(); ++it)
		{
			bLoaded = LoadCivXml(m_pFXml, *it); // Load the XML
			if (!bLoaded)
			{
				char	szMessage[1024];
				sprintf( szMessage, "LoadXML call failed for %s. \n Current XML file is: %s", (*it).c_str(), GC.getCurrentXMLFile().GetCString());
				gDLL->MessageBox(szMessage, "XML Load Error");
			}
			if (bLoaded)
			{
				// if the xml is successfully validated
				if (Validate())
				{
					SetGameText("Civ4GameText", "Civ4GameText/TEXT");
				}
			}
		}

		DestroyFXml();

		// write global text info to cache
		bool bOk = gDLL->cacheWrite(cache);
		if (!bLoaded)
		{
			char	szMessage[1024];
			sprintf( szMessage, "Failed writing to Global Text cache. \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
			gDLL->MessageBox(szMessage, "XML Caching Error");
		}
		if (bOk)
		{
			logMsg("Wrote GlobalText to cache");
		}
	}	// didn't read from cache
	else
	{
		logMsg("Read GlobalText from cache");
	}

	gDLL->destroyCache(cache);

	return true;
}

bool CvXMLLoadUtility::LoadBasicInfos()
{
	// load order: 4
	#if 0
	if (!CreateFXml())
	{
		return false;
	}

	LoadGlobalClassInfo(GC.getConceptInfo(), "CIV4BasicInfos", "BasicInfos", "Civ4BasicInfos/ConceptInfos/ConceptInfo", NULL);
	FAssertMsg(GC.getNumConceptInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getCalendarInfo(), "CIV4CalendarInfos", "BasicInfos", "Civ4CalendarInfos/CalendarInfos/CalendarInfo", NULL);
	FAssertMsg(GC.getNumCalendarInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getSeasonInfo(), "CIV4SeasonInfos", "BasicInfos", "Civ4SeasonInfos/SeasonInfos/SeasonInfo", NULL);
	FAssertMsg(GC.getNumSeasonInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getMonthInfo(), "CIV4MonthInfos", "BasicInfos", "Civ4MonthInfos/MonthInfos/MonthInfo", NULL);
	FAssertMsg(GC.getNumMonthInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getDenialInfo(), "CIV4DenialInfos", "BasicInfos", "Civ4DenialInfos/DenialInfos/DenialInfo", NULL);
	FAssertMsg(GC.getNumDenialInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	GC.CheckEnumDenialTypes(); // XML enum check - Nightinggale
	LoadGlobalClassInfo(GC.getInvisibleInfo(), "CIV4InvisibleInfos", "BasicInfos", "Civ4InvisibleInfos/InvisibleInfos/InvisibleInfo", NULL);
	FAssertMsg(GC.getNumInvisibleInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getUnitCombatInfo(), "CIV4UnitCombatInfos", "BasicInfos", "Civ4UnitCombatInfos/UnitCombatInfos/UnitCombatInfo", NULL);
	FAssertMsg(GC.getNumUnitCombatInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getDomainInfo(), "CIV4DomainInfos", "BasicInfos", "Civ4DomainInfos/DomainInfos/DomainInfo", NULL);
	GC.CheckEnumDomainTypes(); // XML enum check - Nightinggale
	LoadGlobalClassInfo(GC.getUnitAIInfo(), "CIV4UnitAIInfos", "BasicInfos", "Civ4UnitAIInfos/UnitAIInfos/UnitAIInfo", NULL);
	GC.CheckEnumUnitAITypes(); // XML enum check - Nightinggale
	LoadGlobalClassInfo(GC.getAttitudeInfo(), "CIV4AttitudeInfos", "BasicInfos", "Civ4AttitudeInfos/AttitudeInfos/AttitudeInfo", NULL);
	GC.CheckEnumAttitudeTypes(); // XML enum check - Nightinggale
	LoadGlobalClassInfo(GC.getMemoryInfo(), "CIV4MemoryInfos", "BasicInfos", "Civ4MemoryInfos/MemoryInfos/MemoryInfo", NULL);
	GC.CheckEnumMemoryTypes(); // XML enum check - Nightinggale
	LoadGlobalClassInfo(GC.getFatherCategoryInfo(), "CIV4FatherCategoryInfos", "BasicInfos", "Civ4FatherCategoryInfos/FatherCategoryInfos/FatherCategoryInfo", NULL);
	FAssertMsg(GC.getNumFatherCategoryInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale

	DestroyFXml();
#endif
	return true;
}

//
// Globals which must be loaded before the main menus.
// Don't put anything in here unless it has to be loaded before the main menus,
// instead try to load things in LoadPostMenuGlobals()
//
bool CvXMLLoadUtility::LoadPreMenuGlobals()
{
	// load order: 5

	if (!CreateFXml())
	{
		return false;
	}

	/// XML load - start - Nightinggale
	bFirstLoadRound = false;
	loadXMLFiles();
	/// XML load - end - Nightinggale
#if 0
	LoadGlobalClassInfo(GC.getColorInfo(), "CIV4ColorVals", "Interface", "Civ4ColorVals/ColorVals/ColorVal", NULL);
	FAssertMsg(GC.getNumColorInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getUnitClassInfo(), "CIV4UnitClassInfos", "Units", "Civ4UnitClassInfos/UnitClassInfos/UnitClassInfo", NULL);
	FAssertMsg(GC.getNumUnitClassInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getCultureLevelInfo(), "CIV4CultureLevelInfo", "GameInfo", "Civ4CultureLevelInfo/CultureLevelInfos/CultureLevelInfo", NULL);
	FAssertMsg(GC.getNumCultureLevelInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getVictoryInfo(), "CIV4VictoryInfo", "GameInfo", "Civ4VictoryInfo/VictoryInfos/VictoryInfo", NULL);
	FAssertMsg(GC.getNumVictoryInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getBuildingClassInfo(), "CIV4BuildingClassInfos", "Buildings", "Civ4BuildingClassInfos/BuildingClassInfos/BuildingClassInfo", NULL);
	FAssertMsg(GC.getNumBuildingClassInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getPlayerColorInfo(), "CIV4PlayerColorInfos", "Interface", "Civ4PlayerColorInfos/PlayerColorInfos/PlayerColorInfo", NULL);
	FAssertMsg(GC.getNumPlayerColorInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getEuropeInfo(), "CIV4EuropeInfo", "GameInfo", "Civ4EuropeInfo/EuropeInfos/EuropeInfo", NULL); // TK Mod XML load order change
	FAssertMsg(GC.getNumEuropeInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getYieldInfo(), "CIV4YieldInfos", "Terrain", "Civ4YieldInfos/YieldInfos/YieldInfo", NULL);
	FAssertMsg(GC.getYieldInfo().size() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	GC.CheckEnumYieldTypes(); // XML enum check - Nightinggale
	LoadGlobalClassInfo(GC.getAlarmInfo(), "CIV4AlarmInfos", "Civilizations", "Civ4AlarmInfos/AlarmInfos/AlarmInfo", NULL);
	FAssertMsg(GC.getNumAlarmInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getGameSpeedInfo(), "CIV4GameSpeedInfo", "GameInfo", "Civ4GameSpeedInfo/GameSpeedInfos/GameSpeedInfo", NULL);
	FAssertMsg(GC.getNumGameSpeedInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getTurnTimerInfo(), "CIV4TurnTimerInfo", "GameInfo", "Civ4TurnTimerInfo/TurnTimerInfos/TurnTimerInfo", NULL);
	FAssertMsg(GC.getNumTurnTimerInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getWorldInfo(), "CIV4WorldInfo", "GameInfo", "Civ4WorldInfo/WorldInfos/WorldInfo", NULL);
	FAssertMsg(GC.getNumWorldInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	GC.CheckEnumWorldSizeTypes(); // XML enum check - Nightinggale
	LoadGlobalClassInfo(GC.getClimateInfo(), "CIV4ClimateInfo", "GameInfo", "Civ4ClimateInfo/ClimateInfos/ClimateInfo", NULL);
	FAssertMsg(GC.getNumClimateInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getSeaLevelInfo(), "CIV4SeaLevelInfo", "GameInfo", "Civ4SeaLevelInfo/SeaLevelInfos/SeaLevelInfo", NULL);
	FAssertMsg(GC.getNumSeaLevelInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getTerrainInfo(), "CIV4TerrainInfos", "Terrain", "Civ4TerrainInfos/TerrainInfos/TerrainInfo", NULL);
	FAssertMsg(GC.getNumTerrainInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getEraInfo(), "CIV4EraInfos", "GameInfo", "Civ4EraInfos/EraInfos/EraInfo", NULL);
	FAssertMsg(GC.getNumEraInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getFeatureInfo(), "Civ4FeatureInfos", "Terrain", "Civ4FeatureInfos/FeatureInfos/FeatureInfo", NULL);
	FAssertMsg(GC.getNumFeatureInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getPromotionInfo(), "CIV4PromotionInfos", "Units", "Civ4PromotionInfos/PromotionInfos/PromotionInfo", &CvDLLUtilityIFaceBase::createPromotionInfoCacheObject);
	FAssertMsg(GC.getNumPromotionInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getProfessionInfo(), "CIV4ProfessionInfos", "Units", "Civ4ProfessionInfos/ProfessionInfos/ProfessionInfo", &CvDLLUtilityIFaceBase::createProfessionInfoCacheObject);
	FAssertMsg(GC.getNumProfessionInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getGoodyInfo(), "CIV4GoodyInfo", "GameInfo", "Civ4GoodyInfo/GoodyInfos/GoodyInfo", NULL);
	FAssertMsg(GC.getNumGoodyInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getTraitInfo(), "CIV4TraitInfos", "Civilizations", "Civ4TraitInfos/TraitInfos/TraitInfo", &CvDLLUtilityIFaceBase::createTraitInfoCacheObject);
	FAssertMsg(GC.getNumTraitInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getSpecialBuildingInfo(), "CIV4SpecialBuildingInfos", "Buildings", "Civ4SpecialBuildingInfos/SpecialBuildingInfos/SpecialBuildingInfo", NULL);
	FAssertMsg(GC.getNumSpecialBuildingInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	for (int i=0; i < GC.getNumProfessionInfos(); ++i)
	{
		GC.getProfessionInfo((ProfessionTypes)i).readPass3();
	}
	LoadGlobalClassInfo(GC.getAnimationCategoryInfo(), "CIV4AnimationInfos", "Units", "Civ4AnimationInfos/AnimationCategories/AnimationCategory", NULL);
	FAssertMsg(GC.getNumAnimationCategoryInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getAnimationPathInfo(), "CIV4AnimationPathInfos", "Units", "Civ4AnimationPathInfos/AnimationPaths/AnimationPath", NULL);
	FAssertMsg(GC.getNumAnimationPathInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getHandicapInfo(), "CIV4HandicapInfo", "GameInfo", "Civ4HandicapInfo/HandicapInfos/HandicapInfo", &CvDLLUtilityIFaceBase::createHandicapInfoCacheObject);
	FAssertMsg(GC.getNumHandicapInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getCursorInfo(), "CIV4CursorInfo", "GameInfo", "Civ4CursorInfo/CursorInfos/CursorInfo", NULL);
	FAssertMsg(GC.getNumCursorInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getCivicOptionInfo(), "CIV4CivicOptionInfos", "GameInfo", "Civ4CivicOptionInfos/CivicOptionInfos/CivicOptionInfo", NULL);
	FAssertMsg(GC.getNumCivicOptionInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getHurryInfo(), "CIV4HurryInfo", "GameInfo", "Civ4HurryInfo/HurryInfos/HurryInfo", NULL);
	FAssertMsg(GC.getNumHurryInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getBuildingInfo(), "CIV4BuildingInfos", "Buildings", "Civ4BuildingInfos/BuildingInfos/BuildingInfo", &CvDLLUtilityIFaceBase::createBuildingInfoCacheObject);
	FAssertMsg(GC.getNumBuildingInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	for (int i=0; i < GC.getNumBuildingClassInfos(); ++i)
	{
		GC.getBuildingClassInfo((BuildingClassTypes)i).readPass3();
	}
	LoadGlobalClassInfo(GC.getBonusInfo(), "CIV4BonusInfos", "Terrain", "Civ4BonusInfos/BonusInfos/BonusInfo", &CvDLLUtilityIFaceBase::createBonusInfoCacheObject);
	FAssertMsg(GC.getNumBonusInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getRouteInfo(), "Civ4RouteInfos", "Misc", "Civ4RouteInfos/RouteInfos/RouteInfo", NULL);
	FAssertMsg(GC.getNumRouteInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getImprovementInfo(), "CIV4ImprovementInfos", "Terrain", "Civ4ImprovementInfos/ImprovementInfos/ImprovementInfo", &CvDLLUtilityIFaceBase::createImprovementInfoCacheObject);
	FAssertMsg(GC.getNumImprovementInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getFatherPointInfo(), "CIV4FatherPointInfos", "GameInfo", "Civ4FatherPointInfos/FatherPointInfos/FatherPointInfo", NULL);
	FAssertMsg(GC.getNumFatherPointInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getFatherInfo(), "CIV4FatherInfos", "GameInfo", "Civ4FatherInfos/FatherInfos/FatherInfo", &CvDLLUtilityIFaceBase::createFatherInfoCacheObject);
	FAssertMsg(GC.getNumFatherInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getSpecialUnitInfo(), "CIV4SpecialUnitInfos", "Units", "Civ4SpecialUnitInfos/SpecialUnitInfos/SpecialUnitInfo", NULL);
	FAssertMsg(GC.getNumSpecialUnitInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getCivicInfo(), "CIV4CivicInfos", "GameInfo", "Civ4CivicInfos/CivicInfos/CivicInfo", &CvDLLUtilityIFaceBase::createCivicInfoCacheObject);
	FAssertMsg(GC.getNumCivicInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getLeaderHeadInfo(), "CIV4LeaderHeadInfos", "Civilizations", "Civ4LeaderHeadInfos/LeaderHeadInfos/LeaderHeadInfo", &CvDLLUtilityIFaceBase::createLeaderHeadInfoCacheObject);
	FAssertMsg(GC.getNumLeaderHeadInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	
	LoadGlobalClassInfo(GC.getEffectInfo(), "CIV4EffectInfos", "Misc", "Civ4EffectInfos/EffectInfos/EffectInfo", NULL);
	FAssertMsg(GC.getNumEffectInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getEntityEventInfo(), "CIV4EntityEventInfos", "Units", "Civ4EntityEventInfos/EntityEventInfos/EntityEventInfo", NULL);
	FAssertMsg(GC.getNumEntityEventInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getBuildInfo(), "CIV4BuildInfos", "Units", "Civ4BuildInfos/BuildInfos/BuildInfo", NULL);
	FAssertMsg(GC.getNumBuildInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getUnitInfo(), "CIV4UnitInfos", "Units", "Civ4UnitInfos/UnitInfos/UnitInfo", &CvDLLUtilityIFaceBase::createUnitInfoCacheObject);
	FAssertMsg(GC.getNumUnitInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	for (int i=0; i < GC.getNumUnitClassInfos(); ++i)
	{
		GC.getUnitClassInfo((UnitClassTypes)i).readPass3();
	}
    //Androrc UnitArtStyles
	LoadGlobalClassInfo(GC.getUnitArtStyleTypeInfo(), "CIV4UnitArtStyleTypeInfos", "Civilizations", "Civ4UnitArtStyleTypeInfos/UnitArtStyleTypeInfos/UnitArtStyleTypeInfo", false);
	FAssertMsg(GC.getNumUnitArtStyleTypeInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	//Androrc End
	LoadGlobalClassInfo(GC.getCivilizationInfo(), "CIV4CivilizationInfos", "Civilizations", "Civ4CivilizationInfos/CivilizationInfos/CivilizationInfo", &CvDLLUtilityIFaceBase::createCivilizationInfoCacheObject);
	FAssertMsg(GC.getNumCivilizationInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getHints(), "CIV4Hints", "GameInfo", "Civ4Hints/HintInfos/HintInfo", NULL);
	LoadGlobalClassInfo(GC.getMainMenus(), "CIV4MainMenus", "Art", "Civ4MainMenus/MainMenus/MainMenu", NULL);
	LoadGlobalClassInfo(GC.getSlideShowInfo(), "CIV4SlideShowInfos", "Interface", "Civ4SlideShowInfos/SlideShowInfos/SlideShowInfo", NULL);
	FAssertMsg(GC.getNumSlideShowInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getSlideShowRandomInfo(), "CIV4SlideShowRandomInfos", "Interface", "Civ4SlideShowRandomInfos/SlideShowRandomInfos/SlideShowRandomInfo", NULL);
	FAssertMsg(GC.getNumSlideShowRandomInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getWorldPickerInfo(), "CIV4WorldPickerInfos", "Interface", "Civ4WorldPickerInfos/WorldPickerInfos/WorldPickerInfo", NULL);
	FAssertMsg(GC.getNumWorldPickerInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale

	LoadGlobalClassInfo(GC.getGameOptionInfo(), "CIV4GameOptionInfos", "GameInfo", "Civ4GameOptionInfos/GameOptionInfos/GameOptionInfo", NULL);
	FAssertMsg(GC.getNumGameOptionInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	GC.CheckEnumGameOptionTypes(); // XML enum check - Nightinggale
	LoadGlobalClassInfo(GC.getMPOptionInfo(), "CIV4MPOptionInfos", "GameInfo", "Civ4MPOptionInfos/MPOptionInfos/MPOptionInfo", NULL);
	FAssertMsg(GC.getNumMPOptionInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getForceControlInfo(), "CIV4ForceControlInfos", "GameInfo", "Civ4ForceControlInfos/ForceControlInfos/ForceControlInfo", NULL);
	FAssertMsg(GC.getNumForceControlInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	GC.CheckEnumForceControlTypes(); // XML enum check - Nightinggale
#endif
	// add types to global var system
	for (int i = 0; i < GC.getNumCursorInfos(); ++i)
	{
		int iVal;
		CvString szType = GC.getCursorInfo((CursorTypes)i).getType();
		if (GC.getDefinesVarSystem()->GetValue(szType, iVal))
		{
			char szMessage[1024];
			sprintf(szMessage, "cursor type already set? \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
			gDLL->MessageBox(szMessage, "XML Error");
		}
		GC.getDefinesVarSystem()->SetValue(szType, i);
	}


	UpdateProgressCB("GlobalOther");

	DestroyFXml();

	return true;
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   LoadPostMenuGlobals()
//
//  PURPOSE :   loads global xml data which isn't needed for the main menus
//		this data is loaded as a secodn stage, when the game is launched
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::LoadPostMenuGlobals()
{
	// load order: 7
	// loads after main menu
#if 0
	PROFILE_FUNC();
	if (!CreateFXml())
	{
		return false;
	}

	UpdateProgressCB("Global Events");

	LoadGlobalClassInfo(GC.getEventInfo(), "CIV4EventInfos", "Events", "Civ4EventInfos/EventInfos/EventInfo", &CvDLLUtilityIFaceBase::createEventInfoCacheObject);
	FAssertMsg(GC.getNumEventInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getEventTriggerInfo(), "CIV4EventTriggerInfos", "Events", "Civ4EventTriggerInfos/EventTriggerInfos/EventTriggerInfo", &CvDLLUtilityIFaceBase::createEventTriggerInfoCacheObject);
	FAssertMsg(GC.getNumEventTriggerInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale

	UpdateProgressCB("Global Routes");

	LoadGlobalClassInfo(GC.getRouteModelInfo(), "Civ4RouteModelInfos", "Art", "Civ4RouteModelInfos/RouteModelInfos/RouteModelInfo", NULL);
	FAssertMsg(GC.getNumRouteModelInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale

	UpdateProgressCB("Global Rivers");

	LoadGlobalClassInfo(GC.getRiverModelInfo(), "CIV4RiverModelInfos", "Art", "Civ4RiverModelInfos/RiverModelInfos/RiverModelInfo", NULL);
	FAssertMsg(GC.getNumRiverModelInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale

	UpdateProgressCB("Global Other");

	LoadGlobalClassInfo(GC.getWaterPlaneInfo(), "CIV4WaterPlaneInfos", "Misc", "Civ4WaterPlaneInfos/WaterPlaneInfos/WaterPlaneInfo", NULL);
	FAssertMsg(GC.getNumWaterPlaneInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getTerrainPlaneInfo(), "CIV4TerrainPlaneInfos", "Misc", "Civ4TerrainPlaneInfos/TerrainPlaneInfos/TerrainPlaneInfo", NULL);
	FAssertMsg(GC.getNumTerrainPlaneInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale
	LoadGlobalClassInfo(GC.getCameraOverlayInfo(), "CIV4CameraOverlayInfos", "Misc", "Civ4CameraOverlayInfos/CameraOverlayInfos/CameraOverlayInfo", NULL);
	FAssertMsg(GC.getNumCameraOverlayInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale

	UpdateProgressCB("Global Emphasize");

	LoadGlobalClassInfo(GC.getEmphasizeInfo(), "CIV4EmphasizeInfo", "GameInfo", "Civ4EmphasizeInfo/EmphasizeInfos/EmphasizeInfo", NULL);
	FAssertMsg(GC.getNumEmphasizeInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale

	UpdateProgressCB("Global Other");

	LoadGlobalClassInfo(GC.getMissionInfo(), "CIV4MissionInfos", "Units", "Civ4MissionInfos/MissionInfos/MissionInfo", NULL);
	GC.CheckEnumMissionTypes(); // XML enum check - Nightinggale
	LoadGlobalClassInfo(GC.getControlInfo(), "CIV4ControlInfos", "Units", "Civ4ControlInfos/ControlInfos/ControlInfo", NULL);
	GC.CheckEnumControlTypes(); // XML enum check - Nightinggale
	LoadGlobalClassInfo(GC.getCommandInfo(), "CIV4CommandInfos", "Units", "Civ4CommandInfos/CommandInfos/CommandInfo", NULL);
	GC.CheckEnumCommandTypes(); // XML enum check - Nightinggale
	LoadGlobalClassInfo(GC.getAutomateInfo(), "CIV4AutomateInfos", "Units", "Civ4AutomateInfos/AutomateInfos/AutomateInfo", NULL);
	FAssertMsg(GC.getNumAutomateInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale

	UpdateProgressCB("Global Interface");

	LoadGlobalClassInfo(GC.getInterfaceModeInfo(), "CIV4InterfaceModeInfos", "Interface", "Civ4InterfaceModeInfos/InterfaceModeInfos/InterfaceModeInfo", NULL);
	GC.CheckEnumInterfaceModeTypes(); // XML enum check - Nightinggale

	SetGlobalActionInfo();


	// Load the formation info
	LoadGlobalClassInfo(GC.getUnitFormationInfo(), "CIV4FormationInfos", "Units", "UnitFormations/UnitFormation", NULL);
	FAssertMsg(GC.getNumUnitFormationInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale

	// Load the attachable infos
	LoadGlobalClassInfo(GC.getAttachableInfo(), "CIV4AttachableInfos", "Misc", "Civ4AttachableInfos/AttachableInfos/AttachableInfo", NULL);
	FAssertMsg(GC.getNumAttachableInfos() == GC.XMLlength, CvString::format("XML read error. \"%s\" is used more than once", f_szXMLname)); // XML length check - Nightinggale

	// Specail Case Diplomacy Info due to double vectored nature and appending of Responses
	LoadDiplomacyInfo(GC.getDiplomacyInfo(), "CIV4DiplomacyInfos", "GameInfo", "Civ4DiplomacyInfos/DiplomacyInfos/DiplomacyInfo", &CvDLLUtilityIFaceBase::createDiplomacyInfoCacheObject);

	DestroyFXml();
#endif
	return true;
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetGlobalStringArray(TCHAR (**ppszString)[256], char* szTagName, int* iNumVals)
//
//  PURPOSE :   takes the szTagName parameter and if it finds it in the m_pFXml member variable
//				then it loads the ppszString parameter with the string values under it and the
//				iNumVals with the total number of tags with the szTagName in the xml file
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetGlobalStringArray(CvString **ppszString, char* szTagName, int* iNumVals, bool bUseEnum)
{
	PROFILE_FUNC();
	logMsg("SetGlobalStringArray %s\n", szTagName);

	int i=0;					//loop counter
	CvString *pszString;	// hold the local pointer to the newly allocated string memory
	pszString = NULL;			// null out the local string pointer so that it can be checked at the
	// end of the function in an FAssert

	// if we locate the szTagName, the current node is set to the first instance of the tag name in the xml file
	if (gDLL->getXMLIFace()->LocateNode(m_pFXml,szTagName))
	{
		if (!bUseEnum)
		{
			// get the total number of times this tag appears in the xml
			*iNumVals = gDLL->getXMLIFace()->NumOfElementsByTagName(m_pFXml,szTagName);
		}
		// initialize the memory based on the total number of tags in the xml and the 256 character length we selected
		*ppszString = new CvString[*iNumVals];
		// set the local pointer to the memory just allocated
		pszString = *ppszString;

		// loop through each of the tags
		for (i=0;i<*iNumVals;i++)
		{
			// get the string value at the current node
			GetXmlVal(pszString[i]);
			GC.setInfoTypeFromString(pszString[i], i);

			// if can't set the current node to a sibling node we will break out of the for loop
			// otherwise we will keep looping
			if (!gDLL->getXMLIFace()->NextSibling(m_pFXml))
			{
				break;
			}
		}
	}

	// if the local string pointer is null then we weren't able to find the szTagName in the xml
	// so we will FAssert to let whoever know it
	if (!pszString)
	{
		char	szMessage[1024];
		sprintf( szMessage, "Error locating tag node in SetGlobalStringArray function \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
}




//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetGlobalActionInfo(CvActionInfo** ppActionInfo, int* iNumVals)
//
//  PURPOSE :   Takes the szTagName parameter and if it exists in the xml it loads the ppActionInfo
//				with the value under it and sets the value of the iNumVals parameter to the total number
//				of occurances of the szTagName tag in the xml.
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetGlobalActionInfo()
{
	PROFILE_FUNC();
	logMsg("SetGlobalActionInfo\n");
	int i=0;					//loop counter

	if(!(NUM_INTERFACEMODE_TYPES > 0))
	{
		char	szMessage[1024];
		sprintf( szMessage, "NUM_INTERFACE_TYPES is not greater than zero in CvXMLLoadUtility::SetGlobalActionInfo \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
	if(!(GC.getNumBuildInfos() > 0))
	{
		char	szMessage[1024];
		sprintf( szMessage, "GC.getNumBuildInfos() is not greater than zero in CvXMLLoadUtility::SetGlobalActionInfo \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
	if(!(GC.getNumPromotionInfos() > 0))
	{
		char	szMessage[1024];
		sprintf( szMessage, "GC.getNumPromotionInfos() is not greater than zero in CvXMLLoadUtility::SetGlobalActionInfo \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
	if(!(GC.getNumUnitClassInfos() > 0) )
	{
		char	szMessage[1024];
		sprintf( szMessage, "GC.getNumUnitClassInfos() is not greater than zero in CvXMLLoadUtility::SetGlobalActionInfo \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
	if(!(GC.getNumBuildingInfos() > 0) )
	{
		char	szMessage[1024];
		sprintf( szMessage, "GC.getNumBuildingInfos() is not greater than zero in CvXMLLoadUtility::SetGlobalActionInfo \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
	if(!(NUM_CONTROL_TYPES > 0) )
	{
		char	szMessage[1024];
		sprintf( szMessage, "NUM_CONTROL_TYPES is not greater than zero in CvXMLLoadUtility::SetGlobalActionInfo \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
	if(!(GC.getNumAutomateInfos() > 0) )
	{
		char	szMessage[1024];
		sprintf( szMessage, "GC.getNumAutomateInfos() is not greater than zero in CvXMLLoadUtility::SetGlobalActionInfo \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
	if(!(NUM_COMMAND_TYPES > 0) )
	{
		char	szMessage[1024];
		sprintf( szMessage, "NUM_COMMAND_TYPES is not greater than zero in CvXMLLoadUtility::SetGlobalActionInfo \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
	if(!(NUM_MISSION_TYPES > 0) )
	{
		char	szMessage[1024];
		sprintf( szMessage, "NUM_MISSION_TYPES is not greater than zero in CvXMLLoadUtility::SetGlobalActionInfo \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}

	int* piOrderedIndex=NULL;

	int iNumOrigVals = GC.getNumActionInfos();

	int iNumActionInfos = iNumOrigVals +
		NUM_INTERFACEMODE_TYPES +
		GC.getNumBuildInfos() +
		GC.getNumPromotionInfos() +
		GC.getNumUnitInfos() +
		NUM_CONTROL_TYPES +
		NUM_COMMAND_TYPES +
		GC.getNumAutomateInfos() +
		NUM_MISSION_TYPES;

	int* piIndexList = new int[iNumActionInfos];
	int* piPriorityList = new int[iNumActionInfos];
	int* piActionInfoTypeList = new int[iNumActionInfos];

	int iTotalActionInfoCount = 0;

	// loop through control info
	for (i=0;i<NUM_COMMAND_TYPES;i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getCommandInfo((CommandTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_COMMAND;
		iTotalActionInfoCount++;
	}

	for (i=0;i<NUM_INTERFACEMODE_TYPES;i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getInterfaceModeInfo((InterfaceModeTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_INTERFACEMODE;
		iTotalActionInfoCount++;
	}

	for (i=0;i<GC.getNumBuildInfos();i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getBuildInfo((BuildTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_BUILD;
		iTotalActionInfoCount++;
	}

	for (i=0;i<GC.getNumPromotionInfos();i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getPromotionInfo((PromotionTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_PROMOTION;
		iTotalActionInfoCount++;
	}

	for (i=0;i<GC.getNumUnitInfos();i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getUnitInfo((UnitTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_UNIT;
		iTotalActionInfoCount++;
	}

	for (i=0;i<NUM_CONTROL_TYPES;i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getControlInfo((ControlTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_CONTROL;
		iTotalActionInfoCount++;
	}

	for (i=0;i<GC.getNumAutomateInfos();i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getAutomateInfo(i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_AUTOMATE;
		iTotalActionInfoCount++;
	}

	for (i=0;i<NUM_MISSION_TYPES;i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getMissionInfo((MissionTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_MISSION;
		iTotalActionInfoCount++;
	}

	SAFE_DELETE_ARRAY(piOrderedIndex);
	piOrderedIndex = new int[iNumActionInfos];

	orderHotkeyInfo(&piOrderedIndex, piPriorityList, iNumActionInfos);
	for (i=0;i<iNumActionInfos;i++)
	{
		CvActionInfo* pActionInfo = new CvActionInfo;
		pActionInfo->setOriginalIndex(piIndexList[piOrderedIndex[i]]);
		pActionInfo->setSubType((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]]);
		if ((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_COMMAND)
		{
			GC.getCommandInfo((CommandTypes)piIndexList[piOrderedIndex[i]]).setActionInfoIndex(i);
		}
		else if ((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_INTERFACEMODE)
		{
			GC.getInterfaceModeInfo((InterfaceModeTypes)piIndexList[piOrderedIndex[i]]).setActionInfoIndex(i);
		}
		else if ((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_BUILD)
		{
			GC.getBuildInfo((BuildTypes)piIndexList[piOrderedIndex[i]]).setMissionType(FindInInfoClass("MISSION_BUILD"));
			GC.getBuildInfo((BuildTypes)piIndexList[piOrderedIndex[i]]).setActionInfoIndex(i);
		}
		else if ((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_PROMOTION)
		{
			GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).setCommandType(FindInInfoClass("COMMAND_PROMOTION"));
			GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).setActionInfoIndex(i);
			GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).setHotKeyDescription(GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).getTextKeyWide(), GC.getCommandInfo((CommandTypes)(GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).getCommandType())).getTextKeyWide(), CreateHotKeyFromDescription(GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).getHotKey(), GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).isShiftDown(), GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).isAltDown(), GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).isCtrlDown()));
		}
		else if ((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_UNIT)
		{
			GC.getUnitInfo((UnitTypes)piIndexList[piOrderedIndex[i]]).setCommandType(FindInInfoClass("COMMAND_UPGRADE"));
			GC.getUnitInfo((UnitTypes)piIndexList[piOrderedIndex[i]]).setActionInfoIndex(i);
			GC.getUnitInfo((UnitTypes)piIndexList[piOrderedIndex[i]]).setHotKeyDescription(GC.getUnitInfo((UnitTypes)piIndexList[piOrderedIndex[i]]).getTextKeyWide(), GC.getCommandInfo((CommandTypes)(GC.getUnitInfo((UnitTypes)piIndexList[piOrderedIndex[i]]).getCommandType())).getTextKeyWide(), CreateHotKeyFromDescription(GC.getUnitInfo((UnitTypes)piIndexList[piOrderedIndex[i]]).getHotKey(), GC.getUnitInfo((UnitTypes)piIndexList[piOrderedIndex[i]]).isShiftDown(), GC.getUnitInfo((UnitTypes)piIndexList[piOrderedIndex[i]]).isAltDown(), GC.getUnitInfo((UnitTypes)piIndexList[piOrderedIndex[i]]).isCtrlDown()));
		}
		else if ((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_CONTROL)
		{
			GC.getControlInfo((ControlTypes)piIndexList[piOrderedIndex[i]]).setActionInfoIndex(i);
		}
		else if ((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_AUTOMATE)
		{
			GC.getAutomateInfo(piIndexList[piOrderedIndex[i]]).setActionInfoIndex(i);
		}
		else if ((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_MISSION)
		{
			GC.getMissionInfo((MissionTypes)piIndexList[piOrderedIndex[i]]).setActionInfoIndex(i + iNumOrigVals);
		}

		GC.getActionInfo().push_back(pActionInfo);
	}
	GC.addToInfosVectors(&GC.getActionInfo());

	SAFE_DELETE_ARRAY(piOrderedIndex);
	SAFE_DELETE_ARRAY(piIndexList);
	SAFE_DELETE_ARRAY(piPriorityList);
	SAFE_DELETE_ARRAY(piActionInfoTypeList);
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetGlobalAnimationPathInfo(CvAnimationPathInfo** ppAnimationPathInfo, char* szTagName, int* iNumVals)
//
//  PURPOSE :   Takes the szTagName parameter and if it exists in the xml it loads the ppAnimationPathInfo
//				with the value under it and sets the value of the iNumVals parameter to the total number
//				of occurances of the szTagName tag in the xml.
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetGlobalAnimationPathInfo(CvAnimationPathInfo** ppAnimationPathInfo, char* szTagName, int* iNumVals)
{
	PROFILE_FUNC();
	logMsg( "SetGlobalAnimationPathInfo %s\n", szTagName );

	int		i;						// Loop counters
	CvAnimationPathInfo * pAnimPathInfo = NULL;	// local pointer to the domain info memory

	if ( gDLL->getXMLIFace()->LocateNode(m_pFXml, szTagName ))
	{
		// get the number of times the szTagName tag appears in the xml file
		*iNumVals = gDLL->getXMLIFace()->NumOfElementsByTagName(m_pFXml,szTagName);

		// allocate memory for the domain info based on the number above
		*ppAnimationPathInfo = new CvAnimationPathInfo[*iNumVals];
		pAnimPathInfo = *ppAnimationPathInfo;

		gDLL->getXMLIFace()->SetToParent(m_pFXml);
		gDLL->getXMLIFace()->SetToChild(m_pFXml);
		gDLL->getXMLIFace()->SetToChild(m_pFXml);


		// Loop through each tag.
		for (i=0;i<*iNumVals;i++)
		{
			SkipToNextVal();	// skip to the next non-comment node

			if (!pAnimPathInfo[i].read(this))
				break;
			GC.setInfoTypeFromString(pAnimPathInfo[i].getType(), i);	// add type to global info type hash map
			if (!gDLL->getXMLIFace()->NextSibling(m_pFXml))
			{
				break;
			}
		}
	}

	// if we didn't find the tag name in the xml then we never set the local pointer to the
	// newly allocated memory and there for we will FAssert to let people know this most
	// interesting fact
	if(!pAnimPathInfo )
	{
		char	szMessage[1024];
		sprintf( szMessage, "Error finding tag node in SetGlobalAnimationPathInfo function \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetGlobalUnitScales(float* pfLargeScale, float* pfSmallScale, char* szTagName)
//
//  PURPOSE :   Takes the szTagName parameter and if it exists in the xml it loads the ppPromotionInfo
//				with the value under it and sets the value of the iNumVals parameter to the total number
//				of occurances of the szTagName tag in the xml.
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetGlobalUnitScales(float* pfLargeScale, float* pfSmallScale, char* szTagName)
{
	PROFILE_FUNC();
	logMsg("SetGlobalUnitScales %s\n", szTagName);
	// if we successfully locate the szTagName node
	if (gDLL->getXMLIFace()->LocateNode(m_pFXml,szTagName))
	{
		// call the function that sets the FXml pointer to the first non-comment child of
		// the current tag and gets the value of that new node
		if (GetChildXmlVal(pfLargeScale))
		{
			// set the current xml node to it's next sibling and then
			// get the sibling's TCHAR value
			GetNextXmlVal(pfSmallScale);

			// set the current xml node to it's parent node
			gDLL->getXMLIFace()->SetToParent(m_pFXml);
		}
	}
	else
	{
		// if we didn't find the tag name in the xml then we never set the local pointer to the
		// newly allocated memory and there for we will FAssert to let people know this most
		// interesting fact
		char	szMessage[1024];
		sprintf( szMessage, "Error finding tag node in SetGlobalUnitScales function \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetGameText()
//
//  PURPOSE :   Reads game text info from XML and adds it to the translation manager
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetGameText(const char* szTextGroup, const char* szTagName)
{
	PROFILE_FUNC();
	logMsg("SetGameText %s\n", szTagName);
	int i=0;		//loop counter - Index into pTextInfo

	if (gDLL->getXMLIFace()->LocateNode(m_pFXml, szTextGroup)) // Get the Text Group 1st
	{
		int iNumVals = gDLL->getXMLIFace()->GetNumChildren(m_pFXml);	// Get the number of Children that the Text Group has
		gDLL->getXMLIFace()->LocateNode(m_pFXml, szTagName); // Now switch to the TEXT Tag
		gDLL->getXMLIFace()->SetToParent(m_pFXml);
		gDLL->getXMLIFace()->SetToChild(m_pFXml);

		// loop through each tag
		for (i=0; i < iNumVals; i++)
		{
			CvGameText textInfo;
			textInfo.read(this);

			gDLL->addText(textInfo.getType() /*id*/, textInfo.getText(), textInfo.getGender(), textInfo.getPlural());
			if (!gDLL->getXMLIFace()->NextSibling(m_pFXml) && i!=iNumVals-1)
			{
				char	szMessage[1024];
				sprintf( szMessage, "failed to find sibling \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
				gDLL->MessageBox(szMessage, "XML Error");
				break;
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetGlobalClassInfo - This is a template function that is USED FOR ALMOST ALL INFO CLASSES.
//		Each info class should have a read(CvXMLLoadUtility*) function that is responsible for initializing
//		the class from xml data.
//
//  PURPOSE :   takes the szTagName parameter and loads the ppszString with the text values
//				under the tags.  This will be the hints displayed during game initialization and load
//
//------------------------------------------------------------------------------------------------------
template <class T>
void CvXMLLoadUtility::SetGlobalClassInfo(std::vector<T*>& aInfos, const char* szTagName)
{
	char szLog[256];
	sprintf(szLog, "SetGlobalClassInfo (%s)", szTagName);
	PROFILE(szLog);
	logMsg(szLog);

	// if we successfully locate the tag name in the xml file
	if (gDLL->getXMLIFace()->LocateNode(m_pFXml, szTagName))
	{
		int iSub = -1; /// info subclass - Nightinggale
		// loop through each tag
		do
		{
			SkipToNextVal();	// skip to the next non-comment node

			T* pClassInfo = new T;

			FAssert(NULL != pClassInfo);
			if (NULL == pClassInfo)
			{
				break;
			}

			/// XML load - start - Nightinggale
			//bool bSuccess = pClassInfo->read(this);
			bool bSuccess = false;
			if (!bLoadOnce)
			{
				bSuccess = pClassInfo->readSub(this, &iSub);
			}
			if (!bFirstLoadRound)
			{
				bSuccess = pClassInfo->read(this);
			}
			/// XML load - end - Nightinggale
			FAssert(bSuccess);
			if (!bSuccess)
			{
				delete pClassInfo;
				break;
			}

			GC.XMLlength++; // XML length check - Nightinggale

			int iIndex = -1;
			if (NULL != pClassInfo->getType())
			{
				iIndex = GC.getInfoTypeForString(pClassInfo->getType(), true);
			}

			if (-1 == iIndex)
			{
				aInfos.push_back(pClassInfo);
				if (NULL != pClassInfo->getType())
				{
					GC.setInfoTypeFromString(pClassInfo->getType(), (int)aInfos.size() - 1);	// add type to global info type hash map
				}
			}
			else
			{
// XML length check - start - Nightinggale
				FAssertMsg(!bLoadOnce && !bFirstLoadRound, CvString::format("Type %s already in use when read in %s", pClassInfo->getType(), szTagName).c_str());
#ifdef FASSERT_ENABLE
				if (strlen(pClassInfo->getType()) >= sizeof(f_szXMLname))
				{
					strncpy(f_szXMLname, pClassInfo->getType(), sizeof(f_szXMLname));
				} else {
					// string can't overflow. Use the function, which skips NULL padding the entire char array
					strcpy(f_szXMLname, pClassInfo->getType());
				}
#endif
// XML length check - end - Nightinggale
				SAFE_DELETE(aInfos[iIndex]);
				aInfos[iIndex] = pClassInfo;
			}

		/// info subclass - start - Nightinggale
		//} while (gDLL->getXMLIFace()->NextSibling(m_pFXml));
		} while (iSub != -1 || gDLL->getXMLIFace()->NextSibling(m_pFXml));
		/// info subclass - end - Nightinggale

		//readPass2
		{
			PROFILE("CvXMLLoadUtility::SetGlobalClassInfo::readPass2");

			// if we successfully locate the szTagName node
			if (gDLL->getXMLIFace()->LocateNode(m_pFXml, szTagName))
			{
				gDLL->getXMLIFace()->SetToParent(m_pFXml);
				gDLL->getXMLIFace()->SetToChild(m_pFXml);

				// loop through each tag
				for (std::vector<T*>::iterator it = aInfos.begin(); it != aInfos.end(); ++it)
				{
					(*it)->readPass2(this);

					if (!gDLL->getXMLIFace()->NextSibling(m_pFXml))
					{
						break;
					}
				}
			}
		}
	}
}

void CvXMLLoadUtility::SetDiplomacyInfo(std::vector<CvDiplomacyInfo*>& DiploInfos, const char* szTagName)
{
	char szLog[256];
	sprintf(szLog, "SetDiplomacyInfo (%s)", szTagName);
	PROFILE(szLog);
	logMsg(szLog);

	// if we successfully locate the tag name in the xml file
	if (gDLL->getXMLIFace()->LocateNode(m_pFXml, szTagName))
	{
		// loop through each tag
		do
		{
			GC.XMLlength++; // XML length check - Nightinggale
			SkipToNextVal();	// skip to the next non-comment node

			CvString szType;
			GetChildXmlValByName(szType, "Type");
			int iIndex = GC.getInfoTypeForString(szType, true);

			if (-1 == iIndex)
			{
				CvDiplomacyInfo* pClassInfo = new CvDiplomacyInfo;

				if (NULL == pClassInfo)
				{
					FAssert(false);
					break;
				}

				pClassInfo->read(this);
				if (NULL != pClassInfo->getType())
				{
					GC.setInfoTypeFromString(pClassInfo->getType(), (int)DiploInfos.size());	// add type to global info type hash map
				}
				DiploInfos.push_back(pClassInfo);
			}
			else
			{
				DiploInfos[iIndex]->read(this);
			}

		} while (gDLL->getXMLIFace()->NextSibling(m_pFXml));
	}
}

template <class T>
void CvXMLLoadUtility::LoadGlobalClassInfo(std::vector<T*>& aInfos, const char* szFileRoot, const char* szFileDirectory, const char* szXmlPath, CvCacheObject* (CvDLLUtilityIFaceBase::*pArgFunction) (const TCHAR*))
{
	bool bLoaded = false;
	bool bWriteCache = true;
	CvCacheObject* pCache = NULL;
	GC.addToInfosVectors(&aInfos);

	// XML length check - start - Nightinggale
	GC.XMLlength = 0;
#ifdef FASSERT_ENABLE
	f_szXMLname[0] = 0;
#endif
	// XML length check - end - Nightinggale

	if (NULL != pArgFunction)
	{
		pCache = (gDLL->*pArgFunction)(CvString::format("%s.dat", szFileRoot));	// cache file name

		if (gDLL->cacheRead(pCache, CvString::format("xml\\\\%s\\\\%s.xml", szFileDirectory, szFileRoot)))
		{
			logMsg("Read %s from cache", szFileDirectory);
			bLoaded = true;
			bWriteCache = false;
		}
	}

	if (!bLoaded)
	{
		bLoaded = LoadCivXml(m_pFXml, CvString::format("xml\\%s/%s.xml", szFileDirectory, szFileRoot));

		if (!bLoaded)
		{
			char szMessage[1024];
			sprintf(szMessage, "LoadXML call failed for %s.", CvString::format("%s/%s.xml", szFileDirectory, szFileRoot).GetCString());
			gDLL->MessageBox(szMessage, "XML Load Error");
		}
		else if (Validate())
		{
			SetGlobalClassInfo(aInfos, szXmlPath);

			if (gDLL->isModularXMLLoading())
			{
				std::vector<CvString> aszFiles;
				gDLL->enumerateFiles(aszFiles, CvString::format("modules\\*_%s.xml", szFileRoot));  // search for the modular files

				for (std::vector<CvString>::iterator it = aszFiles.begin(); it != aszFiles.end(); ++it)
				{
					bLoaded = LoadCivXml(m_pFXml, *it);

					if (!bLoaded)
					{
						char szMessage[1024];
						sprintf(szMessage, "LoadXML call failed for %s.", (*it).GetCString());
						gDLL->MessageBox(szMessage, "XML Load Error");
					}
					else if (Validate())
					{
						SetGlobalClassInfo(aInfos, szXmlPath);
					}
				}
			}

			if (NULL != pArgFunction && bWriteCache)
			{
				// write info to cache
				bool bOk = gDLL->cacheWrite(pCache);
				if (!bOk)
				{
					char szMessage[1024];
					sprintf(szMessage, "Failed writing to %s cache. \n Current XML file is: %s", szFileDirectory, GC.getCurrentXMLFile().GetCString());
					gDLL->MessageBox(szMessage, "XML Caching Error");
				}
				if (bOk)
				{
					logMsg("Wrote %s to cache", szFileDirectory);
				}
			}
		}
	}

	if (NULL != pArgFunction)
	{
		gDLL->destroyCache(pCache);
	}
}


void CvXMLLoadUtility::LoadDiplomacyInfo(std::vector<CvDiplomacyInfo*>& DiploInfos, const char* szFileRoot, const char* szFileDirectory, const char* szXmlPath, CvCacheObject* (CvDLLUtilityIFaceBase::*pArgFunction) (const TCHAR*))
{
	bool bLoaded = false;
	bool bWriteCache = true;
	CvCacheObject* pCache = NULL;
	GC.addToInfosVectors(&DiploInfos);

	// XML length check - start - Nightinggale
	GC.XMLlength = 0;
#ifdef FASSERT_ENABLE
	f_szXMLname[0] = 0;
#endif
	// XML length check - end - Nightinggale

	if (NULL != pArgFunction)
	{
		pCache = (gDLL->*pArgFunction)(CvString::format("%s.dat", szFileRoot));	// cache file name

		if (gDLL->cacheRead(pCache, CvString::format("xml\\\\%s\\\\%s.xml", szFileDirectory, szFileRoot)))
		{
			logMsg("Read %s from cache", szFileDirectory);
			bLoaded = true;
			bWriteCache = false;
		}
	}

	if (!bLoaded)
	{
		bLoaded = LoadCivXml(m_pFXml, CvString::format("xml\\%s/%s.xml", szFileDirectory, szFileRoot));

		if (!bLoaded)
		{
			char szMessage[1024];
			sprintf(szMessage, "LoadXML call failed for %s.", CvString::format("%s/%s.xml", szFileDirectory, szFileRoot).GetCString());
			gDLL->MessageBox(szMessage, "XML Load Error");
		}
		else if (Validate())
		{
			SetDiplomacyInfo(DiploInfos, szXmlPath);

			if (gDLL->isModularXMLLoading())
			{
				std::vector<CvString> aszFiles;
				gDLL->enumerateFiles(aszFiles, CvString::format("modules\\*_%s.xml", szFileRoot));  // search for the modular files

				for (std::vector<CvString>::iterator it = aszFiles.begin(); it != aszFiles.end(); ++it)
				{
					bLoaded = LoadCivXml(m_pFXml, *it);

					if (!bLoaded)
					{
						char szMessage[1024];
						sprintf(szMessage, "LoadXML call failed for %s.", (*it).GetCString());
						gDLL->MessageBox(szMessage, "XML Load Error");
					}
					else if (Validate())
					{
						SetDiplomacyInfo(DiploInfos, szXmlPath);
					}
				}
			}

			if (NULL != pArgFunction && bWriteCache)
			{
				// write info to cache
				bool bOk = gDLL->cacheWrite(pCache);
				if (!bOk)
				{
					char szMessage[1024];
					sprintf(szMessage, "Failed writing to %s cache. \n Current XML file is: %s", szFileDirectory, GC.getCurrentXMLFile().GetCString());
					gDLL->MessageBox(szMessage, "XML Caching Error");
				}
				if (bOk)
				{
					logMsg("Wrote %s to cache", szFileDirectory);
				}
			}
		}
	}

	if (NULL != pArgFunction)
	{
		gDLL->destroyCache(pCache);
	}
}

//
// helper sort predicate
//

struct OrderIndex {int m_iPriority; int m_iIndex;};
bool sortHotkeyPriority(const OrderIndex orderIndex1, const OrderIndex orderIndex2)
{
	return (orderIndex1.m_iPriority > orderIndex2.m_iPriority);
}

template <class T>
void CvXMLLoadUtility::orderHotkeyInfo(int** ppiSortedIndex, T* pHotkeyInfos, int iLength)
{
	int iI;
	int* piSortedIndex;
	std::vector<OrderIndex> viOrderPriority;

	viOrderPriority.resize(iLength);
	piSortedIndex = *ppiSortedIndex;

	// set up vector
	for(iI=0;iI<iLength;iI++)
	{
		viOrderPriority[iI].m_iPriority = pHotkeyInfos[iI].getOrderPriority();
		viOrderPriority[iI].m_iIndex = iI;
	}

	// sort the array
	std::sort(viOrderPriority.begin(), viOrderPriority.end(), sortHotkeyPriority);

	// insert new order into the array to return
	for (iI=0;iI<iLength;iI++)
	{
		piSortedIndex[iI] = viOrderPriority[iI].m_iIndex;
	}
}

void CvXMLLoadUtility::orderHotkeyInfo(int** ppiSortedIndex, int* pHotkeyIndex, int iLength)
{
	int iI;
	int* piSortedIndex;
	std::vector<OrderIndex> viOrderPriority;

	viOrderPriority.resize(iLength);
	piSortedIndex = *ppiSortedIndex;

	// set up vector
	for(iI=0;iI<iLength;iI++)
	{
		viOrderPriority[iI].m_iPriority = pHotkeyIndex[iI];
		viOrderPriority[iI].m_iIndex = iI;
	}

	// sort the array
	std::sort(viOrderPriority.begin(), viOrderPriority.end(), sortHotkeyPriority);

	// insert new order into the array to return
	for (iI=0;iI<iLength;iI++)
	{
		piSortedIndex[iI] = viOrderPriority[iI].m_iIndex;
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetFeatureStruct(int** ppiFeatureTime, int*** ppiFeatureYield, bool** ppbFeatureRemove)
//
//  PURPOSE :   allocate and set the feature struct variables for the CvBuildInfo class
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetFeatureStruct(int** ppiFeatureTime, std::vector<std::vector<int> >& aaiFeatureYield, bool** ppbFeatureRemove)
{
	int iNumSibs;					// the number of siblings the current xml node has
	int iFeatureIndex;
	TCHAR szTextVal[256];	// temporarily hold the text value of the current xml node
	int* paiFeatureTime = NULL;
	bool* pabFeatureRemove = NULL;

	if(GC.getNumFeatureInfos() < 1)
	{
		char	szMessage[1024];
		sprintf( szMessage, "no feature infos set yet! \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
	InitList(ppiFeatureTime, GC.getNumFeatureInfos(), 0);
	aaiFeatureYield.resize(GC.getNumFeatureInfos());
	for (int i = 0; i < GC.getNumFeatureInfos(); ++i)
	{
		aaiFeatureYield[i].resize(NUM_YIELD_TYPES, 0);
	}
	InitList(ppbFeatureRemove, GC.getNumFeatureInfos(), false);
	paiFeatureTime = *ppiFeatureTime;
	pabFeatureRemove = *ppbFeatureRemove;

	if (gDLL->getXMLIFace()->SetToChildByTagName(m_pFXml,"FeatureStructs"))
	{
		iNumSibs = gDLL->getXMLIFace()->GetNumChildren(m_pFXml);

		if (0 < iNumSibs)
		{
			if (gDLL->getXMLIFace()->SetToChildByTagName(m_pFXml,"FeatureStruct"))
			{
				if(!(iNumSibs <= GC.getNumFeatureInfos()))
				{
					char	szMessage[1024];
					sprintf( szMessage, "iNumSibs is greater than GC.getNumFeatureInfos in SetFeatureStruct function \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
					gDLL->MessageBox(szMessage, "XML Error");
				}
				for (i=0;i<iNumSibs;i++)
				{
					GetChildXmlValByName(szTextVal, "FeatureType");
					iFeatureIndex = FindInInfoClass(szTextVal);
					if(!(iFeatureIndex != -1))
					{
						char	szMessage[1024];
						sprintf( szMessage, "iFeatureIndex is -1 inside SetFeatureStruct function \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
						gDLL->MessageBox(szMessage, "XML Error");
					}
					GetChildXmlValByName(&paiFeatureTime[iFeatureIndex], "iTime");
					int* aiFeatureYield;
					SetVariableListTagPair(&aiFeatureYield, "Yields", NUM_YIELD_TYPES, 0);
					for (int j = 0; j < NUM_YIELD_TYPES; ++j)
					{
						aaiFeatureYield[iFeatureIndex][j] = aiFeatureYield[j];
					}
					SAFE_DELETE_ARRAY(aiFeatureYield);
					GetChildXmlValByName(&pabFeatureRemove[iFeatureIndex], "bRemove");

					if (!gDLL->getXMLIFace()->NextSibling(m_pFXml))
					{
						break;
					}
				}

				gDLL->getXMLIFace()->SetToParent(m_pFXml);
			}
		}

		gDLL->getXMLIFace()->SetToParent(m_pFXml);
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetImprovementBonuses(CvImprovementBonusInfo** ppImprovementBonus)
//
//  PURPOSE :   Allocate memory for the improvement bonus pointer and fill it based on the
//				values in the xml.
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetImprovementBonuses(CvImprovementBonusInfo** ppImprovementBonus)
{
	int i=0;				//loop counter
	int iNumSibs;			// the number of siblings the current xml node has
	TCHAR szNodeVal[256];	// temporarily holds the string value of the current xml node
	CvImprovementBonusInfo* paImprovementBonus;	// local pointer to the bonus type struct in memory

	// Skip any comments and stop at the next value we might want
	if (SkipToNextVal())
	{
		// initialize the boolean list to the correct size and all the booleans to false
		InitImprovementBonusList(ppImprovementBonus, GC.getNumBonusInfos());
		// set the local pointer to the memory we just allocated
		paImprovementBonus = *ppImprovementBonus;

		// get the total number of children the current xml node has
		iNumSibs = gDLL->getXMLIFace()->GetNumChildren(m_pFXml);
		// if we can set the current xml node to the child of the one it is at now
		if (gDLL->getXMLIFace()->SetToChild(m_pFXml))
		{
			if(!(iNumSibs <= GC.getNumBonusInfos()))
			{
				char	szMessage[1024];
				sprintf( szMessage, "For loop iterator is greater than array size \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
				gDLL->MessageBox(szMessage, "XML Error");
			}
			// loop through all the siblings
			for (i=0;i<iNumSibs;i++)
			{
				// skip to the next non-comment node
				if (SkipToNextVal())
				{
					// call the function that sets the FXml pointer to the first non-comment child of
					// the current tag and gets the value of that new node
					if (GetChildXmlVal(szNodeVal))
					{
						int iBonusIndex;	// index of the match in the bonus types list
						// call the find in list function to return either -1 if no value is found
						// or the index in the list the match is found at
						iBonusIndex = FindInInfoClass(szNodeVal);
						// if we found a match we will get the next sibling's boolean value at that match's index
						if (iBonusIndex >= 0)
						{
							GetNextXmlVal(&paImprovementBonus[iBonusIndex].m_bBonusMakesValid);
							GetNextXmlVal(&paImprovementBonus[iBonusIndex].m_iDiscoverRand);
							gDLL->getXMLIFace()->SetToParent(m_pFXml);

							SAFE_DELETE_ARRAY(paImprovementBonus[iBonusIndex].m_aiYieldChange);	// free memory - MT, since we are about to reallocate

							SetVariableListTagPair(&paImprovementBonus[iBonusIndex].m_aiYieldChange, "YieldChanges", NUM_YIELD_TYPES, 0);
						}
						else
						{
							gDLL->getXMLIFace()->SetToParent(m_pFXml);
						}

						// set the current xml node to it's parent node
					}

					// if we cannot set the current xml node to it's next sibling then we will break out of the for loop
					// otherwise we will continue looping
					if (!gDLL->getXMLIFace()->NextSibling(m_pFXml))
					{
						break;
					}
				}
			}
			// set the current xml node to it's parent node
			gDLL->getXMLIFace()->SetToParent(m_pFXml);
		}
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetAndLoadVar(int** ppiVar, int iDefault)
//
//  PURPOSE :   set the variable to a default and load it from the xml if there are any children
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::SetAndLoadVar(int** ppiVar, int iDefault)
{
	int iNumSibs;
	int* piVar;
	bool bReturn = false;
	int i; // loop counter

	// Skip any comments and stop at the next value we might want
	if (SkipToNextVal())
	{
		bReturn = true;

		// get the total number of children the current xml node has
		iNumSibs = gDLL->getXMLIFace()->GetNumChildren(m_pFXml);

		// allocate memory
		InitList(ppiVar, iNumSibs, iDefault);

		// set the a local pointer to the newly allocated memory
		piVar = *ppiVar;

		// if the call to the function that sets the current xml node to it's first non-comment
		// child and sets the parameter with the new node's value succeeds
		if (GetChildXmlVal(&piVar[0]))
		{
			// loop through all the siblings, we start at 1 since we already got the first sibling
			for (i=1;i<iNumSibs;i++)
			{
				// if the call to the function that sets the current xml node to it's next non-comment
				// sibling and sets the parameter with the new node's value does not succeed
				// we will break out of this for loop
				if (!GetNextXmlVal(&piVar[i]))
				{
					break;
				}
			}

			// set the current xml node to it's parent node
			gDLL->getXMLIFace()->SetToParent(m_pFXml);
		}
	}

	return bReturn;
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetVariableListTagPairForAudioScripts(int **ppiList, const TCHAR* szRootTagName,
//										int iInfoBaseLength, int iDefaultListVal)
//
//  PURPOSE :   allocate and initialize a list from a tag pair in the xml for audio scripts
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetVariableListTagPairForAudioScripts(int **ppiList, const TCHAR* szRootTagName, int iInfoBaseLength, int iDefaultListVal)
{
	int i;
	int iIndexVal;
	int iNumSibs;
	TCHAR szTextVal[256];
	int* piList;
	CvString szTemp;

	if (gDLL->getXMLIFace()->SetToChildByTagName(m_pFXml,szRootTagName))
	{
		if (SkipToNextVal())
		{
			iNumSibs = gDLL->getXMLIFace()->GetNumChildren(m_pFXml);
			if(!(0 < iInfoBaseLength))
			{
				char	szMessage[1024];
				sprintf( szMessage, "Allocating zero or less memory in CvXMLLoadUtility::SetVariableListTagPair \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
				gDLL->MessageBox(szMessage, "XML Error");
			}
			InitList(ppiList, iInfoBaseLength, iDefaultListVal);
			piList = *ppiList;
			if (0 < iNumSibs)
			{
				if(!(iNumSibs <= iInfoBaseLength))
				{
					char	szMessage[1024];
					sprintf( szMessage, "There are more siblings than memory allocated for them in CvXMLLoadUtility::SetVariableListTagPair \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
					gDLL->MessageBox(szMessage, "XML Error");
				}
				if (gDLL->getXMLIFace()->SetToChild(m_pFXml))
				{
					for (i=0;i<iNumSibs;i++)
					{
						if (GetChildXmlVal(szTextVal))
						{
							iIndexVal = FindInInfoClass(szTextVal);
							GetNextXmlVal(&szTemp);
							if ( szTemp.GetLength() > 0 )
								piList[iIndexVal] = gDLL->getAudioTagIndex(szTemp);
							else
								piList[iIndexVal] = -1;

							gDLL->getXMLIFace()->SetToParent(m_pFXml);
						}

						if (!gDLL->getXMLIFace()->NextSibling(m_pFXml))
						{
							break;
						}
					}

					gDLL->getXMLIFace()->SetToParent(m_pFXml);
				}
			}
		}

		gDLL->getXMLIFace()->SetToParent(m_pFXml);
	}
}

DllExport bool CvXMLLoadUtility::LoadPlayerOptions()
{
	// load order: 1

	if (!CreateFXml())
		return false;
	
/// XML load - start - Nightinggale
	bFirstLoadRound = false;
	bLoadOnce = true;

	// hardcode NONE to -1 when reading XML files
	GC.setInfoTypeFromString("NONE", -1);

	// load options and graphic files
	loadXMLFile(XML_FILE_CIV4PlayerOptionInfos);
	loadXMLFile(XML_FILE_CIV4GraphicOptionInfos);
	
	loadXMLFile(XML_FILE_CIV4ArtDefines_Interface);
	loadXMLFile(XML_FILE_CIV4ArtDefines_Movie);
	loadXMLFile(XML_FILE_CIV4ArtDefines_Misc);
	loadXMLFile(XML_FILE_CIV4ArtDefines_Unit);
	loadXMLFile(XML_FILE_CIV4ArtDefines_Building);
	loadXMLFile(XML_FILE_CIV4ArtDefines_Civilization);
	loadXMLFile(XML_FILE_CIV4ArtDefines_Leaderhead);
	loadXMLFile(XML_FILE_CIV4ArtDefines_Bonus);
	loadXMLFile(XML_FILE_CIV4ArtDefines_Improvement);
	loadXMLFile(XML_FILE_CIV4ArtDefines_Terrain);
	loadXMLFile(XML_FILE_CIV4ArtDefines_Feature);

	// load BasicInfo for XML files
	bFirstLoadRound = true;
	loadXMLFiles();
/// XML load - end - Nightinggale
#if 0

	LoadGlobalClassInfo(GC.getPlayerOptionInfo(), "CIV4PlayerOptionInfos", "GameInfo", "Civ4PlayerOptionInfos/PlayerOptionInfos/PlayerOptionInfo", NULL);
	FAssert(GC.getNumPlayerOptionInfos() == NUM_PLAYEROPTION_TYPES);
	GC.CheckEnumPlayerOptionTypes(); // XML enum check - Nightinggale
#endif

	DestroyFXml();
	return true;
}

DllExport bool CvXMLLoadUtility::LoadGraphicOptions()
{
	// load order: 2

#if 0
	if (!CreateFXml())
		return false;

	LoadGlobalClassInfo(GC.getGraphicOptionInfo(), "CIV4GraphicOptionInfos", "GameInfo", "Civ4GraphicOptionInfos/GraphicOptionInfos/GraphicOptionInfo", NULL);
	FAssert(GC.getNumGraphicOptions() == NUM_GRAPHICOPTION_TYPES);
	GC.CheckEnumGraphicOptionTypes(); // XML enum check - Nightinggale

	DestroyFXml();
#endif
	return true;
}


