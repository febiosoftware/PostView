/*This file is part of the PostView source code and is licensed under the MIT license
listed below.

See Copyright-PostView.txt for details.

Copyright (c) 2020 University of Utah, The Trustees of Columbia University in 
the City of New York, and others.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#pragma once
#include <FSCore/color.h>

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
