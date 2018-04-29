#include "stdafx.h"
#include "Document.h"
#include "MainWindow.h"
#include <PostViewLib/FEModel.h>
#include <PostViewLib/FEFileReader.h>
#include <PostViewLib/3DImage.h>
#include <PostViewLib/VolRender.h>
#include <PostViewLib/ImageSlicer.h>
#include <PostViewLib/FEMeshData_T.h>
#include "GLPlot.h"
#include "GLModel.h"
#include <PostViewLib/constants.h>
#include <PostViewLib/Palette.h>
#include <PostViewLib/FEAsciiExport.h>
#include "GLModel.h"

#ifdef WIN32
	#include "direct.h"	// for getcwd, chdir
	#define getcwd _getcwd
	#define chdir _chdir
#endif 
#ifdef LINUX
	#include "unistd.h"	// for getcwd, chdir
#endif
#ifdef __APPLE__
    #include "unistd.h"	// for getcwd, chdir
#endif

void VIEWSETTINGS::Defaults()
{
	bgcol1 = GLCOLOR(255,255,255);
	bgcol2 = GLCOLOR(128,128,255);
	fgcol  = GLCOLOR(0,0,0);
	bgstyle = BG_FADE_VERT;
	m_shadow_intensity = 0.5f;
	m_bmesh		 = false;
	m_boutline   = true;
	m_bShadows    = false;
	m_bTriad      = true;
	m_bTags       = true;
	m_ntagInfo    = 0;
	m_bTitle      = true;
	m_bconn		 = false;
	m_bext       = true;
	m_bBox        = false;
	m_nproj       = RENDER_PERSP;
	m_bLighting   = true;
	m_ambient     = 0.3f;
	m_diffuse     = 0.7f;
	m_bignoreBackfacingItems = true;
	m_blinesmooth = true;
	m_flinethick  = 1.0f;
	m_fpointsize  = 6.0f;
}

void TIMESETTINGS::Defaults()
{
	m_mode  = MODE_FORWARD;
	m_fps   = 10.0;
	m_start = 1;
	m_end   = 0;	//	has to be set after loading a model
	m_bloop = true;
	m_bfix  = false;
	m_inc   = 1;
	m_dt    = 0.01;
}

//-----------------------------------------------------------------------------
ModelData::ModelData(CGLModel *po)
{
	if (po == 0) return;

	// set model props
	m_mdl.m_bnorm      = po->m_bnorm;
	m_mdl.m_bsmooth    = po->m_bsmooth;
	m_mdl.m_bghost     = po->m_bghost;
	m_mdl.m_bShell2Hex = po->m_bShell2Hex;
	m_mdl.m_nshellref  = po->m_nshellref;
	m_mdl.m_nDivs      = po->m_nDivs;
	m_mdl.m_nrender    = po->m_nrender;
	m_mdl.m_smooth     = po->GetSmoothingAngle();

	// set colormap props
	CGLColorMap* pglmap = po->GetColorMap();
	if (pglmap)
	{
		m_cmap.m_bactive = pglmap->IsActive();
		m_cmap.m_nRangeType = pglmap->GetRangeType();
		m_cmap.m_bDispNodeVals = pglmap->DisplayNodalValues();
		m_cmap.m_nField = pglmap->GetEvalField();
		pglmap->GetRange(m_cmap.m_user);

		CColorTexture* pcm = pglmap->GetColorMap();
		m_cmap.m_ntype = pcm->GetColorMap();
		m_cmap.m_ndivs = pcm->GetDivisions();
		m_cmap.m_bsmooth = pcm->GetSmooth();
//		pcm->GetRange(m_cmap.m_min, m_cmap.m_max);
	}

	// displacement map
	FEModel* ps = po->GetFEModel();
	m_dmap.m_nfield = ps->GetDisplacementField();

	// materials 
	int N = ps->Materials();
	m_mat.resize(N);
	for (int i=0; i<N; ++i) m_mat[i] = *ps->GetMaterial(i);

	// store the data field strings
	m_data.clear();
	FEDataManager* pDM = ps->GetDataManager();
	FEDataFieldPtr pdf = pDM->FirstDataField();
	for (int i=0; i<pDM->DataFields(); ++i, ++pdf) m_data.push_back(string((*pdf)->GetName()));
}

void ModelData::SetData(CGLModel* po)
{
	// set model data
	po->m_bnorm      = m_mdl.m_bnorm;
	po->m_bsmooth    = m_mdl.m_bsmooth;
	po->m_bghost     = m_mdl.m_bghost;
	po->m_bShell2Hex = m_mdl.m_bShell2Hex;
	po->m_nshellref  = m_mdl.m_nshellref;
	po->m_nDivs      = m_mdl.m_nDivs;
	po->m_nrender    = m_mdl.m_nrender;
	po->SetSmoothingAngle(m_mdl.m_smooth);

	// set color map data
	CGLColorMap* pglmap = po->GetColorMap();
	if (pglmap)
	{
		pglmap->SetRangeType(m_cmap.m_nRangeType);
		pglmap->SetRange(m_cmap.m_user);
		pglmap->DisplayNodalValues(m_cmap.m_bDispNodeVals);
		pglmap->SetEvalField(m_cmap.m_nField);
		pglmap->Activate(m_cmap.m_bactive);

		CColorTexture* pcm = pglmap->GetColorMap();
		pcm->SetColorMap(m_cmap.m_ntype);
		pcm->SetDivisions(m_cmap.m_ndivs);
		pcm->SetSmooth(m_cmap.m_bsmooth);
//		pcm->SetRange(m_cmap.m_min, m_cmap.m_max);
	}

	// displacement map
	FEModel* ps = po->GetFEModel();
	ps->SetDisplacementField(m_dmap.m_nfield);

	// materials
	if (!m_mat.empty())
	{
		int N0 = (int)m_mat.size();
		int N1 = ps->Materials();
		int N = (N0<N1?N0:N1);
		for (int i=0; i<N; ++i) *ps->GetMaterial(i) = m_mat[i];

		// update the mesh state
		FEMeshBase* pmesh = po->GetActiveMesh();
		for (int i=0; i<N; ++i)
		{
			FEMaterial* pm = ps->GetMaterial(i);
			if (pm->bvisible == false) po->HideMaterial(i);
		}
	}

	// reload data fields
	int ndata = (int)m_data.size();
	FEDataManager* pDM = ps->GetDataManager();
	FEDataFieldPtr pdf;
	for (int i=0; i<ndata; ++i)
	{
		string& si = m_data[i];

		// see if the model already defines this field
		bool bfound = false;
		pdf = pDM->FirstDataField();
		for (int i=0; i<pDM->DataFields(); ++i, ++pdf)
		{
			if (si.compare((*pdf)->GetName()) == 0) { bfound = true; break; }
		}

		if (bfound == false)
		{
			FEDataField* pdf = 0;
			if      (si.compare("Position"                  ) == 0) pdf = new FEDataField_T<FENodePosition         >("Position"                  );
			else if (si.compare("Initial position"          ) == 0) pdf = new FEDataField_T<FENodeInitPos          >("Initial position"          );
			else if (si.compare("Infinitesimal strain"      ) == 0) pdf = new FEDataField_T<FEInfStrain            >("Infinitesimal strain"      );
			else if (si.compare("Lagrange strain"           ) == 0) pdf = new FEDataField_T<FELagrangeStrain       >("Lagrange strain"           );
			else if (si.compare("Deformation gradient"      ) == 0) pdf = new FEDataField_T<FEDeformationGradient  >("Deformation gradient"      );
			else if (si.compare("Right Cauchy-Green"        ) == 0) pdf = new FEDataField_T<FERightCauchyGreen     >("Right Cauchy-Green"        );
			else if (si.compare("Right stretch"             ) == 0) pdf = new FEDataField_T<FERightStretch         >("Right stretch"             );
			else if (si.compare("Biot strain"               ) == 0) pdf = new FEDataField_T<FEBiotStrain           >("Biot strain"               );
			else if (si.compare("Right Hencky"              ) == 0) pdf = new FEDataField_T<FERightHencky          >("Right Hencky"              );
            else if (si.compare("Left Cauchy-Green"         ) == 0) pdf = new FEDataField_T<FELeftCauchyGreen      >("Left Cauchy-Green"         );
            else if (si.compare("Left stretch"              ) == 0) pdf = new FEDataField_T<FELeftStretch          >("Left stretch"              );
            else if (si.compare("Left Hencky"               ) == 0) pdf = new FEDataField_T<FELeftHencky           >("Left Hencky"               );
            else if (si.compare("Almansi strain"            ) == 0) pdf = new FEDataField_T<FEAlmansi              >("Almansi strain"            );
			else if (si.compare("Volume"                    ) == 0) pdf = new FEDataField_T<FEElementVolume        >("Volume"                    );
			else if (si.compare("Volume ratio"              ) == 0) pdf = new FEDataField_T<FEVolRatio             >("Volume ratio"              );
			else if (si.compare("Volume strain"             ) == 0) pdf = new FEDataField_T<FEVolStrain            >("Volume strain"             );
			else if (si.compare("Aspect ratio"              ) == 0) pdf = new FEDataField_T<FEAspectRatio          >("Aspect ratio"              );
			else if (si.compare("1-Princ curvature"         ) == 0) pdf = new FECurvatureField("1-Princ curvature"         , FECurvatureField::PRINC1_CURVATURE);
			else if (si.compare("2-Princ curvature"         ) == 0) pdf = new FECurvatureField("2-Princ curvature"         , FECurvatureField::PRINC2_CURVATURE);
			else if (si.compare("Gaussian curvature"        ) == 0) pdf = new FECurvatureField("Gaussian curvature"        , FECurvatureField::GAUSS_CURVATURE );
			else if (si.compare("Mean curvature"            ) == 0) pdf = new FECurvatureField("Mean curvature"            , FECurvatureField::MEAN_CURVATURE  );
			else if (si.compare("RMS curvature"             ) == 0) pdf = new FECurvatureField("RMS curvature"             , FECurvatureField::RMS_CURVATURE   );
			else if (si.compare("Princ curvature difference") == 0) pdf = new FECurvatureField("Princ curvature difference", FECurvatureField::DIFF_CURVATURE  );
			else if (si.compare("Congruency"                ) == 0) pdf = new FEDataField_T<FECongruency           >("Congruency"                );
			else if (si.compare("1-Princ curvature vector"  ) == 0) pdf = new FEDataField_T<FEPrincCurvatureVector1>("1-Princ curvature vector");
			else if (si.compare("2-Princ curvature vector"  ) == 0) pdf = new FEDataField_T<FEPrincCurvatureVector2>("2-Princ curvature vector");

			if (pdf) ps->AddDataField(pdf);
		}
	}
}

CDocument::CDocument(CMainWindow* pwnd) : m_wnd(pwnd)
{
	m_bValid = false;

	m_szfile[0] = 0;

	m_pImg = 0;
	m_pVR = 0;
	m_pIS = 0;

	m_fem = 0;
	m_pGLModel = 0;
	m_pImp = 0;
	Reset();

	// initialize view settings
	m_ops.Defaults();
	m_time.Defaults();

	FEElementLibrary::InitLibrary();
}

CDocument::~CDocument()
{
	if (m_pGLModel) delete m_pGLModel;
	if (m_pImp) delete m_pImp;
	ClearPlots();
	ClearObjects();
}

void CDocument::ClearPlots()
{
	list<CGLPlot*>::iterator it = m_pPlot.begin();
	for (int i=0; i<(int) m_pPlot.size(); ++i, ++it) delete (*it);
	m_pPlot.clear();
}

void CDocument::ClearObjects()
{
	list<CGLVisual*>::iterator it = m_pObj.begin();
	for (int i=0; i<(int) m_pObj.size(); ++i, ++it) delete (*it);
	m_pObj.clear();
}

//-----------------------------------------------------------------------------
void CDocument::AddObserver(CDocObserver* observer)
{
	// no duplicates allowed
	for (int i=0; i<m_Observers.size(); ++i)
	{
		if (m_Observers[i] == observer)
		{
			assert(false);
			return;
		}
	}

	m_Observers.push_back(observer);
}

//-----------------------------------------------------------------------------
void CDocument::RemoveObserver(CDocObserver* observer)
{
	for (int i = 0; i<m_Observers.size(); ++i)
	{
		if (m_Observers[i] == observer)
		{
			m_Observers.erase(m_Observers.begin() + i);
			return;
		}
	}
	assert(false);
}

//-----------------------------------------------------------------------------
void CDocument::UpdateObservers()
{
	if (m_Observers.empty()) return;

	for (int i=0; i<m_Observers.size(); ++i)
	{
		CDocObserver* observer = m_Observers[i];
		if (observer) observer->DocumentUpdate(this);
	}
}

//-----------------------------------------------------------------------------
void CDocument::Reset()
{
	m_bValid = false;

	// set the initial camera position
	m_view.Reset();

	// set the light position
	m_light = vec3f(1,0,2);

	// clear the plot list
	ClearPlots();

	// clear the object list
	ClearObjects();

	if (m_pImg) { delete m_pImg; m_pImg = 0; }
	if (m_pVR ) { delete m_pVR ; m_pVR  = 0; }
	if (m_pIS ) { delete m_pIS ; m_pIS  = 0; }
}

//-----------------------------------------------------------------------------
int CDocument::GetEvalField()
{
	if (m_pGLModel == 0) return -1;
	CGLColorMap* pc = m_pGLModel->GetColorMap();
	if (pc == 0) return -1;

	return pc->GetEvalField();
}

//-----------------------------------------------------------------------------
std::string CDocument::GetFieldString() 
{ 
	if (IsValid())
	{
		int nfield = GetGLModel()->GetColorMap()->GetEvalField();
		return GetFEModel()->GetDataManager()->getDataString(nfield, DATA_SCALAR);
	}
	else return "";
}

//-----------------------------------------------------------------------------
int CDocument::currentTime() 
{ 
	if (m_pGLModel)
		return m_pGLModel->currentTimeIndex(); 
	else return -1;
}

//-----------------------------------------------------------------------------
float CDocument::GetTimeValue()
{ 
	if (m_pGLModel) return m_pGLModel->currentTime(); 
	else return 0.f;
}

//-----------------------------------------------------------------------------
float CDocument::GetTimeValue(int n)
{
	if (m_pGLModel) return m_pGLModel->GetTimeValue(n);
	else return 0.f;
}

//-----------------------------------------------------------------------------
void CDocument::UpdateFEModel(bool breset)
{
	if (!m_bValid) return;

	// update the model
	if (m_pGLModel) m_pGLModel->Update(breset);

	// update the plot list
	list<CGLPlot*>::iterator it = m_pPlot.begin();
	for (int i=0; i<(int) m_pPlot.size(); ++i, ++it) 
		if ((*it)->IsActive()) (*it)->Update(m_pGLModel->currentTimeIndex(), 0.0, breset);
}

//-----------------------------------------------------------------------------
void CDocument::ResetView()
{
	// get the boundingbox
	BOUNDINGBOX box = GetBoundingBox();

	// reset the camera
	CGLCamera& cam = m_view.GetCamera();
	cam.Reset();
	cam.SetTargetDistance(box.Radius()*3);
	cam.SetTarget(box.Center());

	// set the current time
	// this will also update the scene
	SetCurrentTime(0);
}

//-----------------------------------------------------------------------------
void CDocument::ZoomExtents(bool bhit)
{
	BOUNDINGBOX box = GetExtentsBox();
	if (box.IsValid())
	{
		CGLCamera& cam = m_view.GetCamera();
		cam.SetTarget(box.Center());
		cam.SetTargetDistance(3.f*box.Radius());
		if (bhit) cam.Update(bhit);
	}
}

//-----------------------------------------------------------------------------
// update all the color maps in use (called after the View Settings dialog)
void CDocument::UpdateColorMaps()
{
	if (m_pGLModel == 0) return;

	CGLColorMap* map = m_pGLModel->GetColorMap();
	map->GetColorMap()->UpdateTexture();

	int N = (int)m_pPlot.size();
	GPlotList::iterator it = m_pPlot.begin();
	for (int i=0; i<N; ++i, ++it)
	{
		CGLPlot* p = *it;
		p->UpdateTexture();
	}
}

//-----------------------------------------------------------------------------
void CDocument::ApplyPalette(const CPalette& pal)
{
	int NCOL = pal.Colors();
	int nmat = m_fem->Materials();
	for (int i = 0; i<nmat; i++)
	{
		GLCOLOR c = pal.Color(i % NCOL);

		FEMaterial& m = *m_fem->GetMaterial(i);
		m.diffuse = c;
		m.ambient = c;
		m.specular = GLCOLOR(128, 128, 128);
		m.emission = GLCOLOR(0, 0, 0);
		m.shininess = 0.5f;
		m.transparency = 1.f;
	}
}

//-----------------------------------------------------------------------------
bool CDocument::LoadFEModel(FEFileReader* pimp, const char* szfile, bool bup)
{
	const int MAXLINE = 512;
	char* ch;

	// store the file importer
	if (pimp)
	{
		if (m_pImp) delete m_pImp;
		m_pImp = pimp;
	}
	else if (m_pImp == 0) return false;

	// extract the path of the szfile
	char szpath[MAXLINE];
	strcpy(szpath, szfile);
	if ((ch = strrchr(szpath, '/')) == 0)
	{
		if ((ch = strrchr(szpath, '\\')) == 0) szpath[0] = 0;
		else ch[1] = 0;
	}
	else ch[1] = 0;

	// extract the file title
	const char* sztitle = 0;
	const char* ch2 = strrchr(szfile, '/');
	if (ch2 == 0) 
	{
		ch2 = strrchr(szfile, '\\'); 
		if (ch2 == 0) ch2 = szfile; else ++ch2;
	}
	else ++ch2;
	sztitle = ch2;

	// change the current workdirectory
	char szwdir[MAXLINE] = {0};
	if (szpath[0])
	{
		getcwd(szwdir, MAXLINE);
		chdir(szpath);
	}

	// if this is a file update, store the model data settings
	ModelData MD(m_pGLModel);

	// get the current time index
	int ntime = (m_pGLModel ? m_pGLModel->currentTimeIndex() : 0);

	// remove the old object
	delete m_pGLModel; m_pGLModel = 0;

	// remove the old scene
	m_bValid = false;
	delete m_fem;

	// create a new scene
	m_fem= new FEModel;

	// set the file name as title
	m_fem->SetTitle(sztitle);

	// load the scene
	if (m_pImp->Load(*m_fem, szfile) == false)
	{
		delete m_fem;
		m_fem = 0;
		return false;
	}

	// assign material attributes
	const CPalette& pal = CPaletteManager::CurrentPalette();
	ApplyPalette(pal);

	// create a new model
	m_pGLModel = new CGLModel(m_fem);

	// go back to the original working directory
	if (szwdir) chdir(szwdir);

	if (ntime >= GetTimeSteps()-1) ntime = GetTimeSteps()-1;

	m_time.m_start = 0;
	m_time.m_end   = GetTimeSteps() - 1;

	// reset settings
	if (bup)
	{
		MD.SetData(m_pGLModel);

		// Since the GL model has changed, we need to update all plots
		// that keep a reference to the model
		list<CGLPlot*>::iterator it;
		for (it = m_pPlot.begin(); it != m_pPlot.end(); ++it) (*it)->SetModel(m_pGLModel);

		m_bValid = true;
		SetCurrentTime(ntime);
	}
	else
	{
		// reset the view
		Reset();

		// create a new model
		m_bValid = true;

		// get the boundingbox
		BOUNDINGBOX box = GetBoundingBox();

		// reset the camera
		CGLCamera& cam = m_view.GetCamera();
		cam.Reset();
		cam.SetTargetDistance(box.Radius()*3);
		cam.SetTarget(box.Center());
		cam.Update(true);

		// set the current time
		// this will also update the scene
		SetCurrentTime(0);
	}

	// make sure the model is up to date
	UpdateFEModel(true);

	// store a copy of the imorpted file
	strcpy(m_szfile, szfile);

	// it's all good !
	return true;
}

//-----------------------------------------------------------------------------
void CDocument::SetCurrentTime(int ntime) 
{
	if (m_bValid && m_pGLModel)
	{
		m_pGLModel->setCurrentTimeIndex(ntime);
		UpdateFEModel();
	}
}

//-----------------------------------------------------------------------------
void CDocument::SetCurrentTimeValue(float ftime)
{
	if (m_bValid && m_pGLModel)
	{
		m_pGLModel->SetTimeValue(ftime);
		UpdateFEModel();
	}
}

//-----------------------------------------------------------------------------
void CDocument::GetTimeRange(double& t0, double& t1)
{
	if (m_fem)
	{
		t0 = m_fem->GetState(0)->m_time;
		t1 = m_fem->GetState(GetTimeSteps()-1)->m_time;
	}
}

//-----------------------------------------------------------------------------
void CDocument::SetFEModel(FEModel* pnew)
{
	// remove the old GL object
	delete m_pGLModel; m_pGLModel = 0;

	// remove the old FE model
	m_bValid = false;
	delete m_fem;

	// set the new scene
	m_fem = pnew;

	// create a new model
	m_pGLModel = new CGLModel(m_fem);

	// reset the view
	Reset();

	// create a new model
	m_bValid = true;

	// get the boundingbox
	BOUNDINGBOX box = GetBoundingBox();

	// reset the camera
	CGLCamera& cam = m_view.GetCamera();
	cam.Reset();
	cam.SetTargetDistance(box.Radius()*3);
	cam.SetTarget(box.Center());

	// set the current time
	// this will also update the scene
	SetCurrentTime(0);

	// make sure we can't update this file
	m_pImp = 0;
	m_szfile[0] = '\0';
}

bool CDocument::ExportDXF(const char* szfile)
{
/*	FILE* fp = fopen(szfile, "wt");
	if (fp == 0) return false;

	int meshes = m_GLScene.Meshes();
	int nodes = m_GLScene.Nodes();
	int faces = m_GLScene.Faces();
	int i, j;

	// create the header section
	fprintf(fp, "0\nSECTION\n2\nHEADER\n9\n$ACADVER\n1\nAC1006\n0\nENDSEC\n");

	// create the entities section
	fprintf(fp, "0\nSECTION\n2\nENTITIES\n");

	// write the polyline
	fprintf(fp, "0\nPOLYLINE\n8\nobject1\n66\n1\n70\n64\n");

	// write the nr. of vertices
	fprintf(fp,"71\n%d\n", nodes);
	
	// write the nr. of faces
	fprintf(fp,"72\n%d\n", faces);

	// write the vertex data
	CGLMesh* pm;
	vec3f r;

	for (i=0; i<meshes; i++)
	{
		pm = m_GLScene.GetMesh(i);
		CGLNode* pn = pm->Node(0);
		for (j=0; j<pm->Nodes(); j++, pn++)
		{
			fprintf(fp, "0\nVERTEX\n8\nobject1\n");
			r = pn->m_pos;
			fprintf(fp, "10\n%g\n20\n%g\n30\n%g\n70\n192\n", r.x, r.y, r.z);
		}
	}

	// write the face data
	int noff = 0;
	int a, b, c;
	for (i=0; i<meshes; i++)
	{
		pm = m_GLScene.GetMesh(i);
		CGLFace* pf = pm->Face(0);
		for (j=0; j<pm->Faces(); j++, pf++)
		{
			a = (pf->m_node[0] + noff)+1;
			b = (pf->m_node[1] + noff)+1;
			c = -((pf->m_node[2] + noff)+1);
			fprintf(fp, "0\nVERTEX\n8\nobject1\n");
			fprintf(fp, "10\n0\n20\n0\n30\n0\n70\n128\n71\n%d\n72\n%d\n73\n%d\n", a, b, c);
		}

		noff += pm->Nodes();
	}

	// wrap it up
	fprintf(fp, "0\nSEQEND\n8\nobject1\n0\nENDSEC\n0\nEOF\n");

	fclose(fp);
*/
	return false;
}

bool CDocument::ExportRAW(const char* szfile)
{
/*	FILE* fp = fopen(szfile, "wt");
	if (fp == 0) return false;

	int meshes = m_GLScene.Meshes();
	int i, j;

	CGLMesh* pm;
	vec3f r0, r1, r2;

	for (i=0; i<meshes; i++)
	{
		pm = m_GLScene.GetMesh(i);
		CGLFace* pf = pm->Face(0);
		CGLNode* pn = pm->Node(0);

		for (j=0; j<pm->Faces(); j++, pf++)
		{
			r0 = pn[pf->m_node[0]].m_pos;
			r1 = pn[pf->m_node[1]].m_pos;
			r2 = pn[pf->m_node[2]].m_pos;

			fprintf(fp, "%g %g %g %g %g %g %g %g %g\n", r0.x, r0.y, r0.z, r1.x, r1.y, r1.z, r2.x, r2.y, r2.z);
		}
	}

	fclose(fp);
*/
	return false;
}

bool CDocument::ExportASE(const char* szfile)
{
/*	int i, m, *n;

	FILE* fp = fopen(szfile, "wt");
	if (fp == 0) return false;

	FEMeshBase& fem = *m_FEScene.GetMesh();
	int nodes = fem.Nodes();
	int elems = fem.Elements();

	fprintf(fp, "* Title     : %s\n", m_FEScene.GetTitle());
	fprintf(fp, "* Nodes     : %d\n", fem.Nodes());
	fprintf(fp, "* Elements  : %d\n", fem.Elements());
	fprintf(fp, "* Materials : %d\n", m_FEScene.Materials());
	fprintf(fp, "* Time      : %g (%d/%d)\n", GetTimeValue(), currentTime()+1, GetTimeSteps());
	fprintf(fp, "* Values    : %s\n", GetFieldString());

	// export nodes
	fprintf(fp, "* NODES\n");
	FENode* pn = fem.NodePtr(0);
	float* pv = fem.GetState(currentTime())->m_pval;
	for (i=0; i<nodes; i++, pn++, pv++)
	{
		vec3f& r = pn->m_r0;
		fprintf(fp, "%8d,%20.13g,%20.13g,%20.13g,%20.13g\n", i+1, r.x, r.y, r.z, *pv);
	}
	fprintf(fp, "* END NODES\n\n");

	// export elements
	fprintf(fp, "* ELEMENTS\n");
	FEElement* pe = fem.ElementPtr(0);
	for (i=0; i<elems; i++, pe++)
	{
		n = pe->m_node;
		m = pe->m_MatID;

		fprintf(fp, "%8d,%5d,%8d,%8d,%8d,%8d,%8d,%8d,%8d,%8d\n",i+1,m+1,n[0]+1,n[1]+1,n[2]+1,n[3]+1,n[4]+1,n[5]+1,n[6]+1,n[7]+1);
	}
	fprintf(fp, "* END ELEMENTS\n");

	fclose(fp);
*/
	return false;
}

//------------------------------------------------------------------------------------------
// NOTE: the ndata relates to the index in DataPanel::on_AddStandard_triggered
// TODO: Find a better mechanism
bool CDocument::AddStandardDataField(int ndata, bool bselection_only)
{
	FEDataField* pdf = 0;
	switch (ndata)
	{
	case  0: pdf = new FEDataField_T<FENodePosition         >("Position"                  ); break;
	case  1: pdf = new FEDataField_T<FENodeInitPos          >("Initial position"          ); break;
	case  2: pdf = new FEDataField_T<FEDeformationGradient  >("Deformation gradient"      ); break;
	case  3: pdf = new FEDataField_T<FEInfStrain            >("Infinitesimal strain"      ); break;
	case  4: pdf = new FEDataField_T<FELagrangeStrain       >("Lagrange strain"           ); break;
	case  5: pdf = new FEDataField_T<FERightCauchyGreen     >("Right Cauchy-Green"        ); break;
	case  6: pdf = new FEDataField_T<FERightStretch         >("Right stretch"             ); break;
	case  7: pdf = new FEDataField_T<FEBiotStrain           >("Biot strain"               ); break;
	case  8: pdf = new FEDataField_T<FERightHencky          >("Right Hencky"              ); break;
    case  9: pdf = new FEDataField_T<FELeftCauchyGreen      >("Left Cauchy-Green"         ); break;
    case 10: pdf = new FEDataField_T<FELeftStretch          >("Left stretch"              ); break;
    case 11: pdf = new FEDataField_T<FELeftHencky           >("Left Hencky"               ); break;
    case 12: pdf = new FEDataField_T<FEAlmansi              >("Almansi strain"            ); break;
	case 13: pdf = new FEDataField_T<FEElementVolume        >("Volume"                    ); break;
	case 14: pdf = new FEDataField_T<FEVolRatio             >("Volume ratio"              ); break;
	case 15: pdf = new FEDataField_T<FEVolStrain            >("Volume strain"             ); break;
	case 16: pdf = new FEDataField_T<FEAspectRatio          >("Aspect ratio"              ); break;
	case 17: pdf = new FECurvatureField("1-Princ curvature"         , FECurvatureField::PRINC1_CURVATURE); break;
	case 18: pdf = new FECurvatureField("2-Princ curvature"         , FECurvatureField::PRINC2_CURVATURE); break;
	case 19: pdf = new FECurvatureField("Gaussian curvature"        , FECurvatureField::GAUSS_CURVATURE ); break;
	case 20: pdf = new FECurvatureField("Mean curvature"            , FECurvatureField::MEAN_CURVATURE  ); break;
	case 21: pdf = new FECurvatureField("RMS curvature"             , FECurvatureField::RMS_CURVATURE   ); break;
	case 22: pdf = new FECurvatureField("Princ curvature difference", FECurvatureField::DIFF_CURVATURE  ); break;
	case 23: pdf = new FEDataField_T<FECongruency           >("Congruency"                ); break;
	case 24: pdf = new FEDataField_T<FEPrincCurvatureVector1>("1-Princ curvature vector"  ); break;
	case 25: pdf = new FEDataField_T<FEPrincCurvatureVector2>("2-Princ curvature vector"  ); break;
	default:
		return false;
	}

	FEModel& fem = *GetFEModel();

	CGLModel* model = GetGLModel();
	if (model)
	{
		// NOTE: This only works with curvatures
		if (bselection_only && (model->GetSelectionMode() == SELECT_FACES))
		{
			vector<int> L;
			model->GetSelectionList(L, model->GetSelectionMode());
			if (L.empty() == false) fem.AddDataField(pdf, L);
			else fem.AddDataField(pdf);
		}
		else fem.AddDataField(pdf);
	}

	return true;
}

//------------------------------------------------------------------------------------------
bool CDocument::AddNodeDataFromFile(const char* szfile, const char* szname, int ntype)
{
	FILE* fp = fopen(szfile, "rt");
	if (fp == 0) return false;

	// get the mesh
	FEModel* pm = GetFEModel();
	FEMeshBase& m = *GetActiveMesh();

	// create a new data field
	int ND = 0;
	switch (ntype)
	{
	case DATA_FLOAT  : pm->AddDataField(new FEDataField_T<FENodeData<float  > >(szname, EXPORT_DATA)); ND = 1; break;
	case DATA_VEC3F  : pm->AddDataField(new FEDataField_T<FENodeData<vec3f  > >(szname, EXPORT_DATA)); ND = 3; break;
	case DATA_MAT3D  : pm->AddDataField(new FEDataField_T<FENodeData<mat3d  > >(szname, EXPORT_DATA)); ND = 9; break;
	case DATA_MAT3F  : pm->AddDataField(new FEDataField_T<FENodeData<mat3f  > >(szname, EXPORT_DATA)); ND = 9; break;
	case DATA_MAT3FS : pm->AddDataField(new FEDataField_T<FENodeData<mat3fs > >(szname, EXPORT_DATA)); ND = 6; break;
	case DATA_MAT3FD : pm->AddDataField(new FEDataField_T<FENodeData<mat3fd > >(szname, EXPORT_DATA)); ND = 3; break;
    case DATA_TENS4FS: pm->AddDataField(new FEDataField_T<FENodeData<tens4fs> >(szname, EXPORT_DATA)); ND = 21; break;
	default:
		assert(false);
		fclose(fp);
		return false;
	}

	// the data should be organized in a comma seperated list. 
	// the first entry identifies the node for which the data is intended
	// the second and following contain the data, one entry for each state
	char szline[8192] = {0}, *ch, *sz;
	int N = 0;
	do
	{
		// read a line
		fgets(szline, 8191, fp);
		sz = szline;

		// read the first entry
		ch = strchr(szline, ',');
		if (ch) *ch = 0;
		int node = atoi(sz)-1;
		if (ch) sz = ch+1;

		if ((node >= 0) && (node < m.Nodes()))
		{
			int nstate = 0;
			while (ch && (nstate < pm->GetStates()))
			{
				float f[21] = {0};
				int nf = 0;
				do
				{
					f[nf++] = (float) atof(sz);
					ch = strchr(sz, ',');
					if (ch) { *ch = 0; sz = ch+1; }
				}
				while (ch && (nf < ND));

				// get the state
				FEState* ps = pm->GetState(nstate);

				int ndf = ps->m_Data.size();

				// get the datafield
				switch (ntype)
				{
				case DATA_FLOAT:
					{
						FENodeData<float>& df = dynamic_cast<FENodeData<float>&>(ps->m_Data[ndf-1]);
						df[node] = f[0];
					}
					break;
				case DATA_VEC3F:
					{
						FENodeData<vec3f>& df = dynamic_cast<FENodeData<vec3f>&>(ps->m_Data[ndf-1]);
						df[node] = vec3f(f[0], f[1], f[2]);
					}
					break;
				case DATA_MAT3D:
					{
						FENodeData<mat3d>& df = dynamic_cast<FENodeData<mat3d>&>(ps->m_Data[ndf-1]);
						df[node] = mat3d(f[0], f[1], f[2], f[3], f[4], f[5], f[6], f[7], f[8]);
					}
					break;
				case DATA_MAT3F:
					{
						FENodeData<mat3f>& df = dynamic_cast<FENodeData<mat3f>&>(ps->m_Data[ndf-1]);
						df[node] = mat3f(f[0], f[1], f[2], f[3], f[4], f[5], f[6], f[7], f[8]);
					}
					break;
				case DATA_MAT3FS:
					{
						FENodeData<mat3fs>& df = dynamic_cast<FENodeData<mat3fs>&>(ps->m_Data[ndf-1]);
						df[node] = mat3fs(f[0], f[1], f[2], f[3], f[4], f[5]);
					}
					break;
				case DATA_MAT3FD:
					{
						FENodeData<mat3fd>& df = dynamic_cast<FENodeData<mat3fd>&>(ps->m_Data[ndf-1]);
						df[node] = mat3fd(f[0], f[1], f[2]);
					}
					break;
                case DATA_TENS4FS:
					{
						FENodeData<tens4fs>& df = dynamic_cast<FENodeData<tens4fs>&>(ps->m_Data[ndf-1]);
						df[node] = tens4fs(f);
					}
                    break;
				};

				++nstate;
			}
		}
		++N;
	}
	while(!feof(fp) && !ferror(fp) && (N < m.Nodes()));

	fclose(fp);
	return true;
}

//------------------------------------------------------------------------------------------
bool CDocument::AddElemDataFromFile(const char* szfile, const char* szname, int ntype)
{
	FILE* fp = fopen(szfile, "rt");
	if (fp == 0) return false;

	// get the mesh
	FEModel* pm = GetFEModel();
	FEMeshBase& m = *GetActiveMesh();

	// create a new data field
	int ND = 0;
	switch (ntype)
	{
	case DATA_FLOAT  : pm->AddDataField(new FEDataField_T<FEElementData<float  , DATA_ITEM> >(szname, EXPORT_DATA)); ND = 1; break;
	case DATA_VEC3F  : pm->AddDataField(new FEDataField_T<FEElementData<vec3f  , DATA_ITEM> >(szname, EXPORT_DATA)); ND = 3; break;
	case DATA_MAT3F  : pm->AddDataField(new FEDataField_T<FEElementData<mat3f  , DATA_ITEM> >(szname, EXPORT_DATA)); ND = 9; break;
	case DATA_MAT3D  : pm->AddDataField(new FEDataField_T<FEElementData<mat3d  , DATA_ITEM> >(szname, EXPORT_DATA)); ND = 9; break;
	case DATA_MAT3FS : pm->AddDataField(new FEDataField_T<FEElementData<mat3fs , DATA_ITEM> >(szname, EXPORT_DATA)); ND = 6; break;
	case DATA_MAT3FD : pm->AddDataField(new FEDataField_T<FEElementData<mat3fd , DATA_ITEM> >(szname, EXPORT_DATA)); ND = 3; break;
    case DATA_TENS4FS: pm->AddDataField(new FEDataField_T<FEElementData<tens4fs, DATA_ITEM> >(szname, EXPORT_DATA)); ND = 21; break;
	default:
		assert(false);
		fclose(fp);
		return false;
	}

	// the data should be organized in a comma seperated list. 
	// the first entry identifies the element for which the data is intended
	// the second and following contain the data, one entry for each state
	char szline[8192] = {0}, *ch, *sz;
	int N = 0;
	do
	{
		// read a line
		fgets(szline, 8191, fp);
		sz = szline;

		// read the first entry
		ch = strchr(szline, ',');
		if (ch) *ch = 0;
		int nelem = atoi(sz)-1;
		if (ch) sz = ch+1;

		if ((nelem >= 0) || (nelem < m.Elements()))
		{
			int nstate = 0;
			while (ch && (nstate < pm->GetStates()))
			{
				float f[21] = {0};
				int nf = 0;
				do
				{
					f[nf++] = (float) atof(sz);
					ch = strchr(sz, ',');
					if (ch) { *ch = 0; sz = ch+1; }
				}
				while (ch && (nf < ND));

				// get the state
				FEState* ps = pm->GetState(nstate);

				int ndf = ps->m_Data.size();

				// get the datafield
				switch (ntype)
				{
				case DATA_FLOAT:
					{
						FEElementData<float, DATA_ITEM>& df = dynamic_cast<FEElementData<float, DATA_ITEM>&>(ps->m_Data[ndf-1]);
						df.add(nelem, f[0]);
					}
					break;
				case DATA_VEC3F:
					{
						FEElementData<vec3f, DATA_ITEM>& df = dynamic_cast<FEElementData<vec3f, DATA_ITEM>&>(ps->m_Data[ndf-1]);
						df.add(nelem, vec3f(f[0], f[1], f[2]));
					}
					break;
				case DATA_MAT3F:
					{
						FEElementData<mat3f, DATA_ITEM>& df = dynamic_cast<FEElementData<mat3f, DATA_ITEM>&>(ps->m_Data[ndf-1]);
						df.add(nelem, mat3f(f[0], f[1], f[2], f[3], f[4], f[5], f[6], f[7], f[8]));
					}
					break;
				case DATA_MAT3D:
					{
						FEElementData<mat3d, DATA_ITEM>& df = dynamic_cast<FEElementData<mat3d, DATA_ITEM>&>(ps->m_Data[ndf-1]);
						df.add(nelem, mat3d(f[0], f[1], f[2], f[3], f[4], f[5], f[6], f[7], f[8]));
					}
					break;
				case DATA_MAT3FS:
					{
						FEElementData<mat3fs, DATA_ITEM>& df = dynamic_cast<FEElementData<mat3fs, DATA_ITEM>&>(ps->m_Data[ndf-1]);
						df.add(nelem, mat3fs(f[0], f[1], f[2], f[3], f[4], f[5]));
					}
					break;
				case DATA_MAT3FD:
					{
						FEElementData<mat3fd, DATA_ITEM>& df = dynamic_cast<FEElementData<mat3fd, DATA_ITEM>&>(ps->m_Data[ndf-1]);
						df.add(nelem, mat3fd(f[0], f[1], f[2]));
					}
					break;
                case DATA_TENS4FS:
					{
						FEElementData<tens4fs, DATA_ITEM>& df = dynamic_cast<FEElementData<tens4fs, DATA_ITEM>&>(ps->m_Data[ndf-1]);
						df.add(nelem, tens4fs(f));
					}
                    break;
				}

				++nstate;
			}
		}
		++N;
	}
	while(!feof(fp) && !ferror(fp) && (N < m.Elements()));

	fclose(fp);
	return true;
}

//------------------------------------------------------------------------------------------
bool CDocument::ExportDataField(const FEDataField& df, const char* szfile)
{
	FILE* fp = fopen(szfile, "wt");
	if (fp == 0) return false;

	// get the mesh
	FEModel& fem = *GetFEModel();
	FEMeshBase& mesh = *GetActiveMesh();

	int nstates = fem.GetStates();

	int nfield = df.GetFieldID();
	if (IS_ELEM_FIELD(nfield))
	{
		int ndata = FIELD_CODE(nfield);

		// loop over all elements
		int NE = mesh.Elements();
		for (int i=0; i<NE; ++i)
		{
			FEElement& el = mesh.Element(i);

			// write the element ID
			fprintf(fp, "%d,", i+1);

			// loop over all states
			for (int n = 0; n<nstates; ++n)
			{
				FEState& s = *fem.GetState(n);

				FEMeshData& d = s.m_Data[ndata];
				Data_Format fmt = d.GetFormat();
				if (fmt == DATA_ITEM)
				{
					switch (d.GetType())
					{
					case DATA_FLOAT:
					{
						FEElementData<float, DATA_ITEM>* pf = dynamic_cast<FEElementData<float, DATA_ITEM>*>(&d);
						float f; pf->eval(i, &f);
						fprintf(fp, "%g", f);
					}
					break;
					case DATA_VEC3F:
					{
						FEElementData<vec3f, DATA_ITEM>* pf = dynamic_cast<FEElementData<vec3f, DATA_ITEM>*>(&d);
						vec3f f; pf->eval(i, &f);
						fprintf(fp, "%g,%g,%g", f.x, f.y, f.z);
					}
					break;
					case DATA_MAT3FS:
					{
						FEElementData<mat3fs, DATA_ITEM>* pf = dynamic_cast<FEElementData<mat3fs, DATA_ITEM>*>(&d);
						mat3fs f; pf->eval(i, &f);
						fprintf(fp, "%g,%g,%g,%g,%g,%g", f.x, f.y, f.z, f.xy, f.yz, f.xz);
					}
					break;
					}
				}
				else if (fmt == DATA_COMP)
				{
					int nn = el.Nodes();

					switch (d.GetType())
					{
					case DATA_FLOAT:
					{
						FEElemData_T<float, DATA_COMP>* pf = dynamic_cast<FEElemData_T<float, DATA_COMP>*>(&d);
						float v[FEGenericElement::MAX_NODES]; pf->eval(i, v);
						float f = 0.0f;
						for (int i=0; i<nn; ++i) f += v[i]; f /= (float) nn;
						fprintf(fp, "%g", f);
					}
					break;
					case DATA_VEC3F:
					{
						FEElemData_T<vec3f, DATA_COMP>* pf = dynamic_cast<FEElemData_T<vec3f, DATA_COMP>*>(&d);
						vec3f v[FEGenericElement::MAX_NODES]; pf->eval(i, v);
						vec3f f(0.f, 0.f, 0.f);
						for (int i = 0; i<nn; ++i) f += v[i]; f /= (float)nn;
						fprintf(fp, "%g,%g,%g", f.x, f.y, f.z);
					}
					break;
					case DATA_MAT3FS:
					{
						FEElemData_T<mat3fs, DATA_COMP>* pf = dynamic_cast<FEElemData_T<mat3fs, DATA_COMP>*>(&d);
						mat3fs v[FEGenericElement::MAX_NODES]; pf->eval(i, v);
						mat3fs f(0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
						for (int i = 0; i<nn; ++i) f += v[i]; f /= (float)nn;
						fprintf(fp, "%g,%g,%g,%g,%g,%g", f.x, f.y, f.z, f.xy, f.yz, f.xz);
					}
					break;
					}
				}
				if (n != nstates - 1) fprintf(fp, ",");
			}
			fprintf(fp, "\n");
		}
	}
	fclose(fp);

	return true;
}

bool CDocument::ExportPlot(const char* szfile, bool bflag[6], int ncode[6])
{
	if (IsValid() == false) return true;

//	FELSDYNAPlotExport ex;
//	return ex.Save(*m_fem, szfile, bflag, ncode);
	return false;
}

//-----------------------------------------------------------------------------
BOUNDINGBOX CDocument::GetBoundingBox()
{
	if (IsValid()) return m_fem->GetBoundingBox();
	else
	{
		BOUNDINGBOX b;
		return b;
	}
}

//-----------------------------------------------------------------------------
BOUNDINGBOX CDocument::GetExtentsBox()
{
	BOUNDINGBOX box;
	if (IsValid() == false)
	{
		box.x0 = box.y0 = box.z0 = -1.f;
		box.x1 = box.y1 = box.z1 =  1.f;
		return box;
	}

	FEMeshBase& mesh = *GetActiveMesh();
	int NE = mesh.Elements(), nvis = 0;
	for (int i=0; i<NE; ++i)
	{
		FEElement& el = mesh.Element(i);
		if (el.IsVisible())
		{
			int ne = el.Nodes();
			for (int j=0; j<ne; ++j) box += mesh.Node(el.m_node[j]).m_rt;
			nvis++;
		}
	}

	if (box.IsValid())
	{
		if ((box.Width() < 1e-5) || (box.Height() < 1e-4) || (box.Depth() < 1e-4))
		{
			float R = box.Radius();
			box.InflateTo(R, R, R);
		}
	}

	return box;
}

//-----------------------------------------------------------------------------
// Get the box around the selection
BOUNDINGBOX CDocument::GetSelectionBox()
{
	BOUNDINGBOX box;

	if (IsValid() == false)
	{
		box.x0 = box.y0 = box.z0 = -1.f;
		box.x1 = box.y1 = box.z1 =  1.f;
		return box;
	}

	FEMeshBase& mesh = *GetActiveMesh();
	const vector<FEElement*> selElems = GetGLModel()->GetElementSelection();
	for (int i=0; i<(int)selElems.size(); ++i)
	{
		FEElement& el = *selElems[i];
		int nel = el.Nodes();
		for (int j=0; j<nel; ++j) box += mesh.Node(el.m_node[j]).m_rt;
	}

	const vector<FEFace*> selFaces = GetGLModel()->GetFaceSelection();
	for (int i=0; i<(int)selFaces.size(); ++i)
	{
		FEFace& face = *selFaces[i];
		int nel = face.Nodes();
		for (int j=0; j<nel; ++j) box += mesh.Node(face.node[j]).m_rt;
	}

	const vector<FEEdge*> selEdges = GetGLModel()->GetEdgeSelection();
	for (int i=0; i<(int)selEdges.size(); ++i)
	{
		FEEdge& edge = *selEdges[i];
		int nel = edge.Nodes();
		for (int j=0; j<nel; ++j) box += mesh.Node(edge.node[j]).m_rt;
	}

	const vector<FENode*> selNodes = GetGLModel()->GetNodeSelection();
	for (int i=0; i<(int)selNodes.size(); ++i)
	{
		FENode& node = *selNodes[i];
		box += node.m_rt;
	}

	if (box.IsValid())
	{
		if ((box.Width() < 1e-5) || (box.Height() < 1e-4) || (box.Depth() < 1e-4))
		{
			float R = box.Radius();
			box.InflateTo(R, R, R);
		}
	}

	return box;
}

//-----------------------------------------------------------------------------
bool CDocument::ExportVRML(const char* szfile)
{
//	VRMLExporter exporter;
//	return exporter.Save(m_fem, szfile);
	return false;
}

//-----------------------------------------------------------------------------
bool CDocument::ExportBYU(const char* szfile)
{
/*	int i, j, n;

	FILE* fp = fopen(szfile, "wt");
	if (fp == 0) return false;

	// get the selected mesh
	FEMeshBase& mesh = *m_fem->GetMesh();

	// for now we put everything in one part
	int parts = 1;

	// count nr of faces and face edges
	int faces = 0;
	int edges = 0;
	for (i=0; i<mesh.Nodes(); ++i) mesh.Node(i).m_ntag = 0;
	for (i=0; i<mesh.Faces(); ++i)
	{
		FEFace& f = mesh.Face(i);
		n = f.Nodes();
		assert(n==3||n==4);
		edges += (n==3? 3 : 6) ;
		faces += (n==3? 1 : 2);
		for (j=0; j<n; ++j) mesh.Node(f.node[j]).m_ntag = 1;
	}

	// count nr of nodes
	int nodes = 0;
	for (i=0; i<mesh.Nodes(); ++i) if (mesh.Node(i).m_ntag == 1) mesh.Node(i).m_ntag = ++nodes;

	// --- H E A D E R ---
	fprintf(fp, "%d %d %d %d\n", parts, nodes, faces, edges);

	// --- P A R T ---
	int nfirst = 1;
	int nlast = faces;
	fprintf(fp, "%d %d\n", nfirst, nlast);

	// --- N O D E S ---
	for (i=0; i<mesh.Nodes(); ++i)
	{
		FENode& n = mesh.Node(i);
		if (n.m_ntag)
			fprintf(fp, "%g %g %g\n", n.m_rt.x, n.m_rt.y, n.m_rt.z);
	}

	// --- E D G E S ---
	for (i=0; i<mesh.Faces(); ++i)
	{
		FEFace& f = mesh.Face(i);
		n = f.Nodes();
		if (n == 3)
			fprintf(fp, "%d %d %d\n", mesh.Node(f.node[0]).m_ntag, mesh.Node(f.node[1]).m_ntag, -mesh.Node(f.node[2]).m_ntag);
		else if (n == 4)
		{
			fprintf(fp, "%d %d %d\n", mesh.Node(f.node[0]).m_ntag, mesh.Node(f.node[1]).m_ntag, -mesh.Node(f.node[2]).m_ntag);
			fprintf(fp, "%d %d %d\n", mesh.Node(f.node[2]).m_ntag, mesh.Node(f.node[3]).m_ntag, -mesh.Node(f.node[0]).m_ntag);
		}
		else 
			assert(false);
	}

	fclose(fp);

	return true;
*/
	return false;
}

//-----------------------------------------------------------------------------
void CDocument::SelectElemsInRange(float fmin, float fmax, bool bsel)
{
	FEMeshBase* pm = GetActiveMesh();
	int N = pm->Elements();
	FEState* ps = m_pGLModel->currentState();
	for (int i=0; i<N; ++i)
	{
		FEElement& el = pm->Element(i);
		if (el.IsEnabled() && el.IsVisible() && ((bsel == false) || (el.IsSelected())))
		{
			float v = ps->m_ELEM[i].m_val;
			if ((v >= fmin) && (v <= fmax)) el.Select();
			else el.Unselect();
		}
	}
	m_pGLModel->UpdateSelectionLists();
}

//-----------------------------------------------------------------------------
void CDocument::SelectNodesInRange(float fmin, float fmax, bool bsel)
{
	FEMeshBase* pm = GetActiveMesh();
	int N = pm->Nodes();
	FEState* ps = m_pGLModel->currentState();
	for (int i=0; i<N; ++i)
	{
		FENode& node = pm->Node(i);
		if (node.IsEnabled() && node.IsVisible() && ((bsel == false) || (node.IsSelected())))
		{
			float v = ps->m_NODE[i].m_val;
			if ((v >= fmin) && (v <= fmax)) node.Select();
			else node.Unselect();
		}
	}
	m_pGLModel->UpdateSelectionLists();
}

//-----------------------------------------------------------------------------
void CDocument::SelectEdgesInRange(float fmin, float fmax, bool bsel)
{
	FEMeshBase* pm = GetActiveMesh();
	int N = pm->Edges();
	FEState* ps = m_pGLModel->currentState();
	for (int i=0; i<N; ++i)
	{
		FEEdge& edge = pm->Edge(i);
		if (edge.IsEnabled() && edge.IsVisible() && ((bsel == false) || (edge.IsSelected())))
		{
			float v = ps->m_EDGE[i].m_val;
			if ((v >= fmin) && (v <= fmax)) edge.Select();
			else edge.Unselect();
		}
	}
	m_pGLModel->UpdateSelectionLists();
}

//-----------------------------------------------------------------------------
void CDocument::SelectFacesInRange(float fmin, float fmax, bool bsel)
{
	FEMeshBase* pm = GetActiveMesh();
	FEState* ps = m_pGLModel->currentState();
	int N = pm->Faces();
	for (int i=0; i<N; ++i)
	{
		FEFace& f = pm->Face(i);
		if (f.IsEnabled() && f.IsVisible() && ((bsel == false) || (f.IsSelected())))
		{
			float v = ps->m_FACE[i].m_val;
			if ((v >= fmin) && (v <= fmax)) f.Select();
			else f.Unselect();
		}
	}
	m_pGLModel->UpdateSelectionLists();
}

//-----------------------------------------------------------------------------
void CDocument::UpdateViews()
{
	m_wnd->RedrawGL();
}

//-----------------------------------------------------------------------------
int CDocument::GetFileName(char* szfilename)
{
	char* ch = strrchr(m_szfile, '\\');
	if (ch == 0) 
	{
		ch = strrchr(m_szfile, '/'); 
		if (ch == 0) ch = m_szfile; else ch++;
	} else ch++;

	if (szfilename) strcpy(szfilename, ch); 

	return (int)strlen(ch);
}

int CDocument::GetFilePath(char *szpath)
{
	char* ch = strrchr(m_szfile, '\\');
	if (ch == 0) 
	{
		ch = strrchr(m_szfile, '/'); 
		if (ch == 0) ch = m_szfile; else ++ch;
	}
	else ++ch;

	int n = (int) (ch - m_szfile);

	if (szpath) 
	{
		if (n) strncpy(szpath, m_szfile, n);
		szpath[n] = 0;
	}

	return n;
}

int CDocument::GetDocTitle(char* sztitle)
{
	char* ch = strrchr(m_szfile, '\\');
	if (ch == 0) 
	{
		ch = strrchr(m_szfile, '/'); 
		if (ch == 0) ch = m_szfile; else ch++;
	} else ch++;

	int n;
	char* ce = strrchr(m_szfile, '.');
	if ((ce == 0) || (ce < ch)) n = (int)strlen(ch);
	else n = (int)(ce-ch);

	if (sztitle)
	{
		if (n) strncpy(sztitle, ch, n);
		sztitle[n] = 0;
	}

	return n;
}

void CDocument::AddPlot(CGLPlot* pplot)
{ 
	m_pPlot.push_back(pplot); 
	pplot->Update(currentTime(), 0.f, true); 
	UpdateObservers();
}

void CDocument::DeleteObject(CGLObject *po)
{
	CGLPlot* pp = dynamic_cast<CGLPlot*>(po);
	if (pp)
	{
		list<CGLPlot*>::iterator it = m_pPlot.begin();
		for (int i=0; i<(int) m_pPlot.size(); ++i, ++it)
		{
			pp = (*it);
			if (pp == po)
			{
				delete pp;
				m_pPlot.erase(it);
				break;
			}
		}
	}
	else if (dynamic_cast<CGLVisual*>(po))
	{
		list<CGLVisual*>::iterator it = m_pObj.begin();
		for (int i = 0; i<(int)m_pObj.size(); ++i, ++it)
		{
			CGLVisual* pv = (*it);
			if (pv == po)
			{
				delete pv;
				m_pObj.erase(it);
				break;
			}
		}
	}
	else if (dynamic_cast<GLCameraTransform*>(po))
	{
		GLCameraTransform* pt = dynamic_cast<GLCameraTransform*>(po);
		CGView* pview = GetView();
		pview->DeleteKey(pt);
	}
	else if (dynamic_cast<CImageSlicer*>(po))
	{
		delete m_pIS;
		m_pIS = 0;
	}
	else if (dynamic_cast<CVolRender*>(po))
	{
		delete m_pVR;
		m_pVR = 0;
	}
	else if (dynamic_cast<CGLDisplacementMap*>(po))
	{
		CGLDisplacementMap* map = dynamic_cast<CGLDisplacementMap*>(po);
		CGLModel* m = GetGLModel();
		assert(map == m->GetDisplacementMap());
		m->RemoveDisplacementMap();
		UpdateFEModel(true);
	}
}

//------------------------------------------------------------------------------------------
void CDocument::Add3DImage(C3DImage* pimg, double x0, double y0, double z0, double x1, double y1, double z1, int nvisOption)
{
	BOUNDINGBOX box;
	box.x0 = x0;
	box.y0 = y0;
	box.z0 = z0;
	box.x1 = x1;
	box.y1 = y1;
	box.z1 = z1;

	if (m_pImg) delete m_pImg;
	m_pImg = pimg;

	if (nvisOption == 0)
	{
		m_pVR = new CVolRender;
		m_pVR->Create(*m_pImg, box);
	}
	else
	{
		m_pIS = new CImageSlicer;
		m_pIS->Create(*m_pImg, box);
	}
}

//------------------------------------------------------------------------------------------
void CDocument::UpdateAllStates()
{
	if (m_bValid == false) return;
	int N = m_fem->GetStates();
	int ntime = currentTime();
	for (int i=0; i<N; ++i) SetCurrentTime(i);
	SetCurrentTime(ntime);
}

// get the number of time steps
int CDocument::GetTimeSteps() { return (m_fem?m_fem->GetStates():0); }

const char* CDocument::GetTitle() { return (m_fem?m_fem->GetTitle():0); }
void CDocument::SetTitle(const char* sztitle) { if (m_fem) m_fem->SetTitle(sztitle); }
