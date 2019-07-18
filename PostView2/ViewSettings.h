#pragma once
#include <PostViewLib/color.h>

// projection modes
#define RENDER_ORTHO	0
#define RENDER_PERSP	1

// Background styles
#define BG_COLOR_1		0
#define BG_COLOR_2		1
#define BG_FADE_HORZ	2
#define BG_FADE_VERT	3
#define BG_FADE_DIAG	4

//-----------------------------------------------------------------------------
// View Settings
struct VIEWSETTINGS
{
	GLColor	bgcol1;
	GLColor	bgcol2;
	GLColor	fgcol;

	int		bgstyle;

	bool	m_bShadows;
	float	m_shadow_intensity;
	float	m_ambient;		// scene light ambient intensity
	float	m_diffuse;		// scene light diffuse inentisty

	bool	m_bTriad;		// show triad or not
	bool	m_bTitle;		// show title or not
	bool	m_bconn;		// select connected
	bool	m_bext;			// ignore interior flag

	bool	m_bmesh;		// show mesh or not
	bool	m_boutline;		// show outline or not
	bool	m_bBox;			// show bounding box or not
	int		m_nproj;		// projection mode or not
	bool	m_bLighting;	// use lighting or not
    int     m_nconv;        // multiview projection convention

	// selections
	bool	m_bTags;						// show tags or not
	int		m_ntagInfo;						// tag info level
	bool	m_bignoreBackfacingItems;		// ignore backfacing items or not
	float	m_angleTol;						// angle tolerance for connected face selection

	// OpenGL settings
	bool	m_blinesmooth;	// line smoothing
	float	m_flinethick;	// line thickness
	float	m_fpointsize;	// point size
	float	m_fspringthick;	// line thickness for springs

	void Defaults();
};
