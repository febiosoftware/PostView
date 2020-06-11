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

#include "StatePanel.h"
#include "ShellThicknessTool.h"
#include "Document.h"
#include <PostLib/FEPostModel.h>
using namespace Post;

//-----------------------------------------------------------------------------
CShellThicknessTool::Props::Props(CShellThicknessTool* ptool) : m_ptool(ptool)
{
	addProperty("shell thickness", CProperty::Float);
}

//-----------------------------------------------------------------------------
QVariant CShellThicknessTool::Props::GetPropertyValue(int i)
{
	switch (i)
	{
	case 0: return m_ptool->m_h; break;
	}
	return QVariant();
}

//-----------------------------------------------------------------------------
void CShellThicknessTool::Props::SetPropertyValue(int i, const QVariant& v)
{
	if (i==0) m_ptool->m_h = v.toDouble();
}

//-----------------------------------------------------------------------------
CShellThicknessTool::CShellThicknessTool(CMainWindow* wnd) : CBasicTool("Shell Thickness", wnd, CBasicTool::HAS_APPLY_BUTTON)
{
	m_h = 0.0;
}

//-----------------------------------------------------------------------------
CPropertyList* CShellThicknessTool::getPropertyList()
{ 
	return new Props(this); 
}

//-----------------------------------------------------------------------------
void CShellThicknessTool::OnApply()
{
	CDocument* doc = GetActiveDocument();
	if (doc && doc->IsValid())
	{
		FEPostModel& fem = *doc->GetFEModel();
		Post::FEPostMesh& mesh = *fem.GetFEMesh(0);

		int NS = fem.GetStates();
		for (int n=0; n<NS; ++n)
		{
			FEState& state = *fem.GetState(n);
			for (int i=0; i<mesh.Elements(); ++i)
			{
				FEElement_& elem = mesh.ElementRef(i);
				if (elem.IsSelected() && elem.IsShell())
				{
					int ne = elem.Nodes();
					for (int k=0; k<ne; ++k) state.m_ELEM[i].m_h[k] = m_h;
				}
			}
		}
		doc->UpdateFEModel(true);
	}
	updateUi();
}
