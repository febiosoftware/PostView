#include "GLView.h"
#include <gl/GLU.h>
#include "MainWindow.h"
#include "Document.h"
#include "GLWidget.h"
#include "GLModel.h"
#include "GLPlaneCutPlot.h"
#include "GLContext.h"
#include <PostViewLib/VolRender.h>
#include <QMouseEvent>
#include <QPainter>

CGLView::CGLView(CMainWindow* pwnd, QWidget* parent) : QOpenGLWidget(parent), m_wnd(pwnd)
{
	m_panim = 0;
	m_nanim = ANIM_STOPPED;
	m_video_fmt = GL_RGB;

/*	QSurfaceFormat fmt = format();
	fmt.setSamples(4);
	fmt.setRenderableType(QSurfaceFormat::OpenGL);
	setFormat(fmt);
*/
	m_bdrag = false;

	m_bZoomRect = false;

	m_nview = VIEW_USER;

	m_fov = 45.f;

	m_btrack = false;
	CDocument* pdoc = pwnd->GetDocument();
	CGLModel* po = pdoc->GetGLModel();

	m_Widget = CGLWidgetManager::GetInstance();
	m_Widget->AttachToView(this);

	m_Widget->AddWidget(m_ptitle = new GLBox(po, 20, 20, 300, 20, pdoc, "%title")); 
	m_Widget->AddWidget(m_psubtitle = new GLBox(po, 20, 40, 300, 40, pdoc, "%field\\nTime = %time"));
	m_Widget->AddWidget(m_ptriad = new GLTriad(po, 0, 0, 150, 150, &GetCamera()));
	m_ptriad->align(GLW_ALIGN_LEFT | GLW_ALIGN_BOTTOM);
	m_Widget->AddWidget(m_pframe = new GLSafeFrame(po, 0, 0, 800, 600));
	m_pframe->align(GLW_ALIGN_HCENTER | GLW_ALIGN_VCENTER);
	m_pframe->hide();

	setFocusPolicy(Qt::StrongFocus);
	setAttribute(Qt::WA_AcceptTouchEvents, true);
}

CGLView::~CGLView()
{
}

CDocument* CGLView::GetDocument()
{
	return m_wnd->GetDocument();
}

void CGLView::initializeGL()
{
	GLfloat ones[] = {1.f, 1.f, 1.f, 1.f};
	glClearColor(1.f, 0.f, 0.f, 1.f);

	VIEWSETTINGS& view = GetDocument()->GetViewSettings();

	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glDepthFunc(GL_LEQUAL);

//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//	glShadeModel(GL_FLAT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glLineWidth(view.m_flinethick);
	if (view.m_blinesmooth)
	{
		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	}
	else glDisable(GL_LINE_SMOOTH);

	glPointSize(6.0f);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

	// enable lighting and set default options
	glEnable(GL_LIGHTING);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ones);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, ones);
	glLightfv(GL_LIGHT0, GL_SPECULAR, ones);
	glEnable(GL_LIGHT0);

	// enable color tracking for diffuse color of materials
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);

	// set the texture parameters
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);

	// why do I need 2D textures?
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glEnable(GL_STENCIL_TEST);

	// initialize clipping planes
	CGLPlaneCutPlot::InitClipPlanes();
}

void CGLView::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);

	// resize widgets
	for (int i=0; i<m_Widget->Widgets(); ++i)
	{
		GLWidget* pw = (*m_Widget)[i];

		int x0 = pw->x();
		if (x0 < 0) x0 = 0;

		int y0 = pw->y();
		if (y0 < 0) y0 = 0;
		
		int x1 = x0 + pw->w();
		if (x1 >= w) { x1 = w-1; x0 = x1 - pw->w(); }
		if (x0 < 0) x0 = 0;

		int y1 = y0 + pw->h();
		if (y1 >= h) { y1 = h-1; y0 = y1 - pw->h(); }
		if (y0 < 0) y0 = 0;

		pw->resize(x0, y0, x1-x0, y1-y0);
	}
}

void CGLView::paintGL()
{
/*	if (m_nanim == ANIM_RECORDING)
	{
		assert(m_panim);
		CRGBImage im;
		CaptureScreen(&im, m_video_fmt);
		m_panim->Write(im);
	}
*/
	// clear the Graphics view
	// This renders the background
	Clear();

	// get the document
	CDocument* pdoc = m_wnd->GetDocument();
	if (pdoc->IsValid() == false) return;

	// get the scene's view settings
	VIEWSETTINGS view = pdoc->GetViewSettings();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// turn on/off lighting
	if (view.m_bLighting)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);

	// get the scene's bounding box
	BOUNDINGBOX box = pdoc->GetBoundingBox();

	// set up the projection matrix
	double radius = box.Radius();
	vec3f rc = box.Center();

	m_fnear = 0.01*radius;
	m_ffar  = 100*radius;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (height() == 0) m_ar = 1; else m_ar = (GLfloat) width() / (GLfloat) height();

	if (view.m_nproj == RENDER_ORTHO)
	{
		double z = GetCamera().GetTargetDistance();
		double dx = z*tan(0.5*m_fov*PI/180.0)*m_ar;
		double dy = z*tan(0.5*m_fov*PI/180.0);
		glOrtho(-dx, dx, -dy, dy, m_fnear, m_ffar);
	}
	else
	{
		gluPerspective(m_fov, m_ar, m_fnear, m_ffar);
	}

	// set the model_view matrix mode
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// position the light
	vec3f lp = pdoc->GetLightPosition();
	GLfloat fv[4] = {0};
	fv[0] = lp.x; fv[1] = lp.y; fv[2] = lp.z;
	glLightfv(GL_LIGHT0, GL_POSITION, fv);

	// position the camera
	PositionCam();

	// render the model
	if (pdoc->IsValid()) RenderModel();

	// render the tracking
//	if (m_btrack) RenderTrack();

	// render the tags
//	if (view.m_bTags && pdoc->IsValid()) RenderTags();

	// give the command window a chance to render stuff
	CGLContext cgl(this);
	cgl.m_x = cgl.m_y = 0;

	// set the projection matrix to ortho2d so we can draw some stuff on the screen
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width(), 0, height());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	RenderWidgets();

	// render the selection rectangle
//	if (m_bdrag) RenderRubberBand();

	if (m_nanim != ANIM_STOPPED)
	{
		glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		int x = width()-200;
		int y = height()-40;
		glPopAttrib();
	}

	// if the camera is animating, we need to redraw
	if (GetCamera().IsAnimating())
	{
		GetCamera().Update();
//		Fl::add_timeout(0.05, cb_redraw, (void*) this);
	}
}

//-----------------------------------------------------------------------------
void CGLView::mousePressEvent(QMouseEvent* ev)
{
	int mode = 0;
	Qt::KeyboardModifiers key = ev->modifiers();
	if (key & Qt::ShiftModifier  ) mode |= SELECT_ADD;
	if (key & Qt::ControlModifier) mode |= SELECT_SUB;

	int x = ev->x();
	int y = ev->y();

	// let the widget manager handle it first
	if (m_Widget->handle(x, y, CGLWidgetManager::PUSH) == 1)
	{
		repaint();
		return;
	}

	m_p1.x = m_p0.x = m_xp = x;
	m_p1.y = m_p0.y = m_yp = y;
	if (mode != 0) m_bdrag = true;

	ev->accept();
}

//-----------------------------------------------------------------------------
void CGLView::mouseMoveEvent(QMouseEvent* ev)
{
	int x = ev->x();
	int y = ev->y();

	// store mouse position
	m_p1.x = x;
	m_p1.y = y;

	// let the widget manager handle it first
	if (m_Widget->handle(x, y, CGLWidgetManager::DRAG) == 1)
	{
		repaint();
		return;
	}

	CGLCamera* pcam = &GetCamera();

	int mode = 0;
	Qt::KeyboardModifiers key = ev->modifiers();
	if (key & Qt::ShiftModifier  ) mode |= SELECT_ADD;
	if (key & Qt::ControlModifier) mode != SELECT_SUB;

	Qt::MouseButtons buttons = ev->buttons();

	bool but1 = (buttons & Qt::LeftButton);
	bool but2 = (buttons & Qt::MiddleButton);
	bool but3 = (buttons & Qt::RightButton);

	bool balt   = (key & Qt::AltModifier);
	bool bshift = (key & Qt::ShiftModifier);

	if (mode == 0)
	{
		if (but1)
		{
			// see if alt-button is pressed
			if (balt)
			{
				// rotate in-plane
				quat4f qz = quat4f((y - m_yp)*0.01f, vec3f(0, 0, 1));
				pcam->Orbit(qz);
			}
			else
			{
				quat4f qx = quat4f((y - m_yp)*0.01f, vec3f(1, 0, 0));
				quat4f qy = quat4f((x - m_xp)*0.01f, vec3f(0, 1, 0));
				pcam->Orbit(qx);
				pcam->Orbit(qy);
			}
			m_nview = VIEW_USER;
			repaint();
		}
		else if ((but2) || (but3 && balt))
		{
			vec3f r = vec3f(-(float)(x - m_xp), (float)(y - m_yp), 0.f);
			PanView(r);
			repaint();
		}
		else if (but3)
		{
			if (m_yp > y) pcam->Zoom(0.95f);
			if (m_yp < y) pcam->Zoom(1.0f/0.95f);
	
//				if (m_zoom > 8) m_zoom = 8;
//				if (m_zoom < 1) m_zoom = 1;

			repaint();
		}
	}
	else // mode != 0
	{
		if (but1)
		{
			m_p1.x = x;
			m_p1.y = y;
			repaint();
		}
		else if (but3)
		{
			m_p1.x = x;
			m_p1.y = y;
			repaint();
		}
	}
	m_xp = x;
	m_yp = y;
	pcam->Update(true);
}

//-----------------------------------------------------------------------------
bool CGLView::event(QEvent* event)
{
	switch (event->type())
	{
	case QEvent::TouchBegin:
	case QEvent::TouchCancel:
	case QEvent::TouchEnd:
	case QEvent::TouchUpdate:
		event->accept();
		{
			QTouchEvent* te = static_cast<QTouchEvent*>(event);
			QList<QTouchEvent::TouchPoint> points = te->touchPoints();
			if (points.count() == 2)
			{
				QTouchEvent::TouchPoint p0 = points.first();
				QTouchEvent::TouchPoint p1 = points.last();
				QLineF line1(p0.startPos(), p1.startPos());
				QLineF line2(p0.pos(), p1.pos());
				double scale = line2.length() / line1.length();

				CGLCamera* pcam = &GetCamera();
				if (scale > 1.0) pcam->Zoom(0.95f);
				else pcam->Zoom(1.0f/0.95f);

				repaint();
			}
		}
		return true;
	}
	return QOpenGLWidget::event(event);
}

//-----------------------------------------------------------------------------
void CGLView::mouseReleaseEvent(QMouseEvent* ev)
{
	int x = ev->x();
	int y = ev->y();

	// let the widget manager handle it first
	if (m_Widget->handle(x, y, CGLWidgetManager::RELEASE) == 1)
	{
		repaint();
		return;
	}

	int mode = 0;
	Qt::KeyboardModifiers key = ev->modifiers();
	if (key & Qt::ShiftModifier  ) mode |= SELECT_ADD;
	if (key & Qt::ControlModifier) mode != SELECT_SUB;

	Qt::MouseButton button = ev->button();

	bool but1 = (button == Qt::LeftButton);
	bool but2 = (button == Qt::MiddleButton);
	bool but3 = (button == Qt::RightButton);

	bool balt   = (key & Qt::AltModifier);
	bool bshift = (key & Qt::ShiftModifier);


	// get the view mode
	CDocument* pdoc = GetDocument();
	int view_mode = pdoc->GetSelectionMode();

	if (but3)
	{
//		if ((m_p0.x == m_p1.x) && (m_p0.y == m_p1.y)) m_pop->popup();
	}
	else
	{
		m_bdrag = false;
		if (but1)
		{
			if (m_bZoomRect)
			{
				ZoomRect(m_p0, m_p1);
			}
			else
			{
				// select items
				if ((mode != 0) || (m_p0 == m_p1))
				{
					switch (view_mode)
					{
					case SELECT_ELEMS: SelectElements(m_p0.x, m_p0.y, m_p1.x, m_p1.y, mode); break;
					case SELECT_FACES: SelectFaces   (m_p0.x, m_p0.y, m_p1.x, m_p1.y, mode); break;
					case SELECT_NODES: SelectNodes   (m_p0.x, m_p0.y, m_p1.x, m_p1.y, mode); break;
					}

					// update the TrackView
//					m_pParent->UpdateTrackWnd();
				}
			}
		}
		else if (but3)
		{
//				ZoomRect(m_p0, m_p1);
		}
	}

	repaint();
}

void CGLView::Clear()
{
	// get the document
	CDocument* pdoc = m_wnd->GetDocument();

	// get the scene's view settings
	VIEWSETTINGS view = pdoc->GetViewSettings();

	// clear the scene
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// render the background
	RenderBkGround(view.bgcol1, view.bgcol2, view.bgstyle);
}

void CGLView::RenderBkGround(GLCOLOR c1, GLCOLOR c2, int style)
{
	const int w = width();
	const int h = height();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// store attributes
	glPushAttrib(GL_ENABLE_BIT);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glBegin(GL_QUADS);
	{
		switch (style)
		{
		case BG_COLOR_1:
			glColor3ub(c1.r, c1.g, c1.b); glVertex2i(0, 0);
			glColor3ub(c1.r, c1.g, c1.b); glVertex2i(w, 0);
			glColor3ub(c1.r, c1.g, c1.b); glVertex2i(w, h);
			glColor3ub(c1.r, c1.g, c1.b); glVertex2i(0, h);
			break;
		case BG_COLOR_2:
			glColor3ub(c2.r, c2.g, c2.b); glVertex2i(0, 0);
			glColor3ub(c2.r, c2.g, c2.b); glVertex2i(w, 0);
			glColor3ub(c2.r, c2.g, c2.b); glVertex2i(w, h);
			glColor3ub(c2.r, c2.g, c2.b); glVertex2i(0, h);
			break;
		case BG_FADE_HORZ:
			glColor3ub(c1.r, c1.g, c1.b); glVertex2i(0, 0);
			glColor3ub(c2.r, c2.g, c2.b); glVertex2i(w, 0);
			glColor3ub(c2.r, c2.g, c2.b); glVertex2i(w, h);
			glColor3ub(c1.r, c1.g, c1.b); glVertex2i(0, h);
			break;
		case BG_FADE_VERT:
			glColor3ub(c2.r, c2.g, c2.b); glVertex2i(0, 0);
			glColor3ub(c2.r, c2.g, c2.b); glVertex2i(w, 0);
			glColor3ub(c1.r, c1.g, c1.b); glVertex2i(w, h);
			glColor3ub(c1.r, c1.g, c1.b); glVertex2i(0, h);
			break;
		case BG_FADE_DIAG:
			glColor3ub(c1.r, c1.g, c1.b); glVertex2i(0, 0);
			glColor3ub(c2.r, c2.g, c2.b); glVertex2i(w, 0);
			glColor3ub(c1.r, c1.g, c1.b); glVertex2i(w, h);
			glColor3ub(c2.r, c2.g, c2.b); glVertex2i(0, h);
			break;
		}
	}
	glEnd();

	// restore attributes
	glPopAttrib();
}

//-----------------------------------------------------------------------------
void CGLView::RenderBox()
{
	CDocument* pdoc = m_wnd->GetDocument();
	VIEWSETTINGS& ops = pdoc->GetViewSettings();
	BOUNDINGBOX box = pdoc->GetBoundingBox();

	glColor3ub(ops.fgcol.r, ops.fgcol.g, ops.fgcol.b);

	// push attributes
	glPushAttrib(GL_ENABLE_BIT);

	// set attributes
	glEnable(GL_LINE_SMOOTH);
	glDisable(GL_LIGHTING);
		
	glBegin(GL_LINES);
	{
		glVertex3d(box.x0, box.y0, box.z0); glVertex3d(box.x1, box.y0, box.z0);
		glVertex3d(box.x1, box.y0, box.z0); glVertex3d(box.x1, box.y1, box.z0);
		glVertex3d(box.x1, box.y1, box.z0); glVertex3d(box.x0, box.y1, box.z0);
		glVertex3d(box.x0, box.y1, box.z0); glVertex3d(box.x0, box.y0, box.z0);

		glVertex3d(box.x0, box.y0, box.z1); glVertex3d(box.x1, box.y0, box.z1);
		glVertex3d(box.x1, box.y0, box.z1); glVertex3d(box.x1, box.y1, box.z1);
		glVertex3d(box.x1, box.y1, box.z1); glVertex3d(box.x0, box.y1, box.z1);
		glVertex3d(box.x0, box.y1, box.z1); glVertex3d(box.x0, box.y0, box.z1);

		glVertex3d(box.x0, box.y0, box.z0); glVertex3d(box.x0, box.y0, box.z1);
		glVertex3d(box.x1, box.y0, box.z0); glVertex3d(box.x1, box.y0, box.z1);
		glVertex3d(box.x0, box.y1, box.z0); glVertex3d(box.x0, box.y1, box.z1);
		glVertex3d(box.x1, box.y1, box.z0); glVertex3d(box.x1, box.y1, box.z1);
	}
	glEnd();

	// restore attributes
	glPopAttrib();
}

//-----------------------------------------------------------------------------
void CGLView::RenderWidgets()
{
	CDocument* pdoc = GetDocument();

	VIEWSETTINGS& view = pdoc->GetViewSettings();

	// render the title
	if (pdoc->IsValid() && view.m_bTitle) 
	{
		m_ptitle->show();
		m_psubtitle->show();
	}
	else
	{
		m_ptitle->hide();
		m_psubtitle->hide();
	}

	// render the triad
	if (view.m_bTriad) m_ptriad->show();
	else m_ptriad->hide();

	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	m_Widget->DrawWidgets(&painter);
	painter.end();
}

CGLCamera& CGLView::GetCamera()
{
	CDocument* pdoc = m_wnd->GetDocument();
	return pdoc->GetView()->GetCamera();
}

//-----------------------------------------------------------------------------
//! Render the tags on the selected items.
void CGLView::RenderTags()
{
	CDocument* pdoc = m_wnd->GetDocument();
	BOUNDINGBOX box = pdoc->GetBoundingBox();

	VIEWSETTINGS& view = pdoc->GetViewSettings();

	double radius = box.Radius();
	vec3f rc = box.Center();

	m_fnear = 0.01*radius;
	m_ffar  = 100*radius;

	FEMesh* pfe = pdoc->GetFEModel()->GetMesh();

	// create the tag array.
	// We add a tag for each selected item
	GLTAG tag;
	vector<GLTAG> vtag;

	// process elements
	for (int i=0; i<pfe->Elements(); i++)
	{
		FEElement& el = pfe->Element(i);
		if (el.IsSelected())
		{
			tag.r = pfe->ElementCenter(el);
			tag.bvis = false;
			tag.ntag = 0;
			sprintf(tag.sztag, "E%d", el.m_nId);
			vtag.push_back(tag);
		}
	}

	// process faces
	for (int i=0; i<pfe->Faces(); ++i)
	{
		FEFace& f = pfe->Face(i);
		if (f.IsSelected())
		{
			tag.r = pfe->FaceCenter(f);
			tag.bvis = false;
			tag.ntag = 0;
			sprintf(tag.sztag, "F%d", i+1);
			vtag.push_back(tag);
		}
	}

	// process nodes
	for (int i=0; i<pfe->Nodes(); i++)
	{
		FENode& node = pfe->Node(i);
		if (node.IsSelected())
		{
			tag.r = node.m_rt;
			tag.bvis = false;
			tag.ntag = (node.m_bext?0:1);
			sprintf(tag.sztag, "N%d", i+1);
			vtag.push_back(tag);
		}
	}

	// if we don't have any tags, just return
	if (vtag.empty()) return;

	// Okay, we got some tags to render. We now need to figure out
	// the screen location of each tag. We do this by rendering the 
	// tags in 3D in feedback mode. This will return the screen locations
	// of the tags.

	// set the feedback buffer
	int nsel = (int) vtag.size();
	int size = nsel*5;
	GLfloat *pbuf = new GLfloat[size];
	GLfloat* pb = pbuf;
	glFeedbackBuffer(size, GL_2D, pbuf);

	// enter feedback mode
	glRenderMode(GL_FEEDBACK);

	// switch to projection and save the matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// apply perspective projection
	if (view.m_nproj == RENDER_ORTHO)
	{
		double z = GetCamera().GetTargetDistance();
		double dx = z*tan(0.5*m_fov*PI/180.0)*m_ar;
		double dy = z*tan(0.5*m_fov*PI/180.0);
		glOrtho(-dx, dx, -dy, dy, m_fnear, m_ffar);
	}
	else
	{
		gluPerspective(m_fov, m_ar, m_fnear, m_ffar);
	}

	// draw the scene
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// render the tags
	PositionCam();
	for (int i=0; i<nsel; i++)
	{
		glPassThrough((float) i);
		glBegin(GL_POINTS);
		{
			glVertex3f(vtag[i].r.x, vtag[i].r.y, vtag[i].r.z);
		}
		glEnd();
	}

	// leave feedback mode
	glRenderMode(GL_RENDER);

	// parse the feedback buffer
	for (int i=0; i<nsel; i++)
	{
		int n = (int)(pb[1] + 0.5f);

		if ((int) (pb[2] + 0.5f) == 1793)
		{
			vtag[n].wx = pb[3];
			vtag[n].wy = pb[4];
			vtag[n].bvis = true;

			pb+=5;
		}
		else pb += 2;
	}

	// render the tags
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width(), 0, height());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_LIGHTING);

	for (int i=0; i<nsel; i++)
		if (vtag[i].bvis)
		{
			glBegin(GL_POINTS);
			{
				glColor3ub(0,0,0);
				glVertex2f(vtag[i].wx, vtag[i].wy);
				if (vtag[i].ntag == 0) glColor3ub(255,255,0);
				else glColor3ub(255,0,0);
				glVertex2f(vtag[i].wx-1, vtag[i].wy+1);
			}
			glEnd();

/*			gl_color(FL_BLACK);
			gl_draw(vtag[i].sztag, vtag[i].wx+3, vtag[i].wy);
			
			if (vtag[i].ntag == 0) gl_color(FL_YELLOW);
			else gl_color(FL_RED);
			gl_draw(vtag[i].sztag, vtag[i].wx+2, vtag[i].wy+1);
*/
		}

	// clean up
	delete [] pbuf;
}


//-----------------------------------------------------------------------------
void CGLView::PanView(vec3f r)
{
	CDocument* pdoc = GetDocument();
	CGLCamera* pcam = &GetCamera();

	if (pdoc->GetViewSettings().m_nproj == RENDER_ORTHO)
	{
		// get the scene's bounding box
		double z = GetCamera().GetTargetDistance();
		double dx = m_ar*z*tan(0.5*m_fov*PI/180.0);
		double dy = z*tan(0.5*m_fov*PI/180.0);

		r.x *= (float) (2.0*dx / width());
		r.y *= (float) (2.0*dy / height());
	}
	else
	{
		double hf = 2.0*m_fnear*tan(PI/180.0*m_fov*0.5);
		double wf = m_ar*hf;

		double sx = wf/width();
		double sy = hf/height();
		double sz = pcam->GetTargetDistance()/m_fnear;

		r.x *= (float)(sz*sx);
		r.y *= (float)(sz*sy);
	}

	pcam->Truck(r);
}

//-----------------------------------------------------------------------------
void CGLView::ZoomRect(MyPoint p0, MyPoint p1)
{
	CDocument* pdoc = GetDocument();

	CGLCamera* pcam = &GetCamera();

	if (p0.x > p1.x) { int a = p1.x; p1.x = p0.x; p0.x = a; }
	if (p0.y > p1.y) { int a = p1.y; p1.y = p0.y; p0.y = a; }

	// get the screen width and height in pixels
	double wi = width();
	double hi = height();

	// get the new width and height in pixels
	double dx = p1.x - p0.x;
	double dy = p1.y - p0.y;

	if (dx < dy) dx = m_ar*dy;
	else dy = dx / m_ar;

	// displacement on near plane in pixels
	double ex = 0.5*(p1.x + p0.x) - 0.5*wi;
	double ey = -0.5*(p1.y + p0.y) + 0.5*hi;

	// calculate physical width and height
	double wy = 2*m_fnear*tan(PI/180.0*m_fov*0.5);
	double wx = wy*m_ar;

	// convert ex and ey to physical units
	double lx = ex/wi*wx;
	double ly = ey/hi*wy;

	// get the camera's target distance
	double R = pcam->GetTargetDistance();

	vec3f v = vec3f((float) (-R*lx/m_fnear), (float) (-R*ly/m_fnear), 0.f);
	pcam->SetTargetDistance((float) (R*dx/wi));
	pcam->Truck(v);
}

//-----------------------------------------------------------------------------

void CGLView::SelectFaces(int x0, int y0, int x1, int y1, int mode)
{
	CDocument* pdoc = GetDocument();
	if (pdoc->IsValid() == false) return;

	int x = (x0+x1)/2;
	int y = (y0+y1)/2;
	int wx = abs(x1-x0);
	int wy = abs(y1-y0);

	bool bsingle = (wx==0) && (wy==0);

	if (wx == 0) wx = 4;
	if (wy == 0) wy = 4;

	// activate the gl rendercontext
	makeCurrent();

	BOUNDINGBOX box = pdoc->GetBoundingBox();
	VIEWSETTINGS& view = pdoc->GetViewSettings();

	FEModel* ps = pdoc->GetFEModel();
	FEMesh* pm = ps->GetMesh();

	double radius = box.Radius();
	vec3f rc = box.Center();

	m_fnear = 0.01*radius;
	m_ffar  = 10*radius;

/*	if (!valid())
	{
		SetupGL();
		glViewport(0,0,w(),h());
		valid(1);	// validate window
	}
*/
	// set up selection buffer
	int size = pm->Faces();
	GLuint *pbuf = new GLuint[4*size];

	glSelectBuffer(4*size, pbuf);

	// viewport storage
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	// switch to projection and save the matrix
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	// change render mode
	glRenderMode(GL_SELECT);

	// establish new clipping volume
	glLoadIdentity();
	gluPickMatrix(x, viewport[3] - y, wx, wy, viewport);

	// apply perspective projection
	if (view.m_nproj == RENDER_ORTHO)
	{
		double z = GetCamera().GetTargetDistance();
		double dx = z*tan(0.5*m_fov*PI/180.0)*m_ar;
		double dy = z*tan(0.5*m_fov*PI/180.0);
		glOrtho(-dx, dx, -dy, dy, m_fnear, m_ffar);
	}
	else
	{
		gluPerspective(m_fov, m_ar, m_fnear, m_ffar);
	}

	glInitNames();
	glPushName(0);

	// draw the scene
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// store attributes
	glPushAttrib(GL_ENABLE_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	// setup the rendering context
	CGLContext grc(this);

	// render the scene
	PositionCam();

	if (view.m_bcull) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	pdoc->GetGLModel()->RenderFaces(ps, grc);

	glPopMatrix();

	// restore attributes
	glPopAttrib();


	// collect the hits
	GLint hits = glRenderMode(GL_RENDER);

	if (mode == 0)
	{
		int faces = pm->Faces();
		for (int i=0; i<faces; i++) pm->Face(i).Unselect();

		mode = SELECT_ADD;
	}

	// parse the selection buffer
	if (hits > 0)
	{
		if (bsingle)
		{
			unsigned int index = pbuf[3];
			unsigned int minz = pbuf[1];
			for (int i=1; i<hits; i++)
			{
				if (pbuf[4*i+1] < minz)
				{
					minz = pbuf[4*i+1];
					index = pbuf[4*i+3];
				}
			}

			if (index >= 0) 
			{
				FEFace& f = pm->Face(index-1);
				if (mode == SELECT_ADD) 
				{
					if (view.m_bconn == false) f.Select();
					else pm->SelectConnectedFaces(f);
				}
				else f.Unselect();
			}
		}
		else
		{
			int n;
			for (int i=0; i<hits; i++)
			{
				n = pbuf[4*i+3];
				if ((n>0) && (n<=size))
				{
					FEFace& face = pm->Face(n-1);
					if (mode == SELECT_ADD)	face.Select();
					else face.Unselect();
				}
			}
		}
	}

	// count selection
	int N = 0, nn = -1, i;
	for (i=0; i<pm->Faces(); ++i)
	{
		if (pm->Face(i).IsSelected())
		{
			N++;
			nn = i;
		}
	}

	if (N==1)
	{
//		m_wnd->SetStatusBar("1 face selected: Id = %d", nn+1);
	}
	else if (N > 1)
	{
//		m_wnd->SetStatusBar("%d faces selected", N);
	}
//	else m_wnd->SetStatusBar(0);

	// Restore the projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	// clean up
	delete [] pbuf;
}

//-----------------------------------------------------------------------------

GLNODE_PICK CGLView::PickNode(int x, int y, bool bselect)
{
	int wx = 6;
	int wy = 6;

	// activate the gl rendercontext
	makeCurrent();

	CDocument* pdoc = GetDocument();
	BOUNDINGBOX box = pdoc->GetBoundingBox();
	VIEWSETTINGS& view = pdoc->GetViewSettings();

	FEModel* ps = pdoc->GetFEModel();
	FEMesh* pm = ps->GetMesh();

	double radius = box.Radius();
	vec3f rc = box.Center();

	m_fnear = 0.01*radius;
	m_ffar  = 10*radius;

/*	if (!valid())
	{
		SetupGL();
		glViewport(0,0,w(),h());
		valid(1);	// validate window
	}
*/
	// set up selection buffer
	int size = pm->Nodes();
	GLuint *pbuf = new GLuint[4*size];

	glSelectBuffer(4*size, pbuf);

	// viewport storage
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	// switch to projection and save the matrix
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	// change render mode
	glRenderMode(GL_SELECT);

	// establish new clipping volume
	glLoadIdentity();
	gluPickMatrix(x, viewport[3] - y, wx, wy, viewport);

	// apply perspective projection
	if (view.m_nproj == RENDER_ORTHO)
	{
		double z = GetCamera().GetTargetDistance();
		double dx = z*tan(0.5*m_fov*PI/180.0)*m_ar;
		double dy = z*tan(0.5*m_fov*PI/180.0);
		glOrtho(-dx, dx, -dy, dy, m_fnear, m_ffar);
	}
	else
	{
		gluPerspective(m_fov, m_ar, m_fnear, m_ffar);
	}

	glInitNames();
	glPushName(0);

	// draw the scene
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// store attributes
	glPushAttrib(GL_ENABLE_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	// setup the rendering context
	CGLContext grc(this);

	// render the scene
	PositionCam();

	if (view.m_bcull) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	pdoc->GetGLModel()->RenderNodes(ps, grc);

	glPopMatrix();

	// collect the hits
	GLint hits = glRenderMode(GL_RENDER);

	// parse the selection buffer
	unsigned int index = 0;
	if (hits > 0)
	{
		index = pbuf[3];
		unsigned int minz = pbuf[1];
		for (int i=1; i<hits; i++)
		{
			if (pbuf[4*i+1] < minz)
			{
				minz = pbuf[4*i+1];
				index = pbuf[4*i+3];
			}
		}
	}

	// Restore the projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	// restore attributes
	glPopAttrib();

	// clean up
	delete [] pbuf;

	GLNODE_PICK p;
	p.m_pm = 0;
	p.m_node = (int) index - 1;

	if (bselect)
	{
		FEMesh& mesh = *pdoc->GetFEModel()->GetMesh();
		int N = mesh.Nodes();
		if ((index > 0) && (index <= N))
		{
			mesh.ClearSelection();
			mesh.Node(index-1).Select();
		}
	}

	return p;
}

//-----------------------------------------------------------------------------

FEElement* CGLView::PickElement(int x, int y)
{
	int wx = 5;
	int wy = 5;

	// activate the gl rendercontext
	makeCurrent();

	CDocument* pdoc = GetDocument();
	BOUNDINGBOX box = pdoc->GetBoundingBox();
	VIEWSETTINGS& view = pdoc->GetViewSettings();

	FEModel* ps = pdoc->GetFEModel();
	FEMesh* pm = ps->GetMesh();

	double radius = box.Radius();
	vec3f rc = box.Center();

	m_fnear = 0.01*radius;
	m_ffar  = 10*radius;

/*	if (!valid())
	{
		SetupGL();
		glViewport(0,0,w(),h());
		valid(1);	// validate window
	}
*/
	// set up selection buffer
	int size = 10*pdoc->GetFEModel()->GetMesh()->Elements();
	GLuint *pbuf = new GLuint[4*size];

	glSelectBuffer(4*size, pbuf);

	// viewport storage
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	// switch to projection and save the matrix
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	// change render mode
	glRenderMode(GL_SELECT);

	// establish new clipping volume
	glLoadIdentity();
	gluPickMatrix(x, viewport[3] - y, wx, wy, viewport);

	if (view.m_nproj == RENDER_ORTHO)
	{
		double z = GetCamera().GetTargetDistance();
		double dx = z*tan(0.5*m_fov*PI/180.0)*m_ar;
		double dy = z*tan(0.5*m_fov*PI/180.0);
		glOrtho(-dx, dx, -dy, dy, m_fnear, m_ffar);
	}
	else
	{
		gluPerspective(m_fov, m_ar, m_fnear, m_ffar);
	}

	glInitNames();
	glPushName(0);

	// draw the scene
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// store attributes
	glPushAttrib(GL_ENABLE_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	CGLContext RC(this);
	PositionCam();
	pdoc->GetGLModel()->RenderFaces(RC);

	glPopMatrix();

	// collect the hits
	GLint hits = glRenderMode(GL_RENDER);

	// parse the selection buffer
	FEElement* pel = 0;
	if (hits > 0)
	{
		unsigned int index = pbuf[3];
		unsigned int minz = pbuf[1];
		for (int i=1; i<hits; i++)
		{
			if (pbuf[4*i+1] < minz)
			{
				minz = pbuf[4*i+1];
				index = pbuf[4*i+3];
			}
		}
		if (index > 0) pel = &pm->Element(index-1);
	}

	// Restore the projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	// restore attributes
	glPopAttrib();

	// clean up
	delete [] pbuf;

	return pel;
}

//-----------------------------------------------------------------------------

void CGLView::SelectElements(int x0, int y0, int x1, int y1, int mode)
{
	CDocument* pdoc = GetDocument();
	if (pdoc->IsValid() == false) return;

	int x = (x0+x1)/2;
	int y = (y0+y1)/2;
	int wx = abs(x1-x0);
	int wy = abs(y1-y0);

	bool bsingle = (wx==0) && (wy==0);

	if (wx == 0) wx = 4;
	if (wy == 0) wy = 4;

	// activate the gl rendercontext
	makeCurrent();

	BOUNDINGBOX box = pdoc->GetBoundingBox();
	VIEWSETTINGS& view = pdoc->GetViewSettings();

	FEModel* ps = pdoc->GetFEModel();
	FEMesh* pm = ps->GetMesh();

	double radius = box.Radius();
	vec3f rc = box.Center();

	m_fnear = 0.01*radius;
	m_ffar  = 10*radius;

/*	if (!valid())
	{
		SetupGL();
		glViewport(0,0,w(),h());
		valid(1);	// validate window
	}
*/
	// set up selection buffer
	int size = 6*pdoc->GetFEModel()->GetMesh()->Elements();
	if (size > 1000000) size = 1000000;

	GLuint *pbuf = new GLuint[4*size];
	glSelectBuffer(4*size, pbuf);

	// viewport storage
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	// switch to projection and save the matrix
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	// change render mode
	glRenderMode(GL_SELECT);

	// establish new clipping volume
	glLoadIdentity();
	gluPickMatrix(x, viewport[3] - y, wx, wy, viewport);

	if (view.m_nproj == RENDER_ORTHO)
	{
		double z = GetCamera().GetTargetDistance();
		double dx = z*tan(0.5*m_fov*PI/180.0)*m_ar;
		double dy = z*tan(0.5*m_fov*PI/180.0);
		glOrtho(-dx, dx, -dy, dy, m_fnear, m_ffar);
	}
	else
	{
		gluPerspective(m_fov, m_ar, m_fnear, m_ffar);
	}

	glInitNames();
	glPushName(0);

	// draw the scene
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// store attributes
	glPushAttrib(GL_ENABLE_BIT);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	VIEWSETTINGS& v = pdoc->GetViewSettings();

	CGLContext RC(this);
	PositionCam();

	if (v.m_bcull) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	pdoc->GetGLModel()->RenderAllElements();

	glPopMatrix();

	// restore attributes
	glPopAttrib();

	// collect the hits
	GLint hits = glRenderMode(GL_RENDER);

	// clear selection if mode = 0
	if (mode == 0)
	{
		int elems = pm->Elements();
		for (int i=0; i<elems; i++) pm->Element(i).Unselect();

		mode = SELECT_ADD;
	}

	// parse the selection buffer
	if (hits > 0)
	{
		int n;
		if (bsingle)
		{
			unsigned int index = pbuf[3];
			unsigned int minz = pbuf[1];
			for (int i=1; i<hits; i++)
			{
				if (pbuf[4*i+1] < minz)
				{
					minz = pbuf[4*i+1];
					index = pbuf[4*i+3];
				}
			}

			if (index > 0) 
			{
				FEElement& e = pm->Element(index-1);
				if (mode == SELECT_ADD) 
				{
					if (v.m_bconn == false) e.Select();
					else
					{
						if (v.m_bext) 
							pm->SelectConnectedSurfaceElements(e);
						else
							pm->SelectConnectedVolumeElements(e);
					}
				}
				else 
				{
					e.Unselect();
				}
			}
		}
		else
		{
			for (int i=0; i<hits; i++)
			{
				n = pbuf[4*i+3];
				if ((n>0) && (n<=size))
				{
					FEElement& e = pm->Element(n-1);
					if (mode == SELECT_ADD)	e.Select();
					else e.Unselect();
				}
			}
		}
	}


	// count element selection
	int N = 0, ne = -1, i;
	for (i=0; i<pm->Elements(); ++i)
	{
		if (pm->Element(i).IsSelected())
		{
			N++;
			ne = i;
		}
	}
	if (N==1)
	{
//		m_wnd->SetStatusBar("1 element selected: Id = %d", ne+1);
	}
	else if (N > 1)
	{
//		m_wnd->SetStatusBar("%d elements selected", N);
	}
//	else m_pParent->SetStatusBar(0);


	// Restore the projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	// clean up
	delete [] pbuf;
}

void CGLView::SelectNodes(int x0, int y0, int x1, int y1, int mode)
{
	CDocument* pdoc = GetDocument();
	if (pdoc->IsValid() == false) return;

	int x = (x0+x1)/2;
	int y = (y0+y1)/2;
	int wx = abs(x1-x0);
	int wy = abs(y1-y0);

	bool bsingle = (wx==0) && (wy==0);

	if (wx == 0) wx = 4;
	if (wy == 0) wy = 4;

	// activate the gl rendercontext
	makeCurrent();

	BOUNDINGBOX box = pdoc->GetBoundingBox();
	VIEWSETTINGS& view = pdoc->GetViewSettings();

	FEModel* ps = pdoc->GetFEModel();
	FEMesh* pm = ps->GetMesh();

	double radius = box.Radius();
	vec3f rc = box.Center();

	m_fnear = 0.01*radius;
	m_ffar  = 10*radius;

/*	if (!valid())
	{
		SetupGL();
		glViewport(0,0,w(),h());
		valid(1);	// validate window
	}
*/
	// set up selection buffer
	int size = pm->Nodes();
	GLuint *pbuf = new GLuint[4*size];

	glSelectBuffer(4*size, pbuf);

	// viewport storage
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	// switch to projection and save the matrix
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	// change render mode
	glRenderMode(GL_SELECT);

	// establish new clipping volume
	glLoadIdentity();
	gluPickMatrix(x, viewport[3] - y, wx, wy, viewport);

	// apply perspective projection
	if (view.m_nproj == RENDER_ORTHO)
	{
		double z = GetCamera().GetTargetDistance();
		double dx = z*tan(0.5*m_fov*PI/180.0)*m_ar;
		double dy = z*tan(0.5*m_fov*PI/180.0);
		glOrtho(-dx, dx, -dy, dy, m_fnear, m_ffar);
	}
	else
	{
		gluPerspective(m_fov, m_ar, m_fnear, m_ffar);
	}

	glInitNames();
	glPushName(0);

	// draw the scene
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// store attributes
	glPushAttrib(GL_ENABLE_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	// setup the rendering context
	CGLContext grc(this);

	// render the scene
	PositionCam();

	if (view.m_bcull) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	pdoc->GetGLModel()->RenderNodes(ps, grc);

	glPopMatrix();

	// restore attributes
	glPopAttrib();

	// collect the hits
	GLint hits = glRenderMode(GL_RENDER);

	if (mode == 0)
	{
		int nodes = pm->Nodes();
		for (int i=0; i<nodes; i++) pm->Node(i).Unselect();

		mode = SELECT_ADD;
	}

	// parse the selection buffer
	if (hits > 0)
	{
		if (bsingle)
		{
			unsigned int index = pbuf[3];
			unsigned int minz = pbuf[1];
			for (int i=1; i<hits; i++)
			{
				if (pbuf[4*i+1] < minz)
				{
					minz = pbuf[4*i+1];
					index = pbuf[4*i+3];
				}
			}

			if (index >= 0) 
			{
				FENode& n = pm->Node(index-1);
				if (mode == SELECT_ADD) 
				{
					n.Select();
					if (view.m_bconn == false) n.Select();
					else 
					{
						if (view.m_bext)
							pm->SelectConnectedSurfaceNodes(index-1);
						else
							pm->SelectConnectedVolumeNodes(index-1);
					}
				}
				else n.Unselect();
			}
		}
		else
		{
			int n;
			for (int i=0; i<hits; i++)
			{
				n = pbuf[4*i+3];
				if ((n>0) && (n<=size))
				{
					FENode& node = pm->Node(n-1);
					if (mode == SELECT_ADD)	node.Select();
					else node.Unselect();
				}
			}
		}
	}

	// count nodal selection
	int N = 0, nn = -1, i;
	for (i=0; i<pm->Nodes(); ++i)
	{
		if (pm->Node(i).IsSelected())
		{
			N++;
			nn = i;
		}
	}

	if (N==1)
	{
		FENode& n = pm->Node(nn);
		vec3f r = n.m_rt;
		float f = pdoc->GetCurrentState()->m_NODE[nn].m_val;
//		m_wnd->SetStatusBar("1 node selected: Id = %d, val = %g, pos = (%g, %g, %g)", nn+1, f, r.x, r.y, r.z);
	}
	else if (N > 1)
	{
//		m_wnd->SetStatusBar("%d nodes selected", N);
	}
//	else m_wnd->SetStatusBar(0);

	// Restore the projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	// clean up
	delete [] pbuf;
}

QImage CGLView::CaptureScreen()
{
	return grabFramebuffer();
}

void CGLView::RenderRubberBand()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width(), 0, height());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// store attributes
	glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT);

	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);
	glColor3ub(0,0,0);
	glLineWidth(1.f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineStipple(1, (GLushort) 0xF0F0);
	glDisable(GL_CULL_FACE);
	glEnable(GL_LINE_STIPPLE);
	glRecti(m_p0.x, height() - m_p0.y, m_p1.x, height() - m_p1.y);
	glEnable(GL_CULL_FACE);
	glDisable(GL_LINE_STIPPLE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// restore attributes
	glPopAttrib();
}

//-----------------------------------------------------------------------------
// Render the FE model
void CGLView::RenderModel()
{
	CDocument* pdoc = GetDocument();
	VIEWSETTINGS& view = pdoc->GetViewSettings();

	FEModel* ps = pdoc->GetFEModel();

	BOUNDINGBOX box = pdoc->GetBoundingBox();

	// set the diffuse lighting intensity
	GLfloat d = view.m_diffuse;
	GLfloat dv[4] = {d, d, d, 1.f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, dv);

	// set the ambient lighting intensity
	GLfloat f = view.m_ambient;
	GLfloat av[4] = {f, f, f, 1.f};
	glLightfv(GL_LIGHT0, GL_AMBIENT, av);

	if (view.m_bShadows)
	{
		float a = view.m_shadow_intensity;
		GLfloat shadow[] = {a, a, a, 1};
		GLfloat zero[] = {0, 0, 0, 1};
		GLfloat ones[] = {1,1,1,1};
		GLfloat lp[4] = {0};

		glEnable( GL_STENCIL_TEST );

		float inf = box.Radius()*100.f;

		vec3f lpv = pdoc->GetLightPosition();

		quat4f q = GetCamera().GetOrientation();
		q.Inverse().RotateVector(lpv);

		lp[0] = lpv.x;
		lp[1] = lpv.y;
		lp[2] = lpv.z;

		// set coloring for shadows
		glLightfv(GL_LIGHT0, GL_DIFFUSE, shadow);
		glLightfv(GL_LIGHT0, GL_SPECULAR, zero);

		glStencilFunc(GL_ALWAYS, 0x00, 0xff);
		glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);

		// render the scene
		RenderDoc();

		// Create mask in stencil buffer
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);

		pdoc->GetGLModel()->RenderShadows(ps, lpv, inf);

		glCullFace(GL_BACK);
		glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);

		pdoc->GetGLModel()->RenderShadows(ps, lpv, inf);
	
		// Render the scene in light
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);

		glLightfv(GL_LIGHT0, GL_DIFFUSE, dv);
		glLightfv(GL_LIGHT0, GL_SPECULAR, ones);
		
		glStencilFunc(GL_EQUAL, 0, 0xff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		glDisable(GL_CULL_FACE);

		glClear(GL_DEPTH_BUFFER_BIT);
	}

	// render the scene
	RenderDoc();
}

//-----------------------------------------------------------------------------
// Render the plots
void CGLView::RenderPlots(CGLContext& rc)
{
	CDocument* pdoc = GetDocument();
	list<CGLPlot*>& PL = pdoc->GetPlotList();
	list<CGLPlot*>::iterator it = PL.begin();
	for (int i=0; i<(int) PL.size(); ++i, ++it)
	{
		CGLPlot* pl = *it;

		if (pl->AllowClipping()) CGLPlaneCutPlot::EnableClipPlanes();
		else CGLPlaneCutPlot::DisableClipPlanes();

		if ((*it)->IsActive()) (*it)->Render(rc);
	}
}

//-----------------------------------------------------------------------------
// Render the model (for real)
void CGLView::RenderDoc()
{
	CDocument* pdoc = GetDocument();
	if (pdoc->IsValid() == false) return;

	glPushAttrib(GL_ENABLE_BIT);
	{
		int i;

		CGLCamera& cam = GetCamera();
		VIEWSETTINGS& view = pdoc->GetViewSettings();
		int mode = pdoc->GetSelectionMode();

		if (view.m_bcull) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);

		// setup the rendering context
		CGLContext rc(this);

		// first we render all the plots
		RenderPlots(rc);

		// activate all clipping planes
		CGLPlaneCutPlot::EnableClipPlanes();

		// render the mesh
		CGLModel* po = pdoc->GetGLModel();
		if (po && po->IsActive())
		{
			// Render the model
			po->Render(rc);

			// render the lines
			// Notice that we change the depth range for rendering the lines
			// We do this to prevent z-fighting between the mesh' lines and the
			// the mesh polygons. I could have used glPolygonOffset but I found
			// that this approach gave better results. Furthermore, this way works
			// with more than just polygons.
			if (view.m_bmesh)
			{
				glDepthRange(0, 0.99999);
				po->RenderMeshLines(pdoc->GetFEModel());
				glDepthRange(0, 1);
			}

			// render the nodes
			if (mode == SELECT_NODES) 
			{
				glDepthRange(0, 0.999985);
				po->RenderNodes(pdoc->GetFEModel(), rc);
				glDepthRange(0, 1);
			}

			// render the bounding box
			if (view.m_bBox) RenderBox();
		}

		// render the other objects
		list<CGLVisual*>& OL = pdoc->GetObjectList();
		list<CGLVisual*>::iterator ot = OL.begin();
		for (i=0; i<(int) OL.size(); ++i, ++ot)
		{
			if ((*ot)->IsActive()) (*ot)->Render(rc);
		}

		CGLPlaneCutPlot::DisableClipPlanes();

		// render the volume image data if present
		CVolRender* pvr = pdoc->GetVolumeRenderer();
		if (pvr)
		{
			CGLCamera& cam = GetCamera();
			quat4f q = cam.GetOrientation();
			pvr->Render(q);
		}
	}
	glPopAttrib();
}

void CGLView::RenderTrack()
{
	if (m_btrack == false) return;

	CDocument* pdoc = GetDocument();
	FEMesh* pm = pdoc->GetFEModel()->GetMesh();
	int* nt = m_ntrack;

	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	vec3f a = pm->Node(nt[0]).m_rt;
	vec3f b = pm->Node(nt[1]).m_rt;
	vec3f c = pm->Node(nt[2]).m_rt;

	vec3f e1 = (b - a);
	vec3f e3 = e1^(c - a);
	vec3f e2 = e3^e1;
	e1.Normalize();
	e2.Normalize();
	e3.Normalize();

	vec3f A, B, C;
	A = a + e1;
	B = a + e2;
	C = a + e3;

	glColor3ub(255, 0, 255);
	glBegin(GL_LINES);
	{
		glVertex3f(a.x, a.y, a.z); glVertex3f(A.x, A.y, A.z);
		glVertex3f(a.x, a.y, a.z); glVertex3f(B.x, B.y, B.z);
		glVertex3f(a.x, a.y, a.z); glVertex3f(C.x, C.y, C.z);
	}
	glEnd();

	glPopAttrib();
}

inline vec3f mult_matrix(GLfloat m[4][4], vec3f r)
{
	vec3f a;
	a.x = m[0][0]*r.x + m[0][1]*r.y + m[0][2]*r.z;
	a.y = m[1][0]*r.x + m[1][1]*r.y + m[1][2]*r.z;
	a.z = m[2][0]*r.x + m[2][1]*r.y + m[2][2]*r.z;
	return a;
}

void CGLView::PositionCam()
{
	CDocument* pdoc = GetDocument();

	// position the camera
	GetCamera().Transform();

	// see if we need to track anything
	if (pdoc->IsValid() && m_btrack)
	{
		FEMesh* pm = pdoc->GetFEModel()->GetMesh();
		int NN = pm->Nodes();
		int* nt = m_ntrack;
		if ((nt[0] >= NN) || (nt[1] >= NN) || (nt[2] >= NN)) { m_btrack = false; return; }

		vec3f a = pm->Node(nt[0]).m_r0;
		vec3f b = pm->Node(nt[1]).m_r0;
		vec3f c = pm->Node(nt[2]).m_r0;

		vec3f r0 = a;

		vec3f E1 = (b - a);
		vec3f E3 = E1^(c - a);
		vec3f E2 = E3^E1;
		E1.Normalize();
		E2.Normalize();
		E3.Normalize();

		a = pm->Node(nt[0]).m_rt;
		b = pm->Node(nt[1]).m_rt;
		c = pm->Node(nt[2]).m_rt;

		vec3f r1 = a;

		vec3f e1 = (b - a);
		vec3f e3 = e1^(c - a);
		vec3f e2 = e3^e1;
		e1.Normalize();
		e2.Normalize();
		e3.Normalize();

		vec3f dr = r0 - r1;
		glTranslatef(dr.x, dr.y, dr.z);

		glTranslatef(r1.x, r1.y, r1.z);

		// setup the rotation matrix
		GLfloat m[4][4] = {0}, Q[4][4] = {0}, Qi[4][4] = {0};
		m[3][3] = 1.f;
		Q[3][3] = 1.f;
		Qi[3][3] = 1.f;

		Q[0][0] = E1.x; Q[0][1] = E1.y; Q[0][2] = E1.z;
		Q[1][0] = E2.x; Q[1][1] = E2.y; Q[1][2] = E2.z;
		Q[2][0] = E3.x; Q[2][1] = E3.y; Q[2][2] = E3.z;

		Qi[0][0] = E1.x; Qi[1][0] = E1.y; Qi[2][0] = E1.z;
		Qi[0][1] = E2.x; Qi[1][1] = E2.y; Qi[2][1] = E2.z;
		Qi[0][2] = E3.x; Qi[1][2] = E3.y; Qi[2][2] = E3.z;

		m[0][0] = E1*e1; m[0][1] = E1*e2; m[0][2] = E1*e3;
		m[1][0] = E2*e1; m[1][1] = E2*e2; m[1][2] = E2*e3;
		m[2][0] = E3*e1; m[2][1] = E3*e2; m[2][2] = E3*e3;
		glMultMatrixf(&Q[0][0]);
		glMultMatrixf(&m[0][0]);
		glMultMatrixf(&Qi[0][0]);

		vec3f rq = mult_matrix(Q, r1);
		rq = mult_matrix(m, rq);
		rq = mult_matrix(Qi, rq);

		glTranslatef(-r1.x, -r1.y, -r1.z);
	}
}
/*
void CGLView::OnZoomExtents(Fl_Widget* pw, void* pd)
{
	CDocument* pdoc = m_pParent->GetDocument();
	BOUNDINGBOX box = pdoc->GetBoundingBox();

	CGLCamera* pcam = &GetCamera();
	pcam->SetTarget(box.Center());
	pcam->SetTargetDistance(3.f*box.Radius());

	Redraw();
}

void CGLView::OnZoomRect(Fl_Widget* pw, void* pd)
{
	m_bZoomRect = !m_bZoomRect;
}

void CGLView::OnZoomSelect(Fl_Widget* pw, void* pd)
{
	CDocument* pdoc = m_pParent->GetDocument();
	BOUNDINGBOX box = pdoc->GetSelectionBox();

	CGLCamera* pcam = &GetCamera();
	pcam->SetTarget(box.Center());
	pcam->SetTargetDistance(3.f*box.Radius());

	Redraw();
}
*/

void CGLView::SetRenderStyle(int nstyle)
{
	CDocument* pdoc = GetDocument();
	VIEWSETTINGS& view = pdoc->GetViewSettings();
	CGLModel* po = pdoc->GetGLModel();

	GLCOLOR c = view.fgcol;

	int i;
	switch (nstyle)
	{
	case RENDER_DEFAULT:
		{
			for (i=0; i<m_Widget->Widgets(); ++i) (*m_Widget)[i]->set_fg_color(c.r,c.g,c.b);
			po->m_boutline = false;
			po->m_bghost = false;
			view.m_nproj = RENDER_PERSP;
		}
		break;
	case RENDER_CAD:
		{
			for (i=0; i<m_Widget->Widgets(); ++i) (*m_Widget)[i]->set_fg_color(c.r,c.g,c.b);
			po->m_boutline = true;
			po->m_bghost = false;
			view.m_nproj = RENDER_ORTHO;
		}
		break;
	}
}


void CGLView::SetView(View_Mode n)
{
	quat4f q;
	switch (n)
	{
	case VIEW_FRONT : q = quat4f(-90*DEG2RAD, vec3f(1,0,0)); break;
	case VIEW_BACK  : q = quat4f(180*DEG2RAD, vec3f(0,1,0)); q *= quat4f(-90*DEG2RAD, vec3f(1,0,0)); break;
	case VIEW_RIGHT : q = quat4f(-90*DEG2RAD, vec3f(1,0,0)); q *= quat4f( 90*DEG2RAD, vec3f(0,0,1)); break;
	case VIEW_LEFT  : q = quat4f(-90*DEG2RAD, vec3f(0,1,0)); q *= quat4f(-90*DEG2RAD, vec3f(1,0,0)); break;
	case VIEW_TOP   : q = quat4f(0, vec3f(1,0,0)); break;
	case VIEW_BOTTOM: q = quat4f(180*DEG2RAD, vec3f(1,0,0)); break;
	default:
		assert(false);
	}

	m_nview = n;

	CDocument* pdoc = GetDocument();
	CGLCamera* pcam = &GetCamera();
	pcam->SetOrientation(q);
	repaint();
}

void CGLView::TogglePerspective()
{
	CDocument* pdoc = GetDocument();
	VIEWSETTINGS& view = pdoc->GetViewSettings();

	view.m_nproj = (view.m_nproj == RENDER_ORTHO? RENDER_PERSP : RENDER_ORTHO);
	repaint();
}
/*
void CGLView::OnPopup(Fl_Widget* pw, void* pd)
{
	switch(m_pop->value())
	{
	case 0: TogglePerspective(); break;
	case 1: SetView(VIEW_FRONT ); break;
	case 2: SetView(VIEW_BACK  ); break;
	case 3: SetView(VIEW_LEFT  ); break;
	case 4: SetView(VIEW_RIGHT ); break;
	case 5: SetView(VIEW_TOP   ); break;
	case 6: SetView(VIEW_BOTTOM); break;
	}
}
*/

void CGLView::NewAnimation(const char* szfile, CAnimation* panim, GLenum fmt)
{
	m_panim = panim;
	SetVideoFormat(fmt);

	// get the width/height of the animation
	int cx = m_pframe->w();
	int cy = m_pframe->h();

	// get the frame rate
	float fps = 0.f; //m_wnd->GetTimeController()->GetFPS();
	if (fps == 0.f) fps = 10.f;

	// create the animation
	if (m_panim->Create(szfile, cx, cy, fps) == false)
	{
//		flx_error("Failed creating animation stream.");
		delete m_panim;
		m_panim = 0;
		m_nanim = ANIM_STOPPED;
	}
	else
	{
		// lock the frame
		m_pframe->lock(true);

		// set the animation mode to paused
		m_nanim = ANIM_PAUSED;
	}
}

void CGLView::StartAnimation()
{
	if (m_panim)
	{
		// set the animation mode to recording
		m_nanim = ANIM_RECORDING;

		repaint();
	}
}

void CGLView::StopAnimation()
{
	if (m_panim)
	{
		// stop the animation
		m_nanim = ANIM_STOPPED;

		// close the stream
		m_panim->Close();

		// delete the object
		delete m_panim;
		m_panim = 0;

		// unlock the frame
		m_pframe->lock(false);

		repaint();
	}
}

void CGLView::PauseAnimation()
{
	if (m_panim)
	{
		// pause the recording
		m_nanim = ANIM_PAUSED;
		repaint();
	}
}

void CGLView::Global2Screen(vec3f r, int n[2])
{
	vec3f q = Global2Local(r);
	GLdouble g[2];
	Local2View(q, g);
	View2Screen(g[0], g[1], n);
}

vec3f CGLView::Global2Local(vec3f r)
{
	CGLCamera& cam = GetCamera();
	vec3f pos = cam.GetPosition();
	quat4f qi = cam.GetOrientation().Inverse();
	float D = cam.GetTargetDistance();
	return qi*(r + pos) + vec3f(0,0,D);
}

void CGLView::Local2View(vec3f r, GLdouble g[2])
{
	VIEWSETTINGS& view = GetDocument()->GetViewSettings();

	if (view.m_nproj == RENDER_PERSP)
	{
		g[0] = m_fnear*r.x/r.z;
		g[1] = m_fnear*r.y/r.z;
	}
	else
	{
		g[0] = r.x;
		g[1] = r.y;
	}
}

void CGLView::View2Screen(double x, double y, int n[2])
{
	const double DEG_TO_RAD = 3.1415926/180.0;

	double wy = 2.0*m_fnear*tan(DEG_TO_RAD*m_fov*0.5);
	double wx = m_ar*wy;

	int vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);

	n[0] = vp[0] + (int) ((wx/2 + x)/wx * vp[2]);
	n[1] = vp[1] + (int) ((wy/2 + y)/wy * vp[3]);
}

void CGLView::TrackSelection(bool b)
{
	if (b==false)
	{
		m_btrack = false;
	}
	else
	{
		m_btrack = false;
		CDocument* pdoc = GetDocument();
		if (pdoc->IsValid())
		{
			int nmode = pdoc->GetSelectionMode();
			FEMesh* pm = pdoc->GetFEModel()->GetMesh();
			if (nmode == SELECT_ELEMS)
			{
				for (int i=0; i<pm->Elements(); ++i) 
					if (pm->Element(i).IsSelected()) 
					{
						int* n = pm->Element(i).m_node;
						m_ntrack[0] = n[0];
						m_ntrack[1] = n[1];
						m_ntrack[2] = n[2];
						m_btrack = true; 
						break; 
					}
			}
			else if (nmode == SELECT_NODES)
			{
				int ns = 0;
				for (int i=0; i<pm->Nodes(); ++i)
				{
					if (pm->Node(i).IsSelected()) m_ntrack[ns++] = i;
					if (ns == 3)
					{
						m_btrack = true;
						break;
					}
				}
			}
		}
	}
}
