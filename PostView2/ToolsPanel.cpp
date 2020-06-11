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

#include "ToolsPanel.h"
#include <QBoxLayout>
#include <QSplitter>
#include <QGroupBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QButtonGroup>
#include <QLabel>
#include <QFormLayout>
#include <QSpinBox>
#include <QCloseEvent>
#include <QShowEvent>
#include <QScrollArea>
#include "Tool.h"
#include "MainWindow.h"
#include "Document.h"
#include <PostLib/FEPostMesh.h>
#include <PostLib/FEPostModel.h>
#include <GLLib/GDecoration.h>
#include <PostGL/GLModel.h>
#include "PointDistanceTool.h"
#include "3PointAngleTool.h"
#include "4PointAngleTool.h"
#include "AddPointTool.h"
#include "MeasureAreaTool.h"
#include "TransformTool.h"
#include "ShellThicknessTool.h"
#include "SphereFitTool.h"
#include "PointCongruencyTool.h"
#include "ImportLinesTool.h"
#include "DistanceMapTool.h"
#include "PlaneTool.h"
#include "PlotMixTool.h"
#include "AreaCoverageTool.h"
#include "StrainMapTool.h"
#include "MeasureVolumeTool.h"

static QList<CAbstractTool*>	tools;

class Ui::CToolsPanel
{
public:
	QStackedWidget*	stack;
	QButtonGroup*	group;
	CAbstractTool*		activeTool;
	int					activeID;

public:
	void setupUi(::CToolsPanel* parent)
	{
		activeID = -1;
		activeTool = 0;

		QVBoxLayout* pg = new QVBoxLayout(parent);
		pg->setMargin(2);
		
		QGroupBox* box = new QGroupBox("Tools");
		
		QGridLayout* grid = new QGridLayout;
		box->setLayout(grid);
		grid->setSpacing(2);

		group = new QButtonGroup(box);
		group->setObjectName("buttons");

		int ntools = tools.size();
		QList<CAbstractTool*>::Iterator it = tools.begin();
		for (int i=0; i<ntools; ++i, ++it)
		{
			CAbstractTool* tool = *it;
			QPushButton* but = new QPushButton(tool->name());
			but->setCheckable(true);

			grid->addWidget(but, i/3, i%3);
			group->addButton(but); group->setId(but, i+1);
		}

		stack = new QStackedWidget;
		QLabel* label = new QLabel("(No tool selected)");
		label->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
		stack->addWidget(label);

		it = tools.begin();
		for (int i=0; i<ntools; ++i, ++it)
		{
			CAbstractTool* tool = *it;
			QGroupBox* pg = new QGroupBox(tool->name());
			QVBoxLayout* layout = new QVBoxLayout;
			pg->setLayout(layout);

			QWidget* pw = tool->createUi();
			if (pw == 0)
			{
				QLabel* pl = new QLabel("(no properties)");
				pl->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
				layout->addWidget(pl);
			}
			else layout->addWidget(pw);
			stack->addWidget(pg);
		}
		
		QScrollArea* scrollArea = new QScrollArea();
		
		scrollArea->setWidget(stack);

		pg->addWidget(box);
		pg->addWidget(scrollArea);

		QMetaObject::connectSlotsByName(parent);
	}
};

CToolsPanel::CToolsPanel(CMainWindow* window, QWidget* parent) : CCommandPanel(window, parent), ui(new Ui::CToolsPanel)
{
	initTools();
	ui->setupUi(this);
}

void CToolsPanel::Update(bool breset)
{
	if (ui->activeTool)
	{
		ui->activeTool->update(breset);

		// repaint parent
		m_wnd->RedrawGL();
	}
}

void CToolsPanel::initTools()
{
	tools.push_back(new CPointDistanceTool  (m_wnd));
	tools.push_back(new C3PointAngleTool    (m_wnd));
	tools.push_back(new C4PointAngleTool    (m_wnd));
	tools.push_back(new CPlaneTool          (m_wnd));
	tools.push_back(new CPlotMixTool        (m_wnd));
	tools.push_back(new CMeasureAreaTool    (m_wnd));
	tools.push_back(new CImportLinesTool    (m_wnd));
	tools.push_back(new CKinematTool        (m_wnd));
	tools.push_back(new CDistanceMapTool    (m_wnd));
	tools.push_back(new CCurvatureMapTool   (m_wnd));
	tools.push_back(new CPointCongruencyTool(m_wnd));
	tools.push_back(new CSphereFitTool      (m_wnd));
	tools.push_back(new CTransformTool      (m_wnd));
	tools.push_back(new CShellThicknessTool (m_wnd));
	tools.push_back(new CAddPointTool       (m_wnd));
	tools.push_back(new CImportPointsTool   (m_wnd));
	tools.push_back(new CAreaCoverageTool   (m_wnd));
	tools.push_back(new CStrainMapTool      (m_wnd));
	tools.push_back(new CMeasureVolumeTool  (m_wnd));
}

void CToolsPanel::on_buttons_buttonClicked(int id)
{
	if (ui->activeID == id)
	{
		ui->stack->setCurrentIndex(0);
		ui->activeTool = 0;
		ui->activeID = -1;
	}
	else
	{
		// deactivate the active tool
		ui->activeTool = 0;
		ui->activeID = id;

		// find the tool
		ui->activeTool = tools.at(id - 1); assert(ui->activeTool);

		// show the tab
		ui->stack->setCurrentIndex(id);
	}

	// repaint parent
	m_wnd->RedrawGL();
}
