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

void TriMesh::Merge(TriMesh& tri)
{
	int N = (int)m_Face.size();
	m_Face.resize(m_Face.size() + tri.m_Face.size());
	for (size_t i = 0; i < tri.m_Face.size(); ++i)
		m_Face[N + i] = tri.m_Face[i];
}

CMarchingCubes::CMarchingCubes(CImageModel* img) : CGLImageRenderer(img)
{
	m_val = 0.5f;
	m_oldVal = -1.f;
	m_bsmooth = true;
	m_col = GLCOLOR(200, 185, 185);
}

CMarchingCubes::~CMarchingCubes()
{

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

	float ref = m_val * 255;

	C3DGradientMap grad(im3d, b);

	#pragma omp parallel default(shared)
	{
		TriMesh temp;
		temp.Reserve(1000000);
		float val[8];
		vec3f r[8], g[8];

		#pragma omp for schedule(dynamic)
		for (int k = 0; k < NZ - 1; ++k)
		{
			for (int j = 0; j < NY - 1; ++j)
			{
				for (int i = 0; i < NX - 1; ++i)
				{
					// get the voxel's values
					val[0] = im3d.value(i, j, k);
					val[1] = im3d.value(i + 1, j, k);
					val[2] = im3d.value(i + 1, j + 1, k);
					val[3] = im3d.value(i, j + 1, k);

					val[4] = im3d.value(i, j, k + 1);
					val[5] = im3d.value(i + 1, j, k + 1);
					val[6] = im3d.value(i + 1, j + 1, k + 1);
					val[7] = im3d.value(i, j + 1, k + 1);

					// calculate the case of the element
					int ncase = 0;
					for (int l = 0; l < 8; ++l)
						if (val[l] <= ref) ncase |= (1 << l);

					if ((ncase != 0) && (ncase != 255))
					{
						// get the corners
						r[0].x = b.x0 + i    *(b.x1 - b.x0) / (NX - 1); r[0].y = b.y0 + j    *(b.y1 - b.y0) / (NY - 1); r[0].z = b.z0 + k    *(b.z1 - b.z0) / (NZ - 1);
						r[1].x = b.x0 + (i + 1)*(b.x1 - b.x0) / (NX - 1); r[1].y = b.y0 + j    *(b.y1 - b.y0) / (NY - 1); r[1].z = b.z0 + k    *(b.z1 - b.z0) / (NZ - 1);
						r[2].x = b.x0 + (i + 1)*(b.x1 - b.x0) / (NX - 1); r[2].y = b.y0 + (j + 1)*(b.y1 - b.y0) / (NY - 1); r[2].z = b.z0 + k    *(b.z1 - b.z0) / (NZ - 1);
						r[3].x = b.x0 + i    *(b.x1 - b.x0) / (NX - 1); r[3].y = b.y0 + (j + 1)*(b.y1 - b.y0) / (NY - 1); r[3].z = b.z0 + k    *(b.z1 - b.z0) / (NZ - 1);
						r[4].x = b.x0 + i    *(b.x1 - b.x0) / (NX - 1); r[4].y = b.y0 + j    *(b.y1 - b.y0) / (NY - 1); r[4].z = b.z0 + (k + 1)*(b.z1 - b.z0) / (NZ - 1);
						r[5].x = b.x0 + (i + 1)*(b.x1 - b.x0) / (NX - 1); r[5].y = b.y0 + j    *(b.y1 - b.y0) / (NY - 1); r[5].z = b.z0 + (k + 1)*(b.z1 - b.z0) / (NZ - 1);
						r[6].x = b.x0 + (i + 1)*(b.x1 - b.x0) / (NX - 1); r[6].y = b.y0 + (j + 1)*(b.y1 - b.y0) / (NY - 1); r[6].z = b.z0 + (k + 1)*(b.z1 - b.z0) / (NZ - 1);
						r[7].x = b.x0 + i    *(b.x1 - b.x0) / (NX - 1); r[7].y = b.y0 + (j + 1)*(b.y1 - b.y0) / (NY - 1); r[7].z = b.z0 + (k + 1)*(b.z1 - b.z0) / (NZ - 1);

						// calculate gradients
						g[0] = grad.Value(i, j, k);
						g[1] = grad.Value(i + 1, j, k);
						g[2] = grad.Value(i + 1, j + 1, k);
						g[3] = grad.Value(i, j + 1, k);
						g[4] = grad.Value(i, j, k + 1);
						g[5] = grad.Value(i + 1, j, k + 1);
						g[6] = grad.Value(i + 1, j + 1, k + 1);
						g[7] = grad.Value(i, j + 1, k + 1);
					}

					// loop over faces
					int* pf = LUT[ncase];
					for (int l = 0; l < 5; l++)
					{
						if (*pf == -1) break;

						// calculate nodal positions
						TriMesh::TRI tri;
						for (int m = 0; m < 3; m++)
						{
							int n1 = ET_HEX[pf[m]][0];
							int n2 = ET_HEX[pf[m]][1];

							float w = (ref - val[n1]) / (val[n2] - val[n1]);

							tri.m_node[m] = r[n1] * (1 - w) + r[n2] * w;

							vec3f normal = g[n1] * (1 - w) + g[n2] * w;
							normal.Normalize();
							tri.m_norm[m] = -normal;
						}

						vec3f normal = (tri.m_node[1] - tri.m_node[0]) ^ (tri.m_node[2] - tri.m_node[0]);
						normal.Normalize();
						tri.m_faceNorm = normal;

						// add it to the (temp) mesh
						temp.AddFace(tri);

						pf += 3;
					}
				}
			}
		}

		#pragma omp critical
		m_mesh.Merge(temp);
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
		if (m_bsmooth)
		{
			glNormal3f(face.m_norm[0].x, face.m_norm[0].y, face.m_norm[0].z);
			glVertex3f(face.m_node[0].x, face.m_node[0].y, face.m_node[0].z);
			glNormal3f(face.m_norm[1].x, face.m_norm[1].y, face.m_norm[1].z);
			glVertex3f(face.m_node[1].x, face.m_node[1].y, face.m_node[1].z);
			glNormal3f(face.m_norm[2].x, face.m_norm[2].y, face.m_norm[2].z);
			glVertex3f(face.m_node[2].x, face.m_node[2].y, face.m_node[2].z);
		}
		else
		{
			glNormal3f(face.m_faceNorm.x, face.m_faceNorm.y, face.m_faceNorm.z);
			glVertex3f(face.m_node[0].x, face.m_node[0].y, face.m_node[0].z);
			glVertex3f(face.m_node[1].x, face.m_node[1].y, face.m_node[1].z);
			glVertex3f(face.m_node[2].x, face.m_node[2].y, face.m_node[2].z);
		}
	}
	glEnd();
}
