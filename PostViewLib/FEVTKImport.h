#pragma once
#include "FEFileReader.h"

#include <vector>
using namespace std;

class FEState;

class FEVTKimport :	public FEFileReader
{

public:
	FEVTKimport();
	~FEVTKimport(void);

	bool Load(FEModel& fem, const char* szfile);

protected:
	bool readPointData(char* ch);
	
protected:
	bool BuildMesh();
	FEModel*	m_pfem;
	FEState*	m_ps;
};
