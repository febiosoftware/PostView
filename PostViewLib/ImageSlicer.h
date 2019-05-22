#pragma once
#include "3DImage.h"
#include "bbox.h"
#include "GLImageRenderer.h"

class CImageModel;

class CImageSlicer : public CGLImageRenderer
{
public:
	CImageSlicer(CImageModel* img);
	~CImageSlicer();

	void Create();

	void Update();

	void Render(CGLContext& rc) override;

	int GetOrientation() const { return m_op; }
	void SetOrientation(int n) { m_op = n; }

	double GetOffset() const { return m_off; }
	void SetOffset(double f) { m_off = f; }

	GLCOLOR GetColor1() const { return m_col1; }
	GLCOLOR GetColor2() const { return m_col2; }

	void SetColor1(const GLCOLOR& c) { m_col1 = c; }
	void SetColor2(const GLCOLOR& c) { m_col2 = c; }

private:
	void BuildLUT();

private:
	C3DImage		m_im3d;	// resampled 3D image data
	CRGBAImage		m_im;	// 2D image that will be displayed
	int				m_LUTC[4][256];	// color lookup table
	BOUNDINGBOX		m_box;
	bool			m_reloadTexture;

	int		m_op;	// x,y,z
	double	m_off;	// offset (0 - 1)
	GLCOLOR	m_col1, m_col2;

	unsigned int m_texID;
};
