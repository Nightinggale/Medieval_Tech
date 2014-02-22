#include "CvGameCoreDLL.h"

#include "CyPlotGroup.h"

//
// python interface for CyPlotGroup
//
void CyPlotGroupPythonInterface1(python::class_<CyPlotGroup>& x)
{
	OutputDebugString("Python Extension Module - CyPlotGroupPythonInterface1\n");
	x
		.def("getLengthPlots", &CyPlotGroup::getLengthPlots, "int ()")
		
	;
}
