#pragma once
#include <QtCore/QThread>

class CMainWindow;
class FEFileReader;

class CFileThread : public QThread
{
	Q_OBJECT

		void run() Q_DECL_OVERRIDE;

public:
	CFileThread(CMainWindow* wnd, FEFileReader* file, const QString& fileName);

	float getFileProgress() const;

	FEFileReader* GetFileReader() { return m_fileReader; }

signals:
	void resultReady(bool, const QString&);

private:
	CMainWindow*	m_wnd;
	FEFileReader*	m_fileReader;
	QString			m_fileName;
};
