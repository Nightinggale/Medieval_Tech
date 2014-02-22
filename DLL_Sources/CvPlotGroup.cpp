// plotGroup.cpp

#include "CvGameCoreDLL.h"
#include "CvPlotGroup.h"
#include "CvPlot.h"
#include "CvGlobals.h"
#include "CvPlayerAI.h"
#include "CvMap.h"
#include "CvCity.h"
#include "CvDLLFAStarIFaceBase.h"
#include "FProfiler.h"

// Public Functions...

CvPlotGroup::CvPlotGroup()
{
	reset(0, NO_PLAYER, true);
}


CvPlotGroup::~CvPlotGroup()
{
	uninit();
}


void CvPlotGroup::init(int iID, PlayerTypes eOwner, CvPlot* pPlot)
{
	//--------------------------------
	// Init saved data
	reset(iID, eOwner);

	//--------------------------------
	// Init non-saved data

	//--------------------------------
	// Init other game data
	addPlot(pPlot);
}


void CvPlotGroup::uninit()
{
#ifdef USE_PLOTGROUP_RESOURCES
	m_aiNumBonuses.reset();
#endif

	m_plots.clear();
}

// FUNCTION: reset()
// Initializes data members that are serialized.
void CvPlotGroup::reset(int iID, PlayerTypes eOwner, bool bConstructorCall)
{
	//--------------------------------
	// Uninit class
	uninit();

	m_iID = iID;
	m_eOwner = eOwner;
}


void CvPlotGroup::addPlot(CvPlot* pPlot)
{
	XYCoords xy;

	xy.iX = pPlot->getX_INLINE();
	xy.iY = pPlot->getY_INLINE();

	insertAtEndPlots(xy);

	pPlot->setPlotGroup(getOwnerINLINE(), this);
}


void CvPlotGroup::removePlot(CvPlot* pPlot)
{
	CLLNode<XYCoords>* pPlotNode;

	pPlotNode = headPlotsNode();

	while (pPlotNode != NULL)
	{
		if (GC.getMapINLINE().plotSorenINLINE(pPlotNode->m_data.iX, pPlotNode->m_data.iY) == pPlot)
		{
			pPlot->setPlotGroup(getOwnerINLINE(), NULL);

			pPlotNode = deletePlotsNode(pPlotNode); // can delete this PlotGroup...
			break;
		}
		else
		{
			pPlotNode = nextPlotsNode(pPlotNode);
		}
	}
}


// homemade function - Nightinggale
// delete all plots in plotgroup and the plotgroup itself
// called when recalculatePlots() will produce the same result, except this one is faster due to less check
void CvPlotGroup::deleteAllPlots()
{
	CLLNode<XYCoords>* pPlotNode = headPlotsNode();
	PlayerTypes eOwner = getOwnerINLINE();

	while (pPlotNode != NULL)
	{
		CvPlot* pPlot = GC.getMapINLINE().plotSorenINLINE(pPlotNode->m_data.iX, pPlotNode->m_data.iY);

		FAssertMsg(pPlot != NULL, "Plot is not assigned a valid value");

		XYCoords xy;

		xy.iX = pPlot->getX_INLINE();
		xy.iY = pPlot->getY_INLINE();

		pPlot->setPlotGroup(eOwner, NULL);

		pPlotNode = deletePlotsNode(pPlotNode); // will delete this PlotGroup...
	}
}

void CvPlotGroup::recalculatePlots()
{
	PROFILE_FUNC();

	CLLNode<XYCoords>* pPlotNode;
	CvPlot* pPlot;
	CLinkList<XYCoords> oldPlotGroup;
	XYCoords xy;
	PlayerTypes eOwner;

	eOwner = getOwnerINLINE();

	pPlotNode = headPlotsNode();

	oldPlotGroup.clear();

	pPlotNode = headPlotsNode();

	while (pPlotNode != NULL)
	{
		pPlot = GC.getMapINLINE().plotSorenINLINE(pPlotNode->m_data.iX, pPlotNode->m_data.iY);

		FAssertMsg(pPlot != NULL, "Plot is not assigned a valid value");

		xy.iX = pPlot->getX_INLINE();
		xy.iY = pPlot->getY_INLINE();

		oldPlotGroup.insertAtEnd(xy);

		pPlot->setPlotGroup(eOwner, NULL);

		pPlotNode = deletePlotsNode(pPlotNode); // will delete this PlotGroup...
	}

	pPlotNode = oldPlotGroup.head();

	while (pPlotNode != NULL)
	{
		pPlot = GC.getMapINLINE().plotSorenINLINE(pPlotNode->m_data.iX, pPlotNode->m_data.iY);

		FAssertMsg(pPlot != NULL, "Plot is not assigned a valid value");

		pPlot->updatePlotGroup(eOwner, true);

		pPlotNode = oldPlotGroup.deleteNode(pPlotNode);
	}

	// update the city cache
	GET_PLAYER(eOwner).clearPlotgroupCityCache();
}

#ifdef USE_PLOTGROUP_RESOURCES
void CvPlotGroup::changeNumBonuses(BonusTypes eBonus, int iChange)
{
	CLLNode<XYCoords>* pPlotNode;
	CvCity* pCity;
	int iOldNumBonuses;

	FAssertMsg(eBonus >= 0, "eBonus is expected to be non-negative (invalid Index)");
	FAssertMsg(eBonus < GC.getNumBonusInfos(), "eBonus is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		iOldNumBonuses = getNumBonuses(eBonus);

		m_aiNumBonuses.add(iChange, eBonus);

		//FAssertMsg(m_paiNumBonuses[eBonus] >= 0, "m_paiNumBonuses[eBonus] is expected to be non-negative (invalid Index)"); XXX

		pPlotNode = headPlotsNode();

		while (pPlotNode != NULL)
		{
			pCity = GC.getMapINLINE().plotSorenINLINE(pPlotNode->m_data.iX, pPlotNode->m_data.iY)->getPlotCity();

			if (pCity != NULL)
			{
				if (pCity->getOwnerINLINE() == getOwnerINLINE())
				{
					pCity->changeNumBonuses(eBonus, iChange);
				}
			}

			pPlotNode = nextPlotsNode(pPlotNode);
		}
	}
}
#endif


void CvPlotGroup::insertAtEndPlots(XYCoords xy)
{
	m_plots.insertAtEnd(xy);
}


CLLNode<XYCoords>* CvPlotGroup::deletePlotsNode(CLLNode<XYCoords>* pNode)
{
	CLLNode<XYCoords>* pPlotNode;

	pPlotNode = m_plots.deleteNode(pNode);

	if (getLengthPlots() == 0)
	{
		GET_PLAYER(getOwnerINLINE()).deletePlotGroup(getID());
	}

  return pPlotNode;
}


CLLNode<XYCoords>* CvPlotGroup::nextPlotsNode(CLLNode<XYCoords>* pNode)
{
	return m_plots.next(pNode);
}


int CvPlotGroup::getLengthPlots()
{
	return m_plots.getLength();
}


CLLNode<XYCoords>* CvPlotGroup::headPlotsNode()
{
	return m_plots.head();
}


void CvPlotGroup::read(FDataStreamBase* pStream)
{
	// Init saved data
	reset();

	uint uiFlag=0;
	pStream->Read(&uiFlag);	// flags for expansion

	uint arrayBitmap = 0;
	pStream->Read(&arrayBitmap);

	pStream->Read(&m_iID);

	pStream->Read((int*)&m_eOwner);

	//m_aiNumBonuses.read(pStream, arrayBitmap);

	m_plots.Read(pStream);
}


void CvPlotGroup::write(FDataStreamBase* pStream)
{
	uint uiFlag=0;
	pStream->Write(uiFlag);		// flag for expansion

	uint arrayBitmap = 0;
	//arrayBitmap |= m_aiNumBonuses.hasContent()               ? 1 : 0;

	pStream->Write(arrayBitmap);

	pStream->Write(m_iID);

	pStream->Write(m_eOwner);

	//m_aiNumBonuses.write(pStream, arrayBitmap);

	m_plots.Write(pStream);
}
