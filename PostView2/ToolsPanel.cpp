#include "ToolsPanel.h"
#include <QBoxLayout>
#include <QSplitter>
#include <QGroupBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QButtonGroup>
#include <QLabel>

class CTool
{
public:
	CTool(const QString& s) : m_name(s) {}

	const QString& name() { return m_name; }
	void setName(const QString& s) { m_name = s; }

	QWidget* createUi()
	{
		QGroupBox* pg = new QGroupBox(name());
		QVBoxLayout* layout = new QVBoxLayout;
		pg->setLayout(layout);
		layout->addWidget(new QLabel(name()));
		return pg;
	}

private:
	QString	m_name;
};

static QList<CTool*>	tools;

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
		QList<CTool*>::Iterator it = tools.begin();
		for (int i=0; i<ntools; ++i, ++it)
		{
			CTool* tool = *it;
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
			CTool* tool = *it;
			QWidget* w = tool->createUi();
			stack->addWidget(w);
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

void CToolsPanel::initTools()
{
	tools.push_back(new CTool("Pt. Distance"));
	tools.push_back(new CTool("3Point Angle"));
	tools.push_back(new CTool("4Point Angle"));
	tools.push_back(new CTool("Plane"));
	tools.push_back(new CTool("Plotmix"));
	tools.push_back(new CTool("Measure Area"));
	tools.push_back(new CTool("Import Lines"));
	tools.push_back(new CTool("Kinemat"));
	tools.push_back(new CTool("Distance map"));
	tools.push_back(new CTool("Curvature map"));
	tools.push_back(new CTool("Pt. Congruency"));
	tools.push_back(new CTool("Add image"));
	tools.push_back(new CTool("Sphere fit"));
	tools.push_back(new CTool("Transform"));
	tools.push_back(new CTool("Shell thickness"));
	tools.push_back(new CTool("Line cut"));
	tools.push_back(new CTool("Add Point"));
	tools.push_back(new CTool("Import points"));
}

void CToolsPanel::on_buttons_buttonClicked(int id)
{
	ui->stack->setCurrentIndex(id);
}
