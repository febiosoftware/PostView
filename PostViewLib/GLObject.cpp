#include "stdafx.h"
#include "GLObject.h"
#include <string.h>

CGLObject::CGLObject()
{
	m_szname[0] = 0;
	m_bactive = true;
}

CGLObject::~CGLObject(void)
{

}

void CGLObject::SetName(const char* szname) { strcpy(m_szname, szname); }
