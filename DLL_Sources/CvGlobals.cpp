//
// globals.cpp
// Author -	Mustafa Thamer
//
#include "CvGameCoreDLL.h"
#include "CvGlobals.h"
#include "CvRandom.h"
#include "CvGameAI.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvMap.h"
#include "CvPlayerAI.h"
#include "CvTeamAI.h"
#include "CvInfos.h"
#include "CvDLLUtilityIFaceBase.h"
#include "CvArtFileMgr.h"
#include "CvDLLXMLIFaceBase.h"
#include "CvPlayerAI.h"
#include "CvInfoWater.h"
#include "CvGameTextMgr.h"
#include "FProfiler.h"
#include "FVariableSystem.h"
#include "CvInitCore.h"
#include "CvXMLLoadUtility.h"

#define COPY(dst, src, typeName) \
	{ \
		int iNum = sizeof(src)/sizeof(typeName); \
		dst = new typeName[iNum]; \
		for (int i =0;i<iNum;i++) \
			dst[i] = src[i]; \
	}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

CvGlobals gGlobals;

//
// CONSTRUCTOR
//
CvGlobals::CvGlobals() :
m_bGraphicsInitialized(false),
m_bLogging(false),
m_bRandLogging(false),
m_bOverwriteLogs(false),
m_bSynchLogging(false),
m_pkMainMenu(NULL),
m_iNewPlayers(0),
m_bZoomOut(false),
m_bZoomIn(false),
m_bLoadGameFromFile(false),
m_pFMPMgr(NULL),
m_asyncRand(NULL),
m_interface(NULL),
m_game(NULL),
m_messageQueue(NULL),
m_hotJoinMsgQueue(NULL),
m_messageControl(NULL),
m_messageCodes(NULL),
m_dropMgr(NULL),
m_portal(NULL),
m_setupData(NULL),
m_initCore(NULL),
m_statsReporter(NULL),
m_map(NULL),
m_diplomacyScreen(NULL),
m_mpDiplomacyScreen(NULL),
m_pathFinder(NULL),
m_interfacePathFinder(NULL),
m_stepFinder(NULL),
m_routeFinder(NULL),
m_borderFinder(NULL),
m_areaFinder(NULL),
m_pDLL(NULL),
m_aiPlotDirectionX(NULL),
m_aiPlotDirectionY(NULL),
m_aiPlotCardinalDirectionX(NULL),
m_aiPlotCardinalDirectionY(NULL),
m_aiCityPlotX(NULL),
m_aiCityPlotY(NULL),
m_aiCityPlotPriority(NULL),
m_aeTurnLeftDirection(NULL),
m_aeTurnRightDirection(NULL),
m_VarSystem(NULL),
m_iMOVE_DENOMINATOR(0),
m_iFOOD_CONSUMPTION_PER_POPULATION(0),
m_iMAX_HIT_POINTS(0),
m_iHILLS_EXTRA_DEFENSE(0),
m_iRIVER_ATTACK_MODIFIER(0),
m_iAMPHIB_ATTACK_MODIFIER(0),
m_iHILLS_EXTRA_MOVEMENT(0),
m_iPEAK_EXTRA_MOVEMENT(0),
m_iMAX_PLOT_LIST_ROWS(0),
m_iUNIT_MULTISELECT_MAX(0),
m_iEVENT_MESSAGE_TIME(0),
m_iROUTE_FEATURE_GROWTH_MODIFIER(0),
m_iFEATURE_GROWTH_MODIFIER(0),
m_iMIN_CITY_RANGE(0),
m_iCITY_MAX_NUM_BUILDINGS(0),
m_iLAKE_MAX_AREA_SIZE(0),
m_iMIN_WATER_SIZE_FOR_OCEAN(0),
m_iFORTIFY_MODIFIER_PER_TURN(0),
m_iMAX_CITY_DEFENSE_DAMAGE(0),
m_iPEAK_SEE_THROUGH_CHANGE(0),
m_iHILLS_SEE_THROUGH_CHANGE(0),
m_iSEAWATER_SEE_FROM_CHANGE(0),
m_iPEAK_SEE_FROM_CHANGE(0),
m_iHILLS_SEE_FROM_CHANGE(0),
m_iMAX_REBEL_YIELD_MODIFIER(0),
m_fCAMERA_MIN_YAW(0),
m_fCAMERA_MAX_YAW(0),
m_fCAMERA_FAR_CLIP_Z_HEIGHT(0),
m_fCAMERA_MAX_TRAVEL_DISTANCE(0),
m_fCAMERA_START_DISTANCE(0),
m_fPLOT_SIZE(0),
m_fCAMERA_SPECIAL_PITCH(0),
m_fCAMERA_MAX_TURN_OFFSET(0),
m_fCAMERA_MIN_DISTANCE(0),
m_fCAMERA_UPPER_PITCH(0),
m_fCAMERA_LOWER_PITCH(0),
m_fFIELD_OF_VIEW(0),
m_fUNIT_MULTISELECT_DISTANCE(0),
m_iUSE_CAN_FOUND_CITIES_ON_WATER_CALLBACK(0),
m_iUSE_CANNOT_DO_CIVIC_CALLBACK(0),
m_iUSE_CAN_DO_CIVIC_CALLBACK(0),
m_iUSE_CANNOT_CONSTRUCT_CALLBACK(0),
m_iUSE_CAN_CONSTRUCT_CALLBACK(0),
m_iUSE_CAN_DECLARE_WAR_CALLBACK(0),
m_iUSE_GET_UNIT_COST_MOD_CALLBACK(0),
m_iUSE_GET_CITY_FOUND_VALUE_CALLBACK(0),
m_iUSE_CANNOT_HANDLE_ACTION_CALLBACK(0),
m_iUSE_CAN_BUILD_CALLBACK(0),
m_iUSE_CANNOT_TRAIN_CALLBACK(0),
m_iUSE_CAN_TRAIN_CALLBACK(0),
m_iUSE_UNIT_CANNOT_MOVE_INTO_CALLBACK(0),
m_iUSE_FINISH_TEXT_CALLBACK(0),
m_iUSE_ON_UNIT_SET_XY_CALLBACK(0),
m_iUSE_ON_UNIT_SELECTED_CALLBACK(0),
m_iUSE_ON_MISSIONARY_CONVERTED_UNIT_CALLBACK(0),
m_iUSE_ON_UPDATE_CALLBACK(0),
m_iUSE_ON_UNIT_CREATED_CALLBACK(0),
m_iUSE_ON_UNIT_LOST_CALLBACK(0),

m_paHints(NULL),
m_paMainMenus(NULL)
{
}

CvGlobals::~CvGlobals()
{
}

//
// allocate
//
void CvGlobals::init()
{
	//
	// These vars are used to initialize the globals.
	//

	int aiPlotDirectionX[NUM_DIRECTION_TYPES] =
	{
		0,	// DIRECTION_NORTH
		1,	// DIRECTION_NORTHEAST
		1,	// DIRECTION_EAST
		1,	// DIRECTION_SOUTHEAST
		0,	// DIRECTION_SOUTH
		-1,	// DIRECTION_SOUTHWEST
		-1,	// DIRECTION_WEST
		-1,	// DIRECTION_NORTHWEST
	};

	int aiPlotDirectionY[NUM_DIRECTION_TYPES] =
	{
		1,	// DIRECTION_NORTH
		1,	// DIRECTION_NORTHEAST
		0,	// DIRECTION_EAST
		-1,	// DIRECTION_SOUTHEAST
		-1,	// DIRECTION_SOUTH
		-1,	// DIRECTION_SOUTHWEST
		0,	// DIRECTION_WEST
		1,	// DIRECTION_NORTHWEST
	};

	int aiPlotCardinalDirectionX[NUM_CARDINALDIRECTION_TYPES] =
	{
		0,	// CARDINALDIRECTION_NORTH
		1,	// CARDINALDIRECTION_EAST
		0,	// CARDINALDIRECTION_SOUTH
		-1,	// CARDINALDIRECTION_WEST
	};

	int aiPlotCardinalDirectionY[NUM_CARDINALDIRECTION_TYPES] =
	{
		1,	// CARDINALDIRECTION_NORTH
		0,	// CARDINALDIRECTION_EAST
		-1,	// CARDINALDIRECTION_SOUTH
		0,	// CARDINALDIRECTION_WEST
	};

	int aiCityPlotX[NUM_CITY_PLOTS] =
	{
		0,
		0, 1, 1, 1, 0,-1,-1,-1,
//		0, 1, 2, 2, 2, 1, 0,-1,-2,-2,-2,-1,
	};

	int aiCityPlotY[NUM_CITY_PLOTS] =
	{
		0,
		1, 1, 0,-1,-1,-1, 0, 1,
//		2, 2, 1, 0,-1,-2,-2,-2,-1, 0, 1, 2,
	};

	int aiCityPlotPriority[NUM_CITY_PLOTS] =
	{
		0,
		1, 2, 1, 2, 1, 2, 1, 2,
//		3, 4, 4, 3, 4, 4, 3, 4, 4, 3, 4, 4,
	};

	int aaiXYCityPlot[CITY_PLOTS_DIAMETER][CITY_PLOTS_DIAMETER] =
	{
		{6, 7, 8,},

		{5, 0, 1,},

		{4, 3, 2,}
	};

	DirectionTypes aeTurnRightDirection[NUM_DIRECTION_TYPES] =
	{
		DIRECTION_NORTHEAST,	// DIRECTION_NORTH
		DIRECTION_EAST,				// DIRECTION_NORTHEAST
		DIRECTION_SOUTHEAST,	// DIRECTION_EAST
		DIRECTION_SOUTH,			// DIRECTION_SOUTHEAST
		DIRECTION_SOUTHWEST,	// DIRECTION_SOUTH
		DIRECTION_WEST,				// DIRECTION_SOUTHWEST
		DIRECTION_NORTHWEST,	// DIRECTION_WEST
		DIRECTION_NORTH,			// DIRECTION_NORTHWEST
	};

	DirectionTypes aeTurnLeftDirection[NUM_DIRECTION_TYPES] =
	{
		DIRECTION_NORTHWEST,	// DIRECTION_NORTH
		DIRECTION_NORTH,			// DIRECTION_NORTHEAST
		DIRECTION_NORTHEAST,	// DIRECTION_EAST
		DIRECTION_EAST,				// DIRECTION_SOUTHEAST
		DIRECTION_SOUTHEAST,	// DIRECTION_SOUTH
		DIRECTION_SOUTH,			// DIRECTION_SOUTHWEST
		DIRECTION_SOUTHWEST,	// DIRECTION_WEST
		DIRECTION_WEST,				// DIRECTION_NORTHWEST
	};

	DirectionTypes aaeXYDirection[DIRECTION_DIAMETER][DIRECTION_DIAMETER] =
	{
		DIRECTION_SOUTHWEST, DIRECTION_WEST,	DIRECTION_NORTHWEST,
		DIRECTION_SOUTH,     NO_DIRECTION,    DIRECTION_NORTH,
		DIRECTION_SOUTHEAST, DIRECTION_EAST,	DIRECTION_NORTHEAST,
	};

	FAssertMsg(gDLL != NULL, "Civ app needs to set gDLL");

	m_VarSystem = new FVariableSystem;
	m_asyncRand = new CvRandom;
	m_initCore = new CvInitCore;
	m_loadedInitCore = new CvInitCore;
	m_iniInitCore = new CvInitCore;

	gDLL->initGlobals();	// some globals need to be allocated outside the dll

	m_game = new CvGameAI;
	m_map = new CvMap;

	CvPlayerAI::initStatics();
	CvTeamAI::initStatics();

	m_pt3Origin = NiPoint3(0.0f, 0.0f, 0.0f);

	COPY(m_aiPlotDirectionX, aiPlotDirectionX, int);
	COPY(m_aiPlotDirectionY, aiPlotDirectionY, int);
	COPY(m_aiPlotCardinalDirectionX, aiPlotCardinalDirectionX, int);
	COPY(m_aiPlotCardinalDirectionY, aiPlotCardinalDirectionY, int);
	COPY(m_aiCityPlotX, aiCityPlotX, int);
	COPY(m_aiCityPlotY, aiCityPlotY, int);
	COPY(m_aiCityPlotPriority, aiCityPlotPriority, int);
	COPY(m_aeTurnLeftDirection, aeTurnLeftDirection, DirectionTypes);
	COPY(m_aeTurnRightDirection, aeTurnRightDirection, DirectionTypes);
	memcpy(m_aaiXYCityPlot, aaiXYCityPlot, sizeof(m_aaiXYCityPlot));
	memcpy(m_aaeXYDirection, aaeXYDirection,sizeof(m_aaeXYDirection));
}

//
// free
//
void CvGlobals::uninit()
{
	//
	// See also CvXMLLoadUtilityInit.cpp::CleanUpGlobalVariables()
	//
	SAFE_DELETE_ARRAY(m_aiPlotDirectionX);
	SAFE_DELETE_ARRAY(m_aiPlotDirectionY);
	SAFE_DELETE_ARRAY(m_aiPlotCardinalDirectionX);
	SAFE_DELETE_ARRAY(m_aiPlotCardinalDirectionY);
	SAFE_DELETE_ARRAY(m_aiCityPlotX);
	SAFE_DELETE_ARRAY(m_aiCityPlotY);
	SAFE_DELETE_ARRAY(m_aiCityPlotPriority);
	SAFE_DELETE_ARRAY(m_aeTurnLeftDirection);
	SAFE_DELETE_ARRAY(m_aeTurnRightDirection);

	CvPlayerAI::freeStatics();
	CvTeamAI::freeStatics();

	SAFE_DELETE(m_game);
	SAFE_DELETE(m_map);

	SAFE_DELETE(m_asyncRand);
	SAFE_DELETE(m_initCore);
	SAFE_DELETE(m_loadedInitCore);
	SAFE_DELETE(m_iniInitCore);
	gDLL->uninitGlobals();	// free globals allocated outside the dll
	SAFE_DELETE(m_VarSystem);

	// already deleted outside of the dll, set to null for safety
	m_messageQueue=NULL;
	m_hotJoinMsgQueue=NULL;
	m_messageControl=NULL;
	m_setupData=NULL;
	m_messageCodes=NULL;
	m_dropMgr=NULL;
	m_portal=NULL;
	m_statsReporter=NULL;
	m_interface=NULL;
	m_diplomacyScreen=NULL;
	m_mpDiplomacyScreen=NULL;
	m_pathFinder=NULL;
	m_interfacePathFinder=NULL;
	m_stepFinder=NULL;
	m_routeFinder=NULL;
	m_borderFinder=NULL;
	m_areaFinder=NULL;

	deleteInfoArrays();
}

void CvGlobals::clearTypesMap()
{
	infoTypeFromStringReset();
	if (m_VarSystem)
	{
		m_VarSystem->UnInit();
	}
}


CvDiplomacyScreen* CvGlobals::getDiplomacyScreen()
{
	return m_diplomacyScreen;
}

CMPDiplomacyScreen* CvGlobals::getMPDiplomacyScreen()
{
	return m_mpDiplomacyScreen;
}

CvMessageCodeTranslator& CvGlobals::getMessageCodes()
{
	return *m_messageCodes;
}

FMPIManager*& CvGlobals::getFMPMgrPtr()
{
	return m_pFMPMgr;
}

CvPortal& CvGlobals::getPortal()
{
	return *m_portal;
}

CvSetupData& CvGlobals::getSetupData()
{
	return *m_setupData;
}

CvInitCore& CvGlobals::getInitCore()
{
	return *m_initCore;
}

CvInitCore& CvGlobals::getLoadedInitCore()
{
	return *m_loadedInitCore;
}

CvInitCore& CvGlobals::getIniInitCore()
{
	return *m_iniInitCore;
}

CvStatsReporter& CvGlobals::getStatsReporter()
{
	return *m_statsReporter;
}

CvStatsReporter* CvGlobals::getStatsReporterPtr()
{
	return m_statsReporter;
}

CvInterface& CvGlobals::getInterface()
{
	return *m_interface;
}

CvInterface* CvGlobals::getInterfacePtr()
{
	return m_interface;
}

CvRandom& CvGlobals::getASyncRand()
{
	return *m_asyncRand;
}

CMessageQueue& CvGlobals::getMessageQueue()
{
	return *m_messageQueue;
}

CMessageQueue& CvGlobals::getHotMessageQueue()
{
	return *m_hotJoinMsgQueue;
}

CMessageControl& CvGlobals::getMessageControl()
{
	return *m_messageControl;
}

CvDropMgr& CvGlobals::getDropMgr()
{
	return *m_dropMgr;
}

FAStar& CvGlobals::getPathFinder()
{
	return *m_pathFinder;
}

FAStar& CvGlobals::getInterfacePathFinder()
{
	return *m_interfacePathFinder;
}

FAStar& CvGlobals::getStepFinder()
{
	return *m_stepFinder;
}

FAStar& CvGlobals::getRouteFinder()
{
	return *m_routeFinder;
}

FAStar& CvGlobals::getBorderFinder()
{
	return *m_borderFinder;
}

FAStar& CvGlobals::getAreaFinder()
{
	return *m_areaFinder;
}

NiPoint3& CvGlobals::getPt3Origin()
{
	return m_pt3Origin;
}

std::vector<CvInterfaceModeInfo*>& CvGlobals::getInterfaceModeInfo()		// For Moose - XML Load Util and CvInfos
{
	return m_paInterfaceModeInfo;
}

CvInterfaceModeInfo& CvGlobals::getInterfaceModeInfo(InterfaceModeTypes e)
{
	FAssert(e > -1);
	FAssert(e < NUM_INTERFACEMODE_TYPES);
	return *(m_paInterfaceModeInfo[e]);
}

NiPoint3& CvGlobals::getPt3CameraDir()
{
	return m_pt3CameraDir;
}

bool& CvGlobals::getLogging()
{
	return m_bLogging;
}

bool& CvGlobals::getRandLogging()
{
	return m_bRandLogging;
}

bool& CvGlobals::getSynchLogging()
{
	return m_bSynchLogging;
}

bool& CvGlobals::overwriteLogs()
{
	return m_bOverwriteLogs;
}

int* CvGlobals::getPlotDirectionX()
{
	return m_aiPlotDirectionX;
}

int* CvGlobals::getPlotDirectionY()
{
	return m_aiPlotDirectionY;
}

int* CvGlobals::getPlotCardinalDirectionX()
{
	return m_aiPlotCardinalDirectionX;
}

int* CvGlobals::getPlotCardinalDirectionY()
{
	return m_aiPlotCardinalDirectionY;
}

int* CvGlobals::getCityPlotX()
{
	return m_aiCityPlotX;
}

int* CvGlobals::getCityPlotY()
{
	return m_aiCityPlotY;
}

int* CvGlobals::getCityPlotPriority()
{
	return m_aiCityPlotPriority;
}

int CvGlobals::getXYCityPlot(int i, int j)
{
	FAssertMsg(i < CITY_PLOTS_DIAMETER, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < CITY_PLOTS_DIAMETER, "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return m_aaiXYCityPlot[i][j];
}

DirectionTypes* CvGlobals::getTurnLeftDirection()
{
	return m_aeTurnLeftDirection;
}

DirectionTypes CvGlobals::getTurnLeftDirection(int i)
{
	FAssertMsg(i < NUM_DIRECTION_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aeTurnLeftDirection[i];
}

DirectionTypes* CvGlobals::getTurnRightDirection()
{
	return m_aeTurnRightDirection;
}

DirectionTypes CvGlobals::getTurnRightDirection(int i)
{
	FAssertMsg(i < NUM_DIRECTION_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aeTurnRightDirection[i];
}

DirectionTypes CvGlobals::getXYDirection(int i, int j)
{
	FAssertMsg(i < DIRECTION_DIAMETER, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < DIRECTION_DIAMETER, "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return m_aaeXYDirection[i][j];
}

int CvGlobals::getNumWorldInfos()
{
	return (int)m_paWorldInfo.size();
}

std::vector<CvWorldInfo*>& CvGlobals::getWorldInfo()
{
	return m_paWorldInfo;
}

CvWorldInfo& CvGlobals::getWorldInfo(WorldSizeTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumWorldInfos());
	return *(m_paWorldInfo[e]);
}

/////////////////////////////////////////////
// CLIMATE
/////////////////////////////////////////////

int CvGlobals::getNumClimateInfos()
{
	return (int)m_paClimateInfo.size();
}

std::vector<CvClimateInfo*>& CvGlobals::getClimateInfo()
{
	return m_paClimateInfo;
}

CvClimateInfo& CvGlobals::getClimateInfo(ClimateTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumClimateInfos());
	return *(m_paClimateInfo[e]);
}

/////////////////////////////////////////////
// SEALEVEL
/////////////////////////////////////////////

int CvGlobals::getNumSeaLevelInfos()
{
	return (int)m_paSeaLevelInfo.size();
}

std::vector<CvSeaLevelInfo*>& CvGlobals::getSeaLevelInfo()
{
	return m_paSeaLevelInfo;
}

CvSeaLevelInfo& CvGlobals::getSeaLevelInfo(SeaLevelTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumSeaLevelInfos());
	return *(m_paSeaLevelInfo[e]);
}

/////////////////////////////////////////////
// EUROPE
/////////////////////////////////////////////

int CvGlobals::getNumEuropeInfos()
{
	return (int)m_paEuropeInfo.size();
}

std::vector<CvEuropeInfo*>& CvGlobals::getEuropeInfo()
{
	return m_paEuropeInfo;
}

CvEuropeInfo& CvGlobals::getEuropeInfo(EuropeTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumEuropeInfos());
	return *(m_paEuropeInfo[e]);
}

int CvGlobals::getNumHints()
{
	return (int)m_paHints.size();
}

std::vector<CvInfoBase*>& CvGlobals::getHints()
{
	return m_paHints;
}

CvInfoBase& CvGlobals::getHints(int i)
{
	return *(m_paHints[i]);
}

/// language selection - start - Nightinggale
int CvGlobals::getNumLanguageInfos()
{
	return (int)m_paLanguages.size();
}

std::vector<LanguageInfo*>& CvGlobals::getLanguageInfos()
{
	return m_paLanguages;
}

LanguageInfo& CvGlobals::getLanguageInfo(int i)
{
	return *(m_paLanguages[i]);
}
/// language selection - end - Nightinggale

int CvGlobals::getNumMainMenus()
{
	return (int)m_paMainMenus.size();
}

std::vector<CvMainMenuInfo*>& CvGlobals::getMainMenus()
{
	return m_paMainMenus;
}

CvMainMenuInfo& CvGlobals::getMainMenus(int i)
{
	if (i >= getNumMainMenus())
	{
		return *(m_paMainMenus[0]);
	}

	return *(m_paMainMenus[i]);
}

int CvGlobals::getNumColorInfos()
{
	return (int)m_paColorInfo.size();
}

std::vector<CvColorInfo*>& CvGlobals::getColorInfo()
{
	return m_paColorInfo;
}

CvColorInfo& CvGlobals::getColorInfo(ColorTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumColorInfos());
	return *(m_paColorInfo[e]);
}


int CvGlobals::getNumPlayerColorInfos()
{
	return (int)m_paPlayerColorInfo.size();
}

std::vector<CvPlayerColorInfo*>& CvGlobals::getPlayerColorInfo()
{
	return m_paPlayerColorInfo;
}

CvPlayerColorInfo& CvGlobals::getPlayerColorInfo(PlayerColorTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumPlayerColorInfos());
	return *(m_paPlayerColorInfo[e]);
}

int CvGlobals::getNumRouteModelInfos()
{
	return (int)m_paRouteModelInfo.size();
}

std::vector<CvRouteModelInfo*>& CvGlobals::getRouteModelInfo()
{
	return m_paRouteModelInfo;
}

CvRouteModelInfo& CvGlobals::getRouteModelInfo(int i)
{
	FAssert(i > -1);
	FAssert(i < GC.getNumRouteModelInfos());
	return *(m_paRouteModelInfo[i]);
}

int CvGlobals::getNumRiverModelInfos()
{
	return (int)m_paRiverModelInfo.size();
}

std::vector<CvRiverModelInfo*>& CvGlobals::getRiverModelInfo()
{
	return m_paRiverModelInfo;
}

CvRiverModelInfo& CvGlobals::getRiverModelInfo(int i)
{
	FAssert(i > -1);
	FAssert(i < GC.getNumRiverModelInfos());
	return *(m_paRiverModelInfo[i]);
}

int CvGlobals::getNumWaterPlaneInfos()
{
	return (int)m_paWaterPlaneInfo.size();
}

std::vector<CvWaterPlaneInfo*>& CvGlobals::getWaterPlaneInfo()
{
	return m_paWaterPlaneInfo;
}

CvWaterPlaneInfo& CvGlobals::getWaterPlaneInfo(int i)
{
	FAssert(i > -1);
	FAssert(i < GC.getNumWaterPlaneInfos());
	return *(m_paWaterPlaneInfo[i]);
}

int CvGlobals::getNumTerrainPlaneInfos()
{
	return (int)m_paTerrainPlaneInfo.size();
}

std::vector<CvTerrainPlaneInfo*>& CvGlobals::getTerrainPlaneInfo()
{
	return m_paTerrainPlaneInfo;
}

CvTerrainPlaneInfo& CvGlobals::getTerrainPlaneInfo(int i)
{
	FAssert(i > -1);
	FAssert(i < GC.getNumTerrainPlaneInfos());
	return *(m_paTerrainPlaneInfo[i]);
}

int CvGlobals::getNumCameraOverlayInfos()
{
	return (int)m_paCameraOverlayInfo.size();
}

std::vector<CvCameraOverlayInfo*>& CvGlobals::getCameraOverlayInfo()
{
	return m_paCameraOverlayInfo;
}

CvCameraOverlayInfo& CvGlobals::getCameraOverlayInfo(int i)
{
	FAssert(i > -1);
	FAssert(i < GC.getNumCameraOverlayInfos());
	return *(m_paCameraOverlayInfo[i]);
}

int CvGlobals::getNumAnimationPathInfos()
{
	return (int)m_paAnimationPathInfo.size();
}

std::vector<CvAnimationPathInfo*>& CvGlobals::getAnimationPathInfo()
{
	return m_paAnimationPathInfo;
}

CvAnimationPathInfo& CvGlobals::getAnimationPathInfo(AnimationPathTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumAnimationPathInfos());
	return *(m_paAnimationPathInfo[e]);
}

int CvGlobals::getNumAnimationCategoryInfos()
{
	return (int)m_paAnimationCategoryInfo.size();
}

std::vector<CvAnimationCategoryInfo*>& CvGlobals::getAnimationCategoryInfo()
{
	return m_paAnimationCategoryInfo;
}

CvAnimationCategoryInfo& CvGlobals::getAnimationCategoryInfo(AnimationCategoryTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumAnimationCategoryInfos());
	return *(m_paAnimationCategoryInfo[e]);
}

int CvGlobals::getNumEntityEventInfos()
{
	return (int)m_paEntityEventInfo.size();
}

std::vector<CvEntityEventInfo*>& CvGlobals::getEntityEventInfo()
{
	return m_paEntityEventInfo;
}

CvEntityEventInfo& CvGlobals::getEntityEventInfo(EntityEventTypes e)
{
	FAssert( e > -1 );
	FAssert( e < GC.getNumEntityEventInfos() );
	return *(m_paEntityEventInfo[e]);
}

int CvGlobals::getNumEffectInfos()
{
	return (int)m_paEffectInfo.size();
}

std::vector<CvEffectInfo*>& CvGlobals::getEffectInfo()
{
	return m_paEffectInfo;
}

CvEffectInfo& CvGlobals::getEffectInfo(int i)
{
	FAssert(i > -1);
	FAssert(i < GC.getNumEffectInfos());
	return *(m_paEffectInfo[i]);
}


int CvGlobals::getNumAttachableInfos()
{
	return (int)m_paAttachableInfo.size();
}

std::vector<CvAttachableInfo*>& CvGlobals::getAttachableInfo()
{
	return m_paAttachableInfo;
}

CvAttachableInfo& CvGlobals::getAttachableInfo(int i)
{
	FAssert(i > -1);
	FAssert(i < GC.getNumAttachableInfos());
	return *(m_paAttachableInfo[i]);
}

int CvGlobals::getNumUnitFormationInfos()
{
	return (int)m_paUnitFormationInfo.size();
}

std::vector<CvUnitFormationInfo*>& CvGlobals::getUnitFormationInfo()		// For Moose - CvUnitEntity
{
	return m_paUnitFormationInfo;
}

CvUnitFormationInfo& CvGlobals::getUnitFormationInfo(int i)
{
	FAssert(i > -1);
	FAssert(i < GC.getNumUnitFormationInfos());
	return *(m_paUnitFormationInfo[i]);
}

// TEXT
int CvGlobals::getNumGameTextXML()
{
	return (int)m_paGameTextXML.size();
}

std::vector<CvGameText*>& CvGlobals::getGameTextXML()
{
	return m_paGameTextXML;
}

// Landscape INFOS
int CvGlobals::getNumLandscapeInfos()
{
	return (int)m_paLandscapeInfo.size();
}

std::vector<CvLandscapeInfo*>& CvGlobals::getLandscapeInfo()
{
	return m_paLandscapeInfo;
}

CvLandscapeInfo& CvGlobals::getLandscapeInfo(int iIndex)
{
	FAssert(iIndex > -1);
	FAssert(iIndex < GC.getNumLandscapeInfos());
	return *(m_paLandscapeInfo[iIndex]);
}

int CvGlobals::getActiveLandscapeID()
{
	return m_iActiveLandscapeID;
}

void CvGlobals::setActiveLandscapeID(int iLandscapeID)
{
	m_iActiveLandscapeID = iLandscapeID;
}


int CvGlobals::getNumTerrainInfos()
{
	return (int)m_paTerrainInfo.size();
}

std::vector<CvTerrainInfo*>& CvGlobals::getTerrainInfo()		// For Moose - XML Load Util, CvInfos, CvTerrainTypeWBPalette
{
	return m_paTerrainInfo;
}

CvTerrainInfo& CvGlobals::getTerrainInfo(TerrainTypes eTerrainNum)
{
	FAssert(eTerrainNum > -1);
	FAssert(eTerrainNum < (int)m_paTerrainInfo.size());
	return *(m_paTerrainInfo[eTerrainNum]);
}

int CvGlobals::getNumBonusInfos()
{
	return (int)m_paBonusInfo.size();
}

std::vector<CvBonusInfo*>& CvGlobals::getBonusInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paBonusInfo;
}

CvBonusInfo& CvGlobals::getBonusInfo(BonusTypes eBonusNum)
{
	FAssert(eBonusNum > -1);
	FAssert(eBonusNum < (int)m_paBonusInfo.size());
	return *(m_paBonusInfo[eBonusNum]);
}

int CvGlobals::getNumFeatureInfos()
{
	return (int)m_paFeatureInfo.size();
}

std::vector<CvFeatureInfo*>& CvGlobals::getFeatureInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paFeatureInfo;
}

CvFeatureInfo& CvGlobals::getFeatureInfo(FeatureTypes eFeatureNum)
{
	FAssert(eFeatureNum > -1);
	FAssert(eFeatureNum < (int)m_paFeatureInfo.size());
	return *(m_paFeatureInfo[eFeatureNum]);
}

int CvGlobals::getNumCivilizationInfos()
{
	return (int)m_paCivilizationInfo.size();
}

std::vector<CvCivilizationInfo*>& CvGlobals::getCivilizationInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paCivilizationInfo;
}

CvCivilizationInfo& CvGlobals::getCivilizationInfo(CivilizationTypes eCivilizationNum)
{
	FAssert(eCivilizationNum > -1);
	FAssert(eCivilizationNum < (int)m_paCivilizationInfo.size());
	return *(m_paCivilizationInfo[eCivilizationNum]);
}


int CvGlobals::getNumLeaderHeadInfos()
{
	return (int)m_paLeaderHeadInfo.size();
}

std::vector<CvLeaderHeadInfo*>& CvGlobals::getLeaderHeadInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paLeaderHeadInfo;
}

CvLeaderHeadInfo& CvGlobals::getLeaderHeadInfo(LeaderHeadTypes eLeaderHeadNum)
{
	FAssert(eLeaderHeadNum > -1);
	FAssert(eLeaderHeadNum < GC.getNumLeaderHeadInfos());
	return *(m_paLeaderHeadInfo[eLeaderHeadNum]);
}


int CvGlobals::getNumTraitInfos()
{
	return (int)m_paTraitInfo.size();
}

std::vector<CvTraitInfo*>& CvGlobals::getTraitInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paTraitInfo;
}

CvTraitInfo& CvGlobals::getTraitInfo(TraitTypes eTraitNum)
{
	FAssert(eTraitNum > -1);
	FAssert(eTraitNum < GC.getNumTraitInfos());
	return *(m_paTraitInfo[eTraitNum]);
}


int CvGlobals::getNumCursorInfos()
{
	return (int)m_paCursorInfo.size();
}

std::vector<CvCursorInfo*>& CvGlobals::getCursorInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paCursorInfo;
}

CvCursorInfo& CvGlobals::getCursorInfo(CursorTypes eCursorNum)
{
	FAssert(eCursorNum > -1);
	FAssert(eCursorNum < GC.getNumCursorInfos());
	return *(m_paCursorInfo[eCursorNum]);
}

int CvGlobals::getNumSlideShowInfos()
{
	return (int)m_paSlideShowInfo.size();
}

std::vector<CvSlideShowInfo*>& CvGlobals::getSlideShowInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paSlideShowInfo;
}

CvSlideShowInfo& CvGlobals::getSlideShowInfo(int iIndex)
{
	FAssert(iIndex > -1);
	FAssert(iIndex < GC.getNumSlideShowInfos());
	return *(m_paSlideShowInfo[iIndex]);
}

int CvGlobals::getNumSlideShowRandomInfos()
{
	return (int)m_paSlideShowRandomInfo.size();
}

std::vector<CvSlideShowRandomInfo*>& CvGlobals::getSlideShowRandomInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paSlideShowRandomInfo;
}

CvSlideShowRandomInfo& CvGlobals::getSlideShowRandomInfo(int iIndex)
{
	FAssert(iIndex > -1);
	FAssert(iIndex < GC.getNumSlideShowRandomInfos());
	return *(m_paSlideShowRandomInfo[iIndex]);
}

int CvGlobals::getNumWorldPickerInfos()
{
	return (int)m_paWorldPickerInfo.size();
}

std::vector<CvWorldPickerInfo*>& CvGlobals::getWorldPickerInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paWorldPickerInfo;
}

CvWorldPickerInfo& CvGlobals::getWorldPickerInfo(int iIndex)
{
	FAssert(iIndex > -1);
	FAssert(iIndex < GC.getNumWorldPickerInfos());
	return *(m_paWorldPickerInfo[iIndex]);
}

int CvGlobals::getNumUnitInfos()
{
	return (int)m_paUnitInfo.size();
}

std::vector<CvUnitInfo*>& CvGlobals::getUnitInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paUnitInfo;
}

CvUnitInfo& CvGlobals::getUnitInfo(UnitTypes eUnitNum)
{
	FAssert(eUnitNum > -1);
	FAssert(eUnitNum < GC.getNumUnitInfos());
	return *(m_paUnitInfo[eUnitNum]);
}

int CvGlobals::getNumSpecialUnitInfos()
{
	return (int)m_paSpecialUnitInfo.size();
}

std::vector<CvSpecialUnitInfo*>& CvGlobals::getSpecialUnitInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paSpecialUnitInfo;
}

CvSpecialUnitInfo& CvGlobals::getSpecialUnitInfo(SpecialUnitTypes eSpecialUnitNum)
{
	FAssert(eSpecialUnitNum > -1);
	FAssert(eSpecialUnitNum < GC.getNumSpecialUnitInfos());
	return *(m_paSpecialUnitInfo[eSpecialUnitNum]);
}


int CvGlobals::getNumConceptInfos()
{
	return (int)m_paConceptInfo.size();
}

std::vector<CvInfoBase*>& CvGlobals::getConceptInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paConceptInfo;
}

CvInfoBase& CvGlobals::getConceptInfo(ConceptTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumConceptInfos());
	return *(m_paConceptInfo[e]);
}
int CvGlobals::getNumCalendarInfos()
{
	return (int)m_paCalendarInfo.size();
}

std::vector<CvInfoBase*>& CvGlobals::getCalendarInfo()
{
	return m_paCalendarInfo;
}

CvInfoBase& CvGlobals::getCalendarInfo(CalendarTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumCalendarInfos());
	return *(m_paCalendarInfo[e]);
}


int CvGlobals::getNumSeasonInfos()
{
	return (int)m_paSeasonInfo.size();
}

std::vector<CvInfoBase*>& CvGlobals::getSeasonInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paSeasonInfo;
}

CvInfoBase& CvGlobals::getSeasonInfo(SeasonTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumSeasonInfos());
	return *(m_paSeasonInfo[e]);
}


int CvGlobals::getNumMonthInfos()
{
	return (int)m_paMonthInfo.size();
}

std::vector<CvInfoBase*>& CvGlobals::getMonthInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paMonthInfo;
}

CvInfoBase& CvGlobals::getMonthInfo(MonthTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumMonthInfos());
	return *(m_paMonthInfo[e]);
}


int CvGlobals::getNumDenialInfos()
{
	return (int)m_paDenialInfo.size();
}

std::vector<CvInfoBase*>& CvGlobals::getDenialInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paDenialInfo;
}

CvInfoBase& CvGlobals::getDenialInfo(DenialTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumDenialInfos());
	return *(m_paDenialInfo[e]);
}


int CvGlobals::getNumInvisibleInfos()
{
	return (int)m_paInvisibleInfo.size();
}

std::vector<CvInfoBase*>& CvGlobals::getInvisibleInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paInvisibleInfo;
}

CvInfoBase& CvGlobals::getInvisibleInfo(InvisibleTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumInvisibleInfos());
	return *(m_paInvisibleInfo[e]);
}


int CvGlobals::getNumFatherInfos()
{
	return (int)m_paFatherInfo.size();
}

std::vector<CvFatherInfo*>& CvGlobals::getFatherInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paFatherInfo;
}

CvFatherInfo& CvGlobals::getFatherInfo(FatherTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumFatherInfos());
	return *(m_paFatherInfo[e]);
}

int CvGlobals::getNumFatherPointInfos()
{
	return (int)m_paFatherPointInfo.size();
}

std::vector<CvFatherPointInfo*>& CvGlobals::getFatherPointInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paFatherPointInfo;
}

CvFatherPointInfo& CvGlobals::getFatherPointInfo(FatherPointTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumFatherPointInfos());
	return *(m_paFatherPointInfo[e]);
}

int CvGlobals::getNumUnitCombatInfos()
{
	return (int)m_paUnitCombatInfo.size();
}

std::vector<CvInfoBase*>& CvGlobals::getUnitCombatInfo()
{
	return m_paUnitCombatInfo;
}

CvInfoBase& CvGlobals::getUnitCombatInfo(UnitCombatTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumUnitCombatInfos());
	return *(m_paUnitCombatInfo[e]);
}


std::vector<CvInfoBase*>& CvGlobals::getDomainInfo()
{
	return m_paDomainInfo;
}

CvInfoBase& CvGlobals::getDomainInfo(DomainTypes e)
{
	FAssert(e > -1);
	FAssert(e < NUM_DOMAIN_TYPES);
	return *(m_paDomainInfo[e]);
}


std::vector<CvInfoBase*>& CvGlobals::getUnitAIInfo()
{
	return m_paUnitAIInfos;
}

CvInfoBase& CvGlobals::getUnitAIInfo(UnitAITypes eUnitAINum)
{
	FAssert(eUnitAINum >= 0);
	FAssert(eUnitAINum < NUM_UNITAI_TYPES);
	return *(m_paUnitAIInfos[eUnitAINum]);
}


std::vector<CvInfoBase*>& CvGlobals::getAttitudeInfo()
{
	return m_paAttitudeInfos;
}

CvInfoBase& CvGlobals::getAttitudeInfo(AttitudeTypes eAttitudeNum)
{
	FAssert(eAttitudeNum >= 0);
	FAssert(eAttitudeNum < NUM_ATTITUDE_TYPES);
	return *(m_paAttitudeInfos[eAttitudeNum]);
}


std::vector<CvInfoBase*>& CvGlobals::getMemoryInfo()
{
	return m_paMemoryInfos;
}

CvInfoBase& CvGlobals::getMemoryInfo(MemoryTypes eMemoryNum)
{
	FAssert(eMemoryNum >= 0);
	FAssert(eMemoryNum < NUM_MEMORY_TYPES);
	return *(m_paMemoryInfos[eMemoryNum]);
}

int CvGlobals::getNumFatherCategoryInfos()
{
	return (int)m_paFatherCategoryInfos.size();
}

std::vector<CvInfoBase*>& CvGlobals::getFatherCategoryInfo()
{
	return m_paFatherCategoryInfos;
}

CvInfoBase& CvGlobals::getFatherCategoryInfo(FatherCategoryTypes eFatherCategoryNum)
{
	FAssert(eFatherCategoryNum >= 0);
	FAssert(eFatherCategoryNum < GC.getNumFatherCategoryInfos());
	return *(m_paFatherCategoryInfos[eFatherCategoryNum]);
}

int CvGlobals::getNumGameOptionInfos()
{
	return (int)m_paGameOptionInfos.size();
}

std::vector<CvGameOptionInfo*>& CvGlobals::getGameOptionInfo()
{
	return m_paGameOptionInfos;
}

CvGameOptionInfo& CvGlobals::getGameOptionInfo(GameOptionTypes eGameOptionNum)
{
	FAssert(eGameOptionNum >= 0);
	FAssert(eGameOptionNum < GC.getNumGameOptionInfos());
	return *(m_paGameOptionInfos[eGameOptionNum]);
}

int CvGlobals::getNumMPOptionInfos()
{
	return (int)m_paMPOptionInfos.size();
}

std::vector<CvMPOptionInfo*>& CvGlobals::getMPOptionInfo()
{
	 return m_paMPOptionInfos;
}

CvMPOptionInfo& CvGlobals::getMPOptionInfo(MultiplayerOptionTypes eMPOptionNum)
{
	FAssert(eMPOptionNum >= 0);
	FAssert(eMPOptionNum < GC.getNumMPOptionInfos());
	return *(m_paMPOptionInfos[eMPOptionNum]);
}

int CvGlobals::getNumForceControlInfos()
{
	return (int)m_paForceControlInfos.size();
}

std::vector<CvForceControlInfo*>& CvGlobals::getForceControlInfo()
{
	return m_paForceControlInfos;
}

CvForceControlInfo& CvGlobals::getForceControlInfo(ForceControlTypes eForceControlNum)
{
	FAssert(eForceControlNum >= 0);
	FAssert(eForceControlNum < GC.getNumForceControlInfos());
	return *(m_paForceControlInfos[eForceControlNum]);
}

std::vector<CvPlayerOptionInfo*>& CvGlobals::getPlayerOptionInfo()
{
	return m_paPlayerOptionInfos;
}

CvPlayerOptionInfo& CvGlobals::getPlayerOptionInfo(PlayerOptionTypes ePlayerOptionNum)
{
	FAssert(ePlayerOptionNum >= 0);
	FAssert(ePlayerOptionNum < NUM_PLAYEROPTION_TYPES);
	return *(m_paPlayerOptionInfos[ePlayerOptionNum]);
}

std::vector<CvGraphicOptionInfo*>& CvGlobals::getGraphicOptionInfo()
{
	return m_paGraphicOptionInfos;
}

CvGraphicOptionInfo& CvGlobals::getGraphicOptionInfo(GraphicOptionTypes eGraphicOptionNum)
{
	FAssert(eGraphicOptionNum >= 0);
	FAssert(eGraphicOptionNum < NUM_GRAPHICOPTION_TYPES);
	return *(m_paGraphicOptionInfos[eGraphicOptionNum]);
}


std::vector<CvYieldInfo*>& CvGlobals::getYieldInfo()	// For Moose - XML Load Util
{
	return m_paYieldInfo;
}

CvYieldInfo& CvGlobals::getYieldInfo(YieldTypes eYieldNum)
{
	FAssert(eYieldNum > -1);
	FAssert(eYieldNum < NUM_YIELD_TYPES);
	return *(m_paYieldInfo[eYieldNum]);
}


int CvGlobals::getNumRouteInfos()
{
	return (int)m_paRouteInfo.size();
}

std::vector<CvRouteInfo*>& CvGlobals::getRouteInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paRouteInfo;
}

CvRouteInfo& CvGlobals::getRouteInfo(RouteTypes eRouteNum)
{
	FAssert(eRouteNum > -1);
	FAssert(eRouteNum < GC.getNumRouteInfos());
	return *(m_paRouteInfo[eRouteNum]);
}

int CvGlobals::getNumImprovementInfos()
{
	return (int)m_paImprovementInfo.size();
}

std::vector<CvImprovementInfo*>& CvGlobals::getImprovementInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paImprovementInfo;
}

CvImprovementInfo& CvGlobals::getImprovementInfo(ImprovementTypes eImprovementNum)
{
	FAssert(eImprovementNum > -1);
	FAssert(eImprovementNum < GC.getNumImprovementInfos());
	return *(m_paImprovementInfo[eImprovementNum]);
}

int CvGlobals::getNumGoodyInfos()
{
	return (int)m_paGoodyInfo.size();
}

std::vector<CvGoodyInfo*>& CvGlobals::getGoodyInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paGoodyInfo;
}

CvGoodyInfo& CvGlobals::getGoodyInfo(GoodyTypes eGoodyNum)
{
	FAssert(eGoodyNum > -1);
	FAssert(eGoodyNum < GC.getNumGoodyInfos());
	return *(m_paGoodyInfo[eGoodyNum]);
}

int CvGlobals::getNumBuildInfos()
{
	return (int)m_paBuildInfo.size();
}

std::vector<CvBuildInfo*>& CvGlobals::getBuildInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paBuildInfo;
}

CvBuildInfo& CvGlobals::getBuildInfo(BuildTypes eBuildNum)
{
	FAssert(eBuildNum > -1);
	FAssert(eBuildNum < GC.getNumBuildInfos());
	return *(m_paBuildInfo[eBuildNum]);
}

int CvGlobals::getNumHandicapInfos()
{
	return (int)m_paHandicapInfo.size();
}

std::vector<CvHandicapInfo*>& CvGlobals::getHandicapInfo()	// Do NOT export outside of the DLL	// For Moose - XML Load Util
{
	return m_paHandicapInfo;
}

CvHandicapInfo& CvGlobals::getHandicapInfo(HandicapTypes eHandicapNum)
{
	FAssert(eHandicapNum > -1);
	FAssert(eHandicapNum < GC.getNumHandicapInfos());
	return *(m_paHandicapInfo[eHandicapNum]);
}

int CvGlobals::getNumGameSpeedInfos()
{
	return (int)m_paGameSpeedInfo.size();
}

std::vector<CvGameSpeedInfo*>& CvGlobals::getGameSpeedInfo()	// Do NOT export outside of the DLL	// For Moose - XML Load Util
{
	return m_paGameSpeedInfo;
}

CvGameSpeedInfo& CvGlobals::getGameSpeedInfo(GameSpeedTypes eGameSpeedNum)
{
	FAssert(eGameSpeedNum > -1);
	FAssert(eGameSpeedNum < GC.getNumGameSpeedInfos());
	return *(m_paGameSpeedInfo[eGameSpeedNum]);
}

int CvGlobals::getNumAlarmInfos()
{
	return (int)m_paAlarmInfo.size();
}
///TKs Invention Core Mod v 1.0
int CvGlobals::getCostToResearch(CivicTypes eCivic)
{
    if (eCivic != NO_CIVIC)
    {
        ///TK Update 1.1b
        CvCivicInfo& kCivicInfo = GC.getCivicInfo(eCivic);
        int iCivicResearchCost = kCivicInfo.getCostToResearch();
        iCivicResearchCost = (iCivicResearchCost * GC.getXMLval(XML_TK_HUMAN_RESEARCH_COST_MOD_PERCENT)) / 100;
        return iCivicResearchCost;
    }
    return 0;

}

///Equipment Types 0 = Any; 1 = Heavy Armor; 2 = Any Armor; 3 = Armor & Horses
bool CvGlobals::isEquipmentType(YieldTypes eEquipment, UnitEquipmentTypes eType) const
{
    if (eType == EQUIPMENT_ANY)
    {
        if (YieldGroup_Armor(eEquipment))
        //if (eEquipment == YIELD_LEATHER_ARMOR || eEquipment == YIELD_SCALE_ARMOR)
        {
            return true;
        }

        if (eEquipment == YIELD_WEAPONS || eEquipment == YIELD_HORSES)
        {
            return true;
        }
    }
    else if (eType == EQUIPMENT_HEAVY_ARMOR)
    {
        if (YieldGroup_Heavy_Armor(eEquipment))
        //if (eEquipment == YIELD_SCALE_ARMOR)
        {
            return true;
        }
    }
    else if (eType == EQUIPMENT_ANY_ARMOR)
    {
        if (YieldGroup_Armor(eEquipment))
        //if (eEquipment == YIELD_LEATHER_ARMOR || eEquipment == YIELD_SCALE_ARMOR)
        {
            return true;
        }
    }
    else if (eType == EQUIPMENT_ARMOR_HORSES)
    {
        if (YieldGroup_Armor(eEquipment))
        //if (eEquipment == YIELD_LEATHER_ARMOR || eEquipment == YIELD_SCALE_ARMOR || eEquipment == YIELD_MAIL_ARMOR || eEquipment == YIELD_PLATE_ARMOR)
        {
            return true;
        }

        if (eEquipment == YIELD_HORSES)
        {
            return true;
        }
    }
    return false;
}
///TKe

std::vector<CvAlarmInfo*>& CvGlobals::getAlarmInfo()	// Do NOT export outside of the DLL	// For Moose - XML Load Util
{
	return m_paAlarmInfo;
}

CvAlarmInfo& CvGlobals::getAlarmInfo(AlarmTypes eAlarm)
{
	FAssert(eAlarm > -1);
	FAssert(eAlarm < GC.getNumAlarmInfos());
	return *(m_paAlarmInfo[eAlarm]);
}

int CvGlobals::getNumTurnTimerInfos()
{
	return (int)m_paTurnTimerInfo.size();
}

std::vector<CvTurnTimerInfo*>& CvGlobals::getTurnTimerInfo()	// Do NOT export outside of the DLL	// For Moose - XML Load Util
{
	return m_paTurnTimerInfo;
}

CvTurnTimerInfo& CvGlobals::getTurnTimerInfo(TurnTimerTypes eTurnTimerNum)
{
	FAssert(eTurnTimerNum > -1);
	FAssert(eTurnTimerNum < GC.getNumTurnTimerInfos());
	return *(m_paTurnTimerInfo[eTurnTimerNum]);
}

int CvGlobals::getNumBuildingClassInfos()
{
	return (int)m_paBuildingClassInfo.size();
}

std::vector<CvBuildingClassInfo*>& CvGlobals::getBuildingClassInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paBuildingClassInfo;
}

CvBuildingClassInfo& CvGlobals::getBuildingClassInfo(BuildingClassTypes eBuildingClassNum)
{
	FAssert(eBuildingClassNum > -1);
	FAssert(eBuildingClassNum < GC.getNumBuildingClassInfos());
	return *(m_paBuildingClassInfo[eBuildingClassNum]);
}

int CvGlobals::getNumBuildingInfos()
{
	return (int)m_paBuildingInfo.size();
}

std::vector<CvBuildingInfo*>& CvGlobals::getBuildingInfo()	// For Moose - XML Load Util, CvInfos, CvCacheObject
{
	return m_paBuildingInfo;
}

CvBuildingInfo& CvGlobals::getBuildingInfo(BuildingTypes eBuildingNum)
{
	FAssert(eBuildingNum > -1);
	FAssert(eBuildingNum < GC.getNumBuildingInfos());
	return *(m_paBuildingInfo[eBuildingNum]);
}

int CvGlobals::getNumSpecialBuildingInfos()
{
	return (int)m_paSpecialBuildingInfo.size();
}

std::vector<CvSpecialBuildingInfo*>& CvGlobals::getSpecialBuildingInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paSpecialBuildingInfo;
}

CvSpecialBuildingInfo& CvGlobals::getSpecialBuildingInfo(SpecialBuildingTypes eSpecialBuildingNum)
{
	FAssert(eSpecialBuildingNum > -1);
	FAssert(eSpecialBuildingNum < GC.getNumSpecialBuildingInfos());
	return *(m_paSpecialBuildingInfo[eSpecialBuildingNum]);
}

int CvGlobals::getNumUnitClassInfos()
{
	return (int)m_paUnitClassInfo.size();
}

std::vector<CvUnitClassInfo*>& CvGlobals::getUnitClassInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paUnitClassInfo;
}

CvUnitClassInfo& CvGlobals::getUnitClassInfo(UnitClassTypes eUnitClassNum)
{
	FAssert(eUnitClassNum > -1);
	FAssert(eUnitClassNum < GC.getNumUnitClassInfos());
	return *(m_paUnitClassInfo[eUnitClassNum]);
}

int CvGlobals::getNumActionInfos()
{
	return (int)m_paActionInfo.size();
}

std::vector<CvActionInfo*>& CvGlobals::getActionInfo()	// For Moose - XML Load Util
{
	return m_paActionInfo;
}

CvActionInfo& CvGlobals::getActionInfo(int i)
{
	FAssertMsg(i < getNumActionInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return *(m_paActionInfo[i]);
}

std::vector<CvMissionInfo*>& CvGlobals::getMissionInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paMissionInfo;
}

CvMissionInfo& CvGlobals::getMissionInfo(MissionTypes eMissionNum)
{
	FAssert(eMissionNum > -1);
	FAssert(eMissionNum < NUM_MISSION_TYPES);
	return *(m_paMissionInfo[eMissionNum]);
}

std::vector<CvControlInfo*>& CvGlobals::getControlInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paControlInfo;
}

CvControlInfo& CvGlobals::getControlInfo(ControlTypes eControlNum)
{
	FAssert(eControlNum > -1);
	FAssert(eControlNum < NUM_CONTROL_TYPES);
	FAssert(m_paControlInfo.size() > 0);
	return *(m_paControlInfo[eControlNum]);
}

std::vector<CvCommandInfo*>& CvGlobals::getCommandInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paCommandInfo;
}

CvCommandInfo& CvGlobals::getCommandInfo(CommandTypes eCommandNum)
{
	FAssert(eCommandNum > -1);
	FAssert(eCommandNum < NUM_COMMAND_TYPES);
	return *(m_paCommandInfo[eCommandNum]);
}

int CvGlobals::getNumAutomateInfos()
{
	return (int)m_paAutomateInfo.size();
}

std::vector<CvAutomateInfo*>& CvGlobals::getAutomateInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paAutomateInfo;
}

CvAutomateInfo& CvGlobals::getAutomateInfo(int iAutomateNum)
{
	FAssertMsg(iAutomateNum < getNumAutomateInfos(), "Index out of bounds");
	FAssertMsg(iAutomateNum > -1, "Index out of bounds");
	return *(m_paAutomateInfo[iAutomateNum]);
}

int CvGlobals::getNumPromotionInfos()
{
	return (int)m_paPromotionInfo.size();
}

std::vector<CvPromotionInfo*>& CvGlobals::getPromotionInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paPromotionInfo;
}

CvPromotionInfo& CvGlobals::getPromotionInfo(PromotionTypes ePromotionNum)
{
	FAssert(ePromotionNum > -1);
	FAssert(ePromotionNum < GC.getNumPromotionInfos());
	return *(m_paPromotionInfo[ePromotionNum]);
}

int CvGlobals::getNumProfessionInfos()
{
	return (int)m_paProfessionInfo.size();
}

std::vector<CvProfessionInfo*>& CvGlobals::getProfessionInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paProfessionInfo;
}

CvProfessionInfo& CvGlobals::getProfessionInfo(ProfessionTypes eProfessionNum)
{
	FAssert(eProfessionNum > -1);
	FAssert(eProfessionNum < (int)m_paProfessionInfo.size());
	return *(m_paProfessionInfo[eProfessionNum]);
}
int CvGlobals::getNumCivicOptionInfos()
{
	return (int)m_paCivicOptionInfo.size();
}

std::vector<CvInfoBase*>& CvGlobals::getCivicOptionInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paCivicOptionInfo;
}

CvInfoBase& CvGlobals::getCivicOptionInfo(CivicOptionTypes eCivicOptionNum)
{
	FAssert(eCivicOptionNum > -1);
	FAssert(eCivicOptionNum < GC.getNumCivicOptionInfos());
	return *(m_paCivicOptionInfo[eCivicOptionNum]);
}

int CvGlobals::getNumCivicInfos()
{
	return (int)m_paCivicInfo.size();
}

std::vector<CvCivicInfo*>& CvGlobals::getCivicInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paCivicInfo;
}

CvCivicInfo& CvGlobals::getCivicInfo(CivicTypes eCivicNum)
{
	FAssert(eCivicNum > -1);
	FAssert(eCivicNum < GC.getNumCivicInfos());
	return *(m_paCivicInfo[eCivicNum]);
}

int CvGlobals::getNumDiplomacyInfos()
{
	return (int)m_paDiplomacyInfo.size();
}

std::vector<CvDiplomacyInfo*>& CvGlobals::getDiplomacyInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paDiplomacyInfo;
}

CvDiplomacyInfo& CvGlobals::getDiplomacyInfo(int iDiplomacyNum)
{
	FAssertMsg(iDiplomacyNum < getNumDiplomacyInfos(), "Index out of bounds");
	FAssertMsg(iDiplomacyNum > -1, "Index out of bounds");
	return *(m_paDiplomacyInfo[iDiplomacyNum]);
}

int CvGlobals::getNumEraInfos()
{
	return (int)m_aEraInfo.size();
}

std::vector<CvEraInfo*>& CvGlobals::getEraInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_aEraInfo;
}

CvEraInfo& CvGlobals::getEraInfo(EraTypes eEraNum)
{
	FAssert(eEraNum > -1);
	FAssert(eEraNum < GC.getNumEraInfos());
	return *(m_aEraInfo[eEraNum]);
}

int CvGlobals::getNumHurryInfos()
{
	return (int)m_paHurryInfo.size();
}

std::vector<CvHurryInfo*>& CvGlobals::getHurryInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paHurryInfo;
}

CvHurryInfo& CvGlobals::getHurryInfo(HurryTypes eHurryNum)
{
	FAssert(eHurryNum > -1);
	FAssert(eHurryNum < GC.getNumHurryInfos());
	return *(m_paHurryInfo[eHurryNum]);
}

int CvGlobals::getNumEmphasizeInfos()
{
	return (int)m_paEmphasizeInfo.size();
}

std::vector<CvEmphasizeInfo*>& CvGlobals::getEmphasizeInfo()	// For Moose - XML Load Util
{
	return m_paEmphasizeInfo;
}

CvEmphasizeInfo& CvGlobals::getEmphasizeInfo(EmphasizeTypes eEmphasizeNum)
{
	FAssert(eEmphasizeNum > -1);
	FAssert(eEmphasizeNum < GC.getNumEmphasizeInfos());
	return *(m_paEmphasizeInfo[eEmphasizeNum]);
}

int CvGlobals::getNumCultureLevelInfos()
{
	return (int)m_paCultureLevelInfo.size();
}

std::vector<CvCultureLevelInfo*>& CvGlobals::getCultureLevelInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paCultureLevelInfo;
}

CvCultureLevelInfo& CvGlobals::getCultureLevelInfo(CultureLevelTypes eCultureLevelNum)
{
	FAssert(eCultureLevelNum > -1);
	FAssert(eCultureLevelNum < GC.getNumCultureLevelInfos());
	return *(m_paCultureLevelInfo[eCultureLevelNum]);
}

int CvGlobals::getNumVictoryInfos()
{
	return (int)m_paVictoryInfo.size();
}

std::vector<CvVictoryInfo*>& CvGlobals::getVictoryInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paVictoryInfo;
}

CvVictoryInfo& CvGlobals::getVictoryInfo(VictoryTypes eVictoryNum)
{
	FAssert(eVictoryNum > -1);
	FAssert(eVictoryNum < GC.getNumVictoryInfos());
	return *(m_paVictoryInfo[eVictoryNum]);
}

int CvGlobals::getNumEventTriggerInfos()
{
	return (int)m_paEventTriggerInfo.size();
}

std::vector<CvEventTriggerInfo*>& CvGlobals::getEventTriggerInfo()
{
	return m_paEventTriggerInfo;
}

CvEventTriggerInfo& CvGlobals::getEventTriggerInfo(EventTriggerTypes eEventTrigger)
{
	FAssert(eEventTrigger > -1);
	FAssert(eEventTrigger < GC.getNumEventTriggerInfos());
	return *(m_paEventTriggerInfo[eEventTrigger]);
}

int CvGlobals::getNumEventInfos()
{
	return (int)m_paEventInfo.size();
}

std::vector<CvEventInfo*>& CvGlobals::getEventInfo()
{
	return m_paEventInfo;
}

CvEventInfo& CvGlobals::getEventInfo(EventTypes eEvent)
{
	FAssert(eEvent > -1);
	FAssert(eEvent < GC.getNumEventInfos());
	return *(m_paEventInfo[eEvent]);
}

int& CvGlobals::getNumEntityEventTypes()
{
	return m_iNumEntityEventTypes;
}

CvString*& CvGlobals::getEntityEventTypes()
{
	return m_paszEntityEventTypes;
}

CvString& CvGlobals::getEntityEventTypes(EntityEventTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumEntityEventTypes());
	return m_paszEntityEventTypes[e];
}

int& CvGlobals::getNumAnimationOperatorTypes()
{
	return m_iNumAnimationOperatorTypes;
}

CvString*& CvGlobals::getAnimationOperatorTypes()
{
	return m_paszAnimationOperatorTypes;
}

CvString& CvGlobals::getAnimationOperatorTypes(AnimationOperatorTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumAnimationOperatorTypes());
	return m_paszAnimationOperatorTypes[e];
}

CvString*& CvGlobals::getFunctionTypes()
{
	return m_paszFunctionTypes;
}

CvString& CvGlobals::getFunctionTypes(FunctionTypes e)
{
	FAssert(e > -1);
	FAssert(e < NUM_FUNC_TYPES);
	return m_paszFunctionTypes[e];
}

int& CvGlobals::getNumArtStyleTypes()
{
	return m_iNumArtStyleTypes;
}

CvString*& CvGlobals::getArtStyleTypes()
{
	return m_paszArtStyleTypes;
}

CvString& CvGlobals::getArtStyleTypes(ArtStyleTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumArtStyleTypes());
	return m_paszArtStyleTypes[e];
}


//Androrc UnitArtStyles
int CvGlobals::getNumUnitArtStyleTypeInfos()
{
    return (int)m_paUnitArtStyleTypeInfo.size();
}

std::vector<CvUnitArtStyleTypeInfo*>& CvGlobals::getUnitArtStyleTypeInfo()
{
	return m_paUnitArtStyleTypeInfo;
}

CvUnitArtStyleTypeInfo& CvGlobals::getUnitArtStyleTypeInfo(UnitArtStyleTypes eUnitArtStyleTypeNum)
{
	FAssert(eUnitArtStyleTypeNum > -1);
	FAssert(eUnitArtStyleTypeNum < GC.getNumUnitArtStyleTypeInfos());
	return *(m_paUnitArtStyleTypeInfo[eUnitArtStyleTypeNum]);
}
//Androrc End

int& CvGlobals::getNumCitySizeTypes()
{
	return m_iNumCitySizeTypes;
}

CvString*& CvGlobals::getCitySizeTypes()
{
	return m_paszCitySizeTypes;
}

CvString& CvGlobals::getCitySizeTypes(int i)
{
	FAssertMsg(i < getNumCitySizeTypes(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paszCitySizeTypes[i];
}

CvString*& CvGlobals::getContactTypes()
{
	return m_paszContactTypes;
}

CvString& CvGlobals::getContactTypes(ContactTypes e)
{
	FAssert(e > -1);
	FAssert(e < NUM_CONTACT_TYPES);
	return m_paszContactTypes[e];
}

CvString*& CvGlobals::getDiplomacyPowerTypes()
{
	return m_paszDiplomacyPowerTypes;
}

CvString& CvGlobals::getDiplomacyPowerTypes(DiplomacyPowerTypes e)
{
	FAssert(e > -1);
	FAssert(e < NUM_DIPLOMACYPOWER_TYPES);
	return m_paszDiplomacyPowerTypes[e];
}

CvString*& CvGlobals::getAutomateTypes()
{
	return m_paszAutomateTypes;
}

CvString& CvGlobals::getAutomateTypes(AutomateTypes e)
{
	FAssert(e > -1);
	FAssert(e < NUM_AUTOMATE_TYPES);
	return m_paszAutomateTypes[e];
}
///TKs Med
CvString*& CvGlobals::getMedCityTypes()
{
	return m_paszMedCityTypes;
}

CvString*& CvGlobals::getModCodeTypes()
{
	return m_paszModCodeTypes;
}

CvString& CvGlobals::getModCodeTypes(ModCodeTypes e)
{
	FAssert(e > -1);
	FAssert(e < NUM_MOD_CODE_TYPES);
	return m_paszModCodeTypes[e];
}

CvString& CvGlobals::getMedCityTypes(MedCityTypes e)
{
	FAssert(e > -1);
	FAssert(e < NUM_CITY_TYPES);
	return m_paszMedCityTypes[e];
}
CvString*& CvGlobals::getTradeScreenTypes()
{
	return m_paszTradeScreenTypes;
}

CvString& CvGlobals::getTradeScreenTypes(TradeScreenTypes e)
{
	FAssert(e > -1);
	FAssert(e < NUM_TRADE_SCREEN_TYPES);
	return m_paszTradeScreenTypes[e];
}
///Tke

CvString*& CvGlobals::getDirectionTypes()
{
	return m_paszDirectionTypes;
}

CvString& CvGlobals::getDirectionTypes(AutomateTypes e)
{
	FAssert(e > -1);
	FAssert(e < NUM_DIRECTION_TYPES);
	return m_paszDirectionTypes[e];
}

int& CvGlobals::getNumFootstepAudioTypes()
{
	return m_iNumFootstepAudioTypes;
}

CvString*& CvGlobals::getFootstepAudioTypes()
{
	return m_paszFootstepAudioTypes;
}

CvString& CvGlobals::getFootstepAudioTypes(int i)
{
	FAssertMsg(i < getNumFootstepAudioTypes(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paszFootstepAudioTypes[i];
}

int CvGlobals::getFootstepAudioTypeByTag(CvString strTag)
{
	int iIndex = -1;

	if ( strTag.GetLength() <= 0 )
	{
		return iIndex;
	}

	for ( int i = 0; i < m_iNumFootstepAudioTypes; i++ )
	{
		if ( strTag.CompareNoCase(m_paszFootstepAudioTypes[i]) == 0 )
		{
			iIndex = i;
			break;
		}
	}

	return iIndex;
}

CvString*& CvGlobals::getFootstepAudioTags()
{
	return m_paszFootstepAudioTags;
}

CvString& CvGlobals::getFootstepAudioTags(int i)
{
//	FAssertMsg(i < getNumFootstepAudioTags(), "Index out of bounds")
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paszFootstepAudioTags[i];
}

void CvGlobals::setCurrentXMLFile(const TCHAR* szFileName)
{
	m_szCurrentXMLFile = szFileName;
}

CvString& CvGlobals::getCurrentXMLFile()
{
	return m_szCurrentXMLFile;
}

FVariableSystem* CvGlobals::getDefinesVarSystem()
{
	return m_VarSystem;
}

void CvGlobals::cacheGlobals()
{
	m_iMOVE_DENOMINATOR = getDefineINT("MOVE_DENOMINATOR");
	m_iFOOD_CONSUMPTION_PER_POPULATION = getDefineINT("FOOD_CONSUMPTION_PER_POPULATION");
	m_iMAX_HIT_POINTS = getDefineINT("MAX_HIT_POINTS");
	m_iHILLS_EXTRA_DEFENSE = getDefineINT("HILLS_EXTRA_DEFENSE");
	m_iRIVER_ATTACK_MODIFIER = getDefineINT("RIVER_ATTACK_MODIFIER");
	m_iAMPHIB_ATTACK_MODIFIER = getDefineINT("AMPHIB_ATTACK_MODIFIER");
	m_iHILLS_EXTRA_MOVEMENT = getDefineINT("HILLS_EXTRA_MOVEMENT");
	m_iPEAK_EXTRA_MOVEMENT = getDefineINT("PEAK_EXTRA_MOVEMENT");
	m_iMAX_PLOT_LIST_ROWS = getDefineINT("MAX_PLOT_LIST_ROWS");
	m_iUNIT_MULTISELECT_MAX = getDefineINT("UNIT_MULTISELECT_MAX");
	m_iEVENT_MESSAGE_TIME = getDefineINT("EVENT_MESSAGE_TIME");
	m_iROUTE_FEATURE_GROWTH_MODIFIER = getDefineINT("ROUTE_FEATURE_GROWTH_MODIFIER");
	m_iFEATURE_GROWTH_MODIFIER = getDefineINT("FEATURE_GROWTH_MODIFIER");
	m_iMIN_CITY_RANGE = getDefineINT("MIN_CITY_RANGE");
	m_iCITY_MAX_NUM_BUILDINGS = getDefineINT("CITY_MAX_NUM_BUILDINGS");
	m_iLAKE_MAX_AREA_SIZE = getDefineINT("LAKE_MAX_AREA_SIZE");
	m_iMIN_WATER_SIZE_FOR_OCEAN = getDefineINT("MIN_WATER_SIZE_FOR_OCEAN");
	m_iFORTIFY_MODIFIER_PER_TURN = getDefineINT("FORTIFY_MODIFIER_PER_TURN");
	m_iMAX_CITY_DEFENSE_DAMAGE = getDefineINT("MAX_CITY_DEFENSE_DAMAGE");
	m_iPEAK_SEE_THROUGH_CHANGE = getDefineINT("PEAK_SEE_THROUGH_CHANGE");
	m_iHILLS_SEE_THROUGH_CHANGE = getDefineINT("HILLS_SEE_THROUGH_CHANGE");
	m_iSEAWATER_SEE_FROM_CHANGE = getDefineINT("SEAWATER_SEE_FROM_CHANGE");
	m_iPEAK_SEE_FROM_CHANGE = getDefineINT("PEAK_SEE_FROM_CHANGE");
	m_iHILLS_SEE_FROM_CHANGE = getDefineINT("HILLS_SEE_FROM_CHANGE");
	m_iMAX_REBEL_YIELD_MODIFIER = getDefineINT("MAX_REBEL_YIELD_MODIFIER");

	m_fCAMERA_MIN_YAW = getDefineFLOAT("CAMERA_MIN_YAW");
	m_fCAMERA_MAX_YAW = getDefineFLOAT("CAMERA_MAX_YAW");
	m_fCAMERA_FAR_CLIP_Z_HEIGHT = getDefineFLOAT("CAMERA_FAR_CLIP_Z_HEIGHT");
	m_fCAMERA_MAX_TRAVEL_DISTANCE = getDefineFLOAT("CAMERA_MAX_TRAVEL_DISTANCE");
	m_fCAMERA_START_DISTANCE = getDefineFLOAT("CAMERA_START_DISTANCE");
	m_fPLOT_SIZE = getDefineFLOAT("PLOT_SIZE");
	m_fCAMERA_SPECIAL_PITCH = getDefineFLOAT("CAMERA_SPECIAL_PITCH");
	m_fCAMERA_MAX_TURN_OFFSET = getDefineFLOAT("CAMERA_MAX_TURN_OFFSET");
	m_fCAMERA_MIN_DISTANCE = getDefineFLOAT("CAMERA_MIN_DISTANCE");
	m_fCAMERA_UPPER_PITCH = getDefineFLOAT("CAMERA_UPPER_PITCH");
	m_fCAMERA_LOWER_PITCH = getDefineFLOAT("CAMERA_LOWER_PITCH");
	m_fFIELD_OF_VIEW = getDefineFLOAT("FIELD_OF_VIEW");
	m_fUNIT_MULTISELECT_DISTANCE = getDefineFLOAT("UNIT_MULTISELECT_DISTANCE");

	m_iUSE_CAN_FOUND_CITIES_ON_WATER_CALLBACK = getDefineINT("USE_CAN_FOUND_CITIES_ON_WATER_CALLBACK");
	m_iUSE_CANNOT_DO_CIVIC_CALLBACK = getDefineINT("USE_CANNOT_DO_CIVIC_CALLBACK");
	m_iUSE_CAN_DO_CIVIC_CALLBACK = getDefineINT("USE_CAN_DO_CIVIC_CALLBACK");
	m_iUSE_CANNOT_CONSTRUCT_CALLBACK = getDefineINT("USE_CANNOT_CONSTRUCT_CALLBACK");
	m_iUSE_CAN_CONSTRUCT_CALLBACK = getDefineINT("USE_CAN_CONSTRUCT_CALLBACK");
	m_iUSE_CAN_DECLARE_WAR_CALLBACK = getDefineINT("USE_CAN_DECLARE_WAR_CALLBACK");
	m_iUSE_GET_UNIT_COST_MOD_CALLBACK = getDefineINT("USE_GET_UNIT_COST_MOD_CALLBACK");
	m_iUSE_GET_BUILDING_COST_MOD_CALLBACK = getDefineINT("USE_GET_BUILDING_COST_MOD_CALLBACK");
	m_iUSE_GET_CITY_FOUND_VALUE_CALLBACK = getDefineINT("USE_GET_CITY_FOUND_VALUE_CALLBACK");
	m_iUSE_CANNOT_HANDLE_ACTION_CALLBACK = getDefineINT("USE_CANNOT_HANDLE_ACTION_CALLBACK");
	m_iUSE_CAN_BUILD_CALLBACK = getDefineINT("USE_CAN_BUILD_CALLBACK");
	m_iUSE_CANNOT_TRAIN_CALLBACK = getDefineINT("USE_CANNOT_TRAIN_CALLBACK");
	m_iUSE_CAN_TRAIN_CALLBACK = getDefineINT("USE_CAN_TRAIN_CALLBACK");
	m_iUSE_UNIT_CANNOT_MOVE_INTO_CALLBACK = getDefineINT("USE_UNIT_CANNOT_MOVE_INTO_CALLBACK");
	m_iUSE_FINISH_TEXT_CALLBACK = getDefineINT("USE_FINISH_TEXT_CALLBACK");
	m_iUSE_ON_UNIT_SET_XY_CALLBACK = getDefineINT("USE_ON_UNIT_SET_XY_CALLBACK");
	m_iUSE_ON_UNIT_SELECTED_CALLBACK = getDefineINT("USE_ON_UNIT_SELECTED_CALLBACK");
	m_iUSE_ON_MISSIONARY_CONVERTED_UNIT_CALLBACK = getDefineINT("USE_ON_MISSIONARY_CONVERTED_UNIT_CALLBACK");
	m_iUSE_ON_UPDATE_CALLBACK = getDefineINT("USE_ON_UPDATE_CALLBACK");
	m_iUSE_ON_UNIT_CREATED_CALLBACK = getDefineINT("USE_ON_UNIT_CREATED_CALLBACK");
	m_iUSE_ON_UNIT_LOST_CALLBACK = getDefineINT("USE_ON_UNIT_LOST_CALLBACK");
}

// cache XML - start - Nightinggale
int CvGlobals::getXMLuncached(XMLconstantTypes eVal) const
{
	switch (eVal)
	{
		// cache script hook is just what it sounds like: hooks for the cache script
		// everything between start and end will be replaced/updated when running getXMLval.pl
		// XMLconstantTypes from CvEnums.h is used as source to generate this switch-case

		/// cache script hook start
		case XML_UNITARMOR_PLATE:
			return this->getDefineINT("UNITARMOR_PLATE");
			break;
		case XML_UNITARMOR_SHIELD:
			return this->getDefineINT("UNITARMOR_SHIELD");
			break;
		case XML_UNITTACTIC_PARRY:
			return this->getDefineINT("UNITTACTIC_PARRY");
			break;
		case XML_UNITWEAPON_BLUNT:
			return this->getDefineINT("UNITWEAPON_BLUNT");
			break;
		case XML_EUROPE_EAST:
			return this->getDefineINT("TRADE_SCREEN_MOTHERLAND");
			break;
		case XML_AI_ADVANCED_TECH_START:
			return this->getDefineINT("AI_ADVANCED_TECH_START");
			break;
		case XML_AI_CHEAT_AUTO_BUY:
			return this->getDefineINT("AI_CHEAT_AUTO_BUY");
			break;
		case XML_AI_CHEAT_NEEDED_DEFENDERS:
			return this->getDefineINT("AI_CHEAT_NEEDED_DEFENDERS");
			break;
		case XML_AI_CHEAT_SPICE:
			return this->getDefineINT("AI_CHEAT_SPICE");
			break;
		case XML_AI_CHEAT_SPICE_BUY:
			return this->getDefineINT("AI_CHEAT_SPICE_BUY");
			break;
		case XML_AI_CHEAT_SPICE_MULTIPLE:
			return this->getDefineINT("AI_CHEAT_SPICE_MULTIPLE");
			break;
		case XML_AI_FAVORED_TERRAIN_MOD:
			return this->getDefineINT("AI_FAVORED_TERRAIN_MOD");
			break;
		case XML_AI_HANDICAP_CASTLE:
			return this->getDefineINT("AI_HANDICAP_CASTLE");
			break;
		case XML_AI_HANDICAP_MOTTE:
			return this->getDefineINT("AI_HANDICAP_MOTTE");
			break;
		case XML_AI_MILITARY_PROFESSION_HACK:
			return this->getDefineINT("AI_MILITARY_PROFESSION_HACK");
			break;
		case XML_AI_WAREHOUSE_MOD:
			return this->getDefineINT("AI_WAREHOUSE_MOD");
			break;
		case XML_ALLIANCE_CULTURE_PERCENT_DENIAL:
			return this->getDefineINT("ALLIANCE_CULTURE_PERCENT_DENIAL");
			break;
		case XML_ALLIANCE_PACT_PEACE_DENIAL:
			return this->getDefineINT("ALLIANCE_PACT_PEACE_DENIAL");
			break;
		case XML_ANIMAL_BANDITS_GUARD_GOODY_TIMER:
			return this->getDefineINT("ANIMAL_BANDITS_GUARD_GOODY_TIMER");
			break;
		case XML_AUTORESEARCH_ALL:
			return this->getDefineINT("AUTORESEARCH_ALL");
			break;
		case XML_BANQUET_YIELD_AMOUNT:
			return this->getDefineINT("BANQUET_YIELD_AMOUNT");
			break;
		case XML_BARBARIAN_CIVILIZATION:
			return this->getDefineINT("BARBARIAN_CIVILIZATION");
			break;
		case XML_BARBARIAN_LEADER:
			return this->getDefineINT("BARBARIAN_LEADER");
			break;
		case XML_BASE_CITY_GROWTH_THRESHOLD:
			return this->getDefineINT("BASE_CITY_GROWTH_THRESHOLD");
			break;
		case XML_BASE_CITY_LUXURY_FOOD_THRESHOLD_MOD:
			return this->getDefineINT("BASE_CITY_LUXURY_FOOD_THRESHOLD_MOD");
			break;
		case XML_BUILDINGCLASS_TRAVEL_TO_FAIR:
			return this->getDefineINT("BUILDINGCLASS_TRAVEL_TO_FAIR");
			break;
		case XML_CALENDER_YEAR_FIRST_CHANGE_TURN:
			return this->getDefineINT("CALENDER_YEAR_FIRST_CHANGE_TURN");
			break;
		case XML_CALENDER_YEAR_FIRST_MULTIPLIER:
			return this->getDefineINT("CALENDER_YEAR_FIRST_MULTIPLIER");
			break;
		case XML_CALENDER_YEAR_SECOND_CHANGE_TURN:
			return this->getDefineINT("CALENDER_YEAR_SECOND_CHANGE_TURN");
			break;
		case XML_CALENDER_YEAR_SECOND_MULTIPLIER:
			return this->getDefineINT("CALENDER_YEAR_SECOND_MULTIPLIER");
			break;
		case XML_CALENDER_YEAR_THIRD_MULTIPLIER:
			return this->getDefineINT("CALENDER_YEAR_THIRD_MULTIPLIER");
			break;
		case XML_CAPTURED_CARGO_RANDOM_AMOUNT:
			return this->getDefineINT("CAPTURED_CARGO_RANDOM_AMOUNT");
			break;
		case XML_CAPTURED_LUXURY_FOOD_RANDOM_AMOUNT:
			return this->getDefineINT("CAPTURED_LUXURY_FOOD_RANDOM_AMOUNT");
			break;
		case XML_CASTLES_TO_VILLAGES_RATIO:
			return this->getDefineINT("CASTLES_TO_VILLAGES_RATIO");
			break;
		case XML_CHANCE_TO_CAPTURE_CRIMINALS:
			return this->getDefineINT("CHANCE_TO_CAPTURE_CRIMINALS");
			break;
		case XML_CHEAT_TRAVEL_ALL:
			return this->getDefineINT("CHEAT_TRAVEL_ALL");
			break;
		case XML_CITY_PILGRAM_RANDOM:
			return this->getDefineINT("CITY_PILGRAM_RANDOM");
			break;
		case XML_COLONIAL_FORCED_PEACE_TURNS:
			return this->getDefineINT("COLONIAL_FORCED_PEACE_TURNS");
			break;
		case XML_CONTACT_YIELD_GIFT_ENCOMEIDA_TIMER:
			return this->getDefineINT("CONTACT_YIELD_GIFT_ENCOMEIDA_TIMER");
			break;
		case XML_CONTACT_YIELD_GIFT_TECH:
			return this->getDefineINT("CONTACT_YIELD_GIFT_TECH");
			break;
		case XML_DEAL_TRADE_RELATIONS_POINTS:
			return this->getDefineINT("DEAL_TRADE_RELATIONS_POINTS");
			break;
		case XML_DEFAULT_CENSURETYPE_ANATHEMA:
			return this->getDefineINT("DEFAULT_CENSURETYPE_ANATHEMA");
			break;
		case XML_DEFAULT_CENSURETYPE_ANATHEMA_COUNT:
			return this->getDefineINT("DEFAULT_CENSURETYPE_ANATHEMA_COUNT");
			break;
		case XML_DEFAULT_CENSURETYPE_EXCOMMUNICATION:
			return this->getDefineINT("DEFAULT_CENSURETYPE_EXCOMMUNICATION");
			break;
		case XML_DEFAULT_CENSURETYPE_EXCOMMUNICATION_COUNT:
			return this->getDefineINT("DEFAULT_CENSURETYPE_EXCOMMUNICATION_COUNT");
			break;
		case XML_DEFAULT_CENSURETYPE_INTERDICT:
			return this->getDefineINT("DEFAULT_CENSURETYPE_INTERDICT");
			break;
		case XML_DEFAULT_CENSURETYPE_INTERDICT_COUNT:
			return this->getDefineINT("DEFAULT_CENSURETYPE_INTERDICT_COUNT");
			break;
		case XML_DEFAULT_COMBAT_FOR_TRAINING:
			return this->getDefineINT("DEFAULT_COMBAT_FOR_TRAINING");
			break;
		case XML_DEFAULT_DAWN_POPULATION_UNIT:
			return this->getDefineINT("DEFAULT_DAWN_POPULATION_UNIT");
			break;
		case XML_DEFAULT_FUEDALISM_TECH:
			return this->getDefineINT("DEFAULT_FUEDALISM_TECH");
			break;
		case XML_DEFAULT_GRAIN_GROWTH_UNIT_CLASS:
			return this->getDefineINT("DEFAULT_GRAIN_GROWTH_UNIT_CLASS");
			break;
		case XML_DEFAULT_HUNTSMAN_PROFESSION:
			return this->getDefineINT("DEFAULT_HUNTSMAN_PROFESSION");
			break;
		case XML_DEFAULT_INVENTOR_CLASS:
			return this->getDefineINT("DEFAULT_INVENTOR_CLASS");
			break;
		case XML_DEFAULT_KNIGHT_PROFESSION_PROMOTION:
			return this->getDefineINT("DEFAULT_KNIGHT_PROFESSION_PROMOTION");
			break;
		case XML_DEFAULT_KNIGHT_PROMOTION:
			return this->getDefineINT("DEFAULT_KNIGHT_PROMOTION");
			break;
		case XML_DEFAULT_MARAUDER_CLASS:
			return this->getDefineINT("DEFAULT_MARAUDER_CLASS");
			break;
		case XML_DEFAULT_MARUADER_SEA_PROFESSION:
			return this->getDefineINT("DEFAULT_MARUADER_SEA_PROFESSION");
			break;
		case XML_DEFAULT_NATIVE_TRADE_PROFESSION:
			return this->getDefineINT("DEFAULT_NATIVE_TRADE_PROFESSION");
			break;
		case XML_DEFAULT_NOBLEMAN_CLASS:
			return this->getDefineINT("DEFAULT_NOBLEMAN_CLASS");
			break;
		case XML_DEFAULT_NOBLE_GROWTH_UNIT_CLASS:
			return this->getDefineINT("DEFAULT_NOBLE_GROWTH_UNIT_CLASS");
			break;
		case XML_DEFAULT_PILGRAM_CLASS:
			return this->getDefineINT("DEFAULT_PILGRAM_CLASS");
			break;
		case XML_DEFAULT_SHRINE_CLASS:
			return this->getDefineINT("DEFAULT_SHRINE_CLASS");
			break;
		case XML_DEFAULT_SLAVE_CLASS:
			return this->getDefineINT("DEFAULT_SLAVE_CLASS");
			break;
		case XML_DEFAULT_SPECIALBUILDING_COURTHOUSE:
			return this->getDefineINT("DEFAULT_SPECIALBUILDING_COURTHOUSE");
			break;
		case XML_DEFAULT_TEST_DEFINE:
			return this->getDefineINT("DEFAULT_TEST_DEFINE");
			break;
		case XML_DEFAULT_TRAINED_PROMOTION:
			return this->getDefineINT("DEFAULT_TRAINED_PROMOTION");
			break;
		case XML_DEFAULT_TREASURE_YIELD:
			return this->getDefineINT("DEFAULT_TREASURE_YIELD");
			break;
		case XML_DEFAULT_UNTRAINED_PROMOTION:
			return this->getDefineINT("DEFAULT_UNTRAINED_PROMOTION");
			break;
		case XML_DEFAULT_VIKING_ERA:
			return this->getDefineINT("DEFAULT_VIKING_ERA");
			break;
		case XML_DIPLAY_NEW_VIDEOS:
			return this->getDefineINT("DIPLAY_NEW_VIDEOS");
			break;
		case XML_DOCKS_NEXT_UNITS:
			return this->getDefineINT("DOCKS_NEXT_UNITS");
			break;
		case XML_EDUCATION_THRESHOLD:
			return this->getDefineINT("EDUCATION_THRESHOLD");
			break;
		case XML_EDUCATION_THRESHOLD_INCREASE:
			return this->getDefineINT("EDUCATION_THRESHOLD_INCREASE");
			break;
		case XML_ENABLE_MAP_TRADING:
			return this->getDefineINT("ENABLE_MAP_TRADING");
			break;
		case XML_ESTABLISH_TRADEPOST_COST:
			return this->getDefineINT("ESTABLISH_TRADEPOST_COST");
			break;
		case XML_FATHER_POINT_REAL_TRADE:
			return this->getDefineINT("FATHER_POINT_REAL_TRADE");
			break;
		case XML_FOOD_CONSUMPTION_PER_POPULATION:
			return this->getDefineINT("FOOD_CONSUMPTION_PER_POPULATION");
			break;
		case XML_FOUND_MONASTERY_NUMBER:
			return this->getDefineINT("FOUND_MONASTERY_NUMBER");
			break;
		case XML_FOUND_MOTTE_AND_BAILEY_NUMBER:
			return this->getDefineINT("FOUND_MOTTE_AND_BAILEY_NUMBER");
			break;
		case XML_FOUND_OUTPOST_NUMBER:
			return this->getDefineINT("FOUND_OUTPOST_NUMBER");
			break;
		case XML_FOUND_VILLAGE_NUMBER:
			return this->getDefineINT("FOUND_VILLAGE_NUMBER");
			break;
		case XML_FREE_PEASANT_CIVIC:
			return this->getDefineINT("FREE_PEASANT_CIVIC");
			break;
		case XML_HIRE_GUARD_COST:
			return this->getDefineINT("HIRE_GUARD_COST");
			break;
		case XML_HIRE_GUARD_PROMOTION:
			return this->getDefineINT("HIRE_GUARD_PROMOTION");
			break;
		case XML_IMMIGRATION_MAX_CITY_DISTANCE:
			return this->getDefineINT("IMMIGRATION_MAX_CITY_DISTANCE");
			break;
		case XML_IMMIGRATION_THRESHOLD:
			return this->getDefineINT("IMMIGRATION_THRESHOLD");
			break;
		case XML_IMMIGRATION_THRESHOLD_INCREASE:
			return this->getDefineINT("IMMIGRATION_THRESHOLD_INCREASE");
			break;
		case XML_IMMIGRATION_TRAVEL_MOD:
			return this->getDefineINT("IMMIGRATION_TRAVEL_MOD");
			break;
		case XML_INDUSTRIAL_VICTORY_SINGLE_YIELD:
			return this->getDefineINT("INDUSTRIAL_VICTORY_SINGLE_YIELD");
			break;
		case XML_JUNGLE_FEATURE:
			return this->getDefineINT("JUNGLE_FEATURE");
			break;
		case XML_KNIGHT_RANSOM_MOD:
			return this->getDefineINT("KNIGHT_RANSOM_MOD");
			break;
		case XML_MARAUDERS_DAYSOUT_RANDOM:
			return this->getDefineINT("MARAUDERS_DAYSOUT_RANDOM");
			break;
		case XML_MARAUDERS_TOWER_RANGE:
			return this->getDefineINT("MARAUDERS_TOWER_RANGE");
			break;
		case XML_MARAUDER_CRUMBS_EVENT:
			return this->getDefineINT("MARAUDER_CRUMBS_EVENT");
			break;
		case XML_MARAUDER_EVENT_DEFAULT_TURNS:
			return this->getDefineINT("MARAUDER_EVENT_DEFAULT_TURNS");
			break;
		case XML_MAX_CITY_POPULATION_COMMUNE:
			return this->getDefineINT("MAX_CITY_POPULATION_COMMUNE");
			break;
		case XML_MAX_CITY_POPULATION_VILLAGE:
			return this->getDefineINT("MAX_CITY_POPULATION_VILLAGE");
			break;
		case XML_MAX_INSULT_MODIFIER:
			return this->getDefineINT("MAX_INSULT_MODIFIER");
			break;
		case XML_MAX_LEVEL_FROM_ANIMAL_XP:
			return this->getDefineINT("MAX_LEVEL_FROM_ANIMAL_XP");
			break;
		case XML_MAX_MARAUDERS:
			return this->getDefineINT("MAX_MARAUDERS");
			break;
		case XML_MAX_TRAINED_EXPERIENCE:
			return this->getDefineINT("MAX_TRAINED_EXPERIENCE");
			break;
		case XML_MEDIEVAL_CENSURE:
			return this->getDefineINT("MEDIEVAL_CENSURE");
			break;
		case XML_MEDIEVAL_TRADE_TECH:
			return this->getDefineINT("MEDIEVAL_TRADE_TECH");
			break;
		case XML_MONASTERIES_TO_VILLAGES_RATIO:
			return this->getDefineINT("MONASTERIES_TO_VILLAGES_RATIO");
			break;
		case XML_NATIVE_AUTO_SELL_PERCENT:
			return this->getDefineINT("NATIVE_AUTO_SELL_PERCENT");
			break;
		case XML_NATIVE_HORSES_FOR_SALE:
			return this->getDefineINT("NATIVE_HORSES_FOR_SALE");
			break;
		case XML_NATIVE_HORSES_FOR_SALE_PERCENT:
			return this->getDefineINT("NATIVE_HORSES_FOR_SALE_PERCENT");
			break;
		case XML_NATIVE_TECH:
			return this->getDefineINT("NATIVE_TECH");
			break;
		case XML_NATIVE_TRADING_TRADEPOST:
			return this->getDefineINT("NATIVE_TRADING_TRADEPOST");
			break;
		case XML_NOBLE_FIELD_LABOR_PENALTY:
			return this->getDefineINT("NOBLE_FIELD_LABOR_PENALTY");
			break;
		case XML_NO_CITY_SHORTAGE_MESSAGES:
			return this->getDefineINT("NO_CITY_SHORTAGE_MESSAGES");
			break;
		case XML_NO_MARKED_SALES_CAP:
			return this->getDefineINT("NO_MARKED_SALES_CAP");
			break;
		case XML_NO_STARTING_PLOTS_IN_JUNGLE:
			return this->getDefineINT("NO_STARTING_PLOTS_IN_JUNGLE");
			break;
		case XML_PERCENT_TAX_ON_AUTOSELL_GOODS:
			return this->getDefineINT("PERCENT_TAX_ON_AUTOSELL_GOODS");
			break;
		case XML_PILGRAM_OFFER_GOLD:
			return this->getDefineINT("PILGRAM_OFFER_GOLD");
			break;
		case XML_PILGRAM_OFFER_GOLD_DISTANCE_MOD:
			return this->getDefineINT("PILGRAM_OFFER_GOLD_DISTANCE_MOD");
			break;
		case XML_PROFESSION_INVENTOR:
			return this->getDefineINT("PROFESSION_INVENTOR");
			break;
		case XML_PROLIFIC_INVENTOR_THRESHOLD_INCREASE:
			return this->getDefineINT("PROLIFIC_INVENTOR_THRESHOLD_INCREASE");
			break;
		case XML_PROMOTION_BUILD_HOME:
			return this->getDefineINT("PROMOTION_BUILD_HOME");
			break;
		case XML_RANDOM_TURNS_LOST_AT_SEA:
			return this->getDefineINT("RANDOM_TURNS_LOST_AT_SEA");
			break;
		case XML_SLAVE_FOOD_CONSUMPTION_PER_POPULATION:
			return this->getDefineINT("SLAVE_FOOD_CONSUMPTION_PER_POPULATION");
			break;
		case XML_START_YEAR:
			return this->getDefineINT("START_YEAR");
			break;
		case XML_TK_AI_RESEARCH_COST_MOD_PERCENT:
			return this->getDefineINT("TK_AI_RESEARCH_COST_MOD_PERCENT");
			break;
		case XML_TK_HUMAN_RESEARCH_COST_MOD_PERCENT:
			return this->getDefineINT("TK_HUMAN_RESEARCH_COST_MOD_PERCENT");
			break;
		case XML_TK_IDEAS_CITY_VALUE:
			return this->getDefineINT("TK_IDEAS_CITY_VALUE");
			break;
		case XML_TK_PROLIFIC_INVENTOR_PROGRESS:
			return this->getDefineINT("TK_PROLIFIC_INVENTOR_PROGRESS");
			break;
		case XML_TK_PROLIFIC_INVENTOR_THRESHOLD:
			return this->getDefineINT("TK_PROLIFIC_INVENTOR_THRESHOLD");
			break;
		case XML_TK_RESEARCH_PACT_IDEAS_THRESHOLD:
			return this->getDefineINT("TK_RESEARCH_PACT_IDEAS_THRESHOLD");
			break;
		case XML_TK_RESEARCH_TRADE_VALUE:
			return this->getDefineINT("TK_RESEARCH_TRADE_VALUE");
			break;
		case XML_TRADE_POINTS_FOR_ROUTE:
			return this->getDefineINT("TRADE_POINTS_FOR_ROUTE");
			break;
		case XML_TRADE_ROUTE_SPICE:
			return this->getDefineINT("TRADE_ROUTE_SPICE");
			break;
		case XML_TRADE_STIMULATES_RESEARCH_MIN_VALUE:
			return this->getDefineINT("TRADE_STIMULATES_RESEARCH_MIN_VALUE");
			break;
		case XML_TRADE_STIMULATES_RESEARCH_PERCENT:
			return this->getDefineINT("TRADE_STIMULATES_RESEARCH_PERCENT");
			break;
		case XML_TRADING_POINTS_MOD_PERCENT:
			return this->getDefineINT("TRADING_POINTS_MOD_PERCENT");
			break;
		case XML_TREASURE_UNITS_ONLY_SELECT_COASTAL:
			return this->getDefineINT("TREASURE_UNITS_ONLY_SELECT_COASTAL");
			break;
		case XML_TURNS_TO_TRAIN:
			return this->getDefineINT("TURNS_TO_TRAIN");
			break;
		case XML_UNITCOMBAT_USE_ALL_BONUS:
			return this->getDefineINT("UNITCOMBAT_USE_ALL_BONUS");
			break;
		case XML_USE_MEDIEVAL_CALENDER:
			return this->getDefineINT("USE_MEDIEVAL_CALENDER");
			break;
		case XML_VASSAL_CIVILIZATION:
			return this->getDefineINT("VASSAL_CIVILIZATION");
			break;
		case XML_VASSAL_LEADER:
			return this->getDefineINT("VASSAL_LEADER");
			break;
		case XML_WEEKS_PER_MONTHS:
			return this->getDefineINT("WEEKS_PER_MONTHS");
			break;
		case XML_WILD_ANIMAL_LAND_BONUS_NATIVE_WEIGHT:
			return this->getDefineINT("WILD_ANIMAL_LAND_BONUS_NATIVE_WEIGHT");
			break;
		case XML_WILD_ANIMAL_LAND_FEATURE_NATIVE_WEIGHT:
			return this->getDefineINT("WILD_ANIMAL_LAND_FEATURE_NATIVE_WEIGHT");
			break;
		case XML_WILD_ANIMAL_LAND_TERRAIN_NATIVE_WEIGHT:
			return this->getDefineINT("WILD_ANIMAL_LAND_TERRAIN_NATIVE_WEIGHT");
			break;
		case XML_WILD_ANIMAL_LAND_UNIT_VARIATION_WEIGHT:
			return this->getDefineINT("WILD_ANIMAL_LAND_UNIT_VARIATION_WEIGHT");
			break;
		case XML_WILD_ANIMAL_SEA_BONUS_NATIVE_WEIGHT:
			return this->getDefineINT("WILD_ANIMAL_SEA_BONUS_NATIVE_WEIGHT");
			break;
		case XML_WILD_ANIMAL_SEA_FEATURE_NATIVE_WEIGHT:
			return this->getDefineINT("WILD_ANIMAL_SEA_FEATURE_NATIVE_WEIGHT");
			break;
		case XML_WILD_ANIMAL_SEA_TERRAIN_NATIVE_WEIGHT:
			return this->getDefineINT("WILD_ANIMAL_SEA_TERRAIN_NATIVE_WEIGHT");
			break;
		case XML_WILD_ANIMAL_SEA_UNIT_VARIATION_WEIGHT:
			return this->getDefineINT("WILD_ANIMAL_SEA_UNIT_VARIATION_WEIGHT");
			break;
		case XML_ADVANCED_START_ALLOW_UNITS_OUTSIDE_CITIES:
			return this->getDefineINT("ADVANCED_START_ALLOW_UNITS_OUTSIDE_CITIES");
			break;
		case XML_ADVANCED_START_CITY_COST:
			return this->getDefineINT("ADVANCED_START_CITY_COST");
			break;
		case XML_ADVANCED_START_CITY_COST_INCREASE:
			return this->getDefineINT("ADVANCED_START_CITY_COST_INCREASE");
			break;
		case XML_ADVANCED_START_CITY_PLACEMENT_MAX_RANGE:
			return this->getDefineINT("ADVANCED_START_CITY_PLACEMENT_MAX_RANGE");
			break;
		case XML_ADVANCED_START_CULTURE_COST:
			return this->getDefineINT("ADVANCED_START_CULTURE_COST");
			break;
		case XML_ADVANCED_START_MAX_UNITS_PER_CITY:
			return this->getDefineINT("ADVANCED_START_MAX_UNITS_PER_CITY");
			break;
		case XML_ADVANCED_START_POPULATION_COST:
			return this->getDefineINT("ADVANCED_START_POPULATION_COST");
			break;
		case XML_ADVANCED_START_POPULATION_COST_INCREASE:
			return this->getDefineINT("ADVANCED_START_POPULATION_COST_INCREASE");
			break;
		case XML_ADVANCED_START_SIGHT_RANGE:
			return this->getDefineINT("ADVANCED_START_SIGHT_RANGE");
			break;
		case XML_ADVANCED_START_VISIBILITY_COST:
			return this->getDefineINT("ADVANCED_START_VISIBILITY_COST");
			break;
		case XML_ADVANCED_START_VISIBILITY_COST_INCREASE:
			return this->getDefineINT("ADVANCED_START_VISIBILITY_COST_INCREASE");
			break;
		case XML_AI_CAN_DISBAND_UNITS:
			return this->getDefineINT("AI_CAN_DISBAND_UNITS");
			break;
		case XML_AMPHIB_ATTACK_MODIFIER:
			return this->getDefineINT("AMPHIB_ATTACK_MODIFIER");
			break;
		case XML_BASE_OCCUPATION_TURNS:
			return this->getDefineINT("BASE_OCCUPATION_TURNS");
			break;
		case XML_BASE_UNIT_UPGRADE_COST:
			return this->getDefineINT("BASE_UNIT_UPGRADE_COST");
			break;
		case XML_BUILDING_PRODUCTION_DECAY_PERCENT:
			return this->getDefineINT("BUILDING_PRODUCTION_DECAY_PERCENT");
			break;
		case XML_BUILDING_PRODUCTION_DECAY_TIME:
			return this->getDefineINT("BUILDING_PRODUCTION_DECAY_TIME");
			break;
		case XML_BUILDING_PRODUCTION_PERCENT:
			return this->getDefineINT("BUILDING_PRODUCTION_PERCENT");
			break;
		case XML_BUY_PLOT_BASE_CULTURE_COST:
			return this->getDefineINT("BUY_PLOT_BASE_CULTURE_COST");
			break;
		case XML_BUY_PLOT_CULTURE_RANGE:
			return this->getDefineINT("BUY_PLOT_CULTURE_RANGE");
			break;
		case XML_BUY_PLOT_MIN_CULTURE:
			return this->getDefineINT("BUY_PLOT_MIN_CULTURE");
			break;
		case XML_BUY_PLOT_OWNED_COST_MODIFIER:
			return this->getDefineINT("BUY_PLOT_OWNED_COST_MODIFIER");
			break;
		case XML_BUY_PLOT_SELLER_INCOME_PERCENT:
			return this->getDefineINT("BUY_PLOT_SELLER_INCOME_PERCENT");
			break;
		case XML_CAPITAL_BUILDINGCLASS:
			return this->getDefineINT("CAPITAL_BUILDINGCLASS");
			break;
		case XML_CITY_DEFENSE_DAMAGE_HEAL_RATE:
			return this->getDefineINT("CITY_DEFENSE_DAMAGE_HEAL_RATE");
			break;
		case XML_CITY_FREE_CULTURE_GROWTH_FACTOR:
			return this->getDefineINT("CITY_FREE_CULTURE_GROWTH_FACTOR");
			break;
		case XML_CITY_GROWTH_MULTIPLIER:
			return this->getDefineINT("CITY_GROWTH_MULTIPLIER");
			break;
		case XML_CITY_HEAL_RATE:
			return this->getDefineINT("CITY_HEAL_RATE");
			break;
		case XML_CITY_SCREEN_FOG_ENABLED:
			return this->getDefineINT("CITY_SCREEN_FOG_ENABLED");
			break;
		case XML_CITY_YIELD_CAPACITY:
			return this->getDefineINT("CITY_YIELD_CAPACITY");
			break;
		case XML_CITY_YIELD_DECAY_PERCENT:
			return this->getDefineINT("CITY_YIELD_DECAY_PERCENT");
			break;
		case XML_COMBAT_DAMAGE:
			return this->getDefineINT("COMBAT_DAMAGE");
			break;
		case XML_COMBAT_DIE_SIDES:
			return this->getDefineINT("COMBAT_DIE_SIDES");
			break;
		case XML_COMBAT_EXPERIENCE_IN_BORDERS_PERCENT:
			return this->getDefineINT("COMBAT_EXPERIENCE_IN_BORDERS_PERCENT");
			break;
		case XML_CONSUME_EQUIPMENT_ON_FOUND:
			return this->getDefineINT("CONSUME_EQUIPMENT_ON_FOUND");
			break;
		case XML_DEEP_WATER_TERRAIN:
			return this->getDefineINT("DEEP_WATER_TERRAIN");
			break;
		case XML_DEFAULT_POPULATION_UNIT:
			return this->getDefineINT("DEFAULT_POPULATION_UNIT");
			break;
		case XML_DIFFERENT_TEAM_FEATURE_PRODUCTION_PERCENT:
			return this->getDefineINT("DIFFERENT_TEAM_FEATURE_PRODUCTION_PERCENT");
			break;
		case XML_DIPLOMACY_VALUE_REMAINDER:
			return this->getDefineINT("DIPLOMACY_VALUE_REMAINDER");
			break;
		case XML_EDUCATION_BASE_TUITION:
			return this->getDefineINT("EDUCATION_BASE_TUITION");
			break;
		case XML_ENABLE_ALLIANCE_TRADING:
			return this->getDefineINT("ENABLE_ALLIANCE_TRADING");
			break;
		case XML_ENABLE_DEFENSIVE_PACT_TRADING:
			return this->getDefineINT("ENABLE_DEFENSIVE_PACT_TRADING");
			break;
		case XML_ENABLE_GOLD_TRADING:
			return this->getDefineINT("ENABLE_GOLD_TRADING");
			break;
		case XML_ENABLE_OPEN_BORDERS:
			return this->getDefineINT("ENABLE_OPEN_BORDERS");
			break;
		case XML_END_GAME_DISPLAY_WARNING:
			return this->getDefineINT("END_GAME_DISPLAY_WARNING");
			break;
		case XML_ENEMY_HEAL_RATE:
			return this->getDefineINT("ENEMY_HEAL_RATE");
			break;
		case XML_EUROPE_MARKET_CORRELATION_PERCENT:
			return this->getDefineINT("EUROPE_MARKET_CORRELATION_PERCENT");
			break;
		case XML_EVENT_PROBABILITY_ROLL_SIDES:
			return this->getDefineINT("EVENT_PROBABILITY_ROLL_SIDES");
			break;
		case XML_EXPERIENCE_FROM_WITHDRAWL:
			return this->getDefineINT("EXPERIENCE_FROM_WITHDRAWL");
			break;
		case XML_EXTRA_YIELD:
			return this->getDefineINT("EXTRA_YIELD");
			break;
		case XML_FATHER_COST_EXTRA_TEAM_MEMBER_MODIFIER:
			return this->getDefineINT("FATHER_COST_EXTRA_TEAM_MEMBER_MODIFIER");
			break;
		case XML_FEATURE_GROWTH_MODIFIER:
			return this->getDefineINT("FEATURE_GROWTH_MODIFIER");
			break;
		case XML_FEATURE_PRODUCTION_YIELD_MAX_DISTANCE:
			return this->getDefineINT("FEATURE_PRODUCTION_YIELD_MAX_DISTANCE");
			break;
		case XML_FIRST_EVENT_DELAY_TURNS:
			return this->getDefineINT("FIRST_EVENT_DELAY_TURNS");
			break;
		case XML_FORTIFY_MODIFIER_PER_TURN:
			return this->getDefineINT("FORTIFY_MODIFIER_PER_TURN");
			break;
		case XML_FREE_CITY_ADJACENT_CULTURE:
			return this->getDefineINT("FREE_CITY_ADJACENT_CULTURE");
			break;
		case XML_FREE_CITY_CULTURE:
			return this->getDefineINT("FREE_CITY_CULTURE");
			break;
		case XML_FRIENDLY_HEAL_RATE:
			return this->getDefineINT("FRIENDLY_HEAL_RATE");
			break;
		case XML_GREAT_GENERALS_THRESHOLD:
			return this->getDefineINT("GREAT_GENERALS_THRESHOLD");
			break;
		case XML_GREAT_GENERALS_THRESHOLD_INCREASE:
			return this->getDefineINT("GREAT_GENERALS_THRESHOLD_INCREASE");
			break;
		case XML_GREAT_GENERALS_THRESHOLD_INCREASE_TEAM:
			return this->getDefineINT("GREAT_GENERALS_THRESHOLD_INCREASE_TEAM");
			break;
		case XML_HILLS_EXTRA_DEFENSE:
			return this->getDefineINT("HILLS_EXTRA_DEFENSE");
			break;
		case XML_HILLS_EXTRA_MOVEMENT:
			return this->getDefineINT("HILLS_EXTRA_MOVEMENT");
			break;
		case XML_HILLS_SEE_FROM_CHANGE:
			return this->getDefineINT("HILLS_SEE_FROM_CHANGE");
			break;
		case XML_HILLS_SEE_THROUGH_CHANGE:
			return this->getDefineINT("HILLS_SEE_THROUGH_CHANGE");
			break;
		case XML_INITIAL_AI_CITY_PRODUCTION:
			return this->getDefineINT("INITIAL_AI_CITY_PRODUCTION");
			break;
		case XML_INITIAL_CITY_POPULATION:
			return this->getDefineINT("INITIAL_CITY_POPULATION");
			break;
		case XML_INITIAL_CITY_ROUTE_TYPE:
			return this->getDefineINT("INITIAL_CITY_ROUTE_TYPE");
			break;
		case XML_KING_BUY_UNIT_PRICE_MODIFIER:
			return this->getDefineINT("KING_BUY_UNIT_PRICE_MODIFIER");
			break;
		case XML_KING_INITIAL_UNIT_INCREASE:
			return this->getDefineINT("KING_INITIAL_UNIT_INCREASE");
			break;
		case XML_KING_TRANSPORT_TREASURE_COMISSION:
			return this->getDefineINT("KING_TRANSPORT_TREASURE_COMISSION");
			break;
		case XML_LAKE_MAX_AREA_SIZE:
			return this->getDefineINT("LAKE_MAX_AREA_SIZE");
			break;
		case XML_LAKE_PLOT_RAND:
			return this->getDefineINT("LAKE_PLOT_RAND");
			break;
		case XML_LAND_IMPROVEMENT:
			return this->getDefineINT("LAND_IMPROVEMENT");
			break;
		case XML_LAND_TERRAIN:
			return this->getDefineINT("LAND_TERRAIN");
			break;
		case XML_MAX_CITY_DEFENSE_DAMAGE:
			return this->getDefineINT("MAX_CITY_DEFENSE_DAMAGE");
			break;
		case XML_MAX_EXPERIENCE_AFTER_UPGRADE:
			return this->getDefineINT("MAX_EXPERIENCE_AFTER_UPGRADE");
			break;
		case XML_MAX_EXPERIENCE_PER_COMBAT:
			return this->getDefineINT("MAX_EXPERIENCE_PER_COMBAT");
			break;
		case XML_MAX_FORTIFY_TURNS:
			return this->getDefineINT("MAX_FORTIFY_TURNS");
			break;
		case XML_MAX_HIT_POINTS:
			return this->getDefineINT("MAX_HIT_POINTS");
			break;
		case XML_MAX_PLOT_LIST_ROWS:
			return this->getDefineINT("MAX_PLOT_LIST_ROWS");
			break;
		case XML_MAX_PLOT_LIST_SIZE:
			return this->getDefineINT("MAX_PLOT_LIST_SIZE");
			break;
		case XML_MAX_REBEL_COMBAT_MULTIPLER:
			return this->getDefineINT("MAX_REBEL_COMBAT_MULTIPLER");
			break;
		case XML_MAX_REBEL_YIELD_MODIFIER:
			return this->getDefineINT("MAX_REBEL_YIELD_MODIFIER");
			break;
		case XML_MAX_WITHDRAWAL_PROBABILITY:
			return this->getDefineINT("MAX_WITHDRAWAL_PROBABILITY");
			break;
		case XML_MIN_CITY_RANGE:
			return this->getDefineINT("MIN_CITY_RANGE");
			break;
		case XML_MIN_CITY_YIELD_DECAY:
			return this->getDefineINT("MIN_CITY_YIELD_DECAY");
			break;
		case XML_MIN_CIV_STARTING_DISTANCE:
			return this->getDefineINT("MIN_CIV_STARTING_DISTANCE");
			break;
		case XML_MIN_EXPERIENCE_PER_COMBAT:
			return this->getDefineINT("MIN_EXPERIENCE_PER_COMBAT");
			break;
		case XML_MIN_TIMER_UNIT_DOUBLE_MOVES:
			return this->getDefineINT("MIN_TIMER_UNIT_DOUBLE_MOVES");
			break;
		case XML_MIN_WATER_SIZE_FOR_OCEAN:
			return this->getDefineINT("MIN_WATER_SIZE_FOR_OCEAN");
			break;
		case XML_MINIMAP_RENDER_SIZE:
			return this->getDefineINT("MINIMAP_RENDER_SIZE");
			break;
		case XML_MISSIONARY_RATE_EFFECT_ON_SUCCESS:
			return this->getDefineINT("MISSIONARY_RATE_EFFECT_ON_SUCCESS");
			break;
		case XML_MISSIONARY_THRESHOLD:
			return this->getDefineINT("MISSIONARY_THRESHOLD");
			break;
		case XML_MISSIONARY_THRESHOLD_INCREASE:
			return this->getDefineINT("MISSIONARY_THRESHOLD_INCREASE");
			break;
		case XML_MOVE_DENOMINATOR:
			return this->getDefineINT("MOVE_DENOMINATOR");
			break;
		case XML_NATIVE_GOODS_RAID_PERCENT:
			return this->getDefineINT("NATIVE_GOODS_RAID_PERCENT");
			break;
		case XML_NATIVE_GROWTH_THRESHOLD_MULTIPLIER:
			return this->getDefineINT("NATIVE_GROWTH_THRESHOLD_MULTIPLIER");
			break;
		case XML_NATIVE_TEACH_THRESHOLD_INCREASE:
			return this->getDefineINT("NATIVE_TEACH_THRESHOLD_INCREASE");
			break;
		case XML_NEUTRAL_HEAL_RATE:
			return this->getDefineINT("NEUTRAL_HEAL_RATE");
			break;
		case XML_NEW_CITY_BUILDING_VALUE_MODIFIER:
			return this->getDefineINT("NEW_CITY_BUILDING_VALUE_MODIFIER");
			break;
		case XML_NEW_HURRY_MODIFIER:
			return this->getDefineINT("NEW_HURRY_MODIFIER");
			break;
		case XML_NO_BAD_GOODIES_EXPERIENCE_PERCENT:
			return this->getDefineINT("NO_BAD_GOODIES_EXPERIENCE_PERCENT");
			break;
		case XML_NO_BAD_GOODIES_GOLD_PERCENT:
			return this->getDefineINT("NO_BAD_GOODIES_GOLD_PERCENT");
			break;
		case XML_OCCUPATION_CULTURE_PERCENT_THRESHOLD:
			return this->getDefineINT("OCCUPATION_CULTURE_PERCENT_THRESHOLD");
			break;
		case XML_OCCUPATION_TURNS_POPULATION_PERCENT:
			return this->getDefineINT("OCCUPATION_TURNS_POPULATION_PERCENT");
			break;
		case XML_OWNERSHIP_SCORE_DURATION_THRESHOLD:
			return this->getDefineINT("OWNERSHIP_SCORE_DURATION_THRESHOLD");
			break;
		case XML_OWN_TEAM_STARTING_MODIFIER:
			return this->getDefineINT("OWN_TEAM_STARTING_MODIFIER");
			break;
		case XML_PEACE_TREATY_LENGTH:
			return this->getDefineINT("PEACE_TREATY_LENGTH");
			break;
		case XML_PEAK_EXTRA_MOVEMENT:
			return this->getDefineINT("PEAK_EXTRA_MOVEMENT");
			break;
		case XML_PEAK_SEE_FROM_CHANGE:
			return this->getDefineINT("PEAK_SEE_FROM_CHANGE");
			break;
		case XML_PEAK_SEE_THROUGH_CHANGE:
			return this->getDefineINT("PEAK_SEE_THROUGH_CHANGE");
			break;
		case XML_PLAYER_ALWAYS_RAZES_CITIES:
			return this->getDefineINT("PLAYER_ALWAYS_RAZES_CITIES");
			break;
		case XML_PLOTS_PER_RIVER_EDGE:
			return this->getDefineINT("PLOTS_PER_RIVER_EDGE");
			break;
		case XML_PLOT_VISIBILITY_RANGE:
			return this->getDefineINT("PLOT_VISIBILITY_RANGE");
			break;
		case XML_RAZING_CULTURAL_PERCENT_THRESHOLD:
			return this->getDefineINT("RAZING_CULTURAL_PERCENT_THRESHOLD");
			break;
		case XML_REBEL_PERCENT_FOR_REVOLUTION:
			return this->getDefineINT("REBEL_PERCENT_FOR_REVOLUTION");
			break;
		case XML_REBEL_SENTIMENT_BELLS_FACTOR:
			return this->getDefineINT("REBEL_SENTIMENT_BELLS_FACTOR");
			break;
		case XML_REBEL_SENTIMENT_TURN_WEIGHT:
			return this->getDefineINT("REBEL_SENTIMENT_TURN_WEIGHT");
			break;
		case XML_REVOLUTION_EUROPE_UNIT_SHIP_MODIFIER:
			return this->getDefineINT("REVOLUTION_EUROPE_UNIT_SHIP_MODIFIER");
			break;
		case XML_REVOLUTION_EUROPE_UNIT_THRESHOLD:
			return this->getDefineINT("REVOLUTION_EUROPE_UNIT_THRESHOLD");
			break;
		case XML_REVOLUTION_EUROPE_UNIT_THRESHOLD_INCREASE:
			return this->getDefineINT("REVOLUTION_EUROPE_UNIT_THRESHOLD_INCREASE");
			break;
		case XML_RIVAL_TEAM_STARTING_MODIFIER:
			return this->getDefineINT("RIVAL_TEAM_STARTING_MODIFIER");
			break;
		case XML_RIVER_ATTACK_MODIFIER:
			return this->getDefineINT("RIVER_ATTACK_MODIFIER");
			break;
		case XML_RIVER_SOURCE_MIN_RIVER_RANGE:
			return this->getDefineINT("RIVER_SOURCE_MIN_RIVER_RANGE");
			break;
		case XML_RIVER_SOURCE_MIN_SEAWATER_RANGE:
			return this->getDefineINT("RIVER_SOURCE_MIN_SEAWATER_RANGE");
			break;
		case XML_ROUTE_FEATURE_GROWTH_MODIFIER:
			return this->getDefineINT("ROUTE_FEATURE_GROWTH_MODIFIER");
			break;
		case XML_RUINS_IMPROVEMENT:
			return this->getDefineINT("RUINS_IMPROVEMENT");
			break;
		case XML_SCORE_FATHER_FACTOR:
			return this->getDefineINT("SCORE_FATHER_FACTOR");
			break;
		case XML_SCORE_LAND_FACTOR:
			return this->getDefineINT("SCORE_LAND_FACTOR");
			break;
		case XML_SCORE_POPULATION_FACTOR:
			return this->getDefineINT("SCORE_POPULATION_FACTOR");
			break;
		case XML_SCORE_TAX_FACTOR:
			return this->getDefineINT("SCORE_TAX_FACTOR");
			break;
		case XML_SEAWATER_SEE_FROM_CHANGE:
			return this->getDefineINT("SEAWATER_SEE_FROM_CHANGE");
			break;
		case XML_SETTLEMENT_TREASURE_YIELD:
			return this->getDefineINT("SETTLEMENT_TREASURE_YIELD");
			break;
		case XML_SHALLOW_WATER_TERRAIN:
			return this->getDefineINT("SHALLOW_WATER_TERRAIN");
			break;
		case XML_STANDARD_HANDICAP:
			return this->getDefineINT("STANDARD_HANDICAP");
			break;
		case XML_STARTING_DISTANCE_PERCENT:
			return this->getDefineINT("STARTING_DISTANCE_PERCENT");
			break;
		case XML_TAX_INCREASE_CHANCE:
			return this->getDefineINT("TAX_INCREASE_CHANCE");
			break;
		case XML_TAX_RATE_MAX_INCREASE:
			return this->getDefineINT("TAX_RATE_MAX_INCREASE");
			break;
		case XML_TAX_TRADE_THRESHOLD:
			return this->getDefineINT("TAX_TRADE_THRESHOLD");
			break;
		case XML_TAX_TRADE_THRESHOLD_ATTITUDE_PERCENT:
			return this->getDefineINT("TAX_TRADE_THRESHOLD_ATTITUDE_PERCENT");
			break;
		case XML_TAX_TRADE_THRESHOLD_TAX_RATE_PERCENT:
			return this->getDefineINT("TAX_TRADE_THRESHOLD_TAX_RATE_PERCENT");
			break;
		case XML_TREASURE_UNITCLASS:
			return this->getDefineINT("TREASURE_UNITCLASS");
			break;
		case XML_UNIT_PRODUCTION_DECAY_PERCENT:
			return this->getDefineINT("UNIT_PRODUCTION_DECAY_PERCENT");
			break;
		case XML_UNIT_PRODUCTION_DECAY_TIME:
			return this->getDefineINT("UNIT_PRODUCTION_DECAY_TIME");
			break;
		case XML_UNIT_PRODUCTION_PERCENT:
			return this->getDefineINT("UNIT_PRODUCTION_PERCENT");
			break;
		case XML_UNIT_UPGRADE_COST_PER_PRODUCTION:
			return this->getDefineINT("UNIT_UPGRADE_COST_PER_PRODUCTION");
			break;
		case XML_UNIT_VISIBILITY_RANGE:
			return this->getDefineINT("UNIT_VISIBILITY_RANGE");
			break;
		case XML_WARLORD_EXTRA_EXPERIENCE_PER_UNIT_PERCENT:
			return this->getDefineINT("WARLORD_EXTRA_EXPERIENCE_PER_UNIT_PERCENT");
			break;
		case XML_WAR_SUCCESS_ATTACKING:
			return this->getDefineINT("WAR_SUCCESS_ATTACKING");
			break;
		case XML_WAR_SUCCESS_CITY_CAPTURING:
			return this->getDefineINT("WAR_SUCCESS_CITY_CAPTURING");
			break;
		case XML_WAR_SUCCESS_DEFENDING:
			return this->getDefineINT("WAR_SUCCESS_DEFENDING");
			break;
		case XML_WAR_SUCCESS_UNIT_CAPTURING:
			return this->getDefineINT("WAR_SUCCESS_UNIT_CAPTURING");
			break;
		case XML_WATER_IMPROVEMENT:
			return this->getDefineINT("WATER_IMPROVEMENT");
			break;
		case XML_WATER_UNIT_FACING_DIRECTION:
			return this->getDefineINT("WATER_UNIT_FACING_DIRECTION");
			break;
		/// cache script hook end
		default:
			FAssertMsg(false, "CvGlobal::cacheXMLval() needs updating");
			return 0;
			break;
	}
}

void CvGlobals::cacheXMLval()
{
	// EDU remake - start - Nightinggale
	for (int i = 0; i < NUM_TEACH_LEVELS; i++)
	{
		this->m_aiEducationCost[i] = getDefineFLOAT(CvString::format("EDUCATION_COST_LEVEL_%d", i+1).c_str());
	}
	// EDU remake - end - Nightinggale
	// discoverable yield detection - start - Nightinggale
	// compile output cache for CvYieldInfo::isMustBeDiscovered()
	for (int i = 0; i < NUM_YIELD_TYPES; i++)
	{
		this->getYieldInfo((YieldTypes)i).setMustBeDiscovered();
	}
	// discoverable yield detection - end - Nightinggale

	for (int i = 0; i < NUM_XML_CONSTANTS; i++)
	{
		this->m_aiDefineCache[i] = this->getXMLuncached((XMLconstantTypes)i);
#ifdef XML_CACHE_COUNT
		this->m_aiDefineCacheCount[i] = 0;
#endif
	}

	FAssertMsg(getInfoTypeForString("CIVICOPTION_INVENTIONS") == CIVICOPTION_INVENTIONS, CvString::format("Must have index %d", CIVICOPTION_INVENTIONS).c_str());
}
// cache XML - end - Nightinggale

int CvGlobals::getDefineINT( const char * szName ) const
{
	int iReturn = 0;
	GC.getDefinesVarSystem()->GetValue( szName, iReturn );
	return iReturn;
}

float CvGlobals::getDefineFLOAT( const char * szName ) const
{
	float fReturn = 0;
	GC.getDefinesVarSystem()->GetValue( szName, fReturn );
	return fReturn;
}

const char * CvGlobals::getDefineSTRING( const char * szName ) const
{
	const char * szReturn = NULL;
	GC.getDefinesVarSystem()->GetValue( szName, szReturn );
	return szReturn;
}

void CvGlobals::setDefineINT( const char * szName, int iValue )
{
	GC.getDefinesVarSystem()->SetValue( szName, iValue );
	cacheGlobals();
}

void CvGlobals::setDefineFLOAT( const char * szName, float fValue )
{
	GC.getDefinesVarSystem()->SetValue( szName, fValue );
	cacheGlobals();
}

void CvGlobals::setDefineSTRING( const char * szName, const char * szValue )
{
	GC.getDefinesVarSystem()->SetValue( szName, szValue );
	cacheGlobals();
}

int CvGlobals::getMOVE_DENOMINATOR()
{
	return m_iMOVE_DENOMINATOR;
}

int CvGlobals::getFOOD_CONSUMPTION_PER_POPULATION()
{
	return m_iFOOD_CONSUMPTION_PER_POPULATION;
}

int CvGlobals::getMAX_HIT_POINTS()
{
	return m_iMAX_HIT_POINTS;
}

int CvGlobals::getHILLS_EXTRA_DEFENSE()
{
	return m_iHILLS_EXTRA_DEFENSE;
}

int CvGlobals::getRIVER_ATTACK_MODIFIER()
{
	return m_iRIVER_ATTACK_MODIFIER;
}

int CvGlobals::getAMPHIB_ATTACK_MODIFIER()
{
	return m_iAMPHIB_ATTACK_MODIFIER;
}

int CvGlobals::getHILLS_EXTRA_MOVEMENT()
{
	return m_iHILLS_EXTRA_MOVEMENT;
}

int CvGlobals::getPEAK_EXTRA_MOVEMENT()
{
	return m_iPEAK_EXTRA_MOVEMENT;
}

int CvGlobals::getMAX_PLOT_LIST_ROWS()
{
	return m_iMAX_PLOT_LIST_ROWS;
}

int CvGlobals::getUNIT_MULTISELECT_MAX()
{
	return m_iUNIT_MULTISELECT_MAX;
}

int CvGlobals::getEVENT_MESSAGE_TIME()
{
	return m_iEVENT_MESSAGE_TIME;
}

int CvGlobals::getROUTE_FEATURE_GROWTH_MODIFIER()
{
	return m_iROUTE_FEATURE_GROWTH_MODIFIER;
}

int CvGlobals::getFEATURE_GROWTH_MODIFIER()
{
	return m_iFEATURE_GROWTH_MODIFIER;
}

int CvGlobals::getMIN_CITY_RANGE()
{
	return m_iMIN_CITY_RANGE;
}

int CvGlobals::getCITY_MAX_NUM_BUILDINGS()
{
	return m_iCITY_MAX_NUM_BUILDINGS;
}
int CvGlobals::getLAKE_MAX_AREA_SIZE()
{
	return m_iLAKE_MAX_AREA_SIZE;
}
int CvGlobals::getMIN_WATER_SIZE_FOR_OCEAN()
{
	return m_iMIN_WATER_SIZE_FOR_OCEAN;
}

int CvGlobals::getFORTIFY_MODIFIER_PER_TURN()
{
	return m_iFORTIFY_MODIFIER_PER_TURN;
}

int CvGlobals::getMAX_CITY_DEFENSE_DAMAGE()
{
	return m_iMAX_CITY_DEFENSE_DAMAGE;
}

int CvGlobals::getPEAK_SEE_THROUGH_CHANGE()
{
	return m_iPEAK_SEE_THROUGH_CHANGE;
}

int CvGlobals::getHILLS_SEE_THROUGH_CHANGE()
{
	return m_iHILLS_SEE_THROUGH_CHANGE;
}

int CvGlobals::getSEAWATER_SEE_FROM_CHANGE()
{
	return m_iSEAWATER_SEE_FROM_CHANGE;
}

int CvGlobals::getPEAK_SEE_FROM_CHANGE()
{
	return m_iPEAK_SEE_FROM_CHANGE;
}

int CvGlobals::getHILLS_SEE_FROM_CHANGE()
{
	return m_iHILLS_SEE_FROM_CHANGE;
}

int CvGlobals::getMAX_REBEL_YIELD_MODIFIER()
{
	return m_iMAX_REBEL_YIELD_MODIFIER;
}

float CvGlobals::getCAMERA_MIN_YAW()
{
	return m_fCAMERA_MIN_YAW;
}

float CvGlobals::getCAMERA_MAX_YAW()
{
	return m_fCAMERA_MAX_YAW;
}

float CvGlobals::getCAMERA_FAR_CLIP_Z_HEIGHT()
{
	return m_fCAMERA_FAR_CLIP_Z_HEIGHT;
}

float CvGlobals::getCAMERA_MAX_TRAVEL_DISTANCE()
{
	return m_fCAMERA_MAX_TRAVEL_DISTANCE;
}

float CvGlobals::getCAMERA_START_DISTANCE()
{
	return m_fCAMERA_START_DISTANCE;
}

float CvGlobals::getPLOT_SIZE()
{
	return m_fPLOT_SIZE;
}

float CvGlobals::getCAMERA_SPECIAL_PITCH()
{
	return m_fCAMERA_SPECIAL_PITCH;
}

float CvGlobals::getCAMERA_MAX_TURN_OFFSET()
{
	return m_fCAMERA_MAX_TURN_OFFSET;
}

float CvGlobals::getCAMERA_MIN_DISTANCE()
{
	return m_fCAMERA_MIN_DISTANCE;
}

float CvGlobals::getCAMERA_UPPER_PITCH()
{
	return m_fCAMERA_UPPER_PITCH;
}

float CvGlobals::getCAMERA_LOWER_PITCH()
{
	return m_fCAMERA_LOWER_PITCH;
}

float CvGlobals::getFIELD_OF_VIEW()
{
	return m_fFIELD_OF_VIEW;
}

float CvGlobals::getUNIT_MULTISELECT_DISTANCE()
{
	return m_fUNIT_MULTISELECT_DISTANCE;
}

int CvGlobals::getUSE_CAN_FOUND_CITIES_ON_WATER_CALLBACK()
{
	return m_iUSE_CAN_FOUND_CITIES_ON_WATER_CALLBACK;
}
int CvGlobals::getUSE_CANNOT_DO_CIVIC_CALLBACK()
{
	return m_iUSE_CANNOT_DO_CIVIC_CALLBACK;
}

int CvGlobals::getUSE_CAN_DO_CIVIC_CALLBACK()
{
	return m_iUSE_CAN_DO_CIVIC_CALLBACK;
}

int CvGlobals::getUSE_CANNOT_CONSTRUCT_CALLBACK()
{
	return m_iUSE_CANNOT_CONSTRUCT_CALLBACK;
}

int CvGlobals::getUSE_CAN_CONSTRUCT_CALLBACK()
{
	return m_iUSE_CAN_CONSTRUCT_CALLBACK;
}

int CvGlobals::getUSE_CAN_DECLARE_WAR_CALLBACK()
{
	return m_iUSE_CAN_DECLARE_WAR_CALLBACK;
}
int CvGlobals::getUSE_GET_UNIT_COST_MOD_CALLBACK()
{
	return m_iUSE_GET_UNIT_COST_MOD_CALLBACK;
}

int CvGlobals::getUSE_GET_BUILDING_COST_MOD_CALLBACK()
{
	return m_iUSE_GET_BUILDING_COST_MOD_CALLBACK;
}

int CvGlobals::getUSE_GET_CITY_FOUND_VALUE_CALLBACK()
{
	return m_iUSE_GET_CITY_FOUND_VALUE_CALLBACK;
}

int CvGlobals::getUSE_CANNOT_HANDLE_ACTION_CALLBACK()
{
	return m_iUSE_CANNOT_HANDLE_ACTION_CALLBACK;
}

int CvGlobals::getUSE_CAN_BUILD_CALLBACK()
{
	return m_iUSE_CAN_BUILD_CALLBACK;
}

int CvGlobals::getUSE_CANNOT_TRAIN_CALLBACK()
{
	return m_iUSE_CANNOT_TRAIN_CALLBACK;
}

int CvGlobals::getUSE_CAN_TRAIN_CALLBACK()
{
	return m_iUSE_CAN_TRAIN_CALLBACK;
}

int CvGlobals::getUSE_UNIT_CANNOT_MOVE_INTO_CALLBACK()
{
	return m_iUSE_UNIT_CANNOT_MOVE_INTO_CALLBACK;
}

int CvGlobals::getUSE_FINISH_TEXT_CALLBACK()
{
	return m_iUSE_FINISH_TEXT_CALLBACK;
}

int CvGlobals::getUSE_ON_UNIT_SET_XY_CALLBACK()
{
	return m_iUSE_ON_UNIT_SET_XY_CALLBACK;
}

int CvGlobals::getUSE_ON_UNIT_SELECTED_CALLBACK()
{
	return m_iUSE_ON_UNIT_SELECTED_CALLBACK;
}

int CvGlobals::getUSE_ON_MISSIONARY_CONVERTED_UNIT_CALLBACK()
{
	return m_iUSE_ON_MISSIONARY_CONVERTED_UNIT_CALLBACK;
}

int CvGlobals::getUSE_ON_UPDATE_CALLBACK()
{
	return m_iUSE_ON_UPDATE_CALLBACK;
}

int CvGlobals::getUSE_ON_UNIT_CREATED_CALLBACK()
{
	return m_iUSE_ON_UNIT_CREATED_CALLBACK;
}

int CvGlobals::getUSE_ON_UNIT_LOST_CALLBACK()
{
	return m_iUSE_ON_UNIT_LOST_CALLBACK;
}

int CvGlobals::getMAX_CIV_PLAYERS()
{
	return MAX_PLAYERS;
}

int CvGlobals::getMAX_PLAYERS()
{
	return MAX_PLAYERS;
}

int CvGlobals::getMAX_CIV_TEAMS()
{
	return MAX_TEAMS;
}

int CvGlobals::getMAX_TEAMS()
{
	return MAX_TEAMS;
}

int CvGlobals::getINVALID_PLOT_COORD()
{
	return INVALID_PLOT_COORD;
}

int CvGlobals::getNUM_CITY_PLOTS()
{
	return NUM_CITY_PLOTS;
}

int CvGlobals::getCITY_HOME_PLOT()
{
	return CITY_HOME_PLOT;
}

void CvGlobals::setDLLIFace(CvDLLUtilityIFaceBase* pDll)
{
	m_pDLL = pDll;
}

int CvGlobals::getNUM_ENGINE_DIRTY_BITS() const
{
	return NUM_ENGINE_DIRTY_BITS;
}

int CvGlobals::getNUM_INTERFACE_DIRTY_BITS() const
{
	return NUM_INTERFACE_DIRTY_BITS;
}

int CvGlobals::getNUM_YIELD_TYPES() const
{
	return NUM_YIELD_TYPES;
}

int CvGlobals::getNUM_FORCECONTROL_TYPES() const
{
	return NUM_FORCECONTROL_TYPES;
}

int CvGlobals::getNUM_INFOBAR_TYPES() const
{
	return NUM_INFOBAR_TYPES;
}

int CvGlobals::getNUM_HEALTHBAR_TYPES() const
{
	return NUM_HEALTHBAR_TYPES;
}

int CvGlobals::getNUM_CONTROL_TYPES() const
{
	return NUM_CONTROL_TYPES;
}

int CvGlobals::getNUM_LEADERANIM_TYPES() const
{
	return NUM_LEADERANIM_TYPES;
}


void CvGlobals::deleteInfoArrays()
{
	for(int i=0;i<(int)m_aInfoVectors.size();i++)
	{
		deleteInfoArray(*m_aInfoVectors[i]);
	}
    //Androrc UnitArtStyles
	//this probably isn't necessary, due to the loop just before, which wasn't present in the BTS code; however, I want to be safe.
	deleteInfoArray(m_paUnitArtStyleTypeInfo);
	//Androrc End
	SAFE_DELETE_ARRAY(GC.getEntityEventTypes());
	SAFE_DELETE_ARRAY(GC.getAnimationOperatorTypes());
	SAFE_DELETE_ARRAY(GC.getFunctionTypes());
	SAFE_DELETE_ARRAY(GC.getArtStyleTypes());
	SAFE_DELETE_ARRAY(GC.getCitySizeTypes());
	SAFE_DELETE_ARRAY(GC.getContactTypes());
	SAFE_DELETE_ARRAY(GC.getDiplomacyPowerTypes());
	SAFE_DELETE_ARRAY(GC.getAutomateTypes());
	///TKs Med
	SAFE_DELETE_ARRAY(GC.getMedCityTypes());
	SAFE_DELETE_ARRAY(GC.getTradeScreenTypes());
	SAFE_DELETE_ARRAY(GC.getModCodeTypes());
	///TKe
	SAFE_DELETE_ARRAY(GC.getDirectionTypes());
	SAFE_DELETE_ARRAY(GC.getFootstepAudioTypes());
	SAFE_DELETE_ARRAY(GC.getFootstepAudioTags());

	clearTypesMap();
	m_aInfoVectors.clear();
}


//
// Global Infos Hash Map
//

int CvGlobals::getInfoTypeForString(const char* szType, bool hideAssert) const
{
	FAssertMsg(szType, "null info type string");
	InfosMap::const_iterator it = m_infosMap.find(szType);
	if (it!=m_infosMap.end())
	{
		return it->second;
	}

	if(!hideAssert)
	{
		CvString szError;
		szError.Format("info type %s not found, Current XML file is: %s", szType, GC.getCurrentXMLFile().GetCString());
		FAssertMsg(strcmp(szType, "NONE")==0 || strcmp(szType, "")==0, szError.c_str());
		gDLL->logMsg("xml.log", szError);
	}

	return -1;
}

void CvGlobals::setInfoTypeFromString(const char* szType, int idx)
{
	FAssertMsg(szType, "null info type string");
#ifdef _DEBUG
	InfosMap::const_iterator it = m_infosMap.find(szType);
	int iExisting = (it!=m_infosMap.end()) ? it->second : -1;
	FAssertMsg(iExisting==-1 || iExisting==idx || strcmp(szType, "ERROR")==0, CvString::format("xml info type entry %s already exists", szType).c_str());
#endif
	m_infosMap[szType] = idx;
}

void CvGlobals::infoTypeFromStringReset()
{
	m_infosMap.clear();
}

void CvGlobals::addToInfosVectors(void *infoVector)
{
	std::vector<CvInfoBase *> *infoBaseVector = (std::vector<CvInfoBase *> *) infoVector;
	m_aInfoVectors.push_back(infoBaseVector);
}

void CvGlobals::infosReset()
{
	for(int i=0;i<(int)m_aInfoVectors.size();i++)
	{
		std::vector<CvInfoBase *> *infoBaseVector = m_aInfoVectors[i];
		for(int j=0;j<(int)infoBaseVector->size();j++)
			infoBaseVector->at(j)->reset();
	}
}

int CvGlobals::getNumDirections() const { return NUM_DIRECTION_TYPES; }
int CvGlobals::getNumResourceLayers() const { return NUM_RESOURCE_LAYERS; }
int CvGlobals::getNumUnitLayerOptionTypes() const { return NUM_UNIT_LAYER_OPTION_TYPES; }
int CvGlobals::getNumGameOptions() const { return NUM_GAMEOPTION_TYPES; }
int CvGlobals::getNumMPOptions() const { return NUM_MPOPTION_TYPES; }
int CvGlobals::getNumSpecialOptions() const { return NUM_SPECIALOPTION_TYPES; }
int CvGlobals::getNumGraphicOptions() const { return NUM_GRAPHICOPTION_TYPES; }
int CvGlobals::getNumTradeableItems() const { return NUM_TRADEABLE_ITEMS; }
int CvGlobals::getNumBasicItems() const { return NUM_BASIC_ITEMS; }
int CvGlobals::getNumTradeableHeadings() const { return NUM_TRADEABLE_HEADINGS; }
int CvGlobals::getNumCommandInfos() const { return NUM_COMMAND_TYPES; }
int CvGlobals::getNumControlInfos() const { return NUM_CONTROL_TYPES; }
int CvGlobals::getNumMissionInfos() const { return NUM_MISSION_TYPES; }
int CvGlobals::getNumPlayerOptionInfos() const { return NUM_PLAYEROPTION_TYPES; }
int CvGlobals::getMaxNumSymbols() const { return MAX_NUM_SYMBOLS; }
int CvGlobals::getNumGraphicLevels() const { return NUM_GRAPHICLEVELS; }


//
// non-inline versions
//
CvMap& CvGlobals::getMap() { return *m_map; }
CvGameAI& CvGlobals::getGame() { return *m_game; }
CvGameAI *CvGlobals::getGamePointer(){ return m_game; }

int CvGlobals::getMaxCivPlayers() const
{
	return MAX_PLAYERS;
}

bool CvGlobals::IsGraphicsInitialized() const { return m_bGraphicsInitialized;}
void CvGlobals::SetGraphicsInitialized(bool bVal) { m_bGraphicsInitialized = bVal;}
void CvGlobals::setInterface(CvInterface* pVal) { m_interface = pVal; }
void CvGlobals::setDiplomacyScreen(CvDiplomacyScreen* pVal) { m_diplomacyScreen = pVal; }
void CvGlobals::setMPDiplomacyScreen(CMPDiplomacyScreen* pVal) { m_mpDiplomacyScreen = pVal; }
void CvGlobals::setMessageQueue(CMessageQueue* pVal) { m_messageQueue = pVal; }
void CvGlobals::setHotJoinMessageQueue(CMessageQueue* pVal) { m_hotJoinMsgQueue = pVal; }
void CvGlobals::setMessageControl(CMessageControl* pVal) { m_messageControl = pVal; }
void CvGlobals::setSetupData(CvSetupData* pVal) { m_setupData = pVal; }
void CvGlobals::setMessageCodeTranslator(CvMessageCodeTranslator* pVal) { m_messageCodes = pVal; }
void CvGlobals::setDropMgr(CvDropMgr* pVal) { m_dropMgr = pVal; }
void CvGlobals::setPortal(CvPortal* pVal) { m_portal = pVal; }
void CvGlobals::setStatsReport(CvStatsReporter* pVal) { m_statsReporter = pVal; }
void CvGlobals::setPathFinder(FAStar* pVal) { m_pathFinder = pVal; }
void CvGlobals::setInterfacePathFinder(FAStar* pVal) { m_interfacePathFinder = pVal; }
void CvGlobals::setStepFinder(FAStar* pVal) { m_stepFinder = pVal; }
void CvGlobals::setRouteFinder(FAStar* pVal) { m_routeFinder = pVal; }
void CvGlobals::setBorderFinder(FAStar* pVal) { m_borderFinder = pVal; }
void CvGlobals::setAreaFinder(FAStar* pVal) { m_areaFinder = pVal; }
CvDLLUtilityIFaceBase* CvGlobals::getDLLIFaceNonInl() { return m_pDLL; }


// XML enum check - start - Nightinggale

/*
 * Certain XML files have their types hardcoded in enums.
 * While this certainly increase execution speed, it also adds the risk that the XML and DLL goes out of sync.
 * The following code is to detect desyncs when the game starts as undetected desyncs can cause havoc.
 *
 * This code is autogenerated based on the enums.
 *    Nightinggale
 */

void CvGlobals::CheckEnumInterfaceModeTypes() const
{
	FAssertMsg(!strcmp(GC.getInterfaceModeInfo(INTERFACEMODE_SELECTION).getType(), "INTERFACEMODE_SELECTION"), CvString::format("XML error. Found %s instead of INTERFACEMODE_SELECTION at index %d", GC.getInterfaceModeInfo(INTERFACEMODE_SELECTION).getType(), INTERFACEMODE_SELECTION).c_str());
	FAssertMsg(!strcmp(GC.getInterfaceModeInfo(INTERFACEMODE_PING).getType(), "INTERFACEMODE_PING"), CvString::format("XML error. Found %s instead of INTERFACEMODE_PING at index %d", GC.getInterfaceModeInfo(INTERFACEMODE_PING).getType(), INTERFACEMODE_PING).c_str());
	FAssertMsg(!strcmp(GC.getInterfaceModeInfo(INTERFACEMODE_SIGN).getType(), "INTERFACEMODE_SIGN"), CvString::format("XML error. Found %s instead of INTERFACEMODE_SIGN at index %d", GC.getInterfaceModeInfo(INTERFACEMODE_SIGN).getType(), INTERFACEMODE_SIGN).c_str());
	FAssertMsg(!strcmp(GC.getInterfaceModeInfo(INTERFACEMODE_GRIP).getType(), "INTERFACEMODE_GRIP"), CvString::format("XML error. Found %s instead of INTERFACEMODE_GRIP at index %d", GC.getInterfaceModeInfo(INTERFACEMODE_GRIP).getType(), INTERFACEMODE_GRIP).c_str());
	FAssertMsg(!strcmp(GC.getInterfaceModeInfo(INTERFACEMODE_GLOBELAYER_INPUT).getType(), "INTERFACEMODE_GLOBELAYER_INPUT"), CvString::format("XML error. Found %s instead of INTERFACEMODE_GLOBELAYER_INPUT at index %d", GC.getInterfaceModeInfo(INTERFACEMODE_GLOBELAYER_INPUT).getType(), INTERFACEMODE_GLOBELAYER_INPUT).c_str());
	FAssertMsg(!strcmp(GC.getInterfaceModeInfo(INTERFACEMODE_GO_TO).getType(), "INTERFACEMODE_GO_TO"), CvString::format("XML error. Found %s instead of INTERFACEMODE_GO_TO at index %d", GC.getInterfaceModeInfo(INTERFACEMODE_GO_TO).getType(), INTERFACEMODE_GO_TO).c_str());
	FAssertMsg(!strcmp(GC.getInterfaceModeInfo(INTERFACEMODE_GO_TO_TYPE).getType(), "INTERFACEMODE_GO_TO_TYPE"), CvString::format("XML error. Found %s instead of INTERFACEMODE_GO_TO_TYPE at index %d", GC.getInterfaceModeInfo(INTERFACEMODE_GO_TO_TYPE).getType(), INTERFACEMODE_GO_TO_TYPE).c_str());
	FAssertMsg(!strcmp(GC.getInterfaceModeInfo(INTERFACEMODE_GO_TO_ALL).getType(), "INTERFACEMODE_GO_TO_ALL"), CvString::format("XML error. Found %s instead of INTERFACEMODE_GO_TO_ALL at index %d", GC.getInterfaceModeInfo(INTERFACEMODE_GO_TO_ALL).getType(), INTERFACEMODE_GO_TO_ALL).c_str());
	FAssertMsg(!strcmp(GC.getInterfaceModeInfo(INTERFACEMODE_ROUTE_TO).getType(), "INTERFACEMODE_ROUTE_TO"), CvString::format("XML error. Found %s instead of INTERFACEMODE_ROUTE_TO at index %d", GC.getInterfaceModeInfo(INTERFACEMODE_ROUTE_TO).getType(), INTERFACEMODE_ROUTE_TO).c_str());
	FAssertMsg(!strcmp(GC.getInterfaceModeInfo(INTERFACEMODE_PYTHON_PICK_PLOT).getType(), "INTERFACEMODE_PYTHON_PICK_PLOT"), CvString::format("XML error. Found %s instead of INTERFACEMODE_PYTHON_PICK_PLOT at index %d", GC.getInterfaceModeInfo(INTERFACEMODE_PYTHON_PICK_PLOT).getType(), INTERFACEMODE_PYTHON_PICK_PLOT).c_str());
	FAssertMsg(!strcmp(GC.getInterfaceModeInfo(INTERFACEMODE_SAVE_PLOT_NIFS).getType(), "INTERFACEMODE_SAVE_PLOT_NIFS"), CvString::format("XML error. Found %s instead of INTERFACEMODE_SAVE_PLOT_NIFS at index %d", GC.getInterfaceModeInfo(INTERFACEMODE_SAVE_PLOT_NIFS).getType(), INTERFACEMODE_SAVE_PLOT_NIFS).c_str());

	FAssertMsg(GC.getInterfaceModeInfo().size() == NUM_INTERFACEMODE_TYPES, CvString::format("XML error. Expected %d types, but found %d", NUM_INTERFACEMODE_TYPES, GC.getInterfaceModeInfo().size()));
}

void CvGlobals::CheckEnumWorldSizeTypes() const
{
	FAssertMsg(!strcmp(GC.getWorldInfo(WORLDSIZE_TINY).getType(), "WORLDSIZE_TINY"), CvString::format("XML error. Found %s instead of WORLDSIZE_TINY at index %d", GC.getWorldInfo(WORLDSIZE_TINY).getType(), WORLDSIZE_TINY).c_str());
	FAssertMsg(!strcmp(GC.getWorldInfo(WORLDSIZE_SMALL).getType(), "WORLDSIZE_SMALL"), CvString::format("XML error. Found %s instead of WORLDSIZE_SMALL at index %d", GC.getWorldInfo(WORLDSIZE_SMALL).getType(), WORLDSIZE_SMALL).c_str());
	FAssertMsg(!strcmp(GC.getWorldInfo(WORLDSIZE_STANDARD).getType(), "WORLDSIZE_STANDARD"), CvString::format("XML error. Found %s instead of WORLDSIZE_STANDARD at index %d", GC.getWorldInfo(WORLDSIZE_STANDARD).getType(), WORLDSIZE_STANDARD).c_str());
	FAssertMsg(!strcmp(GC.getWorldInfo(WORLDSIZE_LARGE).getType(), "WORLDSIZE_LARGE"), CvString::format("XML error. Found %s instead of WORLDSIZE_LARGE at index %d", GC.getWorldInfo(WORLDSIZE_LARGE).getType(), WORLDSIZE_LARGE).c_str());
	FAssertMsg(!strcmp(GC.getWorldInfo(WORLDSIZE_HUGE).getType(), "WORLDSIZE_HUGE"), CvString::format("XML error. Found %s instead of WORLDSIZE_HUGE at index %d", GC.getWorldInfo(WORLDSIZE_HUGE).getType(), WORLDSIZE_HUGE).c_str());

	FAssertMsg(GC.getWorldInfo().size() == NUM_WORLDSIZE_TYPES, CvString::format("XML error. Expected %d types, but found %d", NUM_WORLDSIZE_TYPES, GC.getWorldInfo().size()));
}

void CvGlobals::CheckEnumGameOptionTypes() const
{
	FAssertMsg(!strcmp(GC.getGameOptionInfo(GAMEOPTION_ADVANCED_START).getType(), "GAMEOPTION_ADVANCED_START"), CvString::format("XML error. Found %s instead of GAMEOPTION_ADVANCED_START at index %d", GC.getGameOptionInfo(GAMEOPTION_ADVANCED_START).getType(), GAMEOPTION_ADVANCED_START).c_str());
	FAssertMsg(!strcmp(GC.getGameOptionInfo(GAMEOPTION_NO_CITY_RAZING).getType(), "GAMEOPTION_NO_CITY_RAZING"), CvString::format("XML error. Found %s instead of GAMEOPTION_NO_CITY_RAZING at index %d", GC.getGameOptionInfo(GAMEOPTION_NO_CITY_RAZING).getType(), GAMEOPTION_NO_CITY_RAZING).c_str());
	FAssertMsg(!strcmp(GC.getGameOptionInfo(GAMEOPTION_AGGRESSIVE_AI).getType(), "GAMEOPTION_AGGRESSIVE_AI"), CvString::format("XML error. Found %s instead of GAMEOPTION_AGGRESSIVE_AI at index %d", GC.getGameOptionInfo(GAMEOPTION_AGGRESSIVE_AI).getType(), GAMEOPTION_AGGRESSIVE_AI).c_str());
	FAssertMsg(!strcmp(GC.getGameOptionInfo(GAMEOPTION_NO_WILD_LAND_ANIMALS).getType(), "GAMEOPTION_NO_WILD_LAND_ANIMALS"), CvString::format("XML error. Found %s instead of GAMEOPTION_NO_WILD_LAND_ANIMALS at index %d", GC.getGameOptionInfo(GAMEOPTION_NO_WILD_LAND_ANIMALS).getType(), GAMEOPTION_NO_WILD_LAND_ANIMALS).c_str());
	FAssertMsg(!strcmp(GC.getGameOptionInfo(GAMEOPTION_NO_WILD_SEA_ANIMALS).getType(), "GAMEOPTION_NO_WILD_SEA_ANIMALS"), CvString::format("XML error. Found %s instead of GAMEOPTION_NO_WILD_SEA_ANIMALS at index %d", GC.getGameOptionInfo(GAMEOPTION_NO_WILD_SEA_ANIMALS).getType(), GAMEOPTION_NO_WILD_SEA_ANIMALS).c_str());
	FAssertMsg(!strcmp(GC.getGameOptionInfo(GAMEOPTION_LEAD_ANY_CIV).getType(), "GAMEOPTION_LEAD_ANY_CIV"), CvString::format("XML error. Found %s instead of GAMEOPTION_LEAD_ANY_CIV at index %d", GC.getGameOptionInfo(GAMEOPTION_LEAD_ANY_CIV).getType(), GAMEOPTION_LEAD_ANY_CIV).c_str());
	FAssertMsg(!strcmp(GC.getGameOptionInfo(GAMEOPTION_PERMANENT_ALLIANCES).getType(), "GAMEOPTION_PERMANENT_ALLIANCES"), CvString::format("XML error. Found %s instead of GAMEOPTION_PERMANENT_ALLIANCES at index %d", GC.getGameOptionInfo(GAMEOPTION_PERMANENT_ALLIANCES).getType(), GAMEOPTION_PERMANENT_ALLIANCES).c_str());
	FAssertMsg(!strcmp(GC.getGameOptionInfo(GAMEOPTION_ALWAYS_WAR).getType(), "GAMEOPTION_ALWAYS_WAR"), CvString::format("XML error. Found %s instead of GAMEOPTION_ALWAYS_WAR at index %d", GC.getGameOptionInfo(GAMEOPTION_ALWAYS_WAR).getType(), GAMEOPTION_ALWAYS_WAR).c_str());
	FAssertMsg(!strcmp(GC.getGameOptionInfo(GAMEOPTION_ALWAYS_PEACE).getType(), "GAMEOPTION_ALWAYS_PEACE"), CvString::format("XML error. Found %s instead of GAMEOPTION_ALWAYS_PEACE at index %d", GC.getGameOptionInfo(GAMEOPTION_ALWAYS_PEACE).getType(), GAMEOPTION_ALWAYS_PEACE).c_str());
	FAssertMsg(!strcmp(GC.getGameOptionInfo(GAMEOPTION_ONE_CITY_CHALLENGE).getType(), "GAMEOPTION_ONE_CITY_CHALLENGE"), CvString::format("XML error. Found %s instead of GAMEOPTION_ONE_CITY_CHALLENGE at index %d", GC.getGameOptionInfo(GAMEOPTION_ONE_CITY_CHALLENGE).getType(), GAMEOPTION_ONE_CITY_CHALLENGE).c_str());
	FAssertMsg(!strcmp(GC.getGameOptionInfo(GAMEOPTION_NO_CHANGING_WAR_PEACE).getType(), "GAMEOPTION_NO_CHANGING_WAR_PEACE"), CvString::format("XML error. Found %s instead of GAMEOPTION_NO_CHANGING_WAR_PEACE at index %d", GC.getGameOptionInfo(GAMEOPTION_NO_CHANGING_WAR_PEACE).getType(), GAMEOPTION_NO_CHANGING_WAR_PEACE).c_str());
	FAssertMsg(!strcmp(GC.getGameOptionInfo(GAMEOPTION_NEW_RANDOM_SEED).getType(), "GAMEOPTION_NEW_RANDOM_SEED"), CvString::format("XML error. Found %s instead of GAMEOPTION_NEW_RANDOM_SEED at index %d", GC.getGameOptionInfo(GAMEOPTION_NEW_RANDOM_SEED).getType(), GAMEOPTION_NEW_RANDOM_SEED).c_str());
	FAssertMsg(!strcmp(GC.getGameOptionInfo(GAMEOPTION_LOCK_MODS).getType(), "GAMEOPTION_LOCK_MODS"), CvString::format("XML error. Found %s instead of GAMEOPTION_LOCK_MODS at index %d", GC.getGameOptionInfo(GAMEOPTION_LOCK_MODS).getType(), GAMEOPTION_LOCK_MODS).c_str());
	FAssertMsg(!strcmp(GC.getGameOptionInfo(GAMEOPTION_NO_GOODY_HUTS).getType(), "GAMEOPTION_NO_GOODY_HUTS"), CvString::format("XML error. Found %s instead of GAMEOPTION_NO_GOODY_HUTS at index %d", GC.getGameOptionInfo(GAMEOPTION_NO_GOODY_HUTS).getType(), GAMEOPTION_NO_GOODY_HUTS).c_str());
	FAssertMsg(!strcmp(GC.getGameOptionInfo(GAMEOPTION_NO_EVENTS).getType(), "GAMEOPTION_NO_EVENTS"), CvString::format("XML error. Found %s instead of GAMEOPTION_NO_EVENTS at index %d", GC.getGameOptionInfo(GAMEOPTION_NO_EVENTS).getType(), GAMEOPTION_NO_EVENTS).c_str());

	FAssertMsg(GC.getGameOptionInfo().size() == NUM_GAMEOPTION_TYPES, CvString::format("XML error. Expected %d types, but found %d", NUM_GAMEOPTION_TYPES, GC.getGameOptionInfo().size()));
}

void CvGlobals::CheckEnumPlayerOptionTypes() const
{
	FAssertMsg(!strcmp(GC.getPlayerOptionInfo(PLAYEROPTION_TUTORIAL).getType(), "PLAYEROPTION_TUTORIAL"), CvString::format("XML error. Found %s instead of PLAYEROPTION_TUTORIAL at index %d", GC.getPlayerOptionInfo(PLAYEROPTION_TUTORIAL).getType(), PLAYEROPTION_TUTORIAL).c_str());
	FAssertMsg(!strcmp(GC.getPlayerOptionInfo(PLAYEROPTION_ADVISOR_POPUPS).getType(), "PLAYEROPTION_ADVISOR_POPUPS"), CvString::format("XML error. Found %s instead of PLAYEROPTION_ADVISOR_POPUPS at index %d", GC.getPlayerOptionInfo(PLAYEROPTION_ADVISOR_POPUPS).getType(), PLAYEROPTION_ADVISOR_POPUPS).c_str());
	FAssertMsg(!strcmp(GC.getPlayerOptionInfo(PLAYEROPTION_ADVISOR_HELP).getType(), "PLAYEROPTION_ADVISOR_HELP"), CvString::format("XML error. Found %s instead of PLAYEROPTION_ADVISOR_HELP at index %d", GC.getPlayerOptionInfo(PLAYEROPTION_ADVISOR_HELP).getType(), PLAYEROPTION_ADVISOR_HELP).c_str());
	FAssertMsg(!strcmp(GC.getPlayerOptionInfo(PLAYEROPTION_WAIT_END_TURN).getType(), "PLAYEROPTION_WAIT_END_TURN"), CvString::format("XML error. Found %s instead of PLAYEROPTION_WAIT_END_TURN at index %d", GC.getPlayerOptionInfo(PLAYEROPTION_WAIT_END_TURN).getType(), PLAYEROPTION_WAIT_END_TURN).c_str());
	FAssertMsg(!strcmp(GC.getPlayerOptionInfo(PLAYEROPTION_MINIMIZE_POP_UPS).getType(), "PLAYEROPTION_MINIMIZE_POP_UPS"), CvString::format("XML error. Found %s instead of PLAYEROPTION_MINIMIZE_POP_UPS at index %d", GC.getPlayerOptionInfo(PLAYEROPTION_MINIMIZE_POP_UPS).getType(), PLAYEROPTION_MINIMIZE_POP_UPS).c_str());
	FAssertMsg(!strcmp(GC.getPlayerOptionInfo(PLAYEROPTION_SHOW_FRIENDLY_MOVES).getType(), "PLAYEROPTION_SHOW_FRIENDLY_MOVES"), CvString::format("XML error. Found %s instead of PLAYEROPTION_SHOW_FRIENDLY_MOVES at index %d", GC.getPlayerOptionInfo(PLAYEROPTION_SHOW_FRIENDLY_MOVES).getType(), PLAYEROPTION_SHOW_FRIENDLY_MOVES).c_str());
	FAssertMsg(!strcmp(GC.getPlayerOptionInfo(PLAYEROPTION_SHOW_ENEMY_MOVES).getType(), "PLAYEROPTION_SHOW_ENEMY_MOVES"), CvString::format("XML error. Found %s instead of PLAYEROPTION_SHOW_ENEMY_MOVES at index %d", GC.getPlayerOptionInfo(PLAYEROPTION_SHOW_ENEMY_MOVES).getType(), PLAYEROPTION_SHOW_ENEMY_MOVES).c_str());
	FAssertMsg(!strcmp(GC.getPlayerOptionInfo(PLAYEROPTION_QUICK_MOVES).getType(), "PLAYEROPTION_QUICK_MOVES"), CvString::format("XML error. Found %s instead of PLAYEROPTION_QUICK_MOVES at index %d", GC.getPlayerOptionInfo(PLAYEROPTION_QUICK_MOVES).getType(), PLAYEROPTION_QUICK_MOVES).c_str());
	FAssertMsg(!strcmp(GC.getPlayerOptionInfo(PLAYEROPTION_QUICK_ATTACK).getType(), "PLAYEROPTION_QUICK_ATTACK"), CvString::format("XML error. Found %s instead of PLAYEROPTION_QUICK_ATTACK at index %d", GC.getPlayerOptionInfo(PLAYEROPTION_QUICK_ATTACK).getType(), PLAYEROPTION_QUICK_ATTACK).c_str());
	FAssertMsg(!strcmp(GC.getPlayerOptionInfo(PLAYEROPTION_QUICK_DEFENSE).getType(), "PLAYEROPTION_QUICK_DEFENSE"), CvString::format("XML error. Found %s instead of PLAYEROPTION_QUICK_DEFENSE at index %d", GC.getPlayerOptionInfo(PLAYEROPTION_QUICK_DEFENSE).getType(), PLAYEROPTION_QUICK_DEFENSE).c_str());
	FAssertMsg(!strcmp(GC.getPlayerOptionInfo(PLAYEROPTION_STACK_ATTACK).getType(), "PLAYEROPTION_STACK_ATTACK"), CvString::format("XML error. Found %s instead of PLAYEROPTION_STACK_ATTACK at index %d", GC.getPlayerOptionInfo(PLAYEROPTION_STACK_ATTACK).getType(), PLAYEROPTION_STACK_ATTACK).c_str());
	FAssertMsg(!strcmp(GC.getPlayerOptionInfo(PLAYEROPTION_AUTO_PROMOTION).getType(), "PLAYEROPTION_AUTO_PROMOTION"), CvString::format("XML error. Found %s instead of PLAYEROPTION_AUTO_PROMOTION at index %d", GC.getPlayerOptionInfo(PLAYEROPTION_AUTO_PROMOTION).getType(), PLAYEROPTION_AUTO_PROMOTION).c_str());
	FAssertMsg(!strcmp(GC.getPlayerOptionInfo(PLAYEROPTION_SAFE_AUTOMATION).getType(), "PLAYEROPTION_SAFE_AUTOMATION"), CvString::format("XML error. Found %s instead of PLAYEROPTION_SAFE_AUTOMATION at index %d", GC.getPlayerOptionInfo(PLAYEROPTION_SAFE_AUTOMATION).getType(), PLAYEROPTION_SAFE_AUTOMATION).c_str());
	FAssertMsg(!strcmp(GC.getPlayerOptionInfo(PLAYEROPTION_NUMPAD_HELP).getType(), "PLAYEROPTION_NUMPAD_HELP"), CvString::format("XML error. Found %s instead of PLAYEROPTION_NUMPAD_HELP at index %d", GC.getPlayerOptionInfo(PLAYEROPTION_NUMPAD_HELP).getType(), PLAYEROPTION_NUMPAD_HELP).c_str());
	FAssertMsg(!strcmp(GC.getPlayerOptionInfo(PLAYEROPTION_NO_UNIT_CYCLING).getType(), "PLAYEROPTION_NO_UNIT_CYCLING"), CvString::format("XML error. Found %s instead of PLAYEROPTION_NO_UNIT_CYCLING at index %d", GC.getPlayerOptionInfo(PLAYEROPTION_NO_UNIT_CYCLING).getType(), PLAYEROPTION_NO_UNIT_CYCLING).c_str());
	FAssertMsg(!strcmp(GC.getPlayerOptionInfo(PLAYEROPTION_NO_UNIT_RECOMMENDATIONS).getType(), "PLAYEROPTION_NO_UNIT_RECOMMENDATIONS"), CvString::format("XML error. Found %s instead of PLAYEROPTION_NO_UNIT_RECOMMENDATIONS at index %d", GC.getPlayerOptionInfo(PLAYEROPTION_NO_UNIT_RECOMMENDATIONS).getType(), PLAYEROPTION_NO_UNIT_RECOMMENDATIONS).c_str());
	FAssertMsg(!strcmp(GC.getPlayerOptionInfo(PLAYEROPTION_RIGHT_CLICK_MENU).getType(), "PLAYEROPTION_RIGHT_CLICK_MENU"), CvString::format("XML error. Found %s instead of PLAYEROPTION_RIGHT_CLICK_MENU at index %d", GC.getPlayerOptionInfo(PLAYEROPTION_RIGHT_CLICK_MENU).getType(), PLAYEROPTION_RIGHT_CLICK_MENU).c_str());
	FAssertMsg(!strcmp(GC.getPlayerOptionInfo(PLAYEROPTION_LEAVE_FORESTS).getType(), "PLAYEROPTION_LEAVE_FORESTS"), CvString::format("XML error. Found %s instead of PLAYEROPTION_LEAVE_FORESTS at index %d", GC.getPlayerOptionInfo(PLAYEROPTION_LEAVE_FORESTS).getType(), PLAYEROPTION_LEAVE_FORESTS).c_str());
	FAssertMsg(!strcmp(GC.getPlayerOptionInfo(PLAYEROPTION_MODDER_1).getType(), "PLAYEROPTION_MODDER_1"), CvString::format("XML error. Found %s instead of PLAYEROPTION_MODDER_1 at index %d", GC.getPlayerOptionInfo(PLAYEROPTION_MODDER_1).getType(), PLAYEROPTION_MODDER_1).c_str());
	FAssertMsg(!strcmp(GC.getPlayerOptionInfo(PLAYEROPTION_MODDER_2).getType(), "PLAYEROPTION_MODDER_2"), CvString::format("XML error. Found %s instead of PLAYEROPTION_MODDER_2 at index %d", GC.getPlayerOptionInfo(PLAYEROPTION_MODDER_2).getType(), PLAYEROPTION_MODDER_2).c_str());
	FAssertMsg(!strcmp(GC.getPlayerOptionInfo(PLAYEROPTION_MODDER_3).getType(), "PLAYEROPTION_MODDER_3"), CvString::format("XML error. Found %s instead of PLAYEROPTION_MODDER_3 at index %d", GC.getPlayerOptionInfo(PLAYEROPTION_MODDER_3).getType(), PLAYEROPTION_MODDER_3).c_str());
	FAssertMsg(!strcmp(GC.getPlayerOptionInfo(PLAYEROPTION_MODDER_4).getType(), "PLAYEROPTION_MODDER_4"), CvString::format("XML error. Found %s instead of PLAYEROPTION_MODDER_4 at index %d", GC.getPlayerOptionInfo(PLAYEROPTION_MODDER_4).getType(), PLAYEROPTION_MODDER_4).c_str());
	FAssertMsg(!strcmp(GC.getPlayerOptionInfo(PLAYEROPTION_MODDER_5).getType(), "PLAYEROPTION_MODDER_5"), CvString::format("XML error. Found %s instead of PLAYEROPTION_MODDER_5 at index %d", GC.getPlayerOptionInfo(PLAYEROPTION_MODDER_5).getType(), PLAYEROPTION_MODDER_5).c_str());
	FAssertMsg(!strcmp(GC.getPlayerOptionInfo(PLAYEROPTION_MODDER_6).getType(), "PLAYEROPTION_MODDER_6"), CvString::format("XML error. Found %s instead of PLAYEROPTION_MODDER_6 at index %d", GC.getPlayerOptionInfo(PLAYEROPTION_MODDER_6).getType(), PLAYEROPTION_MODDER_6).c_str());

	FAssertMsg(GC.getPlayerOptionInfo().size() == NUM_PLAYEROPTION_TYPES, CvString::format("XML error. Expected %d types, but found %d", NUM_PLAYEROPTION_TYPES, GC.getPlayerOptionInfo().size()));
}

void CvGlobals::CheckEnumGraphicOptionTypes() const
{
	FAssertMsg(!strcmp(GC.getGraphicOptionInfo(GRAPHICOPTION_HEALTH_BARS).getType(), "GRAPHICOPTION_HEALTH_BARS"), CvString::format("XML error. Found %s instead of GRAPHICOPTION_HEALTH_BARS at index %d", GC.getGraphicOptionInfo(GRAPHICOPTION_HEALTH_BARS).getType(), GRAPHICOPTION_HEALTH_BARS).c_str());
	FAssertMsg(!strcmp(GC.getGraphicOptionInfo(GRAPHICOPTION_NO_COMBAT_ZOOM).getType(), "GRAPHICOPTION_NO_COMBAT_ZOOM"), CvString::format("XML error. Found %s instead of GRAPHICOPTION_NO_COMBAT_ZOOM at index %d", GC.getGraphicOptionInfo(GRAPHICOPTION_NO_COMBAT_ZOOM).getType(), GRAPHICOPTION_NO_COMBAT_ZOOM).c_str());
	FAssertMsg(!strcmp(GC.getGraphicOptionInfo(GRAPHICOPTION_NO_ENEMY_GLOW).getType(), "GRAPHICOPTION_NO_ENEMY_GLOW"), CvString::format("XML error. Found %s instead of GRAPHICOPTION_NO_ENEMY_GLOW at index %d", GC.getGraphicOptionInfo(GRAPHICOPTION_NO_ENEMY_GLOW).getType(), GRAPHICOPTION_NO_ENEMY_GLOW).c_str());
	FAssertMsg(!strcmp(GC.getGraphicOptionInfo(GRAPHICOPTION_FROZEN_ANIMATIONS).getType(), "GRAPHICOPTION_FROZEN_ANIMATIONS"), CvString::format("XML error. Found %s instead of GRAPHICOPTION_FROZEN_ANIMATIONS at index %d", GC.getGraphicOptionInfo(GRAPHICOPTION_FROZEN_ANIMATIONS).getType(), GRAPHICOPTION_FROZEN_ANIMATIONS).c_str());
	FAssertMsg(!strcmp(GC.getGraphicOptionInfo(GRAPHICOPTION_EFFECTS_DISABLED).getType(), "GRAPHICOPTION_EFFECTS_DISABLED"), CvString::format("XML error. Found %s instead of GRAPHICOPTION_EFFECTS_DISABLED at index %d", GC.getGraphicOptionInfo(GRAPHICOPTION_EFFECTS_DISABLED).getType(), GRAPHICOPTION_EFFECTS_DISABLED).c_str());
	FAssertMsg(!strcmp(GC.getGraphicOptionInfo(GRAPHICOPTION_GLOBE_VIEW_BUILDINGS_DISABLED).getType(), "GRAPHICOPTION_GLOBE_VIEW_BUILDINGS_DISABLED"), CvString::format("XML error. Found %s instead of GRAPHICOPTION_GLOBE_VIEW_BUILDINGS_DISABLED at index %d", GC.getGraphicOptionInfo(GRAPHICOPTION_GLOBE_VIEW_BUILDINGS_DISABLED).getType(), GRAPHICOPTION_GLOBE_VIEW_BUILDINGS_DISABLED).c_str());
	FAssertMsg(!strcmp(GC.getGraphicOptionInfo(GRAPHICOPTION_FULLSCREEN).getType(), "GRAPHICOPTION_FULLSCREEN"), CvString::format("XML error. Found %s instead of GRAPHICOPTION_FULLSCREEN at index %d", GC.getGraphicOptionInfo(GRAPHICOPTION_FULLSCREEN).getType(), GRAPHICOPTION_FULLSCREEN).c_str());
	FAssertMsg(!strcmp(GC.getGraphicOptionInfo(GRAPHICOPTION_LOWRES_TEXTURES).getType(), "GRAPHICOPTION_LOWRES_TEXTURES"), CvString::format("XML error. Found %s instead of GRAPHICOPTION_LOWRES_TEXTURES at index %d", GC.getGraphicOptionInfo(GRAPHICOPTION_LOWRES_TEXTURES).getType(), GRAPHICOPTION_LOWRES_TEXTURES).c_str());
	FAssertMsg(!strcmp(GC.getGraphicOptionInfo(GRAPHICOPTION_HIRES_TERRAIN).getType(), "GRAPHICOPTION_HIRES_TERRAIN"), CvString::format("XML error. Found %s instead of GRAPHICOPTION_HIRES_TERRAIN at index %d", GC.getGraphicOptionInfo(GRAPHICOPTION_HIRES_TERRAIN).getType(), GRAPHICOPTION_HIRES_TERRAIN).c_str());
	FAssertMsg(!strcmp(GC.getGraphicOptionInfo(GRAPHICOPTION_NO_MOVIES).getType(), "GRAPHICOPTION_NO_MOVIES"), CvString::format("XML error. Found %s instead of GRAPHICOPTION_NO_MOVIES at index %d", GC.getGraphicOptionInfo(GRAPHICOPTION_NO_MOVIES).getType(), GRAPHICOPTION_NO_MOVIES).c_str());

	FAssertMsg(GC.getGraphicOptionInfo().size() == NUM_GRAPHICOPTION_TYPES, CvString::format("XML error. Expected %d types, but found %d", NUM_GRAPHICOPTION_TYPES, GC.getGraphicOptionInfo().size()));
}

void CvGlobals::CheckEnumForceControlTypes() const
{
	FAssertMsg(!strcmp(GC.getForceControlInfo(FORCECONTROL_SPEED).getType(), "FORCECONTROL_SPEED"), CvString::format("XML error. Found %s instead of FORCECONTROL_SPEED at index %d", GC.getForceControlInfo(FORCECONTROL_SPEED).getType(), FORCECONTROL_SPEED).c_str());
	FAssertMsg(!strcmp(GC.getForceControlInfo(FORCECONTROL_HANDICAP).getType(), "FORCECONTROL_HANDICAP"), CvString::format("XML error. Found %s instead of FORCECONTROL_HANDICAP at index %d", GC.getForceControlInfo(FORCECONTROL_HANDICAP).getType(), FORCECONTROL_HANDICAP).c_str());
	FAssertMsg(!strcmp(GC.getForceControlInfo(FORCECONTROL_OPTIONS).getType(), "FORCECONTROL_OPTIONS"), CvString::format("XML error. Found %s instead of FORCECONTROL_OPTIONS at index %d", GC.getForceControlInfo(FORCECONTROL_OPTIONS).getType(), FORCECONTROL_OPTIONS).c_str());
	FAssertMsg(!strcmp(GC.getForceControlInfo(FORCECONTROL_VICTORIES).getType(), "FORCECONTROL_VICTORIES"), CvString::format("XML error. Found %s instead of FORCECONTROL_VICTORIES at index %d", GC.getForceControlInfo(FORCECONTROL_VICTORIES).getType(), FORCECONTROL_VICTORIES).c_str());
	FAssertMsg(!strcmp(GC.getForceControlInfo(FORCECONTROL_MAX_TURNS).getType(), "FORCECONTROL_MAX_TURNS"), CvString::format("XML error. Found %s instead of FORCECONTROL_MAX_TURNS at index %d", GC.getForceControlInfo(FORCECONTROL_MAX_TURNS).getType(), FORCECONTROL_MAX_TURNS).c_str());
	FAssertMsg(!strcmp(GC.getForceControlInfo(FORCECONTROL_MAX_CITY_ELIMINATION).getType(), "FORCECONTROL_MAX_CITY_ELIMINATION"), CvString::format("XML error. Found %s instead of FORCECONTROL_MAX_CITY_ELIMINATION at index %d", GC.getForceControlInfo(FORCECONTROL_MAX_CITY_ELIMINATION).getType(), FORCECONTROL_MAX_CITY_ELIMINATION).c_str());
	FAssertMsg(!strcmp(GC.getForceControlInfo(FORCECONTROL_ADVANCED_START).getType(), "FORCECONTROL_ADVANCED_START"), CvString::format("XML error. Found %s instead of FORCECONTROL_ADVANCED_START at index %d", GC.getForceControlInfo(FORCECONTROL_ADVANCED_START).getType(), FORCECONTROL_ADVANCED_START).c_str());

	FAssertMsg(GC.getForceControlInfo().size() == NUM_FORCECONTROL_TYPES, CvString::format("XML error. Expected %d types, but found %d", NUM_FORCECONTROL_TYPES, GC.getForceControlInfo().size()));
}

void CvGlobals::CheckEnumDenialTypes() const
{
	FAssertMsg(!strcmp(GC.getDenialInfo(DENIAL_UNKNOWN).getType(), "DENIAL_UNKNOWN"), CvString::format("XML error. Found %s instead of DENIAL_UNKNOWN at index %d", GC.getDenialInfo(DENIAL_UNKNOWN).getType(), DENIAL_UNKNOWN).c_str());
	FAssertMsg(!strcmp(GC.getDenialInfo(DENIAL_NEVER).getType(), "DENIAL_NEVER"), CvString::format("XML error. Found %s instead of DENIAL_NEVER at index %d", GC.getDenialInfo(DENIAL_NEVER).getType(), DENIAL_NEVER).c_str());
	FAssertMsg(!strcmp(GC.getDenialInfo(DENIAL_TOO_MUCH).getType(), "DENIAL_TOO_MUCH"), CvString::format("XML error. Found %s instead of DENIAL_TOO_MUCH at index %d", GC.getDenialInfo(DENIAL_TOO_MUCH).getType(), DENIAL_TOO_MUCH).c_str());
	FAssertMsg(!strcmp(GC.getDenialInfo(DENIAL_MYSTERY).getType(), "DENIAL_MYSTERY"), CvString::format("XML error. Found %s instead of DENIAL_MYSTERY at index %d", GC.getDenialInfo(DENIAL_MYSTERY).getType(), DENIAL_MYSTERY).c_str());
	FAssertMsg(!strcmp(GC.getDenialInfo(DENIAL_JOKING).getType(), "DENIAL_JOKING"), CvString::format("XML error. Found %s instead of DENIAL_JOKING at index %d", GC.getDenialInfo(DENIAL_JOKING).getType(), DENIAL_JOKING).c_str());
	FAssertMsg(!strcmp(GC.getDenialInfo(DENIAL_CONTACT_THEM).getType(), "DENIAL_CONTACT_THEM"), CvString::format("XML error. Found %s instead of DENIAL_CONTACT_THEM at index %d", GC.getDenialInfo(DENIAL_CONTACT_THEM).getType(), DENIAL_CONTACT_THEM).c_str());
	FAssertMsg(!strcmp(GC.getDenialInfo(DENIAL_VICTORY).getType(), "DENIAL_VICTORY"), CvString::format("XML error. Found %s instead of DENIAL_VICTORY at index %d", GC.getDenialInfo(DENIAL_VICTORY).getType(), DENIAL_VICTORY).c_str());
	FAssertMsg(!strcmp(GC.getDenialInfo(DENIAL_ATTITUDE).getType(), "DENIAL_ATTITUDE"), CvString::format("XML error. Found %s instead of DENIAL_ATTITUDE at index %d", GC.getDenialInfo(DENIAL_ATTITUDE).getType(), DENIAL_ATTITUDE).c_str());
	FAssertMsg(!strcmp(GC.getDenialInfo(DENIAL_ATTITUDE_THEM).getType(), "DENIAL_ATTITUDE_THEM"), CvString::format("XML error. Found %s instead of DENIAL_ATTITUDE_THEM at index %d", GC.getDenialInfo(DENIAL_ATTITUDE_THEM).getType(), DENIAL_ATTITUDE_THEM).c_str());
	FAssertMsg(!strcmp(GC.getDenialInfo(DENIAL_POWER_US).getType(), "DENIAL_POWER_US"), CvString::format("XML error. Found %s instead of DENIAL_POWER_US at index %d", GC.getDenialInfo(DENIAL_POWER_US).getType(), DENIAL_POWER_US).c_str());
	FAssertMsg(!strcmp(GC.getDenialInfo(DENIAL_POWER_YOU).getType(), "DENIAL_POWER_YOU"), CvString::format("XML error. Found %s instead of DENIAL_POWER_YOU at index %d", GC.getDenialInfo(DENIAL_POWER_YOU).getType(), DENIAL_POWER_YOU).c_str());
	FAssertMsg(!strcmp(GC.getDenialInfo(DENIAL_POWER_THEM).getType(), "DENIAL_POWER_THEM"), CvString::format("XML error. Found %s instead of DENIAL_POWER_THEM at index %d", GC.getDenialInfo(DENIAL_POWER_THEM).getType(), DENIAL_POWER_THEM).c_str());
	FAssertMsg(!strcmp(GC.getDenialInfo(DENIAL_TOO_MANY_WARS).getType(), "DENIAL_TOO_MANY_WARS"), CvString::format("XML error. Found %s instead of DENIAL_TOO_MANY_WARS at index %d", GC.getDenialInfo(DENIAL_TOO_MANY_WARS).getType(), DENIAL_TOO_MANY_WARS).c_str());
	FAssertMsg(!strcmp(GC.getDenialInfo(DENIAL_NO_GAIN).getType(), "DENIAL_NO_GAIN"), CvString::format("XML error. Found %s instead of DENIAL_NO_GAIN at index %d", GC.getDenialInfo(DENIAL_NO_GAIN).getType(), DENIAL_NO_GAIN).c_str());
	FAssertMsg(!strcmp(GC.getDenialInfo(DENIAL_NOT_INTERESTED).getType(), "DENIAL_NOT_INTERESTED"), CvString::format("XML error. Found %s instead of DENIAL_NOT_INTERESTED at index %d", GC.getDenialInfo(DENIAL_NOT_INTERESTED).getType(), DENIAL_NOT_INTERESTED).c_str());
	FAssertMsg(!strcmp(GC.getDenialInfo(DENIAL_NOT_ALLIED).getType(), "DENIAL_NOT_ALLIED"), CvString::format("XML error. Found %s instead of DENIAL_NOT_ALLIED at index %d", GC.getDenialInfo(DENIAL_NOT_ALLIED).getType(), DENIAL_NOT_ALLIED).c_str());
	FAssertMsg(!strcmp(GC.getDenialInfo(DENIAL_RECENT_CANCEL).getType(), "DENIAL_RECENT_CANCEL"), CvString::format("XML error. Found %s instead of DENIAL_RECENT_CANCEL at index %d", GC.getDenialInfo(DENIAL_RECENT_CANCEL).getType(), DENIAL_RECENT_CANCEL).c_str());
	FAssertMsg(!strcmp(GC.getDenialInfo(DENIAL_WORST_ENEMY).getType(), "DENIAL_WORST_ENEMY"), CvString::format("XML error. Found %s instead of DENIAL_WORST_ENEMY at index %d", GC.getDenialInfo(DENIAL_WORST_ENEMY).getType(), DENIAL_WORST_ENEMY).c_str());
	FAssertMsg(!strcmp(GC.getDenialInfo(DENIAL_POWER_YOUR_ENEMIES).getType(), "DENIAL_POWER_YOUR_ENEMIES"), CvString::format("XML error. Found %s instead of DENIAL_POWER_YOUR_ENEMIES at index %d", GC.getDenialInfo(DENIAL_POWER_YOUR_ENEMIES).getType(), DENIAL_POWER_YOUR_ENEMIES).c_str());
	FAssertMsg(!strcmp(GC.getDenialInfo(DENIAL_TOO_FAR).getType(), "DENIAL_TOO_FAR"), CvString::format("XML error. Found %s instead of DENIAL_TOO_FAR at index %d", GC.getDenialInfo(DENIAL_TOO_FAR).getType(), DENIAL_TOO_FAR).c_str());
	FAssertMsg(!strcmp(GC.getDenialInfo(DENIAL_WAR_NOT_POSSIBLE_US).getType(), "DENIAL_WAR_NOT_POSSIBLE_US"), CvString::format("XML error. Found %s instead of DENIAL_WAR_NOT_POSSIBLE_US at index %d", GC.getDenialInfo(DENIAL_WAR_NOT_POSSIBLE_US).getType(), DENIAL_WAR_NOT_POSSIBLE_US).c_str());
	FAssertMsg(!strcmp(GC.getDenialInfo(DENIAL_WAR_NOT_POSSIBLE_YOU).getType(), "DENIAL_WAR_NOT_POSSIBLE_YOU"), CvString::format("XML error. Found %s instead of DENIAL_WAR_NOT_POSSIBLE_YOU at index %d", GC.getDenialInfo(DENIAL_WAR_NOT_POSSIBLE_YOU).getType(), DENIAL_WAR_NOT_POSSIBLE_YOU).c_str());
	FAssertMsg(!strcmp(GC.getDenialInfo(DENIAL_PEACE_NOT_POSSIBLE_US).getType(), "DENIAL_PEACE_NOT_POSSIBLE_US"), CvString::format("XML error. Found %s instead of DENIAL_PEACE_NOT_POSSIBLE_US at index %d", GC.getDenialInfo(DENIAL_PEACE_NOT_POSSIBLE_US).getType(), DENIAL_PEACE_NOT_POSSIBLE_US).c_str());
	FAssertMsg(!strcmp(GC.getDenialInfo(DENIAL_PEACE_NOT_POSSIBLE_YOU).getType(), "DENIAL_PEACE_NOT_POSSIBLE_YOU"), CvString::format("XML error. Found %s instead of DENIAL_PEACE_NOT_POSSIBLE_YOU at index %d", GC.getDenialInfo(DENIAL_PEACE_NOT_POSSIBLE_YOU).getType(), DENIAL_PEACE_NOT_POSSIBLE_YOU).c_str());
	FAssertMsg(!strcmp(GC.getDenialInfo(DENIAL_CULTURE_YOU).getType(), "DENIAL_CULTURE_YOU"), CvString::format("XML error. Found %s instead of DENIAL_CULTURE_YOU at index %d", GC.getDenialInfo(DENIAL_CULTURE_YOU).getType(), DENIAL_CULTURE_YOU).c_str());

	FAssertMsg(GC.getDenialInfo().size() == NUM_DENIAL_TYPES, CvString::format("XML error. Expected %d types, but found %d", NUM_DENIAL_TYPES, GC.getDenialInfo().size()));
}

void CvGlobals::CheckEnumDomainTypes() const
{
	FAssertMsg(!strcmp(GC.getDomainInfo(DOMAIN_SEA).getType(), "DOMAIN_SEA"), CvString::format("XML error. Found %s instead of DOMAIN_SEA at index %d", GC.getDomainInfo(DOMAIN_SEA).getType(), DOMAIN_SEA).c_str());
	FAssertMsg(!strcmp(GC.getDomainInfo(DOMAIN_LAND).getType(), "DOMAIN_LAND"), CvString::format("XML error. Found %s instead of DOMAIN_LAND at index %d", GC.getDomainInfo(DOMAIN_LAND).getType(), DOMAIN_LAND).c_str());
	FAssertMsg(!strcmp(GC.getDomainInfo(DOMAIN_IMMOBILE).getType(), "DOMAIN_IMMOBILE"), CvString::format("XML error. Found %s instead of DOMAIN_IMMOBILE at index %d", GC.getDomainInfo(DOMAIN_IMMOBILE).getType(), DOMAIN_IMMOBILE).c_str());

	FAssertMsg(GC.getDomainInfo().size() == NUM_DOMAIN_TYPES, CvString::format("XML error. Expected %d types, but found %d", NUM_DOMAIN_TYPES, GC.getDomainInfo().size()));
}

void CvGlobals::CheckEnumUnitAITypes() const
{
	FAssertMsg(!strcmp(GC.getUnitAIInfo(UNITAI_UNKNOWN).getType(), "UNITAI_UNKNOWN"), CvString::format("XML error. Found %s instead of UNITAI_UNKNOWN at index %d", GC.getUnitAIInfo(UNITAI_UNKNOWN).getType(), UNITAI_UNKNOWN).c_str());
	FAssertMsg(!strcmp(GC.getUnitAIInfo(UNITAI_ANIMAL).getType(), "UNITAI_ANIMAL"), CvString::format("XML error. Found %s instead of UNITAI_ANIMAL at index %d", GC.getUnitAIInfo(UNITAI_ANIMAL).getType(), UNITAI_ANIMAL).c_str());
	FAssertMsg(!strcmp(GC.getUnitAIInfo(UNITAI_COLONIST).getType(), "UNITAI_COLONIST"), CvString::format("XML error. Found %s instead of UNITAI_COLONIST at index %d", GC.getUnitAIInfo(UNITAI_COLONIST).getType(), UNITAI_COLONIST).c_str());
	FAssertMsg(!strcmp(GC.getUnitAIInfo(UNITAI_SETTLER).getType(), "UNITAI_SETTLER"), CvString::format("XML error. Found %s instead of UNITAI_SETTLER at index %d", GC.getUnitAIInfo(UNITAI_SETTLER).getType(), UNITAI_SETTLER).c_str());
	FAssertMsg(!strcmp(GC.getUnitAIInfo(UNITAI_WORKER).getType(), "UNITAI_WORKER"), CvString::format("XML error. Found %s instead of UNITAI_WORKER at index %d", GC.getUnitAIInfo(UNITAI_WORKER).getType(), UNITAI_WORKER).c_str());
	FAssertMsg(!strcmp(GC.getUnitAIInfo(UNITAI_MISSIONARY).getType(), "UNITAI_MISSIONARY"), CvString::format("XML error. Found %s instead of UNITAI_MISSIONARY at index %d", GC.getUnitAIInfo(UNITAI_MISSIONARY).getType(), UNITAI_MISSIONARY).c_str());
	FAssertMsg(!strcmp(GC.getUnitAIInfo(UNITAI_SCOUT).getType(), "UNITAI_SCOUT"), CvString::format("XML error. Found %s instead of UNITAI_SCOUT at index %d", GC.getUnitAIInfo(UNITAI_SCOUT).getType(), UNITAI_SCOUT).c_str());
	FAssertMsg(!strcmp(GC.getUnitAIInfo(UNITAI_WAGON).getType(), "UNITAI_WAGON"), CvString::format("XML error. Found %s instead of UNITAI_WAGON at index %d", GC.getUnitAIInfo(UNITAI_WAGON).getType(), UNITAI_WAGON).c_str());
	FAssertMsg(!strcmp(GC.getUnitAIInfo(UNITAI_TREASURE).getType(), "UNITAI_TREASURE"), CvString::format("XML error. Found %s instead of UNITAI_TREASURE at index %d", GC.getUnitAIInfo(UNITAI_TREASURE).getType(), UNITAI_TREASURE).c_str());
	FAssertMsg(!strcmp(GC.getUnitAIInfo(UNITAI_YIELD).getType(), "UNITAI_YIELD"), CvString::format("XML error. Found %s instead of UNITAI_YIELD at index %d", GC.getUnitAIInfo(UNITAI_YIELD).getType(), UNITAI_YIELD).c_str());
	FAssertMsg(!strcmp(GC.getUnitAIInfo(UNITAI_GENERAL).getType(), "UNITAI_GENERAL"), CvString::format("XML error. Found %s instead of UNITAI_GENERAL at index %d", GC.getUnitAIInfo(UNITAI_GENERAL).getType(), UNITAI_GENERAL).c_str());
	FAssertMsg(!strcmp(GC.getUnitAIInfo(UNITAI_DEFENSIVE).getType(), "UNITAI_DEFENSIVE"), CvString::format("XML error. Found %s instead of UNITAI_DEFENSIVE at index %d", GC.getUnitAIInfo(UNITAI_DEFENSIVE).getType(), UNITAI_DEFENSIVE).c_str());
	FAssertMsg(!strcmp(GC.getUnitAIInfo(UNITAI_OFFENSIVE).getType(), "UNITAI_OFFENSIVE"), CvString::format("XML error. Found %s instead of UNITAI_OFFENSIVE at index %d", GC.getUnitAIInfo(UNITAI_OFFENSIVE).getType(), UNITAI_OFFENSIVE).c_str());
	FAssertMsg(!strcmp(GC.getUnitAIInfo(UNITAI_COUNTER).getType(), "UNITAI_COUNTER"), CvString::format("XML error. Found %s instead of UNITAI_COUNTER at index %d", GC.getUnitAIInfo(UNITAI_COUNTER).getType(), UNITAI_COUNTER).c_str());
	FAssertMsg(!strcmp(GC.getUnitAIInfo(UNITAI_TRANSPORT_SEA).getType(), "UNITAI_TRANSPORT_SEA"), CvString::format("XML error. Found %s instead of UNITAI_TRANSPORT_SEA at index %d", GC.getUnitAIInfo(UNITAI_TRANSPORT_SEA).getType(), UNITAI_TRANSPORT_SEA).c_str());
	FAssertMsg(!strcmp(GC.getUnitAIInfo(UNITAI_ASSAULT_SEA).getType(), "UNITAI_ASSAULT_SEA"), CvString::format("XML error. Found %s instead of UNITAI_ASSAULT_SEA at index %d", GC.getUnitAIInfo(UNITAI_ASSAULT_SEA).getType(), UNITAI_ASSAULT_SEA).c_str());
	FAssertMsg(!strcmp(GC.getUnitAIInfo(UNITAI_COMBAT_SEA).getType(), "UNITAI_COMBAT_SEA"), CvString::format("XML error. Found %s instead of UNITAI_COMBAT_SEA at index %d", GC.getUnitAIInfo(UNITAI_COMBAT_SEA).getType(), UNITAI_COMBAT_SEA).c_str());
	FAssertMsg(!strcmp(GC.getUnitAIInfo(UNITAI_PIRATE_SEA).getType(), "UNITAI_PIRATE_SEA"), CvString::format("XML error. Found %s instead of UNITAI_PIRATE_SEA at index %d", GC.getUnitAIInfo(UNITAI_PIRATE_SEA).getType(), UNITAI_PIRATE_SEA).c_str());
	FAssertMsg(!strcmp(GC.getUnitAIInfo(UNITAI_TRADER).getType(), "UNITAI_TRADER"), CvString::format("XML error. Found %s instead of UNITAI_TRADER at index %d", GC.getUnitAIInfo(UNITAI_TRADER).getType(), UNITAI_TRADER).c_str());
	FAssertMsg(!strcmp(GC.getUnitAIInfo(UNITAI_HUNTSMAN).getType(), "UNITAI_HUNTSMAN"), CvString::format("XML error. Found %s instead of UNITAI_HUNTSMAN at index %d", GC.getUnitAIInfo(UNITAI_HUNTSMAN).getType(), UNITAI_HUNTSMAN).c_str());
	FAssertMsg(!strcmp(GC.getUnitAIInfo(UNITAI_MARAUDER).getType(), "UNITAI_MARAUDER"), CvString::format("XML error. Found %s instead of UNITAI_MARAUDER at index %d", GC.getUnitAIInfo(UNITAI_MARAUDER).getType(), UNITAI_MARAUDER).c_str());

	FAssertMsg(GC.getUnitAIInfo().size() == NUM_UNITAI_TYPES, CvString::format("XML error. Expected %d types, but found %d", NUM_UNITAI_TYPES, GC.getUnitAIInfo().size()));
}

void CvGlobals::CheckEnumMissionTypes() const
{
	FAssertMsg(!strcmp(GC.getMissionInfo(MISSION_MOVE_TO).getType(), "MISSION_MOVE_TO"), CvString::format("XML error. Found %s instead of MISSION_MOVE_TO at index %d", GC.getMissionInfo(MISSION_MOVE_TO).getType(), MISSION_MOVE_TO).c_str());
	FAssertMsg(!strcmp(GC.getMissionInfo(MISSION_ROUTE_TO).getType(), "MISSION_ROUTE_TO"), CvString::format("XML error. Found %s instead of MISSION_ROUTE_TO at index %d", GC.getMissionInfo(MISSION_ROUTE_TO).getType(), MISSION_ROUTE_TO).c_str());
	FAssertMsg(!strcmp(GC.getMissionInfo(MISSION_MOVE_TO_UNIT).getType(), "MISSION_MOVE_TO_UNIT"), CvString::format("XML error. Found %s instead of MISSION_MOVE_TO_UNIT at index %d", GC.getMissionInfo(MISSION_MOVE_TO_UNIT).getType(), MISSION_MOVE_TO_UNIT).c_str());
	FAssertMsg(!strcmp(GC.getMissionInfo(MISSION_SKIP).getType(), "MISSION_SKIP"), CvString::format("XML error. Found %s instead of MISSION_SKIP at index %d", GC.getMissionInfo(MISSION_SKIP).getType(), MISSION_SKIP).c_str());
	FAssertMsg(!strcmp(GC.getMissionInfo(MISSION_SLEEP).getType(), "MISSION_SLEEP"), CvString::format("XML error. Found %s instead of MISSION_SLEEP at index %d", GC.getMissionInfo(MISSION_SLEEP).getType(), MISSION_SLEEP).c_str());
	FAssertMsg(!strcmp(GC.getMissionInfo(MISSION_FORTIFY).getType(), "MISSION_FORTIFY"), CvString::format("XML error. Found %s instead of MISSION_FORTIFY at index %d", GC.getMissionInfo(MISSION_FORTIFY).getType(), MISSION_FORTIFY).c_str());
	FAssertMsg(!strcmp(GC.getMissionInfo(MISSION_HEAL).getType(), "MISSION_HEAL"), CvString::format("XML error. Found %s instead of MISSION_HEAL at index %d", GC.getMissionInfo(MISSION_HEAL).getType(), MISSION_HEAL).c_str());
	FAssertMsg(!strcmp(GC.getMissionInfo(MISSION_SENTRY).getType(), "MISSION_SENTRY"), CvString::format("XML error. Found %s instead of MISSION_SENTRY at index %d", GC.getMissionInfo(MISSION_SENTRY).getType(), MISSION_SENTRY).c_str());
	FAssertMsg(!strcmp(GC.getMissionInfo(MISSION_BOMBARD).getType(), "MISSION_BOMBARD"), CvString::format("XML error. Found %s instead of MISSION_BOMBARD at index %d", GC.getMissionInfo(MISSION_BOMBARD).getType(), MISSION_BOMBARD).c_str());
	FAssertMsg(!strcmp(GC.getMissionInfo(MISSION_PILLAGE).getType(), "MISSION_PILLAGE"), CvString::format("XML error. Found %s instead of MISSION_PILLAGE at index %d", GC.getMissionInfo(MISSION_PILLAGE).getType(), MISSION_PILLAGE).c_str());
	FAssertMsg(!strcmp(GC.getMissionInfo(MISSION_FOUND).getType(), "MISSION_FOUND"), CvString::format("XML error. Found %s instead of MISSION_FOUND at index %d", GC.getMissionInfo(MISSION_FOUND).getType(), MISSION_FOUND).c_str());
	FAssertMsg(!strcmp(GC.getMissionInfo(MISSION_JOIN_CITY).getType(), "MISSION_JOIN_CITY"), CvString::format("XML error. Found %s instead of MISSION_JOIN_CITY at index %d", GC.getMissionInfo(MISSION_JOIN_CITY).getType(), MISSION_JOIN_CITY).c_str());
	FAssertMsg(!strcmp(GC.getMissionInfo(MISSION_BUILD).getType(), "MISSION_BUILD"), CvString::format("XML error. Found %s instead of MISSION_BUILD at index %d", GC.getMissionInfo(MISSION_BUILD).getType(), MISSION_BUILD).c_str());
	FAssertMsg(!strcmp(GC.getMissionInfo(MISSION_LEAD).getType(), "MISSION_LEAD"), CvString::format("XML error. Found %s instead of MISSION_LEAD at index %d", GC.getMissionInfo(MISSION_LEAD).getType(), MISSION_LEAD).c_str());
	FAssertMsg(!strcmp(GC.getMissionInfo(MISSION_BEGIN_COMBAT).getType(), "MISSION_BEGIN_COMBAT"), CvString::format("XML error. Found %s instead of MISSION_BEGIN_COMBAT at index %d", GC.getMissionInfo(MISSION_BEGIN_COMBAT).getType(), MISSION_BEGIN_COMBAT).c_str());
	FAssertMsg(!strcmp(GC.getMissionInfo(MISSION_END_COMBAT).getType(), "MISSION_END_COMBAT"), CvString::format("XML error. Found %s instead of MISSION_END_COMBAT at index %d", GC.getMissionInfo(MISSION_END_COMBAT).getType(), MISSION_END_COMBAT).c_str());
	FAssertMsg(!strcmp(GC.getMissionInfo(MISSION_SURRENDER).getType(), "MISSION_SURRENDER"), CvString::format("XML error. Found %s instead of MISSION_SURRENDER at index %d", GC.getMissionInfo(MISSION_SURRENDER).getType(), MISSION_SURRENDER).c_str());
	FAssertMsg(!strcmp(GC.getMissionInfo(MISSION_CAPTURED).getType(), "MISSION_CAPTURED"), CvString::format("XML error. Found %s instead of MISSION_CAPTURED at index %d", GC.getMissionInfo(MISSION_CAPTURED).getType(), MISSION_CAPTURED).c_str());
	FAssertMsg(!strcmp(GC.getMissionInfo(MISSION_IDLE).getType(), "MISSION_IDLE"), CvString::format("XML error. Found %s instead of MISSION_IDLE at index %d", GC.getMissionInfo(MISSION_IDLE).getType(), MISSION_IDLE).c_str());
	FAssertMsg(!strcmp(GC.getMissionInfo(MISSION_DIE).getType(), "MISSION_DIE"), CvString::format("XML error. Found %s instead of MISSION_DIE at index %d", GC.getMissionInfo(MISSION_DIE).getType(), MISSION_DIE).c_str());
	FAssertMsg(!strcmp(GC.getMissionInfo(MISSION_DAMAGE).getType(), "MISSION_DAMAGE"), CvString::format("XML error. Found %s instead of MISSION_DAMAGE at index %d", GC.getMissionInfo(MISSION_DAMAGE).getType(), MISSION_DAMAGE).c_str());
	FAssertMsg(!strcmp(GC.getMissionInfo(MISSION_MULTI_SELECT).getType(), "MISSION_MULTI_SELECT"), CvString::format("XML error. Found %s instead of MISSION_MULTI_SELECT at index %d", GC.getMissionInfo(MISSION_MULTI_SELECT).getType(), MISSION_MULTI_SELECT).c_str());
	FAssertMsg(!strcmp(GC.getMissionInfo(MISSION_MULTI_DESELECT).getType(), "MISSION_MULTI_DESELECT"), CvString::format("XML error. Found %s instead of MISSION_MULTI_DESELECT at index %d", GC.getMissionInfo(MISSION_MULTI_DESELECT).getType(), MISSION_MULTI_DESELECT).c_str());
	FAssertMsg(!strcmp(GC.getMissionInfo(MISSION_FOUND_MONASTERY).getType(), "MISSION_FOUND_MONASTERY"), CvString::format("XML error. Found %s instead of MISSION_FOUND_MONASTERY at index %d", GC.getMissionInfo(MISSION_FOUND_MONASTERY).getType(), MISSION_FOUND_MONASTERY).c_str());
	FAssertMsg(!strcmp(GC.getMissionInfo(MISSION_FOUND_OUTPOST).getType(), "MISSION_FOUND_OUTPOST"), CvString::format("XML error. Found %s instead of MISSION_FOUND_OUTPOST at index %d", GC.getMissionInfo(MISSION_FOUND_OUTPOST).getType(), MISSION_FOUND_OUTPOST).c_str());
	FAssertMsg(!strcmp(GC.getMissionInfo(MISSION_COLLECT_TAXES).getType(), "MISSION_COLLECT_TAXES"), CvString::format("XML error. Found %s instead of MISSION_COLLECT_TAXES at index %d", GC.getMissionInfo(MISSION_COLLECT_TAXES).getType(), MISSION_COLLECT_TAXES).c_str());
	FAssertMsg(!strcmp(GC.getMissionInfo(MISSION_HUNT).getType(), "MISSION_HUNT"), CvString::format("XML error. Found %s instead of MISSION_HUNT at index %d", GC.getMissionInfo(MISSION_HUNT).getType(), MISSION_HUNT).c_str());

	FAssertMsg(GC.getMissionInfo().size() == NUM_MISSION_TYPES, CvString::format("XML error. Expected %d types, but found %d", NUM_MISSION_TYPES, GC.getMissionInfo().size()));
}

void CvGlobals::CheckEnumCommandTypes() const
{
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_PROMOTION).getType(), "COMMAND_PROMOTION"), CvString::format("XML error. Found %s instead of COMMAND_PROMOTION at index %d", GC.getCommandInfo(COMMAND_PROMOTION).getType(), COMMAND_PROMOTION).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_UPGRADE).getType(), "COMMAND_UPGRADE"), CvString::format("XML error. Found %s instead of COMMAND_UPGRADE at index %d", GC.getCommandInfo(COMMAND_UPGRADE).getType(), COMMAND_UPGRADE).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_AUTOMATE).getType(), "COMMAND_AUTOMATE"), CvString::format("XML error. Found %s instead of COMMAND_AUTOMATE at index %d", GC.getCommandInfo(COMMAND_AUTOMATE).getType(), COMMAND_AUTOMATE).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_WAKE).getType(), "COMMAND_WAKE"), CvString::format("XML error. Found %s instead of COMMAND_WAKE at index %d", GC.getCommandInfo(COMMAND_WAKE).getType(), COMMAND_WAKE).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_CANCEL).getType(), "COMMAND_CANCEL"), CvString::format("XML error. Found %s instead of COMMAND_CANCEL at index %d", GC.getCommandInfo(COMMAND_CANCEL).getType(), COMMAND_CANCEL).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_CANCEL_ALL).getType(), "COMMAND_CANCEL_ALL"), CvString::format("XML error. Found %s instead of COMMAND_CANCEL_ALL at index %d", GC.getCommandInfo(COMMAND_CANCEL_ALL).getType(), COMMAND_CANCEL_ALL).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_STOP_AUTOMATION).getType(), "COMMAND_STOP_AUTOMATION"), CvString::format("XML error. Found %s instead of COMMAND_STOP_AUTOMATION at index %d", GC.getCommandInfo(COMMAND_STOP_AUTOMATION).getType(), COMMAND_STOP_AUTOMATION).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_DELETE).getType(), "COMMAND_DELETE"), CvString::format("XML error. Found %s instead of COMMAND_DELETE at index %d", GC.getCommandInfo(COMMAND_DELETE).getType(), COMMAND_DELETE).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_GIFT).getType(), "COMMAND_GIFT"), CvString::format("XML error. Found %s instead of COMMAND_GIFT at index %d", GC.getCommandInfo(COMMAND_GIFT).getType(), COMMAND_GIFT).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_LOAD).getType(), "COMMAND_LOAD"), CvString::format("XML error. Found %s instead of COMMAND_LOAD at index %d", GC.getCommandInfo(COMMAND_LOAD).getType(), COMMAND_LOAD).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_LOAD_UNIT).getType(), "COMMAND_LOAD_UNIT"), CvString::format("XML error. Found %s instead of COMMAND_LOAD_UNIT at index %d", GC.getCommandInfo(COMMAND_LOAD_UNIT).getType(), COMMAND_LOAD_UNIT).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_LOAD_CARGO).getType(), "COMMAND_LOAD_CARGO"), CvString::format("XML error. Found %s instead of COMMAND_LOAD_CARGO at index %d", GC.getCommandInfo(COMMAND_LOAD_CARGO).getType(), COMMAND_LOAD_CARGO).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_LOAD_YIELD).getType(), "COMMAND_LOAD_YIELD"), CvString::format("XML error. Found %s instead of COMMAND_LOAD_YIELD at index %d", GC.getCommandInfo(COMMAND_LOAD_YIELD).getType(), COMMAND_LOAD_YIELD).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_UNLOAD).getType(), "COMMAND_UNLOAD"), CvString::format("XML error. Found %s instead of COMMAND_UNLOAD at index %d", GC.getCommandInfo(COMMAND_UNLOAD).getType(), COMMAND_UNLOAD).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_UNLOAD_ALL).getType(), "COMMAND_UNLOAD_ALL"), CvString::format("XML error. Found %s instead of COMMAND_UNLOAD_ALL at index %d", GC.getCommandInfo(COMMAND_UNLOAD_ALL).getType(), COMMAND_UNLOAD_ALL).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_LEARN).getType(), "COMMAND_LEARN"), CvString::format("XML error. Found %s instead of COMMAND_LEARN at index %d", GC.getCommandInfo(COMMAND_LEARN).getType(), COMMAND_LEARN).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_KING_TRANSPORT).getType(), "COMMAND_KING_TRANSPORT"), CvString::format("XML error. Found %s instead of COMMAND_KING_TRANSPORT at index %d", GC.getCommandInfo(COMMAND_KING_TRANSPORT).getType(), COMMAND_KING_TRANSPORT).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_ESTABLISH_MISSION).getType(), "COMMAND_ESTABLISH_MISSION"), CvString::format("XML error. Found %s instead of COMMAND_ESTABLISH_MISSION at index %d", GC.getCommandInfo(COMMAND_ESTABLISH_MISSION).getType(), COMMAND_ESTABLISH_MISSION).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_SPEAK_WITH_CHIEF).getType(), "COMMAND_SPEAK_WITH_CHIEF"), CvString::format("XML error. Found %s instead of COMMAND_SPEAK_WITH_CHIEF at index %d", GC.getCommandInfo(COMMAND_SPEAK_WITH_CHIEF).getType(), COMMAND_SPEAK_WITH_CHIEF).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_YIELD_TRADE).getType(), "COMMAND_YIELD_TRADE"), CvString::format("XML error. Found %s instead of COMMAND_YIELD_TRADE at index %d", GC.getCommandInfo(COMMAND_YIELD_TRADE).getType(), COMMAND_YIELD_TRADE).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_SAIL_TO_EUROPE).getType(), "COMMAND_SAIL_TO_EUROPE"), CvString::format("XML error. Found %s instead of COMMAND_SAIL_TO_EUROPE at index %d", GC.getCommandInfo(COMMAND_SAIL_TO_EUROPE).getType(), COMMAND_SAIL_TO_EUROPE).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_CHOOSE_TRADE_ROUTES).getType(), "COMMAND_CHOOSE_TRADE_ROUTES"), CvString::format("XML error. Found %s instead of COMMAND_CHOOSE_TRADE_ROUTES at index %d", GC.getCommandInfo(COMMAND_CHOOSE_TRADE_ROUTES).getType(), COMMAND_CHOOSE_TRADE_ROUTES).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_ASSIGN_TRADE_ROUTE).getType(), "COMMAND_ASSIGN_TRADE_ROUTE"), CvString::format("XML error. Found %s instead of COMMAND_ASSIGN_TRADE_ROUTE at index %d", GC.getCommandInfo(COMMAND_ASSIGN_TRADE_ROUTE).getType(), COMMAND_ASSIGN_TRADE_ROUTE).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_PROMOTE).getType(), "COMMAND_PROMOTE"), CvString::format("XML error. Found %s instead of COMMAND_PROMOTE at index %d", GC.getCommandInfo(COMMAND_PROMOTE).getType(), COMMAND_PROMOTE).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_PROFESSION).getType(), "COMMAND_PROFESSION"), CvString::format("XML error. Found %s instead of COMMAND_PROFESSION at index %d", GC.getCommandInfo(COMMAND_PROFESSION).getType(), COMMAND_PROFESSION).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_CLEAR_SPECIALTY).getType(), "COMMAND_CLEAR_SPECIALTY"), CvString::format("XML error. Found %s instead of COMMAND_CLEAR_SPECIALTY at index %d", GC.getCommandInfo(COMMAND_CLEAR_SPECIALTY).getType(), COMMAND_CLEAR_SPECIALTY).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_HOTKEY).getType(), "COMMAND_HOTKEY"), CvString::format("XML error. Found %s instead of COMMAND_HOTKEY at index %d", GC.getCommandInfo(COMMAND_HOTKEY).getType(), COMMAND_HOTKEY).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_CONVERT_UNIT).getType(), "COMMAND_CONVERT_UNIT"), CvString::format("XML error. Found %s instead of COMMAND_CONVERT_UNIT at index %d", GC.getCommandInfo(COMMAND_CONVERT_UNIT).getType(), COMMAND_CONVERT_UNIT).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_HOLD_FEAST).getType(), "COMMAND_HOLD_FEAST"), CvString::format("XML error. Found %s instead of COMMAND_HOLD_FEAST at index %d", GC.getCommandInfo(COMMAND_HOLD_FEAST).getType(), COMMAND_HOLD_FEAST).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_TRAVEL_TO_FAIR).getType(), "COMMAND_TRAVEL_TO_FAIR"), CvString::format("XML error. Found %s instead of COMMAND_TRAVEL_TO_FAIR at index %d", GC.getCommandInfo(COMMAND_TRAVEL_TO_FAIR).getType(), COMMAND_TRAVEL_TO_FAIR).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_SAIL_SPICE_ROUTE).getType(), "COMMAND_SAIL_SPICE_ROUTE"), CvString::format("XML error. Found %s instead of COMMAND_SAIL_SPICE_ROUTE at index %d", GC.getCommandInfo(COMMAND_SAIL_SPICE_ROUTE).getType(), COMMAND_SAIL_SPICE_ROUTE).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_TRAVEL_SILK_ROAD).getType(), "COMMAND_TRAVEL_SILK_ROAD"), CvString::format("XML error. Found %s instead of COMMAND_TRAVEL_SILK_ROAD at index %d", GC.getCommandInfo(COMMAND_TRAVEL_SILK_ROAD).getType(), COMMAND_TRAVEL_SILK_ROAD).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_ASSIGN_HOME_CITY).getType(), "COMMAND_ASSIGN_HOME_CITY"), CvString::format("XML error. Found %s instead of COMMAND_ASSIGN_HOME_CITY at index %d", GC.getCommandInfo(COMMAND_ASSIGN_HOME_CITY).getType(), COMMAND_ASSIGN_HOME_CITY).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_HIRE_GUARD).getType(), "COMMAND_HIRE_GUARD"), CvString::format("XML error. Found %s instead of COMMAND_HIRE_GUARD at index %d", GC.getCommandInfo(COMMAND_HIRE_GUARD).getType(), COMMAND_HIRE_GUARD).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_CALL_BANNERS).getType(), "COMMAND_CALL_BANNERS"), CvString::format("XML error. Found %s instead of COMMAND_CALL_BANNERS at index %d", GC.getCommandInfo(COMMAND_CALL_BANNERS).getType(), COMMAND_CALL_BANNERS).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_BUILD_HOME).getType(), "COMMAND_BUILD_HOME"), CvString::format("XML error. Found %s instead of COMMAND_BUILD_HOME at index %d", GC.getCommandInfo(COMMAND_BUILD_HOME).getType(), COMMAND_BUILD_HOME).c_str());
	FAssertMsg(!strcmp(GC.getCommandInfo(COMMAND_BUILD_TRADINGPOST).getType(), "COMMAND_BUILD_TRADINGPOST"), CvString::format("XML error. Found %s instead of COMMAND_BUILD_TRADINGPOST at index %d", GC.getCommandInfo(COMMAND_BUILD_TRADINGPOST).getType(), COMMAND_BUILD_TRADINGPOST).c_str());

	FAssertMsg(GC.getCommandInfo().size() == NUM_COMMAND_TYPES, CvString::format("XML error. Expected %d types, but found %d", NUM_COMMAND_TYPES, GC.getCommandInfo().size()));
}

void CvGlobals::CheckEnumControlTypes() const
{
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_CENTERONSELECTION).getType(), "CONTROL_CENTERONSELECTION"), CvString::format("XML error. Found %s instead of CONTROL_CENTERONSELECTION at index %d", GC.getControlInfo(CONTROL_CENTERONSELECTION).getType(), CONTROL_CENTERONSELECTION).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_SELECTYUNITTYPE).getType(), "CONTROL_SELECTYUNITTYPE"), CvString::format("XML error. Found %s instead of CONTROL_SELECTYUNITTYPE at index %d", GC.getControlInfo(CONTROL_SELECTYUNITTYPE).getType(), CONTROL_SELECTYUNITTYPE).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_SELECTYUNITALL).getType(), "CONTROL_SELECTYUNITALL"), CvString::format("XML error. Found %s instead of CONTROL_SELECTYUNITALL at index %d", GC.getControlInfo(CONTROL_SELECTYUNITALL).getType(), CONTROL_SELECTYUNITALL).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_SELECTCITY).getType(), "CONTROL_SELECTCITY"), CvString::format("XML error. Found %s instead of CONTROL_SELECTCITY at index %d", GC.getControlInfo(CONTROL_SELECTCITY).getType(), CONTROL_SELECTCITY).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_SELECTCAPITAL).getType(), "CONTROL_SELECTCAPITAL"), CvString::format("XML error. Found %s instead of CONTROL_SELECTCAPITAL at index %d", GC.getControlInfo(CONTROL_SELECTCAPITAL).getType(), CONTROL_SELECTCAPITAL).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_NEXTCITY).getType(), "CONTROL_NEXTCITY"), CvString::format("XML error. Found %s instead of CONTROL_NEXTCITY at index %d", GC.getControlInfo(CONTROL_NEXTCITY).getType(), CONTROL_NEXTCITY).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_PREVCITY).getType(), "CONTROL_PREVCITY"), CvString::format("XML error. Found %s instead of CONTROL_PREVCITY at index %d", GC.getControlInfo(CONTROL_PREVCITY).getType(), CONTROL_PREVCITY).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_NEXTUNIT).getType(), "CONTROL_NEXTUNIT"), CvString::format("XML error. Found %s instead of CONTROL_NEXTUNIT at index %d", GC.getControlInfo(CONTROL_NEXTUNIT).getType(), CONTROL_NEXTUNIT).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_PREVUNIT).getType(), "CONTROL_PREVUNIT"), CvString::format("XML error. Found %s instead of CONTROL_PREVUNIT at index %d", GC.getControlInfo(CONTROL_PREVUNIT).getType(), CONTROL_PREVUNIT).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_CYCLEUNIT).getType(), "CONTROL_CYCLEUNIT"), CvString::format("XML error. Found %s instead of CONTROL_CYCLEUNIT at index %d", GC.getControlInfo(CONTROL_CYCLEUNIT).getType(), CONTROL_CYCLEUNIT).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_CYCLEUNIT_ALT).getType(), "CONTROL_CYCLEUNIT_ALT"), CvString::format("XML error. Found %s instead of CONTROL_CYCLEUNIT_ALT at index %d", GC.getControlInfo(CONTROL_CYCLEUNIT_ALT).getType(), CONTROL_CYCLEUNIT_ALT).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_LASTUNIT).getType(), "CONTROL_LASTUNIT"), CvString::format("XML error. Found %s instead of CONTROL_LASTUNIT at index %d", GC.getControlInfo(CONTROL_LASTUNIT).getType(), CONTROL_LASTUNIT).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_ENDTURN).getType(), "CONTROL_ENDTURN"), CvString::format("XML error. Found %s instead of CONTROL_ENDTURN at index %d", GC.getControlInfo(CONTROL_ENDTURN).getType(), CONTROL_ENDTURN).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_ENDTURN_ALT).getType(), "CONTROL_ENDTURN_ALT"), CvString::format("XML error. Found %s instead of CONTROL_ENDTURN_ALT at index %d", GC.getControlInfo(CONTROL_ENDTURN_ALT).getType(), CONTROL_ENDTURN_ALT).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_FORCEENDTURN).getType(), "CONTROL_FORCEENDTURN"), CvString::format("XML error. Found %s instead of CONTROL_FORCEENDTURN at index %d", GC.getControlInfo(CONTROL_FORCEENDTURN).getType(), CONTROL_FORCEENDTURN).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_AUTOMOVES).getType(), "CONTROL_AUTOMOVES"), CvString::format("XML error. Found %s instead of CONTROL_AUTOMOVES at index %d", GC.getControlInfo(CONTROL_AUTOMOVES).getType(), CONTROL_AUTOMOVES).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_PING).getType(), "CONTROL_PING"), CvString::format("XML error. Found %s instead of CONTROL_PING at index %d", GC.getControlInfo(CONTROL_PING).getType(), CONTROL_PING).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_SIGN).getType(), "CONTROL_SIGN"), CvString::format("XML error. Found %s instead of CONTROL_SIGN at index %d", GC.getControlInfo(CONTROL_SIGN).getType(), CONTROL_SIGN).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_GRID).getType(), "CONTROL_GRID"), CvString::format("XML error. Found %s instead of CONTROL_GRID at index %d", GC.getControlInfo(CONTROL_GRID).getType(), CONTROL_GRID).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_BARE_MAP).getType(), "CONTROL_BARE_MAP"), CvString::format("XML error. Found %s instead of CONTROL_BARE_MAP at index %d", GC.getControlInfo(CONTROL_BARE_MAP).getType(), CONTROL_BARE_MAP).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_YIELDS).getType(), "CONTROL_YIELDS"), CvString::format("XML error. Found %s instead of CONTROL_YIELDS at index %d", GC.getControlInfo(CONTROL_YIELDS).getType(), CONTROL_YIELDS).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_RESOURCE_ALL).getType(), "CONTROL_RESOURCE_ALL"), CvString::format("XML error. Found %s instead of CONTROL_RESOURCE_ALL at index %d", GC.getControlInfo(CONTROL_RESOURCE_ALL).getType(), CONTROL_RESOURCE_ALL).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_UNIT_ICONS).getType(), "CONTROL_UNIT_ICONS"), CvString::format("XML error. Found %s instead of CONTROL_UNIT_ICONS at index %d", GC.getControlInfo(CONTROL_UNIT_ICONS).getType(), CONTROL_UNIT_ICONS).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_GLOBELAYER).getType(), "CONTROL_GLOBELAYER"), CvString::format("XML error. Found %s instead of CONTROL_GLOBELAYER at index %d", GC.getControlInfo(CONTROL_GLOBELAYER).getType(), CONTROL_GLOBELAYER).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_SCORES).getType(), "CONTROL_SCORES"), CvString::format("XML error. Found %s instead of CONTROL_SCORES at index %d", GC.getControlInfo(CONTROL_SCORES).getType(), CONTROL_SCORES).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_LOAD_GAME).getType(), "CONTROL_LOAD_GAME"), CvString::format("XML error. Found %s instead of CONTROL_LOAD_GAME at index %d", GC.getControlInfo(CONTROL_LOAD_GAME).getType(), CONTROL_LOAD_GAME).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_OPTIONS_SCREEN).getType(), "CONTROL_OPTIONS_SCREEN"), CvString::format("XML error. Found %s instead of CONTROL_OPTIONS_SCREEN at index %d", GC.getControlInfo(CONTROL_OPTIONS_SCREEN).getType(), CONTROL_OPTIONS_SCREEN).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_RETIRE).getType(), "CONTROL_RETIRE"), CvString::format("XML error. Found %s instead of CONTROL_RETIRE at index %d", GC.getControlInfo(CONTROL_RETIRE).getType(), CONTROL_RETIRE).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_SAVE_GROUP).getType(), "CONTROL_SAVE_GROUP"), CvString::format("XML error. Found %s instead of CONTROL_SAVE_GROUP at index %d", GC.getControlInfo(CONTROL_SAVE_GROUP).getType(), CONTROL_SAVE_GROUP).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_SAVE_NORMAL).getType(), "CONTROL_SAVE_NORMAL"), CvString::format("XML error. Found %s instead of CONTROL_SAVE_NORMAL at index %d", GC.getControlInfo(CONTROL_SAVE_NORMAL).getType(), CONTROL_SAVE_NORMAL).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_QUICK_SAVE).getType(), "CONTROL_QUICK_SAVE"), CvString::format("XML error. Found %s instead of CONTROL_QUICK_SAVE at index %d", GC.getControlInfo(CONTROL_QUICK_SAVE).getType(), CONTROL_QUICK_SAVE).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_QUICK_LOAD).getType(), "CONTROL_QUICK_LOAD"), CvString::format("XML error. Found %s instead of CONTROL_QUICK_LOAD at index %d", GC.getControlInfo(CONTROL_QUICK_LOAD).getType(), CONTROL_QUICK_LOAD).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_ORTHO_CAMERA).getType(), "CONTROL_ORTHO_CAMERA"), CvString::format("XML error. Found %s instead of CONTROL_ORTHO_CAMERA at index %d", GC.getControlInfo(CONTROL_ORTHO_CAMERA).getType(), CONTROL_ORTHO_CAMERA).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_CYCLE_CAMERA_FLYING_MODES).getType(), "CONTROL_CYCLE_CAMERA_FLYING_MODES"), CvString::format("XML error. Found %s instead of CONTROL_CYCLE_CAMERA_FLYING_MODES at index %d", GC.getControlInfo(CONTROL_CYCLE_CAMERA_FLYING_MODES).getType(), CONTROL_CYCLE_CAMERA_FLYING_MODES).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_ISOMETRIC_CAMERA_LEFT).getType(), "CONTROL_ISOMETRIC_CAMERA_LEFT"), CvString::format("XML error. Found %s instead of CONTROL_ISOMETRIC_CAMERA_LEFT at index %d", GC.getControlInfo(CONTROL_ISOMETRIC_CAMERA_LEFT).getType(), CONTROL_ISOMETRIC_CAMERA_LEFT).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_ISOMETRIC_CAMERA_RIGHT).getType(), "CONTROL_ISOMETRIC_CAMERA_RIGHT"), CvString::format("XML error. Found %s instead of CONTROL_ISOMETRIC_CAMERA_RIGHT at index %d", GC.getControlInfo(CONTROL_ISOMETRIC_CAMERA_RIGHT).getType(), CONTROL_ISOMETRIC_CAMERA_RIGHT).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_FLYING_CAMERA).getType(), "CONTROL_FLYING_CAMERA"), CvString::format("XML error. Found %s instead of CONTROL_FLYING_CAMERA at index %d", GC.getControlInfo(CONTROL_FLYING_CAMERA).getType(), CONTROL_FLYING_CAMERA).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_MOUSE_FLYING_CAMERA).getType(), "CONTROL_MOUSE_FLYING_CAMERA"), CvString::format("XML error. Found %s instead of CONTROL_MOUSE_FLYING_CAMERA at index %d", GC.getControlInfo(CONTROL_MOUSE_FLYING_CAMERA).getType(), CONTROL_MOUSE_FLYING_CAMERA).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_TOP_DOWN_CAMERA).getType(), "CONTROL_TOP_DOWN_CAMERA"), CvString::format("XML error. Found %s instead of CONTROL_TOP_DOWN_CAMERA at index %d", GC.getControlInfo(CONTROL_TOP_DOWN_CAMERA).getType(), CONTROL_TOP_DOWN_CAMERA).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_CIVILOPEDIA).getType(), "CONTROL_CIVILOPEDIA"), CvString::format("XML error. Found %s instead of CONTROL_CIVILOPEDIA at index %d", GC.getControlInfo(CONTROL_CIVILOPEDIA).getType(), CONTROL_CIVILOPEDIA).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_FOREIGN_SCREEN).getType(), "CONTROL_FOREIGN_SCREEN"), CvString::format("XML error. Found %s instead of CONTROL_FOREIGN_SCREEN at index %d", GC.getControlInfo(CONTROL_FOREIGN_SCREEN).getType(), CONTROL_FOREIGN_SCREEN).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_CONGRESS_SCREEN).getType(), "CONTROL_CONGRESS_SCREEN"), CvString::format("XML error. Found %s instead of CONTROL_CONGRESS_SCREEN at index %d", GC.getControlInfo(CONTROL_CONGRESS_SCREEN).getType(), CONTROL_CONGRESS_SCREEN).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_REVOLUTION_SCREEN).getType(), "CONTROL_REVOLUTION_SCREEN"), CvString::format("XML error. Found %s instead of CONTROL_REVOLUTION_SCREEN at index %d", GC.getControlInfo(CONTROL_REVOLUTION_SCREEN).getType(), CONTROL_REVOLUTION_SCREEN).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_EUROPE_SCREEN).getType(), "CONTROL_EUROPE_SCREEN"), CvString::format("XML error. Found %s instead of CONTROL_EUROPE_SCREEN at index %d", GC.getControlInfo(CONTROL_EUROPE_SCREEN).getType(), CONTROL_EUROPE_SCREEN).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_MILITARY_SCREEN).getType(), "CONTROL_MILITARY_SCREEN"), CvString::format("XML error. Found %s instead of CONTROL_MILITARY_SCREEN at index %d", GC.getControlInfo(CONTROL_MILITARY_SCREEN).getType(), CONTROL_MILITARY_SCREEN).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_FATHER_SCREEN).getType(), "CONTROL_FATHER_SCREEN"), CvString::format("XML error. Found %s instead of CONTROL_FATHER_SCREEN at index %d", GC.getControlInfo(CONTROL_FATHER_SCREEN).getType(), CONTROL_FATHER_SCREEN).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_TECHNOLOGY_SCREEN).getType(), "CONTROL_TECHNOLOGY_SCREEN"), CvString::format("XML error. Found %s instead of CONTROL_TECHNOLOGY_SCREEN at index %d", GC.getControlInfo(CONTROL_TECHNOLOGY_SCREEN).getType(), CONTROL_TECHNOLOGY_SCREEN).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_TRADE_SCREEN).getType(), "CONTROL_TRADE_SCREEN"), CvString::format("XML error. Found %s instead of CONTROL_TRADE_SCREEN at index %d", GC.getControlInfo(CONTROL_TRADE_SCREEN).getType(), CONTROL_TRADE_SCREEN).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_SPICE_ROUTE_SCREEN).getType(), "CONTROL_SPICE_ROUTE_SCREEN"), CvString::format("XML error. Found %s instead of CONTROL_SPICE_ROUTE_SCREEN at index %d", GC.getControlInfo(CONTROL_SPICE_ROUTE_SCREEN).getType(), CONTROL_SPICE_ROUTE_SCREEN).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_SILK_ROAD_SCREEN).getType(), "CONTROL_SILK_ROAD_SCREEN"), CvString::format("XML error. Found %s instead of CONTROL_SILK_ROAD_SCREEN at index %d", GC.getControlInfo(CONTROL_SILK_ROAD_SCREEN).getType(), CONTROL_SILK_ROAD_SCREEN).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_TRADE_FAIR_SCREEN).getType(), "CONTROL_TRADE_FAIR_SCREEN"), CvString::format("XML error. Found %s instead of CONTROL_TRADE_FAIR_SCREEN at index %d", GC.getControlInfo(CONTROL_TRADE_FAIR_SCREEN).getType(), CONTROL_TRADE_FAIR_SCREEN).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_IMMIGRATION_SCREEN).getType(), "CONTROL_IMMIGRATION_SCREEN"), CvString::format("XML error. Found %s instead of CONTROL_IMMIGRATION_SCREEN at index %d", GC.getControlInfo(CONTROL_IMMIGRATION_SCREEN).getType(), CONTROL_IMMIGRATION_SCREEN).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_TURN_LOG).getType(), "CONTROL_TURN_LOG"), CvString::format("XML error. Found %s instead of CONTROL_TURN_LOG at index %d", GC.getControlInfo(CONTROL_TURN_LOG).getType(), CONTROL_TURN_LOG).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_CHAT_ALL).getType(), "CONTROL_CHAT_ALL"), CvString::format("XML error. Found %s instead of CONTROL_CHAT_ALL at index %d", GC.getControlInfo(CONTROL_CHAT_ALL).getType(), CONTROL_CHAT_ALL).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_CHAT_TEAM).getType(), "CONTROL_CHAT_TEAM"), CvString::format("XML error. Found %s instead of CONTROL_CHAT_TEAM at index %d", GC.getControlInfo(CONTROL_CHAT_TEAM).getType(), CONTROL_CHAT_TEAM).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_DOMESTIC_SCREEN).getType(), "CONTROL_DOMESTIC_SCREEN"), CvString::format("XML error. Found %s instead of CONTROL_DOMESTIC_SCREEN at index %d", GC.getControlInfo(CONTROL_DOMESTIC_SCREEN).getType(), CONTROL_DOMESTIC_SCREEN).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_VICTORY_SCREEN).getType(), "CONTROL_VICTORY_SCREEN"), CvString::format("XML error. Found %s instead of CONTROL_VICTORY_SCREEN at index %d", GC.getControlInfo(CONTROL_VICTORY_SCREEN).getType(), CONTROL_VICTORY_SCREEN).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_INFO).getType(), "CONTROL_INFO"), CvString::format("XML error. Found %s instead of CONTROL_INFO at index %d", GC.getControlInfo(CONTROL_INFO).getType(), CONTROL_INFO).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_GLOBE_VIEW).getType(), "CONTROL_GLOBE_VIEW"), CvString::format("XML error. Found %s instead of CONTROL_GLOBE_VIEW at index %d", GC.getControlInfo(CONTROL_GLOBE_VIEW).getType(), CONTROL_GLOBE_VIEW).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_DETAILS).getType(), "CONTROL_DETAILS"), CvString::format("XML error. Found %s instead of CONTROL_DETAILS at index %d", GC.getControlInfo(CONTROL_DETAILS).getType(), CONTROL_DETAILS).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_ADMIN_DETAILS).getType(), "CONTROL_ADMIN_DETAILS"), CvString::format("XML error. Found %s instead of CONTROL_ADMIN_DETAILS at index %d", GC.getControlInfo(CONTROL_ADMIN_DETAILS).getType(), CONTROL_ADMIN_DETAILS).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_HALL_OF_FAME).getType(), "CONTROL_HALL_OF_FAME"), CvString::format("XML error. Found %s instead of CONTROL_HALL_OF_FAME at index %d", GC.getControlInfo(CONTROL_HALL_OF_FAME).getType(), CONTROL_HALL_OF_FAME).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_WORLD_BUILDER).getType(), "CONTROL_WORLD_BUILDER"), CvString::format("XML error. Found %s instead of CONTROL_WORLD_BUILDER at index %d", GC.getControlInfo(CONTROL_WORLD_BUILDER).getType(), CONTROL_WORLD_BUILDER).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_DIPLOMACY).getType(), "CONTROL_DIPLOMACY"), CvString::format("XML error. Found %s instead of CONTROL_DIPLOMACY at index %d", GC.getControlInfo(CONTROL_DIPLOMACY).getType(), CONTROL_DIPLOMACY).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_SELECT_HEALTHY).getType(), "CONTROL_SELECT_HEALTHY"), CvString::format("XML error. Found %s instead of CONTROL_SELECT_HEALTHY at index %d", GC.getControlInfo(CONTROL_SELECT_HEALTHY).getType(), CONTROL_SELECT_HEALTHY).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_FREE_COLONY).getType(), "CONTROL_FREE_COLONY"), CvString::format("XML error. Found %s instead of CONTROL_FREE_COLONY at index %d", GC.getControlInfo(CONTROL_FREE_COLONY).getType(), CONTROL_FREE_COLONY).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_AI_AUTOPLAY).getType(), "CONTROL_AI_AUTOPLAY"), CvString::format("XML error. Found %s instead of CONTROL_AI_AUTOPLAY at index %d", GC.getControlInfo(CONTROL_AI_AUTOPLAY).getType(), CONTROL_AI_AUTOPLAY).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_AI_AUTOPLAY5).getType(), "CONTROL_AI_AUTOPLAY5"), CvString::format("XML error. Found %s instead of CONTROL_AI_AUTOPLAY5 at index %d", GC.getControlInfo(CONTROL_AI_AUTOPLAY5).getType(), CONTROL_AI_AUTOPLAY5).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_AI_AUTOPLAY25).getType(), "CONTROL_AI_AUTOPLAY25"), CvString::format("XML error. Found %s instead of CONTROL_AI_AUTOPLAY25 at index %d", GC.getControlInfo(CONTROL_AI_AUTOPLAY25).getType(), CONTROL_AI_AUTOPLAY25).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_AI_AUTOPLAY100).getType(), "CONTROL_AI_AUTOPLAY100"), CvString::format("XML error. Found %s instead of CONTROL_AI_AUTOPLAY100 at index %d", GC.getControlInfo(CONTROL_AI_AUTOPLAY100).getType(), CONTROL_AI_AUTOPLAY100).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_AI_AUTOPLAY_REV).getType(), "CONTROL_AI_AUTOPLAY_REV"), CvString::format("XML error. Found %s instead of CONTROL_AI_AUTOPLAY_REV at index %d", GC.getControlInfo(CONTROL_AI_AUTOPLAY_REV).getType(), CONTROL_AI_AUTOPLAY_REV).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_LEARN_TECH).getType(), "CONTROL_LEARN_TECH"), CvString::format("XML error. Found %s instead of CONTROL_LEARN_TECH at index %d", GC.getControlInfo(CONTROL_LEARN_TECH).getType(), CONTROL_LEARN_TECH).c_str());
	FAssertMsg(!strcmp(GC.getControlInfo(CONTROL_FOR_TESTING).getType(), "CONTROL_FOR_TESTING"), CvString::format("XML error. Found %s instead of CONTROL_FOR_TESTING at index %d", GC.getControlInfo(CONTROL_FOR_TESTING).getType(), CONTROL_FOR_TESTING).c_str());

	FAssertMsg(GC.getControlInfo().size() == NUM_CONTROL_TYPES, CvString::format("XML error. Expected %d types, but found %d", NUM_CONTROL_TYPES, GC.getControlInfo().size()));
}

void CvGlobals::CheckEnumMemoryTypes() const
{
	FAssertMsg(!strcmp(GC.getMemoryInfo(MEMORY_DECLARED_WAR).getType(), "MEMORY_DECLARED_WAR"), CvString::format("XML error. Found %s instead of MEMORY_DECLARED_WAR at index %d", GC.getMemoryInfo(MEMORY_DECLARED_WAR).getType(), MEMORY_DECLARED_WAR).c_str());
	FAssertMsg(!strcmp(GC.getMemoryInfo(MEMORY_DECLARED_WAR_ON_FRIEND).getType(), "MEMORY_DECLARED_WAR_ON_FRIEND"), CvString::format("XML error. Found %s instead of MEMORY_DECLARED_WAR_ON_FRIEND at index %d", GC.getMemoryInfo(MEMORY_DECLARED_WAR_ON_FRIEND).getType(), MEMORY_DECLARED_WAR_ON_FRIEND).c_str());
	FAssertMsg(!strcmp(GC.getMemoryInfo(MEMORY_HIRED_WAR_ALLY).getType(), "MEMORY_HIRED_WAR_ALLY"), CvString::format("XML error. Found %s instead of MEMORY_HIRED_WAR_ALLY at index %d", GC.getMemoryInfo(MEMORY_HIRED_WAR_ALLY).getType(), MEMORY_HIRED_WAR_ALLY).c_str());
	FAssertMsg(!strcmp(GC.getMemoryInfo(MEMORY_RAZED_CITY).getType(), "MEMORY_RAZED_CITY"), CvString::format("XML error. Found %s instead of MEMORY_RAZED_CITY at index %d", GC.getMemoryInfo(MEMORY_RAZED_CITY).getType(), MEMORY_RAZED_CITY).c_str());
	FAssertMsg(!strcmp(GC.getMemoryInfo(MEMORY_GIVE_HELP).getType(), "MEMORY_GIVE_HELP"), CvString::format("XML error. Found %s instead of MEMORY_GIVE_HELP at index %d", GC.getMemoryInfo(MEMORY_GIVE_HELP).getType(), MEMORY_GIVE_HELP).c_str());
	FAssertMsg(!strcmp(GC.getMemoryInfo(MEMORY_REFUSED_HELP).getType(), "MEMORY_REFUSED_HELP"), CvString::format("XML error. Found %s instead of MEMORY_REFUSED_HELP at index %d", GC.getMemoryInfo(MEMORY_REFUSED_HELP).getType(), MEMORY_REFUSED_HELP).c_str());
	FAssertMsg(!strcmp(GC.getMemoryInfo(MEMORY_ACCEPT_DEMAND).getType(), "MEMORY_ACCEPT_DEMAND"), CvString::format("XML error. Found %s instead of MEMORY_ACCEPT_DEMAND at index %d", GC.getMemoryInfo(MEMORY_ACCEPT_DEMAND).getType(), MEMORY_ACCEPT_DEMAND).c_str());
	FAssertMsg(!strcmp(GC.getMemoryInfo(MEMORY_REJECTED_DEMAND).getType(), "MEMORY_REJECTED_DEMAND"), CvString::format("XML error. Found %s instead of MEMORY_REJECTED_DEMAND at index %d", GC.getMemoryInfo(MEMORY_REJECTED_DEMAND).getType(), MEMORY_REJECTED_DEMAND).c_str());
	FAssertMsg(!strcmp(GC.getMemoryInfo(MEMORY_ACCEPTED_JOIN_WAR).getType(), "MEMORY_ACCEPTED_JOIN_WAR"), CvString::format("XML error. Found %s instead of MEMORY_ACCEPTED_JOIN_WAR at index %d", GC.getMemoryInfo(MEMORY_ACCEPTED_JOIN_WAR).getType(), MEMORY_ACCEPTED_JOIN_WAR).c_str());
	FAssertMsg(!strcmp(GC.getMemoryInfo(MEMORY_DENIED_JOIN_WAR).getType(), "MEMORY_DENIED_JOIN_WAR"), CvString::format("XML error. Found %s instead of MEMORY_DENIED_JOIN_WAR at index %d", GC.getMemoryInfo(MEMORY_DENIED_JOIN_WAR).getType(), MEMORY_DENIED_JOIN_WAR).c_str());
	FAssertMsg(!strcmp(GC.getMemoryInfo(MEMORY_ACCEPTED_STOP_TRADING).getType(), "MEMORY_ACCEPTED_STOP_TRADING"), CvString::format("XML error. Found %s instead of MEMORY_ACCEPTED_STOP_TRADING at index %d", GC.getMemoryInfo(MEMORY_ACCEPTED_STOP_TRADING).getType(), MEMORY_ACCEPTED_STOP_TRADING).c_str());
	FAssertMsg(!strcmp(GC.getMemoryInfo(MEMORY_DENIED_STOP_TRADING).getType(), "MEMORY_DENIED_STOP_TRADING"), CvString::format("XML error. Found %s instead of MEMORY_DENIED_STOP_TRADING at index %d", GC.getMemoryInfo(MEMORY_DENIED_STOP_TRADING).getType(), MEMORY_DENIED_STOP_TRADING).c_str());
	FAssertMsg(!strcmp(GC.getMemoryInfo(MEMORY_STOPPED_TRADING).getType(), "MEMORY_STOPPED_TRADING"), CvString::format("XML error. Found %s instead of MEMORY_STOPPED_TRADING at index %d", GC.getMemoryInfo(MEMORY_STOPPED_TRADING).getType(), MEMORY_STOPPED_TRADING).c_str());
	FAssertMsg(!strcmp(GC.getMemoryInfo(MEMORY_STOPPED_TRADING_RECENT).getType(), "MEMORY_STOPPED_TRADING_RECENT"), CvString::format("XML error. Found %s instead of MEMORY_STOPPED_TRADING_RECENT at index %d", GC.getMemoryInfo(MEMORY_STOPPED_TRADING_RECENT).getType(), MEMORY_STOPPED_TRADING_RECENT).c_str());
	FAssertMsg(!strcmp(GC.getMemoryInfo(MEMORY_HIRED_TRADE_EMBARGO).getType(), "MEMORY_HIRED_TRADE_EMBARGO"), CvString::format("XML error. Found %s instead of MEMORY_HIRED_TRADE_EMBARGO at index %d", GC.getMemoryInfo(MEMORY_HIRED_TRADE_EMBARGO).getType(), MEMORY_HIRED_TRADE_EMBARGO).c_str());
	FAssertMsg(!strcmp(GC.getMemoryInfo(MEMORY_MADE_DEMAND).getType(), "MEMORY_MADE_DEMAND"), CvString::format("XML error. Found %s instead of MEMORY_MADE_DEMAND at index %d", GC.getMemoryInfo(MEMORY_MADE_DEMAND).getType(), MEMORY_MADE_DEMAND).c_str());
	FAssertMsg(!strcmp(GC.getMemoryInfo(MEMORY_MADE_DEMAND_RECENT).getType(), "MEMORY_MADE_DEMAND_RECENT"), CvString::format("XML error. Found %s instead of MEMORY_MADE_DEMAND_RECENT at index %d", GC.getMemoryInfo(MEMORY_MADE_DEMAND_RECENT).getType(), MEMORY_MADE_DEMAND_RECENT).c_str());
	FAssertMsg(!strcmp(GC.getMemoryInfo(MEMORY_CANCELLED_OPEN_BORDERS).getType(), "MEMORY_CANCELLED_OPEN_BORDERS"), CvString::format("XML error. Found %s instead of MEMORY_CANCELLED_OPEN_BORDERS at index %d", GC.getMemoryInfo(MEMORY_CANCELLED_OPEN_BORDERS).getType(), MEMORY_CANCELLED_OPEN_BORDERS).c_str());
	FAssertMsg(!strcmp(GC.getMemoryInfo(MEMORY_EVENT_GOOD_TO_US).getType(), "MEMORY_EVENT_GOOD_TO_US"), CvString::format("XML error. Found %s instead of MEMORY_EVENT_GOOD_TO_US at index %d", GC.getMemoryInfo(MEMORY_EVENT_GOOD_TO_US).getType(), MEMORY_EVENT_GOOD_TO_US).c_str());
	FAssertMsg(!strcmp(GC.getMemoryInfo(MEMORY_EVENT_BAD_TO_US).getType(), "MEMORY_EVENT_BAD_TO_US"), CvString::format("XML error. Found %s instead of MEMORY_EVENT_BAD_TO_US at index %d", GC.getMemoryInfo(MEMORY_EVENT_BAD_TO_US).getType(), MEMORY_EVENT_BAD_TO_US).c_str());
	FAssertMsg(!strcmp(GC.getMemoryInfo(MEMORY_LIBERATED_CITIES).getType(), "MEMORY_LIBERATED_CITIES"), CvString::format("XML error. Found %s instead of MEMORY_LIBERATED_CITIES at index %d", GC.getMemoryInfo(MEMORY_LIBERATED_CITIES).getType(), MEMORY_LIBERATED_CITIES).c_str());
	FAssertMsg(!strcmp(GC.getMemoryInfo(MEMORY_REFUSED_TAX).getType(), "MEMORY_REFUSED_TAX"), CvString::format("XML error. Found %s instead of MEMORY_REFUSED_TAX at index %d", GC.getMemoryInfo(MEMORY_REFUSED_TAX).getType(), MEMORY_REFUSED_TAX).c_str());
	FAssertMsg(!strcmp(GC.getMemoryInfo(MEMORY_REVENGE_TAKEN).getType(), "MEMORY_REVENGE_TAKEN"), CvString::format("XML error. Found %s instead of MEMORY_REVENGE_TAKEN at index %d", GC.getMemoryInfo(MEMORY_REVENGE_TAKEN).getType(), MEMORY_REVENGE_TAKEN).c_str());
	FAssertMsg(!strcmp(GC.getMemoryInfo(MEMORY_MISSIONARY_FAIL).getType(), "MEMORY_MISSIONARY_FAIL"), CvString::format("XML error. Found %s instead of MEMORY_MISSIONARY_FAIL at index %d", GC.getMemoryInfo(MEMORY_MISSIONARY_FAIL).getType(), MEMORY_MISSIONARY_FAIL).c_str());
	FAssertMsg(!strcmp(GC.getMemoryInfo(MEMORY_INSULTED).getType(), "MEMORY_INSULTED"), CvString::format("XML error. Found %s instead of MEMORY_INSULTED at index %d", GC.getMemoryInfo(MEMORY_INSULTED).getType(), MEMORY_INSULTED).c_str());
	FAssertMsg(!strcmp(GC.getMemoryInfo(MEMORY_MADE_VASSAL_DEMAND).getType(), "MEMORY_MADE_VASSAL_DEMAND"), CvString::format("XML error. Found %s instead of MEMORY_MADE_VASSAL_DEMAND at index %d", GC.getMemoryInfo(MEMORY_MADE_VASSAL_DEMAND).getType(), MEMORY_MADE_VASSAL_DEMAND).c_str());

	FAssertMsg(GC.getMemoryInfo().size() == NUM_MEMORY_TYPES, CvString::format("XML error. Expected %d types, but found %d", NUM_MEMORY_TYPES, GC.getMemoryInfo().size()));
}

void CvGlobals::CheckEnumAttitudeTypes() const
{
	FAssertMsg(!strcmp(GC.getAttitudeInfo(ATTITUDE_FURIOUS).getType(), "ATTITUDE_FURIOUS"), CvString::format("XML error. Found %s instead of ATTITUDE_FURIOUS at index %d", GC.getAttitudeInfo(ATTITUDE_FURIOUS).getType(), ATTITUDE_FURIOUS).c_str());
	FAssertMsg(!strcmp(GC.getAttitudeInfo(ATTITUDE_ANNOYED).getType(), "ATTITUDE_ANNOYED"), CvString::format("XML error. Found %s instead of ATTITUDE_ANNOYED at index %d", GC.getAttitudeInfo(ATTITUDE_ANNOYED).getType(), ATTITUDE_ANNOYED).c_str());
	FAssertMsg(!strcmp(GC.getAttitudeInfo(ATTITUDE_CAUTIOUS).getType(), "ATTITUDE_CAUTIOUS"), CvString::format("XML error. Found %s instead of ATTITUDE_CAUTIOUS at index %d", GC.getAttitudeInfo(ATTITUDE_CAUTIOUS).getType(), ATTITUDE_CAUTIOUS).c_str());
	FAssertMsg(!strcmp(GC.getAttitudeInfo(ATTITUDE_PLEASED).getType(), "ATTITUDE_PLEASED"), CvString::format("XML error. Found %s instead of ATTITUDE_PLEASED at index %d", GC.getAttitudeInfo(ATTITUDE_PLEASED).getType(), ATTITUDE_PLEASED).c_str());
	FAssertMsg(!strcmp(GC.getAttitudeInfo(ATTITUDE_FRIENDLY).getType(), "ATTITUDE_FRIENDLY"), CvString::format("XML error. Found %s instead of ATTITUDE_FRIENDLY at index %d", GC.getAttitudeInfo(ATTITUDE_FRIENDLY).getType(), ATTITUDE_FRIENDLY).c_str());

	FAssertMsg(GC.getAttitudeInfo().size() == NUM_ATTITUDE_TYPES, CvString::format("XML error. Expected %d types, but found %d", NUM_ATTITUDE_TYPES, GC.getAttitudeInfo().size()));
}
// XML enum check - end - Nightinggale

/// special building placement xml - start - Nightinggale
void CvGlobals::reloadSpecialBuildings()
{
	CvXMLLoadUtility pXML;
	pXML.reloadSpecialBuildings();
}
/// special building placement xml - end - Nightinggale
