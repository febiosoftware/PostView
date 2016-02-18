#pragma once
#include "FEFileReader.h"

#include <vector>
using namespace std;

class FEVTKimport :	public FEFileReader
{

public:
	FEVTKimport();
	~FEVTKimport(void);

	bool Load(FEModel& fem, const char* szfile);
	
protected:
	bool BuildMesh();
	FEModel*	m_pfem;
};
