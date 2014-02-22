#pragma once

// plotGroup.h

#ifndef CIV4_PLOT_GROUP_H
#define CIV4_PLOT_GROUP_H

//#include "CvStructs.h"
#include "LinkedList.h"

class CvPlot;
class CvPlotGroup
{

public:

	CvPlotGroup();
	virtual ~CvPlotGroup();

	void init(int iID, PlayerTypes eOwner, CvPlot* pPlot);
	void uninit();
	void reset(int iID = 0, PlayerTypes eOwner = NO_PLAYER, bool bConstructorCall=false);

	void addPlot(CvPlot* pPlot);
	void removePlot(CvPlot* pPlot);
	void recalculatePlots();														

	int getID() const;
	void setID(int iID);

	PlayerTypes getOwner() const;
#ifdef _USRDLL
	inline PlayerTypes getOwnerINLINE() const
	{
		return m_eOwner;
	}
#endif

#ifdef USE_PLOTGROUP_RESOURCES
	int getNumBonuses(BonusTypes eBonus) const;
	bool hasBonus(BonusTypes eBonus) const;										
	void changeNumBonuses(BonusTypes eBonus, int iChange);
#endif

	void insertAtEndPlots(XYCoords xy);			
	CLLNode<XYCoords>* deletePlotsNode(CLLNode<XYCoords>* pNode);
	CLLNode<XYCoords>* nextPlotsNode(CLLNode<XYCoords>* pNode);
	int getLengthPlots();
	CLLNode<XYCoords>* headPlotsNode();

	// for serialization
	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);

	// homemade function - Nightinggale
	void deleteAllPlots();

protected:

	int m_iID;

	PlayerTypes m_eOwner;
#ifdef USE_PLOTGROUP_RESOURCES
	BonusArray<int> m_aiNumBonuses;
#endif

	CLinkList<XYCoords> m_plots;
};

inline int CvPlotGroup::getID() const
{
	return m_iID;
}

inline void CvPlotGroup::setID(int iID)
{
	m_iID = iID;
}

inline PlayerTypes CvPlotGroup::getOwner() const
{
	return getOwnerINLINE();
}

#ifdef USE_PLOTGROUP_RESOURCES
inline int CvPlotGroup::getNumBonuses(BonusTypes eBonus) const
{
	return m_aiNumBonuses.get(eBonus);
}

// Nightinggale note: getNumBonuses() always be used instead of hasBonus()
// the reason is that when an int is used as a bool, the compiler will assume != 0 to turn it into a bool
// I added it anyway for completeness because it's part of vanilla Civ4BTS
// both functions should be equally fast due to inlining
// Civ4 doesn't have hasBonus() inlined meaning it adds overhead as an extra function call
inline bool CvPlotGroup::hasBonus(BonusTypes eBonus) const
{
	return(getNumBonuses(eBonus) > 0);
}
#endif
#endif
