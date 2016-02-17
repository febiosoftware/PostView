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

#ifdef GetCurrentTime
#undef GetCurrentTime
#endif

class CPointDistanceDecoration : public GDecoration
{
public:
	CPointDistanceDecoration()
	{
		point[0] = new GPointDecoration(vec3f(0,0,0));
		point[1] = new GPointDecoration(vec3f(1,1,1));
		line = new GLineDecoration(point[0], point[1]);
		setVisible(false);
	}

	void setPosition(const vec3f& a, const vec3f& b)
	{
		point[0]->setPosition(a);
		point[1]->setPosition(b);
	}

	~CPointDistanceDecoration()
	{
		delete line;
		delete point[0];
		delete point[1];
	}

	void render()
	{
		point[0]->render();
		point[1]->render();
		line->render();
	}

private:
	GPointDecoration*	point[2];
	GLineDecoration*	line;
};
class CPointDistanceTool : public CBasicTool
{
	class Props : public CPropertyList
	{
	public:
		Props(CPointDistanceTool* ptool) : tool(ptool)
		{
			addProperty("node 1", CProperty::Int);
			addProperty("node 2", CProperty::Int);
			addProperty("Dx"    , CProperty::Float)->setFlags(CProperty::Visible);
			addProperty("Dy"    , CProperty::Float)->setFlags(CProperty::Visible);
			addProperty("Dz"    , CProperty::Float)->setFlags(CProperty::Visible);
			addProperty("Length", CProperty::Float)->setFlags(CProperty::Visible);
		}

		QVariant GetPropertyValue(int i)
		{
			switch (i)
			{
			case 0: return tool->m_node1; break;
			case 1: return tool->m_node2; break;
			case 2: return fabs(tool->m_d.x); break;
			case 3: return fabs(tool->m_d.y); break;
			case 4: return fabs(tool->m_d.z); break;
			case 5: return tool->m_d.Length(); break;
			}
			return QVariant();
		}

		void SetPropertyValue(int i, const QVariant& v)
		{
			if (i==0) tool->m_node1 = v.toInt();
			if (i==1) tool->m_node2 = v.toInt();
			tool->updateLength();
		}

	private:
		CPointDistanceTool*	tool;
	};

public:
	CPointDistanceTool() : CBasicTool("Pt.Distance") { m_node1 = 0; m_node2 = 0; m_d = vec3f(0,0,0); m_doc = 0; m_deco = 0; }

	CPropertyList* getPropertyList() { return new Props(this); }

	void activate(CDocument* pdoc)
	{
		m_doc = pdoc;
		if (pdoc && pdoc->IsValid())
		{
			updateLength();
			if (m_deco)
			{
				m_doc->GetGLModel()->RemoveDecoration(m_deco);
				delete m_deco;
				m_deco = 0;
			}
			m_deco = new CPointDistanceDecoration;
			m_doc->GetGLModel()->AddDecoration(m_deco);
		}
	}

	void deactivate()
	{
		if (m_deco)
		{
			m_doc->GetGLModel()->RemoveDecoration(m_deco);
			delete m_deco;
			m_deco = 0;
		}
	}

	void updateLength()
	{
		m_d = vec3f(0.f,0.f,0.f);
		if (m_deco) m_deco->setVisible(false);
		if (m_doc && m_doc->IsValid())
		{
			FEModel& fem = *m_doc->GetFEModel();
			FEMesh& mesh = *fem.GetMesh();
			int ntime = m_doc->GetCurrentTime();
			int NN = mesh.Nodes();
			if ((m_node1 > 0)&&(m_node2 > 0)&&(m_node1 <= NN)&&(m_node2 <= NN))
			{
				vec3f a = fem.NodePosition(m_node1-1, ntime);
				vec3f b = fem.NodePosition(m_node2-1, ntime);
				m_d = b - a;

				if (m_deco) 
				{
					m_deco->setPosition(a, b);
					m_deco->setVisible(true);
				}
			}
		}
		updateUi();
	}

private:
	int			m_node1, m_node2;	// mesh nodes
	vec3f		m_d;				// displacement vector
	CDocument*	m_doc;
	CPointDistanceDecoration*	m_deco;

	friend class Props;
};

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
/*	tools.push_back(new CTool("3Point Angle"));
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
*/
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
