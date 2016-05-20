#pragma once
#include <vector>
#include <list>
#include <string>
#include <PostViewLib/FEMaterial.h>
#include <PostViewLib/bbox.h>
#include <PostViewLib/GView.h>

//-----------------------------------------------------------------------------
// Forward declarations
class CMainWindow;
class CGLModel;
class CGLPlot;
class CGLObject;
class CGLVisual;
class C3DImage;
class CVolRender;

class FEModel;
class FEState;
class FEFileReader;
class FEDataField;

//-----------------------------------------------------------------------------
#define MAX_STRING		256
#define MAX_GLMESHES	5

#define VIEW_TEMPLATE			0
#define VIEW_DEFORMED_TEMPLATE	1
#define VIEW_TARGET				2
#define VIEW_DIFFERENCE			3

// Background styles
#define BG_COLOR_1		0
#define BG_COLOR_2		1
#define BG_FADE_HORZ	2
#define BG_FADE_VERT	3
#define BG_FADE_DIAG	4

// the selection modes
#define SELECT_NODES	1
#define SELECT_FACES	2
#define SELECT_ELEMS	4
#define SELECT_ADD		8
#define SELECT_SUB		16

// render styles
#define RENDER_DEFAULT	1
#define RENDER_CAD		2

// projection modes
#define RENDER_ORTHO	0
#define RENDER_PERSP	1

// Timer modes
#define MODE_FORWARD	0
#define MODE_REVERSE	1
#define MODE_CYLCE		2

//-----------------------------------------------------------------------------
typedef std::list<CGLPlot*>	GPlotList;

//-----------------------------------------------------------------------------
// View Settings
struct VIEWSETTINGS
{
	GLCOLOR	bgcol1;
	GLCOLOR	bgcol2;
	GLCOLOR	fgcol;

	int		bgstyle;

	bool	m_bShadows;
	float	m_shadow_intensity;
	float	m_ambient;		// scene light ambient intensity
	float	m_diffuse;		// scene light diffuse inentisty

	bool	m_bTriad;		// show triad or not
	bool	m_bTags;		// show tags or not
	bool	m_bTitle;		// show title or not
	bool	m_bconn;		// select connected
	bool	m_bext;			// ignore interior flag

	bool	m_bmesh;		// show mesh or not
	bool	m_bBox;			// show bounding box or not
	int		m_nproj;		// projection mode or not
	bool	m_bLighting;	// use lighting or not
	bool	m_bcull;		// use backface culling or not

	// OpenGL settings
	bool	m_blinesmooth;	// line smoothing
	float	m_flinethick;	// line thickness
	float	m_fpointsize;	// point size

	void Defaults();
};

//-----------------------------------------------------------------------------
struct TIMESETTINGS
{
	int		m_mode;		// play mode
	double	m_fps;		// frames per second
	int		m_start;	// start time
	int		m_end;		// end time
	int		m_inc;		// used when MODE_CYCLE
	bool	m_bloop;	// loop or not
	bool	m_bfix;		// use a fixed time step
	double	m_dt;		// fixed time step size

	void Defaults();
};

//-----------------------------------------------------------------------------
// model data which is used for file updates
class ModelData
{
private:
	struct MODEL
	{
		bool	m_bnorm;	// calculate normals or not
		bool	m_bsmooth;	// render smooth or not
		bool	m_boutline;	// render as outline
		bool	m_bghost;	// render ghost
		bool	m_bShell2Hex; // render shells as hexes
		int		m_nshellref;	// shell reference surface
		int		m_nDivs;	// nr of element subdivisions
		int		m_nrender;	// render mode
	};

	struct COLORMAP
	{
		bool	m_bactive;
		int		m_ntype;
		int		m_ndivs;
		bool	m_bsmooth;	// smooth gradient or not
		float	m_min;
		float	m_max;
		float	m_user[2];	// user range
		bool	m_bDispNodeVals;	// render nodal values
		int		m_nRangeType;	// range type
		int		m_nField;
	};
	
	struct DISPLACEMENTMAP
	{
		int		m_nfield;	// vector field defining the displacement
		float	m_scale;	// displacement scale factor
	};

public:
	ModelData(CGLModel* po);
	void SetData(CGLModel* po);

protected:
	MODEL						m_mdl;	// CGLModel data
	COLORMAP					m_cmap;	// CColorMap data
	DISPLACEMENTMAP				m_dmap;	// DisplacementMap data
	std::vector<FEMaterial>		m_mat;	// material list
	std::vector<std::string>	m_data;	// data field strings
};

//-----------------------------------------------------------------------------
class CDocument
{
public:
	CDocument(CMainWindow* pwnd);

	virtual ~CDocument();

	// see if the model is currently valid
	bool IsValid() { return m_bValid; }

	// reset all data
	void Reset();

	// --- F E - M O D E L ---

	// get the FE model
	FEModel* GetFEModel() { return m_fem;}

	// set the FE model 
	// TODO: only PlotMix uses this. Maybe I can delete this
	void SetFEModel(FEModel* pnew);

	// update the FE model data
	void UpdateFEModel(bool breset = false);

	// update all the states
	void UpdateAllStates();

	// get the number of time steps
	int GetTimeSteps();

	// get the model's bounding box
	BOUNDINGBOX GetBoundingBox();

	// get the selection bounding box
	BOUNDINGBOX GetSelectionBox();

	// --- T I M E   M A N A G M E N T ---

	// get the index of the active state
	int currentTime();

	// set the active state
	void SetCurrentTime(int ntime);

	// get the of active state
	float GetTimeValue();

	// get the time range
	void GetTimeRange(double& t0, double& t1);

	// --- I/O - R O U T I N E S ---

	// load a project from file
	bool LoadFEModel(FEFileReader* preader, const char* szfile, bool bup = false);

	// save the current session to file
	bool SaveSession(const char* szfile);

	// open a session from file
	bool OpenSession(const char* szfile);

	// export routines
	bool ExportAscii (const char* szfile);
	bool ExportPlot  (const char* szfile, bool bflag[6], int ncode[6]);
	bool ExportVRML  (const char* szfile);
	bool ExportBYU   (const char* szfile);
	bool ExportASE   (const char* szfile);
	bool ExportDXF   (const char* szfile);
	bool ExportRAW   (const char* szfile);

	bool ExportDataField(const FEDataField& df, const char* szfile);

	// get the current file name
	const char* GetFile() { return m_szfile; }

	int GetFileName(char* szfilename);
	int GetFilePath(char* szpath);
	int GetDocTitle(char* sztitle);

	const char* GetTitle();
	void SetTitle(const char* sztitle);

	int GetEvalField();

	std::string GetFieldString();

	TIMESETTINGS& GetTimeSettings() { return m_time; }

	// --- V I E W   M A N A G M E N T ---

	// get the view settings
	VIEWSETTINGS& GetViewSettings() { return m_ops; }

	// get the current view
	CGView* GetView() { return &m_view; }

	// get/set light position
	vec3f GetLightPosition() { return m_light; }
	void SetLightPosition(vec3f lp) { m_light = lp; }

	// selection
	// TODO: Maybe we can let the GLView take care of this
	void SelectNodesInRange(float fmin, float fmax, bool bsel);
	void SelectFacesInRange(float fmin, float fmax, bool bsel);
	void SelectElemsInRange(float fmin, float fmax, bool bsel);

	// update all views
	void UpdateViews();

	// reset the view
	void ResetView();

	// --- S E L E C T I O N ---

	// get selection mode
	int GetSelectionMode() { return m_mode; }

	// set selection mode
	void SetSelectionMode(int mode) { m_mode = mode; }

	// get a list of selected items
	void GetSelectionList(vector<int>& L);

	// --- O B J E C T   M A N A G M E N T ---

	// get the GL model
	CGLModel* GetGLModel() { return m_pGLModel; }

	// edits plots
	void AddPlot(CGLPlot* pplot) { m_pPlot.push_back(pplot); }
	GPlotList& GetPlotList() { return m_pPlot; }
	void DeleteObject(CGLObject* po);

	// edit visuals
	void AddObject(CGLVisual* po) { m_pObj.push_back(po); }
	list<CGLVisual*>& GetObjectList() { return m_pObj; }

	// ---------------------------------------

	// add a 3D image
	void Add3DImage(C3DImage* pimg, double w = 1.0, double h = 1.0, double d = 1.0);

	C3DImage* Get3DImage() { return m_pImg; }
	CVolRender* GetVolumeRenderer() { return m_pVR; }

	// Add a data field
	// NOTE: the ndata relates to the index in DataPanel::on_AddStandard_triggered
	// TODO: Find a better mechanism
	bool AddStandardDataField(int ndata, bool bselection_only);

	bool AddNodeDataFromFile(const char* szfile, const char* szname, int ntype);
	bool AddElemDataFromFile(const char* szfile, const char* szname, int ntype);

protected:
	void ClearPlots();
	void ClearObjects();

protected:
	FEModel*		m_fem;	// the FE model

	CMainWindow*	m_wnd;	// main window

	CGLModel*			m_pGLModel;	// the GL Model
	list<CGLVisual*>	m_pObj;		// additional objects
	GPlotList			m_pPlot;	// list of plots

	C3DImage*		m_pImg;	//!< 3D image
	CVolRender*		m_pVR;	//!< volume renderer

	FEFileReader*	m_pImp;			// last used file importer
	char			m_szfile[1024];	// file name of current model

	// the view data
	VIEWSETTINGS	m_ops;		// the view settings
	CGView			m_view;		// view orientation/position
	vec3f			m_light;	// lightposition // TODO: should I move this to the CGView class?

	// timer data
	TIMESETTINGS	m_time;

	// miscellenaeous
	bool	m_bValid;	// the document is loaded and valid
	int		m_mode;		// current selection mode
};
