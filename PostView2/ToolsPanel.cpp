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

static QList<CAbstractTool*>	tools;

class Ui::CToolsPanel
{
public:
	QStackedWidget*	stack;

public:
	void setupUi(::CToolsPanel* parent)
	{
		QVBoxLayout* pg = new QVBoxLayout(parent);
		pg->setMargin(2);
		
		QGroupBox* box = new QGroupBox("Tools");
		
		QGridLayout* grid = new QGridLayout;
		box->setLayout(grid);
		grid->setSpacing(2);

		QButtonGroup* group = new QButtonGroup(box);
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
	activeTool = 0;
	initTools();
	ui->setupUi(this);
}

void CToolsPanel::Update()
{
	if (activeTool)
	{
		activeTool->deactivate();

		activeTool->activate(m_wnd->GetDocument());
	}
}

void CToolsPanel::initTools()
{
	tools.push_back(new CPointDistanceTool);
	tools.push_back(new C3PointAngleTool  );
	tools.push_back(new C4PointAngleTool  );
	//tools.push_back(new CTool("Plane"));
	//tools.push_back(new CTool("Plotmix"));
	tools.push_back(new CMeasureAreaTool);
	tools.push_back(new CImportLinesTool);
	//tools.push_back(new CTool("Kinemat"));
	tools.push_back(new CDistanceMapTool);
	//tools.push_back(new CTool("Curvature map"));
	tools.push_back(new CPointCongruencyTool);
	//tools.push_back(new CTool("Add image"));
	tools.push_back(new CSphereFitTool);
	tools.push_back(new CTransformTool);
	tools.push_back(new CShellThicknessTool);
	//tools.push_back(new CTool("Line cut"));
	tools.push_back(new CAddPointTool);
	//tools.push_back(new CTool("Import points"));
}

void CToolsPanel::on_buttons_buttonClicked(int id)
{
	// deactivate the active tool
	if (activeTool) activeTool->deactivate();
	activeTool = 0;

	// find the tool
	QList<CAbstractTool*>::iterator it = tools.begin();
	for (int i=0; i<id-1; ++i, ++it);

	// get the active document
	CDocument* doc = m_wnd->GetDocument();

	// activate the tool
	activeTool = *it;
	activeTool->activate(doc);

	// show the tab
	ui->stack->setCurrentIndex(id);
}

void CToolsPanel::hideEvent(QHideEvent* ev)
{
	if (activeTool)
	{
		activeTool->deactivate();
	}
	ev->accept();
}

void CToolsPanel::showEvent(QShowEvent* ev)
{
	if (activeTool)
	{
		activeTool->activate(m_wnd->GetDocument());
	}
	ev->accept();
}
