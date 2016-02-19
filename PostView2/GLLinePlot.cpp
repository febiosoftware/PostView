#include "stdafx.h"
#include "GLLinePlot.h"
#include "Document.h"

//-----------------------------------------------------------------------------
CGLLinePlot::CGLLinePlot(CGLModel* po)
{
	static int n = 1;
	sprintf(m_szname, "Line.%02d", n++);
	m_line = 4.f;
	m_nmode = 1;
	m_col = GLCOLOR(255, 0, 0);
}

//-----------------------------------------------------------------------------
CGLLinePlot::~CGLLinePlot()
{
}

//-----------------------------------------------------------------------------
void CGLLinePlot::Render(CGLContext& rc)
{
/*	CDocument* pdoc = flxGetMainWnd()->GetDocument();
	int ns = pdoc->currentTime();
	FEModel& fem = *pdoc->GetFEModel();

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
					// glDisable(GL_DEPTH_TEST);
					glDisable(GL_LIGHTING);
					RenderLines(s); 
					break;
				case 1: 
					// glDisable(GL_DEPTH_TEST);
					glDisable(GL_LIGHTING);
					RenderLines(s);

					glLineWidth(m_line*0.75f);
					glColor4ub(255, 255, 255, 128);
					RenderLines(s);

					glLineWidth(m_line*0.25f);
					glColor4ub(255, 255, 255, 196);
					RenderLines(s);
					break;
				case 2:
					Render3DLines(s);
					break;
				}
			}
			glPopAttrib();
		}
	}
	glLineWidth(line_old);
*/
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
		double x = R*cos(w);
		double y = R*sin(w);
		glVertex3d(x, y, H);
		glVertex3d(x, y, 0);
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

		glPushMatrix();
		{
			glTranslatef(l.m_r0.x, l.m_r0.y, l.m_r0.z);

			quat4f q(n, vec3f(0,0,1));
			vec3f r = q.GetVector();
			double angle = 180*q.GetAngle()/PI;
			if ((angle > 0) && (r.Length() > 0))
				glRotatef((float) angle, (float) r.x, (float) r.y, (float) r.z);	

			// render bottom cap

			// render cylinder
			glxCylinder(L, 0.01*m_line);
		}
		glPopMatrix();
	}
}

//-----------------------------------------------------------------------------
CGLPointPlot::CGLPointPlot(CGLModel* po)
{
	static int n = 1;
	sprintf(m_szname, "Points.%02d", n++);
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

//-----------------------------------------------------------------------------
void CGLPointPlot::Render(CGLContext& rc)
{
/*	CDocument* pdoc = flxGetMainWnd()->GetDocument();
	int ns = pdoc->currentTime();
	FEModel& fem = *pdoc->GetFEModel();

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
*/
}
