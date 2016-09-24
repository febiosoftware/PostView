// VolRender.cpp: implementation of the CVolRender class.
//
//////////////////////////////////////////////////////////////////////

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
#include "VolRender.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVolRender::CVolRender()
{
	m_pImx = m_pImy = m_pImz = 0;
	m_nx = m_ny = m_nz = 0;

	m_blight = false;
	m_bcalc_lighting = true;
	m_light = vec3f(-1.f, -1.f, -1.f);

	m_texID = 0;

	Reset();
}

CVolRender::~CVolRender()
{
	Clear();
}

void CVolRender::Reset()
{
	m_col1 = GLCOLOR(0,0,0);
	m_col2 = GLCOLOR(255,255,255);
	m_amb = GLCOLOR(0,0,0);

	m_alpha = 0.1f;

	m_blight = false;
	m_bcalc_lighting = true;

	m_I0 = 0;
	m_I1 = 255;
	m_A0 = 0;
	m_A1 = 255;
	m_Amin = 0;
	m_Amax = 255;
}

void CVolRender::Clear()
{
	delete [] m_pImx; m_pImx = 0; m_nx = 0;
	delete [] m_pImy; m_pImy = 0; m_ny = 0;
	delete [] m_pImz; m_pImz = 0; m_nz = 0;
}

//-----------------------------------------------------------------------------
// Create new data for volume renderer
void CVolRender::Create(C3DImage& im3d, BOUNDINGBOX box)
{
	// get the original image dimensions
	int w = im3d.Width();
	int h = im3d.Height();
	int d = im3d.Depth();

	// clear the old data
	Clear();

	// find new image dimenions
	m_nx = closest_pow2(w);
	m_ny = closest_pow2(h);
	m_nz = closest_pow2(d);
	m_im3d.Create(m_nx, m_ny, m_nz);

	// resample image
	im3d.StretchBlt(m_im3d);

	// allocate new texture images
	m_pImx = new CRGBAImage[m_nx];
	for (int i=0; i<m_nx; i++) m_pImx[i].Create(m_ny, m_nz);

	m_pImy = new CRGBAImage[m_ny];
	for (int i=0; i<m_ny; i++) m_pImy[i].Create(m_nx, m_nz);

	m_pImz = new CRGBAImage[m_nz];
	for (int i=0; i<m_nz; i++) m_pImz[i].Create(m_nx, m_ny);

	// copy the box
	m_box = box;

	// calculate alpha scale factors
	int nd = m_nx;
	if (m_ny > nd) nd = m_ny;
	if (m_nz > nd) nd = m_nz;

	m_ax = (double) nd / (double) m_nx;
	m_ay = (double) nd / (double) m_ny;
	m_az = (double) nd / (double) m_nz;

	// update image data
	Update();
}

//-----------------------------------------------------------------------------
//! Calculate the attenuation factors for volume shading
void CVolRender::CalcAttenuation()
{
	m_att.Create(m_nx, m_ny, m_nz);

	vec3f l = m_light; l.Normalize();

	C3DGradientMap map(m_im3d, m_box);
	for (int k=0; k<m_nz; ++k)
		for (int j=0; j<m_ny; ++j)
			for (int i=0; i<m_nx; ++i)
			{
				vec3f f = map.Value(i, j, k); f.Normalize();
				float a = f*l;
				if (a < 0.f) a = 0.f;
				m_att.value(i, j, k) = (byte) (255.f*a);
			}
}

//-----------------------------------------------------------------------------
//! Update texture images for volume rendering
void CVolRender::Update()
{
	// calculate attenuation factors
	if (m_bcalc_lighting)
	{
		CalcAttenuation();
		m_bcalc_lighting = false;
	}

	// build the LUT
	int DI = m_I1 - m_I0;
	if (DI == 0) DI = 1;
	for (int i=0; i<256; ++i)
	{
		m_LUT[i] = (i < m_I0 ? 0 : (i > m_I1 ? 255 : 1+253*(i - m_I0)/DI));
		
		m_LUTC[0][i] = m_col1.r + (m_col2.r - m_col1.r)*i/255;
		m_LUTC[1][i] = m_col1.g + (m_col2.g - m_col1.g)*i/255;
		m_LUTC[2][i] = m_col1.b + (m_col2.b - m_col1.b)*i/255;
		m_LUTC[3][i] = (i == 0 ? m_Amin : (i == 255 ? m_Amax : (m_A0 + i*(m_A1 - m_A0)/255) ));
	}

	// create the x-images
	CImage im;
	im.Create(m_ny, m_nz);
	for (int i=0; i<m_nx; i++) 
	{
		m_im3d.GetSliceX(im, i);
		Colorize(m_pImx[i], im);
		if (m_blight) DepthCueX(m_pImx[i], i);
	}

	// create the y-images
	im.Create(m_nx, m_nz);
	for (int i=0; i<m_ny; i++) 
	{
		m_im3d.GetSliceY(im, i);
		Colorize(m_pImy[i], im);
		if (m_blight) DepthCueY(m_pImy[i], i);
	}

	// create the z-images
	im.Create(m_nx, m_ny);
	for (int i=0; i<m_nz; i++) 
	{
		m_im3d.GetSliceZ(im, i);
		Colorize(m_pImz[i], im);
		if (m_blight) DepthCueZ(m_pImz[i], i);
	}
}

//-----------------------------------------------------------------------------
//! Colorize the texture images
void CVolRender::Colorize(CRGBAImage& imd, CImage& ims)
{
	int nx = imd.Width();
	int ny = imd.Height();
	int nn = nx*ny;
	byte* ps = ims.GetBytes();
	byte* pd = imd.GetBytes();
	for (int i=0; i<nn; i++, ps++, pd+=4)
	{
		int val = m_LUT[*ps];
		pd[0] = m_LUTC[0][val];
		pd[1] = m_LUTC[1][val];
		pd[2] = m_LUTC[2][val];
		pd[3] = m_LUTC[3][val];
	}
}

//-----------------------------------------------------------------------------
//! Attenuate X-textures
void CVolRender::DepthCueX(CRGBAImage& im, int n)
{
	int nx = im.Width();
	int ny = im.Height();
	int nz = m_att.Width();

	byte* p = im.GetBytes();
	for (int j=0; j<ny; ++j)
		for (int i=0; i<nx; ++i, p += 4)
		{
			byte a = m_att.value(n, i, j);
			p[0] = (byte) ((p[0]*a + m_amb.r*(255 - a))/255);
			p[1] = (byte) ((p[1]*a + m_amb.g*(255 - a))/255);
			p[2] = (byte) ((p[2]*a + m_amb.b*(255 - a))/255);
		}
}

//-----------------------------------------------------------------------------
//! Attenuate Y-textures
void CVolRender::DepthCueY(CRGBAImage& im, int n)
{
	int nx = im.Width();
	int ny = im.Height();
	int nz = m_att.Height();

	byte* p = im.GetBytes();
	for (int j=0; j<ny; ++j)
		for (int i=0; i<nx; ++i, p += 4)
		{
			byte a = m_att.value(i, n, j);
			p[0] = (byte) ((p[0]*a + m_amb.r*(255 - a))/255);
			p[1] = (byte) ((p[1]*a + m_amb.g*(255 - a))/255);
			p[2] = (byte) ((p[2]*a + m_amb.b*(255 - a))/255);
		}
}

//-----------------------------------------------------------------------------
//! Attenuate Z-textures
void CVolRender::DepthCueZ(CRGBAImage& im, int n)
{
	int nx = im.Width();
	int ny = im.Height();
	int nz = m_att.Depth();

	byte* p = im.GetBytes();
	for (int j=0; j<ny; ++j)
		for (int i=0; i<nx; ++i, p += 4)
		{
			byte a = m_att.value(i, j, n);
			p[0] = (byte) ((p[0]*a + m_amb.r*(255 - a))/255);
			p[1] = (byte) ((p[1]*a + m_amb.g*(255 - a))/255);
			p[2] = (byte) ((p[2]*a + m_amb.b*(255 - a))/255);
		}
}

//-----------------------------------------------------------------------------
//! Render textures
void CVolRender::Render(quat4f q)
{
	if (m_texID == 0)
	{
		glGenTextures(1, &m_texID);

		glBindTexture(GL_TEXTURE_2D, m_texID);

		// set texture parameter for 2D textures
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	}
	else glBindTexture(GL_TEXTURE_2D, m_texID);

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

//	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	vec3f r(0,0,1);
	q.Inverse().RotateVector(r);

	double x = fabs(r.x);
	double y = fabs(r.y);
	double z = fabs(r.z);

	if ((x>y) && (x>z)) RenderX(r.x > 0 ? 1 : -1);
	if ((y>x) && (y>z)) RenderY(r.y > 0 ? 1 : -1);
	if ((z>y) && (z>x)) RenderZ(r.z > 0 ? 1 : -1);

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}

//-----------------------------------------------------------------------------
void CVolRender::RenderX(int inc)
{
	double a = m_alpha * m_ax;
	glColor4d(1,1,1,a);

	double x;
	double fx;

	if (m_nx == 1) fx = 1; else fx = 1.0 / (m_nx - 1.0);

	int n0, n1;
	if (inc == 1) { n0 = 0; n1 = m_nx; }
	else { n0 = m_nx-1; n1 = -1; }

	double e0 = 0.0;
	double e1 = 1.0 - e0;

	for (int i=n0; i != n1; i += inc)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, 4, m_ny, m_nz, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_pImx[i].GetBytes());

		x = m_box.x0 + i*(m_box.x1 - m_box.x0)*fx;

		glBegin(GL_QUADS);
		{
			glTexCoord2d(e0,e0); glVertex3d(x, m_box.y0, m_box.z0);
			glTexCoord2d(e1,e0); glVertex3d(x, m_box.y1, m_box.z0);
			glTexCoord2d(e1,e1); glVertex3d(x, m_box.y1, m_box.z1);
			glTexCoord2d(e0,e1); glVertex3d(x, m_box.y0, m_box.z1);
		}
		glEnd();
	}
}

//-----------------------------------------------------------------------------
void CVolRender::RenderY(int inc)
{
	double a = m_alpha * m_ay;
	glColor4d(1,1,1,a);

	double y;
	double fy;

	if (m_ny == 1) fy = 1; else fy = 1.0 / (m_ny - 1.0);

	int n0, n1;
	if (inc == 1) { n0 = 0; n1 = m_ny; }
	else { n0 = m_ny-1; n1 = -1; }

	double e0 = 0.0;
	double e1 = 1.0 - e0;

	for (int i=n0; i != n1; i += inc)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, 4, m_nx, m_nz, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_pImy[i].GetBytes());

		y = m_box.y0 + i*(m_box.y1 - m_box.y0)*fy;

		glBegin(GL_QUADS);
		{
			glTexCoord2d(e0,e0); glVertex3d(m_box.x0, y, m_box.z0);
			glTexCoord2d(e1,e0); glVertex3d(m_box.x1, y, m_box.z0);
			glTexCoord2d(e1,e1); glVertex3d(m_box.x1, y, m_box.z1);
			glTexCoord2d(e0,e1); glVertex3d(m_box.x0, y, m_box.z1);
		}
		glEnd();
	}
}

//-----------------------------------------------------------------------------
void CVolRender::RenderZ(int inc)
{
	double a = m_alpha * m_az;
	glColor4d(1,1,1,a);

	double z;
	double fz;

	if (m_nz == 1) fz = 1; else fz = 1.0 / (m_nz - 1.0);

	int n0, n1;
	if (inc == 1) { n0 = 0; n1 = m_nz; }
	else { n0 = m_nz-1; n1 = -1; }

	double e0 = 0.0;
	double e1 = 1.0 - e0;

	for (int i=n0; i != n1; i += inc)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, 4, m_nx, m_ny, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_pImz[i].GetBytes());

		z = m_box.z0 + i*(m_box.z1 - m_box.z0)*fz;

		glBegin(GL_QUADS);
		{
			glTexCoord2d(e0,e0); glVertex3d(m_box.x0, m_box.y0, z);
			glTexCoord2d(e1,e0); glVertex3d(m_box.x1, m_box.y0, z);
			glTexCoord2d(e1,e1); glVertex3d(m_box.x1, m_box.y1, z);
			glTexCoord2d(e0,e1); glVertex3d(m_box.x0, m_box.y1, z);
		}
		glEnd();
	}
}
