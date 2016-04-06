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
CMeasureAreaTool::CMeasureAreaTool() : CBasicTool("Measure Area", CBasicTool::HAS_APPLY_BUTTON)
{
	m_doc = 0;

	m_nsel = 0;
	m_area = 0.0;
}

//-----------------------------------------------------------------------------
CPropertyList* CMeasureAreaTool::getPropertyList()
{ 
	return new Props(this); 
}

//-----------------------------------------------------------------------------
void CMeasureAreaTool::activate(CDocument* pdoc)
{
	m_doc = pdoc;
}

//-----------------------------------------------------------------------------
void CMeasureAreaTool::deactivate()
{
}

//-----------------------------------------------------------------------------
void CMeasureAreaTool::OnApply()
{
	m_nsel = 0;
	m_area = 0.0;
	if (m_doc && m_doc->IsValid())
	{
		FEModel& fem = *m_doc->GetFEModel();
		FEMesh& mesh = *fem.GetMesh();

		for (int i=0; i<mesh.Faces(); ++i)
		{
			FEFace& f = mesh.Face(i);
			if (f.IsSelected())
			{
				++m_nsel;
				m_area += mesh.FaceArea(f);
			}
		}
	}
	updateUi();
}
