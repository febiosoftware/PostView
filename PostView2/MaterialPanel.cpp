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
		AddProperty("Render mode"      , QVariant::Int);
		AddProperty("Diffuse"          , QVariant::Color );
		AddProperty("Ambient"          , QVariant::Color );
		AddProperty("Specular"         , QVariant::Color );
		AddProperty("Emission"         , QVariant::Color );
		AddProperty("Mesh color"       , QVariant::Color );
		AddProperty("Shininess"        , QVariant::Double);
		AddProperty("Transparency"     , QVariant::Double);
		AddProperty("Transparency mode", QVariant::Int);
		AddProperty("Show Mesh"        , QVariant::Bool);
		AddProperty("Cast shadows"     , QVariant::Bool);
		AddProperty("Clip"             , QVariant::Bool);
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
			case 1: { GLCOLOR c = m_mat->diffuse ; v = QColor(c.r, c.g, c.b); } break;
			case 2: { GLCOLOR c = m_mat->ambient ; v = QColor(c.r, c.g, c.b); } break;
			case 3: { GLCOLOR c = m_mat->specular; v = QColor(c.r, c.g, c.b); } break;
			case 4: { GLCOLOR c = m_mat->emission; v = QColor(c.r, c.g, c.b); } break;
			case 5: { GLCOLOR c = m_mat->meshcol ; v = QColor(c.r, c.g, c.b); } break;
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
			case 1: { QColor c = v.value<QColor>(); GLCOLOR rgb(c.red(), c.green(), c.blue()); m_mat->diffuse = rgb; } break;
			case 2: { QColor c = v.value<QColor>(); GLCOLOR rgb(c.red(), c.green(), c.blue()); m_mat->ambient = rgb; } break;
			case 3: { QColor c = v.value<QColor>(); GLCOLOR rgb(c.red(), c.green(), c.blue()); m_mat->specular = rgb; } break;
			case 4: { QColor c = v.value<QColor>(); GLCOLOR rgb(c.red(), c.green(), c.blue()); m_mat->emission = rgb; } break;
			case 5: { QColor c = v.value<QColor>(); GLCOLOR rgb(c.red(), c.green(), c.blue()); m_mat->meshcol = rgb; } break;
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
	CPropertyListView*	m_prop;

public:
	void setupUi(::CMaterialPanel* parent)
	{
		QVBoxLayout* pg = new QVBoxLayout(parent);

		QSplitter* psplitter = new QSplitter;
		psplitter->setOrientation(Qt::Vertical);
		pg->addWidget(psplitter);

		m_list = new QListWidget;
		m_list->setObjectName(QStringLiteral("materialList"));

		m_prop = new CPropertyListView;

		psplitter->addWidget(m_list);
		psplitter->addWidget(m_prop);

		QMetaObject::connectSlotsByName(parent);

	}
};

CMaterialPanel::CMaterialPanel(CMainWindow* pwnd, QWidget* parent) : CCommandPanel(pwnd, parent), ui(new Ui::CMaterialPanel)
{
	ui->setupUi(this);
	m_pmat = new MaterialProps;
}

void CMaterialPanel::Update()
{
	ui->m_list->clear();
	ui->m_prop->Clear();
	m_pmat->SetMaterial(0);

	CDocument* pdoc = m_wnd->GetDocument();
	FEModel& fem = *pdoc->GetFEModel();

	int nmat = fem.Materials();
	for (int i=0; i<nmat; ++i)
	{
		ui->m_list->addItem(fem.GetMaterial(i)->GetName());
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
	}
}
