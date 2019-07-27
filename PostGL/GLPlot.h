#pragma once
#include "PostViewLib/FEModel.h"
#include "PostViewLib/ColorMap.h"
#include "PostViewLib/GLTexture1D.h"
#include "PostViewLib/GLObject.h"
#include "PostViewLib/DataMap.h"

namespace Post {

class CPropertyList;
class CGLModel;

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
