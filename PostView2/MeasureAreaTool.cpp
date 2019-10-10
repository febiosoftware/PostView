#include "stdafx.h"
#include "MeasureAreaTool.h"
#include "Document.h"
#include <PostLib/FEModel.h>
#include "MainWindow.h"
using namespace Post;

//-----------------------------------------------------------------------------
CMeasureAreaTool::Props::Props(CMeasureAreaTool* ptool) : m_ptool(ptool)
{
	addProperty("apply filter"  , CProperty::Bool);
	addProperty("min value"     , CProperty::Float);
	addProperty("max value"     , CProperty::Float);
	addProperty("all time steps", CProperty::Bool);
	addProperty("selected faces", CProperty::Int  )->setFlags(CProperty::Visible);
	addProperty("area"          , CProperty::Float)->setFlags(CProperty::Visible);
}

//-----------------------------------------------------------------------------
QVariant CMeasureAreaTool::Props::GetPropertyValue(int i)
{
	switch (i)
	{
	case 0: return m_ptool->m_bfilter; break;
	case 1: return m_ptool->m_minFilter; break;
	case 2: return m_ptool->m_maxFilter; break;
	case 3: return m_ptool->m_allSteps; break;
	case 4: return m_ptool->m_nsel; break;
	case 5: return m_ptool->m_area; break;
	}
	return QVariant();
}

//-----------------------------------------------------------------------------
void CMeasureAreaTool::Props::SetPropertyValue(int i, const QVariant& v)
{
	switch (i)
	{
	case 0: m_ptool->m_bfilter = v.toBool(); break;
	case 1: m_ptool->m_minFilter = v.toDouble(); break;
	case 2: m_ptool->m_maxFilter = v.toDouble(); break;
	case 3: m_ptool->m_allSteps = v.toBool(); break;
	}
}

//-----------------------------------------------------------------------------
CMeasureAreaTool::CMeasureAreaTool(CMainWindow* wnd) : CBasicTool("Measure Area", wnd, CBasicTool::HAS_APPLY_BUTTON)
{
	m_nsel = 0;
	m_area = 0.0;

	m_bfilter = false;
	m_minFilter = 0.0;
	m_maxFilter = 0.0;
	m_allSteps = false;
}

//-----------------------------------------------------------------------------
CPropertyList* CMeasureAreaTool::getPropertyList()
{ 
	return new Props(this); 
}

//-----------------------------------------------------------------------------
void CMeasureAreaTool::OnApply()
{
	m_nsel = 0;
	m_area = 0.0;
	CDocument* doc = GetActiveDocument();
	if (doc && doc->IsValid())
	{
		CGLModel& m = *doc->GetGLModel();
		FEModel& fem = *doc->GetFEModel();
		int index = m.currentTimeIndex();
		FEState* ps = fem.GetState(index);
		const vector<FEFace*> selectedFaces = doc->GetGLModel()->GetFaceSelection();
		m_nsel = (int)selectedFaces.size();

		Post::FEPostMesh* currentMesh = ps->GetFEMesh();

		if (m_allSteps)
		{
			int nstates = fem.GetStates();
			std::vector<double> area(nstates, 0.0);
			for (int i = 0; i < nstates; ++i)
			{
				FEState* state = fem.GetState(i);
				if (state->GetFEMesh() == currentMesh)
					area[i] = getValue(state, selectedFaces);
				else
					area[i] = 0.0;
			}

			m_area = area[index];

			m_wnd->ShowData(area, "Area");
		}
		else m_area = getValue(ps, selectedFaces);
	}
	updateUi();
}

double CMeasureAreaTool::getValue(FEState* state, const std::vector<FEFace*>& selectedFaces)
{
	double area = 0.0;
	int nsel = 0;
	Post::FEPostMesh& mesh = *state->GetFEMesh();
	int N = (int)selectedFaces.size();
	vector<vec3d> rt;
	for (int i = 0; i<N; ++i)
	{
		FEFace& f = *selectedFaces[i];

		float v = state->m_FACE[f.GetID() - 1].m_val;
		if ((m_bfilter == false) || ((v >= m_minFilter) && (v <= m_maxFilter)))
		{
			rt.resize(f.Nodes());
			for (int j = 0; j < f.Nodes(); ++j) rt[j] = state->m_NODE[f.n[j]].m_rt;
			area += mesh.FaceArea(rt, f.Nodes());
			++nsel;
		}
	}
	return area;
}

// update
void CMeasureAreaTool::update(bool breset)
{
	// Turned this off since otherwise it would show the graph window each time 
	// a user changed the selection or the time step
//	OnApply();
}
