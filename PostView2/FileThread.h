#pragma once
#include <QtCore/QThread>

class CMainWindow;
class FEFileReader;
class CDocument;

class CFileThread : public QThread
{
	Q_OBJECT

		void run() Q_DECL_OVERRIDE;

public:
	CFileThread(CMainWindow* wnd, CDocument* doc, FEFileReader* file, const QString& fileName);

	float getFileProgress() const;

	FEFileReader* GetFileReader() { return m_fileReader; }

	CDocument* GetDocument() { return m_doc; }

signals:
	void resultReady(bool, const QString&);

private:
	CMainWindow*	m_wnd;
	CDocument*		m_doc;
	FEFileReader*	m_fileReader;
	QString			m_fileName;
};
