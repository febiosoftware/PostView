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
#include "PlaneTool.h"
#include "CIntInput.h"
#include <QBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include "Document.h"
#include <PostGL/GLModel.h>
#include <PostLib/FEPostMesh.h>
using namespace Post;

//-----------------------------------------------------------------------------
class CPlaneDecoration : public GDecoration
{
public:
	CPlaneDecoration()
	{
		point[0] = new GPointDecoration(vec3f(0,0,0));
		point[1] = new GPointDecoration(vec3f(0,0,0));
		point[2] = new GPointDecoration(vec3f(0,0,0));
		line[0] = new GLineDecoration(point[0], point[1]);
		line[1] = new GLineDecoration(point[1], point[2]);
		line[2] = new GLineDecoration(point[2], point[0]);
		setVisible(false);
	}

	~CPlaneDecoration()
	{
		delete line[2]; 
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
		line[2]->render();
	}

private:
	GPointDecoration*	point[3];
	GLineDecoration*	line[3];
};

class CPlaneToolUI : public QWidget
{
public:
	int	m_node[3];

public:
	CIntInput* 	node1;
	CIntInput* 	node2;
	CIntInput* 	node3;

	CFloatInput* normx;
	CFloatInput* normy;
	CFloatInput* normz;

public:
	CPlaneToolUI(QObject* parent)
	{
		m_node[0] = 0;
		m_node[1] = 0;
		m_node[2] = 0;

		QVBoxLayout* pv = new QVBoxLayout;
		QGroupBox* pg = new QGroupBox("Select nodes");
		QFormLayout* pform = new QFormLayout;
		pform->addRow("node 1:", node1 = new CIntInput);
		pform->addRow("node 2:", node2 = new CIntInput);
		pform->addRow("node 3:", node3 = new CIntInput);
		pg->setLayout(pform);
		pv->addWidget(pg);

		pg = new QGroupBox("Plane normal:");
		pform = new QFormLayout;
		pform->addRow("x:", normx = new CFloatInput); normx->setReadOnly(true);
		pform->addRow("y:", normy = new CFloatInput); normy->setReadOnly(true);
		pform->addRow("z:", normz = new CFloatInput); normz->setReadOnly(true);
		pg->setLayout(pform);
		pv->addWidget(pg);

		QPushButton* pb = new QPushButton("Align View");
		pv->addWidget(pb);

		pv->addStretch();

		setLayout(pv);

		QObject::connect(node1, SIGNAL(editingFinished()), parent, SLOT(on_change_node1()));
		QObject::connect(node2, SIGNAL(editingFinished()), parent, SLOT(on_change_node2()));
		QObject::connect(node3, SIGNAL(editingFinished()), parent, SLOT(on_change_node3()));

		QObject::connect(pb, SIGNAL(clicked()), parent, SLOT(onAlignView()));
	}
};

CPlaneTool::CPlaneTool(CMainWindow* wnd) : CAbstractTool("Plane tool", wnd)
{
	m_dec = 0;
}

// get the UI widget
QWidget* CPlaneTool::createUi()
{
	return ui = new CPlaneToolUI(this);
}

// activate the tool
void CPlaneTool::activate()
{
	update(true);
}

// deactive the tool
void CPlaneTool::deactivate()
{
	if (m_dec)
	{
		CDocument* doc = GetActiveDocument();
		if (doc) doc->GetGLModel()->RemoveDecoration(m_dec);
		delete m_dec;
		m_dec = 0;
	}
}

void CPlaneTool::on_change_node1()
{
	ui->m_node[0] = ui->node1->value();
	UpdateNormal();
}

void CPlaneTool::on_change_node2()
{
	ui->m_node[1] = ui->node2->value();
	UpdateNormal();
}

void CPlaneTool::on_change_node3()
{
	ui->m_node[2] = ui->node3->value();
	UpdateNormal();
}

void CPlaneTool::UpdateNormal()
{
	int* node = ui->m_node;

	if ((node[0] > 0) && (node[1] > 0) && (node[2] > 0))
	{
		CDocument* doc = GetActiveDocument();
		Post::FEPostMesh* pm = doc->GetActiveMesh();

		FENode& n1 = pm->Node(node[0]-1);
		FENode& n2 = pm->Node(node[1]-1);
		FENode& n3 = pm->Node(node[2]-1);

		vec3d r1 = n1.r;
		vec3d r2 = n2.r;
		vec3d r3 = n3.r;

		vec3d rc = (r1 + r2 + r3)/3.0;

		vec3d e[3];
		e[0] = r1 - r2;
		e[1] = r3 - r2;
		e[2] = e[0] ^ e[1]; 
		e[1] = e[2] ^ e[0];

		e[0].Normalize();
		e[1].Normalize();
		e[2].Normalize();

		ui->normx->setValue(e[2].x);
		ui->normy->setValue(e[2].y);
		ui->normz->setValue(e[2].z);

		if (m_dec) 
		{
			m_dec->setPosition(to_vec3f(n1.r), to_vec3f(n2.r), to_vec3f(n3.r));
			m_dec->setVisible(true);
		}
	}
	else 
	{ 
		ui->normx->setValue(0.0);
		ui->normy->setValue(0.0);
		ui->normz->setValue(0.0);
	}
	updateUi();
}

void CPlaneTool::onAlignView()
{
	int* node = ui->m_node;
	if ((node[0] > 0) && (node[1] > 0) && (node[2] > 0))
	{
		CDocument* doc = GetActiveDocument();
		CGView& view = *doc->GetView();

		vec3f r(ui->normx->value(), ui->normy->value(), ui->normz->value());

		CGLCamera& cam = view.GetCamera();
		cam.SetViewDirection(r);

		updateUi();
	}
}

void CPlaneTool::update(bool breset)
{
	if (breset)
	{
		CDocument* doc = GetActiveDocument();
		int* node = ui->m_node;
		Post::FEPostMesh& mesh = *doc->GetActiveMesh();
		const vector<FENode*> selectedNodes = doc->GetGLModel()->GetNodeSelection();
		int N = (int)selectedNodes.size();
		int nsel = 0;
		for (int i = 0; i<N; ++i)
		{
			int nid = selectedNodes[i]->GetID();
			if      (node[0] == 0) node[0] = nid;
			else if (node[1] == 0) node[1] = nid;
			else if (node[2] == 0) node[2] = nid;
			else
			{
				node[0] = node[1];
				node[1] = node[2];
				node[2] = nid;
			}

			ui->node1->setValue(node[0]);
			ui->node2->setValue(node[1]);
			ui->node3->setValue(node[2]);
		}
		
		if (m_dec)
		{
			doc->GetGLModel()->RemoveDecoration(m_dec);
			delete m_dec;
			m_dec = 0;
		}
		m_dec = new CPlaneDecoration;
		doc->GetGLModel()->AddDecoration(m_dec);
		UpdateNormal();
	}
	else UpdateNormal();
}
