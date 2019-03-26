#include "stdafx.h"
#include "MeasureAreaTool.h"
#include "Document.h"
#include <PostViewLib/FEModel.h>

//-----------------------------------------------------------------------------
CMeasureAreaTool::Props::Props(CMeasureAreaTool* ptool) : m_ptool(ptool)
{
	addProperty("apply filter"  , CProperty::Bool);
	addProperty("min value"     , CProperty::Float);
	addProperty("max value"     , CProperty::Float);
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
	case 3: return m_ptool->m_nsel; break;
	case 4: return m_ptool->m_area; break;
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
		FEMeshBase& mesh = *fem.GetFEMesh(0);
		FEState* ps = fem.GetState(m.currentTimeIndex());
		const vector<FEFace*> selectedFaces = doc->GetGLModel()->GetFaceSelection();
		int N = (int)selectedFaces.size();
		for (int i=0; i<N; ++i)
		{
			FEFace& f = *selectedFaces[i];

			float v = ps->m_FACE[f.GetID() - 1].m_val;
			if ((m_bfilter == false) || ((v >= m_minFilter) && (v <= m_maxFilter)))
			{
				m_area += mesh.FaceArea(f);
				++m_nsel;
			}
		}
	}
	updateUi();
}

// update
void CMeasureAreaTool::update(bool breset)
{
	OnApply();
}
