#include "stdafx.h"
#include "TransformTool.h"
#include "Document.h"
#include <PostLib/FEPostModel.h>
using namespace Post;

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
CTransformTool::CTransformTool(CMainWindow* wnd) : CBasicTool("Transform", wnd, CBasicTool::HAS_APPLY_BUTTON)
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
	CDocument* doc = GetActiveDocument();
	if (doc && doc->IsValid())
	{
		const vector<FENode*> selNodes = doc->GetGLModel()->GetNodeSelection();
		for (int i=0; i<(int)selNodes.size(); ++i)
		{
			FENode& node = *selNodes[i];
			node.r += m_dr;
		}
		doc->UpdateFEModel(true);
	}
	updateUi();
}
