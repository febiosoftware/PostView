#include "stdafx.h"
#include "DlgImportXPLT.h"
#include <QRadioButton>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QBoxLayout>
#include <QLabel>

extern bool string_to_int_list(char* sz, std::vector<int>& list);

class Ui::CDlgImportXPLT
{
public:
	QRadioButton* pb1;
	QRadioButton* pb2;
	QRadioButton* pb3;
	QLineEdit* pitems;

public:
	void setupUi(QDialog* parent)
	{
		QVBoxLayout* pv = new QVBoxLayout;

		QVBoxLayout* pg = new QVBoxLayout;
		pg->addWidget(pb1 = new QRadioButton("Read all states"));
		pg->addWidget(pb2 = new QRadioButton("Read last state only"));
		pg->addWidget(pb3 = new QRadioButton("Read states from list:"));
		pv->addLayout(pg);

		pb1->setChecked(true);

		pv->addWidget(pitems = new QLineEdit);
		pv->addWidget(new QLabel("(e.g.:1,2,3:6,10:100:5)"));

		QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

		pv->addWidget(bb);
		parent->setLayout(pv);

		QObject::connect(bb, SIGNAL(accepted()), parent, SLOT(accept()));
		QObject::connect(bb, SIGNAL(rejected()), parent, SLOT(reject()));
	}
};

CDlgImportXPLT::CDlgImportXPLT(QWidget* parent) : QDialog(parent), ui(new Ui::CDlgImportXPLT)
{
	ui->setupUi(this);
	setWindowTitle("Import XPLT");
}

void CDlgImportXPLT::accept()
{
	if (ui->pb1->isChecked()) m_nop = 0;
	if (ui->pb2->isChecked()) m_nop = 1;
	if (ui->pb3->isChecked()) m_nop = 2;

	std::string s = ui->pitems->text().toStdString();
	char buf[256] = {0}; 
	strcpy(buf, s.c_str());
	string_to_int_list(buf, m_item);

	QDialog::accept();
}
