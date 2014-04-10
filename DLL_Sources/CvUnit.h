#pragma once

// unit.h

#ifndef CIV4_UNIT_H
#define CIV4_UNIT_H

#include "CvDLLEntity.h"
//#include "CvEnums.h"
//#include "CvStructs.h"

#pragma warning( disable: 4251 )		// needs to have dll-interface to be used by clients of class

class CvPlot;
class CvArea;
class CvUnitInfo;
class CvSelectionGroup;
class FAStarNode;
class CvArtInfoUnit;

struct CombatDetails
{
	int iExtraCombatPercent;
	int iNativeCombatModifierTB;
	int iNativeCombatModifierAB;
	int iPlotDefenseModifier;
	int iFortifyModifier;
	int iCityDefenseModifier;
	int iHillsAttackModifier;
	int iHillsDefenseModifier;
	int iFeatureAttackModifier;
	int iFeatureDefenseModifier;
	int iTerrainAttackModifier;
	int iTerrainDefenseModifier;
	int iCityAttackModifier;
	int iDomainDefenseModifier;
	int iClassDefenseModifier;
	int iClassAttackModifier;
	int iCombatModifierT;
	int iCombatModifierA;
	int iDomainModifierA;
	int iDomainModifierT;
	int iRiverAttackModifier;
	int iAmphibAttackModifier;
	int iRebelPercentModifier;
	int iModifierTotal;
	int iBaseCombatStr;
	int iCombat;
	int iMaxCombatStr;
	int iCurrHitPoints;
	int iMaxHitPoints;
	int iCurrCombatStr;
	PlayerTypes eOwner;
	PlayerTypes eVisualOwner;
	std::wstring sUnitName;
};

class CvUnitTemporaryStrengthModifier
{
public:
	CvUnitTemporaryStrengthModifier(CvUnit* pUnit, ProfessionTypes eProfession);
	~CvUnitTemporaryStrengthModifier();

private:
	CvUnit* m_pUnit;
	ProfessionTypes m_eProfession;
};

class CvUnit : public CvDLLEntity
{

public:



	CvUnit();
	virtual ~CvUnit();

	void reloadEntity();
	void init(int iID, UnitTypes eUnit, ProfessionTypes eProfession, UnitAITypes eUnitAI, PlayerTypes eOwner, int iX, int iY, DirectionTypes eFacingDirection, int iYieldStored);
	void uninit();
	void reset(int iID = 0, UnitTypes eUnit = NO_UNIT, PlayerTypes eOwner = NO_PLAYER, bool bConstructorCall = false);
	void setupGraphical();
	void convert(CvUnit* pUnit, bool bKill);
	void kill(bool bDelay, CvUnit* pAttacker = NULL);
	void removeFromMap();
	void addToMap(int iPlotX, int iPlotY);
	void updateOwnerCache(int iChange);

	DllExport void NotifyEntity(MissionTypes eMission);

	void doTurn();

	void updateCombat(bool bQuick = false);

	bool isActionRecommended(int iAction);
	bool isBetterDefenderThan(const CvUnit* pDefender, const CvUnit* pAttacker, bool bBreakTies) const;

	bool canDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible = false, bool bTestBusy = true);
	DllExport void doCommand(CommandTypes eCommand, int iData1, int iData2);

	FAStarNode* getPathLastNode() const;
	CvPlot* getPathEndTurnPlot() const;
	int getPathCost() const;
	bool generatePath(const CvPlot* pToPlot, int iFlags = 0, bool bReuse = false, int* piPathTurns = NULL) const;

	bool canEnterTerritory(PlayerTypes ePlayer, bool bIgnoreRightOfPassage = false) const;
	bool canEnterArea(PlayerTypes ePlayer, const CvArea* pArea, bool bIgnoreRightOfPassage = false) const;
	TeamTypes getDeclareWarUnitMove(const CvPlot* pPlot) const;
	bool canMoveInto(const CvPlot* pPlot, bool bAttack = false, bool bDeclareWar = false, bool bIgnoreLoad = false) const;
	bool canMoveOrAttackInto(const CvPlot* pPlot, bool bDeclareWar = false) const;
	bool canMoveThrough(const CvPlot* pPlot) const;
	void attack(CvPlot* pPlot, bool bQuick);
	void move(CvPlot* pPlot, bool bShow);
	bool jumpToNearestValidPlot();
	bool isValidPlot(const CvPlot* pPlot) const;

	bool canAutomate(AutomateTypes eAutomate) const;
	void automate(AutomateTypes eAutomate);
	bool canScrap() const;
	void scrap();
	bool canGift(bool bTestVisible = false, bool bTestTransport = true);
	void gift(bool bTestTransport = true);
	bool canLoadUnit(const CvUnit* pTransport, const CvPlot* pPlot, bool bCheckCity) const;
	void loadUnit(CvUnit* pTransport);
	bool canLoad(const CvPlot* pPlot, bool bCheckCity) const;
	bool load(bool bCheckCity);
	bool shouldLoadOnMove(const CvPlot* pPlot) const;

	int getLoadedYieldAmount(YieldTypes eYield) const;
	int getLoadYieldAmount(YieldTypes eYield) const;
	bool canLoadYields(const CvPlot* pPlot, bool bTrade) const;
	bool canLoadYield(const CvPlot* pPlot, YieldTypes eYield, bool bTrade) const;
	void loadYield(YieldTypes eYield, bool bTrade);
	void loadYieldAmount(YieldTypes eYield, int iAmount, bool bTrade);
	int getMaxLoadYieldAmount(YieldTypes eYield) const;

	bool canTradeYield(const CvPlot* pPlot) const;
	void tradeYield();

	bool canClearSpecialty() const;
	void clearSpecialty();
	///Tks Med TradeScreens
	bool canAutoCrossOcean(const CvPlot* pPlot, TradeRouteTypes eTradeRouteType=NO_TRADE_ROUTES, bool bAIForce=false) const;
	bool canAutoSailTradeScreen(const CvPlot* pPlot, EuropeTypes eTradeScreenType=NO_EUROPE, bool bAIForce=false) const;
	bool canCrossOcean(const CvPlot* pPlot, UnitTravelStates eNewState, TradeRouteTypes eTradeRouteType=NO_TRADE_ROUTES, bool bAIForce=false, EuropeTypes eEuropeTradeRoute=NO_EUROPE) const;
	void crossOcean(UnitTravelStates eNewState, bool bAIForce=false, EuropeTypes eTradeMarket=NO_EUROPE);
	//TKe
	bool canUnload() const;
	void unload();
	void unloadStoredAmount(int iAmount);
	bool canUnloadAll() const;
	void unloadAll();

	bool canLearn() const;
	void learn();
	void doLiveAmongNatives();
	void doLearn();
	UnitTypes getLearnUnitType(const CvPlot* pPlot) const;
	int getLearnTime() const;
    ///Tks Med
	bool canKingTransport(bool bTestVisible = false) const;
	///Tks Return Home
	bool doRansomKnight(int iKillerPlayerID);
	bool checkHasHomeCity(bool bTestVisible) const;
	bool canHireGuard(bool bTestVisible);
	bool canCollectTaxes(bool bTestVisible);
	bool canCallBanners(bool bTestVisible);
	bool canBuildHome(bool bTestVisible);
	void hireGuard();
	int getEscortPromotion() const;
	void setEscortPromotion(PromotionTypes ePromotion);
	bool collectTaxes();
	void callBanners();
	void buildHome();
	bool canBuildTradingPost(bool bTestVisible);
	void buildTradingPost(bool bTestVisible);
	///Tke
	void kingTransport(bool bSkipPopup);
	void doKingTransport();

	bool canEstablishMission() const;
	void establishMission();
	int getMissionarySuccessPercent() const;

	bool canSpeakWithChief(CvPlot* pPlot) const;
	void speakWithChief();
	bool canHold(const CvPlot* pPlot) const;
	DllExport bool canSleep(const CvPlot* pPlot) const;
	DllExport bool canFortify(const CvPlot* pPlot) const;

	bool canHeal(const CvPlot* pPlot) const;
	bool canSentry(const CvPlot* pPlot) const;

	int healRate(const CvPlot* pPlot) const;
	int healTurns(const CvPlot* pPlot) const;
	void doHeal();
	CvCity* bombardTarget(const CvPlot* pPlot) const;
	bool canBombard(const CvPlot* pPlot) const;
	bool bombard();
	bool canPillage(const CvPlot* pPlot) const;
	bool pillage();
	bool canSack(const CvPlot* pPlot);
	bool sack(CvPlot* pPlot);
	bool canFound(const CvPlot* pPlot, bool bTestVisible = false, int iFoundType = -1) const;
		///Tks Med
	bool found(int iType = -1);
	bool doFound(bool bBuyLand, int iType = -1);
	bool doFoundCheckNatives(int iType = -1);
	CvCity* getEvasionCity(int iWaylayed = -1) const;
		///Tke
	bool canJoinCity(const CvPlot* pPlot, bool bTestVisible = false) const;
	bool canJoinStarvingCity(const CvCity& kCity) const;
	bool joinCity();
	bool canBuild(const CvPlot* pPlot, BuildTypes eBuild, bool bTestVisible = false) const;
	///Tks Med
	int canBuildInt(const CvPlot* pPlot, BuildTypes eBuild, bool bTestVisible = false) const;
	///Tke
	bool build(BuildTypes eBuild);
	bool canPromote(PromotionTypes ePromotion, int iLeaderUnitId) const;
	void promote(PromotionTypes ePromotion, int iLeaderUnitId);

	int canLead(const CvPlot* pPlot, int iUnitId) const;
	bool lead(int iUnitId);
	int canGiveExperience(const CvPlot* pPlot) const;
	bool giveExperience();
	int getStackExperienceToGive(int iNumUnits) const;
	int upgradePrice(UnitTypes eUnit) const;
	bool upgradeAvailable(UnitTypes eFromUnit, UnitClassTypes eToUnitClass, int iCount = 0) const;
	bool canUpgrade(UnitTypes eUnit, bool bTestVisible = false) const;
	bool isReadyForUpgrade() const;
	bool hasUpgrade(bool bSearch = false) const;
	bool hasUpgrade(UnitTypes eUnit, bool bSearch = false) const;
	CvCity* getUpgradeCity(bool bSearch = false) const;
	CvCity* getUpgradeCity(UnitTypes eUnit, bool bSearch = false, int* iSearchValue = NULL) const;
	void upgrade(UnitTypes eUnit);
	HandicapTypes getHandicapType() const;
	DllExport CivilizationTypes getCivilizationType() const;
	const wchar* getVisualCivAdjective(TeamTypes eForTeam) const;
	SpecialUnitTypes getSpecialUnitType() const;
	UnitTypes getCaptureUnitType(CivilizationTypes eCivilization) const;
	UnitCombatTypes getUnitCombatType() const;
	DllExport DomainTypes getDomainType() const;
	InvisibleTypes getInvisibleType() const;
	int getNumSeeInvisibleTypes() const;
	InvisibleTypes getSeeInvisibleType(int i) const;

	bool isHuman() const;
	bool isNative() const;

	int visibilityRange() const;

	int baseMoves() const;
	int maxMoves() const;
	int movesLeft() const;
	DllExport bool canMove() const;
	DllExport bool hasMoved() const;

	bool canBuildRoute() const;
	DllExport BuildTypes getBuildType() const;
	int workRate(bool bMax) const;
	void changeExtraWorkRate(int iChange);
	int getExtraWorkRate() const;
	bool isNoBadGoodies() const;
	bool isOnlyDefensive() const;
	bool isNoUnitCapture() const;
	bool isNoCityCapture() const;
	bool isRivalTerritory() const;
	bool canCoexistWithEnemyUnit(TeamTypes eTeam) const;

	DllExport bool isFighting() const;
	DllExport bool isAttacking() const;
	DllExport bool isDefending() const;
	bool isCombat() const;

	DllExport int maxHitPoints() const;
	DllExport int currHitPoints() const;
	bool isHurt() const;
	DllExport bool isDead() const;

	void setBaseCombatStr(int iCombat);
	DllExport int baseCombatStr() const;
	void updateBestLandCombat();
	int maxCombatStr(const CvPlot* pPlot, const CvUnit* pAttacker, CombatDetails* pCombatDetails = NULL) const;
	int currCombatStr(const CvPlot* pPlot, const CvUnit* pAttacker, CombatDetails* pCombatDetails = NULL) const;
	int currFirepower(const CvPlot* pPlot, const CvUnit* pAttacker) const;
	int currEffectiveStr(const CvPlot* pPlot, const CvUnit* pAttacker, CombatDetails* pCombatDetails = NULL) const;
	DllExport float maxCombatStrFloat(const CvPlot* pPlot, const CvUnit* pAttacker) const;
	DllExport float currCombatStrFloat(const CvPlot* pPlot, const CvUnit* pAttacker) const;
	bool isUnarmed() const;
	int getPower() const;
	int getAsset() const;

	DllExport bool canFight() const;
	bool canAttack() const;
	bool canDefend(const CvPlot* pPlot = NULL) const;
	bool canSiege(TeamTypes eTeam) const;

	bool isAutomated() const;
	DllExport bool isWaiting() const;
	DllExport bool isFortifyable() const;
	int fortifyModifier() const;

	int experienceNeeded() const;
	int attackXPValue() const;
	int defenseXPValue() const;
	int maxXPValue() const;

	DllExport int firstStrikes() const;																								// Exposed to Python
	DllExport int chanceFirstStrikes() const;																					// Exposed to Python
	DllExport int maxFirstStrikes() const;
	DllExport bool immuneToFirstStrikes() const;

	DllExport bool isRanged() const;

	bool alwaysInvisible() const;
	bool noDefensiveBonus() const;
	bool canMoveImpassable() const;
	bool flatMovementCost() const;
	bool ignoreTerrainCost() const;
	bool isNeverInvisible() const;
	DllExport bool isInvisible(TeamTypes eTeam, bool bDebug, bool bCheckCargo = true) const;

	int withdrawalProbability() const;
	int getEvasionProbability(const CvUnit& kAttacker) const;

	int cityAttackModifier() const;
	int cityDefenseModifier() const;
	int hillsAttackModifier() const;
	int hillsDefenseModifier() const;
	int terrainAttackModifier(TerrainTypes eTerrain) const;
	int terrainDefenseModifier(TerrainTypes eTerrain) const;
	int featureAttackModifier(FeatureTypes eFeature) const;
	int featureDefenseModifier(FeatureTypes eFeature) const;
	int unitClassAttackModifier(UnitClassTypes eUnitClass) const;
	int unitClassDefenseModifier(UnitClassTypes eUnitClass) const;
	int unitCombatModifier(UnitCombatTypes eUnitCombat) const;
	int domainModifier(DomainTypes eDomain) const;
	int rebelModifier(PlayerTypes eOtherPlayer) const;

	int bombardRate() const;
	SpecialUnitTypes specialCargo() const;
	DomainTypes domainCargo() const;
	int cargoSpace() const;
	void changeCargoSpace(int iChange);
	bool isFull() const;
	int cargoSpaceAvailable(SpecialUnitTypes eSpecialCargo = NO_SPECIALUNIT, DomainTypes eDomainCargo = NO_DOMAIN) const;
	bool hasCargo() const;
	bool canCargoAllMove() const;
	bool canCargoEnterArea(PlayerTypes ePlayer, const CvArea* pArea, bool bIgnoreRightOfPassage) const;
	int getUnitAICargo(UnitAITypes eUnitAI) const;
	bool canAssignTradeRoute(int iRouteId, bool bReusePath = false) const;

	DllExport int getID() const;
	int getIndex() const;
	DllExport IDInfo getIDInfo() const;
	void setID(int iID);
	int getGroupID() const;
	bool isInGroup() const;
	DllExport bool isGroupHead() const;
	DllExport CvSelectionGroup* getGroup() const;
	bool canJoinGroup(const CvPlot* pPlot, CvSelectionGroup* pSelectionGroup) const;
	DllExport void joinGroup(CvSelectionGroup* pSelectionGroup, bool bRemoveSelected = false, bool bRejoin = true);
	DllExport int getHotKeyNumber();
	void setHotKeyNumber(int iNewValue);

	DllExport int getX() const;
#ifdef _USRDLL
	inline int getX_INLINE() const
	{
		return m_iX;
	}
#endif
	DllExport int getY() const;
#ifdef _USRDLL
	inline int getY_INLINE() const
	{
		return m_iY;
	}
#endif
	void setXY(int iX, int iY, bool bGroup = false, bool bUpdate = true, bool bShow = false, bool bCheckPlotVisible = false);
	bool at(int iX, int iY) const;
	DllExport bool atPlot(const CvPlot* pPlot) const;
	DllExport CvPlot* plot() const;
	CvCity* getCity() const;
	int getArea() const;
	CvArea* area() const;
	int getLastMoveTurn() const;
	void setLastMoveTurn(int iNewValue);
	int getGameTurnCreated() const;
	void setGameTurnCreated(int iNewValue);
	DllExport int getDamage() const;
	void setDamage(int iNewValue, CvUnit* pAttacker = NULL, bool bNotifyEntity = true);
	void changeDamage(int iChange, CvUnit* pAttacker = NULL);

	int getMoves() const;
	void setMoves(int iNewValue);
	void changeMoves(int iChange);
	void finishMoves();

	int getExperience() const;
	void setExperience(int iNewValue, int iMax = -1);
	void changeExperience(int iChange, int iMax = -1, bool bFromCombat = false, bool bInBorders = false, bool bUpdateGlobal = false);

	int getLevel() const;
	void setLevel(int iNewValue);
	void changeLevel(int iChange);
	DllExport int getCargo() const;
	void changeCargo(int iChange);

	CvPlot* getAttackPlot() const;
	void setAttackPlot(const CvPlot* pNewValue);

	DllExport int getCombatTimer() const;
	void setCombatTimer(int iNewValue);
	void changeCombatTimer(int iChange);

    ///TK FS
    int getCombatFirstStrikes() const;
    int getTrainCounter() const;
    int getTraderCode() const;
	void setCombatFirstStrikes(int iNewValue);
	void changeCombatFirstStrikes(int iChange);
	void changeTrainCounter(int iChange);
	bool canTrainUnit() const;
	void changeTraderCode(int iChange);
	///TKe

	int getCombatDamage() const;
	void setCombatDamage(int iNewValue);

	int getFortifyTurns() const;
	void setFortifyTurns(int iNewValue);
	void changeFortifyTurns(int iChange);
	int getBlitzCount() const;
	bool isBlitz() const;
	void changeBlitzCount(int iChange);

	int getAmphibCount() const;
	bool isAmphib() const;
	void changeAmphibCount(int iChange);

	int getRiverCount() const;
	bool isRiver() const;
	void changeRiverCount(int iChange);

	int getEnemyRouteCount() const;
	bool isEnemyRoute() const;
	void changeEnemyRouteCount(int iChange);

	int getAlwaysHealCount() const;
	bool isAlwaysHeal() const;
	void changeAlwaysHealCount(int iChange);

	int getHillsDoubleMoveCount() const;
	bool isHillsDoubleMove() const;
	void changeHillsDoubleMoveCount(int iChange);

    ///TK FS
    int getImmuneToFirstStrikesCount() const;
	void changeImmuneToFirstStrikesCount(int iChange);
	int getExtraFirstStrikes() const;
	void changeExtraFirstStrikes(int iChange);
	int getExtraChanceFirstStrikes() const;	// Exposed to Python
	void changeExtraChanceFirstStrikes(int iChange);
	void getCombatBlockParrys(CvUnit* Defender);
	void setCombatBlockParrys(int iNewValue);
	void setUpCombatBlockParrys(CvUnit* Defender);
	bool setCombatAttackBlows(CvUnit* Defender);
	void setCombatCrushingBlow(bool iNewValue);
	void setCombatGlancingBlow(bool iNewValue);
	bool isCombatCrushingBlow();
	bool isCombatGlancingBlow();
	int getCombatBlockParrys();
	void setAddedFreeBuilding(bool iNewValue);
	bool isAddedFreeBuilding();
    ///TKe

	int getExtraVisibilityRange() const;
	void changeExtraVisibilityRange(int iChange);
	int getExtraMoves() const;
	void changeExtraMoves(int iChange);
	int getExtraMoveDiscount() const;
	void changeExtraMoveDiscount(int iChange);
	int getExtraWithdrawal() const;
	void changeExtraWithdrawal(int iChange);
	int getExtraBombardRate() const;
	void changeExtraBombardRate(int iChange);
	int getExtraEnemyHeal() const;
	void changeExtraEnemyHeal(int iChange);

	int getExtraNeutralHeal() const;
	void changeExtraNeutralHeal(int iChange);

	int getExtraFriendlyHeal() const;
	void changeExtraFriendlyHeal(int iChange);

	int getSameTileHeal() const;
	void changeSameTileHeal(int iChange);

	int getAdjacentTileHeal() const;
	void changeAdjacentTileHeal(int iChange);

	int getExtraCombatPercent() const;
	void changeExtraCombatPercent(int iChange);
	int getExtraCityAttackPercent() const;
	void changeExtraCityAttackPercent(int iChange);
	int getExtraCityDefensePercent() const;
	void changeExtraCityDefensePercent(int iChange);
	int getExtraHillsAttackPercent() const;
	void changeExtraHillsAttackPercent(int iChange);
	int getExtraHillsDefensePercent() const;
	void changeExtraHillsDefensePercent(int iChange);
	int getPillageChange() const;
	void changePillageChange(int iChange);
	int getUpgradeDiscount() const;
	void changeUpgradeDiscount(int iChange);
	int getExperiencePercent() const;
	void changeExperiencePercent(int iChange);
	DllExport DirectionTypes getFacingDirection(bool checkLineOfSightProperty) const;
	void setFacingDirection(DirectionTypes facingDirection);
	void rotateFacingDirectionClockwise();
	void rotateFacingDirectionCounterClockwise();
	DllExport ProfessionTypes getProfession() const;
	void setProfession(ProfessionTypes eProfession, bool bForce = false);
	bool canHaveProfession(ProfessionTypes eProfession, bool bBumpOther,  const CvPlot* pPlot = NULL) const;
	void processProfession(ProfessionTypes eProfession, int iChange, bool bUpdateCity);
	void processProfessionStats(ProfessionTypes eProfession, int iChange);
	int getProfessionChangeYieldRequired(ProfessionTypes eProfession, YieldTypes eYield) const;
	int getEuropeProfessionChangeCost(ProfessionTypes eProfession) const;

	bool isMadeAttack() const;
	void setMadeAttack(bool bNewValue);

	DllExport bool isPromotionReady() const;
	DllExport void setPromotionReady(bool bNewValue);
	void testPromotionReady();

	bool isDelayedDeath() const;
	void startDelayedDeath();
	bool doDelayedDeath();

	bool isCombatFocus() const;

	DllExport bool isInfoBarDirty() const;
	DllExport void setInfoBarDirty(bool bNewValue);

	DllExport PlayerTypes getOwner() const;
#ifdef _USRDLL
	inline PlayerTypes getOwnerINLINE() const
	{
		return m_eOwner;
	}
#endif
	DllExport PlayerTypes getVisualOwner(TeamTypes eForTeam = NO_TEAM) const;
	PlayerTypes getCombatOwner(TeamTypes eForTeam, const CvPlot* pPlot) const;
	DllExport TeamTypes getTeam() const;
	DllExport PlayerColorTypes getPlayerColor(TeamTypes eForTeam = NO_TEAM) const;
	DllExport CivilizationTypes getVisualCiv(TeamTypes eForTeam = NO_TEAM) const;
	TeamTypes getCombatTeam(TeamTypes eForTeam, const CvPlot* pPlot) const;

	PlayerTypes getCapturingPlayer() const;
	void setCapturingPlayer(PlayerTypes eNewValue);
	DllExport UnitTypes getUnitType() const;
	DllExport CvUnitInfo &getUnitInfo() const;
	UnitClassTypes getUnitClassType() const;

	DllExport UnitTypes getLeaderUnitType() const;
	void setLeaderUnitType(UnitTypes leaderUnitType);
    ///TKs Invention Core Mod v 1.0
	DllExport UnitTypes getConvertToUnit() const;
	void setConvertToUnit(UnitTypes eConvertToUnit);
	///Tks Med
	EuropeTypes getUnitTradeMarket() const;
	bool canGarrison() const;
	void setUnitTradeMarket(EuropeTypes eMarket);
	CvPlot* getTravelPlot() const;
	void setTravelPlot();
	bool doUnitPilgram();
	CvPlot* findNearestValidMarauderPlot(CvCity* pSpawnCity, CvCity* pVictimCity, bool bNoCitySpawn, bool bMustBeSameArea);
	int getPlotWorkedBonus() const;
	void changePlotWorkedBonus(int iChange);
	int getBuildingWorkedBonus() const;
	void changeBuildingWorkedBonus(int iChange);
	int getInvisibleTimer() const;
	void setInvisibleTimer(int iNewValue);
	void changeInvisibleTimer(int iChange);
	///TKe
	DllExport CvUnit* getCombatUnit() const;
	void setCombatUnit(CvUnit* pUnit, bool bAttacking = false);
	DllExport CvPlot* getPostCombatPlot() const;
	void setPostCombatPlot(int iX, int iY);
	DllExport CvUnit* getTransportUnit() const;
	bool isCargo() const;
	bool setTransportUnit(CvUnit* pTransportUnit, bool bUnload = true);
	int getExtraDomainModifier(DomainTypes eIndex) const;
	void changeExtraDomainModifier(DomainTypes eIndex, int iChange);
	DllExport const CvWString getName(uint uiForm = 0) const;
	const wchar* getNameKey() const;
	const CvWString getNameNoDesc() const;
	void setName(const CvWString szNewValue);
	const CvWString getNameAndProfession() const;
	const wchar* getNameOrProfessionKey() const;

	// Script data needs to be a narrow string for pickling in Python
	std::string getScriptData() const;
	void setScriptData(std::string szNewValue);

	int getTerrainDoubleMoveCount(TerrainTypes eIndex) const;
	bool isTerrainDoubleMove(TerrainTypes eIndex) const;
	void changeTerrainDoubleMoveCount(TerrainTypes eIndex, int iChange);

	int getFeatureDoubleMoveCount(FeatureTypes eIndex) const;
	bool isFeatureDoubleMove(FeatureTypes eIndex) const;
	void changeFeatureDoubleMoveCount(FeatureTypes eIndex, int iChange);
	int getExtraTerrainAttackPercent(TerrainTypes eIndex) const;
	void changeExtraTerrainAttackPercent(TerrainTypes eIndex, int iChange);
	int getExtraTerrainDefensePercent(TerrainTypes eIndex) const;
	void changeExtraTerrainDefensePercent(TerrainTypes eIndex, int iChange);
	int getExtraFeatureAttackPercent(FeatureTypes eIndex) const;
	void changeExtraFeatureAttackPercent(FeatureTypes eIndex, int iChange);
	int getExtraFeatureDefensePercent(FeatureTypes eIndex) const;
	void changeExtraFeatureDefensePercent(FeatureTypes eIndex, int iChange);
	int getExtraUnitClassAttackModifier(UnitClassTypes eIndex) const;
	void changeExtraUnitClassAttackModifier(UnitClassTypes eIndex, int iChange);
	int getExtraUnitClassDefenseModifier(UnitClassTypes eIndex) const;
	void changeExtraUnitClassDefenseModifier(UnitClassTypes eIndex, int iChange);
	int getExtraUnitCombatModifier(UnitCombatTypes eIndex) const;
	void changeExtraUnitCombatModifier(UnitCombatTypes eIndex, int iChange);
	bool canAcquirePromotion(PromotionTypes ePromotion) const;
	bool canAcquirePromotionAny() const;
	bool isPromotionValid(PromotionTypes ePromotion) const;
	bool isHasPromotion(PromotionTypes eIndex) const;
	bool isHasRealPromotion(PromotionTypes eIndex) const;
	void setHasRealPromotion(PromotionTypes eIndex, bool bValue);
	void changeFreePromotionCount(PromotionTypes eIndex, int iChange);
	void setFreePromotionCount(PromotionTypes eIndex, int iValue);
	int getFreePromotionCount(PromotionTypes eIndex) const;

	int getSubUnitCount() const;
	DllExport int getSubUnitsAlive() const;
	int getSubUnitsAlive(int iDamage) const;

	DllExport bool isEnemy(TeamTypes eTeam, const CvPlot* pPlot = NULL) const;
	bool isPotentialEnemy(TeamTypes eTeam, const CvPlot* pPlot = NULL) const;

	int getTriggerValue(EventTriggerTypes eTrigger, const CvPlot* pPlot, bool bCheckPlot) const;
	bool canApplyEvent(EventTypes eEvent) const;
	void applyEvent(EventTypes eEvent);
	int getImmobileTimer() const;
	void setImmobileTimer(int iNewValue);
	void changeImmobileTimer(int iChange);

	bool potentialWarAction(const CvPlot* pPlot) const;

	bool isAlwaysHostile(const CvPlot* pPlot) const;

	bool verifyStackValid();

	void setYieldStored(int iYieldAmount);
	int getYieldStored() const;
	YieldTypes getYield() const;
	bool isGoods() const;

	void changeBadCityDefenderCount(int iChange);
	int getBadCityDefenderCount() const;
	bool isCityDefender() const;
	void changeUnarmedCount(int iChange);
	int getUnarmedCount() const;

	int getUnitTravelTimer() const;
	void setUnitTravelTimer(int iValue);
	UnitTravelStates getUnitTravelState() const;
	void setUnitTravelState(UnitTravelStates eState, bool bShowEuropeScreen);

	bool setSailEurope(EuropeTypes eEurope);
	bool canSailEurope(EuropeTypes eEurope);

	void setHomeCity(CvCity* pNewValue);
	CvCity* getHomeCity() const;

	DllExport bool isOnMap() const;
	DllExport const CvArtInfoUnit* getArtInfo(int i) const;
	DllExport const TCHAR* getButton() const;
	const TCHAR* getFullLengthIcon() const;

	bool isColonistLocked();
	void setColonistLocked(bool bNewValue);

	// < JAnimals Mod Start >
	bool isBarbarian() const;
	void setBarbarian(bool bNewValue);
	// < JAnimals Mod End >

	bool raidWeapons(CvCity* pCity);
	bool raidWeapons(CvUnit* pUnit);
	bool raidGoods(CvCity* pCity);

	virtual void read(FDataStreamBase* pStream);
	virtual void write(FDataStreamBase* pStream);

	virtual void AI_init() = 0;
	virtual void AI_uninit() = 0;
	virtual void AI_reset() = 0;
	virtual bool AI_update() = 0;
	virtual bool AI_europeUpdate() = 0;
	virtual bool AI_follow() = 0;
	virtual void AI_upgrade() = 0;
	virtual void AI_promote() = 0;
	virtual int AI_groupFirstVal() = 0;
	virtual int AI_groupSecondVal() = 0;
	virtual int AI_attackOdds(const CvPlot* pPlot, bool bPotentialEnemy) const = 0;
	virtual bool AI_bestCityBuild(CvCity* pCity, CvPlot** ppBestPlot = NULL, BuildTypes* peBestBuild = NULL, CvPlot* pIgnorePlot = NULL, CvUnit* pUnit = NULL) = 0;
	virtual bool AI_isCityAIType() const = 0;
	virtual UnitAITypes AI_getUnitAIType() const = 0;
	virtual void AI_setUnitAIType(UnitAITypes eNewValue) = 0;
	virtual UnitAIStates AI_getUnitAIState() const = 0;
	virtual void AI_setUnitAIState(UnitAIStates eNewValue) = 0;
	virtual bool AI_hasAIChanged(int iNumTurns) = 0;
	virtual int AI_sacrificeValue(const CvPlot* pPlot) const = 0;
	virtual CvPlot* AI_determineDestination(CvPlot** ppMissionPlot, MissionTypes* peMission, MissionAITypes* peMissionAI) = 0;
	virtual bool AI_moveFromTransport(CvPlot* pHintPlot) = 0;
	virtual bool AI_attackFromTransport(CvPlot* pHintPlot, int iLowOddsThreshold, int iHighOddsThreshold) = 0;
	virtual int AI_getMovePriority() const = 0;
	virtual void AI_doInitialMovePriority() = 0;
	virtual void AI_setMovePriority(int iNewValue) = 0;
	///TKs Med
	virtual void AI_doFound(int iType = 0) = 0;
	///Tke
	virtual ProfessionTypes AI_getOldProfession() const = 0;
	virtual void AI_setOldProfession(ProfessionTypes eProfession) = 0;
	virtual ProfessionTypes AI_getIdealProfession() const = 0;

	/// Expert working - start - Nightinggale
	// tell if unit is working his expert profession(s)
	bool isCitizenExpertWorking() const;
	/// Expert working - end - Nightinggale

protected:

	int m_iID;
	int m_iGroupID;
	int m_iHotKeyNumber;
	int m_iX;
	int m_iY;
	int m_iLastMoveTurn;
	int m_iGameTurnCreated;
	int m_iDamage;
	int m_iMoves;
	int m_iExperience;
	int m_iLevel;
	int m_iCargo;
	int m_iCargoCapacity;
	int m_iAttackPlotX;
	int m_iAttackPlotY;
	int m_iCombatTimer;
	///TK FS
	int m_iTravelPlotX;
	int m_iTravelPlotY;
	int m_iCombatFirstStrikes;
	int m_iTrainCounter;
	int m_iTraderCode;
	int m_iImmuneToFirstStrikesCount;
	int m_iExtraFirstStrikes;
	int m_iExtraChanceFirstStrikes;
	int m_iCombatBlockParrys;
	int m_iEscortPromotion;
	int m_iPlotWorkedBonus;
	int m_iBuildingWorkedBonus;
	int m_iInvisibleTimer;
	bool m_bCrushingBlows;
	bool m_bGlancingBlows;
	bool m_bFreeBuilding;
	///TKe
	int m_iCombatDamage;
	int m_iFortifyTurns;
	int m_iBlitzCount;
	int m_iAmphibCount;
	int m_iRiverCount;
	int m_iEnemyRouteCount;
	int m_iAlwaysHealCount;
	int m_iHillsDoubleMoveCount;
	int m_iExtraVisibilityRange;
	int m_iExtraMoves;
	int m_iExtraMoveDiscount;
	int m_iExtraWithdrawal;
	int m_iExtraBombardRate;
	int m_iExtraEnemyHeal;
	int m_iExtraNeutralHeal;
	int m_iExtraFriendlyHeal;
	int m_iSameTileHeal;
	int m_iAdjacentTileHeal;
	int m_iExtraCombatPercent;
	int m_iExtraCityAttackPercent;
	int m_iExtraCityDefensePercent;
	int m_iExtraHillsAttackPercent;
	int m_iExtraHillsDefensePercent;
	int m_iPillageChange;
	int m_iUpgradeDiscount;
	int m_iExperiencePercent;
	int m_iBaseCombat;
	DirectionTypes m_eFacingDirection;
	int m_iImmobileTimer;
	int m_iYieldStored;
	int m_iExtraWorkRate;
	int m_iUnitTravelTimer;
	int m_iBadCityDefenderCount;
	int m_iUnarmedCount;

	bool m_bMadeAttack;
	bool m_bPromotionReady;
	bool m_bDeathDelay;
	bool m_bCombatFocus;
	bool m_bInfoBarDirty;
	bool m_bColonistLocked;
	// < JAnimals Mod Start >
	bool m_bBarbarian;
	// < JAnimals Mod End >

	PlayerTypes m_eOwner;
	PlayerTypes m_eCapturingPlayer;
	UnitTypes m_eUnitType;
	UnitTypes m_eLeaderUnitType;
	///TKs Invention Core Mod v 1.0
	UnitTypes m_ConvertToUnit;
	///TKe
	CvUnitInfo *m_pUnitInfo;
	ProfessionTypes m_eProfession;
	UnitTravelStates m_eUnitTravelState;

	IDInfo m_combatUnit;
	IDInfo m_transportUnit;
	IDInfo m_homeCity;
	int m_iPostCombatPlotIndex;

	int* m_aiExtraDomainModifier;

	CvWString m_szName;
	CvString m_szScriptData;

	PromotionArray<bool> m_ja_bHasRealPromotion;
	PromotionArray<unsigned char> m_ja_iFreePromotionCount;
	int* m_paiTerrainDoubleMoveCount;
	///TKs Med
	EuropeTypes m_eUnitTradeMarket;
	int* m_paiAltEquipmentTypes;
	///TKe
	int* m_paiFeatureDoubleMoveCount;
	int* m_paiExtraTerrainAttackPercent;
	int* m_paiExtraTerrainDefensePercent;
	int* m_paiExtraFeatureAttackPercent;
	int* m_paiExtraFeatureDefensePercent;
	int* m_paiExtraUnitClassAttackModifier;
	int* m_paiExtraUnitClassDefenseModifier;
	int* m_paiExtraUnitCombatModifier;

	bool canAdvance(const CvPlot* pPlot, int iThreshold) const;

	int planBattle( CvBattleDefinition & kBattleDefinition ) const;
	int computeUnitsToDie( const CvBattleDefinition & kDefinition, bool bRanged, BattleUnitTypes iUnit ) const;
	bool verifyRoundsValid( const CvBattleDefinition & battleDefinition ) const;
	void increaseBattleRounds( CvBattleDefinition & battleDefinition ) const;
	int computeWaveSize( bool bRangedRound, int iAttackerMax, int iDefenderMax ) const;

	void getDefenderCombatValues(CvUnit& kDefender, const CvPlot* pPlot, int iOurStrength, int iOurFirepower, int& iTheirOdds, int& iTheirStrength, int& iOurDamage, int& iTheirDamage, CombatDetails* pTheirDetails = NULL) const;

	bool isCombatVisible(const CvUnit* pDefender) const;
	void resolveCombat(CvUnit* pDefender, CvPlot* pPlot, CvBattleDefinition& kBattle);

	void doUnitTravelTimer();
	void processPromotion(PromotionTypes ePromotion, int iChange);
	UnitCombatTypes getProfessionUnitCombatType(ProfessionTypes eProfession) const;
	bool hasUnitCombatType(UnitCombatTypes eUnitCombat) const; // CombatGearTypes - Nightinggale
	void processUnitCombatType(UnitCombatTypes eUnitCombat, int iChange);
	void doUnloadYield(int iAmount);
	bool raidWeapons(std::vector<int>& aYields);
};


#endif
