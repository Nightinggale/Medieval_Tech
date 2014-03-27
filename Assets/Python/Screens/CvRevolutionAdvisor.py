## Sid Meier's Civilization 4
## Copyright Firaxis Games 2008

from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums
#import re

# Modified by Solver for Col v1

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

# this class is shared by both the resource and technology foreign advisors
class CvRevolutionAdvisor:
	"Revolution Advisor Screen"

	def __init__(self):
		self.WIDGET_ID = "RevolutionAdvisorWidget"
		self.LINE_ID = "RevolutionAdvisorLine"
		self.SCREEN_NAME = "RevolutionAdvisor"
		self.DEBUG_DROPDOWN_ID =  "RevolutionAdvisorDropdownWidget"
		self.BACKGROUND_ID = "RevolutionAdvisorBackground"

		self.XResolution = 0
		self.YResolution = 0
		
	def interfaceScreen (self):

		self.player = gc.getPlayer(gc.getGame().getActivePlayer())
		if self.player.getParent() == PlayerTypes.NO_PLAYER:
			return
		self.parent = gc.getPlayer(self.player.getParent())
		if self.parent == PlayerTypes.NO_PLAYER:
			return
	
		screen = self.getScreen()
		if screen.isActive():
			return
	
		self.XResolution = self.getScreen().getXResolution()
		self.YResolution = self.getScreen().getYResolution()

		screen.setRenderInterfaceOnly(True);
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)
	
		# Set the background and exit button, and show the screen
		screen.setDimensions(0, 0, self.XResolution, self.YResolution)
		screen.addDDSGFC(self.BACKGROUND_ID, ArtFileMgr.getInterfaceArtInfo("INTERFACE_REVOLUTION_BG").getPath(), 0, 0, self.XResolution, self.YResolution, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDDSGFC("TopPanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TITLE").getPath(), 0, 0, self.XResolution, 55, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDDSGFC("BottomPanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TAB_OFF").getPath(), 0, self.YResolution - 55, self.XResolution, 55, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.showWindowBackground(False)

		self.SCREEN_TITLE = u"<font=4b>" + localText.getText("TXT_KEY_REVOLUTION_ADVISOR_TITLE", ()).upper() + u"</font>"
		self.SCREEN_TITLE = localText.changeTextColor(self.SCREEN_TITLE, gc.getInfoTypeForString("COLOR_FONT_CREAM"))

		screen.setText( "Title", "Background", self.SCREEN_TITLE, CvUtil.FONT_CENTER_JUSTIFY, self.XResolution / 2, 4, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setText( "RevolutionScreenExit", "Background", u"<font=4>" + CyTranslator().getText("TXT_KEY_PEDIA_SCREEN_EXIT", ()).upper() + "</font>", CvUtil.FONT_RIGHT_JUSTIFY, self.XResolution - 30, self.YResolution - 36, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1 )
		screen.setActivation( "RevolutionScreenExit", ActivationTypes.ACTIVATE_MIMICPARENTFOCUS )

		self.Y_REVOLUTION = self.YResolution / 13
		self.W_REVOLUTION = self.XResolution * 2 / 5
		
		self.Y_REBEL_BAR = self.YResolution / 6
		
		self.Y_UNITS_LISTS = self.YResolution * 33 / 100
		self.X_ROYAL_UNITS = self.XResolution * 58 / 100
		self.X_COLONIAL_UNITS = self.XResolution  * 3 / 20
		
		self.BAR_SIDE_MARGIN = self.YResolution / 9
		self.BAR_END_ICON_SIZE = self.YResolution / 13
		self.ICON_BUTTON_SIZE = self.YResolution / 14
		
		self.REVOLUTION_BUTTON = 1776
		self.AMENDMENT_BUTTON = 1778

		#Debug PullDown
		if ( CyGame().isDebugMode() ):
			screen.addDropDownBoxGFC( "CivDropDown", 22, 12, 192, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.SMALL_FONT )
			screen.setActivation( "CivDropDown", ActivationTypes.ACTIVATE_MIMICPARENTFOCUS )

			for j in range(gc.getMAX_PLAYERS()):
				if (gc.getPlayer(j).isAlive()):
					screen.addPullDownString( "CivDropDown", gc.getPlayer(j).getName(), j, j, False )
		else:
			screen.hide( "CivDropDown" )

		# Draw Contents
		self.drawBar()
		self.drawRoyalTroops()
		self.drawPlayerTroops()
		#self.drawColonialTroops()
		self.drawWarMaterials()
		#self.drawCivics()
		
		return 0

	def getScreen(self):
		return CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.REVOLUTION_ADVISOR)

	def drawBar(self):
		screen = self.getScreen()	

		if not self.player.isInRevolution() and gc.getGame().getGameState() == GameStateTypes.GAMESTATE_ON:
			screen.setImageButton("RevolutionButton", ArtFileMgr.getInterfaceArtInfo("SCREEN_DATE_BOX").getPath(), (self.XResolution - self.W_REVOLUTION) / 2, self.Y_REVOLUTION, self.W_REVOLUTION, self.BAR_END_ICON_SIZE * 3 / 2, WidgetTypes.WIDGET_GENERAL, self.REVOLUTION_BUTTON, -1)
			#screen.setTextAt( "RevolutionButtonText", "RevolutionButton", "<font=4>" + localText.getText("INTERFACE_REVOLUTION", ()) + "</font>", CvUtil.FONT_CENTER_JUSTIFY, self.W_REVOLUTION / 2, self.BAR_END_ICON_SIZE * 3 / 4, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, self.REVOLUTION_BUTTON, -1 )
			screen.setTextAt( "RevolutionButtonText", "RevolutionButton", "<font=4>" + self.player.getDeclareKey() + "</font>", CvUtil.FONT_CENTER_JUSTIFY, self.W_REVOLUTION / 2, self.BAR_END_ICON_SIZE * 3 / 4, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, self.REVOLUTION_BUTTON, -1 )
		
			screen.enable("RevolutionButton", gc.getTeam(self.player.getTeam()).canDoRevolution())
			screen.enable("RevolutionButtonText", gc.getTeam(self.player.getTeam()).canDoRevolution())
			
		self.BarHight = self.BAR_END_ICON_SIZE / 2
		
		screen.addDDSGFC( "RebelIcon", ArtFileMgr.getCivilizationArtInfo(gc.getCivilizationInfo(self.player.getCivilizationType()).getArtDefineTag()).getButton(), self.BAR_SIDE_MARGIN, self.Y_REBEL_BAR, self.BAR_END_ICON_SIZE, self.BAR_END_ICON_SIZE, WidgetTypes.WIDGET_HELP_REBEL, 0, -1)
		screen.addDDSGFC( "LoyalIcon", gc.getYieldInfo(YieldTypes.YIELD_BELLS).getButton(), self.XResolution - (self.BAR_SIDE_MARGIN + self.BAR_END_ICON_SIZE), self.Y_REBEL_BAR, self.BAR_END_ICON_SIZE, self.BAR_END_ICON_SIZE, WidgetTypes.WIDGET_HELP_REBEL,  0, -1)

		BarWidth = self.XResolution - ((self.BAR_SIDE_MARGIN  + self.BAR_END_ICON_SIZE) * 2)
		screen.addStackedBarGFC("RebelBar", self.BAR_SIDE_MARGIN + self.BAR_END_ICON_SIZE, self.Y_REBEL_BAR + ((self.BAR_END_ICON_SIZE - self.BarHight) / 2), BarWidth, self.BarHight, InfoBarTypes.NUM_INFOBAR_TYPES, WidgetTypes.WIDGET_HELP_REBEL, 0, -1 )
		screen.setStackedBarColors("RebelBar", InfoBarTypes.INFOBAR_STORED, gc.getInfoTypeForString("COLOR_RED") )
		screen.setStackedBarColors("RebelBar", InfoBarTypes.INFOBAR_RATE, gc.getInfoTypeForString("COLOR_RED") )
		screen.setStackedBarColors("RebelBar", InfoBarTypes.INFOBAR_RATE_EXTRA, gc.getInfoTypeForString("COLOR_EMPTY") )
		screen.setStackedBarColors("RebelBar", InfoBarTypes.INFOBAR_EMPTY, gc.getInfoTypeForString("COLOR_EMPTY") )
		RebelPercentage = (gc.getTeam(self.player.getTeam()).getRebelPercent()) / 100.0
		screen.setBarPercentage("RebelBar", InfoBarTypes.INFOBAR_STORED, RebelPercentage)

		screen.addDDSGFC( "GoldButton",ArtFileMgr.getInterfaceArtInfo("INTERFACE_REVOLUTION_GOLD_BUTTON").getPath(), (self.XResolution / 2) - (self.BAR_END_ICON_SIZE / 2), self.Y_REBEL_BAR, self.BAR_END_ICON_SIZE, self.BAR_END_ICON_SIZE, WidgetTypes.WIDGET_HELP_REBEL, 0, -1)
		screen.addDDSGFC( "Medalion",ArtFileMgr.getInterfaceArtInfo("INTERFACE_REVOLUTION_BROWN_BUTTON").getPath(), self.BAR_SIDE_MARGIN + self.BAR_END_ICON_SIZE + int(RebelPercentage * BarWidth) - (self.BAR_END_ICON_SIZE / 2), self.Y_REBEL_BAR, self.BAR_END_ICON_SIZE, self.BAR_END_ICON_SIZE, WidgetTypes.WIDGET_HELP_REBEL, 0, -1)

		screen.setLabelAt( "RebelBarText", "Medalion", "<font=3>" + str(gc.getTeam(self.player.getTeam()).getRebelPercent()) + " %</font>", CvUtil.FONT_CENTER_JUSTIFY, self.BAR_END_ICON_SIZE / 2, self.BAR_END_ICON_SIZE / 2, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		
		return 0

	def drawWarMaterials(self):
		screen = self.getScreen()
		#Tks Med
		CityCount, iMuskets, iHorses, iLeather, iScale, iMail, iPlate = 0, 0, 0, 0, 0, 0, 0
		(city, iter) = self.player.firstCity(true)
		while(city):
			iHorses += city.getYieldStored(gc.getInfoTypeForString("YIELD_HORSES"))
			iMuskets += city.getYieldStored(gc.getInfoTypeForString("YIELD_WEAPONS"))
			iLeather += city.getYieldStored(gc.getInfoTypeForString("YIELD_LEATHER_ARMOR"))
			iScale += city.getYieldStored(gc.getInfoTypeForString("YIELD_SCALE_ARMOR"))
			iMail += city.getYieldStored(gc.getInfoTypeForString("YIELD_MAIL_ARMOR"))
			iPlate += city.getYieldStored(gc.getInfoTypeForString("YIELD_PLATE_ARMOR"))
			(city, iter) = self.player.nextCity(iter, true)
		iNobles = 0
		for iLoopPlayer in range(gc.getMAX_PLAYERS()):
			loopPlayer = gc.getPlayer(iLoopPlayer)	
			if loopPlayer.getTeam() == self.player.getTeam():
				iNobles += loopPlayer.getUnitClassCount(gc.getDefineINT("DEFAULT_NOBLEMAN_CLASS"))
		yMod1 = 1
		yMod2 = 9
		eNoble = gc.getCivilizationInfo(self.player.getCivilizationType()).getCivilizationUnits(gc.getDefineINT("DEFAULT_NOBLEMAN_CLASS"))
		UnitInfo = gc.getUnitInfo(eNoble)
		iUnitArtStyle = gc.getCivilizationInfo(self.player.getCivilizationType()).getUnitArtStyleType()
		screen.addDDSGFC( "NobleIcon", UnitInfo.getUnitArtStylesArtInfo(0, 0, iUnitArtStyle).getButton(), self.ICON_BUTTON_SIZE / 4, (self.YResolution * (yMod1) / yMod2), self.ICON_BUTTON_SIZE, self.ICON_BUTTON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1)
		NobleText = str(iNobles)
		NobleText = localText.changeTextColor(NobleText, gc.getInfoTypeForString("COLOR_FONT_GOLD"))
		screen.setLabel( "Noble Count", "Background", u"<font=4>" + NobleText + "</font>", CvUtil.FONT_CENTER_JUSTIFY, self.ICON_BUTTON_SIZE * 3 / 4, (self.YResolution * (yMod1) / yMod2) + self.ICON_BUTTON_SIZE, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		screen.addDDSGFC( "HorseIcon", gc.getYieldInfo(gc.getInfoTypeForString("YIELD_HORSES")).getIcon(), self.ICON_BUTTON_SIZE / 4, (self.YResolution * (yMod1 + 1) / yMod2), self.ICON_BUTTON_SIZE, self.ICON_BUTTON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1)
		HorseText = str(iHorses)
		HorseText = localText.changeTextColor(HorseText, gc.getInfoTypeForString("COLOR_FONT_GOLD"))
		screen.setLabel( "Horse Count", "Background", u"<font=4>" + HorseText + "</font>", CvUtil.FONT_CENTER_JUSTIFY, self.ICON_BUTTON_SIZE * 3 / 4, (self.YResolution * (yMod1 + 1) / yMod2) + self.ICON_BUTTON_SIZE, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
	
		screen.addDDSGFC( "MusketIcon", gc.getYieldInfo(gc.getInfoTypeForString("YIELD_WEAPONS")).getIcon(), self.ICON_BUTTON_SIZE / 4, (self.YResolution * (yMod1 + 2) / yMod2), self.ICON_BUTTON_SIZE, self.ICON_BUTTON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1)
		GunText = str(iMuskets) 
		GunText = localText.changeTextColor(GunText, gc.getInfoTypeForString("COLOR_FONT_GOLD"))
		screen.setLabel( "Musket Count", "Background", u"<font=4>" + GunText + "</font>", CvUtil.FONT_CENTER_JUSTIFY, self.ICON_BUTTON_SIZE * 3 / 4, (self.YResolution * (yMod1 + 2) / yMod2) + self.ICON_BUTTON_SIZE, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		
		screen.addDDSGFC( "LeatherIcon", gc.getYieldInfo(gc.getInfoTypeForString("YIELD_LEATHER_ARMOR")).getIcon(), self.ICON_BUTTON_SIZE / 4, (self.YResolution * (yMod1 + 3) / yMod2), self.ICON_BUTTON_SIZE, self.ICON_BUTTON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1)
		LeatherText = str(iLeather) 
		LeatherText = localText.changeTextColor(LeatherText, gc.getInfoTypeForString("COLOR_FONT_GOLD"))
		screen.setLabel( "Leather Count", "Background", u"<font=4>" + LeatherText + "</font>", CvUtil.FONT_CENTER_JUSTIFY, self.ICON_BUTTON_SIZE * 3 / 4, (self.YResolution * (yMod1 + 3) / yMod2) + self.ICON_BUTTON_SIZE, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		
		screen.addDDSGFC( "ScaleIcon", gc.getYieldInfo(gc.getInfoTypeForString("YIELD_SCALE_ARMOR")).getIcon(), self.ICON_BUTTON_SIZE / 4, (self.YResolution * (yMod1 + 4) / yMod2), self.ICON_BUTTON_SIZE, self.ICON_BUTTON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1)
		ScaleText = str(iScale) 
		ScaleText = localText.changeTextColor(ScaleText, gc.getInfoTypeForString("COLOR_FONT_GOLD"))
		screen.setLabel( "Scale Count", "Background", u"<font=4>" + ScaleText + "</font>", CvUtil.FONT_CENTER_JUSTIFY, self.ICON_BUTTON_SIZE * 3 / 4, (self.YResolution * (yMod1 + 4) / yMod2) + self.ICON_BUTTON_SIZE, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		
		screen.addDDSGFC( "MailIcon", gc.getYieldInfo(gc.getInfoTypeForString("YIELD_MAIL_ARMOR")).getIcon(), self.ICON_BUTTON_SIZE / 4, (self.YResolution * (yMod1 + 5) / yMod2), self.ICON_BUTTON_SIZE, self.ICON_BUTTON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1)
		MailText = str(iMail) 
		MailText = localText.changeTextColor(MailText, gc.getInfoTypeForString("COLOR_FONT_GOLD"))
		screen.setLabel( "Mail Count", "Background", u"<font=4>" + MailText + "</font>", CvUtil.FONT_CENTER_JUSTIFY, self.ICON_BUTTON_SIZE * 3 / 4, (self.YResolution * (yMod1 + 5) / yMod2) + self.ICON_BUTTON_SIZE, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		
		screen.addDDSGFC( "PlateIcon", gc.getYieldInfo(gc.getInfoTypeForString("YIELD_PLATE_ARMOR")).getIcon(), self.ICON_BUTTON_SIZE / 4, (self.YResolution * (yMod1 + 6) / yMod2), self.ICON_BUTTON_SIZE, self.ICON_BUTTON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1)
		PlateText = str(iPlate) 
		PlateText = localText.changeTextColor(PlateText, gc.getInfoTypeForString("COLOR_FONT_GOLD"))
		screen.setLabel( "Plate Count", "Background", u"<font=4>" + PlateText + "</font>", CvUtil.FONT_CENTER_JUSTIFY, self.ICON_BUTTON_SIZE * 3 / 4, (self.YResolution * (yMod1 + 6) / yMod2) + self.ICON_BUTTON_SIZE, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		
		#Tke
	def drawCivics(self):
		screen = self.getScreen()
	
		for iType in range(gc.getNumCivicOptionInfos()):
			#TKs Update 1.1b
			if (iType != 5):
				for iCivic in range(gc.getNumCivicInfos()):
					if (gc.getCivicInfo(iCivic).getCivicOptionType() == iType):
						if self.player.isCivic(iCivic):
							screen.addDDSGFC("ArcticleBox" + str(iType), gc.getCivicInfo(iCivic).getButton(), (self.XResolution / 2) - (self.ICON_BUTTON_SIZE * gc.getNumCivicOptionInfos() ) + (self.ICON_BUTTON_SIZE * iType * 2) + (self.ICON_BUTTON_SIZE / 2), self.Y_REVOLUTION * 11, self.ICON_BUTTON_SIZE, self.ICON_BUTTON_SIZE, WidgetTypes.WIDGET_GENERAL, self.AMENDMENT_BUTTON, iCivic)
							
							if self.player.isInRevolution():
								screen.hide("GoldButton")
							#TKe
							break
						
	def drawRoyalTroops(self):
		screen = self.getScreen()

		YMultiplier = int(self.ICON_BUTTON_SIZE * 1.3)
		#TKs Med
		RoyalText = localText.getText("INTERFACE_ROYAL_EXPEDITIONARY_FORCES", ())
		RoyalText = self.player.getAttackForceKey()
		RoyalText = localText.changeTextColor(RoyalText, gc.getInfoTypeForString("COLOR_FONT_GOLD"))
		screen.setLabel( "Royal Troops", "Background", u"<font=4>" + RoyalText + "</font>", CvUtil.FONT_LEFT_JUSTIFY, self.X_ROYAL_UNITS, self.Y_UNITS_LISTS + (YMultiplier * 0), 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		Troops, VeteranTroops, Dragoons, VeteranDragoons, Cannons, Ships = 0, 0, 0, 0, 0, 0
		iCount = 1
		CombatProfessions = []
		bProfessionFound = False
		iDisplayUnit = -1
		HighestBonus = 0
		X_Mod = 0
		X_Mod_Cout = 0
		Y_Mod = 1
		iIconSize = self.ICON_BUTTON_SIZE
		for iLoopPlayer in range(gc.getMAX_PLAYERS()):
			loopPlayer = gc.getPlayer(iLoopPlayer)
			if loopPlayer.getTeam() == self.player.getTeam():
				for iUnit in range(loopPlayer.getNumRevolutionEuropeUnits()):
					Profession = loopPlayer.getRevolutionEuropeProfession(iUnit)
					UnitType = loopPlayer.getRevolutionEuropeUnit(iUnit)
					if Profession >= 0:
						if gc.getProfessionInfo(Profession).getCombatChange() > 2:
							for iLoopProfession in range(len(CombatProfessions)):
								if CombatProfessions[iLoopProfession][0] == Profession:
									addProfession = CombatProfessions[iLoopProfession][1]
									CombatProfessions[iLoopProfession][1] = addProfession + 1
									bProfessionFound = True
									break
							if not bProfessionFound:
								CombatProfessions.append([Profession, 1])
								X_Mod_Cout += 1
								if X_Mod_Cout > 4:
									X_Mod += iIconSize * 2
									X_Mod_Cout = 1
									Y_Mod = 1
								#Parent = loopPlayer
								#for iLoopParent in range(gc.getMAX_PLAYERS()):
								#	loopParent = gc.getPlayer(iLoopParent)
								#	if gc.getTeam(loopParent.getTeam()).isParentOf(self.player.getTeam()):
								#		Parent = loopParent
								#		screen.setLabel( "Royal Troops", "Background", u"<font=4>" + RoyalText + "</font>", CvUtil.FONT_LEFT_JUSTIFY, self.X_ROYAL_UNITS, self.Y_UNITS_LISTS + (YMultiplier * 0), 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
								#		break
								#UnitType = gc.getCivilizationInfo(Parent.getCivilizationType()).getCivilizationUnits(gc.getUnitInfo(UnitType).getUnitClassType())
								UnitInfo = gc.getUnitInfo(UnitType)
								iUnitArtStyle = gc.getCivilizationInfo(loopPlayer.getCivilizationType()).getUnitArtStyleType()
								#screen.addDDSGFC("PlayerTroopIcon" + str(iCount), UnitInfo.getUnitArtStylesArtInfo(0, Profession, iUnitArtStyle).getButton(), self.X_COLONIAL_UNITS + X_Mod, self.Y_UNITS_LISTS + (YMultiplier * Y_Mod), iIconSize, iIconSize, WidgetTypes.WIDGET_PEDIA_JUMP_TO_PROFESSION, Profession, 1)
								screen.addDDSGFC("RoyalTroopIcon" + str(iCount), UnitInfo.getUnitArtStylesArtInfo(0, Profession, iUnitArtStyle).getButton(), self.X_ROYAL_UNITS + X_Mod, self.Y_UNITS_LISTS + (YMultiplier * Y_Mod), iIconSize, iIconSize, WidgetTypes.WIDGET_PEDIA_JUMP_TO_PROFESSION, Profession, 1)
								iCount += 1
								Y_Mod += 1

								
							bProfessionFound = False
					elif isCannon(UnitType):
						Cannons += 1
					elif isWarship(UnitType):
						Ships += 1
		for iLoopPlayer in range(gc.getMAX_PLAYERS()):
			loopPlayer = gc.getPlayer(iLoopPlayer)
			if gc.getTeam(loopPlayer.getTeam()).isParentOf(self.player.getTeam()):
				for i in range(loopPlayer.getNumEuropeUnits()):
					unit = loopPlayer.getEuropeUnit(i)
					Profession = unit.getProfession()
					UnitType = unit.getUnitType()
					if Profession >= 0:
						if gc.getProfessionInfo(Profession).getCombatChange() > 2:
							for iLoopProfession in range(len(CombatProfessions)):
								if CombatProfessions[iLoopProfession][0] == Profession:
									addProfession = CombatProfessions[iLoopProfession][1]
									CombatProfessions[iLoopProfession][1] = addProfession + 1
									bProfessionFound = True
									break
							if not bProfessionFound:
								CombatProfessions.append([Profession, 1])
								X_Mod_Cout += 1
								if X_Mod_Cout > 4:
									X_Mod += iIconSize * 2
									X_Mod_Cout = 1
									Y_Mod = 1
								UnitInfo = gc.getUnitInfo(UnitType)
								iUnitArtStyle = gc.getCivilizationInfo(loopPlayer.getCivilizationType()).getUnitArtStyleType()
								screen.addDDSGFC("RoyalTroopIcon" + str(iCount), UnitInfo.getUnitArtStylesArtInfo(0, Profession, iUnitArtStyle).getButton(), self.X_ROYAL_UNITS + X_Mod, self.Y_UNITS_LISTS + (YMultiplier * Y_Mod), iIconSize, iIconSize, WidgetTypes.WIDGET_PEDIA_JUMP_TO_PROFESSION, Profession, 1)
								iCount += 1
								Y_Mod += 1

								
							bProfessionFound = False
					elif isCannon(UnitType):
						Cannons += 1
					elif isWarship(UnitType):
						Ships += 1		
				(unit, iter) = loopPlayer.firstUnit()
				while(unit):
					Profession = unit.getProfession()
					UnitType = unit.getUnitType()
					if Profession >= 0:
						if gc.getProfessionInfo(Profession).getCombatChange() > 2:
							for iLoopProfession in range(len(CombatProfessions)):
								if CombatProfessions[iLoopProfession][0] == Profession:
									addProfession = CombatProfessions[iLoopProfession][1]
									CombatProfessions[iLoopProfession][1] = addProfession + 1
									bProfessionFound = True
									break
							if not bProfessionFound:
								CombatProfessions.append([Profession, 1])
								X_Mod_Cout += 1
								if X_Mod_Cout > 4:
									X_Mod += iIconSize * 2
									X_Mod_Cout = 1
									Y_Mod = 1
								UnitInfo = gc.getUnitInfo(UnitType)
								iUnitArtStyle = gc.getCivilizationInfo(loopPlayer.getCivilizationType()).getUnitArtStyleType()
								screen.addDDSGFC("RoyalTroopIcon" + str(iCount), UnitInfo.getUnitArtStylesArtInfo(0, Profession, iUnitArtStyle).getButton(), self.X_ROYAL_UNITS + X_Mod, self.Y_UNITS_LISTS + (YMultiplier * Y_Mod), iIconSize, iIconSize, WidgetTypes.WIDGET_PEDIA_JUMP_TO_PROFESSION, Profession, 1)
								iCount += 1
								Y_Mod += 1

								
							bProfessionFound = False
					
					elif isCannon(UnitType):
						Cannons += 1
					elif isWarship(UnitType):
						Ships += 1			
					(unit, iter) = loopPlayer.nextUnit(iter)
		
					
				iIconMod = len(CombatProfessions)
				X_Mod = 0
				X_Mod_Cout = 0
				Y_Mod = 1
				if iIconMod >= 1:
					iCount = 1
					#UnitType = gc.getCivilizationInfo(self.player.getCivilizationType()).getCivilizationUnits(gc.getDefineINT("DEFAULT_POPULATION_UNIT"))
					for iLoopProfession in range(len(CombatProfessions)):
						X_Mod_Cout += 1
						if X_Mod_Cout > 4:
							X_Mod += iIconSize * 2
							X_Mod_Cout = 1
							Y_Mod = 1
						Troops = CombatProfessions[iLoopProfession][1]
						#screen.addDDSGFC("PlayerTroopIcon" + str(iCount), gc.getUnitInfo(UnitType).getButton(), self.X_COLONIAL_UNITS, self.Y_UNITS_LISTS + (YMultiplier * iCount), iIconSize, iIconSize, WidgetTypes.WIDGET_GENERAL, -1, -1 )
						#TroopText = localText.getText("INTERFACE_REVOLUTION_SCREEN_SOLDIERS", (Troops, ))
						TroopText = localText.getText("INTERFACE_REVOLUTION_SCREEN_NUMBER_ONLY", (Troops, ))
						TroopText = localText.changeTextColor(TroopText, gc.getInfoTypeForString("COLOR_FONT_GOLD"))
						screen.setLabel( "RoyalTroopCount" + str(iCount), "Background", u"<font=3>" + TroopText + "</font>", CvUtil.FONT_LEFT_JUSTIFY, self.X_ROYAL_UNITS + X_Mod, self.Y_UNITS_LISTS + (YMultiplier * Y_Mod), 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )		#		Ships += 1				
						iCount += 1
						Y_Mod += 1
		screen.addDDSGFC("Cannon Icon", CyArtFileMgr().getUnitArtInfo("ART_DEF_UNIT_ARTILLERY").getButton(), self.X_ROYAL_UNITS, self.Y_UNITS_LISTS + (YMultiplier * 3), self.ICON_BUTTON_SIZE, self.ICON_BUTTON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		CannonText = localText.getText("INTERFACE_REVOLUTION_SCREEN_ARTY", (Cannons, ))
		CannonText = localText.changeTextColor(CannonText, gc.getInfoTypeForString("COLOR_BROWN_TEXT"))
		screen.setLabel( "Cannon Count", "Background", u"<font=3>" + CannonText + "</font>", CvUtil.FONT_LEFT_JUSTIFY, self.X_ROYAL_UNITS + (self.ICON_BUTTON_SIZE * 5 / 4), self.Y_UNITS_LISTS + (YMultiplier * 3), 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		
		screen.addDDSGFC("Ship Icon", CyArtFileMgr().getUnitArtInfo("ART_DEF_UNIT_MAN_O_WAR").getButton(), self.X_ROYAL_UNITS, self.Y_UNITS_LISTS + (YMultiplier * 4), self.ICON_BUTTON_SIZE, self.ICON_BUTTON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		ShipText = localText.getText("INTERFACE_WARSHIPS", (Ships,))
		ShipText = localText.changeTextColor(ShipText, gc.getInfoTypeForString("COLOR_BROWN_TEXT"))
		screen.setLabel( "Ship Count", "Background", u"<font=3>" + ShipText + "</font>", CvUtil.FONT_LEFT_JUSTIFY, self.X_ROYAL_UNITS + (self.ICON_BUTTON_SIZE * 5 / 4), self.Y_UNITS_LISTS + (YMultiplier * 4), 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		#Troops, Dragoons, Cannons, Ships = 0, 0, 0, 0
		#
		#for iLoopPlayer in range(gc.getMAX_PLAYERS()):
		#	loopPlayer = gc.getPlayer(iLoopPlayer)
		#	if loopPlayer.getTeam() == self.player.getTeam():
		#		for iUnit in range(loopPlayer.getNumRevolutionEuropeUnits()):
		#			Profession = loopPlayer.getRevolutionEuropeProfession(iUnit)
		#			UnitType = loopPlayer.getRevolutionEuropeUnit(iUnit)			
		#			if isSoldier(Profession):
		#				Troops += 1
		#			if isDragoon(Profession):
		#				Dragoons += 1
		#			if isCannon(UnitType):
		#				Cannons += 1
		#			if isWarship(UnitType):
		#				Ships += 1
		#			
		#for iLoopParent in range(gc.getMAX_PLAYERS()):
		#	loopParent = gc.getPlayer(iLoopParent)
		#
		#	if gc.getTeam(loopParent.getTeam()).isParentOf(self.player.getTeam()):
		#
		#		for i in range(loopParent.getNumEuropeUnits()):
		#			unit = loopParent.getEuropeUnit(i)
		#			Profession = unit.getProfession()
		#			UnitType = unit.getUnitType()
		#			if isSoldier(Profession):
		#				Troops += 1
		#			if isDragoon(Profession):
		#				Dragoons += 1
		#			if isCannon(UnitType):
		#				Cannons += 1
		#			if isWarship(UnitType):
		#				Ships += 1				
		#
		#		#(unit, iter) = loopParent.firstUnit()
		#		#while(unit):
		#		#	Profession = unit.getProfession()
		#		#	UnitType = unit.getUnitType()
		#		#	if isSoldier(Profession):
		#		#		Troops += 1
		#		#	if isDragoon(Profession):
		#		#		Dragoons += 1
		#		#	if isCannon(UnitType):
		#		#		Cannons += 1
		#		#	if isWarship(UnitType):
		#		#		Ships += 1				
		#		#	(unit, iter) = loopParent.nextUnit(iter)
		#
		#screen.addDDSGFC("Troop Icon", CyArtFileMgr().getUnitArtInfo("ART_DEF_UNIT_IMPERIAL_REGULAR").getButton(), self.X_ROYAL_UNITS, self.Y_UNITS_LISTS + (YMultiplier * 1), self.ICON_BUTTON_SIZE, self.ICON_BUTTON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		#TroopText = localText.getText("INTERFACE_REVOLUTION_SCREEN_REGULARS", (Troops, ))
		#TroopText = localText.changeTextColor(TroopText, gc.getInfoTypeForString("COLOR_BROWN_TEXT"))
		#screen.setLabel( "Troop Count", "Background", u"<font=3>" + TroopText + "</font>", CvUtil.FONT_LEFT_JUSTIFY, self.X_ROYAL_UNITS + (self.ICON_BUTTON_SIZE * 5 / 4), self.Y_UNITS_LISTS + (YMultiplier * 1), 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		#
		#screen.addDDSGFC("Dragoon Icon", CyArtFileMgr().getUnitArtInfo("ART_DEF_UNIT_PROFESSION_IMPERIAL_HEAVY_CALVARY").getButton(), self.X_ROYAL_UNITS, self.Y_UNITS_LISTS + (YMultiplier * 2), self.ICON_BUTTON_SIZE, self.ICON_BUTTON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		#GoonText = localText.getText("INTERFACE_REVOLUTION_SCREEN_REGULAR_DRAGOONS", (Dragoons, ))
		#GoonText = localText.changeTextColor(GoonText, gc.getInfoTypeForString("COLOR_BROWN_TEXT"))
		#screen.setLabel( "Dragoon Count", "Background", u"<font=3>" + GoonText + "</font>", CvUtil.FONT_LEFT_JUSTIFY, self.X_ROYAL_UNITS + (self.ICON_BUTTON_SIZE * 5 / 4), self.Y_UNITS_LISTS + (YMultiplier * 2), 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		#
		

	def drawPlayerTroops(self):
		screen = self.getScreen()

		YMultiplier = int(self.ICON_BUTTON_SIZE * 1.3)
		#print str(YMultiplier)
		
		RebelText = localText.getText("INTERFACE_COLONIAL_FORCES", (gc.getCivilizationInfo(self.player.getCivilizationType()).getAdjectiveKey(),0))
		RebelText = localText.changeTextColor(RebelText, gc.getInfoTypeForString("COLOR_FONT_GOLD"))
		screen.setLabel( "Colonial Troops", "Background", u"<font=4>" + RebelText + "</font>", CvUtil.FONT_LEFT_JUSTIFY, self.X_COLONIAL_UNITS, self.Y_UNITS_LISTS + (YMultiplier * 0), 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
	
		Troops, VeteranTroops, Dragoons, VeteranDragoons, Cannons, Ships = 0, 0, 0, 0, 0, 0
		iCount = 1
		CombatProfessions = []
		bProfessionFound = False
		iDisplayUnit = -1
		HighestBonus = 0
		X_Mod = 0
		X_Mod_Cout = 0
		Y_Mod = 1
		iIconSize = self.ICON_BUTTON_SIZE
		iNobles = 0
		for iLoopPlayer in range(gc.getMAX_PLAYERS()):
			loopPlayer = gc.getPlayer(iLoopPlayer)
			
			if loopPlayer.getTeam() == self.player.getTeam():
				(unit, iter) = loopPlayer.firstUnit()
				while(unit):
					Profession = unit.getProfession()
					UnitType = unit.getUnitType()
					if Profession >= 0:
						if gc.getProfessionInfo(Profession).getCombatChange() > 2:
							for iLoopProfession in range(len(CombatProfessions)):
								if CombatProfessions[iLoopProfession][0] == Profession:
									addProfession = CombatProfessions[iLoopProfession][1]
									CombatProfessions[iLoopProfession][1] = addProfession + 1
									bProfessionFound = True
									break
							if not bProfessionFound:
								CombatProfessions.append([Profession, 1])
								X_Mod_Cout += 1
								if X_Mod_Cout > 4:
									X_Mod += iIconSize * 2
									X_Mod_Cout = 1
									Y_Mod = 1
								UnitInfo = gc.getUnitInfo(UnitType)
								iUnitArtStyle = gc.getCivilizationInfo(self.player.getCivilizationType()).getUnitArtStyleType()
								screen.addDDSGFC("PlayerTroopIcon" + str(iCount), UnitInfo.getUnitArtStylesArtInfo(0, Profession, iUnitArtStyle).getButton(), self.X_COLONIAL_UNITS + X_Mod, self.Y_UNITS_LISTS + (YMultiplier * Y_Mod), iIconSize, iIconSize, WidgetTypes.WIDGET_PEDIA_JUMP_TO_PROFESSION, Profession, 1)
								iCount += 1
								Y_Mod += 1

								
							bProfessionFound = False
					
					if isCannon(UnitType):
						Cannons += 1
					if isWarship(UnitType):
						Ships += 1			
					(unit, iter) = loopPlayer.nextUnit(iter)
		
					
				iIconMod = len(CombatProfessions)
				X_Mod = 0
				X_Mod_Cout = 0
				Y_Mod = 1
				iCount = 1
				if iIconMod >= 1:
					
					#UnitType = gc.getCivilizationInfo(self.player.getCivilizationType()).getCivilizationUnits(gc.getDefineINT("DEFAULT_POPULATION_UNIT"))
					for iLoopProfession in range(len(CombatProfessions)):
						X_Mod_Cout += 1
						if X_Mod_Cout > 4:
							X_Mod += iIconSize * 2
							X_Mod_Cout = 1
							Y_Mod = 1
						Troops = CombatProfessions[iLoopProfession][1]
						#screen.addDDSGFC("PlayerTroopIcon" + str(iCount), gc.getUnitInfo(UnitType).getButton(), self.X_COLONIAL_UNITS, self.Y_UNITS_LISTS + (YMultiplier * iCount), iIconSize, iIconSize, WidgetTypes.WIDGET_GENERAL, -1, -1 )
						#TroopText = localText.getText("INTERFACE_REVOLUTION_SCREEN_SOLDIERS", (Troops, ))
						TroopText = localText.getText("INTERFACE_REVOLUTION_SCREEN_NUMBER_ONLY", (Troops, ))
						TroopText = localText.changeTextColor(TroopText, gc.getInfoTypeForString("COLOR_FONT_GOLD"))
						screen.setLabel( "TroopCount" + str(iCount), "Background", u"<font=3>" + TroopText + "</font>", CvUtil.FONT_LEFT_JUSTIFY, self.X_COLONIAL_UNITS + X_Mod, self.Y_UNITS_LISTS + (YMultiplier * Y_Mod), 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )		#		Ships += 1				
						iCount += 1
						Y_Mod += 1
				if Cannons > 0:
					#if X_Mod_Cout > 0:
					#	X_Mod += iIconSize * 2
					#	Y_Mod = 1
						
					screen.addDDSGFC("Rebel Cannon Icon", CyArtFileMgr().getUnitArtInfo("ART_DEF_UNIT_TREBUCHET").getButton(), self.X_COLONIAL_UNITS + X_Mod, self.Y_UNITS_LISTS + (YMultiplier * Y_Mod), iIconSize, iIconSize, WidgetTypes.WIDGET_GENERAL, -1, -1 )				
					#CannonText = localText.getText("INTERFACE_REVOLUTION_SCREEN_CANNONS", (Cannons, ))
					#CannonText = localText.changeTextColor(CannonText, gc.getInfoTypeForString("COLOR_BROWN_TEXT"))
					CannonText = localText.getText("INTERFACE_REVOLUTION_SCREEN_NUMBER_ONLY", (Cannons, ))
					CannonText = localText.changeTextColor(CannonText, gc.getInfoTypeForString("COLOR_FONT_GOLD"))
					screen.setLabel( "Rebel Cannon Count", "Background", u"<font=3>" + CannonText + "</font>", CvUtil.FONT_LEFT_JUSTIFY, self.X_COLONIAL_UNITS + X_Mod, self.Y_UNITS_LISTS + (YMultiplier * Y_Mod), 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
				#	
				#	screen.addDDSGFC("Rebel Ship Icon", CyArtFileMgr().getUnitArtInfo("ART_DEF_UNIT_SHIP_OF_THE_LINE").getButton(), self.X_COLONIAL_UNITS, self.Y_UNITS_LISTS + (YMultiplier * 4), self.ICON_BUTTON_SIZE, self.ICON_BUTTON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1 )
				#	ShipText = localText.getText("INTERFACE_WARSHIPS", (Ships,))
				#	ShipText = localText.changeTextColor(ShipText, gc.getInfoTypeForString("COLOR_BROWN_TEXT"))
				#	screen.setLabel( "Rebel Ship Count", "Background", u"<font=3>" + ShipText + "</font>", CvUtil.FONT_LEFT_JUSTIFY, self.X_COLONIAL_UNITS + (self.ICON_BUTTON_SIZE * 5 / 4), self.Y_UNITS_LISTS + (YMultiplier * 4), 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		
		#en.addDDSGFC("Rebel Troop Icon", CyArtFileMgr().getUnitArtInfo("ART_DEF_UNIT_PROFESSION_SOLDIER").getButton(), self.X_COLONIAL_UNITS, self.Y_UNITS_LISTS + (YMultiplier * 1), self.ICON_BUTTON_SIZE, self.ICON_BUTTON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1 )
			
		#TroopText = localText.getText("INTERFACE_REVOLUTION_SCREEN_SOLDIERS", (Troops, ))
		#TroopText = localText.changeTextColor(TroopText, gc.getInfoTypeForString("COLOR_BROWN_TEXT"))
		#screen.setLabel( "Rebel Troop Count", "Background", u"<font=3>" + TroopText + "</font>", CvUtil.FONT_LEFT_JUSTIFY, self.X_COLONIAL_UNITS + (self.ICON_BUTTON_SIZE * 5 / 4), self.Y_UNITS_LISTS + (YMultiplier * 1), 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		#szVeteranString = localText.getText("INTERFACE_REVOLUTION_SCREEN_VETERAN_MILITIA", (VeteranTroops, Troops - VeteranTroops))
		#szVeteranString = localText.changeTextColor(szVeteranString, gc.getInfoTypeForString("COLOR_BROWN_TEXT"))
		#screen.setLabel( "Rebel Veteran Troop Count", "Background", u"<font=3>" + szVeteranString + "</font>", CvUtil.FONT_LEFT_JUSTIFY, self.X_COLONIAL_UNITS + (self.ICON_BUTTON_SIZE * 5 / 4), self.Y_UNITS_LISTS + int(YMultiplier * 1.4), 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		#
		#if VeteranDragoons > 0:
		#	screen.addDDSGFC("Rebel Draggon Icon", CyArtFileMgr().getUnitArtInfo("ART_DEF_UNIT_PROFESSION_UNIT_VETERAN_DRAGOON").getButton(), self.X_COLONIAL_UNITS, self.Y_UNITS_LISTS + (YMultiplier * 2), self.ICON_BUTTON_SIZE, self.ICON_BUTTON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		#else:
		#	screen.addDDSGFC("Rebel Draggon Icon", CyArtFileMgr().getUnitArtInfo("ART_DEF_UNIT_PROFESSION_DRAGOON").getButton(), self.X_COLONIAL_UNITS, self.Y_UNITS_LISTS + (YMultiplier * 2), self.ICON_BUTTON_SIZE, self.ICON_BUTTON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		#
		#GoonText = localText.getText("INTERFACE_REVOLUTION_SCREEN_DRAGOONS", (Dragoons, ))
		#GoonText = localText.changeTextColor(GoonText, gc.getInfoTypeForString("COLOR_BROWN_TEXT"))
		#screen.setLabel( "Rebel Dragoon Count", "Background", u"<font=3>" + GoonText + "</font>", CvUtil.FONT_LEFT_JUSTIFY, self.X_COLONIAL_UNITS + (self.ICON_BUTTON_SIZE * 5 / 4), self.Y_UNITS_LISTS + (YMultiplier * 2), 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		#szVeteranDragoonString = localText.getText("INTERFACE_REVOLUTION_SCREEN_VETERAN_MILITIA", (VeteranDragoons, Dragoons - VeteranDragoons))
		#szVeteranDragoonString = localText.changeTextColor(szVeteranDragoonString, gc.getInfoTypeForString("COLOR_BROWN_TEXT"))
		#screen.setLabel( "Rebel Veteran Dragoon Count", "Background", u"<font=3>" + szVeteranDragoonString + "</font>", CvUtil.FONT_LEFT_JUSTIFY, self.X_COLONIAL_UNITS + (self.ICON_BUTTON_SIZE * 5 / 4), self.Y_UNITS_LISTS + int(YMultiplier * 2.4), 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		
		
		
	def handleInput(self, inputClass):
	
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED):
			self.CivDropDown(inputClass)
			return 1

		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED):
			if (inputClass.getData1() == self.REVOLUTION_BUTTON):
				CyMessageControl().sendChangeWar(gc.getPlayer(self.player.getParent()).getTeam(), true)
				self.getScreen().hideScreen()
			
		return 0

	def update(self, fDelta):
		return 0
		
	def getWidgetHelp(self, argsList):
		iScreen, eWidgetType, iData1, iData2, bOption = argsList
		#TKs
		self.player = gc.getPlayer(gc.getGame().getActivePlayer())
		if eWidgetType == WidgetTypes.WIDGET_GENERAL and iData1 == self.AMENDMENT_BUTTON:
			
			szHelp = CyGameTextMgr().parseCivicInfo(iData2, True, False, True, False, self.player.getCivilizationType())
			#TKe
			szName = gc.getCivicInfo(iData2).getDescription()
			szColoredName = localText.changeTextColor(szName, gc.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"))
			return szColoredName + szHelp

		return u""
		
		szSpecialText = CyGameTextMgr().parseCivicInfo(self.iCivic, True, False, True, False, self.player.getCivilizationType())
		#Tke
		
	def CivDropDown( self, inputClass ):
		if ( inputClass.getNotifyCode() == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED ):
			screen = self.getScreen()
			iIndex = screen.getSelectedPullDownID("CivDropDown")
			self.player = gc.getPlayer(screen.getPullDownData("CivDropDown", iIndex))
			
			self.drawBar()
			self.drawRoyalTroops()
			self.drawPlayerTroops()
			#self.drawColonialTroops()
			self.drawWarMaterials()
			self.drawCivics()


def isSoldier(eProfession):
	if eProfession == ProfessionTypes.NO_PROFESSION:
		return false
	if (gc.getProfessionInfo(eProfession).getCombatChange() == 0):
		return false
	if (gc.getProfessionInfo(eProfession).isUnarmed()):
		return false
	return (getProfessionYieldsRequired(eProfession) <= 2)
	
def isDragoon(eProfession):
	if eProfession == ProfessionTypes.NO_PROFESSION:
		return false
	if (gc.getProfessionInfo(eProfession).getCombatChange() == 0):
		return false
	if (gc.getProfessionInfo(eProfession).isUnarmed()):
		return false
	return (getProfessionYieldsRequired(eProfession) > 2)
def isKnight(UnitType):
	unit = gc.getUnitInfo(UnitType)
	if (unit.getFreePromotions(gc.getDefineINT("DEFAULT_KNIGHT_PROMOTION"))):
		return true
	return false

def isCannon(eUnit):
	unit = gc.getUnitInfo(eUnit)
	if unit.getDomainType() != DomainTypes.DOMAIN_LAND:
		return false
	if (unit.getCombat() == 0):
		return false
	return true

def isWarship(eUnit):
	unit = gc.getUnitInfo(eUnit)
	if unit.getDomainType() != DomainTypes.DOMAIN_SEA:
		return false
	if (unit.getCombat() == 0):
		return false
	if (unit.isOnlyDefensive()):
		return false
	return true

def getProfessionYieldsRequired(eProfession):
	iNumYieldsRequired = 0
	for iYield in range(YieldTypes.NUM_YIELD_TYPES):
		if (gc.getProfessionInfo(eProfession).getYieldEquipmentAmount(iYield) > 0):
			iNumYieldsRequired += 1
	return iNumYieldsRequired

def isVeteran(eUnit):
	unit = gc.getUnitInfo(eUnit)
	for iPromotion in range(gc.getNumPromotionInfos()):
		if unit.getFreePromotions(iPromotion):
			return true
	return false
	