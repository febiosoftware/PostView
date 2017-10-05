#include "stdafx.h"
#include "MeasureAreaTool.h"
#include "Document.h"
#include <PostViewLib/FEModel.h>

//-----------------------------------------------------------------------------
CMeasureAreaTool::Props::Props(CMeasureAreaTool* ptool) : m_ptool(ptool)
{
	addProperty("selected faces", CProperty::Int  )->setFlags(CProperty::Visible);
	addProperty("area"          , CProperty::Float)->setFlags(CProperty::Visible);
}

//-----------------------------------------------------------------------------
QVariant CMeasureAreaTool::Props::GetPropertyValue(int i)
{
	switch (i)
	{
	case 0: return m_ptool->m_nsel; break;
	case 1: return m_ptool->m_area; break;
	}
	return QVariant();
}

//-----------------------------------------------------------------------------
void CMeasureAreaTool::Props::SetPropertyValue(int i, const QVariant& v)
{
}

//-----------------------------------------------------------------------------
CMeasureAreaTool::CMeasureAreaTool(CDocument* doc) : CBasicTool("Measure Area", doc, CBasicTool::HAS_APPLY_BUTTON)
{
	m_nsel = 0;
	m_area = 0.0;
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
	if (m_doc && m_doc->IsValid())
	{
		FEModel& fem = *m_doc->GetFEModel();
		FEMeshBase& mesh = *fem.GetFEMesh(0);
		const vector<FEFace*> selectedFaces = m_doc->GetGLModel()->GetFaceSelection();
		int N = selectedFaces.size();
		for (int i=0; i<N; ++i)
		{
			FEFace& f = *selectedFaces[i];
			m_area += mesh.FaceArea(f);
			++m_nsel;
		}
	}
	updateUi();
}
