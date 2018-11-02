#pragma once
#include "FEBioPlotExport.h"

//-----------------------------------------------------------------------------
class FEModel;

//-----------------------------------------------------------------------------
class FEVTKExport : public FEBioPlotExport
{
public:
    FEVTKExport(void);
    ~FEVTKExport(void);
    
    bool Save(FEModel& fem, const char* szfile);

	void ExportAllStates(bool b);

private:
	bool WriteState(const char* szname, FEModel& fem, int nstate);
    
protected:
	bool	m_bshellthick;		// shell thickness
	bool	m_bscalar_data;		// user scalar data
	bool	m_bwriteAllStates;	// write all states
};
