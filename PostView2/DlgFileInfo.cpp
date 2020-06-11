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
