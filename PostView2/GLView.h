#pragma once
#include <QOpenGLWidget>
#include <PostViewLib/math3d.h>
#include <PostViewLib/Animation.h>
#include <PostViewLib/FEElement.h>
#include "GLWidgetManager.h"

//-----------------------------------------------------------------------------
// forward declarations
class CMainWindow;
class CDocument;
class CGLContext;
class FEMesh;

//-----------------------------------------------------------------------------
// Animation modes
enum ANIMATION_MODE {
	ANIM_RECORDING,
	ANIM_PAUSED,
	ANIM_STOPPED
};

//-----------------------------------------------------------------------------
// preset views
enum View_Mode {
	VIEW_USER,
	VIEW_TOP,
	VIEW_BOTTOM,
	VIEW_LEFT,
	VIEW_RIGHT,
	VIEW_FRONT,
	VIEW_BACK
};

//-----------------------------------------------------------------------------
// tag structure
struct GLTAG
{
	char	sztag[32];	// name of tag
	float	wx, wy;		// window coordinates for tag
	vec3f	r;			// world coordinates of tag
	bool	bvis;		// is the tag visible or not
	int		ntag;		// tag value
};

//-----------------------------------------------------------------------------
// MyPoint structure
struct MyPoint
{
	int x, y;
};

inline bool operator == (const MyPoint& p1, const MyPoint& p2)
{
	return (p1.x == p2.x) && (p1.y == p2.y);
}

//-----------------------------------------------------------------------------
// helper structur for picking nodes
struct GLNODE_PICK
{
	FEMesh*	m_pm;
	int		m_node;
};

class CGLView : public QOpenGLWidget
{
public:
	CGLView(CMainWindow* pwnd, QWidget* parent = 0);
	~CGLView();

	CGLCamera& GetCamera();

protected:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

	void mousePressEvent  (QMouseEvent* ev);
	void mouseMoveEvent   (QMouseEvent* ev);
	void mouseReleaseEvent(QMouseEvent* ev);

protected:
	void Clear();

	void RenderBkGround(GLCOLOR c1, GLCOLOR c2, int style);
	void RenderRubberBand();
	void RenderWidgets();
	void RenderBox();
	void RenderPlots(CGLContext& rc);

	// Render the model
	void RenderModel();

	// Render the model (for real)
	void RenderDoc();

	void SetupGL();

	void SelectElements(int x0, int y0, int x1, int y1, int mode);
	void SelectFaces   (int x0, int y0, int x1, int y1, int mode);
	void SelectNodes   (int x0, int y0, int x1, int y1, int mode);

	FEElement* PickElement(int x, int y);
	GLNODE_PICK PickNode(int x, int y, bool bselect = false);

	//! Render the tags on the selected items
	void RenderTags();

	void CaptureScreen(CRGBImage* pim, GLenum format = GL_RGB);

	void ZoomRect(MyPoint p0, MyPoint p1);
/*
	void OnZoomExtents(Fl_Widget* pw, void* pd);
	void OnZoomSelect (Fl_Widget* pw, void* pd);
	void OnZoomRect   (Fl_Widget* pw, void* pd);
	void OnPopup(Fl_Widget* pw, void* pd);
*/

	void AddWidget(GLWidget* pw)
	{
		m_Widget->AddWidget(pw);
	}

	void DeleteWidget(GLWidget* pw)
	{
		// we only delete user-created widget.
		// user-created widgets do not have an owning object
		if (pw->GetObject() == 0)
		{
			m_Widget->RemoveWidget(pw);
		}
	}

	void ToggleSafeFrame()
	{
		if (m_pframe->visible()) m_pframe->hide();
		else m_pframe->show();
	}

	void SetRenderStyle(int nstyle);

	void NewAnimation(const char* szfile, CAnimation* panim, GLenum fmt = GL_RGB);
	void StartAnimation();
	void StopAnimation();
	void PauseAnimation();

	void TogglePerspective();
	void SetView(View_Mode nview);

	vec3f Global2Local(vec3f r);
	void Local2View(vec3f r, GLdouble g[2]);
	void View2Screen(double x, double y, int n[2]);

	void Global2Screen(vec3f r, int n[2]);

	void TrackSelection();
	void RenderTrack();

	void SetVideoFormat(GLenum fmt) { m_video_fmt = fmt; }

protected:
	int OnPush   (int nevent);
	int OnKeyDown(int nevent);
	int OnDrag   (int nevent);
	int OnRelease(int nevent);

protected:
	void PositionCam();

	void PanView(vec3f r);

	CDocument* GetDocument();

private:
	CMainWindow*	m_wnd;

	double	m_fov;
	double	m_fnear;
	double	m_ffar;
	double	m_ar;

	View_Mode	m_nview;

	// tracking
	bool	m_btrack;
	int		m_ntrack[3];

	// the GL widgets
	GLBox*			m_ptitle;
	GLBox*			m_psubtitle;
	GLTriad*		m_ptriad;
	GLSafeFrame*	m_pframe;
	CGLWidgetManager*	m_Widget;

	// used for selection
	bool	m_bdrag;	// a selection box is being dragged
	MyPoint	m_p0;		// first point of selection
	MyPoint	m_p1;		// second point of selection
	int		m_xp, m_yp;

	bool	m_bZoomRect;	// zoom rect activated

	GLenum	m_video_fmt;

	ANIMATION_MODE	m_nanim;	// the animation mode
	CAnimation*		m_panim;	// animation object
};
