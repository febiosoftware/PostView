#include "stdafx.h"
#include "ColorMap.h"
#ifdef WIN32
#include <Windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gL.h>
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CColorMap::CColorMap()
{
	m_min = 0.f;
	m_max = 255.f;
	m_ndivs = 10;
	m_bsmooth = true;
	m_ncol = 0;

	jet(); // default color map
}

CColorMap::CColorMap(const CColorMap& m)
{
	m_min = m.m_min;
	m_max = m.m_max;
	m_ndivs = m.m_ndivs;
	m_bsmooth = m.m_bsmooth;
	m_ncol = m.m_ncol;
	m_ntype = m.m_ntype;
	for (int i=0; i<MAX_MAP_COLORS; ++i)
	{
		m_col[i] = m.m_col[i];
		m_pos[i] = m.m_pos[i];
	}
	UpdateTexture();
}

CColorMap::~CColorMap()
{

}

void CColorMap::operator = (const CColorMap& m)
{
	m_min = m.m_min;
	m_max = m.m_max;
	m_ndivs = m.m_ndivs;
	m_bsmooth = m.m_bsmooth;
	m_ncol = m.m_ncol;
	m_ntype = m.m_ntype;
	for (int i=0; i<MAX_MAP_COLORS; ++i)
	{
		m_col[i] = m.m_col[i];
		m_pos[i] = m.m_pos[i];
	}
	UpdateTexture();
}

GLCOLOR CColorMap::map(float fval)
{
	float h = m_max - m_min;
	if (h == 0.f) h = 1.f; else h = 1.f/h;
	float val = 255.f*(fval - m_min)*h;

	int n = m_ncol - 1;

	if (val <= m_pos[0]) return m_col[0];
	if (val >= m_pos[n]) return m_col[n];

	int l = 0;
	while (m_pos[l] < val) ++l;

	float dp = m_pos[l] - m_pos[l-1];
	if (dp != 0.f) dp = 1.f/dp; else dp = 1.f;

	GLCOLOR c1 = m_col[l];
	GLCOLOR c2 = m_col[l-1];

	GLCOLOR col;
	col.r = byte(((val - m_pos[l-1])*c1.r + (m_pos[l]-val)*c2.r) * dp);
	col.g = byte(((val - m_pos[l-1])*c1.g + (m_pos[l]-val)*c2.g) * dp);
	col.b = byte(((val - m_pos[l-1])*c1.b + (m_pos[l]-val)*c2.b) * dp);
	col.a = byte(((val - m_pos[l-1])*c1.a + (m_pos[l]-val)*c2.a) * dp);

	return col;
}

void CColorMap::SetType(int ntype)
{
	switch (ntype)
	{
	case COLOR_MAP_AUTUMN : autumn (); break;
	case COLOR_MAP_BLUE   : blue   (); break;
	case COLOR_MAP_FIRE   : fire   (); break;
	case COLOR_MAP_GRAY   : gray   (); break;
	case COLOR_MAP_GREEN  : green  (); break;
	case COLOR_MAP_JET    : jet    (); break;
	case COLOR_MAP_RBB    : rbb    (); break;
	case COLOR_MAP_RED    : red    (); break;
	case COLOR_MAP_SPRING : spring (); break;
	case COLOR_MAP_SUMMER : summer (); break;
	case COLOR_MAP_WINTER : winter (); break;
	case COLOR_MAP_USER   : m_ntype = ntype;
	}
}

void CColorMap::gray()
{
	m_ntype = COLOR_MAP_GRAY;
	m_ncol = 5;

	m_pos[0] =   0; m_col[0] = GLCOLOR(  0,  0,  0);
	m_pos[1] =  64; m_col[1] = GLCOLOR( 64, 64, 64);
	m_pos[2] = 128; m_col[2] = GLCOLOR(128,128,128);
	m_pos[3] = 192; m_col[3] = GLCOLOR(192,192,192);
	m_pos[4] = 255; m_col[4] = GLCOLOR(255,255,255);

	UpdateTexture();
}

void CColorMap::jet()
{
	m_ntype = COLOR_MAP_JET;
	m_ncol = 5;

	m_pos[0] =   0; m_col[0] = GLCOLOR(  0,  0,255);
	m_pos[1] =  80; m_col[1] = GLCOLOR(  0,255,255);
	m_pos[2] = 128; m_col[2] = GLCOLOR(  0,255,  0);
	m_pos[3] = 176; m_col[3] = GLCOLOR(255,255,  0);
	m_pos[4] = 255; m_col[4] = GLCOLOR(255,  0,  0);

	UpdateTexture();
}

void CColorMap::red()
{
	m_ntype = COLOR_MAP_RED;
	m_ncol = 5;

	m_pos[0] =   0; m_col[0] = GLCOLOR(  0,  0,  0);
	m_pos[1] =  64; m_col[1] = GLCOLOR( 64,  0,  0);
	m_pos[2] = 128; m_col[2] = GLCOLOR(128,  0,  0);
	m_pos[3] = 192; m_col[3] = GLCOLOR(192,  0,  0);
	m_pos[4] = 255; m_col[4] = GLCOLOR(255,  0,  0);

	UpdateTexture();
}

void CColorMap::green()
{
	m_ntype = COLOR_MAP_GREEN;
	m_ncol = 5;

	m_pos[0] =   0; m_col[0] = GLCOLOR(  0,  0,  0);
	m_pos[1] =  64; m_col[1] = GLCOLOR(  0, 64,  0);
	m_pos[2] = 128; m_col[2] = GLCOLOR(  0,128,  0);
	m_pos[3] = 192; m_col[3] = GLCOLOR(  0,192,  0);
	m_pos[4] = 255; m_col[4] = GLCOLOR(  0,255,  0);

	UpdateTexture();
}

void CColorMap::blue()
{
	m_ntype = COLOR_MAP_BLUE;
	m_ncol = 5;

	m_pos[0] =   0; m_col[0] = GLCOLOR(  0,  0,  0);
	m_pos[1] =  64; m_col[1] = GLCOLOR(  0,  0, 64);
	m_pos[2] = 128; m_col[2] = GLCOLOR(  0,  0,128);
	m_pos[3] = 192; m_col[3] = GLCOLOR(  0,  0,192);
	m_pos[4] = 255; m_col[4] = GLCOLOR(  0,  0,255);

	UpdateTexture();
}

void CColorMap::autumn()
{
	m_ntype = COLOR_MAP_AUTUMN;
	m_ncol = 5;

	m_pos[0] =   0; m_col[0] = GLCOLOR( 50, 50,  0);
	m_pos[1] =  64; m_col[1] = GLCOLOR(100, 50,  0);
	m_pos[2] = 128; m_col[2] = GLCOLOR(150, 75,  0);
	m_pos[3] = 192; m_col[3] = GLCOLOR(200,150,  0);
	m_pos[4] = 255; m_col[4] = GLCOLOR(255,200,  0);

	UpdateTexture();
}

void CColorMap::winter()
{
	m_ntype = COLOR_MAP_WINTER;
	m_ncol = 5;

	m_pos[0] =   0; m_col[0] = GLCOLOR(255,  0,255);
	m_pos[1] =  64; m_col[1] = GLCOLOR(128,  0,255);
	m_pos[2] = 128; m_col[2] = GLCOLOR(  0,  0,255);
	m_pos[3] = 192; m_col[3] = GLCOLOR(  0,255,255);
	m_pos[4] = 255; m_col[4] = GLCOLOR(255,255,255);

	UpdateTexture();
}

void CColorMap::spring()
{
	m_ntype = COLOR_MAP_SPRING;
	m_ncol = 5;

	m_pos[0] =   0; m_col[0] = GLCOLOR(  0,255,  0);
	m_pos[1] =  64; m_col[1] = GLCOLOR(128,255,  0);
	m_pos[2] = 128; m_col[2] = GLCOLOR(255,255,  0);
	m_pos[3] = 192; m_col[3] = GLCOLOR(255,255,128);
	m_pos[4] = 255; m_col[4] = GLCOLOR(255,255,255);

	UpdateTexture();
}

void CColorMap::summer()
{
	m_ntype = COLOR_MAP_SUMMER;
	m_ncol = 5;

	m_pos[0] =   0; m_col[0] = GLCOLOR(255,128,  0);
	m_pos[1] =  64; m_col[1] = GLCOLOR(255,128,  0);
	m_pos[2] = 128; m_col[2] = GLCOLOR(255,255,  0);
	m_pos[3] = 192; m_col[3] = GLCOLOR(255,255,128);
	m_pos[4] = 255; m_col[4] = GLCOLOR(255,255,255);

	UpdateTexture();
}

void CColorMap::rbb()
{
	m_ntype = COLOR_MAP_RBB;
	m_ncol = 5;

	m_pos[0] =   0; m_col[0] = GLCOLOR(128,128,255);
	m_pos[1] =  64; m_col[1] = GLCOLOR(  0,  0,255);
	m_pos[2] = 128; m_col[2] = GLCOLOR(  0,  0,  0);
	m_pos[3] = 192; m_col[3] = GLCOLOR(255,  0,  0);
	m_pos[4] = 255; m_col[4] = GLCOLOR(255,128,128); // salmon

	UpdateTexture();
}

void CColorMap::fire()
{
	m_ntype = COLOR_MAP_FIRE;
	m_ncol = 5;

	m_pos[0] =   0; m_col[0] = GLCOLOR(  0,  0,  0); // black
	m_pos[1] =  64; m_col[1] = GLCOLOR(128,  0,255); // purple
	m_pos[2] = 128; m_col[2] = GLCOLOR(255,  0,  0); // red
	m_pos[3] = 192; m_col[3] = GLCOLOR(255,255,  0); // yellow
	m_pos[4] = 255; m_col[4] = GLCOLOR(255,255,255); // white

	UpdateTexture();
}

void CColorMap::UpdateTexture()
{
	int n = m_tex.Size();
	GLubyte* pb = m_tex.GetBytes();

	int N = (m_bsmooth ? n : m_ndivs);
	if (N < 2) N = 2;

	int l;
	GLCOLOR c;
	for (int i=0; i<n; i++, pb+=3)
	{
		l = i*N/n;

		c = map(m_min + l*(m_max-m_min)/(N-1));

		pb[0] = c.r;
		pb[1] = c.g;
		pb[2] = c.b;
	}
}

//-----------------------------------------------------------------------------
// Invert the colormap
void CColorMap::Invert()
{
	int i;
	GLCOLOR cn[MAX_MAP_COLORS];
	float fn[MAX_MAP_COLORS];
	for (i=0; i<m_ncol; ++i)
	{
		cn[i] = m_col[m_ncol-i-1];
		fn[i] = 255.f - m_pos[m_ncol-i-1];
	}
	for (i=0; i<m_ncol; ++i)
	{
		m_col[i] = cn[i];
		m_pos[i] = fn[i];
	}
}
