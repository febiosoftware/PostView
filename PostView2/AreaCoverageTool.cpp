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
#include "AreaCoverageTool.h"
#include <QBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <vector>
#include "Document.h"
#include <PostLib/FEAreaCoverage.h>
using namespace std;
using namespace Post;

class CAreaCoverageToolUI : public QWidget
{
public:
	QPushButton* p1;
	QPushButton* p2;
	QLineEdit*	name;

	FEAreaCoverage	m_tool;

public:
	CAreaCoverageToolUI(CAreaCoverageTool* tool) : m_tool(nullptr)
	{
		name = new QLineEdit; name->setPlaceholderText("Enter name here");
		p1 = new QPushButton("Assign to surface 1");
		p2 = new QPushButton("Assign to surface 2");

		QPushButton* apply = new QPushButton("Apply");

		QHBoxLayout* h = new QHBoxLayout;
		h->addWidget(new QLabel("Name:"));
		h->addWidget(name);

		QVBoxLayout* pv = new QVBoxLayout;
		pv->addLayout(h);
		pv->addWidget(p1);
		pv->addWidget(p2);
		pv->addWidget(apply);
		pv->addStretch();

		setLayout(pv);

		QObject::connect(p1, SIGNAL(clicked(bool)), tool, SLOT(OnAssign1()));
		QObject::connect(p2, SIGNAL(clicked(bool)), tool, SLOT(OnAssign2()));
		QObject::connect(apply, SIGNAL(clicked(bool)), tool, SLOT(OnApply()));
	}
};

//=============================================================================

CAreaCoverageTool::CAreaCoverageTool(CMainWindow* wnd) : CAbstractTool("Area Coverage", wnd)
{
	ui = 0;
}

QWidget* CAreaCoverageTool::createUi()
{
	return ui = new CAreaCoverageToolUI(this);
}

void CAreaCoverageTool::OnAssign1()
{
	CDocument* doc = GetActiveDocument();
	if (doc && doc->IsValid())
	{
		vector<int> sel;
		doc->GetGLModel()->GetSelectionList(sel, SELECT_FACES);
		ui->m_tool.SetSelection1(sel);
		int n = (int)sel.size();
		ui->p1->setText(QString("Assign to surface 1 (%1 faces)").arg(n));
	}
}

void CAreaCoverageTool::OnAssign2()
{
	CDocument* doc = GetActiveDocument();
	if (doc && doc->IsValid())
	{
		vector<int> sel;
		doc->GetGLModel()->GetSelectionList(sel, SELECT_FACES);
		ui->m_tool.SetSelection2(sel);
		int n = (int)sel.size();
		ui->p2->setText(QString("Assign to surface 2 (%1 faces)").arg(n));
	}
}

void CAreaCoverageTool::OnApply()
{
	CDocument* doc = GetActiveDocument();
	if (doc && doc->IsValid())
	{
/*		FEAreaCoverage& tool = ui->m_tool;

		tool.SetDataFieldName("");
		QString name = ui->name->text();
		if (name.isEmpty() == false)
		{
			tool.SetDataFieldName(name.toStdString());
		}
		tool.Apply(doc->GetFEModel());
		updateUi();
*/
	}
}
