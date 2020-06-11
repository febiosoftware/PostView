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

#include "stdafx.h"
#include "Document.h"
#include "MainWindow.h"
#include <PostLib/FEPostModel.h>
#include <PostLib/FEFileReader.h>
#include <ImageLib/3DImage.h>
#include <PostLib/GLImageRenderer.h>
#include <PostLib/FEMeshData_T.h>
#include <PostGL/GLPlot.h>
#include <PostGL/GLModel.h>
#include <PostLib/constants.h>
#include <PostLib/Palette.h>
#include <PostLib/FEAsciiExport.h>
#include <PostLib/ImageModel.h>
#include <MeshLib/FEElementLibrary.h>
using namespace Post;

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
	m_mdl.m_bghost     = po->m_bghost;
	m_mdl.m_bShell2Hex = po->ShowShell2Solid();
	m_mdl.m_nshellref  = po->ShellReferenceSurface();
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
	FEPostModel* ps = po->GetFEModel();
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
	po->m_bghost     = m_mdl.m_bghost;
	po->ShowShell2Solid(m_mdl.m_bShell2Hex);
	po->ShellReferenceSurface(m_mdl.m_nshellref);
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
	FEPostModel* ps = po->GetFEModel();
	ps->SetDisplacementField(m_dmap.m_nfield);

	// materials
	if (!m_mat.empty())
	{
		int N0 = (int)m_mat.size();
		int N1 = ps->Materials();
		int N = (N0<N1?N0:N1);
		for (int i=0; i<N; ++i) *ps->GetMaterial(i) = m_mat[i];

		// update the mesh state
		Post::FEPostMesh* pmesh = po->GetActiveMesh();
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
			else if (si.compare("Deformation gradient"      ) == 0) pdf = new FEDataField_T<FEDeformationGradient  >("Deformation gradient"      );
			else if (si.compare("Infinitesimal strain"      ) == 0) pdf = new FEStrainDataField("Infinitesimal strain", FEStrainDataField::INF_STRAIN        );
			else if (si.compare("Lagrange strain"           ) == 0) pdf = new FEStrainDataField("Lagrange strain"     , FEStrainDataField::LAGRANGE          );
			else if (si.compare("Right Cauchy-Green"        ) == 0) pdf = new FEStrainDataField("Right Cauchy-Green"  , FEStrainDataField::RIGHT_CAUCHY_GREEN);
			else if (si.compare("Right stretch"             ) == 0) pdf = new FEStrainDataField("Right stretch"       , FEStrainDataField::RIGHT_STRETCH     );
			else if (si.compare("Biot strain"               ) == 0) pdf = new FEStrainDataField("Biot strain"         , FEStrainDataField::BIOT              );
			else if (si.compare("Right Hencky"              ) == 0) pdf = new FEStrainDataField("Right Hencky"        , FEStrainDataField::RIGHT_HENCKY      );
            else if (si.compare("Left Cauchy-Green"         ) == 0) pdf = new FEStrainDataField("Left Cauchy-Green"   , FEStrainDataField::LEFT_CAUCHY_GREEN );
            else if (si.compare("Left stretch"              ) == 0) pdf = new FEStrainDataField("Left stretch"        , FEStrainDataField::LEFT_STRETCH      );
            else if (si.compare("Left Hencky"               ) == 0) pdf = new FEStrainDataField("Left Hencky"         , FEStrainDataField::LEFT_HENCKY       );
            else if (si.compare("Almansi strain"            ) == 0) pdf = new FEStrainDataField("Almansi strain"      , FEStrainDataField::ALMANSI           );
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

	po->UpdateMeshState();
}

//=============================================================================
CDocObserver::CDocObserver(CDocument* doc) : m_doc(doc)
{
	if (m_doc) m_doc->AddObserver(this);
}

CDocObserver::~CDocObserver()
{
	if (m_doc) m_doc->RemoveObserver(this);
}

void CDocObserver::DocumentDelete()
{
	m_doc = nullptr;
	DocumentUpdate(true);
}

//=============================================================================
CDocument::CDocument(CMainWindow* pwnd) : m_wnd(pwnd)
{
	m_bValid = false;

	m_fem = 0;
	m_pGLModel = 0;
	m_pImp = 0;
	Reset();

	// initialize view settings
	m_time.Defaults();

	FEElementLibrary::InitLibrary();
}

CDocument::~CDocument()
{
	// remove all observers
	for (int i = 0; i < m_Observers.size(); ++i)
		m_Observers[i]->DocumentDelete();

	m_Observers.clear();
	
	if (m_pGLModel) delete m_pGLModel;
	if (m_pImp) delete m_pImp;
	ClearPlots();
	ClearObjects();
}

void CDocument::ClearPlots()
{
	if (m_pGLModel) m_pGLModel->ClearPlots();
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
void CDocument::UpdateObservers(bool bnew)
{
	if (m_Observers.empty()) return;

	for (int i=0; i<m_Observers.size(); ++i)
	{
		CDocObserver* observer = m_Observers[i];
		if (observer) observer->DocumentUpdate(bnew);
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

	// clear image models
	for (int i = 0; i < (int)m_img.size(); ++i) delete m_img[i];
	m_img.clear();
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
		CGLModel* mdl = GetGLModel();
		FEPostModel* fem = (mdl ? mdl->GetFEModel() : nullptr);
		if (fem && mdl)
		{
			int nfield = mdl->GetColorMap()->GetEvalField();
			return fem->GetDataManager()->getDataString(nfield, DATA_SCALAR);
		}
		else return "";
	}
	else return "";
}

//-----------------------------------------------------------------------------
int CDocument::currentTime() 
{ 
	if (m_pGLModel)
		return m_pGLModel->CurrentTimeIndex(); 
	else return -1;
}

//-----------------------------------------------------------------------------
float CDocument::GetTimeValue()
{ 
	if (m_pGLModel) return m_pGLModel->CurrentTime(); 
	else return 0.f;
}

//-----------------------------------------------------------------------------
float CDocument::GetTimeValue(int n)
{
	if (m_pGLModel) return m_pGLModel->GetFEModel()->GetTimeValue(n);
	else return 0.f;
}

//-----------------------------------------------------------------------------
void CDocument::UpdateFEModel(bool breset)
{
	if (!m_bValid) return;

	// update the model
	if (m_pGLModel) m_pGLModel->Update(breset);
}

//-----------------------------------------------------------------------------
void CDocument::ResetView()
{
	// get the boundingbox
	BOX box = GetBoundingBox();

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
	BOX box = GetExtentsBox();
	CGLCamera& cam = m_view.GetCamera();
	cam.SetTarget(box.Center());
	cam.SetTargetDistance(3.f*box.Radius());
	if (bhit) cam.Update(bhit);
}

//-----------------------------------------------------------------------------
// update all the color maps in use (called after the View Settings dialog)
void CDocument::UpdateColorMaps()
{
	if (m_pGLModel == 0) return;

	CGLColorMap* map = m_pGLModel->GetColorMap();
	map->GetColorMap()->UpdateTexture();

	m_pGLModel->UpdateColorMaps();
}

//-----------------------------------------------------------------------------
void CDocument::ApplyPalette(const CPalette& pal)
{
	int NCOL = pal.Colors();
	int nmat = m_fem->Materials();
	for (int i = 0; i<nmat; i++)
	{
		GLColor c = pal.Color(i % NCOL);

		FEMaterial& m = *m_fem->GetMaterial(i);
		m.diffuse = c;
		m.ambient = c;
		m.specular = GLColor(128, 128, 128);
		m.emission = GLColor(0, 0, 0);
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
	int ntime = (m_pGLModel ? m_pGLModel->CurrentTimeIndex() : 0);

	// remove the old object
	if (m_pGLModel) m_pGLModel->SetFEModel(nullptr);

	// remove the old scene
	m_bValid = false;
	delete m_fem;

	// create a new model
	m_fem = new FEPostModel;
	m_fem->SetName(sztitle);

	// set the file name as title
	m_fem->SetTitle(sztitle);

	// load the scene
	m_pImp->SetPostModel(m_fem);
	if (m_pImp->Load(szfile) == false)
	{
		delete m_fem;
		m_fem = 0;
		return false;
	}

	// assign material attributes
	const CPalette& pal = CPaletteManager::CurrentPalette();
	ApplyPalette(pal);

	// create a new model
	if (m_pGLModel == nullptr) m_pGLModel = new CGLModel(m_fem);
	else m_pGLModel->SetFEModel(m_fem);

	// go back to the original working directory
	if (szwdir) chdir(szwdir);

	if (ntime >= GetTimeSteps()-1) ntime = GetTimeSteps()-1;

	m_time.m_start = 0;
	m_time.m_end   = GetTimeSteps() - 1;

	// reset settings
	if (bup)
	{
		MD.SetData(m_pGLModel);

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
		BOX box = GetBoundingBox();

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
	m_fileName = szfile;

	// inform the observers
	UpdateObservers(true);

	// it's all good !
	return true;
}

//-----------------------------------------------------------------------------
void CDocument::SetCurrentTime(int ntime) 
{
	if (m_bValid && m_pGLModel)
	{
		m_pGLModel->SetCurrentTimeIndex(ntime);
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
void CDocument::SetFEModel(FEPostModel* pnew)
{
	// remove the old GL object
	delete m_pGLModel; m_pGLModel = 0;

	// remove the old FE model
	m_bValid = false;
	delete m_fem;

	// set the new scene
	m_fem = pnew;
	FEPostModel::SetInstance(m_fem);

	// create a new model
	m_pGLModel = new CGLModel(m_fem);

	// reset the view
	Reset();

	// assign material attributes
	const CPalette& pal = CPaletteManager::CurrentPalette();
	ApplyPalette(pal);

	// create a new model
	m_bValid = true;

	// get the boundingbox
	BOX box = GetBoundingBox();

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
	m_fileName.clear();

	UpdateObservers(true);
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
bool CDocument::ExportPlot(const char* szfile, bool bflag[6], int ncode[6])
{
	if (IsValid() == false) return true;

//	FELSDYNAPlotExport ex;
//	return ex.Save(*m_fem, szfile, bflag, ncode);
	return false;
}

//-----------------------------------------------------------------------------
BOX CDocument::GetBoundingBox()
{
	BOX b;
	if (m_fem) b = m_fem->GetBoundingBox();
	if (m_img.empty() == false) b += m_img[0]->GetBoundingBox();
	return b;
}

//-----------------------------------------------------------------------------
BOX CDocument::GetExtentsBox()
{
	BOX box;
	if (IsValid() == false)
	{
		box.x0 = box.y0 = box.z0 = -1.f;
		box.x1 = box.y1 = box.z1 =  1.f;
		return box;
	}

	CGLModel* mdl = GetGLModel();
	if (mdl == nullptr)
	{
		if (m_img.empty() == false) box = m_img[0]->GetBoundingBox();
		else box = BOX(-1, -1, -1, 1, 1, 1);
	}
	else
	{
		Post::FEPostMesh& mesh = *GetActiveMesh();
		int NE = mesh.Elements(), nvis = 0;
		for (int i = 0; i < NE; ++i)
		{
			FEElement_& el = mesh.ElementRef(i);
			if (el.IsVisible())
			{
				int ne = el.Nodes();
				for (int j = 0; j < ne; ++j) box += mesh.Node(el.m_node[j]).r;
				nvis++;
			}
		}
	}

//	if (box.IsValid())
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
BOX CDocument::GetSelectionBox()
{
	BOX box;

	if (IsValid() == false)
	{
		box = BOX(-1, -1, -1, 1, 1, 1);
		return box;
	}

	CGLModel* mdl = GetGLModel();
	if (mdl == nullptr)
	{
		if (m_img.empty() == false) box = m_img[0]->GetBoundingBox();
		else box = BOX(-1, -1, -1, 1, 1, 1);
	}
	else
	{
		Post::FEPostMesh& mesh = *GetActiveMesh();
		const vector<FEElement_*> selElems = GetGLModel()->GetElementSelection();
		for (int i = 0; i < (int)selElems.size(); ++i)
		{
			FEElement_& el = *selElems[i];
			int nel = el.Nodes();
			for (int j = 0; j < nel; ++j) box += mesh.Node(el.m_node[j]).r;
		}

		const vector<FEFace*> selFaces = GetGLModel()->GetFaceSelection();
		for (int i = 0; i < (int)selFaces.size(); ++i)
		{
			FEFace& face = *selFaces[i];
			int nel = face.Nodes();
			for (int j = 0; j < nel; ++j) box += mesh.Node(face.n[j]).r;
		}

		const vector<FEEdge*> selEdges = GetGLModel()->GetEdgeSelection();
		for (int i = 0; i < (int)selEdges.size(); ++i)
		{
			FEEdge& edge = *selEdges[i];
			int nel = edge.Nodes();
			for (int j = 0; j < nel; ++j) box += mesh.Node(edge.n[j]).r;
		}

		const vector<FENode*> selNodes = GetGLModel()->GetNodeSelection();
		for (int i = 0; i < (int)selNodes.size(); ++i)
		{
			FENode& node = *selNodes[i];
			box += node.r;
		}
	}

//	if (box.IsValid())
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
	Post::FEPostMesh* pm = GetActiveMesh();
	int N = pm->Elements();
	FEState* ps = m_pGLModel->GetActiveState();
	for (int i=0; i<N; ++i)
	{
		FEElement_& el = pm->ElementRef(i);
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
	Post::FEPostMesh* pm = GetActiveMesh();
	int N = pm->Nodes();
	FEState* ps = m_pGLModel->GetActiveState();
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
	Post::FEPostMesh* pm = GetActiveMesh();
	int N = pm->Edges();
	FEState* ps = m_pGLModel->GetActiveState();
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
	Post::FEPostMesh* pm = GetActiveMesh();
	FEState* ps = m_pGLModel->GetActiveState();
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
const std::string& CDocument::GetFile() const
{ 
	return m_fileName; 
}

//-----------------------------------------------------------------------------
void CDocument::SetFile(const std::string& fileName)
{
	m_fileName = fileName;
}

//-----------------------------------------------------------------------------
std::string CDocument::GetFileName()
{
	const char* szfile = m_fileName.c_str();
	const char* ch = strrchr(szfile, '\\');
	if (ch == 0) 
	{
		ch = strrchr(szfile, '/'); 
		if (ch == 0) ch = szfile; else ch++;
	} else ch++;

	return ch;
}

int CDocument::GetFilePath(char *szpath)
{
	const char* szfile = m_fileName.c_str();
	const char* ch = strrchr(szfile, '\\');
	if (ch == 0) 
	{
		ch = strrchr(szfile, '/'); 
		if (ch == 0) ch = szfile; else ++ch;
	}
	else ++ch;

	int n = (int) (ch - szfile);

	if (szpath) 
	{
		if (n) strncpy(szpath, szfile, n);
		szpath[n] = 0;
	}

	return n;
}

int CDocument::GetDocTitle(char* sztitle)
{
	const char* szfile = m_fileName.c_str();
	const char* ch = strrchr(szfile, '\\');
	if (ch == 0) 
	{
		ch = strrchr(szfile, '/'); 
		if (ch == 0) ch = szfile; else ch++;
	} else ch++;

	int n;
	const char* ce = strrchr(szfile, '.');
	if ((ce == 0) || (ce < ch)) n = (int)strlen(ch);
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
	CGLPlot* pp = dynamic_cast<CGLPlot*>(po);
	if (pp)
	{
		delete pp;
	}
	else if (dynamic_cast<GLCameraTransform*>(po))
	{
		GLCameraTransform* pt = dynamic_cast<GLCameraTransform*>(po);
		CGView* pview = GetView();
		pview->DeleteKey(pt);
	}
	else if (dynamic_cast<CImageModel*>(po))
	{
		CImageModel* img = dynamic_cast<CImageModel*>(po);
		for (int i = 0; i < (int)m_img.size(); ++i)
		{
			if (m_img[i] == img)
			{
				delete img;
				m_img.erase(m_img.begin() + i);
				break;
			}
		}
	}
	else if (dynamic_cast<CGLImageRenderer*>(po))
	{
		CGLImageRenderer* ir = dynamic_cast<CGLImageRenderer*>(po);
		CImageModel* img = ir->GetImageModel();
		img->RemoveRenderer(ir);
	}
	else if (dynamic_cast<CGLDisplacementMap*>(po))
	{
		CGLDisplacementMap* map = dynamic_cast<CGLDisplacementMap*>(po);
		CGLModel* m = GetGLModel();
		assert(map == m->GetDisplacementMap());
		m->RemoveDisplacementMap();
		UpdateFEModel(true);
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
}

//------------------------------------------------------------------------------------------
void CDocument::AddImageModel(CImageModel* img)
{
	m_img.push_back(img);
}

//------------------------------------------------------------------------------------------
// load RAW image from file
bool CDocument::LoadRAWImage(const std::string& fileName, int dim[3], BOX range)
{
	const char* ch = fileName.c_str();
	const char* fileTitle = strrchr(ch, '\\');
	if (fileTitle == nullptr)
	{
		fileTitle = strrchr(ch, '/');
		if (fileTitle == nullptr) fileTitle = ch;
		else fileTitle++;
	}
	else fileTitle++;

	CImageModel* img = new CImageModel(GetGLModel());
	if (img->LoadImageData(fileName, dim[0], dim[1], dim[2], range) == false)
	{
		delete img;
		return false;
	}
	img->SetName(fileTitle);
	AddImageModel(img);

	m_bValid = true;

	return true;
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

string CDocument::GetTitle() const { return (m_fem?m_fem->GetTitle():""); }
void CDocument::SetTitle(const string& title) { if (m_fem) m_fem->SetTitle(title); }
