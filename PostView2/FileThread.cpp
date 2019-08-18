#include "stdafx.h"
#include "FileThread.h"
#include "MainWindow.h"
#include "Document.h"
#include <PostLib/FEFileReader.h>
using namespace Post;

CFileThread::CFileThread(CMainWindow* wnd, CDocument* doc, FEFileReader* file, const QString& fileName) : m_wnd(wnd), m_doc(doc), m_fileReader(file), m_fileName(fileName)
{
	QObject::connect(this, SIGNAL(resultReady(bool, const QString&)), wnd, SLOT(finishedReadingFile(bool, const QString&)));
	QObject::connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

void CFileThread::run()
{
	if (m_fileReader)
	{
		std::string sfile = m_fileName.toStdString();
		CDocument& doc = *m_doc;
		bool ret = doc.LoadFEModel(m_fileReader, sfile.c_str());
		std::string err = m_fileReader->GetErrorMessage();
		emit resultReady(ret, QString(err.c_str()));
	}
	else emit resultReady(false, "No file reader");
}

float CFileThread::getFileProgress() const
{
	if (m_fileReader) return m_fileReader->GetFileProgress();
	return 0.f;
}
