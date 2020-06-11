/*This file is part of the PostView source code and is licensed under the MIT license
listed below.

See Copyright-PostView.txt for details.

Copyright (c) 2020 University of Utah, The Trustees of Columbia University in 
the City of New York, and others.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#include "stdafx.h"
#include "DlgImportRAW.h"
#include <QGridLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QMessageBox>
#include "CIntInput.h"

class Ui::CDlgImportRAW
{
public:
	CIntInput *pnx, *pny, *pnz;
	CFloatInput *pxmin, *pymin, *pzmin;
	CFloatInput *pxmax, *pymax, *pzmax;

public:
	void setup(QDialog* w)
	{
		QVBoxLayout* pv = new QVBoxLayout;

		QLabel* pl;
		QGridLayout* grid = new QGridLayout;
		grid->addWidget(pl = new QLabel("X-pixels"), 0, 0); grid->addWidget(pnx = new CIntInput, 0, 1); pl->setBuddy(pnx); pnx->setFixedWidth(70);
		grid->addWidget(pl = new QLabel("Y-pixels"), 1, 0); grid->addWidget(pny = new CIntInput, 1, 1); pl->setBuddy(pny); pny->setFixedWidth(70);
		grid->addWidget(pl = new QLabel("Z-pixels"), 2, 0); grid->addWidget(pnz = new CIntInput, 2, 1); pl->setBuddy(pnz); pnz->setFixedWidth(70);

		grid->addWidget(pl = new QLabel("x-min"), 3, 0); grid->addWidget(pxmin = new CFloatInput, 3, 1); pl->setBuddy(pxmin); pxmin->setFixedWidth(70);
		grid->addWidget(pl = new QLabel("y-min"), 4, 0); grid->addWidget(pymin = new CFloatInput, 4, 1); pl->setBuddy(pymin); pymin->setFixedWidth(70);
		grid->addWidget(pl = new QLabel("z-min"), 5, 0); grid->addWidget(pzmin = new CFloatInput, 5, 1); pl->setBuddy(pzmin); pzmin->setFixedWidth(70);

		grid->addWidget(pl = new QLabel("x-max"), 3, 2); grid->addWidget(pxmax = new CFloatInput, 3, 3); pl->setBuddy(pxmax); pxmax->setFixedWidth(70);
		grid->addWidget(pl = new QLabel("y-max"), 4, 2); grid->addWidget(pymax = new CFloatInput, 4, 3); pl->setBuddy(pymax); pymax->setFixedWidth(70);
		grid->addWidget(pl = new QLabel("z-max"), 5, 2); grid->addWidget(pzmax = new CFloatInput, 5, 3); pl->setBuddy(pzmax); pzmax->setFixedWidth(70);

		pxmin->setValue(0.0);
		pymin->setValue(0.0);
		pzmin->setValue(0.0);

		pxmax->setValue(1.0);
		pymax->setValue(1.0);
		pzmax->setValue(1.0);

		pv->addLayout(grid);

		QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
		pv->addWidget(bb);

		w->setLayout(pv);

		QObject::connect(bb, SIGNAL(accepted()), w, SLOT(accept()));
		QObject::connect(bb, SIGNAL(rejected()), w, SLOT(reject()));
	}
};

CDlgImportRAW::CDlgImportRAW(QWidget* parent) : QDialog(parent), ui(new Ui::CDlgImportRAW)
{
	ui->setup(this);

	m_nx = 0;
	m_ny = 0;
	m_nz = 0;

	m_min[0] = m_min[1] = m_min[2] = 0.0;
	m_max[0] = m_max[1] = m_max[2] = 1.0;
}

void CDlgImportRAW::accept()
{
	m_nx = ui->pnx->value();
	m_ny = ui->pny->value();
	m_nz = ui->pnz->value();

	m_min[0] = ui->pxmin->value();
	m_min[1] = ui->pymin->value();
	m_min[2] = ui->pzmin->value();

	m_max[0] = ui->pxmax->value();
	m_max[1] = ui->pymax->value();
	m_max[2] = ui->pzmax->value();

	if (m_nx <= 0) { QMessageBox::critical(this, "PostView", "Invalid value for X-pixels"); return;	}
	if (m_ny <= 0) { QMessageBox::critical(this, "PostView", "Invalid value for Y-pixels"); return; }
	if (m_nz <= 0) { QMessageBox::critical(this, "PostView", "Invalid value for Z-pixels"); return; }

	if (m_max[0] <= m_min[0]) { QMessageBox::critical(this, "PostView", "Invalid x range."); return; }
	if (m_max[1] <= m_min[1]) { QMessageBox::critical(this, "PostView", "Invalid y range."); return; }
	if (m_max[2] <= m_min[2]) { QMessageBox::critical(this, "PostView", "Invalid z range."); return; }

	QDialog::accept();
}
