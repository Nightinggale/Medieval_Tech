## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums


## I rewrote most of the page to remove all hardcoded values
## In this process, a lot of the old code was moved and changed to look a lot differently
## Sadly this meant I had to give up preserving comments about who did what, which is a shame
##
##  R&R, Robert Surcouf did a lot of work expanding the Domestic Advisor screen
##   I used his code as a starting point for the new code since the outcome should be the same.
##   As a result the lines doing the actual drawing is his work even though loops and variable name changes makes it hard to see

## Guide to add pages - by Nightinggale
##
## Buttons are added at # Button generation
## They appear in the order they are entered
## 
## Subpages (left/right arrow accessible) are added with createSubpage(iState, iPage)
##   It adds pages to iState until iPage is the highest INDEX for a page (iPage = 2 gives pages 0,1,2)
##   Nothing happens if the page already exist meaning a loop can easily call it every time it writes to a page
##   Together with floor divide (//) and modulus (%) this allows a loop to make columns without knowing how many pages it fills before it starts
##
## Pages are accessed with self.StatePages[iState][iPage]
## Accessing the page (arrows and button pressing) is set up automatically



# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

class CvDomesticAdvisor:
	"Domestic Advisor Screen"
	def __init__(self):
		self.listSelectedCities = []
		self.selectedSelectionGroupHeadUnitID = -1
		
	# Screen construction function
	def interfaceScreen(self):
		player = gc.getPlayer(gc.getGame().getActivePlayer())

		# Create a new screen, called DomesticAdvisur, using the file CvDomesticAdvisor.py for input
		screen = CyGInterfaceScreen( "DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR )

		self.nScreenWidth = screen.getXResolution()
		self.nScreenHeight = (screen.getYResolution() - (screen.getYResolution() * 31 / 100))
		
#VET NewCapacity - begin 1/4
		self.num_yields = YieldTypes.NUM_CARGO_YIELD_TYPES

		self.bNewCapacity = (gc.getDefineINT("NEW_CAPACITY") > 0)
		self.useLuxuryGoods = (gc.getDefineINT("PRICE_DIFF_EUROPE_DOMESTIC_LUXURY_GOODS") > 0)
		self.inventions = (gc.getDefineINT("CIVICOPTION_INVENTIONS") > 0)
		self.culture = (gc.getDefineINT("CULTURE_YIELD") > 0)
		
		if (self.bNewCapacity):
			self.bNewCapacity = 1
		else:
			self.bNewCapacity = 0
			
		if (self.inventions):
			self.inventions = 1
		else:
			self.inventions = 0
			
		if (self.culture):
			self.culture = 1
		else:
			self.culture = 0
		
		if (gc.getDefineINT("UNITCLASS_WHALING_BOAT") > 0):
			self.WhaleOil = gc.getInfoTypeForString("SPECIALBUILDING_WHALE_OIL")
		else:
			self.WhaleOil = -1

#VET NewCapacity - end 1/4
		## R&R, Robert Surcouf,  Domestic Advisor Screen - Start
		#self.nTableWidth = self.nScreenWidth * 19 / 20
		self.nTableWidth = self.nScreenWidth * 99 / 100
		## R&R, Robert Surcouf,  Domestic Advisor Screen - END
		self.nTableHeight = self.nScreenHeight - 150
		self.nNormalizedTableWidth = self.nTableWidth
		
		self.iButtonSpacing = 80
		self.iButtonSize = 60
		self.iCityButtonSize = 48
		## R&R, Robert Surcouf,  Domestic Advisor Screen - Start
		#self.Y_LOWER_ROW = self.nScreenHeight - 70
		self.Y_LOWER_ROW = self.nScreenHeight - 75
		#self.CITY_NAME_COLUMN_WIDTH = 250
		self.CITY_NAME_COLUMN_WIDTH = 190
		#self.PRODUCTION_COLUMN_SIZE = (self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH) / len(range(YieldTypes.YIELD_FOOD, YieldTypes.YIELD_LUXURY_GOODS + 1))
#VET NewCapacity - begin 2/4
		#self.WAREHOUSE_COLUMN_SIZE = (self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH - 60) / len(range(YieldTypes.YIELD_FOOD, YieldTypes.YIELD_LUXURY_GOODS+ 1))
		if self.bNewCapacity:
			##VET 
			##self.iWareHouseW = 120
			self.iWareHouseW = 100
		else:
			## VET self.iWareHouseW = 60
			self.iWareHouseW = 40
		#self.WAREHOUSE_COLUMN_SIZE = (self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH - self.iWareHouseW) / len(range(YieldTypes.YIELD_FOOD, YieldTypes.YIELD_LUXURY_GOODS + 1))
#VET NewCapacity - end 2/4
		#self.BUILDING_COLUMN_SIZE = (self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH) / (gc.getNumSpecialBuildingInfos() -1)
		## R&R, Robert Surcouf,  Domestic Advisor Screen - End
		self.ROW_HIGHT = 48

		self.Y_EXIT = self.nScreenHeight - 36
		self.X_EXIT = self.nScreenWidth - 30

		screen.setRenderInterfaceOnly(True)
		screen.setRenderFrozenWorld(True)
		screen.setDimensions((screen.getXResolution() - self.nScreenWidth) / 2, 0, self.nScreenWidth, self.nScreenHeight)
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		# Here we set the background widget and exit button, and we show the screen
		screen.addPanel( "DomesticAdvisorBG", u"", u"", True, False, 0, 0, self.nScreenWidth, self.nScreenHeight, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDrawControl("DomesticAdvisorBG", ArtFileMgr.getInterfaceArtInfo("MAINMENU_SLIDESHOW_LOAD").getPath(), 0, 0, self.nScreenWidth, self.nScreenHeight, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDDSGFC("TopPanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TITLE").getPath(), 0, 0, self.nScreenWidth, 55, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDDSGFC("BottomPanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TAB_OFF").getPath(), 0, self.nScreenHeight - 55, self.nScreenWidth, 55, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setText("DomesticExit", "Background", u"<font=4>" + localText.getText("TXT_KEY_PEDIA_SCREEN_EXIT", ()).upper() + "</font>", CvUtil.FONT_RIGHT_JUSTIFY, self.X_EXIT, self.Y_EXIT, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1 )

		screen.setLabel("DomesticScreenWidgetHeader", "Background", u"<font=4b>" + localText.getText("TXT_KEY_DOMESTIC_ADVISOR_TITLE", ()).upper() + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.nScreenWidth / 2, 4, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		self.StateButtons = []
		self.StatePages = []
		self.StateNames = []
		self.StateHelp = []
		
		# Button generation
		# index                       = self.addButton( button icon name,                      help popup string)
		
		self.GENERAL_STATE            = self.addButton("INTERFACE_CITY_MAP_BUTTON",                     "TXT_KEY_DOMESTIC_ADVISOR_STATE_GENERAL")
		self.PRODUCTION_STATE         = self.addButton("INTERFACE_NET_YIELD_BUTTON",                    "TXT_KEY_CONCEPT_PRODUCTION")
		self.WAREHOUSE_STATE          = self.addButton("INTERFACE_STORES_BUTTON",                       "TXT_KEY_DOMESTIC_ADVISOR_WAREHOUSE")
		self.BUILDING_STATE           = self.addButton("INTERFACE_CITY_BUILD_BUTTON",                   "TXT_KEY_BUILDINGS")
		self.BUILDING_VACANT_STATE    = self.addButton("INTERFACE_CITY_VACANT_BUILDING_BUTTON",         "TXT_KEY_BUILDING_VACANT")
		self.IMPORTEXPORT_STATE       = self.addButton("INTERFACE_CITY_GOVENOR_BUTTON",                 "TXT_KEY_CONCEPT_TRADE_ROUTE")
		self.CITIZEN_STATE            = self.addButton("INTERFACE_CITY_CITIZEN_BUTTON",                 "TXT_KEY_DOMESTIC_ADVISOR_STATE_CITIZEN")
		self.CITIZEN_COUNT_STATE      = self.addButton("INTERFACE_CITY_CITIZEN_COUNT_BUTTON",           "TXT_KEY_DOMESTIC_ADVISOR_STATE_CITIZEN_COUNT")
		self.CITIZEN_WRONG_STATE      = self.addButton("INTERFACE_CITY_CITIZEN_WRONG_BUTTON",           "TXT_KEY_DOMESTIC_ADVISOR_STATE_CITIZEN_WRONG")
		self.TOTAL_PRODUCTION_STATE   = self.addButton("INTERFACE_WAREHOUSE_STORAGE_BUTTON",            "TXT_KEY_CONCEPT_TOTAL_PRODUCTION")  # total production page - Nightinggale
		self.TRADEROUTE_STATE         = self.addButton("INTERFACE_IMPORT_EXPORT_BUTTON",                "TXT_KEY_DOMESTIC_ADVISOR_STATE_TRADEROUTE")
		self.NATIVE_STATE             = self.addButton("INTERFACE_NATIVE_VILLAGES_BUTTON",              "TXT_KEY_DOMESTIC_ADVISOR_STATE_NATIVE")
		
		# make sure there is room enough on the screen for all buttons
		iButtonSpacing = (self.X_EXIT - 145) // (len(self.StateButtons) - 1)
		if (iButtonSpacing < self.iButtonSpacing):
			self.iButtonSpacing = iButtonSpacing
		
		# set names to those lists not using self.createSubpage
		# in other words it's the lists, which doesn't add cities to the left
		for state in ([ self.TRADEROUTE_STATE, self.NATIVE_STATE ]):
			self.StatePages[state] = [self.StateNames[state]]
		
		self.YieldPages = []
		self.YieldPages.append(self.PRODUCTION_STATE)
		self.YieldPages.append(self.WAREHOUSE_STATE)
		self.YieldPages.append(self.TOTAL_PRODUCTION_STATE)
		self.YieldPages.append(self.IMPORTEXPORT_STATE)
		
		self.BuildingPages = []
		self.BuildingPages.append(self.BUILDING_STATE)
		self.BuildingPages.append(self.BUILDING_VACANT_STATE)
		
		self.CitizenPages = []
		self.CitizenPages.append(self.CITIZEN_COUNT_STATE)
		self.CitizenPages.append(self.CITIZEN_WRONG_STATE)
		
		## R&R, Robert Surcouf,  Domestic Advisor Screen START
		
		# Extra left/right arrow buttons (big ones)... (2)
		self.StateButtons.append("INTERFACE_CITY_LEFT_ARROW")
		self.StateButtons.append("INTERFACE_CITY_RIGHT_ARROW")
		# Next Page / Previous Page
		
		## R&R, Robert Surcouf,  Domestic Advisor Screen END
		
		#TKs Med
		#self.GENERAL_COLUMN_SIZE = (self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH) / (12 + self.inventions + self.culture)
		self.GENERAL_COLUMN_SIZE = 60
		#TKe Med
		
		self.AllowedYields = []
		self.AllowedYieldIndex = []
		
		self.AllowedUnits = []
		self.AllowedUnitIndex = []
		
		self.AllowedBuildings = []
		self.AllowedBuildingIndex = []
		self.AllowedSpecialBuildings = []
		
		for iYield in range(self.num_yields):
			iIndex = -1
			if (player.canUseYield(iYield)):
				iIndex = len(self.AllowedYields)
				self.AllowedYields.append(iYield)
			self.AllowedYieldIndex.append(iIndex)
		
		for iUnit in range(gc.getNumUnitInfos()):
			iIndex = -1
			if (player.canUseUnit(iUnit)):
				if (gc.getUnitInfo(iUnit).isFound()):
					iIndex = len(self.AllowedUnits)
					self.AllowedUnits.append(iUnit)
			self.AllowedUnitIndex.append(iIndex)
			
		specialBuildings = [] 
		for iBuilding in range(gc.getNumSpecialBuildingInfos()):
			specialBuildings.append([])

		for iBuilding in range(gc.getNumBuildingInfos()):
			if (player.canUseBuilding(iBuilding)):
				specialBuilding = gc.getBuildingInfo(iBuilding).getSpecialBuildingType()
				specialBuildings[specialBuilding].append(iBuilding)
		
		for BuildingArray in range(len(specialBuildings)):
			iIndex = -1
			if (len(specialBuildings[BuildingArray]) > 0):
				iIndex = len(self.AllowedBuildings)
				self.AllowedBuildings.append(specialBuildings[BuildingArray])
				self.AllowedSpecialBuildings.append(len(self.AllowedBuildingIndex))
			self.AllowedBuildingIndex.append(iIndex)

		# set number of columns in a page
		# first argument is minimum column width
		# second argument is number of columns (presumably array length)
		
		self.MAX_YIELDS_IN_A_PAGE      = self.getNumColumns(59, len(self.AllowedYields))
		self.MAX_BUILDINGS_IN_A_PAGE   = self.getNumColumns(59, len(self.AllowedBuildings))
		self.MAX_UNITS_IN_A_PAGE       = self.getNumColumns(49, len(self.AllowedUnits))
			
		self.RebuildArrays()

		# setup for general columns
		general_column_setup = []
		general_column_setup.append([self.GENERAL_COLUMN_SIZE, "population", localText.getText("TXT_KEY_POPULATION", ())])
		general_column_setup.append([self.GENERAL_COLUMN_SIZE, "power", (u" %c" %(CyGame().getSymbolID(FontSymbols.POWER_CHAR)))])
		general_column_setup.append([self.GENERAL_COLUMN_SIZE, "food", (u" %c" % gc.getYieldInfo(YieldTypes.YIELD_FOOD).getChar() )])
		general_column_setup.append([self.GENERAL_COLUMN_SIZE, "hammer", (u" %c" % gc.getYieldInfo(YieldTypes.YIELD_HAMMERS).getChar() )])
		general_column_setup.append([self.GENERAL_COLUMN_SIZE, "bell", (u" %c" % gc.getYieldInfo(YieldTypes.YIELD_BELLS).getChar() )])
		general_column_setup.append([self.GENERAL_COLUMN_SIZE, "cross", (u" %c" % gc.getYieldInfo(YieldTypes.YIELD_CROSSES).getChar() )])
		general_column_setup.append([self.GENERAL_COLUMN_SIZE, "education", (u" %c" % gc.getYieldInfo(YieldTypes.YIELD_EDUCATION).getChar() )])
		if self.inventions:
			general_column_setup.append([self.GENERAL_COLUMN_SIZE, "idea", (u" %c" % gc.getYieldInfo(YieldTypes.YIELD_IDEAS).getChar() )])
		if self.culture:
			general_column_setup.append([self.GENERAL_COLUMN_SIZE, "culture", (u" %c" % gc.getYieldInfo(YieldTypes.YIELD_CULTURE).getChar() )])
		general_column_setup.append([self.GENERAL_COLUMN_SIZE, "strength", (u" %c" % CyGame().getSymbolID(FontSymbols.STRENGTH_CHAR)) ])
		general_column_setup.append([self.GENERAL_COLUMN_SIZE, "defence", (u" %c" % CyGame().getSymbolID(FontSymbols.DEFENSE_CHAR)) ])
		general_column_setup.append([self.GENERAL_COLUMN_SIZE * 3, "culture_long", localText.getText("TXT_KEY_ADVISOR_CULTURE", ())])
		
		# setting up page(s) for general state
		self.GENERAL_COLUMNS = []
		self.MAX_GENERAL_IN_A_PAGE = len(general_column_setup)
		iSpace = self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH
		for iColumn in range(len(general_column_setup)):
			if (len(self.GENERAL_COLUMNS) < self.MAX_GENERAL_IN_A_PAGE):
				iNew = iSpace - general_column_setup[iColumn][0]
				if (iNew < 200):
					self.GENERAL_COLUMNS.append([iSpace, "production", localText.getText("TXT_KEY_DOMESTIC_ADVISOR_PRODUCING", ()).upper(), iSpace])
					self.MAX_GENERAL_IN_A_PAGE = len(self.GENERAL_COLUMNS)
				else:
					iSpace = iNew
			if (len(self.GENERAL_COLUMNS) >= self.MAX_GENERAL_IN_A_PAGE and general_column_setup[iColumn][1] == "culture_long"):
				self.GENERAL_COLUMNS.append([self.GENERAL_COLUMN_SIZE, "bell", (u" %c" % gc.getYieldInfo(YieldTypes.YIELD_BELLS).getChar() ), iSpace])
			self.GENERAL_COLUMNS.append(general_column_setup[iColumn])
			self.GENERAL_COLUMNS[len(self.GENERAL_COLUMNS)-1].append(len(self.GENERAL_COLUMNS))
		
		if (len(self.GENERAL_COLUMNS) <= self.MAX_GENERAL_IN_A_PAGE):
			self.GENERAL_COLUMNS.append([iSpace, "production", localText.getText("TXT_KEY_DOMESTIC_ADVISOR_PRODUCING", ()).upper(), iSpace])
			self.MAX_GENERAL_IN_A_PAGE = len(self.GENERAL_COLUMNS)
		
		self.createSubpage(self.GENERAL_STATE, 0, self.MAX_GENERAL_IN_A_PAGE)
		if (len(self.GENERAL_COLUMNS) > self.MAX_GENERAL_IN_A_PAGE):
			self.createSubpage(self.GENERAL_STATE, 1, len(self.GENERAL_COLUMNS) % self.MAX_GENERAL_IN_A_PAGE)
		
		for iIndex in range(len(self.GENERAL_COLUMNS)):
			iIndexOnPage = iIndex % self.MAX_GENERAL_IN_A_PAGE
			iPage = iIndex // self.MAX_GENERAL_IN_A_PAGE
			screen.setTableColumnHeader( self.StatePages[self.GENERAL_STATE][iPage] + "ListBackground", iIndexOnPage + 2, "<font=2>" + self.GENERAL_COLUMNS[iIndex][2] + "</font>", self.GENERAL_COLUMNS[iIndex][0])
		
		if (self.useLuxuryGoods):
			self.createSubpage(self.GENERAL_STATE, 2)
			## R&R, Robert Surcouf, Domestic Market display START
			szListName = self.StatePages[self.GENERAL_STATE][2] + "ListBackground"
			iStartYield=gc.getDefineINT("DOMESTIC_MARKET_SCREEN_START_YIELD_ID")
			for iYield in range(iStartYield, self.num_yields):
				#screen.setTableColumnHeader(szListName, iYield-iStartYield + 2, "<font=2>" + (u" %c" % gc.getYieldInfo(iYield).getChar()) + "</font>", (self.WAREHOUSE_COLUMN_SIZE * self.nTableWidth) / self.nNormalizedTableWidth )
				screen.setTableColumnHeader(szListName, iYield-iStartYield + 2, "<font=2>" + (u" %c" % gc.getYieldInfo(iYield).getChar()) + "</font>", (self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH) / 17 + 1)
			## R&R, Robert Surcouf, Domestic Market display End
		
		#WareHouseState Headers
#VET NewCapacity - begin 3/4
		#screen.setTableColumnHeader( "WareHouseStateListBackground", 2, "<font=3>" + "MAX" + "</font>", 60 )
		## R&R, Robert Surcouf,  Domestic Advisor Screen START
		#### old VET 
		####screen.setTableColumnHeader( "WareHouseStateListBackground", 2, "<font=3>" + "MAX" + "</font>", self.iWareHouseW )
		## R&R, Robert Surcouf,  Domestic Advisor Screen END
#VET NewCapacity - begin 3/4
		for iState in self.YieldPages:
			offset = 0
			#width = (self.PRODUCTION_COLUMN_SIZE * self.nTableWidth) / self.nNormalizedTableWidth
			if self.bNewCapacity and iState == self.WAREHOUSE_STATE:
				offset = 1
				width = (self.WAREHOUSE_COLUMN_SIZE * self.nTableWidth) / self.nNormalizedTableWidth
				for PageName in self.StatePages[self.WAREHOUSE_STATE]:
					screen.setTableColumnHeader( PageName + "ListBackground", 2, "<font=2>" + "MAX" + "</font>", self.iWareHouseW)
				
			for iYieldIndex in range(len(self.AllowedYields)):
				iYield = self.AllowedYields[iYieldIndex]
				iYieldOnPage = iYieldIndex % self.MAX_YIELDS_IN_A_PAGE
				iPage = iYieldIndex // self.MAX_YIELDS_IN_A_PAGE
				[ iWidth, iColumns ] = self.getColumnWidth(iYieldIndex, len(self.AllowedYields), self.MAX_YIELDS_IN_A_PAGE, iPage)
				self.createSubpage(iState, iPage, iColumns)
				screen.setTableColumnHeader( self.StatePages[iState][iPage] + "ListBackground", iYieldOnPage + 2 + offset, "<font=2> " + (u" %c" % gc.getYieldInfo(iYield).getChar()) + "</font>", iWidth )

				
		# Building Headers
		for iIndex in range(len(self.AllowedBuildings)):
			iIndexOnPage = iIndex % self.MAX_BUILDINGS_IN_A_PAGE
			iPage = iIndex // self.MAX_BUILDINGS_IN_A_PAGE
			iSpecial = self.AllowedSpecialBuildings[iIndex]
			[ iWidth, iColumns ] = self.getColumnWidth(iIndex, len(self.AllowedBuildings), self.MAX_BUILDINGS_IN_A_PAGE, iPage)
			for state in self.BuildingPages:
				self.createSubpage(state, iPage, iColumns)
				screen.setTableColumnHeader( self.StatePages[state][iPage] + "ListBackground", iIndexOnPage + 2, "<font=2> " + (u" %c" %  gc.getSpecialBuildingInfo(iSpecial).getChar()) + "</font>", iWidth)
		
		#for iSpecial in range(gc.getNumSpecialBuildingInfos()):	
		#	if (iSpecial != gc.getInfoTypeForString("SPECIALBUILDING_BELLS")):
		#		iBuildingOnPage = (iSpecial-1) % self.MAX_BUILDINGS_IN_A_PAGE
		#		iPage = (iSpecial-1) // self.MAX_BUILDINGS_IN_A_PAGE
		#		self.createSubpage(self.BUILDING_STATE, iPage)
			
		#		if (iSpecial == self.WhaleOil):
		#			screen.setTableColumnHeader( self.StatePages[self.BUILDING_STATE][iPage] + "ListBackground", iBuildingOnPage + 2, "<font=2> " + (u" %c" %  gc.getYieldInfo(YieldTypes.YIELD_WHALE_OIL).getChar()) + "</font>", (self.BUILDING_COLUMN_SIZE * self.nTableWidth) / self.nNormalizedTableWidth )				
		#		else:
		#			screen.setTableColumnHeader( self.StatePages[self.BUILDING_STATE][iPage] + "ListBackground", iBuildingOnPage + 2, "<font=2> " + (u" %c" %  gc.getSpecialBuildingInfo(iSpecial).getChar())         + "</font>", (self.BUILDING_COLUMN_SIZE * self.nTableWidth) / self.nNormalizedTableWidth )
	
		# Citizen Headers
		self.createSubpage(self.CITIZEN_STATE, 0, 3)
		screen.setTableColumnHeader( self.StatePages[self.CITIZEN_STATE][0] + "ListBackground", 2, "<font=2>" +  localText.getText("TXT_KEY_DOMESTIC_ADVISOR_STATE_CITIZEN", ()).upper() + "</font>", self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH)
			
		# Citizen count header
		for iIndex in range(len(self.AllowedUnits)):
			iIndexOnPage = iIndex % self.MAX_UNITS_IN_A_PAGE
			iPage = iIndex // self.MAX_UNITS_IN_A_PAGE
			iUnit = self.AllowedUnits[iIndex]
			[ iWidth, iColumns ] = self.getColumnWidth(iIndex, len(self.AllowedUnits), self.MAX_UNITS_IN_A_PAGE, iPage)
			for iStateIndez in range(len(self.CitizenPages)):
				state = self.CitizenPages[iStateIndez]
				self.createSubpage(state, iPage, iColumns)
				screen.setTableColumnHeader( self.StatePages[state][iPage] + "ListBackground", iIndexOnPage + 2, "<font=2> " + gc.getUnitInfo(iUnit).getDescription() + "</font>", iWidth )

		
		#Default State on Screen opening
		self.CurrentState = self.GENERAL_STATE
		self.CurrentPage = 0
		
		# Draw the city list...
		self.drawContents()
		
	def drawButtons(self):
		screen = CyGInterfaceScreen( "DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR )

		#for iState in range(len(self.StateTypes)):
		for iState in range(len(self.StatePages)):
			for PageName in self.StatePages[iState]:
				szStateName = PageName + "ListBackground"
				## R&R, Robert Surcouf,  Domestic Advisor Screen START
				#screen.setImageButton(szStateName + "Button", ArtFileMgr.getInterfaceArtInfo(self.StateButtons[iState]).getPath(), (self.iButtonSpacing * iState) + (self.iButtonSpacing / 2), self.Y_LOWER_ROW, self.iButtonSize, self.iButtonSize, WidgetTypes.WIDGET_GENERAL, iState, -1 )
				#if (iState < len(self.StatePages)):
				screen.setImageButton(szStateName + "Button", ArtFileMgr.getInterfaceArtInfo(self.StateButtons[iState]).getPath(), (self.iButtonSpacing * iState) + (self.iButtonSpacing / 2), self.Y_LOWER_ROW, self.iButtonSize, self.iButtonSize, WidgetTypes.WIDGET_GENERAL, iState, -1 )
				## R&R, Robert Surcouf,  Domestic Advisor Screen END
				if (int(self.CurrentState) == iState):
					RelativeButtonSize = 130
					## R&R, Robert Surcouf,  Domestic Advisor Screen START
					#if (iState < len(self.StatePages)):
					screen.setImageButton("HighlightButton", ArtFileMgr.getInterfaceArtInfo("INTERFACE_HIGHLIGHTED_BUTTON").getPath(), (self.iButtonSpacing * iState) + (self.iButtonSpacing / 2) - ((self.iButtonSize * RelativeButtonSize / 100) / 2) + (self.iButtonSize / 2), self.Y_LOWER_ROW - ((self.iButtonSize * RelativeButtonSize / 100) / 2) + (self.iButtonSize / 2), self.iButtonSize * RelativeButtonSize / 100, self.iButtonSize * RelativeButtonSize / 100, WidgetTypes.WIDGET_GENERAL, iState, -1 )
				
				
		# auto-generated list creation - Nightinggale
		# Added hardcoded button values 100 and 102
		#
		# turn the button on and off with y_offset
		# as ignoring a button can leave a ghost behind, turning a button off is done by drawing it outside the screen (y = -1000)
		# kind of a hack, but it works
		y_offset = 20
		if (self.CurrentPage == 0):
			y_offset = -1000
		screen.setImageButton("MainLeftButton", ArtFileMgr.getInterfaceArtInfo(self.StateButtons[len(self.StateButtons)-2]).getPath(), 15, y_offset, 2*self.iButtonSize/3, 2*self.iButtonSize/3, WidgetTypes.WIDGET_GENERAL, 100, -1 )
				
		y_offset = -1000
		if (self.CurrentPage < (len(self.StatePages[self.CurrentState]) - 1)):
			y_offset = 20
		screen.setImageButton("MainRightButton", ArtFileMgr.getInterfaceArtInfo(self.StateButtons[len(self.StateButtons)-1]).getPath(), self.nScreenWidth -50, y_offset, 2*self.iButtonSize/3, 2*self.iButtonSize/3, WidgetTypes.WIDGET_GENERAL, 102, -1 )

			## R&R, Robert Surcouf,  Domestic Advisor Screen END
	# Function to draw the contents of the cityList passed in
	def drawContents (self):

		# Get the screen and the player
		screen = CyGInterfaceScreen( "DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR )
		player = gc.getPlayer(CyGame().getActivePlayer())
		screen.moveToFront( "Background" )

		# total production page - start - Nightinggale
		if self.CurrentState == self.TOTAL_PRODUCTION_STATE:
			self.updateTotalProduction()
		# total production page - end - Nightinggale
		#Loop through the cities and update the table
		## R&R, Robert Surcouf,  Domestic Advisor Screen START
		elif (self.CurrentState != self.TRADEROUTE_STATE and self.CurrentState != self.NATIVE_STATE):
		## R&R, Robert Surcouf,  Domestic Advisor Screen END
			for iCity in range(len(self.Cities)):
				if (self.Cities[iCity].getName() in self.listSelectedCities):
					screen.selectRow( self.StatePages[self.CurrentState][self.CurrentPage] + "ListBackground", iCity, True )
				self.updateCityTable(self.Cities[iCity], iCity)

			if (self.CurrentState == self.CITIZEN_STATE):
				for iCity in range(len(self.Cities)):
					self.updateCitizenTable(self.Cities[iCity], iCity)
		## R&R, Robert Surcouf,  Domestic Advisor Screen START					
		#else:
		elif self.CurrentState == self.TRADEROUTE_STATE:
		## R&R, Robert Surcouf,  Domestic Advisor Screen END
			self.RebuildRouteTable()
			BonusRow = 0
			for iRoute in range(player.getNumTradeRoutes()):
				self.updateRouteTable(self.Routes[iRoute], player.getTradeRouteByIndex(iRoute).getID(), iRoute)
				BonusRow += 1
			self.BuildAllTransportsRow(BonusRow)
			## R&R, Robert Surcouf,  Domestic Advisor Screen START
		elif self.CurrentState == self.NATIVE_STATE:
			self.updateNativeTable()
			## R&R, Robert Surcouf,  Domestic Advisor Screen END
		
		self.drawButtons()
		screen.show(self.StatePages[self.CurrentState][self.CurrentPage] + "ListBackground")
		self.updateAppropriateCitySelection()

	def updateCityTable(self, pLoopCity, i):
		screen = CyGInterfaceScreen( "DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR )

		szState = self.StatePages[self.CurrentState][self.CurrentPage]
		screen.setTableText(szState + "ListBackground", 0, i, "", ArtFileMgr.getInterfaceArtInfo("INTERFACE_BUTTONS_CITYSELECTION").getPath(), WidgetTypes.WIDGET_ZOOM_CITY, pLoopCity.getOwner(), pLoopCity.getID(), CvUtil.FONT_LEFT_JUSTIFY);
		## R&R, Robert Surcouf,  Domestic Advisor Screen - Start
		# City Name (text font size reduction)
		#screen.setTableText(szState + "ListBackground", 1, i, "<font=4>" + pLoopCity.getName() + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
		screen.setTableText(szState + "ListBackground", 1, i, "<font=2>" + pLoopCity.getName() + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
		## R&R, Robert Surcouf,  Domestic Advisor Screen - End
		
		if(self.CurrentState == self.GENERAL_STATE):
			if (self.CurrentPage == 0):
				start = 0
				end = self.MAX_GENERAL_IN_A_PAGE
			else:
				start = self.MAX_GENERAL_IN_A_PAGE
				end = len(self.GENERAL_COLUMNS)
				
			for iIndex in range(start, end):
				szText = ""
				szColumn = self.GENERAL_COLUMNS[iIndex][1]
				if (szColumn == "population"):
					szText = unicode(pLoopCity.getPopulation())
				elif (szColumn == "power"):
					szText = unicode(pLoopCity.getRebelPercent()) + "%"
				elif (szColumn == "food"):
					iNetFood = pLoopCity.foodDifference()
					szText = unicode(iNetFood)
					if iNetFood > 0:
						szText = localText.getText("TXT_KEY_COLOR_POSITIVE", ()) + szText + localText.getText("TXT_KEY_COLOR_REVERT", ())
					elif iNetFood < 0:
						szText = localText.getText("TXT_KEY_COLOR_NEGATIVE", ()) + szText + localText.getText("TXT_KEY_COLOR_REVERT", ())	
				elif (szColumn == "hammer"):
					szText = unicode(pLoopCity.calculateNetYield(YieldTypes.YIELD_HAMMERS))
				elif (szColumn == "bell"):
					szText = unicode(pLoopCity.calculateNetYield(YieldTypes.YIELD_BELLS))
				elif (szColumn == "cross"):
					szText = unicode(pLoopCity.calculateNetYield(YieldTypes.YIELD_CROSSES))
				elif (szColumn == "education"):
					szText = unicode(pLoopCity.calculateNetYield(YieldTypes.YIELD_EDUCATION))
				elif (szColumn == "idea"):
					szText = unicode(pLoopCity.calculateNetYield(YieldTypes.YIELD_IDEAS))
				elif (szColumn == "culture"):
					szText = unicode(pLoopCity.calculateNetYield(YieldTypes.YIELD_CULTURE))
				elif (szColumn == "strength"):
					szText = unicode(pLoopCity.plot().getNumDefenders(pLoopCity.getOwner()))
				elif (szColumn == "defence"):
					szText = u"<font=2>%s%%</font>" % (str(pLoopCity.getDefenseModifier()))
				elif (szColumn == "culture_long"):
					szText = localText.getText("TXT_KEY_CITY_BAR_CULTURE", (pLoopCity.getCulture(pLoopCity.getOwner()),pLoopCity.getCultureThreshold(), gc.getCultureLevelInfo(pLoopCity.getCultureLevel()).getText()))
				elif (szColumn == "production"):
					szText = pLoopCity.getProductionName() + " (" + str(pLoopCity.getGeneralProductionTurnsLeft()) + ")"
				else:
					szText = u"ERROR"
				
				screen.setTableInt(szState + "ListBackground", iIndex - start + 2, i, "<font=2>" + szText + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
				
		elif(self.CurrentState == self.PRODUCTION_STATE):
			start = self.YieldStart()
			for iYieldIndex in range(start, self.YieldEnd()):
				iYield = self.AllowedYields[iYieldIndex]
				iNetYield = pLoopCity.calculateNetYield(iYield)
				szText = unicode(iNetYield)
				if iNetYield > 0:
					szText = localText.getText("TXT_KEY_COLOR_POSITIVE", ()) + u"+" + szText + localText.getText("TXT_KEY_COLOR_REVERT", ())
				elif iNetYield < 0:
					szText = localText.getText("TXT_KEY_COLOR_NEGATIVE", ()) + szText + localText.getText("TXT_KEY_COLOR_REVERT", ())
				elif iNetYield == 0:
					szText = ""
				screen.setTableInt(szState + "ListBackground", iYieldIndex - start + 2, i, "<font=1>" + szText + "<font/>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
				
		elif(self.CurrentState == self.WAREHOUSE_STATE):
#VET NewCapacity - begin 4/4
			#screen.setTableInt("WareHouseStateListBackground", 2, i, u"<font=3><color=255,255,255>" + str(pLoopCity.getMaxYieldCapacity()) + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )

			#for iYield in range(YieldTypes.YIELD_FOOD, YieldTypes.YIELD_LUXURY_GOODS + 1):
			#	iNetYield = pLoopCity.getYieldStored(iYield)
			#	szText = unicode(iNetYield)
			#	if iNetYield == 0:
			#		szText = ""
			#	if (pLoopCity.calculateNetYield(iYield) * 5 + pLoopCity.getYieldStored(iYield) <= pLoopCity.getMaxYieldCapacity() or iYield == YieldTypes.YIELD_FOOD):
			#		screen.setTableInt("WareHouseStateListBackground", iYield + 3, i, u"<font=3><color=0,255,255>" + szText + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			#	elif (pLoopCity.getYieldStored(iYield) <= pLoopCity.getMaxYieldCapacity()):
			#		screen.setTableInt("WareHouseStateListBackground", iYield + 3, i, u"<font=3><color=255,255,0>" + szText + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			#	else:
			#		screen.setTableInt("WareHouseStateListBackground", iYield + 3, i, u"<font=3><color=255,0,0>" + szText + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			
			if self.bNewCapacity:
				iMaxYield = pLoopCity.getMaxYieldCapacity()
				aiProducedYields = [[]] * YieldTypes.NUM_YIELD_TYPES
				for iYield in range(YieldTypes.NUM_YIELD_TYPES):
					aiProducedYields[iYield] = 0
				iNetYield = 0
				for iYield in range(YieldTypes.NUM_YIELD_TYPES):
					iUnproducedYield = pLoopCity.calculateActualYieldProduced(iYield) - pLoopCity.getBaseRawYieldProduced(iYield)
					if iUnproducedYield < 0:
						for iProfession in range(gc.getNumProfessionInfos()):
							if gc.getProfessionInfo(iProfession).getYieldsProduced(0) == iYield: #MultipleYieldsProduced Start
								iNeedYield = gc.getProfessionInfo(iProfession).getYieldsConsumed(0) #MultipleYieldsProduced Start
								aiProducedYields[iNeedYield] += iUnproducedYield
					iProducedYield = pLoopCity.calculateNetYield(iYield)
					aiProducedYields[iYield] += iProducedYield

					if iYield == YieldTypes.YIELD_FOOD or not gc.getYieldInfo(iYield).isCargo():
						continue
					iNetYield += pLoopCity.getYieldStored(iYield)

				iProdusedYield = 0
				for iYield in range(YieldTypes.NUM_YIELD_TYPES):
					if iYield != YieldTypes.YIELD_FOOD and gc.getYieldInfo(iYield).isCargo():
						iProdusedYield += aiProducedYields[iYield]

				#szText = u"<font=3><color=" 
				szText = u"<font=2><color=" ## R&R, Robert Surcouf,  Domestic Advisor Screen
				if iNetYield > iMaxYield:
					szText += u"255,0,0"
				elif iNetYield + iProdusedYield > iMaxYield:
					szText += u"255,255,0"
				else:
					szText += u"0,255,0"
				szText += u">" + str(iNetYield)
				if iProdusedYield > 0:
					szText += u"+" + str(iProdusedYield)
				elif iProdusedYield < 0:
					szText += str(iProdusedYield)
				szText += u"/" + str(iMaxYield) + u"</color></font>"
				screen.setTableInt(self.StatePages[self.WAREHOUSE_STATE][self.CurrentPage] + "ListBackground", 2, i, szText, "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
				
			for iYieldIndex in range(self.YieldStart(), self.YieldEnd()):
				iYield = self.AllowedYields[iYieldIndex]
				iNetYield = pLoopCity.getYieldStored(iYield)
				szText = unicode(iNetYield)
				if iNetYield == 0:
					szText = ""
				else:
					screen.setTableInt(szState + "ListBackground", (iYieldIndex % self.MAX_YIELDS_IN_A_PAGE) + 2 + self.bNewCapacity, i, u"<font=2><color=0,255,0>" + szText + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			#else:
			#	screen.setTableInt(self.StatePages[self.WAREHOUSE_STATE][0] + "ListBackground", 2, i, u"<font=2><color=255,255,255>" + str(pLoopCity.getMaxYieldCapacity()) + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			#	for iYield in range(YieldTypes.NUM_YIELD_TYPES):
			#		iNetYield = pLoopCity.getYieldStored(iYield)
			#		szText = unicode(iNetYield)
			#		if iNetYield == 0:
			#			szText = ""
			#		## R&R, Robert Surcouf,  Domestic Advisor Screen - Start
			#		if (iYield < self.MAX_YIELDS_IN_A_PAGE ):
			#			if (pLoopCity.calculateNetYield(iYield) * 5 + pLoopCity.getYieldStored(iYield) <= pLoopCity.getMaxYieldCapacity() or iYield == YieldTypes.YIELD_FOOD):
			#				screen.setTableInt(self.StatePages[self.WAREHOUSE_STATE][0] + "ListBackground", iYield + 3, i, u"<font=1><color=0,255,255>" + szText + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			#			elif (pLoopCity.getYieldStored(iYield) <= pLoopCity.getMaxYieldCapacity()):			
			#				screen.setTableInt(self.StatePages[self.WAREHOUSE_STATE][0] + "ListBackground", iYield + 3, i, u"<font=1><color=255,255,0>" + szText + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			#			else:
			#				screen.setTableInt(self.StatePages[self.WAREHOUSE_STATE][0] + "ListBackground", iYield + 3, i, u"<font=1><color=255,0,0>" + szText + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			#		else:
			#			if (pLoopCity.calculateNetYield(iYield) * 5 + pLoopCity.getYieldStored(iYield) <= pLoopCity.getMaxYieldCapacity() or iYield == YieldTypes.YIELD_FOOD):
			#				screen.setTableInt(self.StatePages[self.WAREHOUSE_STATE][1] + "ListBackground", iYield - self.MAX_YIELDS_IN_A_PAGE + 3, i, u"<font=1><color=0,255,255>" + szText + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			#			elif (pLoopCity.getYieldStored(iYield) <= pLoopCity.getMaxYieldCapacity()):			
			#				screen.setTableInt(self.StatePages[self.WAREHOUSE_STATE][1] + "ListBackground", iYield - self.MAX_YIELDS_IN_A_PAGE + 3, i, u"<font=1><color=255,255,0>" + szText + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			#			else:
			#				screen.setTableInt(self.StatePages[self.WAREHOUSE_STATE][1] + "ListBackground", iYield - self.MAX_YIELDS_IN_A_PAGE + 3, i, u"<font=1><color=255,0,0>" + szText + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
							#screen.setTableText("WareHouseStatePage2ListBackground", iYield - self.MAX_YIELDS_IN_A_PAGE + 3, i, u"<font=1><color=255,0,0>" + szText + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
					## R&R, Robert Surcouf,  Domestic Advisor Screen - End
#VET NewCapacity - end 4/4
		
		elif(self.CurrentState == self.BUILDING_VACANT_STATE):
			buildingUsageList = [0] * len(self.AllowedBuildings)
			for iCitizen in range(pLoopCity.getPopulation() - 1, -1, -1):
				pCitizen = pLoopCity.getPopulationUnitByIndex(iCitizen)
				pProfession = gc.getProfessionInfo(pCitizen.getProfession())
				iIndex = pProfession.getSpecialBuilding()
				if iIndex != -1:
					buildingUsageList[self.AllowedBuildingIndex[iIndex]] += 1
			
			start = self.BuildingStart()
			for iBuildingIndex in range(start, self.BuildingEnd()):
				iSpecial = self.AllowedSpecialBuildings[iBuildingIndex]
				iIconBuilding = -1
				for iBuilding in self.AllowedBuildings[iBuildingIndex]:
					if pLoopCity.isHasBuilding(iBuilding):
						iIconBuilding = iBuilding
						break
				if iIconBuilding != -1:
					iMax = gc.getBuildingInfo(iIconBuilding).getMaxWorkers()
					if iMax > 0:
						iFree = iMax - buildingUsageList[iBuildingIndex]
						if iFree > 0:
							szText = localText.getText("TXT_KEY_COLOR_POSITIVE", ())
						else:
							szText = localText.getText("TXT_KEY_COLOR_NEGATIVE", ())
						szText += unicode(iFree) + localText.getText("TXT_KEY_COLOR_REVERT", ())	
						screen.setTableInt(szState + "ListBackground", iBuildingIndex - start  + 2, i, "<font=2>" + szText + "</font>", "", WidgetTypes.WIDGET_PEDIA_JUMP_TO_BUILDING, iIconBuilding, -1, CvUtil.FONT_LEFT_JUSTIFY )
		
		elif(self.CurrentState == self.BUILDING_STATE):
			start = self.BuildingStart()
			for iBuildingIndex in range(start, self.BuildingEnd()):
				iSpecial = self.AllowedSpecialBuildings[iBuildingIndex]
				iIconBuilding = -1
				for iBuilding in self.AllowedBuildings[iBuildingIndex]:
					if pLoopCity.isHasBuilding(iBuilding):
						iIconBuilding = iBuilding
						break
				if iIconBuilding != -1:
					screen.setTableInt(szState + "ListBackground", iBuildingIndex - start  + 2, i, "", gc.getBuildingInfo(iIconBuilding).getButton(), WidgetTypes.WIDGET_PEDIA_JUMP_TO_BUILDING, iIconBuilding, -1, CvUtil.FONT_LEFT_JUSTIFY )
		
		
			#start = (self.MAX_BUILDINGS_IN_A_PAGE * self.CurrentPage) + 1
			#end = min((self.MAX_BUILDINGS_IN_A_PAGE * (self.CurrentPage + 1)) + 1, gc.getNumSpecialBuildingInfos()-1)
		
			#for iSpecial in range(start, end):
			#	if (iSpecial != gc.getInfoTypeForString("SPECIALBUILDING_BELLS")):
			#		iIconBuilding = -1
			#		for iBuilding in range(gc.getNumBuildingInfos()):
			#			if gc.getBuildingInfo(iBuilding).getSpecialBuildingType() == iSpecial:
			#				if pLoopCity.isHasBuilding(iBuilding):
			#					iIconBuilding = iBuilding
			#					break
			#		if iIconBuilding != -1:
			#			screen.setTableInt(szState + "ListBackground", iSpecial - start  + 2, i, "", gc.getBuildingInfo(iBuilding).getButton(), WidgetTypes.WIDGET_PEDIA_JUMP_TO_BUILDING, iBuilding, -1, CvUtil.FONT_LEFT_JUSTIFY )
						
		elif(self.CurrentState == self.IMPORTEXPORT_STATE):
			start = self.YieldStart()
			for iYieldIndex in range(start, self.YieldEnd()):
				iYield = self.AllowedYields[iYieldIndex]
				bExportYield = pLoopCity.isExport(iYield)
				bImportYield = pLoopCity.isImport(iYield)
				## R&R, Robert Surcouf,  Domestic Advisor Screen - End
				if (bExportYield and bImportYield):
					screen.setTableInt(szState + "ListBackground", iYieldIndex - start + 2, i, u"<font=2><color=255,255,0>" + localText.getText("TXT_KEY_IN_AND_OUT", ()) + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
				elif (not bExportYield and bImportYield):
					screen.setTableInt(szState + "ListBackground", iYieldIndex - start + 2, i, u"<font=2><color=0,255,0>" + localText.getText("TXT_KEY_IN", ()) + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
				elif (bExportYield and not bImportYield):
					screen.setTableInt(szState + "ListBackground", iYieldIndex - start + 2, i, u"<font=2><color=255,0,0>" + localText.getText("TXT_KEY_OUT", ()) + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )

			screen.setTableText(szState + "ListBackground", 1, i, "<font=2>" + pLoopCity.getName() + "</font>", "", WidgetTypes.WIDGET_YIELD_IMPORT_EXPORT, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			# RR - Domestic Advisor Screen - END
		
		elif(self.CurrentState in self.CitizenPages):
			cityPopList = [0] * len(self.AllowedUnits)
			for iCitizen in range(pLoopCity.getPopulation() - 1, -1, -1):
				pCitizen = pLoopCity.getPopulationUnitByIndex(iCitizen)
				iType = pCitizen.getUnitType()
				if (self.CurrentState == self.CITIZEN_COUNT_STATE or not pCitizen.isCitizenExpertWorking()):
					cityPopList[self.AllowedUnitIndex[iType]] += 1
				
			iTeachLevelCity = pLoopCity.getTeachLevel()
			start = self.CitizenStart()
			for iUnitIndex in range(start, self.CitizenEnd()):
				iUnitCount = cityPopList[iUnitIndex]
				iUnit = self.AllowedUnits[iUnitIndex]
				ITeachLevelUnit = gc.getUnitInfo(iUnit).getTeachLevel()
				
				if (pLoopCity.getSpecialistTuition(iUnit) != -1):
					szText = u"<color=0,255,0>"
				elif (ITeachLevelUnit > 100):
					szText = u"<color=255,255,255>"
				elif (iTeachLevelCity >= ITeachLevelUnit):
					szText = u"<color=255,255,0>"
				else:
					szText =u"<color=255,0,0>"
				
				if (iUnitCount > 0):
					szText += unicode(iUnitCount)
				else:
					# a single green line is too tricky to see
					# 3 lines are visible while they still do not take attention away from the numbers
					szText += u"---"
				
				screen.setTableInt(szState + "ListBackground", iUnitIndex - start + 2, i, u"<font=2>" + szText + u"</font>", "", WidgetTypes.WIDGET_PEDIA_JUMP_TO_UNIT, iUnit, -1, CvUtil.FONT_LEFT_JUSTIFY )
		
		## R&R, Robert Surcouf, Domestic Market display START
		elif(self.CurrentState == self.GENERAL_STATE and self.CurrentPage == 2): 
			iStartYield=gc.getDefineINT("DOMESTIC_MARKET_SCREEN_START_YIELD_ID")
			for iYield in range(iStartYield, self.num_yields):
				#screen.setTableInt("GeneralStatePage3ListBackground", iYield-iStartYield + 2, i, "<font=2>" + unicode(pLoopCity.getYieldBuyPrice(iYield)) + "/"+ "<color=0,255,0>" +  unicode(pLoopCity.getYieldDemand(iYield)) + "</color>" "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
				if (pLoopCity.getYieldStored(iYield)<pLoopCity.getYieldDemand(iYield)):
					screen.setTableInt(szState + "ListBackground", iYield-iStartYield + 2, i, "<font=2>" + unicode(pLoopCity.getYieldBuyPrice(iYield)) + "/"+ "<color=255,0,0>" +  unicode(pLoopCity.getYieldDemand(iYield)) + "</color>" "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
				elif (pLoopCity.getYieldStored(iYield)>pLoopCity.getYieldDemand(iYield)):
					screen.setTableInt(szState + "ListBackground", iYield-iStartYield + 2, i, "<font=2>" + unicode(pLoopCity.getYieldBuyPrice(iYield)) + "/"+ "<color=0,255,0>" +  unicode(pLoopCity.getYieldDemand(iYield)) + "</color>" "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
				else:
					screen.setTableInt(szState + "ListBackground", iYield-iStartYield + 2, i, "<font=2>" + unicode(pLoopCity.getYieldBuyPrice(iYield)) + "/"+ "<color=0,255,255>" +  unicode(pLoopCity.getYieldDemand(iYield)) + "</color>" "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
		## R&R, Robert Surcouf, Domestic Market display End
		
	def updateCitizenTable(self, pCity, iRow):
		screen = CyGInterfaceScreen("DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR)

		if(self.CurrentState == self.CITIZEN_STATE):
			szState = self.StatePages[self.CurrentState][self.CurrentPage]
			screen.addPanel("CitizenPanel" + str(iRow), u"", u"", True, False, 0, 0, self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH, 30, PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, -1, -1)
			screen.attachControlToTableCell("CitizenPanel" + str(iRow), szState + "ListBackground", iRow, 2 )

			iSpace = (self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH - (self.iCityButtonSize / 4)) / pCity.getPopulation()
			if iSpace > self.iCityButtonSize:
				iSpace = self.iCityButtonSize
			ButtonSizePercentage = 40
			for iCitizen in range(pCity.getPopulation() - 1, -1, -1):
				pCitizen = pCity.getPopulationUnitByIndex(iCitizen)
				iType = pCitizen.getUnitType()
				szButtonName = "CitizenGroupButton" + str(iRow) + "-" + str(iCitizen)
				# R&R, Robert Surcouf, Rebellion Fix START
				#screen.setImageButtonAt(szButtonName, "CitizenPanel" + str(iRow), gc.getUnitInfo(iType).getButton(), iCitizen * iSpace, self.iCityButtonSize / 5, self.iCityButtonSize * 4 / 5, self.iCityButtonSize * 4 / 5, WidgetTypes.WIDGET_CITIZEN, pCitizen.getID(), pCity.getID())
				#screen.setImageButtonAt("CitizenProfession" + str(iRow) + "-" + str(iCitizen), "CitizenPanel" + str(iRow), gc.getProfessionInfo(pCitizen.getProfession()).getButton(), (iCitizen * iSpace) + (self.iCityButtonSize * 2 / 4), 0, self.iCityButtonSize * ButtonSizePercentage / 100 , self.iCityButtonSize * ButtonSizePercentage / 100, WidgetTypes.WIDGET_CITIZEN, pCitizen.getID(), pCity.getID())
				if pCitizen.getProfession() != ProfessionTypes.NO_PROFESSION:
					screen.setImageButtonAt(szButtonName, "CitizenPanel" + str(iRow), gc.getUnitInfo(iType).getButton(), iCitizen * iSpace, self.iCityButtonSize / 5, self.iCityButtonSize * 4 / 5, self.iCityButtonSize * 4 / 5, WidgetTypes.WIDGET_CITIZEN, pCitizen.getID(), pCity.getID())
					screen.setImageButtonAt("CitizenProfession" + str(iRow) + "-" + str(iCitizen), "CitizenPanel" + str(iRow), gc.getProfessionInfo(pCitizen.getProfession()).getButton(), (iCitizen * iSpace) + (self.iCityButtonSize * 2 / 4), 0, self.iCityButtonSize * ButtonSizePercentage / 100 , self.iCityButtonSize * ButtonSizePercentage / 100, WidgetTypes.WIDGET_CITIZEN, pCitizen.getID(), pCity.getID())
				else:
					#screen.setTextAt( szButtonName, "CitizenPanel" + str(iRow),  u"<font=2>" + u"%c" %(CyGame().getSymbolID(FontSymbols.ANGRY_POP_CHAR))+ "</font>", CvUtil.FONT_RIGHT_JUSTIFY, iCitizen * iSpace, self.iCityButtonSize / 5, -0.3, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
					screen.setImageButtonAt(szButtonName, "CitizenPanel" + str(iRow), gc.getUnitInfo(iType).getButton(), iCitizen * iSpace, self.iCityButtonSize / 5, self.iCityButtonSize * 4 / 5, self.iCityButtonSize * 4 / 5, WidgetTypes.WIDGET_GENERAL, -1, -1)
					screen.setTextAt("CitizenProfession" + str(iRow) + "-" + str(iCitizen), "CitizenPanel" + str(iRow), u"<font=2>" + u"%c" %(CyGame().getSymbolID(FontSymbols.ANGRY_POP_CHAR))+ "</font>", CvUtil.FONT_RIGHT_JUSTIFY, iCitizen * iSpace +  (self.iCityButtonSize * 2 / 4) + self.iCityButtonSize/2, self.iCityButtonSize / 5, -0.3, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )#(iCitizen * iSpace) + (self.iCityButtonSize * 2 / 4), 0, WidgetTypes.WIDGET_GENERAL, -1, -1 )
				
				# R&R, Robert Surcouf, Rebellion Fix END
	def updateRouteTable(self, pRoute, iRoute, iRow):
		screen = CyGInterfaceScreen("DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR)

		if(self.CurrentState == self.TRADEROUTE_STATE):
			szState = self.StatePages[self.CurrentState][self.CurrentPage]
			player = gc.getPlayer(gc.getGame().getActivePlayer())

			screen.setTableText(szState + "ListBackground", 0, iRow, "<font=4>" + pRoute.getSourceCityName() + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
			screen.setTableText(szState + "ListBackground", 1, iRow, "<font=4>" + pRoute.getDestinationCityName() + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			screen.setTableText(szState + "ListBackground", 2, iRow, u"<font=2>%c</font>" % gc.getYieldInfo(pRoute.getYield()).getChar(), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )

			screen.addPanel("RoutePanel" + str(iRow), u"", u"", True, False, 0, 0, (390 * self.nTableWidth) / self.nNormalizedTableWidth, 30, PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, iRoute, -1)
			screen.attachControlToTableCell("RoutePanel" + str(iRow), szState + "ListBackground", iRow, 4 )

			SelectionState = False
			iPosition = 10
			SelectedTransportID = -1
			ToggleButtonSet = False

			if (len(self.Transports)): ## R&R, Robert Surcouf, avoid division by zero !
				iSpace = (self.nTableWidth - 390) / len(self.Transports)
				if iSpace > self.iCityButtonSize:
					iSpace = self.iCityButtonSize
						
			for iSelectionGroupIndex in range(len(self.Transports) -1, -1, -1):
				SelectionGroup = self.Transports[iSelectionGroupIndex]

				if (not SelectionGroup.isNone()):
					GroupHeadID = SelectionGroup.getHeadUnit().getID()
					szButtonName = "UnitGroupButton" + str(SelectionGroup.getID()) + "Route" + str(iRow)
					if(SelectionGroup.isAssignedTradeRoute(pRoute.getID())):
						screen.addCheckBoxGFCAt("RoutePanel" + str(iRow), szButtonName, SelectionGroup.getHeadUnit().getButton(), ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(), iSelectionGroupIndex * iSpace, 0, self.iCityButtonSize, self.iCityButtonSize, WidgetTypes.WIDGET_GENERAL, 10001, SelectionGroup.getHeadUnit().getID(), ButtonStyles.BUTTON_STYLE_LABEL )
					elif (not self.RouteValidity[iSelectionGroupIndex][iRow]):
						screen.addDDSGFCAt( szButtonName, "RoutePanel" + str(iRow), ArtFileMgr.getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL").getPath(), iSelectionGroupIndex * iSpace, 0, self.iCityButtonSize, self.iCityButtonSize, WidgetTypes.WIDGET_GENERAL, -1, -1, False )

					iPosition += 20
					SelectedTransportID = self.selectedSelectionGroupHeadUnitID
					if (SelectedTransportID != -1):
						if (SelectedTransportID == SelectionGroup.getHeadUnit().getID()):
						
							if (self.RouteValidity[iSelectionGroupIndex][iRow]):
								if (SelectionGroup.isAssignedTradeRoute(pRoute.getID())):
									screen.setState(szButtonName, True)
									screen.setButtonGFC("RouteToggle" + str(iRow), "-", "", 0, 0, 60, 30, WidgetTypes.WIDGET_ASSIGN_TRADE_ROUTE, self.selectedSelectionGroupHeadUnitID, iRoute, ButtonStyles.BUTTON_STYLE_STANDARD )
								else:
									screen.setButtonGFC("RouteToggle" + str(iRow), "+", "", 0, 0, 60, 30, WidgetTypes.WIDGET_ASSIGN_TRADE_ROUTE, self.selectedSelectionGroupHeadUnitID, iRoute, ButtonStyles.BUTTON_STYLE_STANDARD )
								screen.attachControlToTableCell("RouteToggle" + str(iRow), szState + "ListBackground", iRow, 3 )
	
	# total production page - start - Nightinggale
	def updateTotalProduction(self):
		screen = CyGInterfaceScreen( "DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR )

		if self.CurrentState != self.TOTAL_PRODUCTION_STATE:
			return
		
		szState = self.StatePages[self.CurrentState][self.CurrentPage]
		start = self.YieldStart()
		
		for i in range(0,2):
			sign = ""
			line_name = "Warehouse"
			if i == 0:
				sign = u"+"
				line_name = "Production"
			screen.setTableText(szState + "ListBackground", 0, i, "<font=2>" +""         + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			screen.setTableText(szState + "ListBackground", 1, i, "<font=2>" + line_name + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			for iYield in range(start, self.YieldEnd()):
				iNetYield = 0
				if i == 0:
					for iCity in range(len(self.Cities)):
						iNetYield += self.Cities[iCity].calculateNetYield(iYield)
				else:
					for iCity in range(len(self.Cities)):
						iNetYield += self.Cities[iCity].getYieldStored(iYield)
				szText = unicode(iNetYield)
				if iNetYield > 0:
					szText = localText.getText("TXT_KEY_COLOR_POSITIVE", ()) + sign + szText + localText.getText("TXT_KEY_COLOR_REVERT", ())
				elif iNetYield < 0:
					szText = localText.getText("TXT_KEY_COLOR_NEGATIVE", ()) + szText + localText.getText("TXT_KEY_COLOR_REVERT", ())
				elif iNetYield == 0:
					szText = ""
				
				screen.setTableInt(szState + "ListBackground", iYield - start + 2, i, "<font=1>" + szText + "<font/>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
	# total production page - end - Nightinggale

	def BuildAllTransportsRow(self, iRow):
		screen = CyGInterfaceScreen("DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR)

		if(self.CurrentState == self.TRADEROUTE_STATE):
			szState = self.StatePages[self.CurrentState][self.CurrentPage]
			player = gc.getPlayer(gc.getGame().getActivePlayer())

			screen.addPanel("AllTransportPanel", u"", u"", True, False, 0, 0, (390 * self.nTableWidth) / self.nNormalizedTableWidth, 20, PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, iRow, -1)
			screen.attachControlToTableCell("AllTransportPanel", szState + "ListBackground", iRow, 4 )

			iSpace = self.iCityButtonSize
			if (len(self.Transports)):
				iSpace = (self.nTableWidth - 390) / len(self.Transports)
				if iSpace > self.iCityButtonSize:
					iSpace = self.iCityButtonSize

			SelectionState = False
			iPosition = 10
			SelectedTransportID = -1
			for iSelectionGroupIndex in range(len(self.Transports) -1, -1, -1):
				SelectionGroup = self.Transports[iSelectionGroupIndex]

				if (not SelectionGroup.isNone()):
					GroupHeadID = SelectionGroup.getHeadUnit().getID()
					szButtonName = "AllTransport" + str(SelectionGroup.getID())
					screen.addCheckBoxGFCAt("AllTransportPanel", szButtonName, SelectionGroup.getHeadUnit().getButton(), ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(), iSelectionGroupIndex * iSpace, 0, self.iCityButtonSize, self.iCityButtonSize, WidgetTypes.WIDGET_GENERAL, 10001, SelectionGroup.getHeadUnit().getID(), ButtonStyles.BUTTON_STYLE_LABEL )
					screen.setLabelAt("SelectionGroupCount" + str(SelectionGroup.getID()), szButtonName, "<font=4>" + str(SelectionGroup.getNumUnits()) + "</font>", CvUtil.FONT_LEFT_JUSTIFY, 35, 35, -1.3, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )

					iPosition += 20
					SelectedTransportID = self.selectedSelectionGroupHeadUnitID
					if (SelectedTransportID != -1):
						if (SelectedTransportID == SelectionGroup.getHeadUnit().getID()):
							screen.setState(szButtonName, True)
							SelectionState = True
							
	def RebuildArrays (self):
		#Get a list of the Players Cities
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		self.Cities = []
		(pLoopCity, iter) = player.firstCity(false)
		while(pLoopCity):
			self.Cities.append(pLoopCity)
			(pLoopCity, iter) = player.nextCity(iter, false)

		self.Routes = []
		for iRoute in range(player.getNumTradeRoutes()):
			self.Routes.append(player.getTradeRouteByIndex(iRoute))

		self.Transports = []
		SelectionGroup, Iterator = player.firstSelectionGroup(false)
		while (SelectionGroup != None):
			if (SelectionGroup.canAssignTradeRoute(-1, false)):
				self.Transports.append(SelectionGroup)		
			SelectionGroup, Iterator = player.nextSelectionGroup(Iterator, false)

		self.RouteValidity = []
		for iTransport in range(len(self.Transports)):
			Transport = self.Transports[iTransport]
			RouteValidArray = []
			bReusePath = false
			for Route in self.Routes:
				RouteValidArray.append(Transport.canAssignTradeRoute(Route.getID(), bReusePath))
				bReusePath = true
			self.RouteValidity.append(RouteValidArray)
		
		
		## R&R, Robert Surcouf,  Domestic Advisor Screen - Start
		self.NativeCities = []
		for iLoopPlayer in range(gc.getMAX_CIV_PLAYERS()):
			ePlayer = gc.getPlayer(iLoopPlayer)
			#if (player.isAlive() and player.isNative() and (gc.getTeam(player.getTeam()).isHasMet(activePlayer.getTeam()))):
			if (ePlayer.isAlive() and ePlayer.isNative()):
				(pLoopCity, iter) = ePlayer.firstCity(false)
				while(pLoopCity):
					self.NativeCities.append(pLoopCity)
					(pLoopCity, iter) = ePlayer.nextCity(iter, false)
		## R&R, Robert Surcouf,  Domestic Advisor Screen - End
	def RebuildTransportTable (self):
		if self.CurrentState != self.TRADEROUTE_STATE:
			return
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		screen = CyGInterfaceScreen( "DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR )

		szStateName = self.StatePages[self.TRADEROUTE_STATE][0]
		screen.setImageButton( szStateName + "Button", ArtFileMgr.getInterfaceArtInfo(self.StateButtons[self.TRADEROUTE_STATE]).getPath(), (self.iButtonSpacing * 5) + (self.iButtonSpacing / 2), self.Y_LOWER_ROW, self.iButtonSize, self.iButtonSize, WidgetTypes.WIDGET_GENERAL, 5, -1 )
		
		screen.addTableControlGFC( szStateName + "ListBackground", 19, (self.nScreenWidth - self.nTableWidth) / 2, 60, self.nTableWidth, self.nTableHeight, True, False, self.iCityButtonSize, self.iCityButtonSize, TableStyles.TABLE_STYLE_STANDARD )
		screen.enableSelect( szStateName + "ListBackground", True )
		screen.enableSort( szStateName + "ListBackground" )
		screen.setStyle( szStateName + "ListBackground", "Table_StandardCiv_Style")
		screen.hide( szStateName + "ListBackground")
		screen.setTableColumnHeader( szStateName + "ListBackground", 0, "", (30 * self.nTableWidth) / self.nNormalizedTableWidth )
		screen.setTableColumnHeader( szStateName + "ListBackground", 1, "<font=2>" + localText.getText("TXT_KEY_DOMESTIC_ADVISOR_NAME", ()) + "</font>", (221 * self.nTableWidth) / self.nNormalizedTableWidth )
		
		
	def RebuildRouteTable (self):
		if self.CurrentState != self.TRADEROUTE_STATE:
			return
	
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		screen = CyGInterfaceScreen( "DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR )

		szTableName = self.StatePages[self.TRADEROUTE_STATE][0] + "ListBackground"
		screen.setImageButton( szTableName + "Button", ArtFileMgr.getInterfaceArtInfo(self.StateButtons[self.TRADEROUTE_STATE]).getPath(), (self.iButtonSpacing * 5) + (self.iButtonSpacing / 2), self.Y_LOWER_ROW, self.iButtonSize, self.iButtonSize, WidgetTypes.WIDGET_GENERAL, 5, -1 )

		screen.addTableControlGFC( szTableName, 19, (self.nScreenWidth - self.nTableWidth) / 2, 60, self.nTableWidth, self.nTableHeight, True, False, self.iCityButtonSize, self.iCityButtonSize, TableStyles.TABLE_STYLE_STANDARD )
		screen.enableSelect( szTableName, True )
		screen.enableSort( szTableName )
		screen.setStyle( szTableName, "Table_StandardCiv_Style" )
		screen.hide( szTableName )

		screen.setTableColumnHeader( szTableName, 0, localText.getText("TXT_KEY_SOURCE", ()), (150 * self.nTableWidth) / self.nNormalizedTableWidth)
		screen.setTableColumnHeader( szTableName, 1, localText.getText("TXT_KEY_DESTINATION", ()), (150 * self.nTableWidth) / self.nNormalizedTableWidth)
		screen.setTableColumnHeader( szTableName, 2, localText.getText("TXT_KEY_YIELD", ()), (65 * self.nTableWidth) / self.nNormalizedTableWidth)
		screen.setTableColumnHeader( szTableName, 3, " +/-", (45 * self.nTableWidth) / self.nNormalizedTableWidth)
		screen.setTableColumnHeader( szTableName, 4, localText.getText("TXT_KEY_TRANSPORT_PANEL", ()), self.nTableWidth - 390)

		for iRoute in range(len(self.Routes)):
			screen.appendTableRow( szTableName )
			screen.setTableRowHeight(szTableName, iRoute, self.ROW_HIGHT)

		screen.appendTableRow( szTableName )
		screen.setTableRowHeight(szTableName, len(self.Routes), self.ROW_HIGHT)
		
	## R&R, Robert Surcouf,  Domestic Advisor Screen START
	def updateNativeTable(self):
		if self.CurrentState != self.NATIVE_STATE:
			return
	
		screen = CyGInterfaceScreen( "DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR )
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		
		szState = self.StatePages[self.NATIVE_STATE][0]
		screen.addTableControlGFC( szState + "ListBackground", 7, (self.nScreenWidth - self.nTableWidth) / 2, 60, self.nTableWidth, self.nTableHeight, True, False, self.iCityButtonSize, self.iCityButtonSize, TableStyles.TABLE_STYLE_STANDARD )
		screen.enableSelect( szState + "ListBackground", True )
		screen.enableSort( szState + "ListBackground" )
		screen.setStyle( szState + "ListBackground", "Table_StandardCiv_Style" )
		screen.hide( szState + "ListBackground" )
		szStateName = szState + "ListBackground"
		screen.setTableColumnHeader( szStateName, 0, "<font=2>" + localText.getText("TXT_KEY_DOMESTIC_ADVISOR_NAME", ()).upper() + "</font>", self.CITY_NAME_COLUMN_WIDTH - 56 )
		screen.setTableColumnHeader( szStateName, 1, "<font=2>" + "CIV" + "</font>", self.CITY_NAME_COLUMN_WIDTH -56)
		screen.setTableColumnHeader( szStateName, 2, "<font=2>" + localText.getText("TXT_KEY_POPULATION", ()) + "</font>", (self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH) / 12 )
		screen.setTableColumnHeader( szStateName, 3, "<font=2>" + (u" %c" % CyGame().getSymbolID(FontSymbols.TRADE_CHAR)) + "</font>", 50 )
		screen.setTableColumnHeader( szStateName, 4, "<font=2>" + (u" %c" % gc.getYieldInfo(YieldTypes.YIELD_CROSSES).getChar()) + "</font>", 130)
		# Native advisor update - start - Nightinggale
		screen.setTableColumnHeader( szStateName, 5, "<font=2>" + localText.getText("TXT_KEY_PEDIA_SPECIAL_ABILITIES", ()).upper() + "</font>", 200)
		# Native advisor update - end - Nightinggale
		
		iter=0
		for iNativeCity in range(len(self.NativeCities)):
			pLoopCity =self.NativeCities[iNativeCity]
			
			ePlayer = gc.getPlayer(pLoopCity.getOwner())
			
			#if (pLoopCity.isRevealed(gc.getGame().getActiveTeam(), false)):
			if (pLoopCity.isScoutVisited(gc.getGame().getActiveTeam())):
			#if (ePlayer.isAlive() and (gc.getTeam(ePlayer.getTeam()).isHasMet(player.getTeam()))):
			#if (ePlayer.isAlive()):
			
				screen.appendTableRow( szStateName )
				screen.setTableRowHeight(szStateName, iter, self.ROW_HIGHT)
				
				screen.setTableText(szState + "ListBackground", 0, iter, "<font=2>" + self.NativeCities[iNativeCity].getName() + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
				screen.setTableText(szState + "ListBackground", 1, iter, "<font=2>" + gc.getPlayer(self.NativeCities[iNativeCity].getOwner()).getCivilizationShortDescription(0) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
				screen.setTableInt(szState + "ListBackground", 2, iter, "<font=2>" + unicode(self.NativeCities[iNativeCity].getPopulation()) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
				
				#screen.setTableInt(szState + "ListBackground", 3, iter, "<font=2>" + unichr(CyGame().getSymbolID(FontSymbols.HAPPY_CHAR)+12+iNativeCity) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
				iYield= self.NativeCities[iNativeCity].AI_getDesiredYield()
				screen.setTableInt(szState + "ListBackground", 3, iter, "<font=2>" + (u" %c" % gc.getYieldInfo(iYield).getChar()) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
				
				#if gc.getPlayer(pLoopCity.getMissionaryPlayer() != PlayerTypes.NO_PLAYER):
				if pLoopCity.getMissionaryRate() > 0 and pLoopCity.getMissionaryPlayer() != -1: #and gc.getPlayer(pLoopCity.getMissionaryPlayer()) != PlayerTypes.NO_PLAYER:
					iModifier = 100 + player.getMissionaryRateModifier()+ ePlayer.getMissionaryRateModifier()
					#if gc.getGame().isOption(GameOptionTypes.GAMEOPTION_NO_MORE_VARIABLES_HIDDEN) and pLoopCity.getMissionaryPlayer()==gc.getPlayer(gc.getGame().getActivePlayer()):
					if gc.getGame().isOption(GameOptionTypes.GAMEOPTION_NO_MORE_VARIABLES_HIDDEN):
						#screen.setTableText(szState + "ListBackground", 4, iter, "<font=2>"  + localText.getText("TXT_KEY_GROWTH", (ePlayer.getMissionaryPoints(gc.getGame().getActivePlayer()),ePlayer.missionaryThreshold(gc.getGame().getActivePlayer()), pLoopCity.getMissionaryRate())) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )	
						screen.setTableText(szState + "ListBackground", 4, iter, "<font=2>"  + localText.getText("TXT_KEY_GROWTH", (ePlayer.getMissionaryPoints(gc.getGame().getActivePlayer()),ePlayer.missionaryThreshold(gc.getGame().getActivePlayer()), pLoopCity.getMissionaryRate() * iModifier /100)) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )	
					else:
						screen.setTableInt(szState + "ListBackground", 4, iter, "<font=2>" + "<font=2>" + unichr(gc.getCivilizationInfo(gc.getPlayer(pLoopCity.getMissionaryPlayer()).getCivilizationType()).getMissionaryChar()) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
						#screen.setTableInt(szState + "ListBackground", 4, iter, "<font=2>" + unicode(pLoopCity.getPopulation()) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
				elif pLoopCity.getMissionaryRate() > 0 and pLoopCity.getMissionaryPlayer() == -1 and gc.getGame().isOption(GameOptionTypes.GAMEOPTION_NO_MORE_VARIABLES_HIDDEN):
					screen.setTableText(szState + "ListBackground", 4, iter, "<font=2>"  + localText.getText("TXT_KEY_GROWTH", (ePlayer.getMissionaryPoints(gc.getGame().getActivePlayer()),ePlayer.missionaryThreshold(gc.getGame().getActivePlayer()), 0)) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )	
				
				# Native advisor update - start - Nightinggale
				screen.setTableText(szState + "ListBackground", 5, iter, "<font=2>" + gc.getUnitInfo(self.NativeCities[iNativeCity].getTeachUnit()).getDescription() + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
				# Native advisor update - end - Nightinggale
				
				iter+= 1
		## R&R, Robert Surcouf,  Domestic Advisor Screen END
	
	
	# Will handle the input for this screen...
	def handleInput (self, inputClass):
		' Calls function mapped in DomesticAdvisorInputMap'
		screen = CyGInterfaceScreen( "DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR )
		if ( inputClass.getNotifyCode() == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED ):
			## R&R, Robert Surcouf,  Domestic Advisor Screen START
			if self.CurrentState != self.TRADEROUTE_STATE and self.CurrentState != self.NATIVE_STATE:
			## R&R, Robert Surcouf,  Domestic Advisor Screen END
				if (inputClass.getMouseX() == 0):
					screen = CyGInterfaceScreen( "DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR )
					screen.hideScreen()

					CyInterface().selectCity(gc.getPlayer(inputClass.getData1()).getCity(inputClass.getData2()), true);

					popupInfo = CyPopupInfo()
					popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON_SCREEN)
					popupInfo.setText(u"showDomesticAdvisor")
					popupInfo.addPopup(inputClass.getData1())
				else:
					self.updateAppropriateCitySelection()

		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED):
			if (inputClass.getButtonType() == WidgetTypes.WIDGET_GENERAL):
				iData = inputClass.getData1()
				if (iData >= 0 and iData < len(self.StatePages)):
					if(self.CurrentState != iData):
						screen.hide(self.StatePages[self.CurrentState][self.CurrentPage] + "ListBackground")
						if self.CurrentState not in self.YieldPages or iData not in self.YieldPages:
							if self.CurrentState not in self.CitizenPages or iData not in self.CitizenPages:
								if self.CurrentState not in self.BuildingPages or iData not in self.BuildingPages:
									self.CurrentPage = 0
						self.CurrentState = iData
						self.drawContents()
				# auto-generated list creation - start - Nightinggale
				elif (iData == 100 or iData == 102):
					# iData == 1 was already taken. (100, 102) - 101 gives the -1/+1 needed
					new_page = self.CurrentPage + iData - 101
					
					if (new_page >= 0 and new_page < len(self.StatePages[self.CurrentState])):
						screen.hide(self.StatePages[self.CurrentState][self.CurrentPage] + "ListBackground")
						self.CurrentPage = new_page
						self.drawContents()
				# auto-generated list creation - end - Nightinggale
				elif (iData == 10001):
					if (self.selectedSelectionGroupHeadUnitID == inputClass.getData2()):
						self.selectedSelectionGroupHeadUnitID = -1
						self.drawContents()
					else:
						self.selectedSelectionGroupHeadUnitID = inputClass.getData2()
						self.drawContents()
		return 0

	def updateAppropriateCitySelection(self):
		nCities = gc.getPlayer(gc.getGame().getActivePlayer()).getNumCities()
		screen = CyGInterfaceScreen( "DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR )
		screen.updateAppropriateCitySelection( self.StatePages[self.CurrentState][self.CurrentPage] + "ListBackground", nCities, 1 )
		self.listSelectedCities = []
		for iCity in range(nCities):
			if screen.isRowSelected(self.StatePages[self.CurrentState][self.CurrentPage] + "ListBackground", iCity):
				self.listSelectedCities.append(screen.getTableText(self.StatePages[self.CurrentState][self.CurrentPage] + "ListBackground", 2, iCity))

	def update(self, fDelta):
		if (CyInterface().isDirty(InterfaceDirtyBits.Domestic_Advisor_DIRTY_BIT)):
			CyInterface().setDirty(InterfaceDirtyBits.Domestic_Advisor_DIRTY_BIT, False)
			player = gc.getPlayer(gc.getGame().getActivePlayer())

			self.RebuildArrays()
			
			for iCity in range(len(self.Cities)):
				self.updateCityTable(self.Cities[iCity], iCity)

			self.updateTotalProduction()
			self.RebuildRouteTable()
			self.RebuildTransportTable()

			## R&R, Robert Surcouf,  Domestic Advisor Screen START
			self.updateNativeTable()
			## R&R, Robert Surcouf,  Domestic Advisor Screen END
			self.drawContents()
			
	def getWidgetHelp(self, argsList):
		iScreen, eWidgetType, iData1, iData2, bOption = argsList

		if eWidgetType == WidgetTypes.WIDGET_GENERAL:
			if iData1 >= 0 and iData1 < len(self.StateHelp):
				return localText.getText(self.StateHelp[iData1], ())
			elif iData1 == 10001:
				unit = gc.getActivePlayer().getUnit(iData2)
				if not unit.isNone():
					return CyGameTextMgr().getSpecificUnitHelp(unit, true, false)
	
	## R&R, Robert Surcouf,  Domestic Advisor Screen - Start
	def getGeneralStateColumnSize(self, iNum):
		return (self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH) / iNum 
	## R&R, Robert Surcouf,  Domestic Advisor Screen - End
	
	# small functions to simplify looping though columns in current page - Nightinggale
	def YieldStart(self):
		return self.MAX_YIELDS_IN_A_PAGE * self.CurrentPage
		
	def YieldEnd(self):
		return min((self.MAX_YIELDS_IN_A_PAGE * (self.CurrentPage + 1)), len(self.AllowedYields))
		
	def BuildingStart(self):
		return self.MAX_BUILDINGS_IN_A_PAGE * self.CurrentPage
		
	def BuildingEnd(self):
		return min((self.MAX_BUILDINGS_IN_A_PAGE * (self.CurrentPage + 1)), len(self.AllowedSpecialBuildings))
	
	def CitizenStart(self):
		return self.MAX_UNITS_IN_A_PAGE * self.CurrentPage
		
	def CitizenEnd(self):
		return min((self.MAX_UNITS_IN_A_PAGE * (self.CurrentPage + 1)), len(self.AllowedUnits))
	
	# auto-generated list creation - start - Nightinggale
	def addButton(self, state_button, state_help):
		index = len(self.StatePages)
		self.StateButtons.append(state_button)
		self.StateNames.append("State" + str(index))
		self.StatePages.append([])
		self.StateHelp.append(state_help)
		return index
		
	def createSubpage(self, iState, iPage, iColumns):
		length = len(self.StatePages[iState])
		if (length == iPage):
			self.StatePages[iState].append(self.StateNames[iState] + "Page" + str(length))
			self.initPage(iState, length, iColumns)
			#self.createSubpage(iState, iPage, iColumns)
			
	def initPage(self, iState, iPage, iColumns):
		if iState != self.TRADEROUTE_STATE and iState != self.NATIVE_STATE:
			screen = CyGInterfaceScreen( "DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR )
			szStateName = self.StatePages[iState][iPage] + "ListBackground"
			## R&R, Robert Surcouf,  Domestic Advisor Screen START
			#screen.addTableControlGFC(szStateName, 22, (self.nScreenWidth - self.nTableWidth) / 2, 60, self.nTableWidth, self.nTableHeight, True, False, self.iCityButtonSize, self.iCityButtonSize, TableStyles.TABLE_STYLE_STANDARD )
			screen.addTableControlGFC(szStateName, iColumns + 2, (self.nScreenWidth - self.nTableWidth) / 2, 60, self.nTableWidth, self.nTableHeight, True, False, self.iCityButtonSize, self.iCityButtonSize, TableStyles.TABLE_STYLE_STANDARD )
			screen.setStyle(szStateName, "Table_StandardCiv_Style")
			screen.hide(szStateName)
			#screen.setTableColumnHeader(szStateName, 0, "", 45 )
			screen.setTableColumnHeader(szStateName, 0, "", 56 )
			## R&R, Robert Surcouf,  Domestic Advisor Screen END
			screen.setTableColumnHeader(szStateName, 1, "<font=2>" + localText.getText("TXT_KEY_DOMESTIC_ADVISOR_NAME", ()).upper() + "</font>", self.CITY_NAME_COLUMN_WIDTH - 56 )

			# total production page - start - Nightinggale
			num_cities = 2
			
			if iState != self.TOTAL_PRODUCTION_STATE:
				num_cities = len(self.Cities)
				screen.enableSelect(szStateName, True)
				screen.enableSort(szStateName)
			
			for iCity in range(num_cities):
			# for iCity in range(len(self.Cities)):
			# total production page - end - Nightinggale
				screen.appendTableRow(szStateName)
				screen.setTableRowHeight(szStateName, iCity, self.ROW_HIGHT)
				
	def getNumColumns(self, iMinSpace, iNumColumns):
		iSpace = self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH
		
		iMaxOnPage = iSpace // iMinSpace
		iPages = iNumColumns // iMaxOnPage
		if ((iPages * iMaxOnPage) < iNumColumns):
			iPages += 1
		
		iColumnsOnPage = iNumColumns // iPages
		if ((iColumnsOnPage * iPages) < iNumColumns):
			iColumnsOnPage += 1
		
		return iColumnsOnPage
		
	def getColumnWidth(self, iIndex, iMax, iNumOnPage, iPage):
		iSpace = self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH
		
		if ((iMax // iNumOnPage) == iPage):
			iCount = iMax % iNumOnPage
		else:
			iCount = iNumOnPage
		
		iWidth = iSpace // iCount
		
		if ((iIndex + 1) == iMax or ((iIndex + 1) % iNumOnPage) == 0):
			iWidth += iSpace % iCount
		
		return [ iWidth, iCount ]
	# auto-generated list creation - end - Nightinggale
