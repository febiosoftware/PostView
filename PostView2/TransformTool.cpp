#include "stdafx.h"
#include "TransformTool.h"
#include "Document.h"
#include <PostViewLib/FEModel.h>

//-----------------------------------------------------------------------------
CTransformTool::Props::Props(CTransformTool* ptool) : m_ptool(ptool)
{
	addProperty("x-translation", CProperty::Float);
	addProperty("y-translation", CProperty::Float);
	addProperty("z-translation", CProperty::Float);
}

//-----------------------------------------------------------------------------
QVariant CTransformTool::Props::GetPropertyValue(int i)
{
	switch (i)
	{
	case 0: return m_ptool->m_dr.x; break;
	case 1: return m_ptool->m_dr.y; break;
	case 2: return m_ptool->m_dr.z; break;
	}
	return QVariant();
}

//-----------------------------------------------------------------------------
void CTransformTool::Props::SetPropertyValue(int i, const QVariant& v)
{
	switch (i)
	{
	case 0: m_ptool->m_dr.x = v.toFloat(); break;
	case 1: m_ptool->m_dr.y = v.toFloat(); break;
	case 2: m_ptool->m_dr.z = v.toFloat(); break;
	}
}

//-----------------------------------------------------------------------------
CTransformTool::CTransformTool() : CBasicTool("Transform", CBasicTool::HAS_APPLY_BUTTON)
{
	m_doc = 0;

	m_dr = vec3f(0.f, 0.f, 0.f);
}

//-----------------------------------------------------------------------------
CPropertyList* CTransformTool::getPropertyList()
{ 
	return new Props(this); 
}

//-----------------------------------------------------------------------------
void CTransformTool::activate(CDocument* pdoc)
{
	m_doc = pdoc;
}

//-----------------------------------------------------------------------------
void CTransformTool::deactivate()
{
}

//-----------------------------------------------------------------------------
void CTransformTool::OnApply()
{
	if (m_doc && m_doc->IsValid())
	{
		FEModel& fem = *m_doc->GetFEModel();
		FEMesh& mesh = *fem.GetMesh();

		for (int i=0; i<mesh.Nodes(); ++i)
		{
			FENode& node = mesh.Node(i);
			if (node.IsSelected()) node.m_r0 += m_dr;
		}
		m_doc->UpdateFEModel(true);
	}
	updateUi();
}
