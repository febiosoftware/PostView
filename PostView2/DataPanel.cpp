#include "DataPanel.h"
#include <QBoxLayout>
#include <QTableView>
#include <QLabel>
#include <QHeaderView>
#include <QPushButton>
#include <QInputDialog>
#include <QMessageBox>
#include <QToolButton>
#include <QFileDialog>
#include <QFormLayout>
#include <QAction>
#include <QComboBox>
#include <QDialogButtonBox>
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
		return dm.DataFields();
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
			FEDataField* pd = *dm.DataField(nrow);

			if      (ncol == 0) return QString(pd->GetName());
			else if (ncol == 1) return QString(pd->TypeStr());
			else if (ncol == 2) 
			{
				switch (pd->DataClass())
				{
				case CLASS_NODE: return QString("NODE"); break;
				case CLASS_FACE: return QString("FACE"); break;
				case CLASS_ELEM: return QString("ELEM"); break;
				default:
					assert(false);
					return QString("(unknown");
				}
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

		const int BW = 60;
		const int BH = 22;

		QAction* addActionStd  = new QAction("Standard ...", parent); addActionStd ->setObjectName("AddStandard");
		QAction* addActionFile = new QAction("From file ...", parent); addActionFile->setObjectName("AddFromFile");

		QToolButton* pbAdd = new QToolButton(); pbAdd->setFixedSize(BW, BH);
		pbAdd->setText("Add...");
		pbAdd->setPopupMode(QToolButton::InstantPopup);
		pbAdd->addAction(addActionStd);
		pbAdd->addAction(addActionFile);

		QPushButton* pbCpy = new QPushButton("Copy"     ); pbCpy->setFixedSize(BW, BH); pbCpy->setObjectName("CopyButton"  );
		QPushButton* pbDel = new QPushButton("Delete"   ); pbDel->setFixedSize(BW, BH); pbDel->setObjectName("DeleteButton");
		QPushButton* pbFlt = new QPushButton("Filter..."); pbFlt->setFixedSize(BW, BH); pbFlt->setObjectName("FilterButton");
		QPushButton* pbExp = new QPushButton("Export..."); pbExp->setFixedSize(BW, BH); pbExp->setObjectName("ExportButton");

		QHBoxLayout* ph = new QHBoxLayout;
		ph->setSpacing(0);
		ph->addWidget(pbAdd);
		ph->addWidget(pbCpy);
		ph->addWidget(pbDel);
		ph->addWidget(pbFlt);
		ph->addWidget(pbExp);
		ph->addStretch();

		pg->addLayout(ph);

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

class Ui::CDlgAddDataFile
{
public:
	QLineEdit*	pfile;
	QLineEdit*	pname;
	QComboBox*	pclass;
	QComboBox*	ptype;

public:
	void setupUi(QDialog* parent)
	{
		QPushButton* buttonBrowse = new QPushButton("Browse...");
		buttonBrowse->setFixedWidth(75);
		QHBoxLayout* phb = new QHBoxLayout;
		phb->addWidget(pfile = new QLineEdit);
		phb->addWidget(buttonBrowse);

		QVBoxLayout* pv = new QVBoxLayout;
		QFormLayout* pl = new QFormLayout;
		pl->addRow("File:" , phb);
		pl->addRow("Name:" , pname = new QLineEdit);
		pname->setText(QString("DataField"));

		pclass = new QComboBox;
		pclass->addItem("Node");
		pclass->addItem("Elem");

		ptype = new QComboBox;
		ptype->addItem("float (1 fl)");
		ptype->addItem("vec3f (3 fl)");
		ptype->addItem("mat3fs (6 fl)");

		pl->addRow("Class:", pclass);
		pl->addRow("Type:" , ptype);

		pv->addLayout(pl);

		QDialogButtonBox* b = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
		pv->addWidget(b);

		parent->setLayout(pv);

		QObject::connect(b, SIGNAL(accepted()), parent, SLOT(accept()));
		QObject::connect(b, SIGNAL(rejected()), parent, SLOT(reject()));
		QObject::connect(buttonBrowse, SIGNAL(clicked()), parent, SLOT(onBrowse()));
	}
};

CDlgAddDataFile::CDlgAddDataFile(QWidget* parent) : QDialog(parent), ui(new Ui::CDlgAddDataFile)
{
	ui->setupUi(this);
}

void CDlgAddDataFile::accept()
{
	m_file = ui->pfile->text().toStdString();
	m_name = ui->pname->text().toStdString();
	m_nclass = ui->pclass->currentIndex();
	m_ntype = ui->ptype->currentIndex();

	QDialog::accept();
}

void CDlgAddDataFile::onBrowse()
{
	QString file = QFileDialog::getOpenFileName(this, "Open File");
	if (file.isEmpty() == false)
	{
		ui->pfile->setText(file);
	}
}

//=================================================================================================
CDataPanel::CDataPanel(CMainWindow* pwnd, QWidget* parent) : CCommandPanel(pwnd, parent), ui(new Ui::CDataPanel)
{
	ui->setupUi(this);
}

void CDataPanel::Update()
{
	CDocument* pdoc = m_wnd->GetDocument();
	ui->data->SetFEModel(pdoc->GetFEModel());
}

void CDataPanel::on_AddStandard_triggered()
{
	QStringList items;
	items.push_back("Position"                  );
	items.push_back("Initial Position"          );
	items.push_back("Deformation gradient"      );
	items.push_back("Infinitesimal strain"      );
	items.push_back("Lagrange strain"           );
	items.push_back("Right Cauchy-Green"        );
	items.push_back("Right stretch"             );
	items.push_back("GL strain"                 );
	items.push_back("Biot strain"               );
	items.push_back("Right Hencky"              );   
	items.push_back("Left Cauchy-Green"         );
	items.push_back("Left stretch"              );
	items.push_back("Left Hencky"               );
	items.push_back("Almansi strain"            );
	items.push_back("Volume"                    );
	items.push_back("Volume ratio"              );
	items.push_back("Volume strain"             );
	items.push_back("Aspect ratio"              );
	items.push_back("1-Princ Curvature"         );
	items.push_back("2-Princ Curvature"         );
	items.push_back("Gaussian Curvature"        );
	items.push_back("Mean Curvature"            );
	items.push_back("RMS Curvature"             );
	items.push_back("Princ Curvature difference");
	items.push_back("Congruency"                );
	items.push_back("1-Princ Curvature vector"  );
	items.push_back("2-Princ Curvature vector"  );


	bool ok = false;
	QString item = QInputDialog::getItem(this, "Select new data field", "data:", items, 0, false, &ok);
	if (ok)
	{
		int nitem = items.indexOf(item);
		if (m_wnd->GetDocument()->AddStandardDataField(nitem, true) == false)
		{
			QMessageBox::critical(this, "Add Data Field", "Failed adding data");
		}

		// update the data list
		Update();
	}
}

void CDataPanel::on_AddFromFile_triggered()
{
	CDocument* doc = m_wnd->GetDocument();
	if (doc->IsValid() == false)
	{
		QMessageBox::critical(this, "PostView", "No model data loaded");
		return;
	}

	CDlgAddDataFile dlg(this);
	if (dlg.exec())
	{
		bool bret = false;
		switch (dlg.m_nclass)
		{
		case 0: bret = doc->AddNodeDataFromFile(dlg.m_file.c_str(), dlg.m_name.c_str(), dlg.m_ntype); break;
		case 1: bret = doc->AddElemDataFromFile(dlg.m_file.c_str(), dlg.m_name.c_str(), dlg.m_ntype); break;
		default:
			assert(false);
		}

		if (bret == false)
		{
			QMessageBox::critical(this, "Add Data From File", "Failed reading data from file.");
		}

		// update the data list
		Update();
	}
}

void CDataPanel::on_CopyButton_clicked()
{
	QItemSelectionModel* select = ui->list->selectionModel();
	QModelIndexList selRow = select->selectedRows();
	if (selRow.count() == 1)
	{
		int nsel = selRow.at(0).row();
		CDocument& doc = *m_wnd->GetDocument();
		FEModel& fem = *doc.GetFEModel();
		FEDataManager& dm = *fem.GetDataManager();
		FEDataField* pdf = *dm.DataField(nsel);
		if (pdf)
		{
			bool bret = false;
			QString text = QInputDialog::getText(this, "Copy Data Field", "Name:", QLineEdit::Normal, QString("%1_copy").arg(pdf->GetName()), &bret);
			if (bret)
			{
				std::string sname = text.toStdString();
				fem.CopyDataField(pdf, sname.c_str());
				Update();
			}
		}
	}
}

void CDataPanel::on_DeleteButton_clicked()
{
	QItemSelectionModel* select = ui->list->selectionModel();
	QModelIndexList selRow = select->selectedRows();
	if (selRow.count() == 1)
	{
		int nsel = selRow.at(0).row();
		CDocument& doc = *m_wnd->GetDocument();
		FEModel& fem = *doc.GetFEModel();
		FEDataManager& dm = *fem.GetDataManager();
		FEDataField* pdf = *dm.DataField(nsel);
		if (pdf)
		{
			QString sz(QString("Are you sure you want to delete the \"%1\" data field?").arg(pdf->GetName()));
			if (QMessageBox::question(this, "Delete Data Field", sz) == QMessageBox::Yes)
			{
				fem.DeleteDataField(pdf);
				Update();
			}
		}
	}
}

void CDataPanel::on_FilterButton_clicked()
{

}

void CDataPanel::on_ExportButton_clicked()
{
	QItemSelectionModel* select = ui->list->selectionModel();
	QModelIndexList selRow = select->selectedRows();
	if (selRow.count() == 1)
	{
		int nsel = selRow.at(0).row();
		CDocument& doc = *m_wnd->GetDocument();
		FEModel& fem = *doc.GetFEModel();
		FEDataManager& dm = *fem.GetDataManager();
		FEDataField* pdf = *dm.DataField(nsel);
		if (pdf)
		{
			QString file = QFileDialog::getSaveFileName(this, "Export Data");
			if (file.isEmpty() == false)
			{
				std::string sfile = file.toStdString();
				if (doc.ExportDataField(*pdf, sfile.c_str()) == false)
				{
					QMessageBox::critical(this, "Export Data", "Export Failed!");
				}
			}			
		}
	}
	else QMessageBox::warning(this, "Export Data", "Please select a data field first.");
}
