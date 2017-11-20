#include "stdafx.h"
#include "3PointAngleTool.h"
#include "Document.h"
#include "GLModel.h"

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
C3PointAngleTool::C3PointAngleTool(CDocument* doc) : CBasicTool("3Point Angle", doc)
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
		m_doc->GetGLModel()->RemoveDecoration(m_deco);
		delete m_deco;
		m_deco = 0;
	}
}

//-----------------------------------------------------------------------------
void C3PointAngleTool::update(bool breset)
{
	if (breset)
	{
		if (m_doc && m_doc->IsValid())
		{
			FEMeshBase& mesh = *m_doc->GetActiveMesh();
			const vector<FENode*> selectedNodes = m_doc->GetGLModel()->GetNodeSelection();
			int N = selectedNodes.size();
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
				m_doc->GetGLModel()->RemoveDecoration(m_deco);
				delete m_deco;
				m_deco = 0;
			}
			m_deco = new C3PointAngleDecoration;
			m_doc->GetGLModel()->AddDecoration(m_deco);
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
	if (m_doc && m_doc->IsValid())
	{
		FEModel& fem = *m_doc->GetFEModel();
		FEMeshBase& mesh = *m_doc->GetActiveMesh();
		int ntime = m_doc->currentTime();
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
