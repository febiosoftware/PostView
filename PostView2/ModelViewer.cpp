#include "ModelViewer.h"
#include <QBoxLayout>
#include <QTreeWidget>
#include <QTableWidget>
#include <QMessageBox>
#include <QHeaderView>
#include <QSplitter>
#include <QLabel>
#include "MainWindow.h"
#include "Document.h"
#include "PropertyListView.h"
#include "GLModel.h"
#include <PostViewLib/FEModel.h>

//-----------------------------------------------------------------------------
class CModelProps : public CPropertyList
{
public:
	CModelProps(CGLModel* fem) : m_fem(fem) 
	{
		AddProperty(CProperty("Element subdivions", "2"));
		AddProperty(CProperty("Render mode", "Solid"));
		AddProperty(CProperty("Render outline", "No"));
		AddProperty(CProperty("Render undeformed outline", "No"));
	}

	QVariant GetPropertyValue(int i)
	{
		QVariant v;
		switch (i)
		{
		case 0: v = m_fem->m_nDivs; break;
		case 1: v = m_fem->m_nrender; break;
		case 2: v = m_fem->m_boutline; break;
		case 3: v = m_fem->m_bghost; break;
		}
		return v;
	}

	void SetPropertyValue(int i, const QVariant& v)
	{
		switch (i)
		{
		case 0: m_fem->m_nDivs = v.value<int>(); break;
		case 1: m_fem->m_nrender = v.value<int>(); break;
		case 2: m_fem->m_boutline = v.value<bool>(); break;
		case 3: m_fem->m_bghost = v.value<bool>(); break;
		}
	}
	
private:
	CGLModel*	m_fem;
};

//-----------------------------------------------------------------------------
class CMeshProps : public CPropertyList
{
public:
	CMeshProps(FEModel* fem) : m_fem(fem) 
	{
		FEMesh& mesh = *fem->GetMesh();
		AddProperty(CProperty("Nodes", QString("%1").arg(mesh.Nodes()), "Number of nodes"));
		AddProperty(CProperty("Faces", QString("%1").arg(mesh.Faces()), "Number of faces"));
		AddProperty(CProperty("Solid Elements", QString("%1").arg(mesh.SolidElements()), "Number of solid elements"));
		AddProperty(CProperty("Shell Elements", QString("%1").arg(mesh.ShellElements()), "Number of shell elemetns"));
	}

	QVariant GetPropertyValue(int i)
	{
		QVariant v;
		return v;
	}

	void SetPropertyValue(int i, const QVariant& v)
	{
	}

private:
	FEModel*	m_fem;
};

//-----------------------------------------------------------------------------
class Ui::CModelViewer
{
public:
	QTreeWidget*		m_tree;
	CPropertyListView*	m_props;

public:
	void setupUi(::CModelViewer* parent)
	{
		QVBoxLayout* pg = new QVBoxLayout(parent);
		
		QSplitter* psplitter = new QSplitter;
		psplitter->setOrientation(Qt::Vertical);
		pg->addWidget(psplitter);

		m_tree = new QTreeWidget;
		m_tree->setObjectName(QStringLiteral("modelTree"));
		m_tree->setColumnCount(1);
		m_tree->setHeaderHidden(true);

		m_props = new CPropertyListView;

		psplitter->addWidget(m_tree);
		psplitter->addWidget(m_props);

		QMetaObject::connectSlotsByName(parent);
	}
};

CModelViewer::CModelViewer(CMainWindow* pwnd, QWidget* parent) : CCommandPanel(pwnd, parent), ui(new Ui::CModelViewer)
{
	ui->setupUi(this);
}

void CModelViewer::Update()
{
	CDocument* pdoc = m_wnd->GetDocument();
	if (pdoc->IsValid())
	{
		FEModel* fem = pdoc->GetFEModel();
		CGLModel* mdl = pdoc->GetGLModel();

		ui->m_tree->clear();
		QTreeWidgetItem* pi1 = new QTreeWidgetItem(ui->m_tree);
		pi1->setText(0, "Model");
		pi1->setData(0, Qt::UserRole, qVariantFromValue((void*) new CModelProps(mdl)));

		// add the mesh
		QTreeWidgetItem* pi2 = new QTreeWidgetItem(pi1);
		pi2->setText(0, "Mesh");
		pi2->setData(0, Qt::UserRole, qVariantFromValue((void*) new CMeshProps(fem)));

		pi2 = new QTreeWidgetItem(pi1);
		pi2->setText(0, "Displacement map");
		pi2->setData(0, Qt::UserRole, QVariant(QVariant::UserType, 0));

		pi2 = new QTreeWidgetItem(pi1);
		pi2->setText(0, "Color map");
		pi2->setData(0, Qt::UserRole, QVariant(QVariant::UserType, 0));
	}
}

void CModelViewer::on_modelTree_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* prev)
{
	QVariant v = current->data(0, Qt::UserRole);
	CPropertyList* pdata = (CPropertyList*) v.value<void*>();

	// clear the property list
	ui->m_props->Clear();

	if (pdata) ui->m_props->Update(pdata);
}
