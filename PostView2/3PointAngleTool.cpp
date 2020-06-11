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
#include "3PointAngleTool.h"
#include "Document.h"
#include <PostGL/GLModel.h>
using namespace Post;

//-----------------------------------------------------------------------------
class C3PointAngleDecoration : public GDecoration
{
public:
	C3PointAngleDecoration()
	{
		point[0] = new GPointDecoration(vec3f(0,0,0));
		point[1] = new GPointDecoration(vec3f(0,0,0));
		point[2] = new GPointDecoration(vec3f(0,0,0));
		line[0] = new GLineDecoration(point[0], point[1]);
		line[1] = new GLineDecoration(point[1], point[2]);
		setVisible(false);
	}

	~C3PointAngleDecoration()
	{
		delete line[1]; 
		delete line[0];
		delete point[2];
		delete point[1];
		delete point[0];
	}

	void setPosition(const vec3f& a, const vec3f& b, const vec3f& c)
	{
		point[0]->setPosition(a);
		point[1]->setPosition(b);
		point[2]->setPosition(c);
	}

	void render()
	{
		point[0]->render();
		point[1]->render();
		point[2]->render();
		line[0]->render();
		line[1]->render();
	}

private:
	GPointDecoration*	point[3];
	GLineDecoration*	line[2];
};

//-----------------------------------------------------------------------------
C3PointAngleTool::Props::Props(C3PointAngleTool* ptool) : m_ptool(ptool)
{
	addProperty("node 1", CProperty::Int);
	addProperty("node 2", CProperty::Int);
	addProperty("node 3", CProperty::Int);
	addProperty("angle", CProperty::Float)->setFlags(CProperty::Visible);
}

//-----------------------------------------------------------------------------
QVariant C3PointAngleTool::Props::GetPropertyValue(int i)
{
	switch (i)
	{
	case 0: return m_ptool->m_node[0]; break;
	case 1: return m_ptool->m_node[1]; break;
	case 2: return m_ptool->m_node[2]; break;
	case 3: return m_ptool->m_angle; break;
	}
	return QVariant();
}

//-----------------------------------------------------------------------------
void C3PointAngleTool::Props::SetPropertyValue(int i, const QVariant& v)
{
	if (i==0) m_ptool->m_node[0] = v.toInt();
	if (i==1) m_ptool->m_node[1] = v.toInt();
	if (i==2) m_ptool->m_node[2] = v.toInt();
	m_ptool->UpdateAngle();
}

//-----------------------------------------------------------------------------
C3PointAngleTool::C3PointAngleTool(CMainWindow* wnd) : CBasicTool("3Point Angle", wnd)
{
	m_deco = 0;

	m_node[0] = 0;
	m_node[1] = 0;
	m_node[2] = 0;
	m_angle = 0.0;
}

//-----------------------------------------------------------------------------
CPropertyList* C3PointAngleTool::getPropertyList()
{ 
	return new Props(this); 
}

//-----------------------------------------------------------------------------
void C3PointAngleTool::activate()
{
	update(true);
}

//-----------------------------------------------------------------------------
void C3PointAngleTool::deactivate()
{
	if (m_deco)
	{
		CDocument* doc = GetActiveDocument();
		if (doc) doc->GetGLModel()->RemoveDecoration(m_deco);
		delete m_deco;
		m_deco = 0;
	}
}

//-----------------------------------------------------------------------------
void C3PointAngleTool::update(bool breset)
{
	if (breset)
	{
		CDocument* doc = GetActiveDocument();
		if (doc && doc->IsValid())
		{
			Post::FEPostMesh& mesh = *doc->GetActiveMesh();
			const vector<FENode*> selectedNodes = doc->GetGLModel()->GetNodeSelection();
			int N = (int) selectedNodes.size();
			int nsel = 0;
			for (int i = 0; i<N; ++i)
			{
				int nid = selectedNodes[i]->GetID();
				if      (m_node[0] == 0) m_node[0] = nid;
				else if (m_node[1] == 0) m_node[1] = nid;
				else if (m_node[2] == 0) m_node[2] = nid;
				else
				{
					m_node[0] = m_node[1];
					m_node[1] = m_node[2];
					m_node[2] = nid;
				}
			}

			if (m_deco)
			{
				doc->GetGLModel()->RemoveDecoration(m_deco);
				delete m_deco;
				m_deco = 0;
			}
			m_deco = new C3PointAngleDecoration;
			doc->GetGLModel()->AddDecoration(m_deco);
			UpdateAngle();
		}
	}
	else UpdateAngle();
}

//-----------------------------------------------------------------------------
void C3PointAngleTool::UpdateAngle()
{
	m_angle = 0.0;
	if (m_deco) m_deco->setVisible(false);
	CDocument* doc = GetActiveDocument();
	if (doc && doc->IsValid())
	{
		FEPostModel& fem = *doc->GetFEModel();
		Post::FEPostMesh& mesh = *doc->GetActiveMesh();
		int ntime = doc->currentTime();
		int NN = mesh.Nodes();
		if ((m_node[0] >   0)&&(m_node[1] >   0)&&(m_node[2] >   0)&&
			(m_node[0] <= NN)&&(m_node[1] <= NN)&&(m_node[2] <= NN))
		{
			vec3f a = fem.NodePosition(m_node[0]-1, ntime);
			vec3f b = fem.NodePosition(m_node[1]-1, ntime);
			vec3f c = fem.NodePosition(m_node[2]-1, ntime);
			
			vec3f e1 = a - b; e1.Normalize();
			vec3f e2 = c - b; e2.Normalize();

			m_angle = 180.0*acos(e1*e2)/PI;

			if (m_deco) 
			{
				m_deco->setPosition(a, b, c);
				m_deco->setVisible(true);
			}
		}
	}
	updateUi();
}
