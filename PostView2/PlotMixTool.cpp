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
#include "PlotMixTool.h"
#include <QPushButton>
#include <QListWidget>
#include <QBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>
#include <PostLib/FEPlotMix.h>
#include "Document.h"
#include "CIntInput.h"
#include <PostLib/FEKinemat.h>
#include <PostLib/FELSDYNAimport.h>
#include "MainWindow.h"
using namespace Post;

class CPlotMixToolUI : public QWidget
{
public:
	QListWidget* list;
public:
	CPlotMixToolUI(QObject* parent)
	{
		QVBoxLayout* pv = new QVBoxLayout;
		QHBoxLayout* ph = new QHBoxLayout;
		
		QPushButton* browse   = new QPushButton("Add file ...");
		QPushButton* remove   = new QPushButton("Remove");
		QPushButton* moveUp   = new QPushButton("Move Up");
		QPushButton* moveDown = new QPushButton("Move Down");
		ph->addWidget(browse);
		ph->addWidget(remove);
		ph->addWidget(moveUp);
		ph->addWidget(moveDown);
		pv->addLayout(ph);

		list = new QListWidget;
		pv->addWidget(list);

		QPushButton* apply = new QPushButton("Load");
		pv->addWidget(apply);
		pv->addStretch();

		setLayout(pv);

		QObject::connect(browse  , SIGNAL(clicked()), parent, SLOT(OnBrowse()));
		QObject::connect(remove  , SIGNAL(clicked()), parent, SLOT(OnRemove()));
		QObject::connect(moveUp  , SIGNAL(clicked()), parent, SLOT(OnMoveUp()));
		QObject::connect(moveDown, SIGNAL(clicked()), parent, SLOT(OnMoveDown()));
		QObject::connect(apply   , SIGNAL(clicked()), parent, SLOT(OnApply()));
	}
};

CPlotMixTool::CPlotMixTool(CMainWindow* wnd) : CAbstractTool("Plot Mix", wnd)
{
	ui = 0;
}

// get the property list
QWidget* CPlotMixTool::createUi()
{
	ui = new CPlotMixToolUI(this);
	return ui;
}

void CPlotMixTool::OnBrowse()
{
	QStringList filenames = QFileDialog::getOpenFileNames(0, "Open file", 0, "XPLT files(*.xplt)");
	if (filenames.isEmpty() == false)
	{
		for (int i=0; i<filenames.count(); ++i)
			ui->list->addItem(filenames[i]);
	}
}

void CPlotMixTool::OnRemove()
{
	qDeleteAll(ui->list->selectedItems());
}

void CPlotMixTool::OnMoveUp()
{
	QList<QListWidgetItem*> items = ui->list->selectedItems();
	QList<QListWidgetItem*>::iterator it;
	for (it = items.begin(); it != items.end(); ++it)
	{
		QListWidgetItem* pi = ui->list->takeItem(ui->list->row(*it));
		ui->list->insertItem(0, pi);
	}
}

void CPlotMixTool::OnMoveDown()
{
	QList<QListWidgetItem*> items = ui->list->selectedItems();
	QList<QListWidgetItem*>::iterator it;
	for (it = items.begin(); it != items.end(); ++it)
	{
		QListWidgetItem* pi = ui->list->takeItem(ui->list->row(*it));
		ui->list->addItem(pi);
	}
}

void CPlotMixTool::OnApply()
{
	FEPlotMix reader;

	int nitems = ui->list->count();
	vector<string> str(nitems);
	for (int i=0; i<nitems; ++i)
	{
		QListWidgetItem* pi = ui->list->item(i);
		QString s = pi->text();
		str[i] = s.toStdString();
	}

	vector<const char*> sz(nitems, 0);
	for (int i=0; i<nitems; ++i) sz[i] = str[i].c_str();

	// Create a new document
	CDocument* doc = m_wnd->NewDocument("plotmix");

	FEPostModel* pnew = reader.Load(&sz[0], nitems);
	if (pnew == 0) QMessageBox::critical(0, "Plot Mix Tool", "An error occured reading the plot files.");
	else
	{
		doc->SetFEModel(pnew);
	}
	ui->list->clear();
	updateUi();
}

//=======================================================================================

class CKinematToolUI : public QWidget
{
public:
	QLineEdit* modelFile;
	QLineEdit* kineFile;
	CIntInput *start, *end, *stride;

public:
	CKinematToolUI(QObject* parent)
	{
		QVBoxLayout* pv = new QVBoxLayout;
		QLabel* pl = new QLabel("Model file:");
		modelFile = new QLineEdit; pl->setBuddy(modelFile);
		QPushButton* browse1 = new QPushButton("..."); browse1->setFixedWidth(30);
		QHBoxLayout* ph = new QHBoxLayout;
		ph->addWidget(modelFile); 
		ph->addWidget(browse1);
		pv->addWidget(pl);
		pv->addLayout(ph);

		pl = new QLabel("Kine file:");
		kineFile = new QLineEdit; pl->setBuddy(kineFile);
		QPushButton* browse2 = new QPushButton("..."); browse2->setFixedWidth(30);
		ph = new QHBoxLayout;
		ph->addWidget(kineFile);
		ph->addWidget(browse2);
		pv->addWidget(pl);
		pv->addLayout(ph);

		ph = new QHBoxLayout;
		ph->addWidget(pl = new QLabel("From:"));
		ph->addWidget(start = new CIntInput); pl->setBuddy(start); start->setFixedWidth(70); start->setValue(1);
		ph->addWidget(pl = new QLabel("To:"));
		ph->addWidget(end = new CIntInput); pl->setBuddy(end); end->setFixedWidth(70); end->setValue(999);
		ph->addWidget(pl = new QLabel("Stride:"));
		ph->addWidget(stride = new CIntInput); pl->setBuddy(stride); stride->setFixedWidth(70); stride->setValue(1);
		pv->addLayout(ph);

		QPushButton* apply = new QPushButton("Apply");
		pv->addWidget(apply);

		pv->addStretch();
		setLayout(pv);

		QObject::connect(browse1, SIGNAL(clicked()), parent, SLOT(OnBrowse1()));
		QObject::connect(browse2, SIGNAL(clicked()), parent, SLOT(OnBrowse2()));
		QObject::connect(apply  , SIGNAL(clicked()), parent, SLOT(OnApply()));
	}
};

CKinematTool::CKinematTool(CMainWindow* wnd) : CAbstractTool("Kinemat", wnd)
{
	ui = 0;
}

// get the property list
QWidget* CKinematTool::createUi()
{
	ui = new CKinematToolUI(this);
	return ui;
}

void CKinematTool::OnBrowse1()
{
	QString filename = QFileDialog::getOpenFileName(0, "Open file", 0, "LSDYNA Keyword (*.k)");
	if (filename.isEmpty() == false)
	{
		ui->modelFile->setText(filename);
	}
}

void CKinematTool::OnBrowse2()
{
	QString filename = QFileDialog::getOpenFileName(0, "Open file", 0, "All files(*)");
	if (filename.isEmpty() == false)
	{
		ui->kineFile->setText(filename);
	}
}

void CKinematTool::OnApply()
{
	int n0 = ui->start->value();
	int n1 = ui->end->value();
	int ni = ui->stride->value();

	// create a new document
	CDocument* doc = m_wnd->NewDocument("Kinemat");

	FEKinemat kine;
	kine.SetRange(n0, n1, ni);

	string modelFile = ui->modelFile->text().toStdString();
	string kineFile = ui->kineFile->text().toStdString();

	// load the file
	FELSDYNAimport* preader = new FELSDYNAimport(nullptr);
	preader->read_displacements(true);
	if (doc->LoadFEModel(preader, modelFile.c_str()) == false)
	{
		QMessageBox::critical(m_wnd, "PostView2", "Failed to load model file");
		return;
	}

	// apply the kinemat tool to the model
	if (kine.Apply(doc->GetGLModel(), kineFile.c_str()) == false)
	{
		QMessageBox::critical(m_wnd, "Kinemat", "Failed applying Kinemat tool");
	}

	doc->ZoomExtents();
	m_wnd->SetCurrentTime(0);
	m_wnd->UpdateMainToolbar();
	m_wnd->UpdateUi(true);
}
