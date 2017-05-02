#include "stdafx.h"
#include "ColorMap.h"
#ifdef WIN32
#include <Windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CColorMap::CColorMap()
{
	m_ncol = 0;
	jet(); // default color map
}

CColorMap::CColorMap(const CColorMap& m)
{
	m_ncol = m.m_ncol;
	for (int i=0; i<MAX_MAP_COLORS; ++i)
	{
		m_col[i] = m.m_col[i];
		m_pos[i] = m.m_pos[i];
	}
}

CColorMap::~CColorMap()
{

}

void CColorMap::operator = (const CColorMap& m)
{
	m_ncol = m.m_ncol;
	for (int i=0; i<MAX_MAP_COLORS; ++i)
	{
		m_col[i] = m.m_col[i];
		m_pos[i] = m.m_pos[i];
	}
}

GLCOLOR CColorMap::map(float fval) const
{
	int n = m_ncol - 1;
	if (fval <= m_pos[0]) return m_col[0];
	if (fval >= m_pos[n]) return m_col[n];

	int l = 0;
	while (m_pos[l] < fval) ++l;

	float dp = m_pos[l] - m_pos[l-1];
	if (dp != 0.f) dp = 1.f/dp; else dp = 1.f;

	GLCOLOR c1 = m_col[l];
	GLCOLOR c2 = m_col[l-1];

	GLCOLOR col;
	col.r = byte(((fval - m_pos[l-1])*c1.r + (m_pos[l]-fval)*c2.r) * dp);
	col.g = byte(((fval - m_pos[l-1])*c1.g + (m_pos[l]-fval)*c2.g) * dp);
	col.b = byte(((fval - m_pos[l-1])*c1.b + (m_pos[l]-fval)*c2.b) * dp);
	col.a = byte(((fval - m_pos[l-1])*c1.a + (m_pos[l]-fval)*c2.a) * dp);

	return col;
}

void CColorMap::gray()
{
	m_ncol = 5;

	m_pos[0] = 0.00f; m_col[0] = GLCOLOR(  0,  0,  0);
	m_pos[1] = 0.25f; m_col[1] = GLCOLOR( 64, 64, 64);
	m_pos[2] = 0.50f; m_col[2] = GLCOLOR(128,128,128);
	m_pos[3] = 0.75f; m_col[3] = GLCOLOR(192,192,192);
	m_pos[4] = 1.00f; m_col[4] = GLCOLOR(255,255,255);
}

void CColorMap::jet()
{
	m_ncol = 5;

	m_pos[0] = 0.00f; m_col[0] = GLCOLOR(0, 0, 255);
	m_pos[1] = 0.25f; m_col[1] = GLCOLOR(0, 255, 255);
	m_pos[2] = 0.50f; m_col[2] = GLCOLOR(0, 255, 0);
	m_pos[3] = 0.75f; m_col[3] = GLCOLOR(255, 255, 0);
	m_pos[4] = 1.00f; m_col[4] = GLCOLOR(255, 0, 0);
}

void CColorMap::red()
{
	m_ncol = 5;

	m_pos[0] = 0.00f; m_col[0] = GLCOLOR(0, 0, 0);
	m_pos[1] = 0.25f; m_col[1] = GLCOLOR(64, 0, 0);
	m_pos[2] = 0.50f; m_col[2] = GLCOLOR(128, 0, 0);
	m_pos[3] = 0.75f; m_col[3] = GLCOLOR(192, 0, 0);
	m_pos[4] = 1.00f; m_col[4] = GLCOLOR(255, 0, 0);
}

void CColorMap::green()
{
	m_ncol = 5;

	m_pos[0] = 0.00f; m_col[0] = GLCOLOR(0, 0, 0);
	m_pos[1] = 0.25f; m_col[1] = GLCOLOR(0, 64, 0);
	m_pos[2] = 0.50f; m_col[2] = GLCOLOR(0, 128, 0);
	m_pos[3] = 0.75f; m_col[3] = GLCOLOR(0, 192, 0);
	m_pos[4] = 1.00f; m_col[4] = GLCOLOR(0, 255, 0);
}

void CColorMap::blue()
{
	m_ncol = 5;

	m_pos[0] = 0.00f; m_col[0] = GLCOLOR(0, 0, 0);
	m_pos[1] = 0.25f; m_col[1] = GLCOLOR(0, 0, 64);
	m_pos[2] = 0.50f; m_col[2] = GLCOLOR(0, 0, 128);
	m_pos[3] = 0.75f; m_col[3] = GLCOLOR(0, 0, 192);
	m_pos[4] = 1.00f; m_col[4] = GLCOLOR(0, 0, 255);
}

void CColorMap::autumn()
{
	m_ncol = 5;

	m_pos[0] = 0.00f; m_col[0] = GLCOLOR(50, 50, 0);
	m_pos[1] = 0.25f; m_col[1] = GLCOLOR(100, 50, 0);
	m_pos[2] = 0.50f; m_col[2] = GLCOLOR(150, 75, 0);
	m_pos[3] = 0.75f; m_col[3] = GLCOLOR(200, 150, 0);
	m_pos[4] = 1.00f; m_col[4] = GLCOLOR(255, 200, 0);
}

void CColorMap::winter()
{
	m_ncol = 5;

	m_pos[0] = 0.00f; m_col[0] = GLCOLOR(255, 0, 255);
	m_pos[1] = 0.25f; m_col[1] = GLCOLOR(128, 0, 255);
	m_pos[2] = 0.50f; m_col[2] = GLCOLOR(0, 0, 255);
	m_pos[3] = 0.75f; m_col[3] = GLCOLOR(0, 255, 255);
	m_pos[4] = 1.00f; m_col[4] = GLCOLOR(255, 255, 255);
}

void CColorMap::spring()
{
	m_ncol = 5;

	m_pos[0] = 0.00f; m_col[0] = GLCOLOR(0, 255, 0);
	m_pos[1] = 0.25f; m_col[1] = GLCOLOR(128, 255, 0);
	m_pos[2] = 0.50f; m_col[2] = GLCOLOR(255, 255, 0);
	m_pos[3] = 0.75f; m_col[3] = GLCOLOR(255, 255, 128);
	m_pos[4] = 1.00f; m_col[4] = GLCOLOR(255, 255, 255);
}

void CColorMap::summer()
{
	m_ncol = 5;

	m_pos[0] = 0.00f; m_col[0] = GLCOLOR(255, 128, 0);
	m_pos[1] = 0.25f; m_col[1] = GLCOLOR(255, 128, 0);
	m_pos[2] = 0.50f; m_col[2] = GLCOLOR(255, 255, 0);
	m_pos[3] = 0.75f; m_col[3] = GLCOLOR(255, 255, 128);
	m_pos[4] = 1.00f; m_col[4] = GLCOLOR(255, 255, 255);
}

void CColorMap::rbb()
{
	m_ncol = 5;

	m_pos[0] = 0.00f; m_col[0] = GLCOLOR(128, 128, 255);
	m_pos[1] = 0.25f; m_col[1] = GLCOLOR(0, 0, 255);
	m_pos[2] = 0.50f; m_col[2] = GLCOLOR(0, 0, 0);
	m_pos[3] = 0.75f; m_col[3] = GLCOLOR(255, 0, 0);
	m_pos[4] = 1.00f; m_col[4] = GLCOLOR(255, 128, 128); // salmon
}

void CColorMap::fire()
{
	m_ncol = 5;

	m_pos[0] = 0.00f; m_col[0] = GLCOLOR(0, 0, 0); // black
	m_pos[1] = 0.25f; m_col[1] = GLCOLOR(128, 0, 255); // purple
	m_pos[2] = 0.50f; m_col[2] = GLCOLOR(255, 0, 0); // red
	m_pos[3] = 0.75f; m_col[3] = GLCOLOR(255, 255, 0); // yellow
	m_pos[4] = 1.00f; m_col[4] = GLCOLOR(255, 255, 255); // white
}

//-----------------------------------------------------------------------------
// Invert the colormap
void CColorMap::Invert()
{
	for (int i=0; i<m_ncol/2; ++i)
	{
		GLCOLOR c = m_col[i];
		m_col[i] = m_col[m_ncol-i-1];
		m_col[m_ncol-i-1] = c;
	}
}

//=============================================================================
CColorTexture::CColorTexture()
{
	m_colorMap = 0;
	m_ndivs = 10;
	m_bsmooth = true;

	UpdateTexture();
}

CColorTexture::CColorTexture(const CColorTexture& col)
{
	m_colorMap = col.m_colorMap;
	m_ndivs = col.m_ndivs;
	m_bsmooth = col.m_bsmooth;

	m_tex = col.m_tex;
}

void CColorTexture::operator = (const CColorTexture& col)
{
	m_colorMap = col.m_colorMap;
	m_ndivs = col.m_ndivs;
	m_bsmooth = col.m_bsmooth;

	m_tex = col.m_tex;
}

void CColorTexture::UpdateTexture()
{
	int n = m_tex.Size();
	GLubyte* pb = m_tex.GetBytes();

	int N = (m_bsmooth ? n : m_ndivs);
	if (N < 2) N = 2;

	CColorMap& map = ColorMapManager::GetColorMap(m_colorMap);

	GLCOLOR c;
	for (int i = 0; i<n; i++, pb += 3)
	{
		float f = (float)(i*N / n);

		c = map.map(f / (N - 1));

		pb[0] = c.r;
		pb[1] = c.g;
		pb[2] = c.b;
	}
}

int CColorTexture::GetDivisions() const
{
	return m_ndivs;
}

void CColorTexture::SetDivisions(int n)
{
	if (n != m_ndivs)
	{
		m_ndivs = n;
		UpdateTexture();
	}
}

bool CColorTexture::GetSmooth() const
{
	return m_bsmooth;
}

void CColorTexture::SetSmooth(bool b)
{
	if (b != m_bsmooth)
	{
		m_bsmooth = b;
		UpdateTexture();
	}
}

void CColorTexture::SetColorMap(int n)
{
	if (n != m_colorMap)
	{
		m_colorMap = n;
		UpdateTexture();
	}
}

int CColorTexture::GetColorMap() const
{
	return m_colorMap;
}


//=============================================================================
class ColorMapTemplate
{
public:
	ColorMapTemplate(){}
	ColorMapTemplate(const string& name, const CColorMap& map) : m_name(name), m_map(map) {}
	ColorMapTemplate(const ColorMapTemplate& cmt)
	{
		m_name = cmt.m_name;
		m_map  = cmt.m_map;
	}
	void operator = (const ColorMapTemplate& cmt)
	{
		m_name = cmt.m_name;
		m_map  = cmt.m_map;
	}

	const string& name() const { return m_name; }

	CColorMap& colormap() { return m_map; }

private: 
	string		m_name;
	CColorMap	m_map;
};

//=============================================================================
vector<class ColorMapTemplate>	ColorMapManager::m_map;

int ColorMapManager::ColorMaps()
{
	return (int) m_map.size();
}

void ColorMapManager::Initialize()
{
	if (m_map.empty() == false) return;

	CColorMap map;
	map.autumn(); AddColormap("Autumn", map);
	map.blue  (); AddColormap("Blue"  , map);
	map.fire  (); AddColormap("Fire"  , map);
	map.gray  (); AddColormap("Gray"  , map);
	map.green (); AddColormap("Green" , map);
	map.jet   (); AddColormap("Jet"   , map);
	map.rbb   (); AddColormap("RBB"   , map);
	map.red   (); AddColormap("Red"   , map);
	map.spring(); AddColormap("Spring", map);
	map.summer(); AddColormap("Summer", map);
	map.winter(); AddColormap("Winter", map);
}

string ColorMapManager::GetColorMapName(int n)
{
	if ((n>=0) && (n < ColorMaps()))
	{
		return m_map[n].name();
	}
	else 
	{
		assert(false);	
		return "";
	}
}

CColorMap& ColorMapManager::GetColorMap(int n)
{
	return m_map[n].colormap();
}

void ColorMapManager::AddColormap(const string& name, const CColorMap& map)
{
	ColorMapTemplate newTemplate(name, map);
	m_map.push_back(newTemplate);	
}
