// VolRender.h: interface for the CVolRender class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VOLRENDER_H__C7D04B9A_3A3A_4CB1_AA86_488412332A7A__INCLUDED_)
#define AFX_VOLRENDER_H__C7D04B9A_3A3A_4CB1_AA86_488412332A7A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "3DImage.h"
#include "bbox.h"
#include "GLImageRenderer.h"

class CImageModel;

class CVolRender : public CGLImageRenderer
{
public:
	CVolRender(CImageModel* img);
	virtual ~CVolRender();

	void Clear();

	void Create();

	void Update();

	void Render(CGLContext& rc) override;

	void Reset();

	BOUNDINGBOX GetBoundingBox() { return m_box; }
	void SetBoundingBox(BOUNDINGBOX box) { m_box = box; }

	vec3f GetLightPosition() { return m_light; }
	void SetLightPosition(vec3f r) { m_light = r; m_bcalc_lighting = true; }

protected:
	void RenderX(int inc);
	void RenderY(int inc);
	void RenderZ(int inc);

	void Colorize(CRGBAImage& imd, CImage& ims);

	void CalcAttenuation();
	void DepthCueX(CRGBAImage& im, int n);
	void DepthCueY(CRGBAImage& im, int n);
	void DepthCueZ(CRGBAImage& im, int n);

public:
	GLCOLOR	m_col1, m_col2;		// the two colors
	GLCOLOR	m_amb;				//!< ambient color
	int		m_I0, m_I1;			// intensity range
	int		m_A0, m_A1;			// transparency range
	int		m_Amin, m_Amax;		// clamp transparency levels
	float	m_alpha;			// alpha scale factor
	bool	m_blight;	// use lighting

protected:
	C3DImage		m_im3d;	// resampled 3D image data
	C3DImage		m_att;	// attenuation map (for lighting)

	CRGBAImage*	m_pImx;	// Image array in x-direction
	CRGBAImage*	m_pImy;	// Image array in y-direction
	CRGBAImage*	m_pImz;	// Image array in x-direction
	unsigned int m_texID;

	int m_nx;	// nr of images in x-direction
	int m_ny;	// nr of images in y-direction
	int	m_nz;	// nr of images in z-direction

	double m_ax, m_ay, m_az;	//!< alpha scale factors

	bool	m_bcalc_lighting;	//!< calculate shading?
	vec3f	m_light;	// light direction

	BOUNDINGBOX	m_box;

	int	m_LUT[256], m_LUTC[4][256];
};

#endif // !defined(AFX_VOLRENDER_H__C7D04B9A_3A3A_4CB1_AA86_488412332A7A__INCLUDED_)
