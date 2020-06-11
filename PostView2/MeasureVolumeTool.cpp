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
#include "MeasureVolumeTool.h"
#include "Document.h"
#include <PostLib/FEPostModel.h>
using namespace Post;

//-----------------------------------------------------------------------------
CMeasureVolumeTool::Props::Props(CMeasureVolumeTool* ptool) : m_ptool(ptool)
{
	addProperty("selected faces", CProperty::Int)->setFlags(CProperty::Visible);
	addProperty("volume", CProperty::Float)->setFlags(CProperty::Visible);
	addProperty("symmetry", CProperty::Enum)->setEnumValues(QStringList() << "(None)" << "X" << "Y" << "Z");
}

//-----------------------------------------------------------------------------
QVariant CMeasureVolumeTool::Props::GetPropertyValue(int i)
{
	switch (i)
	{
	case 0: return m_ptool->m_nsel; break;
	case 1: return m_ptool->m_vol; break;
	case 2: return m_ptool->m_nformula; break;
	}
	return QVariant();
}

//-----------------------------------------------------------------------------
void CMeasureVolumeTool::Props::SetPropertyValue(int i, const QVariant& v)
{
	if (i == 2) m_ptool->m_nformula = v.toInt();
}

//-----------------------------------------------------------------------------
CMeasureVolumeTool::CMeasureVolumeTool(CMainWindow* wnd) : CBasicTool("Measure Volume", wnd, CBasicTool::HAS_APPLY_BUTTON)
{
	m_nsel = 0;
	m_vol = 0.0;
	m_nformula = 0;
}

//-----------------------------------------------------------------------------
CPropertyList* CMeasureVolumeTool::getPropertyList()
{
	return new Props(this);
}

//-----------------------------------------------------------------------------
void CMeasureVolumeTool::OnApply()
{
	m_nsel = 0;
	m_vol = 0.0;
	CDocument* doc = GetActiveDocument();
	if (doc && doc->IsValid())
	{
		FEPostModel& fem = *doc->GetFEModel();
		int ntime = fem.CurrentTime();
		Post::FEPostMesh& mesh = *fem.GetFEMesh(0);
		const vector<FEFace*> selectedFaces = doc->GetGLModel()->GetFaceSelection();
		int N = (int)selectedFaces.size();
		for (int i = 0; i<N; ++i)
		{
			FEFace& f = *selectedFaces[i];

			// get the average position, area and normal
			vec3d r = mesh.FaceCenter(f);
			double area = mesh.FaceArea(f);
			vec3d N = f.m_fn;

			switch (m_nformula)
			{
			case 0: m_vol += area*(N*r) / 3.f; break;
			case 1: m_vol += 2.f*area*(r.x*N.x); break;
			case 2: m_vol += 2.f*area*(r.y*N.y); break;
			case 3: m_vol += 2.f*area*(r.z*N.z); break;
			}
			++m_nsel;
		}

		m_vol = fabs(m_vol);
	}
	updateUi();
}
