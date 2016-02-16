#include "DataPanel.h"
#include <QBoxLayout>
#include <QTableView>
#include <QLabel>
#include <QHeaderView>
#include "MainWindow.h"
#include "Document.h"
#include <PostViewLib/FEModel.h>
#include "GLModel.h"
#include <QtCore/QAbstractTableModel>

class CDataModel : public QAbstractTableModel
{
public:
	CDataModel(QWidget* pw) : QAbstractTableModel(pw), m_fem(0) {}

	void SetFEModel(FEModel* pfem)
	{
		beginResetModel();
		m_fem = pfem;
		endResetModel();
	}

	int rowCount(const QModelIndex& index) const 
	{
		if (m_fem == 0) return 0;
		FEDataManager& dm = *m_fem->GetDataManager();
		return (dm.NodeFields() + dm.FaceFields() + dm.ElementFields());
	}

	int columnCount(const QModelIndex& index) const { return 3; }

	QVariant headerData(int section, Qt::Orientation orient, int role) const
	{
		if ((orient == Qt::Horizontal)&&(role == Qt::DisplayRole))
		{
			switch (section)
			{
			case 0: return QVariant(QString("Data field")); break;
			case 1: return QVariant(QString("Type")); break;
			case 2: return QVariant(QString("Class")); break;
			}
		}
		return QAbstractTableModel::headerData(section, orient, role);
	}

	QVariant data(const QModelIndex& index, int role) const
	{
		const char* szclass[] = {"NODE", "FACE", "ELEM"};
		if (m_fem == 0) return QVariant();

		if (!index.isValid()) return QVariant();
		if (role == Qt::DisplayRole)
		{
			int nrow = index.row();
			int ncol = index.column();
			FEDataManager& dm = *m_fem->GetDataManager();
			FEDataField* pd = 0;
			if (nrow < dm.NodeFields()) pd = *dm.NodeData(nrow);
			else
			{
				nrow -= dm.NodeFields();
				if (nrow < dm.FaceFields()) pd = *dm.FaceData(nrow);
				else
				{
					nrow -= dm.FaceFields();
					if (nrow < dm.ElementFields()) pd = *dm.ElementData(nrow);
				}
			}

			if (pd)
			{
				if (ncol == 0) return QString(pd->GetName());
				else if (ncol==1) return QString(pd->TypeStr());
				else if (ncol==2) return QString(szclass[pd->DataClass()]);
			}
		}
		return QVariant();
	}


private:
	FEModel*	m_fem;
};

class Ui::CDataPanel
{
public:
	CDataModel*	data;
	QTableView*	list;

public:
	void setupUi(::CDataPanel* parent)
	{
		QVBoxLayout* pg = new QVBoxLayout(parent);
		list = new QTableView;
		list->setObjectName(QStringLiteral("dataList"));
		list->setSelectionBehavior(QAbstractItemView::SelectRows);
		list->setSelectionMode(QAbstractItemView::SingleSelection);
		list->horizontalHeader()->setStretchLastSection(true);
		list->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
		list->verticalHeader()->setDefaultSectionSize(24);
		list->verticalHeader()->hide();

		data = new CDataModel(list);
		list->setModel(data);

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
	CDocument* pdoc = m_wnd->GetDocument();
	ui->data->SetFEModel(pdoc->GetFEModel());
}
