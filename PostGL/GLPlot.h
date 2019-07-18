#pragma once
#include <PostViewLib/FEModel.h>
#include "GLMesh.h"
#include <PostViewLib/ColorMap.h>
#include <PostViewLib/GLTexture1D.h>
#include "GLModel.h"
#include <PostViewLib/GLObject.h>
#include <PostViewLib/DataMap.h>

class CPropertyList;

namespace Post {

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

	virtual CPropertyList* propertyList() { return 0; }

	virtual void UpdateTexture() {}
};
}
