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

#include <QFileSystemModel>
#include <QListView>
#include <QBoxLayout>
#include <QComboBox>
#include "FileViewer.h"
#include "MainWindow.h"
#include <QToolButton>

class Ui::CFileViewer
{
public:
	QComboBox*	m_fileFilter;
	QListView*	m_fileList;
	QComboBox*	m_folder;

public:
	void setupUi(QWidget* parent)
	{
		QVBoxLayout* pg = new QVBoxLayout(parent);
		QHBoxLayout* pg2 = new QHBoxLayout;

		QToolButton* pb = new QToolButton(parent);
		pb->setObjectName(QStringLiteral("toolUp"));
		pb->setIcon(QIcon(":/icons/up.png"));
		pb->setAutoRaise(true);

		m_folder = new QComboBox;
		m_folder->setObjectName("folder");
		m_folder->setEditable(true);
		m_folder->setInsertPolicy(QComboBox::NoInsert);
		m_folder->setMaxCount(15);
		m_folder->setMaximumWidth(350);

		m_fileFilter = new QComboBox(parent);
		m_fileFilter->setObjectName(QStringLiteral("fileFilter"));

		m_fileList = new QListView(parent);
		m_fileList->setObjectName(QStringLiteral("fileList"));

		pg2->addWidget(pb);
		pg2->addWidget(m_folder);
		pg->addLayout(pg2);
		pg->addWidget(m_fileFilter);
		pg->addWidget(m_fileList);
	}

	void addFolder(const QString& folder)
	{
		// see if this item already exists
		int n = m_folder->findText(folder);
		if (n != -1) return;

		// if not, add it
		n = m_folder->count();
		if (n == m_folder->maxCount())
		{
			m_folder->removeItem(0);
		}
		m_folder->addItem(folder);
		m_folder->setCurrentIndex(m_folder->count()-1);
	}
};

CFileViewer::CFileViewer(CMainWindow* pwnd, QWidget* parent) : QWidget(parent), m_wnd(pwnd), ui(new Ui::CFileViewer)
{
	// build Ui
	ui->setupUi(this);

	// build the filter list
	// Make sure this list matches the one in CMainWindow::on_actionOpen_triggered()
	// TODO: Can I somehow ensure that this is the case ?
	m_filters.push_back(pair<QString, QString>("XPLT files (*.xplt)"   , "*.xplt"));
	m_filters.push_back(pair<QString, QString>("FEBio files (*.feb)"   , "*.feb" ));
	m_filters.push_back(pair<QString, QString>("LSDYNA database (*)"   , "*"     ));
	m_filters.push_back(pair<QString, QString>("LSDYNA keyword (*.k)"  , "*.k"   ));
	m_filters.push_back(pair<QString, QString>("GMesh (*.msh)"         , "*.msh" ));
	m_filters.push_back(pair<QString, QString>("NIKE3D (*.n)"          , "*.n"   ));
	m_filters.push_back(pair<QString, QString>("ASCII data (*.txt)"    , "*.txt" ));
	m_filters.push_back(pair<QString, QString>("STL ASCII (*.stl)"     , "*.stl" ));
	m_filters.push_back(pair<QString, QString>("RAW image data (*.raw)", "*.raw" ));
	m_filters.push_back(pair<QString, QString>("VTK files (*.vtk)"     , "*.vtk" ));
	m_filters.push_back(pair<QString, QString>("U3D files (*.u3d)"     , "*.u3d" ));

	// add filters to drop down
	int nflts = (int) m_filters.size();
	for (int i=0; i<nflts; ++i)
	{
		pair<QString, QString>& flt = m_filters[i];
		ui->m_fileFilter->addItem(flt.first);
	}

	// create a model for the file system
    m_fileSystem = new QFileSystemModel;
    m_fileSystem->setRootPath("C:\\");
    QStringList flt;
	flt << m_filters[0].second;
    m_fileSystem->setNameFilters(flt);
    m_fileSystem->setNameFilterDisables(false);
    
	// set the file system model
	ui->m_fileList->setModel(m_fileSystem);
    ui->m_fileList->setRootIndex(m_fileSystem->index(QDir::homePath()));

	// set the folder
	ui->m_folder->setEditText(currentPath());

	QMetaObject::connectSlotsByName(this);
}

QString CFileViewer::currentPath() const
{
	return m_fileSystem->filePath(ui->m_fileList->rootIndex());
}

void CFileViewer::setCurrentPath(const QString& s)
{
	ui->m_fileList->setRootIndex(m_fileSystem->index(s));

	int n = ui->m_folder->findText(s);
	if (n >= 0) ui->m_folder->setCurrentIndex(n);
	else ui->m_folder->setEditText(currentPath());
}

void CFileViewer::on_fileList_doubleClicked(const QModelIndex& index)
{
	if (m_fileSystem->isDir(index))
	{
		ui->m_fileList->setRootIndex(index);
		ui->m_folder->setEditText(currentPath());
	}
	else
	{
		m_wnd->OpenFile(m_fileSystem->filePath(index), ui->m_fileFilter->currentIndex());

		QString filePath = currentPath();
		m_wnd->SetCurrentFolder(filePath);

		ui->addFolder(filePath);
	}
}

void CFileViewer::on_fileFilter_currentIndexChanged(int index)
{
	if ((index >= 0)&&(index < m_filters.size()))
	{
		QStringList filters;
		pair<QString, QString>& flt = m_filters[index];
		filters << flt.second;
		m_fileSystem->setNameFilters(filters);
	}
}

void CFileViewer::on_toolUp_clicked()
{
    QModelIndex n = ui->m_fileList->rootIndex();
    n = m_fileSystem->parent(n);
    ui->m_fileList->setRootIndex(n);
	ui->m_folder->setEditText(currentPath());
}

void CFileViewer::on_folder_editTextChanged(const QString& text)
{
	setCurrentPath(text);
}

QStringList CFileViewer::FolderList()
{
	QStringList folders;
	for (int i=0; i<ui->m_folder->count(); ++i)
	{
		folders << ui->m_folder->itemText(i);
	}
	return folders;
}

void CFileViewer::SetFolderList(const QStringList& folders)
{
	ui->m_folder->clear();
	ui->m_folder->addItems(folders);
	ui->m_folder->setCurrentIndex(0);
}
