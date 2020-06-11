/*This file is part of the PostView source code and is licensed under the MIT license
listed below.

See Copyright-PostView.txt for details.

Copyright (c) 2020 University of Utah, The Trustees of Columbia University in 
the City of New York, and others.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#include "stdafx.h"
#include "AddPointTool.h"
#include "Document.h"
#include <PostGL/GLModel.h>
using namespace Post;

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
CAddPointTool::CAddPointTool(CMainWindow* wnd) : CBasicTool("Add Point", wnd)
{
	m_deco = 0;
	m_pos = vec3f(0.f, 0.f, 0.f);
}

//-----------------------------------------------------------------------------
CPropertyList* CAddPointTool::getPropertyList()
{ 
	return new Props(this); 
}

//-----------------------------------------------------------------------------
void CAddPointTool::activate()
{
	CDocument* doc = GetActiveDocument();
	if (doc && doc->IsValid())
	{
		if (m_deco)
		{
			doc->GetGLModel()->RemoveDecoration(m_deco);
			delete m_deco;
			m_deco = 0;
		}
		m_deco = new GPointDecoration;
		doc->GetGLModel()->AddDecoration(m_deco);
		UpdateNode();
	}
}

//-----------------------------------------------------------------------------
void CAddPointTool::deactivate()
{
	if (m_deco)
	{
		CDocument* doc = GetActiveDocument();
		doc->GetGLModel()->RemoveDecoration(m_deco);
		delete m_deco;
		m_deco = 0;
	}
}

//-----------------------------------------------------------------------------
void CAddPointTool::UpdateNode()
{
	if (m_deco) m_deco->setVisible(false);
	CDocument* doc = GetActiveDocument();
	if (doc && doc->IsValid())
	{
		if (m_deco) 
		{
			m_deco->setPosition(m_pos);
			m_deco->setVisible(true);
		}
	}
	updateUi();
}
