#include "stdafx.h"
#include "PointDistanceTool.h"
#include <PostViewLib/GDecoration.h>
#include "Document.h"
#include <PostGL/GLModel.h>
using namespace Post;

class CPointDistanceDecoration : public GDecoration
{
public:
	CPointDistanceDecoration()
	{
		point[0] = new GPointDecoration(vec3f(0,0,0));
		point[1] = new GPointDecoration(vec3f(1,1,1));
		line = new GLineDecoration(point[0], point[1]);
		setVisible(false);
	}

	void setPosition(const vec3f& a, const vec3f& b)
	{
		point[0]->setPosition(a);
		point[1]->setPosition(b);
	}

	~CPointDistanceDecoration()
	{
		delete line;
		delete point[0];
		delete point[1];
	}

	void render()
	{
		point[0]->render();
		point[1]->render();
		line->render();
	}

private:
	GPointDecoration*	point[2];
	GLineDecoration*	line;
};

CPointDistanceTool::Props::Props(CPointDistanceTool* ptool) : tool(ptool)
{
	addProperty("node 1" , CProperty::Int);
	addProperty("node 2" , CProperty::Int);
	addProperty("Dx"     , CProperty::Float)->setFlags(CProperty::Visible);
	addProperty("Dy"     , CProperty::Float)->setFlags(CProperty::Visible);
	addProperty("Dz"     , CProperty::Float)->setFlags(CProperty::Visible);
	addProperty("Length" , CProperty::Float)->setFlags(CProperty::Visible);
	addProperty("Stretch", CProperty::Float)->setFlags(CProperty::Visible);
}

QVariant CPointDistanceTool::Props::GetPropertyValue(int i)
{
	switch (i)
	{
	case 0: return tool->m_node1; break;
	case 1: return tool->m_node2; break;
	case 2: return fabs(tool->m_d.x); break;
	case 3: return fabs(tool->m_d.y); break;
	case 4: return fabs(tool->m_d.z); break;
	case 5: return tool->m_d.Length(); break;
	case 6: 
		{
			double s = 0.0;
			if (tool->m_bvalid)
			{
				double L = tool->m_d.Length();
				double L0 = tool->m_d0.Length();
				if (L0 != 0.0) s = L / L0;
			}
			return s;
		}
		break;
	}
	return QVariant();
}

void CPointDistanceTool::Props::SetPropertyValue(int i, const QVariant& v)
{
	if (i==0) tool->m_node1 = v.toInt();
	if (i==1) tool->m_node2 = v.toInt();
	tool->updateLength();
}

CPointDistanceTool::CPointDistanceTool(CMainWindow* wnd) : CBasicTool("Pt.Distance", wnd)
{ 
	m_node1 = 0; 
	m_node2 = 0; 
	m_d = vec3f(0,0,0); 
	m_deco = 0;
	m_bvalid = false;
}

CPropertyList* CPointDistanceTool::getPropertyList()
{ 
	return new Props(this); 
}

void CPointDistanceTool::activate()
{
	update(true);
}

void CPointDistanceTool::deactivate()
{
	if (m_deco)
	{
		CDocument* doc = GetActiveDocument();
		if (doc) doc->GetGLModel()->RemoveDecoration(m_deco);
		delete m_deco;
		m_deco = 0;
	}
}

void CPointDistanceTool::updateUi()
{
	CBasicTool::updateUi();
}

void CPointDistanceTool::updateLength()
{
	m_bvalid = false;
	m_d = vec3f(0.f,0.f,0.f);
	if (m_deco) m_deco->setVisible(false);
	CDocument* doc = GetActiveDocument();
	if (doc && doc->IsValid())
	{
		FEModel& fem = *doc->GetFEModel();
		FEMeshBase& mesh = *doc->GetActiveMesh();
		int ntime = doc->currentTime();
		int NN = mesh.Nodes();
		if ((m_node1 > 0)&&(m_node2 > 0)&&(m_node1 <= NN)&&(m_node2 <= NN))
		{
			vec3f a0 = mesh.Node(m_node1 - 1).m_r0;
			vec3f b0 = mesh.Node(m_node2 - 1).m_r0;
			m_d0 = b0 - a0;
			vec3f at = fem.NodePosition(m_node1 - 1, ntime);
			vec3f bt = fem.NodePosition(m_node2 - 1, ntime);
			m_d = bt - at;
			m_bvalid = true;
			if (m_deco) 
			{
				m_deco->setPosition(at, bt);
				m_deco->setVisible(true);
			}
		}
	}
}

void CPointDistanceTool::update(bool reset)
{
	if (reset)
	{
		CDocument* doc = GetActiveDocument();
		if (doc && doc->IsValid())
		{
			FEMeshBase& mesh = *doc->GetActiveMesh();
			const vector<FENode*> selectedNodes = doc->GetGLModel()->GetNodeSelection();
			int N = (int) selectedNodes.size();
			for (int i = 0; i<N; ++i)
			{
				int nid = selectedNodes[i]->GetID();
				if (m_node1 == 0) m_node1 = nid;
				else if (m_node2 == 0) m_node2 = nid;
				else
				{
					m_node1 = m_node2;
					m_node2 = nid;
				}
			}

			if (m_deco)
			{
				doc->GetGLModel()->RemoveDecoration(m_deco);
				delete m_deco;
				m_deco = 0;
			}
			m_deco = new CPointDistanceDecoration;
			doc->GetGLModel()->AddDecoration(m_deco);
			updateLength();
		}
	}
	else updateLength();

	updateUi();
}
