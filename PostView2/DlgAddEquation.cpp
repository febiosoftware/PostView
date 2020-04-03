#include "stdafx.h"
#include "DlgAddEquation.h"
#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QStackedWidget>
#include <QGridLayout>

class CDlgAddEquation_UI
{
public:
	QLineEdit*	name;
	QLineEdit*	scl;
	QLineEdit*	vec[3];
	QLineEdit*	mat[9];
	QComboBox*	type;
	QLabel*	lbl[3];

public:
	void setup(QDialog* dlg)
	{
		QFormLayout* f = new QFormLayout;

		f->addRow("Name:", name = new QLineEdit);
		f->addRow("Type:", type = new QComboBox);
		type->addItems(QStringList() << "FLOAT" << "VEC3F" << "MAT3F");

		// scalar equation input
		QHBoxLayout* hs = new QHBoxLayout;
		QLabel* ls = new QLabel("Equation:");
		scl = new QLineEdit;
		ls->setBuddy(scl);
		hs->addWidget(ls);
		hs->addWidget(scl);
		QWidget* sw = new QWidget;
		sw->setLayout(hs);

		// vector equation input
		QFormLayout* hv = new QFormLayout;
		hv->addRow("X:", vec[0] = new QLineEdit);
		hv->addRow("Y:", vec[1] = new QLineEdit);
		hv->addRow("Z:", vec[2] = new QLineEdit);
		hv->setLabelAlignment(Qt::AlignRight);
		QWidget* vw = new QWidget;
		vw->setLayout(hv);

		// math equation input
		QGridLayout* grid = new QGridLayout;
		grid->addWidget(ls = new QLabel("X"), 0, 1, 1, 1); ls->setAlignment(Qt::AlignHCenter);
		grid->addWidget(ls = new QLabel("Y"), 0, 2, 1, 1); ls->setAlignment(Qt::AlignHCenter);
		grid->addWidget(ls = new QLabel("Z"), 0, 3, 1, 1); ls->setAlignment(Qt::AlignHCenter);

		grid->addWidget(new QLabel("X"), 1, 0, 1, 1);
		grid->addWidget(mat[0] = new QLineEdit, 1, 1, 1, 1);
		grid->addWidget(mat[1] = new QLineEdit, 1, 2, 1, 1);
		grid->addWidget(mat[2] = new QLineEdit, 1, 3, 1, 1);

		grid->addWidget(new QLabel("Y"), 2, 0, 1, 1);
		grid->addWidget(mat[3] = new QLineEdit, 2, 1, 1, 1);
		grid->addWidget(mat[4] = new QLineEdit, 2, 2, 1, 1);
		grid->addWidget(mat[5] = new QLineEdit, 2, 3, 1, 1);

		grid->addWidget(new QLabel("Z"), 3, 0, 1, 1);
		grid->addWidget(mat[6] = new QLineEdit, 3, 1, 1, 1);
		grid->addWidget(mat[7] = new QLineEdit, 3, 2, 1, 1);
		grid->addWidget(mat[8] = new QLineEdit, 3, 3, 1, 1);
		QWidget* mw = new QWidget;
		mw->setLayout(grid);

		QStackedWidget* stack = new QStackedWidget;
		stack->addWidget(sw);
		stack->addWidget(vw);
		stack->addWidget(mw);

		QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

		QVBoxLayout* l = new QVBoxLayout;
		l->addLayout(f);
		l->addWidget(stack);
		l->addWidget(bb);

		dlg->setLayout(l);

		QObject::connect(bb, SIGNAL(accepted()), dlg, SLOT(accept()));
		QObject::connect(bb, SIGNAL(rejected()), dlg, SLOT(reject()));
		QObject::connect(type, SIGNAL(currentIndexChanged(int)), stack, SLOT(setCurrentIndex(int)));
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

int CDlgAddEquation::GetDataType()
{
	return ui->type->currentIndex();
}

QString CDlgAddEquation::GetScalarEquation()
{
	return ui->scl->text();
}

QStringList CDlgAddEquation::GetVectorEquations()
{
	QStringList s;
	s << ui->vec[0]->text();
	s << ui->vec[1]->text();
	s << ui->vec[2]->text();
	return s;
}

QStringList CDlgAddEquation::GetMatrixEquations()
{
	QStringList s;
	for (int i = 0; i < 9; ++i) s << ui->mat[i]->text();
	return s;
}
