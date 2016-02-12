// GLPlot.h: interface for the CGLPlot class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLPLOT_H__2FD9290B_968B_4C2C_AE36_933F5B0AE3D1__INCLUDED_)
#define AFX_GLPLOT_H__2FD9290B_968B_4C2C_AE36_933F5B0AE3D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PostViewLib/FEModel.h"
#include "GLMesh.h"
#include "PostViewLib/ColorMap.h"
#include "PostViewLib/GLTexture1D.h"
#include "GLModel.h"
#include "PostViewLib/GLObject.h"
#include "PostViewLib/DataMap.h"

class CGLPlot : public CGLVisual
{
protected:
	struct SUBELEMENT
	{
		float   vf[8];		// vector values
		float    h[8][8];	// shapefunctions
	};

public:
	CGLPlot(CGLModel* po = 0);
	virtual ~CGLPlot();

	CGLModel* GetModel() { return m_pObj; }
	void SetModel(CGLModel* pm) { m_pObj = pm; }

protected:
	CGLModel*	m_pObj;
};

#endif // !defined(AFX_GLPLOT_H__2FD9290B_968B_4C2C_AE36_933F5B0AE3D1__INCLUDED_)
