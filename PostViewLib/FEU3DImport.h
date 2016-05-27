#pragma once
#include "FEFileReader.h"
using namespace std;

class FEU3DImport : public FEFileReader
{
public:
	FEU3DImport();

	bool Load(FEModel& fem, const char* szfile);
};
