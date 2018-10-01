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
#include "Tool.h"
#include "MainWindow.h"
#include "Document.h"
#include <PostViewLib/FEMesh.h>
#include <PostViewLib/FEModel.h>
#include <PostViewLib/GDecoration.h>
#include "GLModel.h"
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
#include "AddImage3DTool.h"
#include "PlotMixTool.h"
#include "AreaCoverageTool.h"
#include "StrainMapTool.h"

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

		pg->addWidget(box);
		pg->addWidget(stack);
		pg->addStretch();

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
	CDocument* doc = m_wnd->GetDocument();

	tools.push_back(new CPointDistanceTool  (doc));
	tools.push_back(new C3PointAngleTool    (doc));
	tools.push_back(new C4PointAngleTool    (doc));
	tools.push_back(new CPlaneTool          (doc));
	tools.push_back(new CPlotMixTool        (doc));
	tools.push_back(new CMeasureAreaTool    (doc));
	tools.push_back(new CImportLinesTool    (doc));
	tools.push_back(new CKinematTool        (doc));
	tools.push_back(new CDistanceMapTool    (doc));
	tools.push_back(new CCurvatureMapTool   (doc));
	tools.push_back(new CPointCongruencyTool(doc));
	tools.push_back(new CAddImage3DTool     (doc));
	tools.push_back(new CSphereFitTool      (doc));
	tools.push_back(new CTransformTool      (doc));
	tools.push_back(new CShellThicknessTool (doc));
	tools.push_back(new CAddPointTool       (doc));
	tools.push_back(new CImportPointsTool   (doc));
	tools.push_back(new CAreaCoverageTool   (doc));
	tools.push_back(new CStrainMapTool      (doc));
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
