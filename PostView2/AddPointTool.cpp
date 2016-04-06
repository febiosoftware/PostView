#include "stdafx.h"
#include "AddPointTool.h"
#include "Document.h"
#include "GLModel.h"

//-----------------------------------------------------------------------------
CAddPointTool::Props::Props(CAddPointTool* ptool) : m_ptool(ptool)
{
	addProperty("X", CProperty::Float);
	addProperty("Y", CProperty::Float);
	addProperty("Z", CProperty::Float);
}

//-----------------------------------------------------------------------------
QVariant CAddPointTool::Props::GetPropertyValue(int i)
{
	switch (i)
	{
	case 0: return m_ptool->m_pos.x; break;
	case 1: return m_ptool->m_pos.y; break;
	case 2: return m_ptool->m_pos.z; break;
	}
	return QVariant();
}

//-----------------------------------------------------------------------------
void CAddPointTool::Props::SetPropertyValue(int i, const QVariant& v)
{
	if (i==0) m_ptool->m_pos.x = v.toFloat();
	if (i==1) m_ptool->m_pos.y = v.toFloat();
	if (i==2) m_ptool->m_pos.z = v.toFloat();
	m_ptool->UpdateNode();
}

//-----------------------------------------------------------------------------
CAddPointTool::CAddPointTool() : CBasicTool("Add Point")
{
	m_doc = 0;
	m_deco = 0;

	m_pos = vec3f(0.f, 0.f, 0.f);
}

//-----------------------------------------------------------------------------
CPropertyList* CAddPointTool::getPropertyList()
{ 
	return new Props(this); 
}

//-----------------------------------------------------------------------------
void CAddPointTool::activate(CDocument* pdoc)
{
	m_doc = pdoc;
	if (pdoc && pdoc->IsValid())
	{
		if (m_deco)
		{
			m_doc->GetGLModel()->RemoveDecoration(m_deco);
			delete m_deco;
			m_deco = 0;
		}
		m_deco = new GPointDecoration;
		m_doc->GetGLModel()->AddDecoration(m_deco);
		UpdateNode();
	}
}

//-----------------------------------------------------------------------------
void CAddPointTool::deactivate()
{
	if (m_deco)
	{
		m_doc->GetGLModel()->RemoveDecoration(m_deco);
		delete m_deco;
		m_deco = 0;
	}
}

//-----------------------------------------------------------------------------
void CAddPointTool::UpdateNode()
{
	if (m_deco) m_deco->setVisible(false);
	if (m_doc && m_doc->IsValid())
	{
		if (m_deco) 
		{
			m_deco->setPosition(m_pos);
			m_deco->setVisible(true);
		}
	}
	updateUi();
}
