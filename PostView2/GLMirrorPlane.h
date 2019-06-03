#pragma once
#include "GLPlot.h"

class CGLMirrorPlane : public CGLPlot
{
public:
	CGLMirrorPlane(CGLModel* fem);

	CPropertyList* propertyList();

	// render the object to the 3D view
	void Render(CGLContext& rc) override;

private:
	void RenderPlane();

public:
	int		m_plane;
	float	m_transparency;
	bool	m_showPlane;

private:
	vec3f	m_norm;
};
