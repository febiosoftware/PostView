#include "ModelViewer.h"
#include <QBoxLayout>
#include <QTreeWidget>
#include <QTableWidget>
#include <QMessageBox>
#include <QHeaderView>
#include "MainWindow.h"
#include "Document.h"
#include "GLModel.h"
#include <PostViewLib/FEModel.h>

//-----------------------------------------------------------------------------
class CModelProps : public CPropertyList
{
public:
	CModelProps(FEModel* fem) : m_fem(fem) 
	{
		AddProperty(CProperty("Element subdivions", "2"));
		AddProperty(CProperty("Render mode", "Solid"));
		AddProperty(CProperty("Render outline", "No"));
		AddProperty(CProperty("Render undeformed outline", "No"));
	}

private:
	FEModel*	m_fem;
};

//-----------------------------------------------------------------------------
class Ui::CModelViewer
{
public:
	QTreeWidget*	m_tree;
	QTableWidget*	m_props;

public:
	void setupUi(::CModelViewer* parent)
	{
		QVBoxLayout* pg = new QVBoxLayout(parent);
		
		m_tree = new QTreeWidget(parent);
		m_tree->setObjectName(QStringLiteral("modelTree"));
		m_tree->setColumnCount(1);
		m_tree->setHeaderHidden(true);

		m_props = new QTableWidget(parent);
		m_props->setObjectName(QStringLiteral("modelProps"));

		pg->addWidget(m_tree);
		pg->addWidget(m_props, 2);

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
		pi1->setData(0, Qt::UserRole, qVariantFromValue((void*) new CModelProps(fem)));

		// add the mesh
		QTreeWidgetItem* pi2 = new QTreeWidgetItem(pi1);
		pi2->setText(0, "Mesh");
		pi2->setData(0, Qt::UserRole, qVariantFromValue((void*) new CModelProps(fem)));

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
	ui->m_props->clear();
	ui->m_props->setColumnCount(1);
	ui->m_props->horizontalHeader()->setStretchLastSection(true);
	ui->m_props->horizontalHeader()->hide();
	ui->m_props->verticalHeader()->setMinimumWidth(width()/2);

	if (pdata)
	{
		int n = pdata->Properties();
		if (n > 0)
		{
			ui->m_props->setRowCount(n);
			for (int i=0; i<n; ++i)
			{
				const CPropertyList::CProperty& p = pdata->Property(i);

				QTableWidgetItem* pitem = new QTableWidgetItem;
				pitem->setText(p.m_name);
				ui->m_props->setVerticalHeaderItem(i, pitem);

				pitem = new QTableWidgetItem;
				pitem->setText(p.m_val);
				ui->m_props->setItem(i, 0, pitem);
			}
		}
	}
}
