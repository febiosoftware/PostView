#include "stdafx.h"
#include "ViewSettings.h"

void VIEWSETTINGS::Defaults()
{
	bgcol1 = GLCOLOR(255,255,255);
	bgcol2 = GLCOLOR(128,128,255);
	fgcol  = GLCOLOR(0,0,0);
	bgstyle = BG_FADE_VERT;
	m_shadow_intensity = 0.5f;
	m_bmesh		 = false;
	m_boutline   = true;
	m_bShadows    = false;
	m_bTriad      = true;
	m_bTags       = true;
	m_ntagInfo    = 0;
	m_bTitle      = true;
	m_bconn		 = false;
	m_bext       = true;
	m_bBox        = false;
	m_nproj       = RENDER_PERSP;
    m_nconv       = 0;
	m_bLighting   = true;
	m_ambient     = 0.3f;
	m_diffuse     = 0.7f;
	m_bignoreBackfacingItems = true;
	m_blinesmooth  = true;
	m_flinethick   = 1.0f;
	m_fspringthick = 1.0f;
	m_fpointsize   = 6.0f;
	m_angleTol     = 90.f;
}
