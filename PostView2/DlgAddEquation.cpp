#include "stdafx.h"
#include "DlgAddEquation.h"
#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>

class CDlgAddEquation_UI
{
public:
	QLineEdit*	name;
	QLineEdit*	eq;

public:
	void setup(QDialog* dlg)
	{
		QFormLayout* f = new QFormLayout;

		f->addRow("Name:", name = new QLineEdit);
		f->addRow("Equation:", eq = new QLineEdit);
		
		QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

		QVBoxLayout* l = new QVBoxLayout;
		l->addLayout(f);
		l->addWidget(bb);

		dlg->setLayout(l);

		QObject::connect(bb, SIGNAL(accepted()), dlg, SLOT(accept()));
		QObject::connect(bb, SIGNAL(rejected()), dlg, SLOT(reject()));
	}
};

CDlgAddEquation::CDlgAddEquation(QWidget* parent) : QDialog(parent), ui(new CDlgAddEquation_UI)
{
	ui->setup(this);
}

QString CDlgAddEquation::GetDataName()
{
	return ui->name->text();
}

QString CDlgAddEquation::GetEquation()
{
	return ui->eq->text();
}
