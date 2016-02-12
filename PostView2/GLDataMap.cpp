#include "stdafx.h"
#include "GLDataMap.h"
#include "PostViewLib/GLObject.h"
#include "GLModel.h"
#include "GLWidgetManager.h"
#include "PostViewLib/constants.h"

CGLDataMap::CGLDataMap(CGLObject* po)
{
	m_po = po;
}

CGLDataMap::~CGLDataMap(void)
{
	m_po = 0;
}

//-----------------------------------------------------------------------------

CGLDisplacementMap::CGLDisplacementMap(CGLObject* po) : CGLDataMap(po)
{
	sprintf(m_szname, "Displacement Map");
	m_scl = 1.f;
}

//-----------------------------------------------------------------------------

void CGLDisplacementMap::Activate(bool b)
{
	CGLObject::Activate(b);

	if (b == false)
	{
		CGLModel* po = dynamic_cast<CGLModel*>(m_po);
		FEMesh* pm = po->GetMesh();
		for (int i=0; i<pm->Nodes(); ++i) pm->Node(i).m_rt = pm->Node(i).m_r0;
		pm->UpdateNormals(po->RenderSmooth());
	}
}

//-----------------------------------------------------------------------------

void CGLDisplacementMap::Update(int ntime, float dt, bool breset)
{
	CGLModel* po = dynamic_cast<CGLModel*>(m_po);
	FEMesh* pm = po->GetMesh();
	FEModel* pfem = po->GetFEModel();

	// get the number of states and make sure we have something
	int N = pfem->GetStates();
	if (N == 0) return;

	int n0 = ntime;
	int n1 = (ntime+1>=N? ntime : ntime+1);

	FEState& s1= *pfem->GetState(n0);
	FEState& s2= *pfem->GetState(n1);

	// update the states
	UpdateState(n0, breset);
	UpdateState(n1, breset);

	float df = s2.m_time - s1.m_time;
	if (df == 0) df = 1.f;

	// set the current nodal positions
	for (int i=0; i<pm->Nodes(); ++i)
	{
		FENode& node = pm->Node(i);
		vec3f d1 = s1.m_NODE[i].m_rt - node.m_r0;
		vec3f d2 = s2.m_NODE[i].m_rt - node.m_r0;

		// the actual nodal position is stored in the state
		// this is the field that will be used for strain calculations
		s1.m_NODE[i].m_rt = node.m_r0 + d1;

		vec3f du = d2*dt/df + d1*(df - dt)/df;

		// the scaled displacement is stored on the mesh
		node.m_rt = node.m_r0 + du*m_scl;
	}
	pm->UpdateNormals(po->RenderSmooth());
}

//-----------------------------------------------------------------------------

void CGLDisplacementMap::UpdateState(int ntime, bool breset)
{
	CGLModel* po = dynamic_cast<CGLModel*>(m_po);
	FEMesh* pm = po->GetMesh();
	FEModel* pfem = po->GetFEModel();

	int N = pfem->GetStates();
	if (breset || (N != m_ntag.size())) m_ntag.assign(N, -1);

	int nfield = pfem->GetDisplacementField();

	if ((nfield >= 0) && (m_ntag[ntime] != nfield))
	{
		m_ntag[ntime] = nfield;

		FEState& s= *pfem->GetState(ntime);

		// set the current nodal positions
		for (int i=0; i<pm->Nodes(); ++i)
		{
			FENode& node = pm->Node(i);
			vec3f dr = pfem->EvaluateNodeVector(i, ntime, nfield);

			// the actual nodal position is stored in the state
			// this is the field that will be used for strain calculations
			s.m_NODE[i].m_rt = node.m_r0 + dr;
		}
	}
}

//-----------------------------------------------------------------------------

CGLModel* CGLDisplacementMap::GetModel()
{
	return dynamic_cast<CGLModel*>(m_po);
}

//-----------------------------------------------------------------------------
// CGLColorMap
//-----------------------------------------------------------------------------

CGLColorMap::CGLColorMap(CGLModel *po) : CGLDataMap(po)
{
	m_range.min = m_range.max = 0;
	m_range.ntype = RANGE_DYNA;

	m_nfield = BUILD_FIELD(1,0,0);
	m_breset = true;
	m_bDispNodeVals = true;

	m_Col.jet();
	sprintf(m_szname, "Color Map");

	m_pbar = new GLLegendBar(this, &m_Col, 0, 0, 120, 600);
	m_pbar->align(GLW_ALIGN_RIGHT | GLW_ALIGN_VCENTER);
	m_pbar->hide();
	CGLWidgetManager::GetInstance()->AddWidget(m_pbar);

	// we start the colormap as inactive
	Activate(false);
}

//-----------------------------------------------------------------------------

CGLColorMap::~CGLColorMap()
{
	CGLWidgetManager::GetInstance()->RemoveWidget(m_pbar);
	delete m_pbar;
}

//-----------------------------------------------------------------------------

CGLModel* CGLColorMap::GetModel()
{
	return dynamic_cast<CGLModel*>(m_po);
}

//-----------------------------------------------------------------------------

void CGLColorMap::Update(int ntime, float dt, bool breset)
{
	// get the object
	CGLModel* po = GetModel();

	// get the mesh
	FEMesh* pm = po->GetMesh();
	FEModel* pfem = po->GetFEModel();

	// make sure the field variable is still valid
	if (pfem->IsValidFieldCode(m_nfield, ntime) == false)
	{
		// This may happen after an update if fields are deleted.
		// reset the field code
		m_nfield = BUILD_FIELD(1,0,0);
		breset = true;
	}

	// evaluate the mesh
	pfem->Evaluate(m_nfield, ntime, breset);

	// get the state
	FEState& s = *pfem->GetState(ntime);
 
	// update the range
	float fmin = 1e29f, fmax = -1e29f;
	if (IS_ELEM_FIELD(m_nfield) && (m_bDispNodeVals==false))
	{
		int NF = pm->Faces();
		for (int i=0; i<NF; ++i)
		{
			FEFace& face = pm->Face(i);
			FACEDATA& fd = s.m_FACE[i];
//			if (face.IsEnabled() && (face.m_ntag > 0))
			{
				int nf = face.Nodes();
				for (int j=0; j<nf; ++j)
				{
					if (fd.m_nv[j] > fmax) fmax = fd.m_nv[j];
					if (fd.m_nv[j] < fmin) fmin = fd.m_nv[j];
				}
			}
		}
	}
	else
	{
		for (int i=0; i<pm->Nodes(); ++i)
		{
			NODEDATA& node = s.m_NODE[i];
			if (pm->Node(i).IsEnabled() && (node.m_ntag > 0))
			{
				if (node.m_val > fmax) fmax = node.m_val;
				if (node.m_val < fmin) fmin = node.m_val;
			}
		}
	}

	if (m_range.ntype != RANGE_USER)
	{
		if (m_breset || breset)
		{
			m_range.max = fmax;
			m_range.min = fmin;
			m_breset = false;
		}
		else
		{
			switch (m_range.ntype)
			{
			case RANGE_DYNA:
				m_range.max = fmax;
				m_range.min = fmin;
				break;
			case RANGE_STAT:
				if (fmax > m_range.max) m_range.max = fmax;
				if (fmin < m_range.min) m_range.min = fmin;
				break;
			}
		}
	}

	// set the colormap's range
	m_Col.SetRange(m_range.min, m_range.max);

	// update mesh texture coordinates
	float min = m_range.min;
	float max = m_range.max;
	if (min == max) max++;
	for (int i=0; i<pm->Nodes(); ++i)
	{
		FENode& node = pm->Node(i);
		NODEDATA& data = s.m_NODE[i];
		if (node.IsEnabled() && (data.m_ntag > 0))
			node.m_tex = (s.m_NODE[i].m_val - min) / (max - min);
		else node.m_tex = 0;
	}

	float dti = 1.f / (max - min);
	for (int i=0; i<pm->Faces(); ++i)
	{
		FEFace& face = pm->Face(i);
		if (face.IsEnabled())
		{
//			for (int j=0; j<face.Nodes(); ++j) face.m_tex[j] = (s.m_NODE[face.node[j]].m_val - min)*dti;
			for (int j=0; j<face.Nodes(); ++j) face.m_tex[j] = (s.m_FACE[i].m_nv[j] - min)*dti;
			if (s.m_FACE[i].m_ntag > 0) face.Activate(); else face.Deactivate();
			face.m_texe = 0;
	//		face.m_texe  = (pm->Element(face.m_elem).m_val - min )*dti;
		}
		else 
		{
			for (int j=0; j<face.Nodes(); ++j) face.m_tex[j] = 0;
			face.m_texe = 0;
		}
	}
}
