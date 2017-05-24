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
#include <vector>
#include <string>
using namespace std;

#define MAX_MAP_COLORS	9

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

	GLCOLOR map(float fval) const;

	int Colors() const { return m_ncol; }
	void SetColors(int n) { m_ncol = n; }

	GLCOLOR GetColor(int i) const { return m_col[i]; }
	void SetColor(int i, GLCOLOR c) { m_col[i] = c; }

	float GetColorPos(int i) const { return m_pos[i]; }
	void SetColorPos(int i, float v) { m_pos[i] = v; }

	void Invert();

protected:
	int		m_ncol;
	GLCOLOR	m_col[MAX_MAP_COLORS];
	float	m_pos[MAX_MAP_COLORS];
};

//-----------------------------------------------------------------------------
class CColorTexture
{
public:
	CColorTexture();
	CColorTexture(const CColorTexture& col);
	void operator = (const CColorTexture& col);

	GLTexture1D& GetTexture() { return m_tex; }

	void UpdateTexture();

	int GetDivisions() const;
	void SetDivisions(int n);

	bool GetSmooth() const;
	void SetSmooth(bool b);

	void SetColorMap(int n);
	int GetColorMap() const;

private:
	int		m_colorMap;		// index of template to use
	int		m_ndivs;		// number of divisions
	bool	m_bsmooth;		// smooth interpolation or not

	GLTexture1D m_tex;	// the actual texture
};

//-----------------------------------------------------------------------------
// Class for managing available color maps
class ColorMapManager
{
public:
	enum DefaultMap
	{
		AUTUMN,
		BLUE,
		FIRE,
		GRAY,
		GREEN,
		JET,
		RBB,
		RED,
		SPRING,
		SUMMER,
		WINTER,
		USER
	};

public:
	// calls this to generate a list of default maps
	static void Initialize();

	// get the number of templates available
	static int ColorMaps();

	// return the name of a template
	static string GetColorMapName(int n);

	// add a color map template
	static void AddColormap(const string& name, const CColorMap& map);

	// get a reference to the color map template
	static CColorMap& GetColorMap(int n);

private:
	// this is a singleton so don't try to instantiate this
	ColorMapManager(){}
	ColorMapManager(const ColorMapManager&){}

private:
	static vector<class ColorMapTemplate>	m_map;
};

#endif // !defined(AFX_COLORMAP_H__5CE5C222_17D8_4BD3_8CDB_D4FF17C64525__INCLUDED_)
