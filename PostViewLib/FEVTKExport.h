#pragma once

//-----------------------------------------------------------------------------
struct VTKEXPORT
{
	bool	bshellthick;	// shell thickness
	bool	bscalar_data;   //user scalar data
};

class FEModel;

class FEVTKExport
{
public:
	FEVTKExport(void);
	~FEVTKExport(void);

	bool Save(FEModel& fem, int ntime, const char* szfile);
	void SetOptions(VTKEXPORT o) { m_ops = o; }

protected:
	VTKEXPORT m_ops;
};
