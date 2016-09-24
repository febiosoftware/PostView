#include "stdafx.h"
#ifdef WIN32
#include <Windows.h>
#include <gl/GL.h>
#endif
#ifdef __APPLE__
#include <OpenGL/gl.h>
#endif
#ifdef LINUX
#include <GL/gl.h>
#endif
#include "ImageSlicer.h"
#include <assert.h>

CImageSlicer::CImageSlicer()
{
	m_op = 0;
	m_off = 0.5;
	m_col1 = GLCOLOR(  0,   0,   0);
	m_col2 = GLCOLOR(255, 255, 255);
	m_texID = 0;
	m_reloadTexture = true;
}

CImageSlicer::~CImageSlicer()
{
}

void CImageSlicer::Create(C3DImage& im3d, BOUNDINGBOX box)
{
	// store the bounding box
	m_box = box;

	// get the original image dimensions
	int w = im3d.Width();
	int h = im3d.Height();
	int d = im3d.Depth();

	// find new image dimenions
	int nx = closest_pow2(w);
	int ny = closest_pow2(h);
	int nz = closest_pow2(d);
	m_im3d.Create(nx, ny, nz);

	// resample image
	im3d.StretchBlt(m_im3d);

	// call update to initialize all other data
	Update();
}

void CImageSlicer::Update()
{
	// get the 2D image
	CImage im2d;
	switch (m_op)
	{
	case 0: // X
		m_im3d.GetSampledSliceX(im2d, m_off);
		break;
	case 1: // Y
		m_im3d.GetSampledSliceY(im2d, m_off);
		break;
	case 2: // Z
		m_im3d.GetSampledSliceZ(im2d, m_off);
		break;
	default:
		assert(false);
	}

	// build the looktp table
	BuildLUT();

	// create the 2D image
	int W = im2d.Width();
	int H = im2d.Height();
	m_im.Create(W, H);

	// colorize the image
	int nn = W*H;
	byte* ps = im2d.GetBytes();
	byte* pd = m_im.GetBytes();
	for (int i = 0; i<nn; i++, ps++, pd += 4)
	{
		int val = *ps;
		pd[0] = m_LUTC[0][val];
		pd[1] = m_LUTC[1][val];
		pd[2] = m_LUTC[2][val];
		pd[3] = m_LUTC[3][val];
	}

	m_reloadTexture = true;
}

void CImageSlicer::BuildLUT()
{
	// build the LUT
	for (int i = 0; i<256; ++i)
	{
		m_LUTC[0][i] = m_col1.r + (m_col2.r - m_col1.r)*i / 255;
		m_LUTC[1][i] = m_col1.g + (m_col2.g - m_col1.g)*i / 255;
		m_LUTC[2][i] = m_col1.b + (m_col2.b - m_col1.b)*i / 255;
		m_LUTC[3][i] = m_col1.a + (m_col2.a - m_col1.a)*i / 255;
	}
}

//-----------------------------------------------------------------------------
//! Render textures
void CImageSlicer::Render()
{
	if (m_texID == 0)
	{
		glDisable(GL_TEXTURE_2D);

		glGenTextures(1, &m_texID);
		glBindTexture(GL_TEXTURE_2D, m_texID);

		// set texture parameter for 2D textures
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}
	else glBindTexture(GL_TEXTURE_2D, m_texID);

	int nx = m_im.Width();
	int ny = m_im.Height();
	if (m_reloadTexture && (nx*ny > 0))
	{
		glTexImage2D(GL_TEXTURE_2D, 0, 4, nx, ny, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_im.GetBytes());
		m_reloadTexture = false;
	}

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	//	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glColor4d(1, 1, 1, 1);

	double x[4], y[4], z[4];
	switch (m_op)
	{
	case 0:
		x[0] = x[1] = x[2] = x[3] = m_box.x0 + m_off*(m_box.x1 - m_box.x0);
		y[0] = y[3] = m_box.y1;  y[1] = y[2] = m_box.y0;
		z[0] = z[1] = m_box.z0;  z[2] = z[3] = m_box.z1;
		break;
	case 1:
		y[0] = y[1] = y[2] = y[3] = m_box.y0 + m_off*(m_box.y1 - m_box.y0);
		x[0] = x[3] = m_box.x1;  x[1] = x[2] = m_box.x0;
		z[0] = z[1] = m_box.z0;  z[2] = z[3] = m_box.z1;
		break;
	case 2:
		z[0] = z[1] = z[2] = z[3] = m_box.z0 + m_off*(m_box.z1 - m_box.z0);
		x[0] = x[3] = m_box.x1;  x[1] = x[2] = m_box.x0;
		y[0] = y[1] = m_box.y0;  y[2] = y[3] = m_box.y1;
		break;
	}

	glBegin(GL_QUADS);
	{
		glTexCoord2d(1, 0); glVertex3d(x[0], y[0], z[0]);
		glTexCoord2d(0, 0); glVertex3d(x[1], y[1], z[1]);
		glTexCoord2d(0, 1); glVertex3d(x[2], y[2], z[2]);
		glTexCoord2d(1, 1); glVertex3d(x[3], y[3], z[3]);
	}
	glEnd();

	glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
}
