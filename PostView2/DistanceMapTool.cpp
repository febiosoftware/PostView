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
#include "DistanceMapTool.h"
#include <QWidget>
#include <QBoxLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QFormLayout>
#include <QLineEdit>
#include "Document.h"
#include "MainWindow.h"
#include <PostLib/FEPostModel.h>
#include <MeshTools/SphereFit.h>
#include "PropertyListView.h"
#include <PostLib/FEPointCongruency.h>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <PostGL/GLLinePlot.h>
#include <PostLib/FEDistanceMap.h>
#include <PostLib/FECurvatureMap.h>
using namespace Post;

class CDistanceMapToolUI : public QWidget
{
public:
	QCheckBox*	check;
	QPushButton* p1;
	QPushButton* p2;
	FEDistanceMap	m_map;

public:
	CDistanceMapToolUI(CDistanceMapTool* ptool) : m_map(nullptr)
	{
		QPushButton* apply;
		QVBoxLayout* pv = new QVBoxLayout;
		{
			p1 = new QPushButton("Assign to surface 1");
			p2 = new QPushButton("Assign to surface 2");
			check = new QCheckBox("Signed distance");
			apply = new QPushButton("Apply");

			pv->addWidget(p1);
			pv->addWidget(p2);
			pv->addWidget(check);
			pv->addWidget(apply);
			pv->addStretch();
		}
		setLayout(pv);
		QObject::connect(p1, SIGNAL(clicked(bool)), ptool, SLOT(OnAssign1()));
		QObject::connect(p2, SIGNAL(clicked(bool)), ptool, SLOT(OnAssign2()));
		QObject::connect(apply, SIGNAL(clicked(bool)), ptool, SLOT(OnApply()));
	}
};

// constructor
CDistanceMapTool::CDistanceMapTool(CMainWindow* wnd) : CAbstractTool("Distance Map", wnd)
{
	ui = 0;
}

// get the property list
QWidget* CDistanceMapTool::createUi()
{
	return ui = new CDistanceMapToolUI(this);
}

void CDistanceMapTool::OnAssign1()
{
	CDocument* doc = GetActiveDocument();
	if (doc && doc->IsValid())
	{
		vector<int> sel;
		doc->GetGLModel()->GetSelectionList(sel, SELECT_FACES);
		ui->m_map.SetSelection1(sel);
		int n = (int)sel.size();
		ui->p1->setText(QString("Assign to surface 1 (%1 faces)").arg(n));
	}
}

void CDistanceMapTool::OnAssign2()
{
	CDocument* doc = GetActiveDocument();
	if (doc && doc->IsValid())
	{
		vector<int> sel;
		doc->GetGLModel()->GetSelectionList(sel, SELECT_FACES);
		ui->m_map.SetSelection2(sel);
		int n = (int)sel.size();
		ui->p2->setText(QString("Assign to surface 2 (%1 faces)").arg(n));
	}
}

void CDistanceMapTool::OnApply()
{
	CDocument* doc = GetActiveDocument();
	if (doc && doc->IsValid())
	{
		bool bcheck = ui->check->isChecked();
		FEDistanceMap& map = ui->m_map;
		map.m_bsigned = bcheck;
		map.Apply(doc->GetFEModel());
		doc->UpdateObservers(true);
		updateUi();
	}
}


class CCurvatureMapToolUI : public QWidget
{
public:
	QPushButton* p1;
	QPushButton* p2;
	FECongruencyMap	m_map;

public:
	CCurvatureMapToolUI(CCurvatureMapTool* ptool)
	{
		QPushButton* apply;
		QVBoxLayout* pv = new QVBoxLayout;
		{
			p1 = new QPushButton("Assign to surface 1");
			p2 = new QPushButton("Assign to surface 2");
			apply = new QPushButton("Apply");

			pv->addWidget(p1);
			pv->addWidget(p2);
			pv->addWidget(apply);
			pv->addStretch();
		}
		setLayout(pv);
		QObject::connect(p1, SIGNAL(clicked(bool)), ptool, SLOT(OnAssign1()));
		QObject::connect(p2, SIGNAL(clicked(bool)), ptool, SLOT(OnAssign2()));
		QObject::connect(apply, SIGNAL(clicked(bool)), ptool, SLOT(OnApply()));
	}
};

// constructor
CCurvatureMapTool::CCurvatureMapTool(CMainWindow* wnd) : CAbstractTool("Curvature Map", wnd)
{
	ui = 0;
}

// get the property list
QWidget* CCurvatureMapTool::createUi()
{
	return ui = new CCurvatureMapToolUI(this);
}

void CCurvatureMapTool::OnAssign1()
{
	CDocument* doc = GetActiveDocument();
	if (doc && doc->IsValid())
	{
		vector<int> sel;
		doc->GetGLModel()->GetSelectionList(sel, SELECT_FACES);
		ui->m_map.SetSelection1(sel);
		int n = (int)sel.size();
		ui->p1->setText(QString("Assign to surface 1 (%1 faces)").arg(n));
	}
}

void CCurvatureMapTool::OnAssign2()
{
	CDocument* doc = GetActiveDocument();
	if (doc && doc->IsValid())
	{
		vector<int> sel;
		doc->GetGLModel()->GetSelectionList(sel, SELECT_FACES);
		ui->m_map.SetSelection2(sel);
		int n = (int)sel.size();
		ui->p2->setText(QString("Assign to surface 2 (%1 faces)").arg(n));
	}
}

void CCurvatureMapTool::OnApply()
{
	CDocument* doc = GetActiveDocument();
	if (doc && doc->IsValid())
	{
		FECongruencyMap& map = ui->m_map;
		map.Apply(*doc->GetFEModel());
		updateUi();

		// this tool adds a data field to the model
		// so we need to update the main toolbar
		m_wnd->UpdateMainToolbar();
	}
}
