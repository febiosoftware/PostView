#include "stdafx.h"
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
#include "MarchingCubes.h"
#include "ImageModel.h"
#include "3DImage.h"
#include <sstream>
using namespace std;

extern int LUT[256][15];
extern int ET_HEX[12][2];

TriMesh::TriMesh()
{
}

void TriMesh::Clear()
{
	m_Face.clear();
}

void TriMesh::Reserve(size_t nsize)
{
	m_Face.reserve(nsize);
}

void TriMesh::Resize(size_t nsize)
{
	m_Face.resize(nsize);
}

void TriMesh::Merge(TriMesh& tri, int ncount)
{
	if (ncount < 0) ncount = tri.Faces();
	int N = (int)m_Face.size();
	m_Face.resize(m_Face.size() + ncount);
	for (size_t i = 0; i < ncount; ++i)
		m_Face[N + i] = tri.m_Face[i];
}

CMarchingCubes::CMarchingCubes(CImageModel* img) : CGLImageRenderer(img)
{
	static int n = 1;
	stringstream ss;
	ss << "ImageIsosurface" << n++;
	SetName(ss.str());

	m_val = 0.5f;
	m_oldVal = -1.f;
	m_bsmooth = true;
	m_col = GLCOLOR(200, 185, 185);
}

CMarchingCubes::~CMarchingCubes()
{

}

void CMarchingCubes::SetSmooth(bool b)
{ 
	m_bsmooth = b; 
	m_oldVal = -1.f;
	Create();
}

void CMarchingCubes::Create()
{
	if (m_oldVal == m_val) return;
	m_oldVal = m_val;

	m_mesh.Clear();

	CImageModel& im = *GetImageModel();
	C3DImage& im3d = *im.Get3DImage();
	BOUNDINGBOX b = im.GetBoundingBox();

	int NX = im3d.Width();
	int NY = im3d.Height();
	int NZ = im3d.Depth();
	if ((NX == 1) || (NY == 1) || (NZ == 1)) return;

	float ref = m_val * 255;

	C3DGradientMap grad(im3d, b);

	#pragma omp parallel default(shared)
	{
		const int MAX_FACES = 1000000;
		TriMesh temp;
		temp.Resize(MAX_FACES);
		int nfaces = 0;
		float val[8];
		vec3f r[8], g[8];

		float dxi = (b.x1 - b.x0) / (NX - 1);
		float dyi = (b.y1 - b.y0) / (NY - 1);
		float dzi = (b.z1 - b.z0) / (NZ - 1);

		#pragma omp for schedule(dynamic)
		for (int k = 0; k < NZ - 1; ++k)
		{
			for (int j = 0; j < NY - 1; ++j)
			{
				for (int i = 0; i < NX - 1; ++i)
				{
					// get the voxel's values
					if (i == 0)
					{
						val[0] = im3d.value(i, j, k);
						val[4] = im3d.value(i, j, k + 1);
						val[3] = im3d.value(i, j + 1, k);
						val[7] = im3d.value(i, j + 1, k + 1);
					}

					val[1] = im3d.value(i + 1, j, k);
					val[2] = im3d.value(i + 1, j + 1, k);
					val[5] = im3d.value(i + 1, j, k + 1);
					val[6] = im3d.value(i + 1, j + 1, k + 1);

					// calculate the case of the element
					int ncase = 0;
					for (int l = 0; l < 8; ++l)
						if (val[l] <= ref) ncase |= (1 << l);

					if ((ncase != 0) && (ncase != 255))
					{
						// get the corners
						r[0].x = b.x0 + i      *dxi; r[0].y = b.y0 + j      *dyi; r[0].z = b.z0 + k      *dzi;
						r[1].x = b.x0 + (i + 1)*dxi; r[1].y = b.y0 + j      *dyi; r[1].z = b.z0 + k      *dzi;
						r[2].x = b.x0 + (i + 1)*dxi; r[2].y = b.y0 + (j + 1)*dyi; r[2].z = b.z0 + k      *dzi;
						r[3].x = b.x0 + i      *dxi; r[3].y = b.y0 + (j + 1)*dyi; r[3].z = b.z0 + k      *dzi;
						r[4].x = b.x0 + i      *dxi; r[4].y = b.y0 + j      *dyi; r[4].z = b.z0 + (k + 1)*dzi;
						r[5].x = b.x0 + (i + 1)*dxi; r[5].y = b.y0 + j      *dyi; r[5].z = b.z0 + (k + 1)*dzi;
						r[6].x = b.x0 + (i + 1)*dxi; r[6].y = b.y0 + (j + 1)*dyi; r[6].z = b.z0 + (k + 1)*dzi;
						r[7].x = b.x0 + i      *dxi; r[7].y = b.y0 + (j + 1)*dyi; r[7].z = b.z0 + (k + 1)*dzi;

						// calculate gradients
						if (m_bsmooth)
						{
							g[0] = grad.Value(i, j, k);
							g[1] = grad.Value(i + 1, j, k);
							g[2] = grad.Value(i + 1, j + 1, k);
							g[3] = grad.Value(i, j + 1, k);
							g[4] = grad.Value(i, j, k + 1);
							g[5] = grad.Value(i + 1, j, k + 1);
							g[6] = grad.Value(i + 1, j + 1, k + 1);
							g[7] = grad.Value(i, j + 1, k + 1);
						}
					}

					// loop over faces
					int* pf = LUT[ncase];
					for (int l = 0; l < 5; l++)
					{
						if (*pf == -1) break;

						// calculate nodal positions
						TriMesh::TRI& tri = temp.Face(nfaces++);
						for (int m = 0; m < 3; m++)
						{
							int n1 = ET_HEX[pf[m]][0];
							int n2 = ET_HEX[pf[m]][1];

							float w = (ref - val[n1]) / (val[n2] - val[n1]);

							tri.m_node[m] = r[n1] * (1 - w) + r[n2] * w;

							if (m_bsmooth)
							{
								vec3f normal = g[n1] * (1 - w) + g[n2] * w;
								normal.Normalize();
								tri.m_norm[m] = -normal;
							}
						}

						if (m_bsmooth == false)
						{
							vec3f normal = (tri.m_node[1] - tri.m_node[0]) ^ (tri.m_node[2] - tri.m_node[0]);
							normal.Normalize();
							tri.m_norm[0] = normal;
							tri.m_norm[1] = normal;
							tri.m_norm[2] = normal;
						}

						pf += 3;

						if (nfaces == MAX_FACES)
						{
							#pragma omp critical
							m_mesh.Merge(temp, nfaces);
							nfaces = 0;
						}
					}

					val[0] = val[1];
					val[4] = val[5];
					val[3] = val[2];
					val[7] = val[6];
				}
			}
		}

		#pragma omp critical
		m_mesh.Merge(temp, nfaces);
	}
}

void CMarchingCubes::SetIsoValue(float v)
{
	m_val = v;
	Create();
}

void CMarchingCubes::Render(CGLContext& rc)
{
	glColor3ub(m_col.r, m_col.g, m_col.b);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < m_mesh.Faces(); ++i)
	{
		TriMesh::TRI& face = m_mesh.Face(i);
		glNormal3f(face.m_norm[0].x, face.m_norm[0].y, face.m_norm[0].z);
		glVertex3f(face.m_node[0].x, face.m_node[0].y, face.m_node[0].z);
		glNormal3f(face.m_norm[1].x, face.m_norm[1].y, face.m_norm[1].z);
		glVertex3f(face.m_node[1].x, face.m_node[1].y, face.m_node[1].z);
		glNormal3f(face.m_norm[2].x, face.m_norm[2].y, face.m_norm[2].z);
		glVertex3f(face.m_node[2].x, face.m_node[2].y, face.m_node[2].z);
	}
	glEnd();
}
