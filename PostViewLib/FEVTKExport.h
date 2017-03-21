#pragma once
#include "FEBioPlotExport.h"

//-----------------------------------------------------------------------------
class FEModel;

//-----------------------------------------------------------------------------
struct VTKEXPORT
{
    bool	bshellthick;	// shell thickness
    bool	bscalar_data;   //user scalar data
};

class FEVTKExport : public FEBioPlotExport
{
public:
    FEVTKExport(void);
    ~FEVTKExport(void);
    
    bool Save(FEModel& fem, const char* szfile);
    void SetOptions(VTKEXPORT o) { m_ops = o; }
    
    void Space2_(char* szname);
    
protected:
    VTKEXPORT m_ops;
};
