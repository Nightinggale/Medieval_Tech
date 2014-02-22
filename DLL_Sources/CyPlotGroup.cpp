//
// Python wrapper class for CvPlotGroup
//
//
#include "CvGameCoreDLL.h"
#include "CyPlotGroup.h"
#include "CvPlotGroup.h"

CyPlotGroup::CyPlotGroup() : m_pPlotGroup(NULL)
{
}

CyPlotGroup::CyPlotGroup(CvPlotGroup* pPlotGroup) : m_pPlotGroup(pPlotGroup)
{
}

int CyPlotGroup::getLengthPlots() const
{
	return m_pPlotGroup ? m_pPlotGroup->getLengthPlots() : 0;
}

