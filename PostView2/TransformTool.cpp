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
CTransformTool::CTransformTool(CDocument* doc) : CBasicTool("Transform", doc, CBasicTool::HAS_APPLY_BUTTON)
{
	m_dr = vec3f(0.f, 0.f, 0.f);
}

//-----------------------------------------------------------------------------
CPropertyList* CTransformTool::getPropertyList()
{ 
	return new Props(this); 
}

//-----------------------------------------------------------------------------
void CTransformTool::OnApply()
{
	if (m_doc && m_doc->IsValid())
	{
		const vector<FENode*> selNodes = m_doc->GetGLModel()->GetNodeSelection();
		for (int i=0; i<(int)selNodes.size(); ++i)
		{
			FENode& node = *selNodes[i];
			node.m_r0 += m_dr;
		}
		m_doc->UpdateFEModel(true);
	}
	updateUi();
}
