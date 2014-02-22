#pragma once
#ifndef CyPlotGroup_h
#define CyPlotGroup_h
//
// Python wrapper class for CvPlotGroup
//

class CvPlotGroup;

class CyPlotGroup
{
public:
	CyPlotGroup();	
	CyPlotGroup(CvPlotGroup*);			// Call from C++

	int getLengthPlots() const;

private:
	CvPlotGroup* m_pPlotGroup;
};
#endif	// CyPlotGroup_h

