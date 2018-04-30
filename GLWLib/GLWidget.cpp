#ifdef WIN32
#include <Windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <QOpenGLWidget>

#include "GLWidget.h"
#include <assert.h>
#include <PostView2\convert.h>

//-----------------------------------------------------------------------------

GLWidget* GLWidget::m_pfocus = 0;

GLWidget::GLWidget(int x, int y, int w, int h, const char* szlabel)
{
	m_minw = 30;
	m_minh = 20;

	m_x = x;
	m_y = y;
	m_w = w;
	m_h = h;
	m_szlabel = (char*) szlabel;

	m_fgc = GLCOLOR(0,0,0);
	m_bgc[0] = GLCOLOR(0,0,0,0);
	m_bgc[1] = GLCOLOR(0,0,0,0);
	m_nbg = NONE;

	m_font = QFont("Helvetica", 13);
	m_font.setBold(false);
	m_font.setItalic(false);

	m_nsnap = 0;

	m_balloc = false;

	m_bshow = true;
}

GLWidget::~GLWidget(void)
{
	if (m_balloc) delete [] m_szlabel;
	m_szlabel = 0;
}

void GLWidget::copy_label(const char* szlabel)
{
	if (m_balloc) delete [] m_szlabel;
	m_szlabel = 0;
	m_balloc = false;

	int n = (int)strlen(szlabel);
	if (n)
	{
		m_szlabel = new char[n+1];
		strcpy(m_szlabel, szlabel);
		m_balloc = true;
	}
}

bool GLWidget::is_inside(int x, int y)
{
	if ((x>=m_x) && (x <=m_x+m_w) && 
		(y>=m_y) && (y <=m_y+m_h)) return true;

	return false;
}

//-----------------------------------------------------------------------------

GLBox::GLBox(int x, int y, int w, int h, const char *szlabel) : GLWidget(x, y, w, h, szlabel)
{
	m_bshadow = false;
	m_shc = GLCOLOR(200,200,200);
}

void GLBox::fit_to_size()
{
	if (m_szlabel)
	{
		QFontMetrics fm(m_font);
		QSize size = fm.size(Qt::TextExpandTabs, m_szlabel);
		resize(x(), y(), size.width(), size.height());
	}
}

void GLBox::draw_bg(int x0, int y0, int x1, int y1, QPainter* painter)
{
	QColor c1 = toQColor(m_bgc[0]);
	QColor c2 = toQColor(m_bgc[1]);

	QRect rt(x0, y0, x1 - x0, y1 - y0);

	switch (m_nbg)
	{
	case NONE: break;
	case COLOR1:
		painter->fillRect(rt, c1);
		break;
	case COLOR2:
		painter->fillRect(rt, c2);
		break;
	case HORIZONTAL:
		{
			QLinearGradient grad(rt.topLeft(), rt.topRight());
			grad.setColorAt(0, c1);
			grad.setColorAt(1, c2);
			painter->fillRect(rt, grad);
		}
		break;
	case VERTICAL:
		{
			QLinearGradient grad(rt.topLeft(), rt.bottomLeft());
			grad.setColorAt(0, c1);
			grad.setColorAt(1, c2);
			painter->fillRect(rt, grad);
		}
		break;
	}
}

void GLBox::draw(QPainter* painter)
{
	int x0 = m_x;
	int y0 = m_y;
	int x1 = m_x + m_w;
	int y1 = m_y + m_h;

	draw_bg(x0, y0, x1, y1, painter);

	if (m_szlabel)
	{
		if (m_bshadow)
		{
			int dx = m_font.pointSize()/10+1;
			painter->setPen(QColor(m_shc.r, m_shc.g, m_shc.b));
			painter->setFont(m_font);
			painter->drawText(x0+dx, y0+dx, m_w, m_h, Qt::AlignLeft| Qt::AlignVCenter, QString(m_szlabel));
		}
		QPen pen = painter->pen();
		pen.setColor(QColor(m_fgc.r, m_fgc.g, m_fgc.b));
		painter->setFont(m_font);
		painter->setPen(pen);
		painter->drawText(x0, y0, m_w, m_h, Qt::AlignLeft| Qt::AlignVCenter, QString(m_szlabel));
	}
}

//-----------------------------------------------------------------------------

GLLegendBar::GLLegendBar(CColorTexture* pm, int x, int y, int w, int h, int orientation) : GLWidget(x, y, w, h, 0)
{
	m_pMap = pm;
	m_ntype = GRADIENT;
	m_nrot = orientation;
	m_btitle = false;
	m_blabels = true;
	m_nprec = 3;
	m_labelPos = 0;

	m_fmin = 0.0f;
	m_fmax = 1.0f;
}

void GLLegendBar::draw(QPainter* painter)
{
	switch (m_ntype)
	{
	case GRADIENT: 
		if (m_nrot == VERTICAL) draw_gradient_vert(painter);
		else draw_gradient_horz(painter);
		break;
	case DISCRETE:
		if (m_nrot == VERTICAL) draw_discrete_vert(painter); 
		else draw_discrete_horz(painter);
		break;
	default:
		assert(false);
	}
}

void GLLegendBar::SetOrientation(int n)
{
	if (m_nrot == n) return;
	m_nrot = n;

	// swap width and height
	int tmp = m_w;
	m_w = m_h;
	m_h = tmp;
}

void GLLegendBar::SetRange(float fmin, float fmax)
{
	m_fmin = fmin;
	m_fmax = fmax;
}

void GLLegendBar::GetRange(float& fmin, float& fmax)
{
	fmin = m_fmin;
	fmax = m_fmax;
}

void GLLegendBar::draw_gradient_vert(QPainter* painter)
{
	painter->beginNativePainting();
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	// draw the legend
	int nsteps = m_pMap->GetDivisions();

	glDisable(GL_CULL_FACE);

	glEnable(GL_TEXTURE_1D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	m_pMap->GetTexture().MakeCurrent();

	GLint dfnc;
	glGetIntegerv(GL_DEPTH_FUNC, &dfnc);
	glDepthFunc(GL_ALWAYS);

	glDisable(GL_BLEND);

	int x0 = x() + w() - 50;
	int y0 = y() + 30;
	int x1 = x0 + 25;
	int y1 = y0 + h() - 40;

	if (m_labelPos == 1)
	{
		x0 = x() + 25;
		x1 = x0 + 25;
	}

	glColor3ub(255, 255, 255);
	glBegin(GL_QUADS);
	{
		glTexCoord1d(0); glVertex2i(x0, y1); 
		glTexCoord1d(0); glVertex2i(x1, y1); 
		glTexCoord1d(1); glVertex2i(x1, y0); 
		glTexCoord1d(1); glVertex2i(x0, y0); 
	}
	glEnd();

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glDisable(GL_TEXTURE_1D);

	glDisable(GL_LIGHTING);

	int i, yt, ipow;
	double f, p=1;
	char str[256], pstr[256];
	
	double a = MAX(fabs(m_fmin),fabs(m_fmax));
	if (a > 0)
	{
		double g = log10(a);
		ipow = (int) floor(g);
	}
	else ipow = 0;

	glColor3ub(m_fgc.r,m_fgc.g,m_fgc.b);
	glLineWidth(1.f);
	glBegin(GL_LINES);
	{
		for (i=0; i<=nsteps; i++)
		{
			yt = y0 + i*(y1 - y0)/nsteps;
			f = m_fmax + i*(m_fmin - m_fmax)/nsteps;
		
			glVertex2i(x0+1, yt);
			glVertex2i(x1-1, yt);
		}
	}
	glEnd();

	glDepthFunc(dfnc);

	glPopAttrib();
	painter->endNativePainting();

	if (m_blabels)
	{
		painter->setPen(QColor(m_fgc.r,m_fgc.g,m_fgc.b));
		painter->setFont(m_font);
		QFontMetrics fm(m_font);
	
		if((abs(ipow)>2))
		{
			sprintf(pstr, "x10");
			painter->drawText(x0, y0-5, QString(pstr));

			// change font size and draw superscript
			sprintf(pstr, "%d", ipow);
			QFontMetrics fm = painter->fontMetrics();
			int l = fm.width(QString("x10"));
			QFont f = m_font;
			f.setPointSize(m_font.pointSize() - 2);
			painter->setFont(f);
			painter->drawText(x0+l, y0-14, QString(pstr));
			
			// reset font size
			painter->setFont(m_font);
			p = pow(10.0, ipow);
		}

		char szfmt[16]={0};
		sprintf(szfmt, "%%.%dg", m_nprec);

		for (i=0; i<=nsteps; i++)
		{
			yt = y0 + i*(y1 - y0)/nsteps;
			f = m_fmax + i*(m_fmin - m_fmax)/nsteps;

			sprintf(str, szfmt, (fabs(f/p) < 1e-5 ? 0 : f/p));
			QString s(str);

			if (m_labelPos == 0)
			{
				int w = fm.width(s);
				int h = fm.ascent()/2;
		
				painter->drawText(x0-w-5, yt + h, s);
			}
			else
			{
				int h = fm.ascent() / 2;
				painter->drawText(x1 + 5, yt + h, s);
			}
		}
	}
}

void GLLegendBar::draw_gradient_horz(QPainter* painter)
{
	painter->beginNativePainting();
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	// draw the legend
	int nsteps = m_pMap->GetDivisions();

	glDisable(GL_CULL_FACE);

	glEnable(GL_TEXTURE_1D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	m_pMap->GetTexture().MakeCurrent();

	GLint dfnc;
	glGetIntegerv(GL_DEPTH_FUNC, &dfnc);
	glDepthFunc(GL_ALWAYS);

	glDisable(GL_BLEND);

	int x0 = x() + 10;
	int y0 = y() + h() - 50;
	int x1 = x() + w() - 30;
	int y1 = y0 + 25;

	if (m_labelPos == 1)
	{
		y0 = y() + 25;
		y1 = y0 + 25;
	}

	glColor3ub(255, 255, 255);
	glBegin(GL_QUADS);
	{
		glTexCoord1d(0); glVertex2i(x0, y0);
		glTexCoord1d(0); glVertex2i(x0, y1);
		glTexCoord1d(1); glVertex2i(x1, y1);
		glTexCoord1d(1); glVertex2i(x1, y0);
	}
	glEnd();

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glDisable(GL_TEXTURE_1D);

	glDisable(GL_LIGHTING);

	int i, ipow;
	double p = 1;
	char str[256], pstr[256];

	double a = MAX(fabs(m_fmin), fabs(m_fmax));
	if (a > 0)
	{
		double g = log10(a);
		ipow = (int)floor(g);
	}
	else ipow = 0;

	glColor3ub(m_fgc.r, m_fgc.g, m_fgc.b);
	glLineWidth(1.f);
	glBegin(GL_LINES);
	{
		for (i = 0; i <= nsteps; i++)
		{
			int xt = x0 + i*(x1 - x0) / nsteps;
			double f = m_fmax + i*(m_fmin - m_fmax) / nsteps;

			glVertex2i(xt, y0 + 1);
			glVertex2i(xt, y1 - 1);
		}
	}
	glEnd();

	glDepthFunc(dfnc);

	glPopAttrib();
	painter->endNativePainting();

	if (m_blabels)
	{
		painter->setPen(QColor(m_fgc.r, m_fgc.g, m_fgc.b));
		painter->setFont(m_font);
		QFontMetrics fm(m_font);

		if ((abs(ipow)>2))
		{
			sprintf(pstr, "x10");
			int x = x1 + 5;
			int y = y1 - 5;
			painter->drawText(x, y, QString(pstr));

			// change font size and draw superscript
			sprintf(pstr, "%d", ipow);
			QFontMetrics fm = painter->fontMetrics();
			int l = fm.width(QString("x10"));
			QFont f = m_font;
			f.setPointSize(m_font.pointSize() - 2);
			painter->setFont(f);
			painter->drawText(x + l, y - 14, QString(pstr));

			// reset font size
			painter->setFont(m_font);
			p = pow(10.0, ipow);
		}

		char szfmt[16] = { 0 };
		sprintf(szfmt, "%%.%dg", m_nprec);

		for (i = 0; i <= nsteps; i++)
		{
			int xt = x0 + i*(x1 - x0) / nsteps;
			double f = m_fmin + i*(m_fmax - m_fmin) / nsteps;

			sprintf(str, szfmt, (fabs(f / p) < 1e-5 ? 0 : f / p));
			QString s(str);

			if (m_labelPos == 0)
			{
				int w = fm.width(s);
				int h = fm.ascent() / 2;

				painter->drawText(xt - w/2, y0 - h - 5, s);
			}
			else
			{
				int w = fm.width(s);
				int h = fm.ascent() / 2;
				painter->drawText(xt - w / 2, y1 + h + 10, s);
			}
		}
	}
}

void GLLegendBar::draw_discrete_vert(QPainter* painter)
{
	// TODO: implement this
	assert(false);
}

void GLLegendBar::draw_discrete_horz(QPainter* painter)
{
	painter->beginNativePainting();
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	// draw the legend
	int nsteps = m_pMap->GetDivisions();

	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);

	GLint dfnc;
	glGetIntegerv(GL_DEPTH_FUNC, &dfnc);
	glDepthFunc(GL_ALWAYS);

	int x0, y0, x1, y1;
	if (m_nrot == VERTICAL)
	{
		x0 = x() + w() - 50;
		y0 = y() + 30;
		x1 = x0 + 25;
		y1 = y0 - 40;
	}
	else
	{
		x0 = x()+30;
		y0 = y() + h() - 90;
		x1 = x()+w()-50;
		y1 = y0 + 25;
	}

	glColor3ub(m_fgc.r,m_fgc.g,m_fgc.b);

	int i, yt, ipow;
	double f, p=1;
	
	double a = MAX(fabs(m_fmin),fabs(m_fmax));
	if (a > 0)
	{
		double g = log10(a);
		ipow = (int) floor(g);
	}
	else ipow = 0;

//	gl_font(m_font, m_font_size);
	
/*	if((abs(ipow)>2) && m_blabels)
	{
		sprintf(pstr, "x10");
		gl_draw(pstr, x1+10, y1, 28, 20, (Fl_Align)(FL_ALIGN_LEFT | FL_ALIGN_INSIDE));
		// change font size and draw superscript
		sprintf(pstr, "%d", ipow);
		int l = (int) fl_width("x10");
		gl_font(m_font, m_font_size-2);
		gl_draw(pstr, x1+10+l, y1, 28, 20, (Fl_Align) (FL_ALIGN_LEFT | FL_ALIGN_INSIDE));
		// reset font size
		gl_font(m_font, m_font_size);
		p = pow(10.0, ipow);
	}

*/

	if (m_blabels)
	{
//		char szfmt[16]={0};
//		sprintf(szfmt, "%%.%dg", m_nprec);

		CColorMap& map = ColorMapManager::GetColorMap(m_pMap->GetColorMap());

		// render the lines and text
		for (i=1; i<nsteps+1; i++)
		{
			if (m_nrot == VERTICAL)
			{
				yt = y0 + i*(y1 - y0)/(nsteps+1);
				f = m_fmax + i*(m_fmin - m_fmax)/(nsteps+1);
		
//				sprintf(str, szfmt, (fabs(f/p) < 1e-5 ? 0 : f/p));

				glColor3ub(m_fgc.r,m_fgc.g,m_fgc.b);
//				gl_draw(str, x0 - 55, yt-8, 50, 20, FL_ALIGN_RIGHT);

				GLCOLOR c = map.map((float) f);
				glColor3ub(c.r, c.g, c.b);

				glLineWidth(5.f);
				glBegin(GL_LINES);
				{
					glVertex2i(x0+1, yt);
					glVertex2i(x1-1, yt);
				}
				glEnd();
			}
			else
			{
				int xt = x0 + i*(x1 - x0)/(nsteps+1);
				f = m_fmin + i*(m_fmax - m_fmin)/(nsteps+1);
		
//				sprintf(str, szfmt, (fabs(f/p) < 1e-5 ? 0 : f/p));

				glColor3ub(m_fgc.r,m_fgc.g,m_fgc.b);
//				gl_draw(str, xt - 25, y1-30, 50, 20, FL_ALIGN_CENTER);

				GLCOLOR c = map.map((float)f);
				glColor3ub(c.r, c.g, c.b);

				glLineWidth(5.f);
				glBegin(GL_LINES);
				{
					glVertex2i(xt, y0+1);
					glVertex2i(xt, y1-1);
				}	
				glEnd();
			}
		}
	}

	glDepthFunc(dfnc);

	glPopAttrib();
	painter->endNativePainting();

	// render the title
	if (m_btitle && m_szlabel)
	{
		painter->setPen(QColor(m_fgc.r, m_fgc.g, m_fgc.b));
		painter->setFont(m_font);

		if (m_nrot == HORIZONTAL)
		{
			painter->drawText(x(), y(), w(), h(), Qt::AlignCenter | Qt::AlignBottom, m_szlabel);
		}
	}

	if (m_blabels)
	{
		painter->setPen(QColor(m_fgc.r, m_fgc.g, m_fgc.b));
		painter->setFont(m_font);
		QFontMetrics fm(m_font);
		int fh = fm.height();

		char szfmt[16]={0}, str[128] = {0};
		sprintf(szfmt, "%%.%dg", m_nprec);

		// render the lines and text
		for (i = 1; i<nsteps + 1; i++)
		{
			if (m_nrot == VERTICAL)
			{
				yt = y0 + i*(y1 - y0) / (nsteps + 1);
				f = m_fmax + i*(m_fmin - m_fmax) / (nsteps + 1);

				sprintf(str, szfmt, (fabs(f/p) < 1e-5 ? 0 : f/p));

				painter->drawText(x0 - 55, yt - 8, 50, 20, Qt::AlignRight, str);
			}
			else
			{
				int xt = x0 + i*(x1 - x0) / (nsteps + 1);
				f = m_fmin + i*(m_fmax - m_fmin) / (nsteps + 1);

				sprintf(str, szfmt, (fabs(f/p) < 1e-5 ? 0 : f/p));

				int fw = fm.width(str);
				painter->drawText(xt - fw/2, y1 + fh + 5, str);
			}
		}
	}
}

//-----------------------------------------------------------------------------

GLTriad::GLTriad(int x, int y, int w, int h) : GLWidget(x, y, w, h)
{
	m_rot = quat4f(0.f, vec3f(1.f,0.f,0.f));
	m_bcoord_labels = true;
}

void GLTriad::draw(QPainter* painter)
{
	painter->beginNativePainting();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	GLfloat ones[] = {1.f, 1.f, 1.f, 1.f};
	GLfloat ambient[] = {0.0f,0.0f,0.0f,1.f};
	GLfloat specular[] = {0.5f,0.5f,0.5f,1};
	GLfloat emission[] = {0,0,0,1};
	GLfloat	light[] = {0, 0, -1, 0};

	int view[4];
	glGetIntegerv(GL_VIEWPORT, view);
    
	int DPR = painter->device()->devicePixelRatio();

	int x0 = DPR*x();
	int y0 = view[3]-DPR*(y() + h());
	int x1 = x0 + DPR*w();
	int y1 = view[3]-DPR*y();
	if (x1 < x0) { x0 ^= x1; x1 ^= x0; x0 ^= x1; }
	if (y1 < y0) { y0 ^= y1; y1 ^= y0; y0 ^= y1; }

	glViewport(x0, y0, x1-x0, y1-y0);

	float ar = 1.f;
	if (h() != 0) ar = fabs((float) w() / (float) h());

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	float d = 1.2f;
	if (ar >= 1.f)	gluOrtho2D(-d*ar, d*ar, -d, d); else gluOrtho2D(-d, d, -d/ar, d/ar);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glClear(GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	glDisable(GL_CULL_FACE);
	glFrontFace(GL_CW);

	glLightfv(GL_LIGHT0, GL_POSITION, light);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ones);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, ones);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 32);

	quat4f q = m_rot;
	vec3f r = q.GetVector();
	float a = 180*q.GetAngle()/PI;

	if ((a > 0) && (r.Length() > 0))
		glRotatef(a, r.x, r.y, r.z);	

	// create the cylinder object
	glEnable(GL_LIGHTING);
	GLUquadricObj* pcyl = gluNewQuadric();

	const GLdouble r0 = .05;
	const GLdouble r1 = .15;

	glPushMatrix();
	glRotatef(90, 0, 1, 0);
	glColor3ub(255, 0, 0);
	gluCylinder(pcyl, r0, r0, .9, 5, 1);
	glTranslatef(0,0,.8f);
	gluCylinder(pcyl, r1, 0, 0.2, 10, 1);
	glPopMatrix();

	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	glColor3ub(0, 255, 0);
	gluCylinder(pcyl, r0, r0, .9, 5, 1);
	glTranslatef(0,0,.8f);
	gluCylinder(pcyl, r1, 0, 0.2, 10, 1);
	glPopMatrix();

	glPushMatrix();
	glColor3ub(0, 0, 255);
	gluCylinder(pcyl, r0, r0, .9, 5, 1);
	glTranslatef(0,0,.8f);
	gluCylinder(pcyl, r1, 0, 0.2, 10, 1);
	glPopMatrix();

	gluDeleteQuadric(pcyl);

	// restore project matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	// restore modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	// restore attributes
	glPopAttrib();

	// restore viewport
	glViewport(view[0], view[1], view[2], view[3]);

	painter->endNativePainting();

	// restore identity matrix
	if (m_bcoord_labels)
	{
		float a = 0.8f;
		vec3f ex(a, 0.f, 0.f);
		vec3f ey(0.f, a, 0.f);
		vec3f ez(0.f, 0.f, a);
		q.RotateVector(ex);
		q.RotateVector(ey);
		q.RotateVector(ez);

        x0 /= DPR;
        x1 /= DPR;
		y0 = (view[3] - y0)/DPR;
		y1 = (view[3] - y1)/DPR;

		ex.x = x0 + (x1 - x0)*(ex.x + 1)*0.5; ex.y = y0 + (y1 - y0)*(ex.y + 1)*0.5;
		ey.x = x0 + (x1 - x0)*(ey.x + 1)*0.5; ey.y = y0 + (y1 - y0)*(ey.y + 1)*0.5;
		ez.x = x0 + (x1 - x0)*(ez.x + 1)*0.5; ez.y = y0 + (y1 - y0)*(ez.y + 1)*0.5;

		painter->setFont(m_font);
		painter->setPen(toQColor(m_fgc));
		painter->drawText(ex.x, ex.y, "X");
		painter->drawText(ey.x, ey.y, "Y");
		painter->drawText(ez.x, ez.y, "Z");
	}
}

//-----------------------------------------------------------------------------

GLSafeFrame::GLSafeFrame(int x, int y, int w, int h) : GLWidget(x, y, w, h)
{
	m_block = false;
}

void GLSafeFrame::draw(QPainter* painter)
{
	painter->beginNativePainting();

	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	if (m_block)
		glColor3ub(255, 0, 0);
	else
		glColor3ub(255, 255, 0);

	glLineWidth(2.f);
	int x0 = x()-1;
	int y0 = y()-1;
	int x1 = x()+w()+1;
	int y1 = y()+h()+1;
	glBegin(GL_LINE_LOOP);
	{
		glVertex2i(x0, y0);
		glVertex2i(x1, y0);
		glVertex2i(x1, y1);
		glVertex2i(x0, y1);
	}
	glEnd();

	glPopAttrib();
	painter->endNativePainting();
}

bool GLSafeFrame::is_inside(int x, int y)
{
	int x0 = m_x;
	int y0 = m_y;
	int x1 = m_x+m_w;
	int y1 = m_y+m_h;

	int a = 2;

	if ((x>=x0-a) && (x<=x0+a) && (y >= y0) && (y <= y1)) return true;
	if ((x>=x1-a) && (x<=x1+a) && (y >= y0) && (y <= y1)) return true;
	if ((y>=y0-a) && (y<=y0+a) && (x >= x0) && (x <= x1)) return true;
	if ((y>=y1-a) && (y<=y1+a) && (x >= x0) && (x <= x1)) return true;

	return false;
}
