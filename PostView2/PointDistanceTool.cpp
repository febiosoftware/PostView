#include "stdafx.h"
#include "PointDistanceTool.h"
#include <PostViewLib/GDecoration.h>
#include "Document.h"
#include "GLModel.h"

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
	addProperty("node 1", CProperty::Int);
	addProperty("node 2", CProperty::Int);
	addProperty("Dx"    , CProperty::Float)->setFlags(CProperty::Visible);
	addProperty("Dy"    , CProperty::Float)->setFlags(CProperty::Visible);
	addProperty("Dz"    , CProperty::Float)->setFlags(CProperty::Visible);
	addProperty("Length", CProperty::Float)->setFlags(CProperty::Visible);
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
	}
	return QVariant();
}

void CPointDistanceTool::Props::SetPropertyValue(int i, const QVariant& v)
{
	if (i==0) tool->m_node1 = v.toInt();
	if (i==1) tool->m_node2 = v.toInt();
	tool->updateLength();
}

CPointDistanceTool::CPointDistanceTool() : CBasicTool("Pt.Distance")
{ 
	m_node1 = 0; 
	m_node2 = 0; 
	m_d = vec3f(0,0,0); 
	m_doc = 0; 
	m_deco = 0; 
}

CPropertyList* CPointDistanceTool::getPropertyList()
{ 
	return new Props(this); 
}

void CPointDistanceTool::activate(CDocument* pdoc)
{
	m_doc = pdoc;
	if (pdoc && pdoc->IsValid())
	{
		updateLength();
		if (m_deco)
		{
			m_doc->GetGLModel()->RemoveDecoration(m_deco);
			delete m_deco;
			m_deco = 0;
		}
		m_deco = new CPointDistanceDecoration;
		m_doc->GetGLModel()->AddDecoration(m_deco);
	}
}

void CPointDistanceTool::deactivate()
{
	if (m_deco)
	{
		m_doc->GetGLModel()->RemoveDecoration(m_deco);
		delete m_deco;
		m_deco = 0;
	}
}

void CPointDistanceTool::updateLength()
{
	m_d = vec3f(0.f,0.f,0.f);
	if (m_deco) m_deco->setVisible(false);
	if (m_doc && m_doc->IsValid())
	{
		FEModel& fem = *m_doc->GetFEModel();
		FEMesh& mesh = *fem.GetMesh();
		int ntime = m_doc->currentTime();
		int NN = mesh.Nodes();
		if ((m_node1 > 0)&&(m_node2 > 0)&&(m_node1 <= NN)&&(m_node2 <= NN))
		{
			vec3f a = fem.NodePosition(m_node1-1, ntime);
			vec3f b = fem.NodePosition(m_node2-1, ntime);
			m_d = b - a;

			if (m_deco) 
			{
				m_deco->setPosition(a, b);
				m_deco->setVisible(true);
			}
		}
	}
	updateUi();
}
