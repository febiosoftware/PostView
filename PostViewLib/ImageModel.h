#pragma once
#include <vector>
#include "bbox.h"
#include "GLObject.h"

class C3DImage;
class CGLImageRenderer;

class CImageModel : public CGLObject
{
public:
	CImageModel();
	~CImageModel();

	void Set3DImage(C3DImage* img, BOUNDINGBOX b);

	C3DImage* Get3DImage() { return m_pImg; }

	int ImageRenderers() const { return (int)m_render.size(); }
	CGLImageRenderer* GetImageRenderer(int i) { return m_render[i]; }
	bool RemoveRenderer(CGLImageRenderer* render);

	void AddImageRenderer(CGLImageRenderer* render);

	BOUNDINGBOX& GetBoundingBox() { return m_box; }

private:
	BOUNDINGBOX		m_box;						//!< physical dimensions of image
	C3DImage*		m_pImg;						//!< 3D image
	std::vector<CGLImageRenderer*>	m_render;	//!< image renderers
};
