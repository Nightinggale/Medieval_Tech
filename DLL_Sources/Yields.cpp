//
// Yields.cpp
// Written by Nightinggale
//
#include "CvGameCoreDLL.h"
#include "CvGlobals.h"
#include "YieldsTestBase.h"

void BaseCheckYieldGroup::build()
{
	FAssertMsg(false, "The base function shouldn't call this virtual function");
}

bool BaseCheckYieldGroup::function(YieldTypes eYield)
{
	FAssertMsg(false, "The base function shouldn't call this virtual function");
	return false;
}

void BaseCheckYieldGroup::check()
{
	this->build();

	bool checklist[NUM_YIELD_TYPES];

	for (int i = 0; i < NUM_YIELD_TYPES; i++)
	{
		checklist[i] = false;
	}

	for(unsigned int iYield = 0; iYield < YieldVector.size(); iYield++) {
		FAssert(YieldVector[iYield] < NUM_YIELD_TYPES);
		FAssert(YieldVector[iYield] >= 0);
		checklist[YieldVector[iYield]] = true;
	}

	for (int i = 0; i < NUM_YIELD_TYPES; i++)
	{
		YieldTypes eYield = (YieldTypes)i;
		if (checklist[i])
		{
			FAssertMsg(bAllowVirtual || !YieldGroup_Virtual(eYield), CvString::format("Virtual yield %s used in %s even though this group doesn't allow virtual yields", GC.getYieldInfo(eYield).getType(), func_name).c_str());
			FAssertMsg(this->function(eYield), CvString::format("Yield %s missing in %s", GC.getYieldInfo(eYield).getType(), func_name).c_str());
		} else {
			FAssertMsg(!this->function(eYield), CvString::format("Yield %s is a false positive in %s", GC.getYieldInfo(eYield).getType(), func_name).c_str());
		}
	}
}

void BaseCheckYieldGroup::checkSingleXMLType(YieldTypes eYield, const char* XMLname)
{
	FAssert(eYield >= 0 && eYield < NUM_YIELD_TYPES);
	XMLnameChecked[eYield] = true;
	FAssertMsg(!strcmp(GC.getYieldInfo(eYield).getType(), XMLname), CvString::format("XML error. Found %s instead of %s at index %d", GC.getYieldInfo(eYield).getType(), XMLname, eYield).c_str());
}

void CvGlobals::CheckEnumYieldTypes() const
{
#ifdef FASSERT_ENABLE
	BaseCheckYieldGroup BaseGroup;
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		BaseGroup.XMLnameChecked[iYield] = false;
	}
	
	BaseGroup.checkXML();
	FAssertMsg(GC.getYieldInfo().size() == NUM_YIELD_TYPES, CvString::format("XML error. Expected %d types, but found %d", NUM_YIELD_TYPES, GC.getYieldInfo().size()));

	for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		YieldTypes eYield = (YieldTypes)iYield;
		FAssertMsg(BaseGroup.XMLnameChecked[iYield], CvString::format("Yield %s not checked for consistency between enum and XML", GC.getYieldInfo(eYield).getType()).c_str());
		if (YieldGroup_Virtual(eYield))
		{
			FAssertMsg(!GC.getYieldInfo(eYield).isCargo(), CvString::format("Virtual yield %s has bCargo set in XML", GC.getYieldInfo(eYield).getType()).c_str());
		} else {
			FAssertMsg(GC.getYieldInfo(eYield).isCargo(), CvString::format("Non-virtual yield %s has bCargo unset in XML", GC.getYieldInfo(eYield).getType()).c_str());
		}
	}

	Check_YieldGroup_AI_Sell AI_Sell;
	AI_Sell.check();
	Check_YieldGroup_AI_Sell_To_Europe AI_Sell_To_Europe;
	AI_Sell_To_Europe.check();
	Check_YieldGroup_AI_Buy_From_Natives AI_Buy_From_Natives;
	AI_Buy_From_Natives.check();
	Check_YieldGroup_AI_Buy_From_Europe AI_Buy_From_Europe;
	AI_Buy_From_Europe.check();
	Check_YieldGroup_AI_Raw_Material AI_Raw_Material;
	AI_Raw_Material.check();
	Check_YieldGroup_AI_Native_Product AI_Native_Product;
	AI_Native_Product.check();
	Check_YieldGroup_City_Billboard City_Billboard;
	City_Billboard.check();
	Check_YieldGroup_City_Billboard_Offset_Fix City_Billboard_Offset_Fix;
	City_Billboard_Offset_Fix.check();
	Check_YieldGroup_Armor Armor;
	Armor.check();
	Check_YieldGroup_Light_Armor Light_Armor;
	Light_Armor.check();
	Check_YieldGroup_Heavy_Armor Heavy_Armor;
	Heavy_Armor.check();

	// check armor groups
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		YieldTypes eYield = (YieldTypes)iYield;
		if (YieldGroup_Armor(eYield))
		{
			FAssertMsg(YieldGroup_Light_Armor(eYield) || YieldGroup_Heavy_Armor(eYield), CvString::format("%s is armor, but neither light or heavy armor", GC.getYieldInfo(eYield).getType()).c_str());
			FAssertMsg(!(YieldGroup_Light_Armor(eYield) && YieldGroup_Heavy_Armor(eYield)), CvString::format("%s is both light and heavy armor", GC.getYieldInfo(eYield).getType()).c_str());
		} else {
			FAssertMsg(!YieldGroup_Light_Armor(eYield), CvString::format("%s is light armor, but not armor", GC.getYieldInfo(eYield).getType()).c_str());
			FAssertMsg(!YieldGroup_Heavy_Armor(eYield), CvString::format("%s is heavy armor, but not armor", GC.getYieldInfo(eYield).getType()).c_str());
		}
	}
#endif
}