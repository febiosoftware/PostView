#include "stdafx.h"
#include "DlgExportVTK.h"
#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QRadioButton>

class CDlgExportVTK_UI
{
public:
	QRadioButton*	allStates;
	QRadioButton*	currState;

public:
	void setup(QDialog* dlg)
	{
		allStates = new QRadioButton("Export all states");
		currState = new QRadioButton("Export current state only");

		QVBoxLayout* l = new QVBoxLayout;
		l->addWidget(allStates);
		l->addWidget(currState);

		allStates->setChecked(true);

		QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
		l->addWidget(bb);

		dlg->setLayout(l);

		QObject::connect(bb, SIGNAL(accepted()), dlg, SLOT(accept()));
		QObject::connect(bb, SIGNAL(rejected()), dlg, SLOT(reject()));
	}
};

CDlgExportVTK::CDlgExportVTK(QWidget* parent) : QDialog(parent), ui(new CDlgExportVTK_UI)
{
	m_ops[0] = true;
	m_ops[1] = false;

	ui->setup(this);
}

void CDlgExportVTK::accept()
{
	m_ops[0] = ui->allStates->isChecked();
	m_ops[1] = ui->currState->isChecked();

	QDialog::accept();
}
