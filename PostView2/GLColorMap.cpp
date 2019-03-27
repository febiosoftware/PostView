#include "stdafx.h"
#include "GLColorMap.h"
#include "GLModel.h"
#include <GLWLib/GLWidgetManager.h>
#include "PostViewLib/constants.h"

//-----------------------------------------------------------------------------
// CGLColorMap
//-----------------------------------------------------------------------------

CGLColorMap::CGLColorMap(CGLModel *po) : CGLDataMap(po)
{
	m_range.min = m_range.max = 0;
	m_range.ntype = RANGE_DYNA;

	m_nfield = 0;
	m_breset = true;
	m_bDispNodeVals = true;

	SetName("Color Map");

	m_pbar = new GLLegendBar(&m_Col, 0, 0, 120, 600);
	m_pbar->align(GLW_ALIGN_RIGHT | GLW_ALIGN_VCENTER);
	m_pbar->copy_label(GetName().c_str());
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
void CGLColorMap::SetEvalField(int n)
{
	if (n != m_nfield)
	{
		m_nfield = n;
		m_breset = true;
	}
}

//-----------------------------------------------------------------------------
bool CGLColorMap::GetColorSmooth()
{
	return m_Col.GetSmooth();
}

//-----------------------------------------------------------------------------
void CGLColorMap::SetColorSmooth(bool b)
{
	m_Col.SetSmooth(b);
}

//-----------------------------------------------------------------------------
void CGLColorMap::Update(int ntime, float dt, bool breset)
{
	// get the object
	CGLModel* po = GetModel();

	// get the mesh
	FEMeshBase* pm = po->GetActiveMesh();
	FEModel* pfem = po->GetFEModel();

	int N = pfem->GetStates();
	if (N == 0) return;

	int n0 = ntime;
	int n1 = (ntime + 1 >= N ? ntime : ntime + 1);
	if (dt == 0.f) n1 = n0;

	UpdateState(n0, breset);
	if (n0 != n1) UpdateState(n1, breset);

	// get the state
	FEState& s0 = *pfem->GetState(n0);
	FEState& s1 = *pfem->GetState(n1);

	float df = s1.m_time - s0.m_time;
	if (df == 0) df = 1.f;

	float w = dt / df;

	// update the range
	float fmin = 1e29f, fmax = -1e29f;
	ValArray& faceData0 = s0.m_FaceData;
	ValArray& faceData1 = s1.m_FaceData;
	if (IS_ELEM_FIELD(m_nfield) && (m_bDispNodeVals == false))
	{
		int NE = pm->Elements();
		for (int i=0; i<NE; ++i)
		{
			FEElement& el = pm->Element(i);
			ELEMDATA& d0 = s0.m_ELEM[i];
			ELEMDATA& d1 = s1.m_ELEM[i];
			if ((d0.m_state & StatusFlags::ACTIVE) && (d1.m_state & StatusFlags::ACTIVE))
			{
				float f0 = d0.m_val;
				float f1 = d1.m_val;
				float f = f0 + (f1 - f0)*w;
				if (f > fmax) fmax = f;
				if (f < fmin) fmin = f;
			}			
		}
	}
	else
	{
		for (int i = 0; i<pm->Nodes(); ++i)
		{
			FENode& node = pm->Node(i);
			NODEDATA& d0 = s0.m_NODE[i];
			NODEDATA& d1 = s1.m_NODE[i];
			if ((pm->Node(i).IsEnabled()) && (d0.m_ntag > 0) && (d1.m_ntag > 0))
			{
				float f0 = d0.m_val;
				float f1 = d1.m_val;
				float f = f0 + (f1 - f0)*w;
				node.m_tex = f;
				node.m_ntag = 1;
				if (f > fmax) fmax = f;
				if (f < fmin) fmin = f;
			}
			else node.m_ntag = 0;
		}
	}

	if (m_bDispNodeVals == false)
	{
		int NF = pm->Faces();
		for (int i = 0; i<NF; ++i)
		{
			FEFace& face = pm->Face(i);
			FACEDATA& fd0 = s0.m_FACE[i];
			FACEDATA& fd1 = s1.m_FACE[i];
			//			if (face.IsEnabled() && (face.m_ntag > 0))
			{
				face.m_ntag = 1;
				int nf = face.Nodes();
				for (int j = 0; j<nf; ++j)
				{
					float f0 = faceData0.value(i, j);
					float f1 = (n0 == n1 ? f0 : faceData1.value(i, j));
					float f = f0 + (f1 - f0)*w;
					face.m_tex[j] = f;
					if (f > fmax) fmax = f;
					if (f < fmin) fmin = f;
				}
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
	m_pbar->SetRange(m_range.min, m_range.max);

	// update mesh texture coordinates
	float min = m_range.min;
	float max = m_range.max;
	if (min == max) max++;
	for (int i = 0; i<pm->Nodes(); ++i)
	{
		FENode& node = pm->Node(i);
		if (node.IsEnabled() && (node.m_ntag > 0))
			node.m_tex = (node.m_tex - min) / (max - min);
		else node.m_tex = 0;
	}

	float dti = 1.f / (max - min);
	for (int i = 0; i<pm->Faces(); ++i)
	{
		FEFace& face = pm->Face(i);
		FACEDATA& fd = s0.m_FACE[i];
		if (face.IsEnabled())
		{
			for (int j = 0; j<face.Nodes(); ++j) face.m_tex[j] = (face.m_tex[j] - min)*dti;
			if (fd.m_ntag > 0) face.Activate(); else face.Deactivate();
			face.m_texe = 0;
		}
		else
		{
			for (int j = 0; j<face.Nodes(); ++j) face.m_tex[j] = 0;
			face.m_texe = 0;
		}
	}

	// update element textures
	for (int i = 0; i<pm->Elements(); ++i)
	{
		FEElement& el = pm->Element(i);
		ELEMDATA& d0 = s0.m_ELEM[i];
		ELEMDATA& d1 = s1.m_ELEM[i];
		if ((d0.m_state & StatusFlags::ACTIVE) && (d1.m_state & StatusFlags::ACTIVE))
		{
			float f0 = d0.m_val;
			float f1 = d1.m_val;
			float f = f0 + (f1 - f0)*w;
			el.m_tex = (f - min) / (max - min);

			el.Activate();
		}
		else el.Deactivate();
	}

	// update the internal surfaces of the model
	int NS = po->InternalSurfaces();
	for (int i=0; i<NS; ++i)
	{
		GLSurface& surf = po->InteralSurface(i);
		int NF = surf.Faces();
		for (int j=0; j<NF; ++j)
		{
			FEFace& face = surf.Face(j);
			if (face.m_elem[0] == -1) face.Deactivate();
			else
			{
				face.Activate();
				int iel = face.m_elem[0];
				
				ELEMDATA& d0 = s0.m_ELEM[iel];
				ELEMDATA& d1 = s1.m_ELEM[iel];

				if (((d0.m_state & StatusFlags::ACTIVE)==0) || ((d1.m_state & StatusFlags::ACTIVE)== 0)) face.Deactivate();
				else
				{
					float v0 = d0.m_val;
					float v1 = d1.m_val;
					float v = v0 + (v1 - v0)*w;

					float tex = (v - min) / (max - min);

					int nf = face.Nodes();
					face.m_texe = tex;
					for (int k=0; k<nf; ++k) face.m_tex[k] = tex;
				}
			}
		}
	}
}

void CGLColorMap::UpdateState(int ntime, bool breset)
{
	// get the model
	CGLModel* po = GetModel();
	FEModel* pfem = po->GetFEModel();

	// make sure the field variable is still valid
	if (pfem->IsValidFieldCode(m_nfield, ntime) == false)
	{
		// This may happen after an update if fields are deleted.
		// reset the field code
		m_nfield = BUILD_FIELD(1, 0, 0);
		breset = true;
	}

	// evaluate the mesh
	pfem->Evaluate(m_nfield, ntime, breset);
}
