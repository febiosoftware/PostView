// GLVectorPlot.cpp: implementation of the CGLVectorPlot class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "GLVectorPlot.h"
#include "PostViewLib/ColorMap.h"
#include "PostViewLib/constants.h"
#include "PropertyList.h"

class CVectorPlotProps : public CPropertyList
{
public:
	CVectorPlotProps(CGLVectorPlot* v) : m_vec(v)
	{
		addProperty("Allow clipping", CProperty::Bool );
		addProperty("Density"       , CProperty::Float)->setFloatRange(0.0, 1.0);
		addProperty("Glyph"         , CProperty::Enum )->setEnumValues(QStringList() << "Arrow" << "Cone" << "Cylinder" << "Sphere" << "Box" << "Line");
		addProperty("Glyph Color"   , CProperty::Enum )->setEnumValues(QStringList() << "Solid" << "Length" << "Orientation");
		addProperty("Normalize"     , CProperty::Bool );
		addProperty("Auto-scale"    , CProperty::Bool );
		addProperty("Scale"         , CProperty::Float);
	}

	QVariant GetPropertyValue(int i)
	{
		switch (i)
		{
		case 0: return m_vec->AllowClipping(); break;
		case 1: return m_vec->GetDensity(); break;
		case 2: return m_vec->GetGlyphType(); break;
		case 3: return toQColor(m_vec->GetGlyphColor()); break;
		case 4: return m_vec->NormalizeVectors(); break;
		case 5: return m_vec->GetAutoScale(); break;
		case 6: return m_vec->GetScaleFactor(); break;
		}
		return QVariant();
	}

	void SetPropertyValue(int i, const QVariant& v)
	{
		switch (i)
		{
		case 0: m_vec->AllowClipping(v.toBool()); break;
		case 1: m_vec->SetDensity(v.toFloat()); break;
		case 2: m_vec->SetGlyphType(v.toInt()); break;
		case 3: m_vec->SetGlyphColor(toGLColor(v.value<QColor>())); break;
		case 4: m_vec->NormalizeVectors(v.toBool()); break;
		case 5: m_vec->SetAutoScale(v.toBool()); break;
		case 6: m_vec->SetScaleFactor(v.toFloat()); break;
		}
	}

private:
	CGLVectorPlot*	m_vec;
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGLVectorPlot::CGLVectorPlot(CGLModel* po) : CGLPlot(po)
{
	static int n = 1;
	sprintf(m_szname, "Vector plot.%02d", n++);

	m_scale = 1;
	m_dens = 1;

	m_nvec = BUILD_FIELD(1,0,0);

	m_nglyph = GLYPH_ARROW;

	m_ncol = GLYPH_COL_SOLID;

	m_gcl.r = 255;
	m_gcl.g = 255;
	m_gcl.b = 255;
	m_gcl.a = 255;

	m_bnorm = false;
	m_bautoscale = false;

	m_seed = rand();

	m_Col.SetType(COLOR_MAP_JET);
}

CGLVectorPlot::~CGLVectorPlot()
{

}

CPropertyList* CGLVectorPlot::propertyList()
{
	return new CVectorPlotProps(this);
}

inline double frand() { return (double) rand() / (double) RAND_MAX; }

void CGLVectorPlot::Render(CGLContext& rc)
{
	GLfloat ambient[] = {0.1f,0.1f,0.1f,1.f};
	GLfloat specular[] = {0.0f,0.0f,0.0f,1};
	GLfloat emission[] = {0,0,0,1};

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
//	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 32);

	// store attributes
	glPushAttrib(GL_LIGHTING_BIT);

	// create the cylinder object
//	glEnable(GL_LIGHTING);
	GLUquadricObj* pglyph = gluNewQuadric();

	gluQuadricNormals(pglyph, GLU_SMOOTH);

	GLdouble r0 = .05;
	GLdouble r1 = .15;
	GLdouble l0, l1, L;

	vec3f r, z(0,0,1), p;
	quat4f q;
	float w;

	FEModel* ps = m_pObj->GetFEModel();

	vec2f& rng = m_crng;

	m_Col.SetRange(rng.x, rng.y);

	GLCOLOR col;

	srand(m_seed);

	FEModel* pfem = m_pObj->GetFEModel();
	FEMesh* pm = m_pObj->GetMesh();

	float scale = 0.02f*m_scale*pfem->GetBoundingBox().Radius();

	if (m_nglyph == GLYPH_LINE) glDisable(GL_LIGHTING);
	else
	{
		glEnable(GL_COLOR_MATERIAL);

		GLfloat dif[] = {1.f, 1.f, 1.f, 1.f};

		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		glLightfv(GL_LIGHT0, GL_AMBIENT, dif);
	}

	int i, j, n;
	for (i=0; i<pm->Nodes(); ++i) pm->Node(i).m_ntag = 0;
	for (i=0; i<pm->Elements(); ++i)
	{
		FEElement& e = pm->Element(i);
		if (ps->GetMaterial(e.m_MatID)->benable)
		{
			n = e.Nodes();
			for (j=0; j<n; ++j) pm->Node(e.m_node[j]).m_ntag = 1;
		}
	}

	float auto_scale = 1.f;
	if (m_bautoscale)
	{
		float Lmax = 0.f;
		for (i=0; i<pm->Nodes(); ++i)
		{
			float L = m_val[i].Length();
			if (L > Lmax) Lmax = L;
		}
		if (Lmax == 0.f) Lmax = 1.f;
		auto_scale = 1.f/Lmax;
	}

	for (i=0; i<pm->Nodes(); ++i)
	{
		FENode& node = pm->Node(i);
		if ((frand() <= m_dens) && node.m_ntag)
		{
			vec3f r = node.m_rt;

			vec3f v = m_val[i];

			L = v.Length();

			if (L > 0)
			{
				col = m_Col.map((float) L);
				v.Normalize();

				switch (m_ncol)
				{
				case GLYPH_COL_LENGTH:
						glColor3ub(col.r, col.g, col.b);
					break;
				case GLYPH_COL_ORIENT:
					{
						GLdouble r = fabs(v.x);
						GLdouble g = fabs(v.y);
						GLdouble b = fabs(v.z);
						glColor3d(r, g, b);
					}
					break;
				case GLYPH_COL_SOLID: 
				default:
					glColor3ub(m_gcl.r, m_gcl.g, m_gcl.b);
				}
	
				if (m_bnorm) L = 1;

				L *= scale*auto_scale;
				l0 = L*.9;
				l1 = L*.2;
				r0 = L*0.05;
				r1 = L*0.15;
	
				glPushMatrix();
	
				glTranslatef(r.x, r.y, r.z);
				q = quat4f(z, v);
				w = q.GetAngle();
				if (fabs(w) > 1e-6)
				{
					p = q.GetVector();
					if (p.Length() > 1e-6) glRotatef(w*180/PI, p.x, p.y, p.z);
					else glRotatef(w*180/PI, 1, 0, 0);
				}

				switch (m_nglyph)
				{
				case GLYPH_ARROW:
					gluCylinder(pglyph, r0, r0, l0, 5, 1);
					glTranslatef(0.f,0.f,(float) l0*0.9f);
					gluCylinder(pglyph, r1, 0, l1, 10, 1);
					break;
				case GLYPH_CONE:
					gluCylinder(pglyph, r1, 0, l0, 10, 1);
					break;
				case GLYPH_CYLINDER:
					gluCylinder(pglyph, r1, r1, l0, 10, 1);
					break;
				case GLYPH_SPHERE:
					gluSphere(pglyph, r1, 10, 5);
					break;
				case GLYPH_BOX:
					glBegin(GL_QUADS);
					{
						glNormal3d(1,0,0);
						glVertex3d(r0, -r0, -r0);
						glVertex3d(r0,  r0, -r0);
						glVertex3d(r0,  r0,  r0);
						glVertex3d(r0, -r0,  r0);

						glNormal3d(-1,0,0);
						glVertex3d(-r0,  r0, -r0);
						glVertex3d(-r0, -r0, -r0);
						glVertex3d(-r0, -r0,  r0);
						glVertex3d(-r0,  r0,  r0);

						glNormal3d(0,1,0);
						glVertex3d( r0, r0, -r0);
						glVertex3d(-r0, r0, -r0);
						glVertex3d(-r0, r0,  r0);
						glVertex3d( r0, r0,  r0);

						glNormal3d(0,-1,0);
						glVertex3d(-r0, -r0, -r0);
						glVertex3d( r0, -r0, -r0);
						glVertex3d( r0, -r0,  r0);
						glVertex3d(-r0, -r0,  r0);

						glNormal3d(0,0,1);
						glVertex3d(-r0, r0, r0);
						glVertex3d( r0, r0, r0);
						glVertex3d( r0,-r0, r0);
						glVertex3d(-r0,-r0, r0);
	
						glNormal3d(0,0,-1);
						glVertex3d( r0, r0, -r0);
						glVertex3d(-r0, r0, -r0);
						glVertex3d(-r0,-r0, -r0);
						glVertex3d( r0,-r0, -r0);
					}
					glEnd();
					break;
				case GLYPH_LINE:
					glBegin(GL_LINES);
					{
						glVertex3d(0,0,0);
						glVertex3d(0,0,L);
					}
					glEnd();
				}

				glPopMatrix();
			}
		}
	}

	gluDeleteQuadric(pglyph);

	// restore attributes
	glPopAttrib();

}

void CGLVectorPlot::Update(int ntime, float dt, bool breset)
{
	if (breset) { m_map.Clear(); m_rng.clear(); m_val.clear(); }

	FEMesh* pm = m_pObj->GetMesh();
	FEModel* pfem = m_pObj->GetFEModel();

	if (m_map.States() == 0)
	{
		int NS = pfem->GetStates();
		int NN = pm->Nodes();

		m_map.Create(NS, NN, vec3f(0,0,0), -1);
		m_rng.resize(NS);
		m_val.resize(NN);
	}

	 // check the tag
	int ntag = m_map.GetTag(ntime);

	// see if we need to update
	if (ntag != m_nvec)
	{
		m_map.SetTag(ntime, m_nvec);

		// get the state we are interested in
		vector<vec3f>& val = m_map.State(ntime);
		
		vec2f& rng = m_rng[ntime];
		rng.x = rng.y = 0;

		float L;

		for (int i=0; i<pm->Nodes(); ++i)
		{
			val[i] = pfem->EvaluateNodeVector(i, ntime, m_nvec);
			L = val[i].Length();
			if (L > rng.y) rng.y = L;
		}

		if (rng.y == rng.x) ++rng.y;
	}

	// copy nodal values
	m_val = m_map.State(ntime);
	m_crng = m_rng[ntime];
}
