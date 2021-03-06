/*This file is part of the PostView source code and is licensed under the MIT license
listed below.

See Copyright-PostView.txt for details.

Copyright (c) 2020 University of Utah, The Trustees of Columbia University in 
the City of New York, and others.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#pragma once
#include <vector>
#include <list>
#include <string>
#include "PostLib/FEMaterial.h"
#include <FSCore/box.h>
#include <GLLib/GView.h>
#include <PostLib/FEPostMesh.h>
#include <PostGL/GLModel.h>

//-----------------------------------------------------------------------------
// Forward declarations
class CMainWindow;
class CDocument;

namespace Post {
	class FEPostModel;
	class FEState;
	class FEFileReader;
	class FEDataField;
	class CImageModel;
	class CGLPlot;
	class CGLObject;
	class CGLVisual;
	class CPalette;
}

//-----------------------------------------------------------------------------
#define MAX_STRING		256
#define MAX_GLMESHES	5

#define VIEW_TEMPLATE			0
#define VIEW_DEFORMED_TEMPLATE	1
#define VIEW_TARGET				2
#define VIEW_DIFFERENCE			3

//-----------------------------------------------------------------------------

// Timer modes
#define MODE_FORWARD	0
#define MODE_REVERSE	1
#define MODE_CYLCE		2

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
		bool	m_boutline;	// render as outline
		bool	m_bghost;	// render ghost
		bool	m_bShell2Hex; // render shells as hexes
		int		m_nshellref;	// shell reference surface
		int		m_nDivs;	// nr of element subdivisions
		int		m_nrender;	// render mode
		double	m_smooth;	// smoothing angle
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
	ModelData(Post::CGLModel* po);
	void SetData(Post::CGLModel* po);

protected:
	MODEL						m_mdl;	// CGLModel data
	COLORMAP					m_cmap;	// CColorMap data
	DISPLACEMENTMAP				m_dmap;	// DisplacementMap data
	std::vector<Post::FEMaterial>		m_mat;	// material list
	std::vector<std::string>	m_data;	// data field strings
};

//-----------------------------------------------------------------------------
// Class that can be used to monitor changes to the document
class CDocObserver
{
public:
	CDocObserver(CDocument* doc);
	virtual ~CDocObserver();

	// bnewFlag is set when a new model was loaded
	virtual void DocumentUpdate(bool bnewFlag) {}

	// this function is called when the document is about to be deleted
	virtual void DocumentDelete();

	// get the document
	CDocument* GetDocument() { return m_doc; }

private:
	CDocument*	m_doc;
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
	Post::FEPostModel* GetFEModel() { return m_fem;}

	// set the FE model 
	// TODO: only PlotMix uses this. Maybe I can delete this
	void SetFEModel(Post::FEPostModel* pnew);

	// update the FE model data
	void UpdateFEModel(bool breset = false);

	// update all the states
	void UpdateAllStates();

	// get the number of time steps
	int GetTimeSteps();

	// get the model's bounding box
	BOX GetBoundingBox();

	// get the selection bounding box
	BOX GetSelectionBox();

	// get the extents bounding box
	// (i.e. only visible parts contribute)
	BOX GetExtentsBox();

	// --- T I M E   M A N A G M E N T ---

	// get the index of the active state
	int currentTime();

	// set the active state
	void SetCurrentTime(int ntime);
	void SetCurrentTimeValue(float ftime);

	// get the of active state
	float GetTimeValue();

	// get the time value of a state
	float GetTimeValue(int n);

	// get the time range
	void GetTimeRange(double& t0, double& t1);

	// --- I/O - R O U T I N E S ---

	// load a project from file
	bool LoadFEModel(Post::FEFileReader* preader, const char* szfile, bool bup = false);

	// export routines
	bool ExportPlot  (const char* szfile, bool bflag[6], int ncode[6]);
	bool ExportBYU   (const char* szfile);
	bool ExportASE   (const char* szfile);
	bool ExportDXF   (const char* szfile);
	bool ExportRAW   (const char* szfile);

	void ApplyPalette(const Post::CPalette& pal);

	// get the current file name
	const std::string& GetFile() const;

	void SetFile(const std::string& fileName);

	std::string GetFileName();
	int GetFilePath(char* szpath);
	int GetDocTitle(char* sztitle);

	string GetTitle() const;
	void SetTitle(const string& title);

	int GetEvalField();

	std::string GetFieldString();

	TIMESETTINGS& GetTimeSettings() { return m_time; }

	Post::FEPostMesh* GetActiveMesh() { return m_pGLModel->GetActiveMesh(); }

	// --- V I E W   M A N A G M E N T ---

	// get the current view
	CGView* GetView() { return &m_view; }

	// get/set light position
	vec3f GetLightPosition() { return m_light; }
	void SetLightPosition(vec3f lp) { m_light = lp; }

	// selection
	// TODO: Maybe we can let the GLView take care of this
	void SelectNodesInRange(float fmin, float fmax, bool bsel);
	void SelectEdgesInRange(float fmin, float fmax, bool bsel);
	void SelectFacesInRange(float fmin, float fmax, bool bsel);
	void SelectElemsInRange(float fmin, float fmax, bool bsel);

	// update all views
	void UpdateViews();

	// reset the view
	void ResetView();

	// reset camera
	void ZoomExtents(bool bhit = false);

	// update all the color maps in use (called after the View Settings dialog)
	void UpdateColorMaps();

	// --- O B J E C T   M A N A G M E N T ---

	// get the GL model
	Post::CGLModel* GetGLModel() { return m_pGLModel; }

	// edits plots
	void DeleteObject(Post::CGLObject* po);

	// edit visuals
	void AddObject(Post::CGLVisual* po) { m_pObj.push_back(po); }
	list<Post::CGLVisual*>& GetObjectList() { return m_pObj; }

	// ---------------------------------------

	// load RAW image from file
	bool LoadRAWImage(const std::string& fileName, int dim[3], BOX range);

	// add a 3D image
	void AddImageModel(Post::CImageModel* img);

	// number of image models
	int ImageModels() const { return (int)m_img.size(); }

	// get image model
	Post::CImageModel* GetImageModel(int i) { return m_img[i]; }

	CMainWindow* GetWindow() { return m_wnd; }

public:
	void AddObserver(CDocObserver* observer);
	void RemoveObserver(CDocObserver* observer);
	void UpdateObservers(bool bnew);

protected:
	void ClearPlots();
	void ClearObjects();

protected:
	Post::FEPostModel*		m_fem;	// the FE model

	CMainWindow*	m_wnd;	// main window

	Post::CGLModel*			m_pGLModel;	// the GL Model
	list<Post::CGLVisual*>	m_pObj;		// additional objects

	std::vector<Post::CImageModel*>	m_img;

	Post::FEFileReader*	m_pImp;			// last used file importer
	std::string			m_fileName;		// file name of current model

	// the view data
	CGView			m_view;		// view orientation/position
	vec3f			m_light;	// lightposition // TODO: should I move this to the CGView class?

	// timer data
	TIMESETTINGS	m_time;

	// miscellenaeous
	bool	m_bValid;	// the document is loaded and valid

private:
	std::vector<CDocObserver*>	m_Observers;
};
