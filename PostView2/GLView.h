#pragma once
#include <QOpenGLWidget>
#include <PostViewLib/math3d.h>
#include <PostViewLib/Animation.h>
#include <PostViewLib/FEElement.h>
#include <PostViewLib/Intersect.h>
#include <GLWLib/GLWidgetManager.h>
#include <QNativeGestureEvent>
#include "ViewSettings.h"

//-----------------------------------------------------------------------------
// forward declarations
class CMainWindow;
class CDocument;

namespace Post {
	class FEMeshBase;
	class CGLModel;
	class CGLContext;
}

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
	Post::FEMeshBase*	m_pm;
	int		m_node;
};

//-----------------------------------------------------------------------------
class SelectRegion
{
public:
	SelectRegion(){}
	virtual ~SelectRegion(){}

	virtual bool IsInside(int x, int y) const = 0;

	// see if a line intersects this region
	// default implementation only checks if one of the end points is inside.
	// derived classes should provide a better implementation.
	virtual bool LineIntersects(int x0, int y0, int x1, int y1) const;

	// see if a triangle intersects this region
	// default implementation checks for line intersections
	virtual bool TriangleIntersect(int x0, int y0, int x1, int y1, int x2, int y2) const;
};

class BoxRegion : public SelectRegion
{
public:
	BoxRegion(int x0, int y0, int x1, int y1);
	bool IsInside(int x, int y) const;
	bool LineIntersects(int x0, int y0, int x1, int y1) const;
private:
	int	m_x0, m_x1;
	int	m_y0, m_y1;
};

class CircleRegion : public SelectRegion
{
public:
	CircleRegion(int x0, int y0, int x1, int y1);
	bool IsInside(int x, int y) const;
	bool LineIntersects(int x0, int y0, int x1, int y1) const;
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

namespace Post {
	class CGLCamera;
}

class CGLView : public QOpenGLWidget
{
	Q_OBJECT

public:
	CGLView(CMainWindow* pwnd, QWidget* parent = 0);
	~CGLView();

	Post::CGLCamera& GetCamera();

	QImage CaptureScreen();

	void TrackSelection(bool b);

	void setPerspective(bool b);

	void showSafeFrame(bool b);

	void showWidgets(bool b);

	void SetView(View_Mode nview);

	void OnZoomExtents();
	void OnZoomSelect ();

	CDocument* GetDocument();

	VIEWSETTINGS& GetViewSettings();

	int GetProjectionMode();
    
    int GetViewConvention();

	double GetFOV() { return m_fov; }
	double GetAspectRatio() { return m_ar; }
	double GetNearPlane() { return m_fnear; }
	double GetFarPlane() { return m_ffar; }

	void GetViewport(int vp[4])
	{
		vp[0] = m_viewport[0];
		vp[1] = m_viewport[1];
		vp[2] = m_viewport[2];
		vp[3] = m_viewport[3];
	}

	void PositionCamera();

	void UpdateCamera(bool hitCameraTarget = false);

	void UpdateWidgets(bool bposition = true);

public:
	bool NewAnimation(const char* szfile, CAnimation* panim, GLenum fmt = GL_RGB);
	void StartAnimation();
	void StopAnimation();
	void PauseAnimation();
	void SetVideoFormat(GLenum fmt) { m_video_fmt = fmt; }

	ANIMATION_MODE AnimationMode() const;
	bool HasRecording() const;

	bool FindFaceIntersection(const Post::Ray& ray, const Post::FEMeshBase& mesh, Post::Intersection& q);
	bool FindElementIntersection(const Post::Ray& ray, const Post::FEMeshBase& mesh, Post::Intersection& q);

protected:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

	void mousePressEvent  (QMouseEvent* ev);
	void mouseMoveEvent   (QMouseEvent* ev);
	void mouseReleaseEvent(QMouseEvent* ev);
	void mouseDoubleClickEvent(QMouseEvent* ev);
	void wheelEvent(QWheelEvent* ev);
	void keyPressEvent(QKeyEvent* ev);
    bool gestureEvent(QNativeGestureEvent* ev);
	bool event(QEvent* event);

protected slots:
	void repaintEvent();
	
protected:
	void Clear();

	void RenderBkGround(GLColor c1, GLColor c2, int style);
	void RenderRubberBand();
	void RenderWidgets();
	void RenderBox(const BOUNDINGBOX& box);
	void RenderPlots(Post::CGLContext& rc);

	// calculate a ray based on screen position (x, y)
	Post::Ray PointToRay(int x, int y);

	// Render the model
	void RenderModel();

	// Render the model (for real)
	void RenderDoc();

	void SetupGL();

	void SelectElements(int x0, int y0, int mode);
	void SelectFaces   (int x0, int y0, int mode);
	void SelectNodes   (int x0, int y0, int mode);
	void SelectEdges   (int x0, int y0, int mode);

	void RegionSelectElements(const SelectRegion& region, int mode);
	void RegionSelectFaces   (const SelectRegion& region, int mode);
	void RegionSelectNodes   (const SelectRegion& region, int mode);
	void RegionSelectEdges   (const SelectRegion& region, int mode);

	void TagBackfacingElements(Post::FEMeshBase& mesh);
	void TagBackfacingFaces(Post::FEMeshBase& mesh);
	void TagBackfacingNodes(Post::FEMeshBase& mesh);
	void TagBackfacingEdges(Post::FEMeshBase& mesh);

	void AddRegionPoint(int x, int y);

	//! Render the tags on the selected items
	void RenderTags();

	void ZoomRect(MyPoint p0, MyPoint p1);

/*	void OnZoomRect   (Fl_Widget* pw, void* pd);
	void OnPopup(Fl_Widget* pw, void* pd);
*/

	void AddWidget(GLWidget* pw);

	bool DeleteWidget(GLWidget* pw);

	void RenderTrack();

public:
	// convert from device pixel to physical pixel
    static int DevicePixelRatio() { return m_dpr; }
    static QPoint DeviceToPhysical(int x, int y)
    {
        return QPoint(m_dpr*x, m_viewport[3] - m_dpr*y);
    }

protected:
	void setupProjectionMatrix();

protected:
	int OnPush   (int nevent);
	int OnKeyDown(int nevent);
	int OnDrag   (int nevent);
	int OnRelease(int nevent);

protected:

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
	bool			m_bShowWidgets;
	GLBox*			m_ptitle;
	GLBox*			m_psubtitle;
	GLTriad*		m_ptriad;
	GLSafeFrame*	m_pframe;
	CGLWidgetManager*	m_Widget;
	char			m_szsubtitle[256];

	// used for selection
	vector<pair<int,int> >		m_pl;
	bool	m_bdrag;	// a selection box is being dragged
	MyPoint	m_p0;		// first point of selection
	MyPoint	m_p1;		// second point of selection
	int		m_xp, m_yp;
	bool	m_bsingle;

	bool	m_bZoomRect;	// zoom rect activated

	GLenum	m_video_fmt;

	ANIMATION_MODE	m_nanim;	// the animation mode
	CAnimation*		m_panim;	// animation object

private:
	static int	m_viewport[4];		//!< store viewport coordinates
	static int m_dpr;				//!< device pixel ratio for converting from physical to device-independent pixels
};
