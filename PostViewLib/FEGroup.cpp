#include "stdafx.h"
#include "FEGroup.h"
#include "FEMesh.h"
#include <string.h>

//-----------------------------------------------------------------------------
void FEGroup::SetName(const char* szname)
{
	strcpy(m_szname, szname);
}

//-----------------------------------------------------------------------------
const char* FEGroup::GetName()
{
	return m_szname;
}

//-----------------------------------------------------------------------------
// FEDomain constructor
FEDomain::FEDomain(FEMesh *pm, int nmat)
{
	m_pm = pm;
	m_nmat = nmat;

	int i, n;

	// count faces
	int N = pm->Faces();
	for (i=0, n = 0; i<N; ++i) if (pm->Face(i).m_mat == nmat) n++;

	m_Face.resize(n);
	for (i=0, n = 0; i<N; ++i) if (pm->Face(i).m_mat == nmat) m_Face[n++] = i;

	// count elements
	N = pm->Elements();
	for (i=0, n = 0; i<N; ++i) if (pm->Element(i).m_MatID == nmat) ++n;

	m_Elem.resize(n);
	for (i=0, n = 0; i<N; ++i) if (pm->Element(i).m_MatID == nmat) m_Elem[n++] = i;
}

//-----------------------------------------------------------------------------
FEFace& FEDomain::Face(int n)
{ 
	return m_pm->Face(m_Face[n]); 
}

//-----------------------------------------------------------------------------
FEElement& FEDomain::Element(int n)
{
	return m_pm->Element(m_Elem[n]); 
}
