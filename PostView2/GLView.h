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
class CGLModel;
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

//-----------------------------------------------------------------------------
class SelectRegion
{
public:
	SelectRegion(){}
	virtual ~SelectRegion(){}

	virtual bool IsInside(int x, int y) const = 0;
};

class BoxRegion : public SelectRegion
{
public:
	BoxRegion(int x0, int x1, int y0, int y1);
	bool IsInside(int x, int y) const;
private:
	int	m_x0, m_x1;
	int	m_y0, m_y1;
};

class CircleRegion : public SelectRegion
{
public:
	CircleRegion(int x0, int x1, int y0, int y1);
	bool IsInside(int x, int y) const;
private:
	int	m_xc, m_yc;
	int	m_R;
};

class FreeRegion : public SelectRegion
{
public:
	FreeRegion(vector<pair<int, int> >& pl);
	bool IsInside(int x, int y) const;
private:
	vector<pair<int, int> >& m_pl;
	int m_x0, m_x1;
	int m_y0, m_y1;
};

class CGLView : public QOpenGLWidget
{
	Q_OBJECT


	enum {MAX_FEEDBACK_BUFFER_SIZE = 10000000};

public:
	CGLView(CMainWindow* pwnd, QWidget* parent = 0);
	~CGLView();

	CGLCamera& GetCamera();

	QImage CaptureScreen();

	void TrackSelection(bool b);

	void setPerspective(bool b);

	void showSafeFrame(bool b);

	void SetView(View_Mode nview);

	void OnZoomExtents();
	void OnZoomSelect ();

	CDocument* GetDocument();

protected:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

	void mousePressEvent  (QMouseEvent* ev);
	void mouseMoveEvent   (QMouseEvent* ev);
	void mouseReleaseEvent(QMouseEvent* ev);
	void mouseDoubleClickEvent(QMouseEvent* ev);
	void wheelEvent(QWheelEvent* ev);
	bool event(QEvent* event);

	void contextMenuEvent(QContextMenuEvent* ev);

protected slots:
	void repaintEvent();
	
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
	void SelectEdges   (int x0, int y0, int x1, int y1, int mode);

	void RegionSelectElements(const SelectRegion& region, int mode);
	void RegionSelectFaces   (const SelectRegion& region, int mode);
	void RegionSelectNodes   (const SelectRegion& region, int mode);
	void RegionSelectEdges   (const SelectRegion& region, int mode);

	void AddRegionPoint(int x, int y);

	FEElement* PickElement(int x, int y);
	GLNODE_PICK PickNode(int x, int y, bool bselect = false);

	//! Render the tags on the selected items
	void RenderTags();

	void ZoomRect(MyPoint p0, MyPoint p1);

/*	void OnZoomRect   (Fl_Widget* pw, void* pd);
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

	void SetRenderStyle(int nstyle);

	void NewAnimation(const char* szfile, CAnimation* panim, GLenum fmt = GL_RGB);
	void StartAnimation();
	void StopAnimation();
	void PauseAnimation();

	vec3f Global2Local(vec3f r);
	void Local2View(vec3f r, GLdouble g[2]);
	void View2Screen(double x, double y, int n[2]);

	void Global2Screen(vec3f r, int n[2]);

	void RenderTrack();

	void SetVideoFormat(GLenum fmt) { m_video_fmt = fmt; }

protected:
	void setupProjectionMatrix();

	int getFeedback(GLfloat* feedbackBuffer, int bufferSize, void (CGLModel::*renderFunc)(void));
	void parseFeedbackBuffer(GLfloat* feedbackBuffer, int hits, const SelectRegion& region, vector<int>& list, GLfloat token);

protected:
	int OnPush   (int nevent);
	int OnKeyDown(int nevent);
	int OnDrag   (int nevent);
	int OnRelease(int nevent);

protected:
	void PositionCam();

	void PanView(vec3f r);


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
	vector<pair<int,int> >		m_pl;
	bool	m_bdrag;	// a selection box is being dragged
	MyPoint	m_p0;		// first point of selection
	MyPoint	m_p1;		// second point of selection
	int		m_xp, m_yp;

	bool	m_bZoomRect;	// zoom rect activated

	GLenum	m_video_fmt;

	ANIMATION_MODE	m_nanim;	// the animation mode
	CAnimation*		m_panim;	// animation object
};
