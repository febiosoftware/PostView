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
