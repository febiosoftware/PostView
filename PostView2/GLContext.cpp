#include "stdafx.h"
#include "GLContext.h"
#include "GLView.h"

CGLContext::CGLContext(CGLView* pv)
{
	m_pview = pv;
}

CGLContext::~CGLContext(void)
{
}
