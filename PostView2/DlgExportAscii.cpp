// DlgExportAscii.cpp: implementation of the CDlgExportAscii class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DlgExportAscii.h"
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QBoxLayout>
#include <QFormLayout>
#include <string>
using namespace std;

class Ui::CDlgExportAscii
{
public:
	QCheckBox* coords;
	QCheckBox* face;
	QCheckBox* normals;
	QCheckBox* elem;
	QCheckBox* nodeData;
	QCheckBox* elemData;
	QCheckBox* selOnly;
	QComboBox* step;
	QLineEdit* fmt;

public:
	void setupUi(QWidget* wnd)
	{
		QFormLayout* form = new QFormLayout;
		form->addRow("Nodal coordinates"   , coords   = new QCheckBox);
		form->addRow("Facet connectivity"  , face     = new QCheckBox);
		form->addRow("Facet normals"       , normals  = new QCheckBox);
		form->addRow("Element connectivity", elem     = new QCheckBox);
		form->addRow("Nodal values"        , nodeData = new QCheckBox);
		form->addRow("Element values"      , elemData = new QCheckBox);
		form->addRow("Selection only"      , selOnly  = new QCheckBox);

		form->addRow("Time steps", step = new QComboBox);
		step->addItem("Current time step");
		step->addItem("All time steps");

		form->addRow("Format", fmt = new QLineEdit);

		QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

		QVBoxLayout* l = new QVBoxLayout;
		l->addLayout(form);
		l->addWidget(bb);

		wnd->setLayout(l);

		QObject::connect(bb, SIGNAL(accepted()), wnd, SLOT(accept()));
		QObject::connect(bb, SIGNAL(rejected()), wnd, SLOT(reject()));
	}
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDlgExportAscii::CDlgExportAscii(QWidget* parent) : QDialog(parent), ui(new Ui::CDlgExportAscii)
{
	ui->setupUi(this);
}

void CDlgExportAscii::accept()
{
	m_bcoords = ui->coords->isChecked();
	m_belem   = ui->elem->isChecked();
	m_bndata  = ui->nodeData->isChecked();
	m_bedata  = ui->elemData->isChecked();
	m_bface   = ui->face->isChecked();
	m_bsel    = ui->selOnly->isChecked();
	m_bfnormals= ui ->normals->isChecked();

	m_nstep = ui->step->currentIndex();

	string s = ui->fmt->text().toStdString();
	strcpy(m_szfmt, s.c_str());

	QDialog::accept();
}
