#include "stdafx.h"
#include "GLLinePlot.h"
#include "PostView2/Document.h"
#include "PostView2/PropertyList.h"
using namespace Post;

class CLineProps : public CPropertyList
{
public:
	CLineProps(CGLLinePlot* p) : m_line(p)
	{
		QStringList cols;

		for (int i = 0; i<ColorMapManager::ColorMaps(); ++i)
		{
			string name = ColorMapManager::GetColorMapName(i);
			cols << name.c_str();
		}

		addProperty("line width" , CProperty::Float);
		addProperty("Color mode" , CProperty::Enum)->setEnumValues(QStringList() << "Solid" << "Line Data" << "Model Data");
		addProperty("Data field", CProperty::DataScalar);
		addProperty("Solid color", CProperty::Color);
		addProperty("Color map"  , CProperty::Enum)->setEnumValues(cols);
		addProperty("render mode", CProperty::Enum )->setEnumValues(QStringList() << "lines" << "3D lines");
	}

	QVariant GetPropertyValue(int i)
	{
		switch (i)
		{
		case 0: return m_line->GetLineWidth(); break;
		case 1: return m_line->GetColorMode(); break;
		case 2: return m_line->GetDataField(); break;
		case 3: return toQColor(m_line->GetSolidColor()); break;
		case 4: return m_line->GetColorMap()->GetColorMap();
		case 5: return m_line->GetRenderMode(); break;
		}
		return QVariant();
	}

	void SetPropertyValue(int i, const QVariant& v)
	{
		switch (i)
		{
		case 0: m_line->SetLineWidth(v.toFloat()); break;
		case 1: m_line->SetColorMode(v.toInt()); break;
		case 2: m_line->SetDataField(v.toInt()); break;
		case 3: m_line->SetSolidColor(toGLColor(v.value<QColor>())); break;
		case 4: m_line->GetColorMap()->SetColorMap(v.toInt()); break;
		case 5: m_line->SetRenderMode(v.toInt()); break;
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
	m_ncolor = 0;
	m_col = GLColor(255, 0, 0);
	m_nfield = -1;

	m_rng.x = 0.f;
	m_rng.y = 1.f;
}

//-----------------------------------------------------------------------------
CGLLinePlot::~CGLLinePlot()
{
}

CPropertyList* CGLLinePlot::propertyList()
{
	return new CLineProps(this);
}

void CGLLinePlot::SetColorMode(int m) 
{
	m_ncolor = m;
	Update(GetModel()->currentTimeIndex(), 0.0, false);
}

void CGLLinePlot::SetDataField(int n)
{ 
	m_nfield = n; 
	Update(GetModel()->currentTimeIndex(), 0.0, false);
}

//-----------------------------------------------------------------------------
void CGLLinePlot::Render(CGLContext& rc)
{
	CGLModel& glm = *GetModel();
	FEModel& fem = *glm.GetFEModel();
	int ns = glm.currentTimeIndex();

	GLfloat zero[4] = { 0.f };
	GLfloat one[4] = { 1.f, 1.f, 1.f, 1.f };
	GLfloat col[4] = { (GLfloat)m_col.r, (GLfloat)m_col.g, (GLfloat)m_col.b, 1.f};
	GLfloat amb[4] = { 0.1f, 0.1f, 0.1f, 1.f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, col);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, one);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, zero);
	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 64);

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
	if (m_ncolor == 0)
	{
		glBegin(GL_LINES);
		{
			int NL = s.Lines();
			for (int i = 0; i < NL; ++i)
			{
				LINEDATA& l = s.Line(i);
				glVertex3f(l.m_r0.x, l.m_r0.y, l.m_r0.z);
				glVertex3f(l.m_r1.x, l.m_r1.y, l.m_r1.z);
			}
		}
		glEnd();
	}
	else
	{
		CColorMap& map = ColorMapManager::GetColorMap(m_Col.GetColorMap());

		float vmin = m_rng.x;
		float vmax = m_rng.y;
		if (vmin == vmax) vmax++;

		glBegin(GL_LINES);
		{
			int NL = s.Lines();
			for (int i = 0; i < NL; ++i)
			{
				LINEDATA& l = s.Line(i);

				float f0 = (l.m_val[0] - vmin) / (vmax - vmin);
				float f1 = (l.m_val[1] - vmin) / (vmax - vmin);

				GLColor c0 = map.map(f0);
				GLColor c1 = map.map(f1);

				glColor3ub(c0.r, c0.g, c0.b);
				glVertex3f(l.m_r0.x, l.m_r0.y, l.m_r0.z);
				glColor3ub(c1.r, c1.g, c1.b);
				glVertex3f(l.m_r1.x, l.m_r1.y, l.m_r1.z);
			}
		}
		glEnd();

	}
}

//-----------------------------------------------------------------------------
void glxCylinder(float H, float R, float t0 = 0.f, float t1 = 1.f)
{
	glBegin(GL_QUAD_STRIP);
	const int N = 8;
	for (int i=0; i<=N; ++i)
	{
		double w = 2*PI*i/(double)N;
		double x = cos(w);
		double y = sin(w);
		glNormal3d(x, y, 0.0);
		glTexCoord1d(t1); glVertex3d(R*x, R*y, H);
		glTexCoord1d(t0); glVertex3d(R*x, R*y, 0);
	}
	glEnd();
}

//-----------------------------------------------------------------------------
void CGLLinePlot::Render3DLines(FEState& s)
{
	if (m_ncolor == 0)
	{
		int NL = s.Lines();
		for (int i = 0; i < NL; ++i)
		{
			LINEDATA& l = s.Line(i);
			vec3f n = l.m_r1 - l.m_r0;
			float L = n.Length();
			n.Normalize();

			glPushMatrix();
			{
				glTranslatef(l.m_r0.x, l.m_r0.y, l.m_r0.z);

				quat4f q(vec3f(0, 0, 1), n);
				vec3f r = q.GetVector();
				double angle = 180 * q.GetAngle() / PI;
				if ((angle != 0.0) && (r.Length() > 0))
					glRotatef((float)angle, (float)r.x, (float)r.y, (float)r.z);

				// render cylinder
				glxCylinder(L, m_line);
			}
			glPopMatrix();
		}
	}
	else
	{
		glColor3ub(255, 255, 255);

		glPushAttrib(GL_ENABLE_BIT);
		glEnable(GL_TEXTURE_1D);
		m_Col.GetTexture().MakeCurrent();

		float vmin = m_rng.x;
		float vmax = m_rng.y;
		if (vmin == vmax) vmax++;

		int NL = s.Lines();
		for (int i = 0; i < NL; ++i)
		{
			LINEDATA& l = s.Line(i);
			vec3f n = l.m_r1 - l.m_r0;
			float L = n.Length();
			n.Normalize();

			glPushMatrix();
			{
				glTranslatef(l.m_r0.x, l.m_r0.y, l.m_r0.z);

				quat4f q(vec3f(0, 0, 1), n);
				vec3f r = q.GetVector();
				double angle = 180 * q.GetAngle() / PI;
				if ((angle != 0.0) && (r.Length() > 0))
					glRotatef((float)angle, (float)r.x, (float)r.y, (float)r.z);

				float f0 = (l.m_val[0] - vmin) / (vmax - vmin);
				float f1 = (l.m_val[1] - vmin) / (vmax - vmin);

				// render cylinder
				glxCylinder(L, m_line, f0, f1);
			}
			glPopMatrix();
		}

		glPopAttrib();
	}
}

void CGLLinePlot::Update(int ntime, float dt, bool breset)
{
	if ((m_ncolor == 0) || ((m_ncolor==2)&&(m_nfield == -1))) return;

	float vmax = -1e20f;
	float vmin = 1e20f;
	if (m_ncolor == 1)
	{
		CGLModel& glm = *GetModel();
		FEModel& fem = *glm.GetFEModel();

		FEState& s = *fem.GetState(ntime);
		int NL = s.Lines();

		for (int i = 0; i < NL; ++i)
		{
			LINEDATA& line = s.Line(i);

			line.m_val[0] = line.m_user_data[0];
			line.m_val[1] = line.m_user_data[1];

			if (line.m_val[0] > vmax) vmax = line.m_val[0];
			if (line.m_val[0] < vmin) vmin = line.m_val[0];

			if (line.m_val[1] > vmax) vmax = line.m_val[1];
			if (line.m_val[1] < vmin) vmin = line.m_val[1];
		}
	}
	else if (m_ncolor == 2)
	{
		CGLModel& glm = *GetModel();
		FEModel& fem = *glm.GetFEModel();

		FEState& s = *fem.GetState(ntime);
		int NL = s.Lines();

		NODEDATA nd1, nd2;
		for (int i = 0; i < NL; ++i)
		{
			LINEDATA& line = s.Line(i);

			fem.EvaluateNode(line.m_r0, ntime, m_nfield, nd1);
			fem.EvaluateNode(line.m_r1, ntime, m_nfield, nd2);
			line.m_val[0] = nd1.m_val;
			line.m_val[1] = nd2.m_val;

			if (line.m_val[0] > vmax) vmax = line.m_val[0];
			if (line.m_val[0] < vmin) vmin = line.m_val[0];

			if (line.m_val[1] > vmax) vmax = line.m_val[1];
			if (line.m_val[1] < vmin) vmin = line.m_val[1];
		}
	}

	m_rng.x = vmin;
	m_rng.y = vmax;
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
		m_set[i].col = GLColor(0, 0, 255);
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
