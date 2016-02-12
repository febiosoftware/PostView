// ColorMap.h: interface for the CColorMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLORMAP_H__5CE5C222_17D8_4BD3_8CDB_D4FF17C64525__INCLUDED_)
#define AFX_COLORMAP_H__5CE5C222_17D8_4BD3_8CDB_D4FF17C64525__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GLTexture1D.h"
#include "color.h"

#define MAX_MAP_COLORS	9

#define COLOR_MAP_AUTUMN	0
#define COLOR_MAP_BLUE		1
#define COLOR_MAP_FIRE		2
#define COLOR_MAP_GRAY		3
#define COLOR_MAP_GREEN		4
#define COLOR_MAP_JET		5
#define COLOR_MAP_RBB		6
#define COLOR_MAP_RED 		7
#define COLOR_MAP_SPRING	8
#define COLOR_MAP_SUMMER	9
#define COLOR_MAP_WINTER	10
#define COLOR_MAP_USER		11

class CGLTexture1D;

class CColorMap  
{
public:
	CColorMap();
	CColorMap(const CColorMap& map);
	virtual ~CColorMap();

	void operator = (const CColorMap& map);

	void jet   ();
	void gray  ();
	void autumn();
	void winter();
	void spring();
	void summer();
	void red   ();
	void green ();
	void blue  ();
	void rbb   ();
	void fire  ();

	int Type() { return m_ntype; }
	void SetType(int ntype);

	GLCOLOR map(float fval);

	void SetRange(float min, float max, bool bup = true) { m_min = min; m_max = max; if (bup) UpdateTexture(); }
	void GetRange(float& min, float& max) { min = m_min; max = m_max; }

	GLTexture1D& GetTexture() { return m_tex; }

	int GetDivisions() { return m_ndivs; }
	void SetDivisions(int n, bool bup = true) { m_ndivs = n; if (bup) UpdateTexture(); }

	bool Smooth() { return m_bsmooth; }
	void Smooth(bool b) { m_bsmooth = b; }

	void UpdateTexture();

	int Colors() { return m_ncol; }
	void SetColors(int n) { SetType(COLOR_MAP_USER); m_ncol = n; }

	GLCOLOR GetColor(int i) { return m_col[i]; }
	void SetColor(int i, GLCOLOR c) { SetType(COLOR_MAP_USER); m_col[i] = c; }

	float GetColorPos(int i) { return m_pos[i]; }
	void SetColorPos(int i, float v) { SetType(COLOR_MAP_USER); m_pos[i] = v; }

	void Invert();

protected:
	int		m_ncol;
	GLCOLOR	m_col[MAX_MAP_COLORS];
	float	m_pos[MAX_MAP_COLORS];

	int		m_ntype;
	int		m_ndivs;
	bool	m_bsmooth;	// smooth gradient or not

	float	m_min;
	float	m_max;

	GLTexture1D m_tex;
};

#endif // !defined(AFX_COLORMAP_H__5CE5C222_17D8_4BD3_8CDB_D4FF17C64525__INCLUDED_)
