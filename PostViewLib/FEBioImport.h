#pragma once
#include "FEFileReader.h"
#include "XMLReader.h"

class FEModel;
class FEMesh;

class FEBioImport :	public FEFileReader
{
public:
	FEBioImport();
	~FEBioImport();

	bool Load(FEModel& fem, const char* szfile);

protected:
	void ParseMaterialSection(FEModel& fem, XMLTag& tag);
	void ParseGeometrySection(FEModel& fem, XMLTag& tag);
	void ParseGeometrySection2(FEModel& fem, XMLTag& tag);
	bool ParseVersion(XMLTag& tag);

protected:
	FEModel*	m_pfem;
	FEMesh*		m_pm;

	int		m_nmat;
	int		m_nversion;

	XMLReader	m_xml;
};
