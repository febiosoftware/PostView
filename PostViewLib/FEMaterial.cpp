#include "stdafx.h"
#include "FEMaterial.h"

FEMaterial::FEMaterial()
{ 
	m_szname[0] = 0; 
	bclip = true; 
	m_nrender = RENDER_MODE_DEFAULT; 
	m_ntransmode = RENDER_TRANS_CONSTANT; 

	benable = true;
	bvisible = true;
	bmesh = true;
	bcast_shadows = true;
}

const char* FEMaterial::GetName() { return m_szname; }
void FEMaterial::SetName(const char* szname) { strcpy(m_szname, szname); }
