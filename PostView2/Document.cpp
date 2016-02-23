#include "stdafx.h"
#include "Document.h"
#include "MainWindow.h"
#include <PostViewLib/FEModel.h>
#include <PostViewLib/FEFileReader.h>
#include <PostViewLib/3DImage.h>
#include <PostViewLib/VolRender.h>
#include <PostViewLib/FEMeshData_T.h>
#include "GLPlot.h"
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
	m_bShadows    = false;
	m_bTriad      = true;
	m_bTags       = false;
	m_bTitle      = true;
	m_bconn		 = false;
	m_bext       = true;
	m_bBox        = false;
	m_nproj       = RENDER_PERSP;
	m_bLighting   = true;
	m_ambient     = 0.3f;
	m_diffuse     = 0.7f;
	m_bcull		 = false;
	m_blinesmooth = true;
	m_flinethick  = 0.5f;
}


//-----------------------------------------------------------------------------
ModelData::ModelData(CGLModel *po)
{
	if (po == 0) return;

	// set model props
	m_mdl.m_bnorm      = po->m_bnorm;
	m_mdl.m_bsmooth    = po->m_bsmooth;
	m_mdl.m_boutline   = po->m_boutline;
	m_mdl.m_bghost     = po->m_bghost;
	m_mdl.m_bShell2Hex = po->m_bShell2Hex;
	m_mdl.m_nshellref  = po->m_nshellref;
	m_mdl.m_nDivs      = po->m_nDivs;
	m_mdl.m_nrender    = po->m_nrender;

	// set colormap props
	CGLColorMap* pglmap = po->GetColorMap();
	if (pglmap)
	{
		m_cmap.m_bactive = pglmap->IsActive();
		m_cmap.m_nRangeType = pglmap->GetRangeType();
		m_cmap.m_bDispNodeVals = pglmap->DisplayNodalValues();
		m_cmap.m_nField = pglmap->GetEvalField();
		pglmap->GetRange(m_cmap.m_user);

		CColorMap* pcm = pglmap->GetColorMap();
		m_cmap.m_ntype = pcm->Type();
		m_cmap.m_ndivs = pcm->GetDivisions();
		m_cmap.m_bsmooth = pcm->Smooth();
		pcm->GetRange(m_cmap.m_min, m_cmap.m_max);
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
	po->m_boutline   = m_mdl.m_boutline;
	po->m_bghost     = m_mdl.m_bghost;
	po->m_bShell2Hex = m_mdl.m_bShell2Hex;
	po->m_nshellref  = m_mdl.m_nshellref;
	po->m_nDivs      = m_mdl.m_nDivs;
	po->m_nrender    = m_mdl.m_nrender;

	// set color map data
	CGLColorMap* pglmap = po->GetColorMap();
	if (pglmap)
	{
		pglmap->SetRangeType(m_cmap.m_nRangeType);
		pglmap->SetRange(m_cmap.m_user);
		pglmap->DisplayNodalValues(m_cmap.m_bDispNodeVals);
		pglmap->SetEvalField(m_cmap.m_nField);
		pglmap->Activate(m_cmap.m_bactive);

		CColorMap* pcm = pglmap->GetColorMap();
		pcm->SetType(m_cmap.m_ntype);
		pcm->SetDivisions(m_cmap.m_ndivs);
		pcm->Smooth(m_cmap.m_bsmooth);
		pcm->SetRange(m_cmap.m_min, m_cmap.m_max);
	}

	// displacement map
	FEModel* ps = po->GetFEModel();
	ps->SetDisplacementField(m_dmap.m_nfield);

	// materials
	if (!m_mat.empty())
	{
		int N0 = m_mat.size();
		int N1 = ps->Materials();
		int N = (N0<N1?N0:N1);
		for (int i=0; i<N; ++i) *ps->GetMaterial(i) = m_mat[i];

		// update the mesh state
		FEMesh* pmesh = ps->GetMesh();
		for (int i=0; i<N; ++i)
		{
			FEMaterial* pm = ps->GetMaterial(i);
			if (pm->bvisible == false) pmesh->HideElements(i);
		}
	}

	// reload data fields
	int ndata = m_data.size();
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
			else if (si.compare("GL strain"                 ) == 0) pdf = new FEDataField_T<FEGLStrain             >("GL strain"                 );
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
			else if (si.compare("1-Princ curvature"         ) == 0) pdf = new FEDataField_T<FEPrincCurvature1      >("1-Princ curvature"         );
			else if (si.compare("2-Princ curvature"         ) == 0) pdf = new FEDataField_T<FEPrincCurvature2      >("2-Princ curvature"         );
			else if (si.compare("Gaussian curvature"        ) == 0) pdf = new FEDataField_T<FEGaussCurvature       >("Gaussian curvature"        );
			else if (si.compare("Mean curvature"            ) == 0) pdf = new FEDataField_T<FEMeanCurvature        >("Mean curvature"            );
			else if (si.compare("RMS curvature"             ) == 0) pdf = new FEDataField_T<FERMSCurvature         >("RMS curvature"             );
			else if (si.compare("Princ curvature difference") == 0) pdf = new FEDataField_T<FEDiffCurvature        >("Princ curvature difference");
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
	m_nTime = 0;
	m_fTime = 0.f;

	m_szfile[0] = 0;

	m_mode = SELECT_ELEMS;

	m_pImg = 0;
	m_pVR = 0;

	m_fem = 0;
	m_pGLModel = 0;
	m_pImp = 0;
	Reset();

	// initialize view settings
	m_ops.Defaults();
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
void CDocument::UpdateFEModel(bool breset)
{
	if (!m_bValid) return;

	// get the time inc value
	float dt = m_fTime - GetTimeValue(m_nTime);

	// update the model
	if (m_pGLModel) m_pGLModel->Update(m_nTime, dt, breset);

	// update the plot list
	list<CGLPlot*>::iterator it = m_pPlot.begin();
	for (int i=0; i<(int) m_pPlot.size(); ++i, ++it) 
		if ((*it)->IsActive()) (*it)->Update(m_nTime, dt, breset);
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

	// create a new model
	m_pGLModel = new CGLModel(m_fem);

	// go back to the original working directory
	if (szwdir) chdir(szwdir);

	if (m_nTime >= GetTimeSteps()-1) m_nTime = GetTimeSteps()-1;

	// reset settings
	if (bup)
	{
		MD.SetData(m_pGLModel);
		m_bValid = true;
		SetCurrentTime(m_nTime);
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

		// set the current time
		// this will also update the scene
		SetCurrentTime(0);
	}

	// Since the GL model has changed, we need to update all plots
	// that keep a reference to the model
	list<CGLPlot*>::iterator it;
	for (it = m_pPlot.begin(); it != m_pPlot.end(); ++it)
	{
		(*it)->SetModel(m_pGLModel);
	}

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
		m_nTime = ntime;
		m_fTime = GetTimeValue(m_nTime);
		UpdateFEModel();
	}
}

//-----------------------------------------------------------------------------
void CDocument::SetTimeValue(float ftime)
{
	if (m_bValid && m_pGLModel)
	{
		m_nTime = GetClosestTime(ftime);
		m_fTime = ftime;
		UpdateFEModel();
	}
}

//-----------------------------------------------------------------------------
float CDocument::GetTimeValue(int ntime)
{ 
	if (m_fem && (m_fem->GetStates() > 0)) return m_fem->GetState(ntime)->m_time;
	return 0.f; 
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

	FEMesh& fem = *m_FEScene.GetMesh();
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
// This returns the time step whose time value is closest but less than t
//
int CDocument::GetClosestTime(double t)
{
	if ((m_fem == 0) || (m_fem->GetStates() <= 1)) return 0;

	FEState& s = *m_fem->GetState(0);
	if (s.m_time >= t) return 0;

	for (int i=1; i<m_fem->GetStates(); ++i)
	{
		FEState& s = *m_fem->GetState(i);
		if (s.m_time >= t) return i-1;
	}
	return m_fem->GetStates()-1;
}

bool CDocument::ExportPlot(const char* szfile, bool bflag[6], int ncode[6])
{
	if (IsValid() == false) return true;

//	FELSDYNAPlotExport ex;
//	return ex.Save(*m_fem, szfile, bflag, ncode);
	return false;
}

bool CDocument::ExportAscii(const char* szfile)
{
/*	CDlgExportAscii dlg;

	if (dlg.DoModal() != FLX_OK) return true;

	// decide which time steps to export
	int n0, n1;
	if (dlg.m_bcur) n0 = n1 = currentTime();
	else
	{
		n0 = 0;
		n1 = m_fem->GetStates() - 1;
	}

	// export the data
	FEASCIIExport out;
	out.m_bcoords   = dlg.m_bcoords;
	out.m_bedata    = dlg.m_bedata;
	out.m_belem     = dlg.m_belem;
	out.m_bface     = dlg.m_bface;
	out.m_bfnormals = dlg.m_bfnormals;
	out.m_bndata    = dlg.m_bndata;
	out.m_bselonly  = dlg.m_bsel;

	if (out.Save(m_fem, n0, n1, szfile) == false) return false;

	return true;
*/
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

	int i;
	box.x0 = 1e20f; box.x1 = -1e20f;
	box.y0 = 1e20f; box.y1 = -1e20f;
	box.z0 = 1e20f; box.z1 = -1e20f;

	FEMesh* pm = m_fem->GetMesh();

	int nsel = 0;

	for (i=0; i<pm->Elements(); ++i)
	{
		FEElement& el = pm->Element(i);
		if (el.IsSelected())
		{
			int nel = el.Nodes();
			for (int j=0; j<nel; ++j)
			{
				box += pm->Node(el.m_node[j]).m_rt;
			}
			++nsel;
		}
	}

	for (i=0; i<pm->Nodes(); ++i)
	{
		FENode& node = pm->Node(i);
		if (node.IsSelected()) 
		{
			box += node.m_rt;
			++nsel;
		}
	}

	if (nsel == 0) return GetBoundingBox();

	if ((box.Width() < 1e-5) || (box.Height() < 1e-4) || (box.Depth() < 1e-4))
	{
		float R = box.Radius();
		box.InflateTo(R, R, R);
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
	FEMesh& mesh = *m_fem->GetMesh();

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
	FEMesh* pm = m_fem->GetMesh();
	int N = pm->Elements();
	FEState* ps = m_fem->GetState(m_nTime);
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
}

//-----------------------------------------------------------------------------
void CDocument::SelectNodesInRange(float fmin, float fmax, bool bsel)
{
	FEMesh* pm = m_fem->GetMesh();
	int N = pm->Nodes();
	FEState* ps = m_fem->GetState(m_nTime);
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
}

//-----------------------------------------------------------------------------
void CDocument::SelectFacesInRange(float fmin, float fmax, bool bsel)
{
	FEMesh* pm = m_fem->GetMesh();
	FEState* ps = m_fem->GetState(m_nTime);
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
}

//-----------------------------------------------------------------------------
void CDocument::UpdateViews()
{
	m_wnd->repaint();
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

	return strlen(ch);
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

int CDocument::GetFileTitle(char* sztitle)
{
	char* ch = strrchr(m_szfile, '\\');
	if (ch == 0) 
	{
		ch = strrchr(m_szfile, '/'); 
		if (ch == 0) ch = m_szfile; else ch++;
	} else ch++;

	int n;
	char* ce = strrchr(m_szfile, '.');
	if ((ce == 0) || (ce < ch)) n = strlen(ch);
	else n = (int)(ce-ch);

	if (sztitle)
	{
		if (n) strncpy(sztitle, ch, n);
		sztitle[n] = 0;
	}

	return n;
}

void CDocument::DeleteObject(CGLObject *po)
{
	int i;
	CGLPlot* pp = dynamic_cast<CGLPlot*>(po);
	if (pp)
	{
		list<CGLPlot*>::iterator it = m_pPlot.begin();
		for (i=0; i<(int) m_pPlot.size(); ++i, ++it)
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
		for (i=0; i<(int) m_pObj.size(); ++i, ++it)
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
}

//------------------------------------------------------------------------------------------
void CDocument::Add3DImage(C3DImage* pimg, double w, double h, double d)
{
	BOUNDINGBOX box;
	box.x0 = box.y0 = box.z0 = 0;
	box.x1 = w;
	box.y1 = h;
	box.z1 = d;

	m_pImg = pimg;
	m_pVR = new CVolRender;
	m_pVR->Create(*m_pImg, box);
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

//-----------------------------------------------------------------------------
void CDocument::GetSelectionList(vector<int>& L)
{
	L.clear();
	FEMesh& m = *m_fem->GetMesh();
	switch (GetSelectionMode())
	{
	case SELECT_NODES:
		{
			for (int i=0; i<m.Nodes(); ++i) if (m.Node(i).IsSelected()) L.push_back(i);
		}
		break;
	case SELECT_FACES:
		{
			for (int i=0; i<m.Faces(); ++i) if (m.Face(i).IsSelected()) L.push_back(i);
		}
		break;
	case SELECT_ELEMS:
		{
			for (int i=0; i<m.Elements(); ++i) if (m.Element(i).IsSelected()) L.push_back(i);
		}
		break;
	}
}

// get the number of time steps
int CDocument::GetTimeSteps() { return (m_fem?m_fem->GetStates():0); }

// get the current active state
FEState* CDocument::GetCurrentState() { return (m_fem? m_fem->GetState(m_nTime) : 0); }

const char* CDocument::GetTitle() { return (m_fem?m_fem->GetTitle():0); }
void CDocument::SetTitle(const char* sztitle) { if (m_fem) m_fem->SetTitle(sztitle); }
