#include "StatePanel.h"
#include "MainWindow.h"
#include "Document.h"
#include <PostViewLib/FEModel.h>
#include <QBoxLayout>
#include <QTableWidget>
#include <QLabel>
#include <QHeaderView>

class Ui::CStatePanel
{
public:
	QTableWidget* list;

public:
	void setupUi(::CStatePanel* parent)
	{
		QVBoxLayout* pg = new QVBoxLayout(parent);
		list = new QTableWidget;
		list->setObjectName(QStringLiteral("stateList"));
		pg->addWidget(list);

		QMetaObject::connectSlotsByName(parent);
	}
};


CStatePanel::CStatePanel(CMainWindow* pwnd, QWidget* parent) : CCommandPanel(pwnd, parent), ui(new Ui::CStatePanel)
{
	ui->setupUi(this);
}

void CStatePanel::Update()
{
	CDocument* pdoc = m_wnd->GetDocument();
	FEModel& fem = *pdoc->GetFEModel();

	ui->list->clear();
	ui->list->setColumnCount(2);
	int nstates = fem.GetStates();
	ui->list->setRowCount(nstates);
	ui->list->setHorizontalHeaderLabels(QStringList() << "State" << "Time");
	ui->list->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->list->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->list->horizontalHeader()->setStretchLastSection(true);
	ui->list->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	ui->list->verticalHeader()->setDefaultSectionSize(24);
	ui->list->verticalHeader()->hide();
	for (int i=0; i<nstates; ++i)
	{
		FEState& state = *fem.GetState(i);

		QTableWidgetItem* pi = new QTableWidgetItem;
		pi->setText(QString("%1").arg(i+1));
		pi->setFlags(pi->flags() & ~Qt::ItemIsEditable);
		ui->list->setItem(i, 0, pi);

		pi = new QTableWidgetItem;
		pi->setFlags(pi->flags() & ~Qt::ItemIsEditable);
		pi->setText(QString("%1").arg(state.m_time));
		ui->list->setItem(i, 1, pi);
	}	
}

void CStatePanel::on_stateList_cellDoubleClicked(int row, int column)
{
	CDocument* pdoc = m_wnd->GetDocument();
	pdoc->SetCurrentTime(row);
	m_wnd->repaint();
}
