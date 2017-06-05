#include "stdafx.h"
#include "GLLinePlot.h"
#include "Document.h"
#include "PropertyList.h"

class CLineProps : public CPropertyList
{
public:
	CLineProps(CGLLinePlot* p) : m_line(p)
	{
		addProperty("line width" , CProperty::Float);
		addProperty("color"      , CProperty::Color);
		addProperty("render mode", CProperty::Enum )->setEnumValues(QStringList() << "lines" << "3D lines");
	}

	QVariant GetPropertyValue(int i)
	{
		switch (i)
		{
		case 0: return m_line->GetLineWidth(); break;
		case 1: return toQColor(m_line->GetLineColor()); break;
		case 3: return m_line->GetRenderMode(); break;
		}
		return QVariant();
	}

	void SetPropertyValue(int i, const QVariant& v)
	{
		switch (i)
		{
		case 0: m_line->SetLineWidth(v.toFloat()); break;
		case 1: m_line->SetLineColor(toGLColor(v.value<QColor>())); break;
		case 2: m_line->SetRenderMode(v.toInt()); break;
		}
	}

private:
	CGLLinePlot* m_line;
};

//-----------------------------------------------------------------------------
CGLLinePlot::CGLLinePlot(CGLModel* po) : CGLPlot(po)
{
	static int n = 1;
	char szname[128] = { 0 };
	sprintf(szname, "Line.%02d", n++);
	SetName(szname);

	m_line = 4.f;
	m_nmode = 0;
	m_col = GLCOLOR(255, 0, 0);
}

//-----------------------------------------------------------------------------
CGLLinePlot::~CGLLinePlot()
{
}

CPropertyList* CGLLinePlot::propertyList()
{
	return new CLineProps(this);
}

//-----------------------------------------------------------------------------
void CGLLinePlot::Render(CGLContext& rc)
{
	CGLModel& glm = *GetModel();
	FEModel& fem = *glm.GetFEModel();;
	int ns = glm.currentTimeIndex();

	GLfloat line_old;
	glGetFloatv(GL_LINE_WIDTH, &line_old);
	glLineWidth(m_line);
	if ((ns >= 0) && (ns <fem.GetStates()))
	{
		FEState& s = *fem.GetState(ns);
		int NL = s.Lines();
		if (NL > 0)
		{
			glPushAttrib(GL_ENABLE_BIT);
			{
				glColor3ub(m_col.r, m_col.g, m_col.b);

				switch (m_nmode)
				{
				case 0: 
					glDisable(GL_LIGHTING);
					RenderLines(s); 
					break;
				case 1:
					Render3DLines(s);
					break;
				}
			}
			glPopAttrib();
		}
	}
	glLineWidth(line_old);
}

//-----------------------------------------------------------------------------
void CGLLinePlot::RenderLines(FEState& s)
{
	glBegin(GL_LINES);
	{
		int NL = s.Lines();
		for (int i=0; i<NL; ++i)
		{
			LINEDATA& l = s.Line(i);
			glVertex3f(l.m_r0.x, l.m_r0.y, l.m_r0.z);
			glVertex3f(l.m_r1.x, l.m_r1.y, l.m_r1.z);
		}
	}
	glEnd();
}

//-----------------------------------------------------------------------------
void glxCylinder(float H, float R)
{
	glBegin(GL_QUAD_STRIP);
	const int N = 16;
	for (int i=0; i<=N; ++i)
	{
		double w = 2*PI*i/(double)N;
		double x = cos(w);
		double y = sin(w);
		glNormal3d(x, y, 0.0);
		glVertex3d(R*x, R*y, H);
		glVertex3d(R*x, R*y, 0);
	}
	glEnd();
}

//-----------------------------------------------------------------------------
void CGLLinePlot::Render3DLines(FEState& s)
{
	int NL = s.Lines();
	for (int i=0; i<NL; ++i)
	{
		LINEDATA& l = s.Line(i);
		vec3f n = l.m_r1 - l.m_r0;
		float L = n.Length();
		n.Normalize();

		glPushMatrix();
		{
			glTranslatef(l.m_r0.x, l.m_r0.y, l.m_r0.z);

			quat4f q(vec3f(0,0,1), n);
			vec3f r = q.GetVector();
			double angle = 180*q.GetAngle()/PI;
			if ((angle != 0.0) && (r.Length() > 0))
				glRotatef((float) angle, (float) r.x, (float) r.y, (float) r.z);	

			// render cylinder
			glxCylinder(L, m_line);
		}
		glPopMatrix();
	}
}

//=============================================================================

class CPointProps : public CPropertyList
{
public:
	CPointProps(CGLPointPlot* p) : m_pt(p)
	{
		addProperty("point size" , CProperty::Float);
		addProperty("color"      , CProperty::Color);
		addProperty("render mode", CProperty::Enum )->setEnumValues(QStringList() << "points");
	}

	QVariant GetPropertyValue(int i)
	{
		switch (i)
		{
		case 0: return m_pt->GetPointSize(); break;
		case 1: return toQColor(m_pt->GetPointColor()); break;
		case 3: return m_pt->GetRenderMode(); break;
		}
		return QVariant();
	}

	void SetPropertyValue(int i, const QVariant& v)
	{
		switch (i)
		{
		case 0: m_pt->SetPointSize(v.toFloat()); break;
		case 1: m_pt->SetPointColor(toGLColor(v.value<QColor>())); break;
		case 2: m_pt->SetRenderMode(v.toInt()); break;
		}
	}

private:
	CGLPointPlot* m_pt;
};

//-----------------------------------------------------------------------------
CGLPointPlot::CGLPointPlot(CGLModel* po) : CGLPlot(po)
{
	static int n = 1;
	char szname[128] = { 0 };
	sprintf(szname, "Points.%02d", n++);
	SetName(szname);

	for (int i=0; i<MAX_SETTINGS; ++i)
	{
		m_set[i].size = 8.f;
		m_set[i].nmode = 1;
		m_set[i].col = GLCOLOR(0, 0, 255);
		m_set[i].nvisible = 1;
	}
}

//-----------------------------------------------------------------------------
CGLPointPlot::~CGLPointPlot()
{
}

CPropertyList* CGLPointPlot::propertyList()
{
	return new CPointProps(this);
}

//-----------------------------------------------------------------------------
void CGLPointPlot::Render(CGLContext& rc)
{
	FEModel& fem = *GetModel()->GetFEModel();;
	int ns = fem.currentTime();

	GLfloat size_old;
	glGetFloatv(GL_POINT_SIZE, &size_old);
	if ((ns >= 0) && (ns <fem.GetStates()))
	{
		FEState& s = *fem.GetState(ns);
		int NP = s.Points();
		if (NP > 0)
		{
			glPushAttrib(GL_ENABLE_BIT);
			{
				glDisable(GL_LIGHTING);
				glDisable(GL_DEPTH_TEST);

				int NC = 0;
				for (int n=0; n<MAX_SETTINGS; ++n)
				{
					glPointSize(m_set[n].size);
					glColor3ub(m_set[n].col.r, m_set[n].col.g, m_set[n].col.b);

					glBegin(GL_POINTS);
					{
						for (int i=0; i<NP; ++i)
						{
							POINTDATA& p = s.Point(i);
							if (p.nlabel == n) 
							{
								if (m_set[n].nvisible) glVertex3f(p.m_r.x, p.m_r.y, p.m_r.z);
								NC++;
							}
						}
					}
					glEnd();

					if (NC >= NP) break;
				}
				assert(NC == NP);
			}
			glPopAttrib();
		}
	}
	glPointSize(size_old);
}
