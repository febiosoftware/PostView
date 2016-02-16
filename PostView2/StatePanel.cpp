#include "StatePanel.h"
#include "MainWindow.h"
#include "Document.h"
#include <PostViewLib/FEModel.h>
#include <QBoxLayout>
#include <QTableWidget>
#include <QLabel>
#include <QHeaderView>
#include <QtCore/QAbstractTableModel>

class CStateModel : public QAbstractTableModel
{
public:
	CStateModel(QWidget* parent) : QAbstractTableModel(parent), m_fem(0)
	{
	}

	int rowCount(const QModelIndex& parent) const
	{
		if (m_fem) return m_fem->GetStates();
		return 0;
	}

	int columnCount(const QModelIndex& parent) const
	{
		return 2;
	}

	void SetFEModel(FEModel* pfem)
	{
		beginResetModel();
		m_fem = pfem;
		endResetModel();
	}

	QVariant headerData(int section, Qt::Orientation orient, int role) const
	{
		if ((orient == Qt::Horizontal)&&(role == Qt::DisplayRole))
		{
			switch (section)
			{
			case 0: return QVariant(QString("State")); break;
			case 1: return QVariant(QString("Time")); break;
			}
		}
		return QAbstractTableModel::headerData(section, orient, role);
	}

	QVariant data(const QModelIndex& index, int role) const
	{
		if (m_fem == 0) return QVariant();

		if (!index.isValid()) return QVariant();
		if (role == Qt::DisplayRole)
		{
			if (index.column() == 0) return index.row()+1;
			else 
			{
				FEState* s = m_fem->GetState(index.row());
				if (s) return s->m_time;
			}
		}
		return QVariant();
	}

private:
	FEModel*	m_fem;
};

class Ui::CStatePanel
{
public:
	CStateModel*	data;
	QTableView*		list;

public:
	void setupUi(::CStatePanel* parent)
	{
		QVBoxLayout* pg = new QVBoxLayout(parent);
		list = new QTableView;
		list->setObjectName(QStringLiteral("stateList"));
		list->setSelectionBehavior(QAbstractItemView::SelectRows);
		list->setSelectionMode(QAbstractItemView::SingleSelection);
		list->horizontalHeader()->show();
		list->horizontalHeader()->setStretchLastSection(true);
		list->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
		list->verticalHeader()->setDefaultSectionSize(24);
		list->verticalHeader()->hide();

		data = new CStateModel(list);
		list->setModel(data);

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
	FEModel* fem = pdoc->GetFEModel();

	ui->data->SetFEModel(fem);
}

void CStatePanel::on_stateList_doubleClicked(const QModelIndex& index)
{
	CDocument* pdoc = m_wnd->GetDocument();
	pdoc->SetCurrentTime(index.row());
	m_wnd->repaint();
}
