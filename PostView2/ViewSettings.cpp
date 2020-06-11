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

#include "stdafx.h"
#include "ViewSettings.h"

void VIEWSETTINGS::Defaults()
{
	bgcol1 = GLColor(255,255,255);
	bgcol2 = GLColor(128,128,255);
	fgcol  = GLColor(0,0,0);
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
