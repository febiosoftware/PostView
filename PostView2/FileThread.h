#pragma once
#include <QtCore/QThread>
#include <PostLib/FEFileReader.h>

class CMainWindow;
class CDocument;

class CFileThread : public QThread
{
	Q_OBJECT

		void run() Q_DECL_OVERRIDE;

public:
	CFileThread(CMainWindow* wnd, CDocument* doc, Post::FEFileReader* file, const QString& fileName);

	float getFileProgress() const;

	Post::FEFileReader* GetFileReader() { return m_fileReader; }

	CDocument* GetDocument() { return m_doc; }

signals:
	void resultReady(bool, const QString&);

private:
	CMainWindow*	m_wnd;
	CDocument*		m_doc;
	Post::FEFileReader*	m_fileReader;
	QString			m_fileName;
};
