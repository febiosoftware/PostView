#include "stdafx.h"
#include "GLModel.h"
#include "GLPlaneCutPlot.h"
#include "PostViewLib/FEDataManager.h"
#include "GLContext.h"
#include "PostViewLib/constants.h"
#include "GLView.h"
#include "GLContext.h"
#include <PostViewLib/GLCamera.h>

//-----------------------------------------------------------------------------
extern int ET_HEX[12][2];
extern int ET_HEX20[12][3];
extern int ET_TET[6][2];
extern int ET_PENTA[9][2];
extern int ET_TET10[6][3];

//-----------------------------------------------------------------------------
// constructor
CGLModel::CGLModel(FEModel* ps)
{
	FEMesh* pm = ps->GetMesh();

	// see if the mesh has any vector fields
	// which can be used for displacement maps
	FEDataManager* pdm = ps->GetDataManager();
	FEDataFieldPtr pd = pdm->FirstNode();
	int nv = 0;
	int ndisp = -1;
	for (int i=0; i<pdm->NodeFields(); ++i, ++pd)
	{
		if ((*pd)->Type() == DATA_VEC3F) ++nv;
		if (strcmp((*pd)->GetName(), "displacement") == 0) ndisp = i;
	}

	m_pdis = (nv ? new CGLDisplacementMap(this) : 0);
	if (ndisp != -1)
	{
		ps->SetDisplacementField(BUILD_FIELD(1, ndisp, 0));
	}

	m_pcol = new CGLColorMap(this);
	m_ps = ps;

	sprintf(m_szname, "Model");

	m_bnorm   = false;
	m_bsmooth = (pm->Elements() > 100000 ? false : true);
	m_boutline = false;
	m_bghost = false;
	m_nDivs = 2;

	m_bShell2Hex  = false;
	m_nshellref   = 0;

	m_line_col = GLCOLOR(0,0,0);
	m_node_col = GLCOLOR(0,0,255);
	m_sel_col  = GLCOLOR(255,0,0);

	m_nrender = RENDER_MODE_SOLID;
}

//-----------------------------------------------------------------------------
//! destructor
CGLModel::~CGLModel(void)
{
	delete m_pdis;
	delete m_pcol;
}

//-----------------------------------------------------------------------------
// Update the model data
void CGLModel::Update(int ntime, float dt, bool breset)
{
	// update displacement map
	if (m_pdis && m_pdis->IsActive()) m_pdis->Update(ntime, dt, breset);

	// update the colormap
	if (m_pcol && m_pcol->IsActive()) m_pcol->Update(ntime, dt, breset);
}

//-----------------------------------------------------------------------------
void CGLModel::SetMaterialParams(FEMaterial* pm)
{
	GLfloat fv[4] = {0,0,0,1};
	const float f = 1.f / 255.f;

	GLubyte a = (GLubyte) (255.f*pm->transparency);

	glColor4ub(pm->diffuse.r, pm->diffuse.g, pm->diffuse.b, a);

	fv[0] = (float) pm->ambient.r*f;
	fv[1] = (float) pm->ambient.g*f;
	fv[2] = (float) pm->ambient.b*f;
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, fv);

	fv[0] = (float) pm->specular.r*f;
	fv[1] = (float) pm->specular.g*f;
	fv[2] = (float) pm->specular.b*f;
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, fv);

	fv[0] = (float) pm->emission.r*f;
	fv[1] = (float) pm->emission.g*f;
	fv[2] = (float) pm->emission.b*f;
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, fv);

	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, pm->shininess*64.f);
}

//-----------------------------------------------------------------------------
bool CGLModel::AddDisplacementMap()
{
	FEModel* ps = GetFEModel();

	// see if the mesh has any vector fields
	// which can be used for displacement maps
	FEDataManager* pdm = ps->GetDataManager();
	FEDataFieldPtr pd = pdm->FirstNode();
	int nv = 0;
	int ndisp = -1;
	for (int i=0; i<pdm->NodeFields(); ++i, ++pd)
	{
		if ((*pd)->Type() == DATA_VEC3F) ++nv;
		if (strcmp((*pd)->GetName(), "displacement") == 0) ndisp = i;
	}

	if (nv == 0) return false;

	if (m_pdis) delete m_pdis;
	m_pdis = new CGLDisplacementMap(this);
	if (ndisp != -1)
	{
		ps->SetDisplacementField(BUILD_FIELD(1, ndisp, 0));
	}	
	return true;
}

void CGLModel::Render(CGLContext& rc)
{
	// render the faces
	RenderFaces(rc);

	// render the outline
	glDepthRange(0, 0.99998);
	if (m_boutline) RenderOutline(rc);
	glDepthRange(0, 1);

	// render the selected elements and faces
	RenderSelection(rc);

	// render the normals
	if (m_bnorm) RenderNormals(rc);

	// render the ghost
	if (m_bghost) RenderGhost(rc);

	// render decorations
	RenderDecorations();
}

//-----------------------------------------------------------------------------

void CGLModel::RenderFaces(CGLContext& rc)
{
	glPushAttrib(GL_ENABLE_BIT);

	// get the mesh
	FEModel* ps = m_ps;
	FEMesh* pm = ps->GetMesh();

	// we render the mesh by looping over the materials
	// first we render the opaque meshes
	for (int m=0; m<ps->Materials(); ++m)
	{
		// get the material
		FEMaterial* pmat = ps->GetMaterial(m);

		// make sure the material is visible
		if (pmat->bvisible && (pmat->transparency>.99f)) RenderMaterial(ps, m);
	}

	// next, we render the transparent meshes
	for (int m=0; m<ps->Materials(); ++m)
	{
		// get the material
		FEMaterial* pmat = ps->GetMaterial(m);

		// make sure the material is visible
		if (pmat->bvisible && (pmat->transparency<=.99f) && (pmat->transparency>0.001f)) 
		{
			if (pmat->m_ntransmode == RENDER_TRANS_CONSTANT) RenderMaterial(ps, m);
			else RenderTransparentMaterial(rc, ps, m);
		}
	}

	glPopAttrib();
}

//-----------------------------------------------------------------------------

void CGLModel::RenderSelection(CGLContext &rc)
{
	int i;
	bool bnode = m_pcol->DisplayNodalValues();

	// get the mesh
	FEModel* ps = m_ps;
	FEMesh* pm = ps->GetMesh();

	glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT);

	// now render the selected faces
	glDisable(GL_TEXTURE_1D);
	GLCOLOR c = m_sel_col;
	glColor4ub(c.r,c.g,c.g,128);
	glDisable(GL_LIGHTING);

	// render the selected faces
	for ( i=0; i<pm->Faces(); ++i)
	{
		FEFace& face = pm->Face(i);
		FEElement& el = pm->Element(face.m_elem[0]);
		FEMaterial* pmat = ps->GetMaterial(el.m_MatID);

		if (pmat->bvisible && face.IsVisible() && (el.IsSelected() || face.IsSelected()))
		{
			// okay, we got one, so let's render it
			glLoadName(face.m_elem[0]+1);
			RenderFace(face, pm, m_nDivs, bnode);
		}
	}
	glEnable(GL_LIGHTING);

	// render the outline of the selected elements
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glColor3ub(255,255,0);
	glLineWidth(1.5f);
	for (i=0; i<pm->Elements(); ++i)
	{
		FEElement& el = pm->Element(i);
		if (el.IsSelected()) RenderElementOutline(el, pm);
	}
	vec3f r[FEFace::MAX_NODES];
	for (i=0; i<pm->Faces(); ++i)
	{
		FEFace& f = pm->Face(i);
		if (f.IsSelected())
		{
			int n = f.Nodes();
			for (int j=0; j<n; ++j) r[j] = pm->Node(f.node[j]).m_rt;
			switch (f.m_ntype)
			{
			case FACE_TRI3:
				glBegin(GL_LINE_LOOP);
				{
					glVertex3f(r[0].x, r[0].y, r[0].z);
					glVertex3f(r[1].x, r[1].y, r[1].z);
					glVertex3f(r[2].x, r[2].y, r[2].z);
				}
				glEnd();
				break;
			case FACE_QUAD4:
				glBegin(GL_LINE_LOOP);
				{
					glVertex3f(r[0].x, r[0].y, r[0].z);
					glVertex3f(r[1].x, r[1].y, r[1].z);
					glVertex3f(r[2].x, r[2].y, r[2].z);
					glVertex3f(r[3].x, r[3].y, r[3].z);
				}
				glEnd();
				break;
			case FACE_TRI6:
				glBegin(GL_LINE_LOOP);
				{
					glVertex3f(r[0].x, r[0].y, r[0].z);
					glVertex3f(r[3].x, r[3].y, r[3].z);
					glVertex3f(r[1].x, r[1].y, r[1].z);
					glVertex3f(r[4].x, r[4].y, r[4].z);
					glVertex3f(r[2].x, r[2].y, r[2].z);
					glVertex3f(r[5].x, r[5].y, r[5].z);
				}
				glEnd();
				break;
			case FACE_TRI7:
				glBegin(GL_LINE_LOOP);
				{
					glVertex3f(r[0].x, r[0].y, r[0].z);
					glVertex3f(r[3].x, r[3].y, r[3].z);
					glVertex3f(r[1].x, r[1].y, r[1].z);
					glVertex3f(r[4].x, r[4].y, r[4].z);
					glVertex3f(r[2].x, r[2].y, r[2].z);
					glVertex3f(r[5].x, r[5].y, r[5].z);
				}
				glEnd();
				break;
			case FACE_QUAD8:
				glBegin(GL_LINE_LOOP);
				{
					glVertex3f(r[0].x, r[0].y, r[0].z);
					glVertex3f(r[4].x, r[4].y, r[4].z);
					glVertex3f(r[1].x, r[1].y, r[1].z);
					glVertex3f(r[5].x, r[5].y, r[5].z);
					glVertex3f(r[2].x, r[2].y, r[2].z);
					glVertex3f(r[6].x, r[6].y, r[6].z);
					glVertex3f(r[3].x, r[3].y, r[3].z);
					glVertex3f(r[7].x, r[7].y, r[7].z);
				}
				glEnd();
				break;
			case FACE_QUAD9:
				glBegin(GL_LINE_LOOP);
				{
					glVertex3f(r[0].x, r[0].y, r[0].z);
					glVertex3f(r[4].x, r[4].y, r[4].z);
					glVertex3f(r[1].x, r[1].y, r[1].z);
					glVertex3f(r[5].x, r[5].y, r[5].z);
					glVertex3f(r[2].x, r[2].y, r[2].z);
					glVertex3f(r[6].x, r[6].y, r[6].z);
					glVertex3f(r[3].x, r[3].y, r[3].z);
					glVertex3f(r[7].x, r[7].y, r[7].z);
				}
				glEnd();
				break;
			default:
				assert(false);
			}
		}
	}

	glPopAttrib();
}

//-----------------------------------------------------------------------------

void CGLModel::RenderTransparentMaterial(CGLContext& rc, FEModel* ps, int m)
{
	FEMaterial* pmat = ps->GetMaterial(m);
	FEMesh* pm = ps->GetMesh();

	// get the camera's orientation
	quat4f q = rc.m_pview->GetCamera().GetOrientation();

	// make sure a part with this material exists
	if (m >= pm->Parts()) return;

	// set the material properties
	bool benable = false;
	if (pmat->benable && m_pcol->IsActive())
	{
		benable = true;
		glEnable(GL_TEXTURE_1D);
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
		GLubyte a = (GLubyte) (255.f*pmat->transparency);
		glColor4ub(255,255,255,a);
		m_pcol->GetColorMap()->GetTexture().MakeCurrent();
	}
	else
	{
		glDisable(GL_TEXTURE_1D);
		SetMaterialParams(pmat);
	}

	// see if we allow the model to be clipped
	glPushAttrib(GL_ENABLE_BIT);
	if (pmat->bclip == false) CGLPlaneCutPlot::DisableClipPlanes();

	bool bnode = m_pcol->DisplayNodalValues();

	// set the rendering mode
	int nmode = m_nrender;
	if (pmat->m_nrender != RENDER_MODE_DEFAULT) nmode = pmat->m_nrender;
	if (nmode == RENDER_MODE_WIRE) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render the unselected faces
	int i;
	FEDomain& dom = pm->Domain(m);
	int NF = dom.Faces();

	// for better transparency we first draw all the backfacing polygons.
	glPushAttrib(GL_ENABLE_BIT);
	glEnable(GL_CULL_FACE);

	GLCOLOR d = pmat->diffuse;
	GLCOLOR c[4];
	double tm = pmat->transparency;

	glCullFace(GL_FRONT);
	for ( i=0; i<NF; ++i)
	{
		FEFace& face = dom.Face(i);
		FEElement& el = pm->Element(face.m_elem[0]);

		if (!el.IsSelected() && face.IsVisible())
		{
			GLubyte a[4];
			if (m_bsmooth)
			{
				for (int j=0; j<face.Nodes(); ++j)
				{
					vec3f r = face.m_nn[j];
					q.RotateVector(r);
					double z = 1-fabs(r.z);
					a[j] = (GLubyte)(255*(tm + 0.5*(1-tm)*(z*z)));
				}
			}
			else
			{
				vec3f r = face.m_fn;
				q.RotateVector(r);
				double z = 1-fabs(r.z);
				a[0] = a[1] = a[2] = a[3] = (GLubyte)(255*(tm + 0.5*(1-tm)*(z*z)));
			}

			if (benable)
			{
				c[0] = GLCOLOR(255, 255, 255, a[0]);
				c[1] = GLCOLOR(255, 255, 255, a[1]);
				c[2] = GLCOLOR(255, 255, 255, a[2]);
				c[3] = GLCOLOR(255, 255, 255, a[3]);
			}
			else
			{
				c[0] = GLCOLOR(d.r, d.g, d.b, a[0]);
				c[1] = GLCOLOR(d.r, d.g, d.b, a[1]);
				c[2] = GLCOLOR(d.r, d.g, d.b, a[2]);
				c[3] = GLCOLOR(d.r, d.g, d.b, a[3]);
			}

			// okay, we got one, so let's render it
			glLoadName(face.m_elem[0]+1);
			RenderFace(face, pm, c, m_nDivs, bnode);
		}
	}

	// and then we draw the front-facing ones.
	glCullFace(GL_BACK);
	for ( i=0; i<NF; ++i)
	{
		FEFace& face = dom.Face(i);
		FEElement& el = pm->Element(face.m_elem[0]);

		if (!el.IsSelected() && face.IsVisible())
		{
			GLubyte a[4];
			if (m_bsmooth)
			{
				for (int j=0; j<face.Nodes(); ++j)
				{
					vec3f r = face.m_nn[j];
					q.RotateVector(r);
					double z = 1-fabs(r.z);
					a[j] = (GLubyte)(255*(tm + 0.5*(1-tm)*(z*z)));
				}
			}
			else
			{
				vec3f r = face.m_fn;
				q.RotateVector(r);
				double z = 1-fabs(r.z);
				a[0] = a[1] = a[2] = a[3] = (GLubyte)(255*(tm + 0.5*(1-tm)*(z*z)));
			}

			if (benable)
			{
				c[0] = GLCOLOR(255, 255, 255, a[0]);
				c[1] = GLCOLOR(255, 255, 255, a[1]);
				c[2] = GLCOLOR(255, 255, 255, a[2]);
				c[3] = GLCOLOR(255, 255, 255, a[3]);
			}
			else
			{
				c[0] = GLCOLOR(d.r, d.g, d.b, a[0]);
				c[1] = GLCOLOR(d.r, d.g, d.b, a[1]);
				c[2] = GLCOLOR(d.r, d.g, d.b, a[2]);
				c[3] = GLCOLOR(d.r, d.g, d.b, a[3]);
			}

			// okay, we got one, so let's render it
			glLoadName(face.m_elem[0]+1);
			RenderFace(face, pm, c, m_nDivs, bnode);
		}
	}
	glPopAttrib();

	// reset the polygon mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// render the selected elements
	int NE = dom.Elements();
	for (i=0; i<NE; ++i)
	{
		FEElement& el = dom.Element(i);
		if (el.IsVisible() && !el.IsSelected())
		{
			glLoadName(i+1);
			RenderElement(el, pm);
		}
	}

	if (pmat->benable && m_pcol->IsActive())
	{
		glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	}

	glPopAttrib();
}

//-----------------------------------------------------------------------------

void CGLModel::RenderMaterial(FEModel* ps, int m)
{
	FEMaterial* pmat = ps->GetMaterial(m);
	FEMesh* pm = ps->GetMesh();

	// make sure a part with this material exists
	if (m >= pm->Domains()) return;

	// set the material properties
	bool btex = false;
	if (pmat->benable && m_pcol->IsActive())
	{
		btex = true;
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
		GLubyte a = (GLubyte) (255.f*pmat->transparency);
		glColor4ub(255,255,255,a);
		m_pcol->GetColorMap()->GetTexture().MakeCurrent();
	}
	else
	{
		btex = false;
		SetMaterialParams(pmat);
	}

	// see if we allow the model to be clipped
	glPushAttrib(GL_ENABLE_BIT);
	if (pmat->bclip == false) CGLPlaneCutPlot::DisableClipPlanes();

	bool bnode = m_pcol->DisplayNodalValues();

	// set the rendering mode
	int nmode = m_nrender;
	if (pmat->m_nrender != RENDER_MODE_DEFAULT) nmode = pmat->m_nrender;
	if (nmode == RENDER_MODE_WIRE) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render the unselected faces
	int i;
	FEDomain& dom = pm->Domain(m);
	int NF = dom.Faces();

	if (pmat->transparency > .999f)
	{
		if (btex) glEnable(GL_TEXTURE_1D);
		// render active faces
		for ( i=0; i<NF; ++i)
		{
			FEFace& face = dom.Face(i);
			FEElement& el = pm->Element(face.m_elem[0]);
			if (!el.IsSelected() && face.IsVisible() && face.IsActive())
			{
				// okay, we got one, so let's render it
				glLoadName(face.m_elem[0]+1);
				RenderFace(face, pm, m_nDivs, bnode);
			}
		}

		glDisable(GL_TEXTURE_1D);
		// render inactive faces
		if (m_pcol->IsActive()) glColor3ub(200, 200, 200);
		for ( i=0; i<NF; ++i)
		{
			FEFace& face = dom.Face(i);
			FEElement& el = pm->Element(face.m_elem[0]);
			if (!el.IsSelected() && face.IsVisible() && !face.IsActive())
			{
				// okay, we got one, so let's render it
				glLoadName(face.m_elem[0]+1);
				RenderFace(face, pm, m_nDivs, bnode);
			}
		}

		if (btex) glEnable(GL_TEXTURE_1D);
	}
	else
	{
		// for better transparency we first draw all the backfacing polygons.
		glPushAttrib(GL_ENABLE_BIT);
		glEnable(GL_CULL_FACE);

		glCullFace(GL_FRONT);
		for ( i=0; i<NF; ++i)
		{
			FEFace& face = dom.Face(i);
			FEElement& el = pm->Element(face.m_elem[0]);

			if (!el.IsSelected() && face.IsVisible())
			{
				// okay, we got one, so let's render it
				glLoadName(face.m_elem[0]+1);
				RenderFace(face, pm, m_nDivs, bnode);
			}
		}

		// and then we draw the front-facing ones.
		glCullFace(GL_BACK);
		for ( i=0; i<NF; ++i)
		{
			FEFace& face = dom.Face(i);
			FEElement& el = pm->Element(face.m_elem[0]);

			if (!el.IsSelected() && face.IsVisible())
			{
				// okay, we got one, so let's render it
				glLoadName(face.m_elem[0]+1);
				RenderFace(face, pm, m_nDivs, bnode);
			}
		}

		glPopAttrib();
	}

	// reset the polygon mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// render the unselected elements
	int NE = dom.Elements();
	if (btex) glColor3ub(255,255,255);
	for (i=0; i<NE; ++i)
	{
		FEElement& el = dom.Element(i);
		if (el.IsVisible() && !el.IsSelected())
		{
			glLoadName(i+1);
			RenderElement(el, pm);
		}
	}

	if (pmat->benable && m_pcol->IsActive())
	{
		glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	}

	glPopAttrib();
}

//-----------------------------------------------------------------------------
// This algorithm is identical to the RenderOutline, except that it uses the
// original coordinates instead of the current ones
void CGLModel::RenderGhost(CGLContext &rc)
{
	int i, j, n;
	int a, b;
	vec3f r1, r2;

	FEModel* ps = m_ps;
	FEMesh* pm = ps->GetMesh();

	glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glColor3ub(96,96,96);

	quat4f q = rc.m_pview->GetCamera().GetOrientation();

	double eps = cos(pm->GetSmoothingAngleRadians());

	for (i=0; i<pm->Faces(); ++i)
	{
		FEFace& f = pm->Face(i);
		if (f.IsVisible())
		{
			n = f.Edges();
			for (j=0; j<n; ++j)
			{
				bool bdraw = false;

				if (f.m_nbr[j] < 0)
				{
					bdraw = true;
				}
				else
				{
					FEFace& f2 = pm->Face(f.m_nbr[j]);
					if (f.m_mat != f2.m_mat)
					{
						bdraw = true;
					}
					else if (f.m_fn*f2.m_fn <= eps)
					{
						bdraw = true;
					}
					else
					{
						vec3f n1 = f.m_fn;
						vec3f n2 = f2.m_fn;
						q.RotateVector(n1);
						q.RotateVector(n2);
						if (n1.z*n2.z <= 0) 
						{
							bdraw = true;
						}
					}
				}

				if (bdraw)
				{
					a = f.node[j];
					b = f.node[(j+1)%n];

					if (a > b) { a ^= b; b ^= a; a ^= b; }

					r1 = pm->Node(a).m_r0;
					r2 = pm->Node(b).m_r0;

					glBegin(GL_LINES);
					{
						glVertex3f(r1.x, r1.y, r1.z);
						glVertex3f(r2.x, r2.y, r2.z);
					}
					glEnd();
				}
			}
		}
	}

	glPopAttrib();
}

//-----------------------------------------------------------------------------
// NOTE: This algorithm does not always give satisfactory results. 
// In the case of perspective projection, the normal product should 
// be less than some value depending on the location of the edge, 
// in stead of zero (which is the correct value for ortho projection).

void CGLModel::RenderOutline(CGLContext& rc)
{
	int i, j, n;
	int a, b;
	vec3f r1, r2, r3;

	FEModel* ps = m_ps;
	FEMesh* pm = ps->GetMesh();

	glPushAttrib(GL_ENABLE_BIT);

	glDisable(GL_LIGHTING);

	GLCOLOR c = m_line_col;
	glColor3ub(c.r,c.g,c.b);

	quat4f q = rc.m_pview->GetCamera().GetOrientation();

	double eps = cos(pm->GetSmoothingAngleRadians());

	for (i=0; i<pm->Faces(); ++i)
	{
		FEFace& f = pm->Face(i);
		if (f.IsVisible())
		{
			n = f.Edges();
			for (j=0; j<n; ++j)
			{
				bool bdraw = false;

				if (f.m_nbr[j] < 0)
				{
					bdraw = true;
				}
				else
				{
					FEFace& f2 = pm->Face(f.m_nbr[j]);
					if (f.m_mat != f2.m_mat)
					{
						bdraw = true;
					}
					else if (f.m_fn*f2.m_fn <= eps)
					{
						bdraw = true;
					}
/*					else
					{
						vec3f n1 = f.m_fn;
						vec3f n2 = f2.m_fn;
						q.RotateVector(n1);
						q.RotateVector(n2);
						if (n1.z*n2.z <= 0) 
						{
							bdraw = true;
						}
					}
*/				}

				if (bdraw)
				{
					a = f.node[j];
					b = f.node[(j+1)%n];

					if (a > b) { a ^= b; b ^= a; a ^= b; }

					switch (f.m_ntype)
					{
					case FACE_TRI3:
					case FACE_QUAD4:
						{
							r1 = pm->Node(a).m_rt;
							r2 = pm->Node(b).m_rt;

							glBegin(GL_LINES);
							{
								glVertex3f(r1.x, r1.y, r1.z);
								glVertex3f(r2.x, r2.y, r2.z);
							}
							glEnd();
						}
						break;
					case FACE_QUAD8:
					case FACE_QUAD9:
						{
							r1 = pm->Node(a).m_rt;
							r2 = pm->Node(b).m_rt;
							r3 = pm->Node(f.node[j+4]).m_rt;

							glBegin(GL_LINES);
							{
								float r, H[3];
								vec3f p;
								int n = (m_nDivs<=1?2:m_nDivs);
								for (int i=0; i<n; ++i)
								{
									r = -1.f + 2.f*i/n;
									H[0] = 0.5f*r*(r - 1.f);
									H[1] = 0.5f*r*(r + 1.f);
									H[2] = 1.f - r*r;
									p = r1*H[0] + r2*H[1] + r3*H[2];
									glVertex3f(p.x, p.y, p.z); 

									r = -1.f + 2.f*(i+1)/n;
									H[0] = 0.5f*r*(r - 1.f);
									H[1] = 0.5f*r*(r + 1.f);
									H[2] = 1.f - r*r;
									p = r1*H[0] + r2*H[1] + r3*H[2];
									glVertex3f(p.x, p.y, p.z); 
								}
							}
							glEnd();						
						}
						break;
					case FACE_TRI6:
					case FACE_TRI7:
						{
							r1 = pm->Node(a).m_rt;
							r2 = pm->Node(b).m_rt;
							r3 = pm->Node(f.node[j+3]).m_rt;

							glBegin(GL_LINES);
							{
								float r, H[3];
								vec3f p;
								int n = (m_nDivs<=1?2:m_nDivs);
								for (int i=0; i<n; ++i)
								{
									r = -1.f + 2.f*i/n;
									H[0] = 0.5f*r*(r - 1.f);
									H[1] = 0.5f*r*(r + 1.f);
									H[2] = 1.f - r*r;
									p = r1*H[0] + r2*H[1] + r3*H[2];
									glVertex3f(p.x, p.y, p.z); 

									r = -1.f + 2.f*(i+1)/n;
									H[0] = 0.5f*r*(r - 1.f);
									H[1] = 0.5f*r*(r + 1.f);
									H[2] = 1.f - r*r;
									p = r1*H[0] + r2*H[1] + r3*H[2];
									glVertex3f(p.x, p.y, p.z); 
								}
							}
							glEnd();
						}
						break;
					default:
						assert(false);
					}
				}
			}
		}
	}

	glPopAttrib();
}


///////////////////////////////////////////////////////////////////////////////

void CGLModel::RenderNormals(CGLContext& rc)
{
	int i, j, n;

	// get the mesh
	FEModel* ps = m_ps;
	FEMesh* pm = ps->GetMesh();

	BOUNDINGBOX box = ps->GetBoundingBox();

	float scale = 0.05f*box.Radius();

	// store the attributes
	glPushAttrib(GL_ENABLE_BIT);

	// disable lighting
	glDisable(GL_LIGHTING);

	GLfloat r, g, b;

	glBegin(GL_LINES);
	{
		// render the normals
		for (i=0; i<pm->Faces(); ++i)
		{	
			FEFace& face = pm->Face(i);

			// see if it is visible
			if (face.IsVisible())
			{
				vec3f r1(0,0,0);

				n = face.Nodes();
				for (j=0; j<n; ++j) r1 += pm->Node(face.node[j]).m_rt;
				r1 /= (float) n;

				r = (GLfloat) fabs(face.m_fn.x);
				g = (GLfloat) fabs(face.m_fn.y);
				b = (GLfloat) fabs(face.m_fn.z);

				vec3f r2 = r1 + face.m_fn*scale;

				glColor3ub(255,255,255); glVertex3f(r1.x, r1.y, r1.z);
				glColor3f(r, g, b); glVertex3f(r2.x, r2.y, r2.z);
			}
		}
	}
	glEnd();

	// restore attributes
	glPopAttrib();
}


//-----------------------------------------------------------------------------
// Render an element. This function will only render a face of an element
// whose neighbor is not visible. That is, it only renders interior faces.
// 
void CGLModel::RenderElement(FEElement& el, FEMesh* pm)
{
	if ((el.m_ntype == FE_TRUSS2) || (el.m_ntype == FE_TRUSS3))
	{
		vec3f r1 = pm->Node(el.m_node[0]).m_rt;
		vec3f r2 = pm->Node(el.m_node[1]).m_rt;

		float t1 = pm->Node(el.m_node[0]).m_tex;
		float t2 = pm->Node(el.m_node[1]).m_tex;

		glNormal3f(1,1,1);
		glBegin(GL_LINES);
		{
			glTexCoord1f(t1); glVertex3f(r1.x, r1.y, r1.z);
			glTexCoord1f(t2); glVertex3f(r2.x, r2.y, r2.z);
		}
		glEnd();
	}
    else
	{
		for (int i=0; i<el.Faces(); ++i)
		{
			if (el.m_pElem[i] && (el.m_pElem[i]->IsSelected() || !el.m_pElem[i]->IsVisible()))
			{
				FEFace face = el.GetFace(i);

				switch (face.m_ntype)
				{
				case FACE_QUAD4:
					glBegin(GL_QUADS);
					{
						vec3f r1 = pm->Node( face.node[0] ).m_rt;
						vec3f r2 = pm->Node( face.node[1] ).m_rt;
						vec3f r3 = pm->Node( face.node[2] ).m_rt;
						vec3f r4 = pm->Node( face.node[3] ).m_rt;

						vec3f nf = (r2-r1)^(r3-r1);
						nf.Normalize();
						glNormal3f(nf.x, nf.y, nf.z);

						float t1 = pm->Node(face.node[0]).m_tex;
						float t2 = pm->Node(face.node[1]).m_tex;
						float t3 = pm->Node(face.node[2]).m_tex;
						float t4 = pm->Node(face.node[3]).m_tex;

						glTexCoord1f(t1); glVertex3f(r1.x, r1.y, r1.z);
						glTexCoord1f(t2); glVertex3f(r2.x, r2.y, r2.z);
						glTexCoord1f(t3); glVertex3f(r3.x, r3.y, r3.z);
						glTexCoord1f(t4); glVertex3f(r4.x, r4.y, r4.z);
					}
					glEnd();
					break;
				case FACE_QUAD8:
				case FACE_QUAD9:
					glBegin(GL_TRIANGLES);
					{
						vec3f r1 = pm->Node( face.node[0] ).m_rt;
						vec3f r2 = pm->Node( face.node[1] ).m_rt;
						vec3f r3 = pm->Node( face.node[2] ).m_rt;
						vec3f r4 = pm->Node( face.node[3] ).m_rt;
						vec3f r5 = pm->Node( face.node[4] ).m_rt;
						vec3f r6 = pm->Node( face.node[5] ).m_rt;
						vec3f r7 = pm->Node( face.node[6] ).m_rt;
						vec3f r8 = pm->Node( face.node[7] ).m_rt;

						vec3f nf = (r2-r1)^(r3-r1);
						nf.Normalize();
						glNormal3f(nf.x, nf.y, nf.z);

						float t1 = pm->Node(face.node[0]).m_tex;
						float t2 = pm->Node(face.node[1]).m_tex;
						float t3 = pm->Node(face.node[2]).m_tex;
						float t4 = pm->Node(face.node[3]).m_tex;
						float t5 = pm->Node(face.node[4]).m_tex;
						float t6 = pm->Node(face.node[5]).m_tex;
						float t7 = pm->Node(face.node[6]).m_tex;
						float t8 = pm->Node(face.node[7]).m_tex;

						glTexCoord1f(t8); glVertex3f(r8.x, r8.y, r8.z);
						glTexCoord1f(t1); glVertex3f(r1.x, r1.y, r1.z);
						glTexCoord1f(t5); glVertex3f(r5.x, r5.y, r5.z);

						glTexCoord1f(t5); glVertex3f(r5.x, r5.y, r5.z);
						glTexCoord1f(t2); glVertex3f(r2.x, r2.y, r2.z);
						glTexCoord1f(t6); glVertex3f(r6.x, r6.y, r6.z);

						glTexCoord1f(t6); glVertex3f(r6.x, r6.y, r6.z);
						glTexCoord1f(t3); glVertex3f(r3.x, r3.y, r3.z);
						glTexCoord1f(t7); glVertex3f(r7.x, r7.y, r7.z);

						glTexCoord1f(t7); glVertex3f(r7.x, r7.y, r7.z);
						glTexCoord1f(t4); glVertex3f(r4.x, r4.y, r4.z);
						glTexCoord1f(t8); glVertex3f(r8.x, r8.y, r8.z);

						glTexCoord1f(t8); glVertex3f(r8.x, r8.y, r8.z);
						glTexCoord1f(t5); glVertex3f(r5.x, r5.y, r5.z);
						glTexCoord1f(t6); glVertex3f(r6.x, r6.y, r6.z);

						glTexCoord1f(t8); glVertex3f(r8.x, r8.y, r8.z);
						glTexCoord1f(t6); glVertex3f(r6.x, r6.y, r6.z);
						glTexCoord1f(t7); glVertex3f(r7.x, r7.y, r7.z);
					}
					glEnd();
					break;
				case FACE_TRI3:
					glBegin(GL_TRIANGLES);
					{
						vec3f r1 = pm->Node( face.node[0] ).m_rt;
						vec3f r2 = pm->Node( face.node[1] ).m_rt;
						vec3f r3 = pm->Node( face.node[2] ).m_rt;

						vec3f nf = (r2-r1)^(r3-r1);
						nf.Normalize();
						glNormal3f(nf.x, nf.y, nf.z);

						float t1 = pm->Node(face.node[0]).m_tex;
						float t2 = pm->Node(face.node[1]).m_tex;
						float t3 = pm->Node(face.node[2]).m_tex;

						glTexCoord1f(t1); glVertex3f(r1.x, r1.y, r1.z);
						glTexCoord1f(t2); glVertex3f(r2.x, r2.y, r2.z);
						glTexCoord1f(t3); glVertex3f(r3.x, r3.y, r3.z);
					}
					glEnd();
					break;
				case FACE_TRI6:
					glBegin(GL_TRIANGLES);
					{
						vec3f r1 = pm->Node( face.node[0] ).m_rt;
						vec3f r2 = pm->Node( face.node[1] ).m_rt;
						vec3f r3 = pm->Node( face.node[2] ).m_rt;
						vec3f r4 = pm->Node( face.node[3] ).m_rt;
						vec3f r5 = pm->Node( face.node[4] ).m_rt;
						vec3f r6 = pm->Node( face.node[5] ).m_rt;

						vec3f nf = (r2-r1)^(r3-r1);
						nf.Normalize();
						glNormal3f(nf.x, nf.y, nf.z);

						float t1 = pm->Node(face.node[0]).m_tex;
						float t2 = pm->Node(face.node[1]).m_tex;
						float t3 = pm->Node(face.node[2]).m_tex;
						float t4 = pm->Node(face.node[3]).m_tex;
						float t5 = pm->Node(face.node[4]).m_tex;
						float t6 = pm->Node(face.node[5]).m_tex;

						glTexCoord1f(t1); glVertex3f(r1.x, r1.y, r1.z);
						glTexCoord1f(t4); glVertex3f(r4.x, r4.y, r4.z);
						glTexCoord1f(t6); glVertex3f(r6.x, r6.y, r6.z);

						glTexCoord1f(t2); glVertex3f(r2.x, r2.y, r2.z);
						glTexCoord1f(t5); glVertex3f(r5.x, r5.y, r5.z);
						glTexCoord1f(t4); glVertex3f(r4.x, r4.y, r4.z);

						glTexCoord1f(t3); glVertex3f(r3.x, r3.y, r3.z);
						glTexCoord1f(t6); glVertex3f(r6.x, r6.y, r6.z);
						glTexCoord1f(t5); glVertex3f(r5.x, r5.y, r5.z);

						glTexCoord1f(t4); glVertex3f(r4.x, r4.y, r4.z);
						glTexCoord1f(t5); glVertex3f(r5.x, r5.y, r5.z);
						glTexCoord1f(t6); glVertex3f(r6.x, r6.y, r6.z);
					}
					glEnd();
					break;
				case FACE_TRI7:
					glBegin(GL_TRIANGLES);
					{
						vec3f r1 = pm->Node( face.node[0] ).m_rt;
						vec3f r2 = pm->Node( face.node[1] ).m_rt;
						vec3f r3 = pm->Node( face.node[2] ).m_rt;
						vec3f r4 = pm->Node( face.node[3] ).m_rt;
						vec3f r5 = pm->Node( face.node[4] ).m_rt;
						vec3f r6 = pm->Node( face.node[5] ).m_rt;
						vec3f r7 = pm->Node( face.node[6] ).m_rt;

						vec3f nf = (r2-r1)^(r3-r1);
						nf.Normalize();
						glNormal3f(nf.x, nf.y, nf.z);

						float t1 = pm->Node(face.node[0]).m_tex;
						float t2 = pm->Node(face.node[1]).m_tex;
						float t3 = pm->Node(face.node[2]).m_tex;
						float t4 = pm->Node(face.node[3]).m_tex;
						float t5 = pm->Node(face.node[4]).m_tex;
						float t6 = pm->Node(face.node[5]).m_tex;
						float t7 = pm->Node(face.node[6]).m_tex;

						glTexCoord1f(t1); glVertex3f(r1.x, r1.y, r1.z);
						glTexCoord1f(t4); glVertex3f(r4.x, r4.y, r4.z);
						glTexCoord1f(t7); glVertex3f(r7.x, r7.y, r7.z);

						glTexCoord1f(t2); glVertex3f(r2.x, r2.y, r2.z);
						glTexCoord1f(t7); glVertex3f(r7.x, r7.y, r7.z);
						glTexCoord1f(t4); glVertex3f(r4.x, r4.y, r4.z);

						glTexCoord1f(t2); glVertex3f(r2.x, r2.y, r2.z);
						glTexCoord1f(t5); glVertex3f(r5.x, r5.y, r5.z);
						glTexCoord1f(t7); glVertex3f(r7.x, r7.y, r7.z);

						glTexCoord1f(t3); glVertex3f(r3.x, r3.y, r3.z);
						glTexCoord1f(t7); glVertex3f(r7.x, r7.y, r7.z);
						glTexCoord1f(t5); glVertex3f(r5.x, r5.y, r5.z);

						glTexCoord1f(t3); glVertex3f(r3.x, r3.y, r3.z);
						glTexCoord1f(t6); glVertex3f(r6.x, r6.y, r6.z);
						glTexCoord1f(t7); glVertex3f(r7.x, r7.y, r7.z);

						glTexCoord1f(t1); glVertex3f(r1.x, r1.y, r1.z);
						glTexCoord1f(t7); glVertex3f(r7.x, r7.y, r7.z);
						glTexCoord1f(t6); glVertex3f(r6.x, r6.y, r6.z);
					}
					glEnd();
					break;				
				default:
					assert(false);
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Render the mesh lines for a specific material
//
void CGLModel::RenderMeshLines(FEModel* ps, int nmat)
{
	// get the material
	FEMaterial* pmat = ps->GetMaterial(nmat);

	// set the material properties
	GLCOLOR c = pmat->meshcol;
	glColor3ub(c.r, c.g, c.b);

	// get the mesh
	FEMesh* pm = ps->GetMesh();

	// now loop over all faces and see which face belongs to this material
	for (int i=0; i<pm->Faces(); ++i)
	{
		FEFace& face = pm->Face(i);
		if ((face.m_mat == nmat) && (face.IsVisible()))
		{
			// okay, we got one, so let's render it
			RenderFaceOutline(face, pm, m_nDivs);
		}
	}

	// draw elements
	for (int i=0; i<pm->Elements(); ++i)
	{
		FEElement& el = pm->Element(i);
		if ((el.m_MatID == nmat) && el.IsVisible())
		{
			int nf = el.Faces();
			for (int j=0; j<nf; ++j)
			{
				if (el.m_pElem[j] && !el.m_pElem[j]->IsVisible())
				{
					FEFace f = el.GetFace(j);
					RenderFaceOutline(f, pm, m_nDivs);
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Render the mesh lines of the model.

void CGLModel::RenderMeshLines(FEModel* ps)
{
	// store attributes
	glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT);

	glDisable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// we render the mesh by looping over the materials
	// first we draw the materials that are not transparant
	for (int m=0; m<ps->Materials(); ++m)
	{
		// get the material
		FEMaterial* pmat = ps->GetMaterial(m);

		if (pmat->bclip == false) CGLPlaneCutPlot::DisableClipPlanes();

		// make sure the material is visible
		if (pmat->bvisible && pmat->bmesh) RenderMeshLines(ps, m);

		CGLPlaneCutPlot::EnableClipPlanes();
	}

	// restore attributes
	glPopAttrib();
}

///////////////////////////////////////////////////////////////////////////////

void CGLModel::RenderShadows(FEModel* ps, vec3f n, float inf)
{
	FEMesh* pm = ps->GetMesh();

	// find all silhouette edges
	vec3f fn, fn2;
	n.Normalize();
	int m;
	bool bvalid;
	for (int i=0; i<pm->Faces(); i++)
	{
		FEFace& f = pm->Face(i);

		m = f.Edges();
		bvalid = true;
		if (f.node[0] == f.node[1]) bvalid = false;
		else if (f.node[0] == f.node[2]) bvalid = false;
		else if (f.node[1] == f.node[2]) bvalid = false;

		FEElement& el = pm->Element(f.m_elem[0]);
		FEMaterial* pmat = ps->GetMaterial(el.m_MatID);

		// see it this face is visible
		if (!f.IsVisible() || !pmat->bvisible) bvalid = false;

		// make sure this material casts shadows
		if (pmat->bcast_shadows == false) bvalid = false;

		if (bvalid)
		{
			// only look at front facing faces
			fn = f.m_fn;
			if (fn*n > 0)
			{
				for (int j=0; j<m; j++)
				{
					FEFace* pf2 = 0;
					if (f.m_nbr[j] >= 0) pf2 = &pm->Face(f.m_nbr[j]);

					if (pf2)
					{
						fn2 = pf2->m_fn;
					}

					// we got one!
					if ((pf2 == 0) || (fn2*n < 0))
					{
						vec3f a, b, c, d;
						a = pm->Node(f.node[j]).m_rt;
						b = pm->Node(f.node[(j+1)%m]).m_rt;
	
						c = a - n*inf;
						d = b - n*inf;

						glBegin(GL_QUADS);
						{
							vec3f n = (c-a)^(d-a);
							n.Normalize();

							glNormal3f(n.x, n.y, n.z);
							glVertex3f(a.x, a.y, a.z);
							glVertex3f(c.x, c.y, c.z);
							glVertex3f(d.x, d.y, d.z);
							glVertex3f(b.x, b.y, b.z);
						}
						glEnd();
					}
				}
			}
			else 
			{
				vec3f r1 = pm->Node(f.node[0]).m_rt;
				vec3f r2 = pm->Node(f.node[1]).m_rt;
				vec3f r3 = pm->Node(f.node[2]).m_rt;
				vec3f r4 = pm->Node(f.node[3]).m_rt;
	
				glNormal3f(-fn.x, -fn.y, -fn.z);

				switch (f.m_ntype)
				{
				case FACE_QUAD4:
				case FACE_QUAD8:
				case FACE_QUAD9:
					glBegin(GL_QUADS);
					{
						glVertex3f(r4.x, r4.y, r4.z);
						glVertex3f(r3.x, r3.y, r3.z);
						glVertex3f(r2.x, r2.y, r2.z);
						glVertex3f(r1.x, r1.y, r1.z);
					}
					glEnd();
					break;
				case FACE_TRI3:
				case FACE_TRI6:
				case FACE_TRI7:
					glBegin(GL_TRIANGLES);
					{
						glVertex3f(r3.x, r3.y, r3.z);
						glVertex3f(r2.x, r2.y, r2.z);
						glVertex3f(r1.x, r1.y, r1.z);
					}
					glEnd();
					break;
				default:
					assert(false);
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

void CGLModel::RenderNodes(FEModel* ps, CGLContext& rc)
{
	int i, j;

	FEMesh* pm = ps->GetMesh();

	// store attributes
	glPushAttrib(GL_ENABLE_BIT);

	glDisable(GL_LIGHTING);


	// reset tags and check visibility
/*	for (i=0; i<pm->Nodes(); ++i)
	{
		FENode& n = pm->Node(i);
		if (n.IsVisible() && n.m_bext) n.m_ntag = 1;
		else n.m_ntag = 0;
	}
*/

	// reset tags and check visibility
	for (i=0; i<pm->Nodes(); ++i)
	{
		FENode& n = pm->Node(i);
		n.m_ntag = 0;
	}

	for (i=0; i<pm->Elements(); ++i)
	{
		FEElement& e = pm->Element(i);
		if (e.IsVisible())
		{
			// solid elements
			int nf = e.Faces();
			for (int j=0; j<nf; ++j)
			{
				FEElement* pe = e.m_pElem[j];
				if ((pe == 0) || (pe->IsVisible() == false))
				{
					FEFace f = e.GetFace(j);
					int n = f.Nodes();
					for (int k=0; k<n; ++k) pm->Node(f.node[k]).m_ntag = 1;
				}
			}

			// shell elements
			int ne = e.Edges();
			if (ne > 0)
			{
				int nn = e.Nodes();
				for (int j=0; j<nn; ++j) pm->Node(e.m_node[j]).m_ntag = 1;
			}
		}
	}

	// see if backface-culling is enabled or not
	GLboolean bcull;
	glGetBooleanv(GL_CULL_FACE, &bcull);
	if (bcull)
	{
		quat4f q = rc.m_pview->GetCamera().GetOrientation();
		vec3f f;
		int NF = pm->Faces();
		for (i=0; i<NF; ++i)
		{
			FEFace& face = pm->Face(i);
			int n = face.Nodes();
			for (j=0; j<n; ++j) 
			{
				vec3f f = face.m_nn[j];
				q.RotateVector(f);
				if (f.z < 0) pm->Node(face.node[j]).m_ntag = 0;
			}
		}
	}

	// render all tagged nodes
	for (i=0; i<pm->Nodes(); ++i)
	{
		FENode& node = pm->Node(i);
		if (node.m_ntag)
		{
			// get the nodal coordinate
			vec3f r = node.m_rt;

			// set the color
			GLCOLOR c;
			if (node.IsSelected()) c = m_sel_col;
			else c = m_node_col;
			glColor3ub(c.r,c.g,c.b);

			// render the point
			glLoadName(i+1);
			glBegin(GL_POINTS);
			{
				glVertex3f(r.x, r.y, r.z);
			}
			glEnd();
		}
	}

	// restore attributes
	glPopAttrib();
}

//-----------------------------------------------------------------------------

void CGLModel::RenderFace(FEFace& face, FEMesh* pm, int ndivs, bool bnode)
{
	if (m_bShell2Hex)
	{
		int ntype = pm->Element(face.m_elem[0]).m_ntype;
		if ((ntype == FE_QUAD4) || (ntype == FE_QUAD8) || (ntype == FE_QUAD9) || (ntype == FE_TRI3) || (ntype == FE_TRI6))
		{
			RenderThickShell(face, pm);
			return;
		}
	}

	// Render the facet
	switch (face.m_ntype)
	{
	case FACE_QUAD4:
		if (ndivs <= 1) RenderQUAD4(face, m_bsmooth, bnode);
		else RenderSmoothQUAD4(face, pm, ndivs, bnode);
		break;
	case FACE_QUAD8:
		if (ndivs <= 1) RenderQUAD8(face, m_bsmooth, bnode);
		else RenderSmoothQUAD8(face, pm, ndivs, bnode);
		break;
	case FACE_QUAD9:
		if (ndivs <= 1) RenderQUAD9(face, m_bsmooth, bnode);
		else RenderSmoothQUAD9(face, pm, ndivs, bnode);
		break;
	case FACE_TRI3:
		RenderTRI3(face, m_bsmooth, bnode);
		break;
	case FACE_TRI6:
		if (ndivs <= 1) RenderTRI6(face, m_bsmooth, bnode);
		else RenderSmoothTRI6(face, pm, ndivs, bnode);
		break;
	case FACE_TRI7:
		if (ndivs <= 1) RenderTRI7(face, m_bsmooth, bnode);
		else RenderSmoothTRI7(face, pm, ndivs, bnode);
		break;
	default:
		assert(false);
	}
}

//-----------------------------------------------------------------------------

void CGLModel::RenderFace(FEFace& face, FEMesh* pm, GLCOLOR c[4], int ndivs, bool bnode)
{
	if (m_bShell2Hex)
	{
		int ntype = pm->Element(face.m_elem[0]).m_ntype;
		if ((ntype == FE_QUAD4) || (ntype == FE_QUAD8) || (ntype == FE_QUAD9) || (ntype == FE_TRI3) || (ntype == FE_TRI6))
		{
			RenderThickShell(face, pm);
			return;
		}
	}

	vec3f& r1 = pm->Node(face.node[0]).m_rt;
	vec3f& r2 = pm->Node(face.node[1]).m_rt;
	vec3f& r3 = pm->Node(face.node[2]).m_rt;
	vec3f& r4 = pm->Node(face.node[3]).m_rt;

	vec3f& n1 = face.m_nn[0];
	vec3f& n2 = face.m_nn[1];
	vec3f& n3 = face.m_nn[2];
	vec3f& n4 = face.m_nn[3];

	vec3f& fn = face.m_fn;

	float t1, t2, t3, t4;
	if (bnode)
	{
		t1 = face.m_tex[0];
		t2 = face.m_tex[1];
		t3 = face.m_tex[2];
		t4 = face.m_tex[3];
	}
	else t1 = t2 = t3 = t4 = face.m_texe;

	if (m_bsmooth)
	{
		switch (face.m_ntype)
		{
		case FACE_QUAD4:
		case FACE_QUAD8:
		case FACE_QUAD9:
			if (ndivs <= 1)
			{
				glBegin(GL_QUADS);
				{
					glNormal3f(n1.x, n1.y, n1.z); glColor4ub(c[0].r, c[0].g, c[0].b, c[0].a); glTexCoord1f(t1); glVertex3f(r1.x, r1.y, r1.z);
					glNormal3f(n2.x, n2.y, n2.z); glColor4ub(c[1].r, c[1].g, c[1].b, c[1].a); glTexCoord1f(t2); glVertex3f(r2.x, r2.y, r2.z);
					glNormal3f(n3.x, n3.y, n3.z); glColor4ub(c[2].r, c[2].g, c[2].b, c[2].a); glTexCoord1f(t3); glVertex3f(r3.x, r3.y, r3.z);
					glNormal3f(n4.x, n4.y, n4.z); glColor4ub(c[3].r, c[3].g, c[3].b, c[3].a); glTexCoord1f(t4); glVertex3f(r4.x, r4.y, r4.z);
				}
				glEnd();
			}
			else RenderSmoothQUAD4(face, pm, ndivs, bnode);
			break;
		case FACE_TRI3:
		case FACE_TRI6:
		case FACE_TRI7:
			glBegin(GL_TRIANGLES);
			{
				glNormal3f(n1.x, n1.y, n1.z); glColor4ub(c[0].r, c[0].g, c[0].b, c[0].a); glTexCoord1f(t1); glVertex3f(r1.x, r1.y, r1.z);
				glNormal3f(n2.x, n2.y, n2.z); glColor4ub(c[1].r, c[1].g, c[1].b, c[1].a); glTexCoord1f(t2); glVertex3f(r2.x, r2.y, r2.z);
				glNormal3f(n3.x, n3.y, n3.z); glColor4ub(c[2].r, c[2].g, c[2].b, c[2].a); glTexCoord1f(t3); glVertex3f(r3.x, r3.y, r3.z);
			}
			glEnd();
			break;
		default:
			assert(false);
		}
	}
	else
	{
		glNormal3f(fn.x, fn.y, fn.z);

		switch (face.m_ntype)
		{
		case FACE_QUAD4:
		case FACE_QUAD8:
		case FACE_QUAD9:
			glBegin(GL_QUADS);
			{
				glColor4ub(c[0].r, c[0].g, c[0].b, c[0].a); glTexCoord1f(t1); glVertex3f(r1.x, r1.y, r1.z);
				glColor4ub(c[1].r, c[1].g, c[1].b, c[1].a); glTexCoord1f(t2); glVertex3f(r2.x, r2.y, r2.z);
				glColor4ub(c[2].r, c[2].g, c[2].b, c[2].a); glTexCoord1f(t3); glVertex3f(r3.x, r3.y, r3.z);
				glColor4ub(c[3].r, c[3].g, c[3].b, c[3].a); glTexCoord1f(t4); glVertex3f(r4.x, r4.y, r4.z);
			}
			glEnd();
			break;
		case FACE_TRI3:
		case FACE_TRI6:
		case FACE_TRI7:
			glBegin(GL_TRIANGLES);
			{
				glColor4ub(c[0].r, c[0].g, c[0].b, c[0].a); glTexCoord1f(t1); glVertex3f(r1.x, r1.y, r1.z);
				glColor4ub(c[1].r, c[1].g, c[1].b, c[1].a); glTexCoord1f(t2); glVertex3f(r2.x, r2.y, r2.z);
				glColor4ub(c[2].r, c[2].g, c[2].b, c[2].a); glTexCoord1f(t3); glVertex3f(r3.x, r3.y, r3.z);
			}
			glEnd();
			break;
		default:
			assert(false);
		}
	}
}


//-----------------------------------------------------------------------------

void CGLModel::RenderElementOutline(FEElement& el, FEMesh* pm)
{
	glBegin(GL_LINES);
	{
		switch (el.m_ntype)
		{
		case FE_HEX8:
			{
				int (*et)[2] = ET_HEX;
				for (int i=0; i<12; ++i)
				{
					vec3f& r0 = pm->Node(el.m_node[et[i][0]]).m_rt;
					vec3f& r1 = pm->Node(el.m_node[et[i][1]]).m_rt;

					glVertex3f(r0.x, r0.y, r0.z);
					glVertex3f(r1.x, r1.y, r1.z);
				}
			}
			break;
		case FE_HEX20:
			{
				int (*et)[3] = ET_HEX20;
				for (int i=0; i<12; ++i)
				{
					vec3f& r0 = pm->Node(el.m_node[et[i][0]]).m_rt;
					vec3f& r1 = pm->Node(el.m_node[et[i][1]]).m_rt;
					vec3f& r2 = pm->Node(el.m_node[et[i][2]]).m_rt;

					glVertex3f(r0.x, r0.y, r0.z); glVertex3f(r2.x, r2.y, r2.z);
					glVertex3f(r2.x, r2.y, r2.z); glVertex3f(r1.x, r1.y, r1.z);
				}
			}
			break;
		case FE_HEX27:
			{
				int (*et)[3] = ET_HEX20;
				for (int i=0; i<12; ++i)
				{
					vec3f& r0 = pm->Node(el.m_node[et[i][0]]).m_rt;
					vec3f& r1 = pm->Node(el.m_node[et[i][1]]).m_rt;
					vec3f& r2 = pm->Node(el.m_node[et[i][2]]).m_rt;

					glVertex3f(r0.x, r0.y, r0.z); glVertex3f(r2.x, r2.y, r2.z);
					glVertex3f(r2.x, r2.y, r2.z); glVertex3f(r1.x, r1.y, r1.z);
				}
			}
			break;
		case FE_PENTA6:
			{
				int (*et)[2] = ET_PENTA;
				for (int i=0; i<9; ++i)
				{
					vec3f& r0 = pm->Node(el.m_node[et[i][0]]).m_rt;
					vec3f& r1 = pm->Node(el.m_node[et[i][1]]).m_rt;

					glVertex3f(r0.x, r0.y, r0.z);
					glVertex3f(r1.x, r1.y, r1.z);
				}
			};
			break;
		case FE_TET4:
			{
				int (*et)[2] = ET_TET;
				for (int i=0; i<6; ++i)
				{
					vec3f& r0 = pm->Node(el.m_node[et[i][0]]).m_rt;
					vec3f& r1 = pm->Node(el.m_node[et[i][1]]).m_rt;

					glVertex3f(r0.x, r0.y, r0.z);
					glVertex3f(r1.x, r1.y, r1.z);
				}
			}
			break;
		case FE_TET10:
		case FE_TET15:
			{
				int (*et)[3] = ET_TET10;
				for (int i=0; i<6; ++i)
				{
					vec3f& r0 = pm->Node(el.m_node[et[i][0]]).m_rt;
					vec3f& r1 = pm->Node(el.m_node[et[i][2]]).m_rt;
					vec3f& r2 = pm->Node(el.m_node[et[i][1]]).m_rt;

					glVertex3f(r0.x, r0.y, r0.z); glVertex3f(r1.x, r1.y, r1.z);
					glVertex3f(r1.x, r1.y, r1.z); glVertex3f(r2.x, r2.y, r2.z);
				}
			}
			break;
		}
	}
	glEnd();
}

//-----------------------------------------------------------------------------

void CGLModel::RenderFaceOutline(FEFace& face, FEMesh* pm, int ndivs)
{
	if (m_bShell2Hex)
	{
		int ntype = pm->Element(face.m_elem[0]).m_ntype;
		if ((ntype == FE_QUAD4) || (ntype == FE_QUAD8) || (ntype == FE_QUAD9) || (ntype == FE_TRI3) || (ntype == FE_TRI6))
		{
			RenderThickShellOutline(face, pm);
			return;
		}
	}

	GLboolean btex;
	glGetBooleanv(GL_TEXTURE_1D, &btex);
	glDisable(GL_TEXTURE_1D);

	// render the edges of the fae
	switch (face.m_ntype)
	{
	case FACE_TRI3:
	case FACE_QUAD4:
		{
			glBegin(GL_LINE_LOOP);
			{
				int NF = face.Nodes();
				for (int i=0; i<NF; ++i) 
				{
					vec3f r = pm->Node(face.node[i]).m_rt;
					glVertex3f(r.x, r.y, r.z);
				}
			}
			glEnd();
		}
		break;
	case FACE_TRI6:
	case FACE_TRI7:
	case FACE_QUAD8:
	case FACE_QUAD9:
		{
			vec3f a[3];
			glBegin(GL_LINE_LOOP);
			{
				int NE = face.Edges();
				for (int i=0; i<NE; ++i)
				{
					FEEdge e = face.Edge(i);
					a[0] = pm->Node(e.node[0]).m_rt;
					a[1] = pm->Node(e.node[1]).m_rt;
					a[2] = pm->Node(e.node[2]).m_rt;
					const int M = 2*ndivs;
					for (int n=0; n<M; ++n)
					{
						double t = n / (double) M;
						vec3f p = e.eval(a, t);
						glVertex3f(p.x, p.y, p.z);
					}
				}
			}
			glEnd();
		}
		break;
	default:
		assert(false);
	}

	if (btex) glEnable(GL_TEXTURE_1D);
}

///////////////////////////////////////////////////////////////////////////////
void CGLModel::RenderThickShell(FEFace &face, FEMesh* pm)
{
	switch(face.m_ntype)
	{
	case FACE_QUAD4:
	case FACE_QUAD8:
	case FACE_QUAD9:
		RenderThickQuad(face, pm);
		break;
	case FACE_TRI3:
	case FACE_TRI6:
	case FACE_TRI7:
		RenderThickTri(face, pm);
		break;
	}
}

void CGLModel::RenderThickQuad(FEFace &face, FEMesh* pm)
{
	FEElement& el = pm->Element(face.m_elem[0]);
	FEState* ps = m_ps->GetState(0);
	float* h = ps->m_ELEM[face.m_elem[0]].m_h;

	vec3f r1 = pm->Node(face.node[0]).m_rt;
	vec3f r2 = pm->Node(face.node[1]).m_rt;
	vec3f r3 = pm->Node(face.node[2]).m_rt;
	vec3f r4 = pm->Node(face.node[3]).m_rt;

	vec3f n1 = face.m_nn[0];
	vec3f n2 = face.m_nn[1];
	vec3f n3 = face.m_nn[2];
	vec3f n4 = face.m_nn[3];

	vec3f r1a, r2a, r3a, r4a;
	vec3f r1b, r2b, r3b, r4b;
	
	switch (m_nshellref)
	{
	case 0:	// midsurface
		r1a = r1 - n1*(0.5f*h[0]); r1b = r1 + n1*(0.5f*h[0]);
		r2a = r2 - n2*(0.5f*h[1]); r2b = r2 + n2*(0.5f*h[1]);
		r3a = r3 - n3*(0.5f*h[2]); r3b = r3 + n3*(0.5f*h[2]);
		r4a = r4 - n4*(0.5f*h[3]); r4b = r4 + n4*(0.5f*h[3]);
		break;
	case 1: // top surface
		r1a = r1; r1b = r1 + n1*h[0];
		r2a = r2; r2b = r2 + n2*h[1];
		r3a = r3; r3b = r3 + n3*h[2];
		r4a = r4; r4b = r4 + n4*h[3];
		break;
	case 2: // bottom surface
		r1a = r1 - n1*h[0]; r1b = r1;
		r2a = r2 - n2*h[1]; r2b = r2;
		r3a = r3 - n3*h[2]; r3b = r3;
		r4a = r4 - n4*h[3]; r4b = r4;
		break;
	}

	vec3f m1 = (r2 - r1)^n1; m1.Normalize();
	vec3f m2 = (r3 - r2)^n1; m2.Normalize();
	vec3f m3 = (r4 - r3)^n1; m3.Normalize();
	vec3f m4 = (r1 - r4)^n1; m4.Normalize();

	vec3f fn = face.m_fn;

	float t1 = face.m_tex[0];
	float t2 = face.m_tex[1];
	float t3 = face.m_tex[2];
	float t4 = face.m_tex[3];

	if (m_bsmooth)
	{
		glBegin(GL_QUADS);
		{
			glNormal3f(n1.x, n1.y, n1.z); glTexCoord1f(t1); glVertex3f(r1b.x, r1b.y, r1b.z);
			glNormal3f(n2.x, n2.y, n2.z); glTexCoord1f(t2); glVertex3f(r2b.x, r2b.y, r2b.z);
			glNormal3f(n3.x, n3.y, n3.z); glTexCoord1f(t3); glVertex3f(r3b.x, r3b.y, r3b.z);
			glNormal3f(n4.x, n4.y, n4.z); glTexCoord1f(t4); glVertex3f(r4b.x, r4b.y, r4b.z);

			glNormal3f(-n4.x, -n4.y, -n4.z); glTexCoord1f(t4); glVertex3f(r4a.x, r4a.y, r4a.z);
			glNormal3f(-n3.x, -n3.y, -n3.z); glTexCoord1f(t3); glVertex3f(r3a.x, r3a.y, r3a.z);
			glNormal3f(-n2.x, -n2.y, -n2.z); glTexCoord1f(t2); glVertex3f(r2a.x, r2a.y, r2a.z);
			glNormal3f(-n1.x, -n1.y, -n1.z); glTexCoord1f(t1); glVertex3f(r1a.x, r1a.y, r1a.z);

			if (face.m_nbr[0] == -1)
			{
				glNormal3f(m1.x, m1.y, m1.z); glTexCoord1f(t1); glVertex3f(r1a.x, r1a.y, r1a.z);
				glNormal3f(m1.x, m1.y, m1.z); glTexCoord1f(t2); glVertex3f(r2a.x, r2a.y, r2a.z);
				glNormal3f(m1.x, m1.y, m1.z); glTexCoord1f(t2); glVertex3f(r2b.x, r2b.y, r2b.z);
				glNormal3f(m1.x, m1.y, m1.z); glTexCoord1f(t1); glVertex3f(r1b.x, r1b.y, r1b.z);
			}

			if (face.m_nbr[1] == -1)
			{
				glNormal3f(m2.x, m2.y, m2.z); glTexCoord1f(t2); glVertex3f(r2a.x, r2a.y, r2a.z);
				glNormal3f(m2.x, m2.y, m2.z); glTexCoord1f(t3); glVertex3f(r3a.x, r3a.y, r3a.z);
				glNormal3f(m2.x, m2.y, m2.z); glTexCoord1f(t3); glVertex3f(r3b.x, r3b.y, r3b.z);
				glNormal3f(m2.x, m2.y, m2.z); glTexCoord1f(t2); glVertex3f(r2b.x, r2b.y, r2b.z);
			}

			if (face.m_nbr[2] == -1)
			{
				glNormal3f(m3.x, m3.y, m3.z); glTexCoord1f(t3); glVertex3f(r3a.x, r3a.y, r3a.z);
				glNormal3f(m3.x, m3.y, m3.z); glTexCoord1f(t4); glVertex3f(r4a.x, r4a.y, r4a.z);
				glNormal3f(m3.x, m3.y, m3.z); glTexCoord1f(t4); glVertex3f(r4b.x, r4b.y, r4b.z);
				glNormal3f(m3.x, m3.y, m3.z); glTexCoord1f(t3); glVertex3f(r3b.x, r3b.y, r3b.z);
			}

			if (face.m_nbr[3] == -1)
			{
				glNormal3f(m4.x, m4.y, m4.z); glTexCoord1f(t4); glVertex3f(r4a.x, r4a.y, r4a.z);
				glNormal3f(m4.x, m4.y, m4.z); glTexCoord1f(t1); glVertex3f(r1a.x, r1a.y, r1a.z);
				glNormal3f(m4.x, m4.y, m4.z); glTexCoord1f(t1); glVertex3f(r1b.x, r1b.y, r1b.z);
				glNormal3f(m4.x, m4.y, m4.z); glTexCoord1f(t4); glVertex3f(r4b.x, r4b.y, r4b.z);
			}
		}
		glEnd();
	}
	else
	{
		glNormal3f(fn.x, fn.y, fn.z);

		glBegin(GL_QUADS);
		{
			glTexCoord1f(t1); glVertex3f(r1b.x, r1b.y, r1b.z);
			glTexCoord1f(t2); glVertex3f(r2b.x, r2b.y, r2b.z);
			glTexCoord1f(t3); glVertex3f(r3b.x, r3b.y, r3b.z);
			glTexCoord1f(t4); glVertex3f(r4b.x, r4b.y, r4b.z);

			glTexCoord1f(t4); glVertex3f(r4a.x, r4a.y, r4a.z);
			glTexCoord1f(t3); glVertex3f(r3a.x, r3a.y, r3a.z);
			glTexCoord1f(t2); glVertex3f(r2a.x, r2a.y, r2a.z);
			glTexCoord1f(t1); glVertex3f(r1a.x, r1a.y, r1a.z);

			if (face.m_nbr[0] == -1)
			{
				glTexCoord1f(t1); glVertex3f(r1a.x, r1a.y, r1a.z);
				glTexCoord1f(t2); glVertex3f(r2a.x, r2a.y, r2a.z);
				glTexCoord1f(t2); glVertex3f(r2b.x, r2b.y, r2b.z);
				glTexCoord1f(t1); glVertex3f(r1b.x, r1b.y, r1b.z);
			}

			if (face.m_nbr[1] == -1)
			{
				glTexCoord1f(t2); glVertex3f(r2a.x, r2a.y, r2a.z);
				glTexCoord1f(t3); glVertex3f(r3a.x, r3a.y, r3a.z);
				glTexCoord1f(t3); glVertex3f(r3b.x, r3b.y, r3b.z);
				glTexCoord1f(t2); glVertex3f(r2b.x, r2b.y, r2b.z);
			}

			if (face.m_nbr[2] == -1)
			{
				glTexCoord1f(t3); glVertex3f(r3a.x, r3a.y, r3a.z);
				glTexCoord1f(t4); glVertex3f(r4a.x, r4a.y, r4a.z);
				glTexCoord1f(t4); glVertex3f(r4b.x, r4b.y, r4b.z);
				glTexCoord1f(t3); glVertex3f(r3b.x, r3b.y, r3b.z);
			}

			if (face.m_nbr[3] == -1)
			{
				glTexCoord1f(t4); glVertex3f(r4a.x, r4a.y, r4a.z);
				glTexCoord1f(t1); glVertex3f(r1a.x, r1a.y, r1a.z);
				glTexCoord1f(t1); glVertex3f(r1b.x, r1b.y, r1b.z);
				glTexCoord1f(t4); glVertex3f(r4b.x, r4b.y, r4b.z);
			}
		}
		glEnd();
	}
}

void CGLModel::RenderThickTri(FEFace &face, FEMesh* pm)
{
	FEElement& el = pm->Element(face.m_elem[0]);
	FEState* ps = m_ps->GetState(0);
	float* h = ps->m_ELEM[face.m_elem[0]].m_h;

	vec3f r1 = pm->Node(face.node[0]).m_rt;
	vec3f r2 = pm->Node(face.node[1]).m_rt;
	vec3f r3 = pm->Node(face.node[2]).m_rt;

	//h[0] = (h[0] + h[1] + h[2])/3;
	//h[1] = h[0];
	//h[2] = h[0];
	vec3f n1 = face.m_nn[0];
	vec3f n2 = face.m_nn[1];
	vec3f n3 = face.m_nn[2];

	vec3f r1a, r2a, r3a;
	vec3f r1b, r2b, r3b;

	switch (m_nshellref)
	{
	case 0:	// midsurface
		r1a = r1 - n1*(0.5f*h[0]); r1b = r1 + n1*(0.5f*h[0]);
		r2a = r2 - n2*(0.5f*h[1]); r2b = r2 + n2*(0.5f*h[1]);
		r3a = r3 - n3*(0.5f*h[2]); r3b = r3 + n3*(0.5f*h[2]);
		break;
	case 1: // top surface
		r1a = r1; r1b = r1 + n1*h[0];
		r2a = r2; r2b = r2 + n2*h[1];
		r3a = r3; r3b = r3 + n3*h[2];
		break;
	case 2: // bottom surface
		r1a = r1 - n1*h[0]; r1b = r1;
		r2a = r2 - n2*h[1]; r2b = r2;
		r3a = r3 - n3*h[2]; r3b = r3;
		break;
	}

	vec3f m1 = (r2 - r1) ^ n1; m1.Normalize();
	vec3f m2 = (r3 - r2) ^ n1; m2.Normalize();
	vec3f m3 = (r1 - r3) ^ n1; m3.Normalize();

	vec3f fn = face.m_fn;

//	float t1 = face.m_tex[0];
//	float t2 = face.m_tex[1];
//	float t3 = face.m_tex[2];
	float t1 = pm->Node(face.node[0]).m_tex;
	float t2 = pm->Node(face.node[1]).m_tex;
	float t3 = pm->Node(face.node[2]).m_tex;

	if (m_bsmooth)
	{
		glBegin(GL_TRIANGLES);
		{
			glNormal3f(n1.x, n1.y, n1.z); glTexCoord1f(t1); glVertex3f(r1b.x, r1b.y, r1b.z);
			glNormal3f(n2.x, n2.y, n2.z); glTexCoord1f(t2); glVertex3f(r2b.x, r2b.y, r2b.z);
			glNormal3f(n3.x, n3.y, n3.z); glTexCoord1f(t3); glVertex3f(r3b.x, r3b.y, r3b.z);

			glNormal3f(-n3.x, -n3.y, -n3.z); glTexCoord1f(t3); glVertex3f(r3a.x, r3a.y, r3a.z);
			glNormal3f(-n2.x, -n2.y, -n2.z); glTexCoord1f(t2); glVertex3f(r2a.x, r2a.y, r2a.z);
			glNormal3f(-n1.x, -n1.y, -n1.z); glTexCoord1f(t1); glVertex3f(r1a.x, r1a.y, r1a.z);
		}
		glEnd();

		glBegin(GL_QUADS);
		{
			if (face.m_nbr[0] == -1)
			{
				glNormal3f(m1.x, m1.y, m1.z); glTexCoord1f(t1); glVertex3f(r1a.x, r1a.y, r1a.z);
				glNormal3f(m1.x, m1.y, m1.z); glTexCoord1f(t2); glVertex3f(r2a.x, r2a.y, r2a.z);
				glNormal3f(m1.x, m1.y, m1.z); glTexCoord1f(t2); glVertex3f(r2b.x, r2b.y, r2b.z);
				glNormal3f(m1.x, m1.y, m1.z); glTexCoord1f(t1); glVertex3f(r1b.x, r1b.y, r1b.z);
			}

			if (face.m_nbr[1] == -1)
			{
				glNormal3f(m2.x, m2.y, m2.z); glTexCoord1f(t2); glVertex3f(r2a.x, r2a.y, r2a.z);
				glNormal3f(m2.x, m2.y, m2.z); glTexCoord1f(t3); glVertex3f(r3a.x, r3a.y, r3a.z);
				glNormal3f(m2.x, m2.y, m2.z); glTexCoord1f(t3); glVertex3f(r3b.x, r3b.y, r3b.z);
				glNormal3f(m2.x, m2.y, m2.z); glTexCoord1f(t2); glVertex3f(r2b.x, r2b.y, r2b.z);
			}

			if (face.m_nbr[2] == -1)
			{
				glNormal3f(m3.x, m3.y, m3.z); glTexCoord1f(t3); glVertex3f(r3a.x, r3a.y, r3a.z);
				glNormal3f(m3.x, m3.y, m3.z); glTexCoord1f(t1); glVertex3f(r1a.x, r1a.y, r1a.z);
				glNormal3f(m3.x, m3.y, m3.z); glTexCoord1f(t1); glVertex3f(r1b.x, r1b.y, r1b.z);
				glNormal3f(m3.x, m3.y, m3.z); glTexCoord1f(t3); glVertex3f(r3b.x, r3b.y, r3b.z);
			}
		}
		glEnd();
	}
	else
	{
		glNormal3f(fn.x, fn.y, fn.z);

		glBegin(GL_TRIANGLES);
		{
			glNormal3f(n1.x, n1.y, n1.z); glTexCoord1f(t1); glVertex3f(r1b.x, r1b.y, r1b.z);
			glNormal3f(n2.x, n2.y, n2.z); glTexCoord1f(t2); glVertex3f(r2b.x, r2b.y, r2b.z);
			glNormal3f(n3.x, n3.y, n3.z); glTexCoord1f(t3); glVertex3f(r3b.x, r3b.y, r3b.z);

			glNormal3f(-n3.x, -n3.y, -n3.z); glTexCoord1f(t3); glVertex3f(r3a.x, r3a.y, r3a.z);
			glNormal3f(-n2.x, -n2.y, -n2.z); glTexCoord1f(t2); glVertex3f(r2a.x, r2a.y, r2a.z);
			glNormal3f(-n1.x, -n1.y, -n1.z); glTexCoord1f(t1); glVertex3f(r1a.x, r1a.y, r1a.z);
		}
		glEnd();

		glBegin(GL_QUADS);
		{
			if (face.m_nbr[0] == -1)
			{
				glTexCoord1f(t1); glVertex3f(r1a.x, r1a.y, r1a.z);
				glTexCoord1f(t2); glVertex3f(r2a.x, r2a.y, r2a.z);
				glTexCoord1f(t2); glVertex3f(r2b.x, r2b.y, r2b.z);
				glTexCoord1f(t1); glVertex3f(r1b.x, r1b.y, r1b.z);
			}

			if (face.m_nbr[1] == -1)
			{
				glTexCoord1f(t2); glVertex3f(r2a.x, r2a.y, r2a.z);
				glTexCoord1f(t3); glVertex3f(r3a.x, r3a.y, r3a.z);
				glTexCoord1f(t3); glVertex3f(r3b.x, r3b.y, r3b.z);
				glTexCoord1f(t2); glVertex3f(r2b.x, r2b.y, r2b.z);
			}

			if (face.m_nbr[2] == -1)
			{
				glTexCoord1f(t3); glVertex3f(r3a.x, r3a.y, r3a.z);
				glTexCoord1f(t1); glVertex3f(r1a.x, r1a.y, r1a.z);
				glTexCoord1f(t1); glVertex3f(r1b.x, r1b.y, r1b.z);
				glTexCoord1f(t3); glVertex3f(r3b.x, r3b.y, r3b.z);
			}
		}
		glEnd();
	}
}


///////////////////////////////////////////////////////////////////////////////

void CGLModel::RenderThickShellOutline(FEFace &face, FEMesh* pm)
{
	FEElement& el = pm->Element(face.m_elem[0]);
	FEState* ps = m_ps->GetState(0);
	float* h = ps->m_ELEM[face.m_elem[0]].m_h;

	vec3f r1 = pm->Node(face.node[0]).m_rt;
	vec3f r2 = pm->Node(face.node[1]).m_rt;
	vec3f r3 = pm->Node(face.node[2]).m_rt;
	vec3f r4 = pm->Node(face.node[3]).m_rt;

	vec3f n1 = face.m_nn[0];
	vec3f n2 = face.m_nn[1];
	vec3f n3 = face.m_nn[2];
	vec3f n4 = face.m_nn[3];

	vec3f r1a, r2a, r3a, r4a;
	vec3f r1b, r2b, r3b, r4b;
	
	switch (m_nshellref)
	{
	case 0:	// midsurface
		r1a = r1 - n1*(0.5f*h[0]); r1b = r1 + n1*(0.5f*h[0]);
		r2a = r2 - n2*(0.5f*h[1]); r2b = r2 + n2*(0.5f*h[1]);
		r3a = r3 - n3*(0.5f*h[2]); r3b = r3 + n3*(0.5f*h[2]);
		r4a = r4 - n4*(0.5f*h[3]); r4b = r4 + n4*(0.5f*h[3]);
		break;
	case 1: // top surface
		r1a = r1; r1b = r1 + n1*h[0];
		r2a = r2; r2b = r2 + n2*h[1];
		r3a = r3; r3b = r3 + n3*h[2];
		r4a = r4; r4b = r4 + n4*h[3];
		break;
	case 2: // bottom surface
		r1a = r1 - n1*h[0]; r1b = r1;
		r2a = r2 - n2*h[1]; r2b = r2;
		r3a = r3 - n3*h[2]; r3b = r3;
		r4a = r4 - n4*h[3]; r4b = r4;
		break;
	}

	GLboolean btex;
	glGetBooleanv(GL_TEXTURE_1D, &btex);
	glDisable(GL_TEXTURE_1D);

	switch (face.m_ntype)
	{
	case FACE_QUAD4:
	case FACE_QUAD8:
	case FACE_QUAD9:
		glBegin(GL_LINES);
		{
			glVertex3f(r1a.x, r1a.y, r1a.z); glVertex3f(r2a.x, r2a.y, r2a.z);
			glVertex3f(r2a.x, r2a.y, r2a.z); glVertex3f(r3a.x, r3a.y, r3a.z);
			glVertex3f(r3a.x, r3a.y, r3a.z); glVertex3f(r4a.x, r4a.y, r4a.z);
			glVertex3f(r4a.x, r4a.y, r4a.z); glVertex3f(r1a.x, r1a.y, r1a.z);

			glVertex3f(r1b.x, r1b.y, r1b.z); glVertex3f(r2b.x, r2b.y, r2b.z);
			glVertex3f(r2b.x, r2b.y, r2b.z); glVertex3f(r3b.x, r3b.y, r3b.z);
			glVertex3f(r3b.x, r3b.y, r3b.z); glVertex3f(r4b.x, r4b.y, r4b.z);
			glVertex3f(r4b.x, r4b.y, r4b.z); glVertex3f(r1b.x, r1b.y, r1b.z);

			glVertex3f(r1a.x, r1a.y, r1a.z); glVertex3f(r1b.x, r1b.y, r1b.z);
			glVertex3f(r2a.x, r2a.y, r2a.z); glVertex3f(r2b.x, r2b.y, r2b.z);
			glVertex3f(r3a.x, r3a.y, r3a.z); glVertex3f(r3b.x, r3b.y, r3b.z);
			glVertex3f(r4a.x, r4a.y, r4a.z); glVertex3f(r4b.x, r4b.y, r4b.z);
		}
		glEnd();
		break;
	case FACE_TRI3:
	case FACE_TRI6:
	case FACE_TRI7:
		glBegin(GL_LINES);
		{
			glVertex3f(r1a.x, r1a.y, r1a.z); glVertex3f(r2a.x, r2a.y, r2a.z);
			glVertex3f(r2a.x, r2a.y, r2a.z); glVertex3f(r3a.x, r3a.y, r3a.z);
			glVertex3f(r3a.x, r3a.y, r3a.z); glVertex3f(r1a.x, r1a.y, r1a.z);

			glVertex3f(r1b.x, r1b.y, r1b.z); glVertex3f(r2b.x, r2b.y, r2b.z);
			glVertex3f(r2b.x, r2b.y, r2b.z); glVertex3f(r3b.x, r3b.y, r3b.z);
			glVertex3f(r3b.x, r3b.y, r3b.z); glVertex3f(r1b.x, r1b.y, r1b.z);

			glVertex3f(r1a.x, r1a.y, r1a.z); glVertex3f(r1b.x, r1b.y, r1b.z);
			glVertex3f(r2a.x, r2a.y, r2a.z); glVertex3f(r2b.x, r2b.y, r2b.z);
			glVertex3f(r3a.x, r3a.y, r3a.z); glVertex3f(r3b.x, r3b.y, r3b.z);
		}
		glEnd();
		break;
	default:
		assert(false);
	}

	if (btex) glEnable(GL_TEXTURE_1D);
}

///////////////////////////////////////////////////////////////////////////////
/*
void CGLModel::RenderFeatureEdges(FEModel* ps)
{
	int i, j, n;
	int a, b;
	vec3f r1, r2, r3;

	FEMesh* pm = ps->GetMesh();

	glPushAttrib(GL_ENABLE_BIT);

	glDisable(GL_LIGHTING);

	glColor3ub(0,0,0);

	for (i=0; i<pm->Faces(); ++i)
	{
		FEFace& f = pm->Face(i);

		n = f.Edges();
		for (j=0; j<n; ++j)
		{
			if (f.m_nbr[j] < 0)
			{
				a = f.node[j];
				b = f.node[(j+1)%n];

				if (a > b) { a ^= b; b ^= a; a ^= b; }

				if (f.m_ntype != FACE_QUAD8)
				{
					r1 = pm->Node(a).m_rt;
					r2 = pm->Node(b).m_rt;

					glBegin(GL_LINES);
					{
						glVertex3f(r1.x, r1.y, r1.z);
						glVertex3f(r2.x, r2.y, r2.z);
					}
					glEnd();
				}
				else
				{
					r1 = pm->Node(a).m_rt;
					r2 = pm->Node(b).m_rt;
					r3 = pm->Node(f.node[j+4]).m_rt;

					glBegin(GL_LINES);
					{
						glVertex3f(r1.x, r1.y, r1.z); glVertex3f(r3.x, r3.y, r3.z);
						glVertex3f(r3.x, r3.y, r3.z); glVertex3f(r2.x, r2.y, r2.z);
					}
					glEnd();
				}
			}
		}
	}

	glPopAttrib();
}
*/

//-----------------------------------------------------------------------------
// Render all the visible faces (used for selection)
void CGLModel::RenderFaces(FEModel *ps, CGLContext &rc)
{
	// get the mesh
	FEMesh* pm = ps->GetMesh();

	vec3f r0, r1, r2, r3;

	for (int i=0; i<pm->Faces(); ++i)
	{
		FEFace& face = pm->Face(i);
		if (face.IsVisible())
		{
			glLoadName(i+1);

			r0 = pm->Node(face.node[0]).m_rt;
			r1 = pm->Node(face.node[1]).m_rt;
			r2 = pm->Node(face.node[2]).m_rt;

			switch (face.m_ntype)
			{
			case FACE_QUAD4:
			case FACE_QUAD8:
			case FACE_QUAD9:
				{
					r3 = pm->Node(face.node[3]).m_rt;

					glBegin(GL_QUADS);
					{
						glVertex3d(r0.x, r0.y, r0.z);
						glVertex3d(r1.x, r1.y, r1.z);
						glVertex3d(r2.x, r2.y, r2.z);
						glVertex3d(r3.x, r3.y, r3.z);
					}
					glEnd();
				}
				break;
			case FACE_TRI3:
			case FACE_TRI6:
			case FACE_TRI7:
				glBegin(GL_TRIANGLES);
				{
					glVertex3d(r0.x, r0.y, r0.z);
					glVertex3d(r1.x, r1.y, r1.z);
					glVertex3d(r2.x, r2.y, r2.z);
				}
				glEnd();
				break;
			default:
				assert(false);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Render all the elements (used for selection)
void CGLModel::RenderAllElements()
{
	// get the mesh
	FEMesh* pm = m_ps->GetMesh();

	for (int i=0; i<pm->Elements(); ++i)
	{
		FEElement& el = pm->Element(i);
		FEMaterial* pmat = m_ps->GetMaterial(el.m_MatID);
		if (el.IsVisible() && ((pmat == 0) || (pmat->bvisible)))
		{
			// solid elements
			int NF = el.Faces();
			for (int j=0; j<NF; ++j)
			{
				FEFace f = el.GetFace(j);
				switch (f.m_ntype)
				{
				case FACE_QUAD4:
				case FACE_QUAD8:
				case FACE_QUAD9:
					{
						vec3f r0 = pm->Node(f.node[0]).m_rt;
						vec3f r1 = pm->Node(f.node[1]).m_rt;
						vec3f r2 = pm->Node(f.node[2]).m_rt;
						vec3f r3 = pm->Node(f.node[3]).m_rt;

						glLoadName(i+1);
						glBegin(GL_QUADS);
						{
							glVertex3f(r0.x, r0.y, r0.z);
							glVertex3f(r1.x, r1.y, r1.z);
							glVertex3f(r2.x, r2.y, r2.z);
							glVertex3f(r3.x, r3.y, r3.z);
						}
						glEnd();
					}
					break;
				case FACE_TRI3:
				case FACE_TRI6:
				case FACE_TRI7:
					{
						vec3f r0 = pm->Node(f.node[0]).m_rt;
						vec3f r1 = pm->Node(f.node[1]).m_rt;
						vec3f r2 = pm->Node(f.node[2]).m_rt;

						glLoadName(i+1);
						glBegin(GL_TRIANGLES);
						{
							glVertex3f(r0.x, r0.y, r0.z);
							glVertex3f(r1.x, r1.y, r1.z);
							glVertex3f(r2.x, r2.y, r2.z);
						}
						glEnd();
					}
					break;
				default:
					assert(false);
				}
			}

			// shell elements
			int NE = el.Edges();
			if (NE > 0)
			{
				if (el.Nodes() == 4)
				{
					vec3f r0 = pm->Node(el.m_node[0]).m_rt;
					vec3f r1 = pm->Node(el.m_node[1]).m_rt;
					vec3f r2 = pm->Node(el.m_node[2]).m_rt;
					vec3f r3 = pm->Node(el.m_node[3]).m_rt;

					glLoadName(i+1);
					glBegin(GL_QUADS);
					{
						glVertex3f(r0.x, r0.y, r0.z);
						glVertex3f(r1.x, r1.y, r1.z);
						glVertex3f(r2.x, r2.y, r2.z);
						glVertex3f(r3.x, r3.y, r3.z);
					}
					glEnd();
				}
				else
				{
					vec3f r0 = pm->Node(el.m_node[0]).m_rt;
					vec3f r1 = pm->Node(el.m_node[1]).m_rt;
					vec3f r2 = pm->Node(el.m_node[2]).m_rt;

					glLoadName(i+1);
					glBegin(GL_TRIANGLES);
					{
						glVertex3f(r0.x, r0.y, r0.z);
						glVertex3f(r1.x, r1.y, r1.z);
						glVertex3f(r2.x, r2.y, r2.z);
					}
					glEnd();
				}
			}
		}
	}
}

void CGLModel::RenderDecorations()
{
	if (m_decor.empty() == false)
	{
		glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);

		std::list<GDecoration*>::iterator it;
		for (it=m_decor.begin(); it != m_decor.end(); ++it)
		{
			GDecoration* pd = *it;
			if (pd->isVisible()) pd->render();
		}
		glPopAttrib();
	}
}

void CGLModel::AddDecoration(GDecoration* pd)
{
	m_decor.push_back(pd);
}

void CGLModel::RemoveDecoration(GDecoration* pd)
{
	std::list<GDecoration*>::iterator it;
	for (it=m_decor.begin(); it != m_decor.end(); ++it)
	{
		if (*it==pd)
		{
			m_decor.erase(it);
			return;
		}
	}
}
