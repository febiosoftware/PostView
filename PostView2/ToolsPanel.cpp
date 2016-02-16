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
#include "PropertyList.h"

class CTool
{
public:
	CTool(const QString& s) : m_name(s) {}

	const QString& name() { return m_name; }
	void setName(const QString& s) { m_name = s; }

	virtual QWidget* createUi()
	{
		CPropertyList* pl = getPropertyList();
		if (pl == 0) return 0;

		QWidget* pw = new QWidget;
		QFormLayout* pg = new QFormLayout(pw);
		pw->setLayout(pg);

		int nprops = pl->Properties();
		for (int i=0; i<nprops; ++i)
		{
			const CProperty& pi = pl->Property(i);
			if (pi.type == CProperty::Int)
			{
				pg->addRow(pi.name, new QSpinBox);
			}
		}
		return pw;
	}

	virtual CPropertyList* getPropertyList()
	{
		return 0;
	}

private:
	QString	m_name;
};

class CPointDistanceTool : public CTool
{
	class Props : public CPropertyList
	{
	public:
		Props(CPointDistanceTool* ptool) : tool(ptool)
		{
			addProperty("node 1", CProperty::Int);
			addProperty("node 2", CProperty::Int);
		}

		QVariant GetPropertyValue(int i)
		{
			if (i==0) return tool->m_node1;
			if (i==1) return tool->m_node2;
			return QVariant();
		}

		void SetPropertyValue(int i, const QVariant& v)
		{
		}

	private:
		CPointDistanceTool*	tool;
	};

public:
	CPointDistanceTool() : CTool("Pt.Distance") {}

	CPropertyList* getPropertyList()
	{
		return new Props(this);
	}

private:
	int		m_node1, m_node2;

	friend class Props;
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

void CToolsPanel::initTools()
{
	tools.push_back(new CPointDistanceTool);
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
