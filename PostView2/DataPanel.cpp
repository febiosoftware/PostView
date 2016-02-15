#include "DataPanel.h"
#include <QBoxLayout>
#include <QTableWidget>
#include <QLabel>
#include <QHeaderView>
#include "MainWindow.h"
#include "Document.h"
#include <PostViewLib/FEModel.h>
#include "GLModel.h"

class Ui::CDataPanel
{
public:
	QTableWidget*	list;

public:
	void setupUi(::CDataPanel* parent)
	{
		QVBoxLayout* pg = new QVBoxLayout(parent);
		list = new QTableWidget;
		list->setObjectName(QStringLiteral("dataList"));
		pg->addWidget(list);

		QMetaObject::connectSlotsByName(parent);
	}
};

CDataPanel::CDataPanel(CMainWindow* pwnd, QWidget* parent) : CCommandPanel(pwnd, parent), ui(new Ui::CDataPanel)
{
	ui->setupUi(this);
}

void CDataPanel::Update()
{
	const char* szclass[] = {"NODE", "FACE", "ELEM"};

	ui->list->clear();
	ui->list->setColumnCount(3);
	ui->list->setRowCount(0);
	ui->list->setHorizontalHeaderLabels(QStringList() << "Field" << "Type" << "Class");
	ui->list->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->list->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->list->horizontalHeader()->setStretchLastSection(true);
	ui->list->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	ui->list->verticalHeader()->setDefaultSectionSize(24);
	ui->list->verticalHeader()->hide();

	CDocument* pdoc = m_wnd->GetDocument();
	if (pdoc && pdoc->IsValid())
	{
		FEModel& fem = *pdoc->GetFEModel();
		FEDataManager& data = *fem.GetDataManager();

		QTableWidgetItem* pi;

		int NN = data.NodeFields();
		int NF = data.FaceFields();
		int NE = data.ElementFields();
		ui->list->setRowCount(NN+NF+NE);

		// Node fields
		int n = 0;
		FEDataFieldPtr it = data.FirstNode();
		for (int i=0; i<NN; ++i, ++it, ++n)
		{
			FEDataField& d = *(*it);

			// set the name
			pi = new QTableWidgetItem;
			pi->setText(d.GetName());
			pi->setFlags(pi->flags() & ~Qt::ItemIsEditable);
			ui->list->setItem(n, 0, pi);

			// set the type
			pi = new QTableWidgetItem;
			pi->setText(d.TypeStr());
			pi->setFlags(pi->flags() & ~Qt::ItemIsEditable);
			ui->list->setItem(n, 1, pi);

			// set the class
			pi = new QTableWidgetItem;
			pi->setText(szclass[d.DataClass()]);
			pi->setFlags(pi->flags() & ~Qt::ItemIsEditable);
			ui->list->setItem(n, 2, pi);
		}

		// Face fields
		it = data.FirstFace();
		for (int i=0; i<NF; ++i, ++it, ++n)
		{
			FEDataField& d = *(*it);

			// set the name
			pi = new QTableWidgetItem;
			pi->setText(d.GetName());
			pi->setFlags(pi->flags() & ~Qt::ItemIsEditable);
			ui->list->setItem(n, 0, pi);

			// set the type
			pi = new QTableWidgetItem;
			pi->setText(d.TypeStr());
			pi->setFlags(pi->flags() & ~Qt::ItemIsEditable);
			ui->list->setItem(n, 1, pi);

			// set the class
			pi = new QTableWidgetItem;
			pi->setText(szclass[d.DataClass()]);
			pi->setFlags(pi->flags() & ~Qt::ItemIsEditable);
			ui->list->setItem(n, 2, pi);
		}

		// Element fields
		it = data.FirstElement();
		for (int i=0; i<NE; ++i, ++it, ++n)
		{
			FEDataField& d = *(*it);

			// set the name
			pi = new QTableWidgetItem;
			pi->setText(d.GetName());
			pi->setFlags(pi->flags() & ~Qt::ItemIsEditable);
			ui->list->setItem(n, 0, pi);

			// set the type
			pi = new QTableWidgetItem;
			pi->setText(d.TypeStr());
			pi->setFlags(pi->flags() & ~Qt::ItemIsEditable);
			ui->list->setItem(n, 1, pi);

			// set the class
			pi = new QTableWidgetItem;
			pi->setText(szclass[d.DataClass()]);
			pi->setFlags(pi->flags() & ~Qt::ItemIsEditable);
			ui->list->setItem(n, 2, pi);
		}
	}
}
