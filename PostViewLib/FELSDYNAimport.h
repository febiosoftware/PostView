// FELSDYNAimport.h: interface for the FELSDYNAimport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FELSDYNAIMPORT_H__50F241DD_FFA6_42F8_9BD0_7ED2DFB2C68F__INCLUDED_)
#define AFX_FELSDYNAIMPORT_H__50F241DD_FFA6_42F8_9BD0_7ED2DFB2C68F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FEFileReader.h"

#include <list>
using namespace std;

class FEMeshBase;

class FELSDYNAimport : public FEFileReader  
{
protected:
	struct ELEMENT_SHELL
	{
		int		id;
		int		mid;
		int		n[4];
		double	h[4];

		ELEMENT_SHELL() { h[0] = h[1] = h[2] = h[3] = 0; }
	};

	struct ELEMENT_SOLID
	{
		int id;
		int	mid;
		int n[8];
	};

	struct NODE
	{
		int id;
		int n;
		double	x, y, z;
		double v;
	};

public:
	FELSDYNAimport();
	virtual ~FELSDYNAimport();

	bool Load(FEModel& fem, const char* szfile);

	int FindNode(int id, list<NODE>::iterator& pn);

	void read_displacements(bool b) { m_bdispl = b; }

protected:
	char* get_line(char* szline);

	bool Read_Element_Solid();
	bool Read_Element_Shell();
	bool Read_Element_Shell_Thickness();
	bool Read_Node();
	bool Read_Nodal_Results();

	void BuildMaterials(FEModel& fem);
	bool BuildMesh(FEModel& fem);

protected:
	list<ELEMENT_SOLID>		m_solid;
	list<ELEMENT_SHELL>		m_shell;
	list<NODE>				m_node;

	FEMeshBase*			m_pm;

	bool	m_bnresults;	// nodal results included?
	bool	m_bshellthick;	// shell thicknesses included?
	bool	m_bdispl;		// define displacement field?

	char			m_szline[256];
};

#endif // !defined(AFX_FELSDYNAIMPORT_H__50F241DD_FFA6_42F8_9BD0_7ED2DFB2C68F__INCLUDED_)
