#include "stdafx.h"
#include "DlgFileInfo.h"
#include <QFormLayout>
#include <QLineEdit>
#include <QDialogButtonBox>

class Ui::CDlgFileInfo
{
public:
	QLineEdit*	software;
public:
	void setupUi(QWidget* parent)
	{
		QFormLayout* form = new QFormLayout;
		form->addRow("Software", software = new QLineEdit); software->setReadOnly(true);

		QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Ok);

		QVBoxLayout* layout = new QVBoxLayout;
		layout->addLayout(form);
		layout->addWidget(bb);

		parent->setLayout(layout);
		QObject::connect(bb, SIGNAL(accepted()), parent, SLOT(accept()));
	}
};

CDlgFileInfo::CDlgFileInfo(QWidget* parent) : QDialog(parent), ui(new Ui::CDlgFileInfo)
{
	ui->setupUi(this);
}

void CDlgFileInfo::setSoftware(const QString& n)
{
	ui->software->setText(n);
}
