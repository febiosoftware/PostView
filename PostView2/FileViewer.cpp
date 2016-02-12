#include <QFileSystemModel>
#include <QMessageBox>
#include "FileViewer.h"
#include "MainWindow.h"

CFileViewer::CFileViewer(CMainWindow* pwnd, QWidget* parent) : QListView(parent), m_wnd(pwnd)
{
	this->setObjectName(QStringLiteral("fileViewer"));

	// create a model for the file system
    m_fileSystem = new QFileSystemModel;
    m_fileSystem->setRootPath("C:\\");
    QStringList flt;
	flt << "*.xplt";
    m_fileSystem->setNameFilters(flt);
    m_fileSystem->setNameFilterDisables(false);
    
	// set the file system model
	setModel(m_fileSystem);
    setRootIndex(m_fileSystem->index("C:\\Users\\steve\\Documents"));

	QMetaObject::connectSlotsByName(this);
}

void CFileViewer::on_fileViewer_doubleClicked(const QModelIndex& index)
{
	if (m_fileSystem->isDir(index))
		setRootIndex(index);
	else
	{
		m_wnd->OpenFile(m_fileSystem->filePath(index));
	}
}
