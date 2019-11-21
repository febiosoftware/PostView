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
#include <PostLib/FEMesh.h>
#include <PostLib/FEModel.h>
#include <PostLib/GDecoration.h>
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
