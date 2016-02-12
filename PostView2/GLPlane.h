#pragma once

#include "PostViewLib/GLObject.h"
#include "PostViewLib/FEModel.h"

//-----------------------------------------------------------------------------
class CGLPlane : public CGLVisual
{
public:
	CGLPlane(FEModel* pfem);
	~CGLPlane(void);

	void Render(CGLContext& rc);

	void Create(int n[3]);

	vec3f Normal() { return m_e[2]; }

protected:
	FEModel*	m_pfem;	// pointer to mesh

	vec3f		m_rc;
	vec3f		m_e[3];
};
