#include "MaterialPanel.h"
#include <QBoxLayout>
#include <QTreeWidget>
#include <QListWidget>
#include <QSplitter>
#include <QLabel>
#include "MainWindow.h"
#include "Document.h"
#include "PropertyListView.h"
#include <PostViewLib/FEModel.h>

class MaterialProps : public CPropertyList
{
public:
	MaterialProps()
	{
		m_mat = 0;
		addProperty("Render mode"      , CProperty::Enum, "Render mode")->setEnumValues(QStringList() << "default" << "wireframe" << "solid");
		addProperty("Diffuse"          , CProperty::Color );
		addProperty("Ambient"          , CProperty::Color );
		addProperty("Specular"         , CProperty::Color );
		addProperty("Emission"         , CProperty::Color );
		addProperty("Mesh color"       , CProperty::Color );
		addProperty("Shininess"        , CProperty::Float)->setFloatRange(0.0, 1.0);
		addProperty("Transparency"     , CProperty::Float)->setFloatRange(0.0, 1.0);
		addProperty("Transparency mode", CProperty::Enum, "Transparency mode")->setEnumValues(QStringList() << "constant" << "normal-weighed");
		addProperty("Show Mesh"        , CProperty::Bool);
		addProperty("Cast shadows"     , CProperty::Bool);
		addProperty("Clip"             , CProperty::Bool);
	}

	void SetMaterial(FEMaterial* pmat) { m_mat = pmat; }

	QVariant GetPropertyValue(int i)
	{
		QVariant v;
		if (m_mat)
		{
			switch (i)
			{
			case 0: v = m_mat->m_nrender; break;
			case 1: v = toQColor(m_mat->diffuse); break;
			case 2: v = toQColor(m_mat->ambient); break;
			case 3: v = toQColor(m_mat->specular); break;
			case 4: v = toQColor(m_mat->emission); break;
			case 5: v = toQColor(m_mat->meshcol); break;
			case 6: v = m_mat->shininess; break;
			case 7: v = m_mat->transparency; break;
			case 8: v = m_mat->m_ntransmode; break;
			case 9: v = m_mat->bmesh; break;
			case 10: v = m_mat->bcast_shadows; break;
			case 11: v = m_mat->bclip; break;
			}
		}
		return v;
	}

	void SetPropertyValue(int i, const QVariant& v)
	{
		if (m_mat)
		{
			switch (i)
			{
			case 0: m_mat->m_nrender = v.toInt(); break;
			case 1: m_mat->diffuse  = toGLColor(v.value<QColor>()); break;
			case 2: m_mat->ambient  = toGLColor(v.value<QColor>()); break;
			case 3: m_mat->specular = toGLColor(v.value<QColor>()); break;
			case 4: m_mat->emission = toGLColor(v.value<QColor>()); break;
			case 5: m_mat->meshcol  = toGLColor(v.value<QColor>()); break;
			case 6: m_mat->shininess = v.toFloat(); break;
			case 7: m_mat->transparency = v.toFloat(); break;
			case 8: m_mat->m_ntransmode = v.toInt(); break;
			case 9: m_mat->bmesh = v.toBool(); break;
			case 10: m_mat->bcast_shadows = v.toBool(); break;
			case 11: m_mat->bclip = v.toBool(); break;
			}
		}
	}

private:
	FEMaterial*	m_mat;
};

class Ui::CMaterialPanel
{
public:
	QListWidget*		m_list;
	::CPropertyListView*	m_prop;
	QLineEdit* name;

public:
	void setupUi(::CMaterialPanel* parent)
	{
		QVBoxLayout* pg = new QVBoxLayout(parent);

		QSplitter* psplitter = new QSplitter;
		psplitter->setOrientation(Qt::Vertical);
		pg->addWidget(psplitter);

		m_list = new QListWidget;
		m_list->setObjectName(QStringLiteral("materialList"));
		m_list->setSelectionMode(QAbstractItemView::ExtendedSelection);

		QWidget* w = new QWidget;
		QVBoxLayout* pvl = new QVBoxLayout;
		pvl->setMargin(0);
		w->setLayout(pvl);

		QPushButton* phide = new QPushButton("Hide"); phide->setObjectName("hideButton"); phide->setFixedWidth(50);
		QPushButton* pshow = new QPushButton("Show"); pshow->setObjectName("showButton"); pshow->setFixedWidth(50);
		QPushButton* pactiv= new QPushButton("Enable" ); pactiv->setObjectName("enableButton" ); pactiv->setFixedWidth(50);
		QPushButton* pdeact= new QPushButton("Disable"); pdeact->setObjectName("disableButton"); pdeact->setFixedWidth(50);
		QHBoxLayout* ph = new QHBoxLayout;
		ph->setSpacing(0);
		QLabel* label = new QLabel("Name:");
		ph->addWidget(label);
		ph->addWidget(name = new QLineEdit); label->setBuddy(name); name->setObjectName("editName");
		ph->addWidget(phide);
		ph->addWidget(pshow);
		ph->addWidget(pactiv);
		ph->addWidget(pdeact);
		ph->addStretch();
		pvl->addLayout(ph);

		m_prop = new ::CPropertyListView;
		pvl->addWidget(m_prop);

		psplitter->addWidget(m_list);
		psplitter->addWidget(w);

		QMetaObject::connectSlotsByName(parent);
	}
};

CMaterialPanel::CMaterialPanel(CMainWindow* pwnd, QWidget* parent) : CCommandPanel(pwnd, parent), ui(new Ui::CMaterialPanel)
{
	ui->setupUi(this);
	m_pmat = new MaterialProps;
}

void CMaterialPanel::Update(bool breset)
{
	if (breset == false) return;

	ui->m_list->clear();
	ui->m_prop->Update(0);
	m_pmat->SetMaterial(0);

	CDocument* pdoc = m_wnd->GetDocument();
	FEModel* fem = pdoc->GetFEModel();

	if (fem)
	{
		int nmat = fem->Materials();
		for (int i=0; i<nmat; ++i)
		{
			FEMaterial& mat = *fem->GetMaterial(i);
			ui->m_list->addItem(mat.GetName());
		}

		UpdateStates();
	}
}

void CMaterialPanel::UpdateStates()
{
	CDocument* pdoc = m_wnd->GetDocument();
	FEModel* fem = pdoc->GetFEModel();
	if (fem == 0) return;

	int nmat = fem->Materials();
	for (int i=0; i<nmat; ++i)
	{
		FEMaterial& mat = *fem->GetMaterial(i);
		QListWidgetItem* pi = ui->m_list->item(i);
		QFont font = pi->font();
		if (mat.visible())
		{
			pi->setForeground(Qt::black);
			font.setBold(true);
			pi->setFont(font);
		}
		else 
		{
			pi->setForeground(Qt::gray);
			font.setBold(false);
			pi->setFont(font);
		}

		pi->setBackgroundColor((mat.enabled() ? Qt::white : Qt::yellow));
	}
}

void CMaterialPanel::on_materialList_currentRowChanged(int nrow)
{
	FEModel& fem = *m_wnd->GetDocument()->GetFEModel();
	if ((nrow >= 0) && (nrow < fem.Materials()))
	{
		FEMaterial* pmat = fem.GetMaterial(nrow);
		m_pmat->SetMaterial(pmat);
		ui->m_prop->Update(m_pmat);
		ui->name->setText(QString(pmat->GetName()));
	}
}

void CMaterialPanel::on_hideButton_clicked()
{
	CDocument& doc = *m_wnd->GetDocument();
	if (doc.IsValid() == false) return;

	FEModel& fem = *doc.GetFEModel();

	QItemSelectionModel* pselect = ui->m_list->selectionModel();
	QModelIndexList selection = pselect->selectedRows();
	int ncount = selection.count();
	for (int i=0; i<ncount; ++i)
	{
		QModelIndex index = selection.at(i);
		int nmat = index.row();

		FEMaterial& mat = *fem.GetMaterial(nmat);
		mat.hide();

	}

	UpdateStates();
	m_wnd->repaint();
}

void CMaterialPanel::on_showButton_clicked()
{
	CDocument& doc = *m_wnd->GetDocument();
	if (doc.IsValid() == false) return;

	FEModel& fem = *doc.GetFEModel();

	QItemSelectionModel* pselect = ui->m_list->selectionModel();
	QModelIndexList selection = pselect->selectedRows();
	int ncount = selection.count();
	for (int i=0; i<ncount; ++i)
	{
		QModelIndex index = selection.at(i);
		int nmat = index.row();

		FEMaterial& mat = *fem.GetMaterial(nmat);
		mat.show();
	}

	UpdateStates();
	m_wnd->repaint();
}

void CMaterialPanel::on_enableButton_clicked()
{
	CDocument& doc = *m_wnd->GetDocument();
	if (doc.IsValid() == false) return;

	FEModel& fem = *doc.GetFEModel();
	FEMesh& mesh = *fem.GetMesh();

	QItemSelectionModel* pselect = ui->m_list->selectionModel();
	QModelIndexList selection = pselect->selectedRows();
	int ncount = selection.count();
	for (int i=0; i<ncount; ++i)
	{
		QModelIndex index = selection.at(i);
		int nmat = index.row();

		FEMaterial& mat = *fem.GetMaterial(nmat);
		mat.enable();

		mesh.EnableElements(nmat);
	}

	UpdateStates();
	m_wnd->repaint();
}

void CMaterialPanel::on_disableButton_clicked()
{
	CDocument& doc = *m_wnd->GetDocument();
	if (doc.IsValid() == false) return;

	FEModel& fem = *doc.GetFEModel();
	FEMesh& mesh = *fem.GetMesh();

	QItemSelectionModel* pselect = ui->m_list->selectionModel();
	QModelIndexList selection = pselect->selectedRows();
	int ncount = selection.count();
	for (int i=0; i<ncount; ++i)
	{
		QModelIndex index = selection.at(i);
		int nmat = index.row();

		FEMaterial& mat = *fem.GetMaterial(nmat);
		mat.disable();

		mesh.DisableElements(nmat);
	}

	UpdateStates();
	m_wnd->repaint();
}

void CMaterialPanel::on_editName_editingFinished()
{
	CDocument& doc = *m_wnd->GetDocument();
	QModelIndex n = ui->m_list->currentIndex();
	if (n.isValid())
	{
		int nmat = n.row();

		FEModel& fem = *doc.GetFEModel();
		FEMaterial& mat = *fem.GetMaterial(nmat);

		QListWidgetItem* item = ui->m_list->item(nmat);
		string name = ui->name->text().toStdString();
		mat.SetName(name.c_str());
		item->setText(ui->name->text());
	}
}
