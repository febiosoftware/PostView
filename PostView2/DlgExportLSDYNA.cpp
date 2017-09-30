#include "stdafx.h"
#include "DlgExportLSDYNA.h"
#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QCheckBox>

class CDlgExportLSDYNA_UI
{
public:
	QCheckBox*	selOnly;
	QCheckBox*	expSurf;
	QCheckBox*	expResu;

public:
	void setup(QDialog* dlg)
	{
		selOnly = new QCheckBox("Selection only");
		expSurf = new QCheckBox("Export surface");
		expResu = new QCheckBox("Export nodal results");

		QVBoxLayout* l = new QVBoxLayout;
		l->addWidget(selOnly);
		l->addWidget(expSurf);
		l->addWidget(expResu);

		QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
		l->addWidget(bb);

		dlg->setLayout(l);

		QObject::connect(bb, SIGNAL(accepted()), dlg, SLOT(accept()));
		QObject::connect(bb, SIGNAL(rejected()), dlg, SLOT(reject()));
	}
};

CDlgExportLSDYNA::CDlgExportLSDYNA(QWidget* parent) : QDialog(parent), ui(new CDlgExportLSDYNA_UI)
{
	m_bsel = false;
	m_bsurf = false;
	m_bnode = false;

	ui->setup(this);
}

void CDlgExportLSDYNA::accept()
{
	m_bsel = ui->selOnly->isChecked();
	m_bsurf = ui->expSurf->isChecked();
	m_bnode = ui->expResu->isChecked();

	QDialog::accept();
}
